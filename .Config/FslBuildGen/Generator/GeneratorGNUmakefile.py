#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
#      its contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************************************************************************************

from typing import Dict
from typing import List
from typing import Optional
from typing import Set
from FslBuildGen import IOUtil
from FslBuildGen import MakeFileHelper
from FslBuildGen import Util
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import InternalErrorException
from FslBuildGen.Generator import GitIgnoreHelper
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorCommandReport import GeneratorCommandReport
from FslBuildGen.Generator.Report.GeneratorExecutableReport import GeneratorExecutableReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.Report.ReportVariableFormatter import ReportVariableFormatter
from FslBuildGen.LibUtil import LibUtil
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageExternalDependency
from FslBuildGen.Packages.Package import PackagePlatformVariant
from FslBuildGen.Packages.Package import PackagePlatformVariantOption
from FslBuildGen.Packages.PackagePlatformExternalDependency import PackagePlatformExternalDependency
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import ToolAddedVariantConfigOption

class LocalMagicBuildVariants:
    GeneratorExeFileExtension = "FSLGEN_GENERATOR_ExeFileExtension"


class GeneratorGNUmakefile(GeneratorBase):
    def __init__(self, config: Config, packages: List[Package], dstMakeFilename: str,
                 templateExe: str, templateLib: str, generatorName: str) -> None:
        super(GeneratorGNUmakefile, self).__init__()
        self.BldTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "build.sh"))
        self.ExeTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, templateExe))
        self.LibTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, templateLib))
        for package in packages:
            if not package.ResolvedPlatformNotSupported:
                if package.Type == PackageType.Library:
                    self.__GenerateLibraryBuildFile(config, generatorName, package, dstMakeFilename)
                elif package.Type == PackageType.Executable:
                    self.__GenerateExecutableBuildFile(config, generatorName, package, dstMakeFilename)
            #elif package.Type == PackageType.Executable:
            #    config.DoPrint("WARNING: Package {0} marked as not supported".format(package.Name))


    def __GenerateLibraryBuildFile(self, config: Config, generatorName: str, package: Package, dstMakeFilename: str) -> None:
        self.__GenerateBuildFile(config, generatorName, package, self.LibTemplate, dstMakeFilename)


    def __GenerateExecutableBuildFile(self, config: Config, generatorName: str, package: Package, dstMakeFilename: str) -> None:
        self.__GenerateBuildFile(config, generatorName, package, self.ExeTemplate, dstMakeFilename)
        self.__GenerateBuildScript(config, generatorName, package, self.BldTemplate)


    def __GenerateBuildScript(self, config: Config, generatorName: str, package: Package, template: str) -> None:
        strContent = ""
        for depPackage in package.ResolvedBuildOrder:
            if not depPackage.IsVirtual:
                if depPackage.AbsolutePath is None:
                    raise Exception("Invalid package")
                strContent += "pushd " + config.ToBashPath(depPackage.AbsolutePath) + " > /dev/null\n"
                strContent += 'make "$@"\n'
                strContent += "popd > /dev/null\n"
        strContent += 'make "$@"\n'

        build = template
        build = build.replace("##PACKAGE_BUILD_COMMANDS##", strContent)

        if not config.DisableWrite:
            if package.AbsolutePath is None or package.ResolvedBuildPath is None:
                raise Exception("Invalid package")
            buildPath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
            # create folder structure
            IOUtil.SafeMakeDirs(buildPath)

            # This file has been superseded by the 'FslBuild.py' script
            # so for now we just write it inside the build dir to keep it around if needed

            #dstFile = IOUtil.Join(package.AbsolutePath, "build.sh")
            dstFile = IOUtil.Join(buildPath, "build.sh")

            IOUtil.WriteFileIfChanged(dstFile, build)
            IOUtil.SetFileExecutable(dstFile)


    def __GenerateBuildFile(self, config: Config, generatorName: str, package: Package, template: str, dstMakeFilename: str) -> None:
        if package.ShortName is None or package.Namespace is None or package.ResolvedMakeObjectPath is None or package.AbsolutePath is None:
            raise Exception("Invalid Package")
        name = GeneratorGNUmakefileUtil.GetTargetName(package)
        installPath = package.Namespace
        if package.Type == PackageType.Library:
            name = "lib" + name

        if package.ResolvedBuildSourceFiles is None or package.ResolvedBuildAllIncludeDirs is None or package.ResolvedBuildAllPrivateDefines is None or package.ResolvedBuildAllPublicDefines is None:
            raise Exception("Invalid Package")

        files = MakeFileHelper.CreateList(package.ResolvedBuildSourceFiles)
        includeDirs = MakeFileHelper.CreateList(package.ResolvedBuildAllIncludeDirs)

        localDefines = Util.ExtractNames(package.ResolvedBuildAllPrivateDefines)
        localDefines += Util.ExtractNames(package.ResolvedBuildAllPublicDefines)
        localDefineNames = MakeFileHelper.CreateList(localDefines)

        variantSection = self.__GetVariantSection(package)

        if package.ResolvedMakeVariantNameHint is None:
            raise InternalErrorException("Package '{0}' ResolvedMakeVariantNameHint can not be None".format(package.Name))

        variantName = package.ResolvedMakeVariantNameHint
        build = template
        build = build.replace("##PACKAGE_TARGET_NAME##", name)
        build = build.replace("##PACKAGE_TARGET_INSTALL_PATH##", installPath)
        build = build.replace("##PACKAGE_SOURCE_FILES##", files)
        build = build.replace("##PACKAGE_INCLUDE_DIRS##", includeDirs)
        build = build.replace("##PACKAGE_DEFINES##", localDefineNames)
        build = build.replace("##PACKAGE_VARIANT_SECTION##", variantSection)
        build = build.replace("##PACKAGE_OBJECT_PATH##", package.ResolvedMakeObjectPath)
        build = build.replace("##PACKAGE_VARIANT_NAME##", variantName)


        if package.Type == PackageType.Executable:
            libraryDependencies = self.__GetLibraryDependencies(config, package)
            strLibraryDependencies = MakeFileHelper.CreateList(libraryDependencies)
            build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES##", strLibraryDependencies)
            extLibraryDependencies = self.__GetExternalLibraryDependencies(package)
            strExtLibraryDependencies = MakeFileHelper.CreateList(extLibraryDependencies)
            build = build.replace("##PACKAGE_EXTERNAL_LIBRARY_DEPENDENCIES##", strExtLibraryDependencies)
            extLibraryPaths = self.__GetExternalLibraryPaths(package, [ExternalDependencyType.StaticLib, ExternalDependencyType.DLL])
            strExtLibraryPaths = MakeFileHelper.CreateList(extLibraryPaths)
            build = build.replace("##PACKAGE_EXTERNAL_LIBRARY_PATHS##", strExtLibraryPaths)
            extDllPaths = self.__GetExternalLibraryPaths(package, [ExternalDependencyType.DLL])
            strExtDllPaths = MakeFileHelper.CreateList(extDllPaths)
            build = build.replace("##PACKAGE_EXTERNAL_DLL_PATHS##", strExtDllPaths)
            executableReport = GeneratorGNUmakefileUtil.TryGenerateExecutableReport(config, generatorName, package)
            if executableReport is not None:
                variableReport = GeneratorGNUmakefileUtil.GenerateVariableReport(config, generatorName, package)
                GitIgnoreHelper.AddFromBuildReport(self.GitIgnoreDict, package, executableReport, variableReport)

        dstFile = IOUtil.Join(package.AbsolutePath, dstMakeFilename)
        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, build)
            GitIgnoreHelper.AddPathIfInPackageRoot(self.GitIgnoreDict, package, dstFile)


    def __GetVariantSection(self, package: Package) -> str:
        variantSection = ''
        allVariants = self.__GetAllVariants(package)  # type: List[PackagePlatformVariant]
        isLibrary = package.Type == PackageType.Library
        for variant in allVariants:
            #if variant.Type != VariantType.Normal:
            #    raise NotImplementedException("This generator only supports Normal variants at the moment")
            isFirstOption = True
            for variantOption in variant.Options:
                variantSection += self.__CreateVariantOption(variant, variantOption, isLibrary, isFirstOption)
                isFirstOption = False
            if not isFirstOption:
                variantSection += self.__CreateVariantOptionEnd(variant)
        return variantSection


    def __CreateVariantOption(self, variant: PackagePlatformVariant,
                              option: PackagePlatformVariantOption,
                              isLibrary: bool,
                              isFirstOption: bool) -> str:
        strContent = ""
        if isFirstOption:
            strContent += "ifeq ($(%s),%s)\n" % (variant.Name, option.Name)
        else:
            strContent += "else ifeq ($(%s),%s)\n" % (variant.Name, option.Name)

        if len(option.DirectDefines) > 0:
            defines = Util.ExtractNames(option.DirectDefines)
            strDefines = MakeFileHelper.CreateList(defines)
            strContent += "CPP_DEFINE_FLAG_NAMES+=%s\n\n" % (strDefines)

        if len(option.ExternalDependencies) > 0:
            includes = self.__ExtractInclude(option.ExternalDependencies)
            if len(includes) > 0:
                includes.reverse() # to make GCC happy
                strIncludes = MakeFileHelper.CreateList(includes)
                strContent += "INCLUDE_DIRS+=%s\n\n" % (strIncludes)

        if not isLibrary and len(option.ExternalDependencies) > 0:
            extLibraryDependencies = Util.ExtractNames(option.ExternalDependencies)
            extLibraryDependencies.reverse() # to make GCC happy
            strExtLibraryDependencies = MakeFileHelper.CreateList(extLibraryDependencies)
            strContent += "USER_EXTLIB_DEPS+=%s\n\n" % (strExtLibraryDependencies)
        return strContent


    def __CreateVariantOptionEnd(self, variant: PackagePlatformVariant) -> str:
        name = variant.Name
        allOptions = ", ".join(Util.ExtractNames(variant.Options))
        strContent = "else\n"
        strContent += "$(error Variant %s not configured expected one of these %s)\n" % (name, allOptions)
        strContent += "endif\n"
        return strContent


    def __GetAllVariants(self, package: Package) -> List[PackagePlatformVariant]:
        variantDict = {}
        for currentPackage in package.ResolvedBuildOrder:
            for variant in currentPackage.ResolvedDirectVariants:
                variantDict[variant.Name] = variant
        return list(variantDict.values())


    def __GetLibraryDependencies(self, config: Config, package: Package) -> List[str]:
        libPaths = []  # type: List[str]
        # GCC apparently needs the list to be in reverse order
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        for entry in buildOrder:
            if entry.Type == PackageType.Library:
                asSdkBasedPath = config.TryLegacyToPath(entry.AbsolutePath)
                libPath = "%s/%s/lib%s$(TARGET_POSTFIX).a" % (asSdkBasedPath, entry.ResolvedMakeObjectPath, entry.Name)
                #libPath = asSdkBasedPath + "/$(OBJ_PATH)/lib" + entry.Name +
                #"$(TARGET_POSTFIX).a"
                libPaths.append(libPath)
        return libPaths


    def __GetExternalLibraryDependencies(self, package: Package) -> List[str]:
        libPaths = []  # type: List[str]
        # GCC apparently needs the list to be in reverse order
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        for entry in buildOrder:
            externalList = Util.ExtractNames(Util.FilterByType(entry.ResolvedDirectExternalDependencies, [ExternalDependencyType.StaticLib, ExternalDependencyType.DLL]))
            externalList.reverse()
            if len(externalList) > 0:
                externalList = self.__ApplyExternalLibNameCorrection(externalList)
            libPaths += externalList
        return libPaths

    def __ApplyExternalLibNameCorrection(self, libraryNameList: List[str]) -> List[str]:
        newList = []
        for libName in libraryNameList:
            libName = LibUtil.ToUnixLibName(libName)
            newList.append(libName)
        return newList

    def __GetExternalLibraryPaths(self, package: Package, dependencyTypeFilter: List[int]) -> List[str]:
        # GCC apparently needs the list to be in reverse order
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        additionalLibraryDirectories = set()  # type: Set[str]
        for currentPackage in buildOrder:
            extDeps = Util.FilterByType(currentPackage.ResolvedDirectExternalDependencies, dependencyTypeFilter)  # type: List[PackageExternalDependency]
            for entry in extDeps:
                if entry.Location is not None:
                    additionalLibraryDirectories.add(entry.Location)
        result = list(additionalLibraryDirectories)
        result.sort()
        return result


    def __ExtractInclude(self, entries: List[PackagePlatformExternalDependency]) -> List[str]:
        resultList = []  # type: List[str]
        for entry in entries:
            if entry.Include is not None:
                resultList.append(entry.Include)
        return resultList


class GeneratorGNUmakefileUtil(object):
    @staticmethod
    def GetTargetName(package: Package) -> str:
        if package.ShortName is None:
            raise Exception("Invalid Package")
        return package.ShortName if package.Type == PackageType.Executable else package.Name


    @staticmethod
    def GenerateVariableReport(log: Log, generatorName: str, package: Package) -> GeneratorVariableReport:
        variableReport = GeneratorVariableReport(log)
        # Add all the package variants
        for variantEntry in package.ResolvedAllVariantDict.values():
            variantEntryOptions = [option.Name for option in variantEntry.Options]
            variableReport.Add(variantEntry.Name, variantEntryOptions)

        # The make files generate executable files in debug mode with the postfix '_d'
        variableReport.Add(LocalMagicBuildVariants.GeneratorExeFileExtension, ['_d', ''], ToolAddedVariant.CONFIG)

        # make builds default to release
        variableReport.SetDefaultOption(ToolAddedVariant.CONFIG, ToolAddedVariantConfigOption.Release)
        return variableReport


    @staticmethod
    def TryGenerateBuildReport(log: Log, generatorName: str, package: Package) -> Optional[GeneratorBuildReport]:
        if package.IsVirtual:
            return None

        # Ubuntu
        # buildCommand = ['make'] + buildConfig.BuildArgs
        # Yocto
        # buildCommand = ['make', '-f', 'GNUmakefile_Yocto'] + buildConfig.BuildArgs

        if generatorName == PlatformNameString.UBUNTU:
            buildCommandArguments = []  # type: List[str]
        elif generatorName == PlatformNameString.YOCTO:
            buildCommandArguments = ['-f', 'GNUmakefile_Yocto']
        else:
            raise Exception("Unknown generator name: {0}".format(generatorName))

        # Configuration (debug, release)
        buildCommandArguments.append("{0}=${{{0}}}".format(ToolAddedVariant.CONFIG))

        # Normal variants
        for normalVariant in package.ResolvedNormalVariantNameList:
            buildCommandArguments.append("{0}=${{{0}}}".format(normalVariant))

        buildCommand = "make"
        buildCommandReport = GeneratorCommandReport(True, buildCommand, buildCommandArguments)
        return GeneratorBuildReport(buildCommandReport)


    @staticmethod
    def TryGenerateExecutableReport(log: Log, generatorName: str, package: Package) -> Optional[GeneratorExecutableReport]:
        if package.Type != PackageType.Executable or package.IsVirtual:
            return None
        if package.AbsolutePath is None:
            raise Exception("Invalid package")

        targetName = GeneratorGNUmakefileUtil.GetTargetName(package)
        # TARGET_NAME=##PACKAGE_TARGET_NAME##
        # VARIANT_NAME=##PACKAGE_VARIANT_NAME##
        # Debug
        # TARGET_POSTFIX=_d
        # Release
        # TARGET_POSTFIX=
        # From make file
        # TARGET= $(TARGET_NAME)$(VARIANT_NAME)$(TARGET_POSTFIX)
        targetPostfix = "${{{0}}}".format(LocalMagicBuildVariants.GeneratorExeFileExtension)
        exeFormatString = "{0}{1}{2}".format(targetName, package.ResolvedVariantNameHint, targetPostfix)
        return GeneratorExecutableReport(False, exeFormatString)


    @staticmethod
    def TryGenerateGeneratorPackageReport(log: Log, generatorName: str, package: Package) -> Optional[PackageGeneratorReport]:
        if package.IsVirtual:
            return None

        buildReport = GeneratorGNUmakefileUtil.TryGenerateBuildReport(log, generatorName, package)
        executableReport = GeneratorGNUmakefileUtil.TryGenerateExecutableReport(log, generatorName, package)
        variableReport = GeneratorGNUmakefileUtil.GenerateVariableReport(log, generatorName, package)
        return PackageGeneratorReport(buildReport, executableReport, variableReport)
