#!/usr/bin/env python

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

import os
import os.path
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.Generator import GitIgnoreHelper

class GeneratorGNUmakefile(object):
    def __init__(self, config, packages, dstMakeFilename, templateExe, templateLib):
        super(GeneratorGNUmakefile, self).__init__()
        self.BldTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "build.sh"))
        self.ExeTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, templateExe))
        self.LibTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, templateLib))
        self.GitIgnoreDict = {}
        for package in packages:
            if not package.ResolvedPlatformNotSupported:
                if package.Type == PackageType.Library:
                    self.__GenerateLibraryBuildFile(config, package, dstMakeFilename)
                elif package.Type == PackageType.Executable:
                    self.__GenerateExecutableBuildFile(config, package, dstMakeFilename)
            elif package.Type == PackageType.Executable:
                config.LogPrint("WARNING: Package %s marked as not supported" % (package.Name))


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return self.GitIgnoreDict


    def __GenerateLibraryBuildFile(self, config, package, dstMakeFilename):
        self.__GenerateBuildFile(config, package, self.LibTemplate, dstMakeFilename)


    def __GenerateExecutableBuildFile(self, config, package, dstMakeFilename):
        self.__GenerateBuildFile(config, package, self.ExeTemplate, dstMakeFilename)
        self.__GenerateBuildScript(config, package, self.BldTemplate)


    def __GenerateBuildScript(self, config, package, template):
        str = ""
        for depPackage in package.ResolvedBuildOrder:
            if not depPackage.IsVirtual:
                str += "pushd " + config.ToBashPath(depPackage.AbsolutePath) + " > /dev/null\n"
                str += 'make "$@"\n'
                str += "popd > /dev/null\n"
        str += 'make "$@"\n'

        build = template
        build = build.replace("##PACKAGE_BUILD_COMMANDS##", str)

        if not config.DisableWrite:
            buildPath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
            # create folder structure
            IOUtil.SafeMakeDirs(buildPath)

            # This file has been superseded by the 'FslBuild.py' script
            # so for now we just write it inside the build dir to keep it around if needed

            #dstFile = IOUtil.Join(package.AbsolutePath, "build.sh")
            dstFile = IOUtil.Join(buildPath, "build.sh")

            IOUtil.WriteFileIfChanged(dstFile, build)
            IOUtil.SetFileExecutable(dstFile)


    def __GenerateBuildFile(self, config, package, template, dstMakeFilename):
        name = package.ShortName if package.Type == PackageType.Executable else package.Name
        installPath = package.Namespace
        if package.Type == PackageType.Library:
            name = "lib" + name

        files = MakeFileHelper.CreateList(package.ResolvedBuildSourceFiles)
        includeDirs = MakeFileHelper.CreateList(package.ResolvedBuildAllIncludeDirs)

        cppLocalDefines = Util.ExtractNames(package.ResolvedBuildAllPrivateCPPDefines)
        cppLocalDefines += Util.ExtractNames(package.ResolvedBuildAllPublicCPPDefines)
        cppLocalDefineNames = MakeFileHelper.CreateList(cppLocalDefines)

        variantSection = self.__GetVariantSection(package)

        if package.ResolvedVariantName == None:
            raise InternalErrorException("Package '%s' ResolvedVariantName can not be None" % (package.Name));

        build = template
        build = build.replace("##PACKAGE_TARGET_NAME##", name)
        build = build.replace("##PACKAGE_TARGET_INSTALL_PATH##", installPath)
        build = build.replace("##PACKAGE_SOURCE_FILES##", files)
        build = build.replace("##PACKAGE_INCLUDE_DIRS##", includeDirs)
        build = build.replace("##PACKAGE_CPP_FLAGS##", cppLocalDefineNames)
        build = build.replace("##PACKAGE_VARIANT_SECTION##", variantSection)
        build = build.replace("##PACKAGE_OBJECT_PATH##", package.ResolvedMakeObjectPath)
        build = build.replace("##PACKAGE_VARIANT_NAME##", package.ResolvedVariantName)

        if package.Type == PackageType.Executable:
            libraryDependencies = self.__GetLibraryDependencies(config, package)
            libraryDependencies = MakeFileHelper.CreateList(libraryDependencies)
            build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES##", libraryDependencies)
            extLibraryDependencies = self.__GetExternalLibraryDependencies(package)
            extLibraryDependencies = MakeFileHelper.CreateList(extLibraryDependencies)
            build = build.replace("##PACKAGE_EXTERNAL_LIBRARY_DEPENDENCIES##", extLibraryDependencies)
            extLibraryPaths = self.__GetExternalLibraryPaths(package)
            extLibraryPaths = MakeFileHelper.CreateList(extLibraryPaths)
            build = build.replace("##PACKAGE_EXTERNAL_LIBRARY_PATHS##", extLibraryPaths)

        dstFile = IOUtil.Join(package.AbsolutePath, dstMakeFilename)
        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, build)
            GitIgnoreHelper.AddPathIfInPackageRoot(self.GitIgnoreDict, package, dstFile)
            if package.Type == PackageType.Executable:
                GitIgnoreHelper.AddVariantExecuteableNames(self.GitIgnoreDict, package)



    def __GetVariantSection(self, package):
        variantSection = ''
        allVariants = self.__GetAllVariants(package)
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


    def __CreateVariantOption(self, variant, option, isLibrary, isFirstOption):
        str = ""
        if isFirstOption:
            str += "ifeq ($(%s),%s)\n" % (variant.Name, option.Name)
        else:
            str += "else ifeq ($(%s),%s)\n" % (variant.Name, option.Name)

        if len(option.DirectCPPDefines) > 0:
            cppDefines = Util.ExtractNames(option.DirectCPPDefines)
            cppDefines = MakeFileHelper.CreateList(cppDefines)
            str += "CPP_DEFINE_FLAG_NAMES+=%s\n\n" % (cppDefines)

        if len(option.ExternalDependencies) > 0:
            includes = self.__ExtractInclude(option.ExternalDependencies)
            if len(includes) > 0:
                includes.reverse() # to make GCC happy
                includes = MakeFileHelper.CreateList(includes)
                str += "INCLUDE_DIRS+=%s\n\n" % (includes)

        if not isLibrary and len(option.ExternalDependencies) > 0:
            extLibraryDependencies = Util.ExtractNames(option.ExternalDependencies)
            extLibraryDependencies.reverse() # to make GCC happy
            extLibraryDependencies = MakeFileHelper.CreateList(extLibraryDependencies)
            str += "USER_EXTLIB_DEPS+=%s\n\n" % (extLibraryDependencies)
        return str


    def __CreateVariantOptionEnd(self, variant):
        name = variant.Name
        allOptions = ", ".join(Util.ExtractNames(variant.Options))
        str = "else\n"
        str += "$(error Variant %s not configured expected one of these %s)\n" % (name, allOptions)
        str += "endif\n"
        return str


    def __GetAllVariants(self, package):
        variantDict = {}
        for currentPackage in package.ResolvedBuildOrder:
            for variant in currentPackage.ResolvedDirectVariants:
                variantDict[variant.Name] = variant
        return variantDict.values()


    def __GetLibraryDependencies(self, config, package):
        libPaths = []
        # GCC apparently needs the list to be in reverse order
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        for entry in buildOrder:
            if entry.Type == PackageType.Library:
                asSdkBasedPath = config.ToPath(entry.AbsolutePath)
                libPath = "%s/%s/lib%s$(TARGET_POSTFIX).a" % (asSdkBasedPath, entry.ResolvedMakeObjectPath, entry.Name)
                #libPath = asSdkBasedPath + "/$(OBJ_PATH)/lib" + entry.Name +
                #"$(TARGET_POSTFIX).a"
                libPaths.append(libPath)
        return libPaths


    def __GetExternalLibraryDependencies(self, package):
        libPaths = []
        # GCC apparently needs the list to be in reverse order
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        for entry in buildOrder:
            externalList = Util.ExtractNames(Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib))
            externalList.reverse()
            libPaths += externalList
        return libPaths



    def __GetExternalLibraryPaths(self, package):
        # GCC apparently needs the list to be in reverse order
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        additionalLibraryDirectories = set()
        for currentPackage in buildOrder:
            extDeps = Util.FilterByType(currentPackage.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
            for entry in extDeps:
                if entry.Location != None:
                    additionalLibraryDirectories.add(entry.Location)
        return list(additionalLibraryDirectories)


    def __ExtractInclude(self, entries):
        list = []
        for entry in entries:
            if entry.Include != None:
                list.append(entry.Include)
        return list
