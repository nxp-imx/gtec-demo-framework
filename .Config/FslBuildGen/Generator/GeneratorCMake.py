#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
from typing import Tuple
from typing import Union
from enum import Enum
import json
from FslBuildGen import IOUtil
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.BuildConfig.CMakeCompileCommandsJson import CompileCommandDefine
from FslBuildGen.BuildExternal import CMakeHelper
from FslBuildGen.BuildExternal.CMakeTypes import CMakeGeneratorMultiConfigCapability
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDefine
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.Generator import CMakeGeneratorUtil
from FslBuildGen.Generator import GitIgnoreHelper
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.Report.Datatypes import FormatStringEnvironmentVariableResolveMethod
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorCommandReport import GeneratorCommandReport
from FslBuildGen.Generator.Report.GeneratorConfigCommandCMakeReport import GeneratorConfigCommandCMakeReport
from FslBuildGen.Generator.Report.GeneratorConfigCommandReport import GeneratorConfigCommandReport
from FslBuildGen.Generator.Report.GeneratorConfigReport import GeneratorConfigReport
from FslBuildGen.Generator.Report.GeneratorExecutableReport import GeneratorExecutableReport
from FslBuildGen.Generator.Report.GeneratorOpenProjectReport import GeneratorOpenProjectReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.PackageGeneratorBuildExecutableInfo import PackageGeneratorBuildExecutableInfo
from FslBuildGen.Generator.Report.PackageGeneratorConfigReport import PackageGeneratorConfigReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.Report.ReportVariableFormatter import ReportVariableFormatter
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import ToolAddedVariantConfigOption
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory

def GetVCBuildConfigurationName(entry: List[str]) -> str:
    return "-".join(entry)

class LocalMagicBuildVariants(object):
    CMakeBuildConfig = "FSL_GENERATOR_CMAKE_BUILD_CONFIG"
    GeneratorExeFileExtension = "FSLGEN_GENERATOR_EXE_FILE_EXTENSION"
    OptionCodeCoverage = "FSL_GENERATOR_CMAKE_OPTION_CODE_COVERAGE"

# Status
# - Variants are not handled
# - Install target does not work due to the way external libs are handled :(
# - Version tags and handling?
# - All apps must be options
# - All source install must be a option
# - Ensure that the 'lib' dependencies get set correctly (currently everything is public)
# - Root CMakeList should work with 'Extended' repos
# - Need to handle FEATURES as options
# - cmake projects currently only handle one platform at a time. A final version should contain support for all platforms.
# - Using the 'root' CMakeLists.txt is kind of a 'work around' to allow us to re-use libraries
#   It would have been better to have a unique build file for each package with its own 'build' dir
#   However that would be more complex to implement and might make it impossible to have config'all'
#   package dependencies added as sub-projects in the IDE.
# - All 'environment' variables should be converted to cmake variables instead
#   These variables should be populated by extracting the env variables if present
# Note:
# - The package headers might not completely follow the FslBuildGen standards
#   Meaning some packages might have access to more than their Fsl.gen file
#   allows them access to

# Handled:
# - External libs with special debug libraries are not handled

# This generator does not work if there are multiple source roots :(


# Store information about the exe filename for the toolchain
# Unfortunately CMake only makes the final "exe" location available at build time.
# meaning its a lot more complex to acquire than in the old system.
#
# Something like this can be used to print the information
#
# add_custom_command(
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeAppRunInfo.json
#  COMMAND ${CMAKE_COMMAND} -E echo $<TARGET_FILE:Console.ConsoleMinimal101>
#  COMMENT "Storing exe info"
# )
#
# This could be adapted to write a "CMakeAppRunInfo.json" using a custom tool
# that is run instead of just doing a "echo".
#
# add_custom_command(
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/.CMakeAppRunInfo.json
#  COMMAND FslBuildSave.py --append "BuildPath=$<TARGET_FILE:Console.ConsoleMinimal101>"
#                          --append "InstallPath=${CMAKE_INSTALL_BINDIR}/##PACKAGE_TARGET_INSTALL_PATH##"
#                          --SaveJSON ${CMAKE_CURRENT_BINARY_DIR}/.CMakeAppRunInfo.json
#  COMMENT "Storing exe info"
#  VERBATIM
# )
#
# Unfortunately our toolchain currently expects the information to be available at "config" time.
#

class CMakeGeneratorMode(Enum):
    Normal = 0
    Tidy = 1

class ProjectContextCacheRecord(object):
    def __init__(self) -> None:
        super().__init__()

class GeneratorCMake(GeneratorBase):
    def __init__(self, log: Log, toolConfig: ToolConfig, packages: List[Package], platformName: str, templateName: str,
                 overrideTemplateName: Optional[str], cmakeBuildPackageDir: str, sdkConfigTemplatePath: str, disableWrite: bool,
                 generatorMode: CMakeGeneratorMode) -> None:
        super().__init__()

        self.__DisableWrite = disableWrite

        uniqueNormalVariables, uniqueEnvironmentVariables = CMakeGeneratorUtil.ExtractUniqueVariables(packages)

        # Create a quick lookup table
        toolProjectContextsDict = {projectContext.ProjectId : projectContext for projectContext in toolConfig.ProjectInfo.Contexts}

        strTemplatePath = templateName
        extTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Ext", False, overrideTemplateName)
        libTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Lib", False, overrideTemplateName)
        exeTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Exe", False, overrideTemplateName)
        rootTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Root", False, overrideTemplateName)
        notSupportedTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "NotSupported", False, overrideTemplateName)

        useExtendedProjectHack = True
        for package in packages:
            isSupported = package.ResolvedPlatformSupported
            if package.Type == PackageType.ExternalLibrary or package.Type == PackageType.HeaderLibrary:
                self.__GenerateCMakeFile(log, toolConfig, cmakeBuildPackageDir, package, platformName,
                                         extTemplate if isSupported else notSupportedTemplate, toolProjectContextsDict, useExtendedProjectHack,
                                         uniqueEnvironmentVariables, generatorMode)
            elif package.Type == PackageType.Library:
                self.__GenerateCMakeFile(log, toolConfig, cmakeBuildPackageDir, package, platformName,
                                         libTemplate if isSupported else notSupportedTemplate, toolProjectContextsDict, useExtendedProjectHack,
                                         uniqueEnvironmentVariables, generatorMode)
            elif package.Type == PackageType.Executable:
                self.__GenerateCMakeFile(log, toolConfig, cmakeBuildPackageDir, package, platformName,
                                         exeTemplate if isSupported else notSupportedTemplate, toolProjectContextsDict, useExtendedProjectHack,
                                         uniqueEnvironmentVariables, generatorMode)
            elif package.Type == PackageType.TopLevel:
                self.__GenerateRootCMakeFile(log, toolConfig, cmakeBuildPackageDir, package, platformName, rootTemplate, toolProjectContextsDict,
                                             useExtendedProjectHack, uniqueEnvironmentVariables)


    def __GenerateCMakeFile(self, log: Log, toolConfig: ToolConfig, cmakeBuildPackageDir: str, package: Package, platformName: str, template: CMakeGeneratorUtil.CodeTemplateCMake,
                            toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext],
                            useExtendedProjectHack: bool, uniqueEnvironmentVariables: Set[str],
                            generatorMode: CMakeGeneratorMode) -> None:
        if package.Type == PackageType.TopLevel:
            raise Exception("Usage error")
        #if package.IsVirtual:
        #    return
        if package.AbsolutePath is None or package.ResolvedBuildPath is None:
            raise Exception("Invalid package")

        packageName = CMakeGeneratorUtil.GetPackageName(package)

        aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName, package.ProjectContext.ProjectName)

        targetIncludeDirectories = CMakeGeneratorUtil.BuildTargetIncludeDirectories(toolConfig, package, template.PackageTargetIncludeDirectories,
                                                                                    template.PackageTargetIncludeDirEntry, template.PackageTargetIncludeDirVirtualEntry,
                                                                                    CMakeGeneratorUtil.CMakePathType.LocalRelative)

        publicIncludeFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildPublicIncludeFiles)
        privateIncludeFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildPrivateIncludeFiles)
        includeFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildAllIncludeFiles)
        sourceFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildSourceFiles)

        linkLibrariesDirectDependencies = CMakeGeneratorUtil.BuildTargetLinkLibrariesForDirectDependencies(log, package,
                                                                                                           template.PackageDependencyTargetLinkLibraries,
                                                                                                           template.PackageDependencyFindPackage)

        extraDefines = None
        if generatorMode == CMakeGeneratorMode.Tidy and not package.IsVirtual:
            extraDefines = GeneratorCMake.__GenerateTidyDefines(package)

        directDefinitions = CMakeGeneratorUtil.BuildDirectDefinitions(log, package, template.PackageDependencyTargetCompileDefinitions, extraDefines)


        findDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(log, package, template.PackageDependencyFindPackage)
        installInstructions = CMakeGeneratorUtil.BuildInstallInstructions(log, package, template.PackageInstall,
                                                                          template.PackageInstallTargets,
                                                                          template.PackageInstallHeaders,
                                                                          template.PackageInstallContent,
                                                                          template.PackageInstallDLL,
                                                                          template.PackageInstallAppInfo)
        targetCompileFeatures = CMakeGeneratorUtil.BuildCompileFeatures(log, package, template.SnippetTargetCompileFeaturesDefault,
                                                                        template.SnippetTargetCompileFeaturesInterface)
        targetCompileOptions = CMakeGeneratorUtil.BuildCompileOptions(log, package, template.SnippetTargetCompileOptionsDefault)
        targetFileCopy = CMakeGeneratorUtil.BuildFileCopy(log, package, template.PackageTargetCopyFile, template.PackageTargetCopyFilePath)

        cacheVariants = CMakeGeneratorUtil.GetCacheVariants(package, template.SnippetCacheVariant)

        contentInBinaryDirectory = True

        packageContentBuilder = CMakeGeneratorUtil.GetContentBuilder(toolConfig, package, platformName, template.PackageContentBuilder, contentInBinaryDirectory)
        #packageContentBuilderOutputFiles = CMakeGeneratorUtil.GetContentBuilderOutputFiles(toolConfig, package, contentInBinaryDirectory)

        packageContentSection = CMakeGeneratorUtil.GetContentSection(toolConfig, package, platformName, template.PackageContent, template.PackageContentFile, contentInBinaryDirectory)
        #packageContentSectionOutputFiles = CMakeGeneratorUtil.GetContentSectionOutputFiles(toolConfig, package, contentInBinaryDirectory)

        packageContentDep = CMakeGeneratorUtil.GetContentDepSection(toolConfig, package, platformName, template.PackageContentDep, contentInBinaryDirectory)
        packageContentDepOutputFiles = CMakeGeneratorUtil.GetContentDepOutputFile(log, package, contentInBinaryDirectory)

        packageCompilerSpecificFileDependencies = CMakeGeneratorUtil.CompilerSpecificFileDependencies(toolConfig, package,
                                                                                                      template.PackageCompilerConditional,
                                                                                                      template.PackageTargetSourceFiles,
                                                                                                      template.PackageCompilerFileDict)

        packageVariantSettings = CMakeGeneratorUtil.GetVariantSettings(log, package, template.PackageVariantSettings,
                                                                       template.PackageDependencyTargetCompileDefinitions,
                                                                       template.PackageDependencyTargetLinkLibraries)


        packageTargetSpecialFiles = CMakeGeneratorUtil.GetTargetSpecialFiles(log, toolConfig, package, template.PackageTargetSpecialFileNatvis)

        sourceGroups = GeneratorCMake.__GenerateSourceGroups(toolConfig, package)

        packagePath = CMakeGeneratorUtil.GetPackageSDKBasedPathUsingCMakeVariable(toolConfig, package, "")
        if packagePath.endswith('/'):
            packagePath = packagePath[0:-1]

        buildCMakeFile = template.Master
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_INCLUDE_FILES##", includeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_PUBLIC_INCLUDE_FILES##", publicIncludeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_PRIVATE_INCLUDE_FILES##", privateIncludeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_SOURCE_FILES##", sourceFiles)
        buildCMakeFile = buildCMakeFile.replace("##TARGET_INCLUDE_DIRECTORIES##", targetIncludeDirectories)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##", linkLibrariesDirectDependencies)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##", directDefinitions)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", findDirectExternalDependencies)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##", targetCompileOptions)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##", targetCompileFeatures)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_COPY_FILES##", targetFileCopy)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_GENERATE_INSTALL_INSTRUCTIONS##", installInstructions)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENTBUILDER##", packageContentBuilder)
        #buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENTBUILDER_OUTPUT_FILES##", packageContentBuilderOutputFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENTSECTION_OUTPUT##", packageContentSection)
        #buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENTSECTION_OUTPUT_FILES##", packageContentSectionOutputFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENTDEP##", packageContentDep)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENTDEP_OUTPUT_FILES##", packageContentDepOutputFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_COMPILER_SPECIFIC_FILE_DEPENDENCIES##", packageCompilerSpecificFileDependencies)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_VARIANT_SETTINGS##", packageVariantSettings)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_TARGET_SPECIAL_FILES##", packageTargetSpecialFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_PATH##", packagePath)

        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_SOURCE_GROUP##", sourceGroups)

        toolProjectContext = toolProjectContextsDict[package.ProjectContext.ProjectId]
        sectionDefinePathEnvAsVariables = CMakeGeneratorUtil.CreateDefineRootDirectoryEnvironmentAsVariables(toolConfig, toolProjectContext,
                                                                                                             useExtendedProjectHack,
                                                                                                             template.PathEnvToVariable,
                                                                                                             uniqueEnvironmentVariables)
        buildCMakeFile = self.__CommonReplace(buildCMakeFile, package.ProjectContext, packageName, aliasPackageName, cacheVariants,
                                              sectionDefinePathEnvAsVariables, toolConfig.CMakeConfiguration.MinimumVersion,
                                              template)

        dstFilename = GeneratorCMake._GetPackageBuildFileName(toolConfig, cmakeBuildPackageDir, package)
        dstFilenameDir = IOUtil.GetDirectoryName(dstFilename)
        if not self.__DisableWrite:
            IOUtil.SafeMakeDirs(dstFilenameDir)
            self.__SaveFile(dstFilename, buildCMakeFile)
            #GitIgnoreHelper.SafeAddEntry(self.GitIgnoreDict, package, "CMakeLists.txt")

    @staticmethod
    def __GenerateSourceGroups(toolConfig: ToolConfig, package: Package) -> str:
        resDict = {}  # type: Dict[str, List[str]]
        if package.ResolvedBuildAllIncludeFiles is not None:
            GeneratorCMake.__GroupByDir(resDict, package.ResolvedBuildAllIncludeFiles)
        if package.ResolvedBuildSourceFiles is not None:
            GeneratorCMake.__GroupByDir(resDict, package.ResolvedBuildSourceFiles)

        #  CMakeGeneratorUtil.GetPackageSDKBasedPathUsingCMakeVariable(toolConfig, package, entry)

        res = [] # type: List[str]

        sortedDirectories = list(resDict.keys())
        sortedDirectories.sort()

        for dirName in sortedDirectories:
            files = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, resDict[dirName])
            res.append('source_group("{0}" FILES {1})'.format(dirName, files))

        return "\n".join(res)

    @staticmethod
    def __GroupByDir(resDict: Dict[str, List[str]], files: List[str]) -> None:
        for entry in files:
            directory = IOUtil.GetDirectoryName(entry)
            if directory not in resDict:
                resDict[directory] = [entry]
            else:
                resDict[directory].append(entry)
        for fileList in resDict.values():
            fileList.sort()


    @staticmethod
    def __GenerateTidyDefines(package: Package) -> List[PackageDefine]:
        packageNameDefineStr = "{0}{1}".format(CompileCommandDefine.PackageName, package.NameInfo.FullName.Value)
        packageNameDefine = GeneratorCMake.__GenerateTidyDefine(packageNameDefineStr, AccessType.Private, package.NameInfo.FullName.Value, AccessType.Private)
        return [packageNameDefine]

    @staticmethod
    def __GenerateTidyDefine(name: str, access: AccessType, introducedByPackageName: str, fromPackageAccess: AccessType) -> PackageDefine:
        return PackageDefine(UnresolvedPackageDefine(name, access), introducedByPackageName, fromPackageAccess)


    def __GenerateRootCMakeFile(self, log: Log, toolConfig: ToolConfig, cmakeBuildPackageDir: str, package: Package, platformName: str,
                                template: CMakeGeneratorUtil.CodeTemplateCMake, toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext],
                                useExtendedProjectHack: bool, uniqueEnvironmentVariables: Set[str]) -> None:
        if package.Type != PackageType.TopLevel:
            return

        projectContexts = CMakeGeneratorUtil.GetProjectContexts(package)

        for projectContext in projectContexts:
            # Top level package handling
            if projectContext.ProjectId not in toolProjectContextsDict:
                raise Exception("Internal error could not locate tool project context")

            toolProjectContext = toolProjectContextsDict[projectContext.ProjectId]
            projectAbsolutePath = toolProjectContext.Location.ResolvedPath

            packageName = "TopLevel_{0}".format(projectContext.ProjectName)

            addSubDirectoriesDirectDependencies = self.__BuildAddSubDirectoriesForTopLevelDirectDependencies(toolConfig, package, projectAbsolutePath, template)
            #findExternalDependencies = self.__BuildFindExternalDependencies(toolConfig, package, template)

            aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName, projectContext.ProjectName)

            findDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(log, package, template.PackageDependencyFindPackage)

            allPackageNames = CMakeGeneratorUtil.GetAllPackageNames(package, projectContext if not useExtendedProjectHack else None)

            cacheVariants = CMakeGeneratorUtil.GetCacheVariants(package, template.SnippetCacheVariant)

            extendedProjectParent = CMakeGeneratorUtil.GetAddExtendedPackageParent(toolConfig, toolProjectContext, template.AddExtendedPackage)

            buildCMakeFile = template.Master
            buildCMakeFile = buildCMakeFile.replace("##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", findDirectExternalDependencies)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##", addSubDirectoriesDirectDependencies)
            buildCMakeFile = buildCMakeFile.replace("##ALL_PACKAGE_NAMES##", "\n  ".join(allPackageNames))
            buildCMakeFile = buildCMakeFile.replace("##EXTENDED_PROJECT_PARENT##", extendedProjectParent)

            sectionDefinePathEnvAsVariables = CMakeGeneratorUtil.CreateDefineRootDirectoryEnvironmentAsVariables(toolConfig, toolProjectContext,
                                                                                                                 useExtendedProjectHack,
                                                                                                                 template.PathEnvToVariable,
                                                                                                                 uniqueEnvironmentVariables)
            buildCMakeFile = self.__CommonReplace(buildCMakeFile, projectContext, packageName, aliasPackageName, cacheVariants,
                                                  sectionDefinePathEnvAsVariables, toolConfig.CMakeConfiguration.MinimumVersion,
                                                  template)

            dstFilename = GeneratorCMake._GetProjectPackageBuildFileName(cmakeBuildPackageDir, toolProjectContext)
            dstFilenameDir = IOUtil.GetDirectoryName(dstFilename)
            if not self.__DisableWrite:
                IOUtil.SafeMakeDirs(dstFilenameDir)
                self.__SaveFile(dstFilename, buildCMakeFile)


    def __CommonReplace(self, content: str, projectContext: Union[PackageProjectContext, ToolConfigProjectContext], packageName: str, aliasPackageName: str,
                        cacheVariants: str, sectionDefinePathEnvAsVariables: str, cmakeMinimumVersion: CMakeVersion,
                        template: CMakeGeneratorUtil.CodeTemplateCMake) -> str:

        cmakeMinimumVersionStr = "{0}.{1}.{2}".format(cmakeMinimumVersion.Major, cmakeMinimumVersion.Minor, cmakeMinimumVersion.Build)

        content = content.replace("##COMMON_HEADER##", template.SnippetCommonHeader)
        content = content.replace("##SNIPPET_COMMON_MODULES##", template.SnippetCommonModules)
        content = content.replace("##SNIPPET_OPTIONS_EXE##", template.SnippetOptionsEXE)
        content = content.replace("##SNIPPET_OPTIONS_LIB##", template.SnippetOptionsLIB)
        content = content.replace("##PROJECT_PREVENT_IN_SOURCE_BUILDS##", template.SnippetProjectPreventSourceBuilds)

        content = content.replace("##PACKAGE_NAME!##", packageName.upper())
        content = content.replace("##PACKAGE_NAME##", packageName)
        content = content.replace("##ALIAS_PACKAGE_NAME##", aliasPackageName)
        content = content.replace("##PROJECT_NAME##", projectContext.ProjectName)
        content = content.replace("##PROJECT_VERSION##", projectContext.ProjectVersion)
        content = content.replace("##SNIPPET_CACHE_VARIANTS##", cacheVariants)
        content = content.replace("##DEFINE_PATH_ENVIRONMENT_AS_VARIABLES##", sectionDefinePathEnvAsVariables)
        content = content.replace("##CMAKE_MINIMUM_VERSION##", cmakeMinimumVersionStr)
        return content


    def __SaveFile(self, dstFileCMakeFile: str, buildCMakeFile: str) -> None:
        if self.__DisableWrite:
            return
        IOUtil.WriteFileIfChanged(dstFileCMakeFile, buildCMakeFile)


    @staticmethod
    def _GetBuildFileName(saveBasePath: str) -> str:
        return IOUtil.Join(saveBasePath, "CMakeLists.txt")

#    def _GetProjectPackageBuildFileName(location: ResolvedPath) -> str:
#        return GeneratorCMake._GetBuildFileName(location.ResolvedPath)

    @staticmethod
    def _GetProjectPackageBuildFileName(cmakeBuildPackageDir: str, toolProjectContext: ToolConfigProjectContext) -> str:
        path = GeneratorCMake._GetProjectDirectoryName(cmakeBuildPackageDir, toolProjectContext)
        return GeneratorCMake._GetBuildFileName(path)

    @staticmethod
    def _GetProjectDirectoryName(cmakeBuildPackageDir: str, projectContext: Union[ToolConfigProjectContext, PackageProjectContext]) -> str:
        return IOUtil.Join(cmakeBuildPackageDir, projectContext.ProjectId.ShortProjectId)

    @staticmethod
    def _GetPackageRootPath(toolConfig: ToolConfig, package: Package) -> ToolConfigRootDirectory:
        if package.AbsolutePath is None:
            raise Exception("Invalid package '{0}'".format(package.NameInfo.FullName))
        rootDirectory = toolConfig.TryFindRootDirectory(package.AbsolutePath)
        if rootDirectory is None:
            raise Exception("Could not find root directory for package '{0}'".format(package.NameInfo.FullName))
        if not package.AbsolutePath.startswith(rootDirectory.ResolvedPathEx):
            raise Exception("Found root directory did not match for package '{0}'".format(package.NameInfo.FullName))
        return rootDirectory

    @staticmethod
    def _GetPackageRelativePath(toolConfig: ToolConfig, package: Package) -> str:
        if package.AbsolutePath is None:
            raise Exception("Invalid package '{0}'".format(package.NameInfo.FullName))

        rootDirectory = GeneratorCMake._GetPackageRootPath(toolConfig, package)
        packageRelativePath = package.AbsolutePath[len(rootDirectory.ResolvedPathEx):]
        packageRelativePath = IOUtil.GetDirectoryName(packageRelativePath)
        return IOUtil.Join(packageRelativePath, package.NameInfo.ShortName.Value)

    @staticmethod
    def _GetPackageBuildFileName(toolConfig: ToolConfig, cmakeBuildPackageDir: str, package: Package) -> str:
        #return GeneratorCMake._GetBuildFileName(package.AbsolutePath)
        finalPath = GeneratorCMake._GetProjectDirectoryName(cmakeBuildPackageDir, package.ProjectContext)
        #packageRelativePath = package.NameInfo.FullName.Value
        #packageRelativePath = IOUtil.GetDirectoryName(package.Path.RootRelativeDirPath)
        #packageRelativePath = IOUtil.Join(packageRelativePath, package.NameInfo.ShortName.Value)

        packageRelativePath = GeneratorCMake._GetPackageRelativePath(toolConfig, package)
        finalPath = IOUtil.Join(finalPath, packageRelativePath)

        #packageRootPath = config.ToPath(package.AbsolutePath)
        return GeneratorCMake._GetBuildFileName(finalPath)


    def __Join(self, srcList: Optional[List[str]]) -> str:
        if srcList is None or len(srcList) <= 0:
            return ''
        return "\n  " + "\n  ".join(srcList)


    def __BuildAddSubDirectoriesForTopLevelDirectDependencies(self, toolConfig: ToolConfig, package: Package, projectAbsolutePath: str,
                                                              template: CMakeGeneratorUtil.CodeTemplateCMake) -> str:
        if package.Type != PackageType.TopLevel:
            return ""
        if len(package.ResolvedBuildOrder) <= 0:
            return ""
        content = ""
        dependencies = list(package.ResolvedBuildOrder)
        dependencies.sort(key=lambda s: s.AbsolutePath.lower() if s.AbsolutePath is not None else s.Name)
        projectAbsolutePathEx = projectAbsolutePath + '/'
        for entry in dependencies:
            if entry != package and entry.Type != PackageType.ToolRecipe:
                if entry.AbsolutePath is None:
                    raise Exception("Invalid package")
                if entry.AbsolutePath.startswith(projectAbsolutePathEx):
                    #path = CMakeGeneratorUtil.GetRelativePath(config, projectAbsolutePath, entry.AbsolutePath)
                    path = GeneratorCMake._GetPackageRelativePath(toolConfig, entry)
                    #path = entry.NameInfo.FullName.Value
                    content += template.PackageDependencyAddSubdirectories.replace("##PACKAGE_PATH##", path)
        return content


    @staticmethod
    def _TryGenerateBuildReport(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig,
                                package: Package, isMasterBuild: bool) -> Optional[GeneratorBuildReport]:
        if package.IsVirtual and not isMasterBuild:
            return None

        # preBuildCommand = ['cmake', '-G', 'Visual Studio 15 2017 Win64'] + buildConfig.BuildArgs
        # buildCommand = ['cmake', '--build', '.', '--config', 'Debug'] + buildConfig.BuildArgs

        buildCommandArguments = ['--build', '.']  # type: List[str]
        buildCommandNativeArguments = [] # type: List[str]
        if generatorConfig.BuildCommand != CommandType.Open:
            # Configuration (Debug, Release) for the configurations that support build time configuration switching
            if CMakeHelper.GetGeneratorMultiConfigCapabilities(cmakeConfig.GeneratorName) == CMakeGeneratorMultiConfigCapability.Yes:
                buildCommandArguments.append("--config")
                buildCommandArguments.append("${{{0}}}".format(LocalMagicBuildVariants.CMakeBuildConfig))

            # Do a fallback solution for a few generators that we know how work
            nativeArguments = CMakeHelper.GetNativeBuildThreadArguments(cmakeConfig.GeneratorName, generatorConfig.NumBuildThreads)
            if len(nativeArguments) > 0:
                buildCommandNativeArguments += nativeArguments
            elif cmakeConfig.CMakeVersion >= CMakeVersion(3, 12, 0):
                # Take advantage of the "--parallel" parameter available from 3.12 when possible
                # We use this as a fallback since testing has shown that even on 3.14 the parameter
                # doesn't always provide any benefits :(
                buildCommandArguments.append("--parallel")
                buildCommandArguments.append("{0}".format(generatorConfig.NumBuildThreads))
            elif isMasterBuild:
                log.LogPrintWarning("BuildThreads not supported for generator '{0}' please upgrade to CMake 3.12+".format(cmakeConfig.GeneratorName))

            # Add extra commands based on the build type
            if generatorConfig.BuildCommand == CommandType.Clean:
                buildCommandArguments.append("--target")
                buildCommandArguments.append("clean")
            elif generatorConfig.BuildCommand == CommandType.Install:
                buildCommandArguments.append("--target")
                buildCommandArguments.append("install")
        else:
            buildCommandArguments = ['--open', '.']

         # set the package build dir
        buildCWD = cmakeConfig.BuildDir
        if not isMasterBuild:
            buildCWD = GeneratorCMake._GetPackageBuildDir(generatorConfig, cmakeConfig, package)

        buildCommand = "cmake"
        buildCommandReport = GeneratorCommandReport(True, buildCommand, buildCommandArguments, buildCommandNativeArguments, buildCWD, nativeArgumentSeparator="--")
        return GeneratorBuildReport(buildCommandReport)

    @staticmethod
    def _GetPackageBuildDir(generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig, package: Package) -> str:
        buildPath = cmakeConfig.BuildDir
        if package.AbsolutePath is None:
            raise Exception("package.Absolute path is None")
        rootDir = generatorConfig.ToolConfig.TryFindRootDirectory(package.AbsolutePath)
        if rootDir is None:
            raise Exception("could not find root dir for package")
        relativePath = package.AbsolutePath[len(rootDir.ResolvedPathEx):]

        return IOUtil.Join(buildPath, relativePath)

    @staticmethod
    def _TryGenerateExecutableReport(log: Log, package: Package) -> Optional[GeneratorExecutableReport]:
        if package.Type != PackageType.Executable or package.IsVirtual:
            return None
        if package.AbsolutePath is None:
            raise Exception("Invalid package")

        targetName = package.Name

        # handle DEBUG builds with "_d" postfix
        targetPostfix = "${{{0}}}".format(LocalMagicBuildVariants.GeneratorExeFileExtension)

        exeFormatString = "{0}{1}{2}".format(targetName, package.ResolvedVariantNameHint, targetPostfix)
        return GeneratorExecutableReport(False, exeFormatString)

    @staticmethod
    def _GenerateVariableReport(log: Log, package: Package, configVariantOptions: List[str],
                                isMasterBuild: bool) -> GeneratorVariableReport:
        variableReport = GeneratorVariableReport(log, configVariantOptions=configVariantOptions)
        # Add all the package variants
        for variantEntry in package.ResolvedAllVariantDict.values():
            variantEntryOptions = [option.Name for option in variantEntry.Options]
            variableReport.Add(variantEntry.Name, variantEntryOptions)

        # The make files generate executable files in debug mode with the postfix '_d'
        exeFileExtensionOptionList = ['_d', '']
        # This is a bit ugly as we just assume coverage will be last
        if ToolAddedVariantConfigOption.Coverage in configVariantOptions:
            exeFileExtensionOptionList.append('_c')
        variableReport.Add(LocalMagicBuildVariants.GeneratorExeFileExtension, exeFileExtensionOptionList, ToolAddedVariant.CONFIG)

        # CMake names for debug and release building
        variableReport.Add(LocalMagicBuildVariants.CMakeBuildConfig, ['Debug', 'Release', 'Debug'], ToolAddedVariant.CONFIG)

        # make builds default to release
        variableReport.SetDefaultOption(ToolAddedVariant.CONFIG, ToolAddedVariantConfigOption.GetDefaultSetting())
        return variableReport

    @staticmethod
    def __TryGenerateOpenReport(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig,
                                cmakeBuildPackageDir: str, package: Package) -> Optional[GeneratorOpenProjectReport]:
        if package.IsVirtual or package.AbsolutePath is None:
            return None

        sourcePath = GeneratorCMake._GetPackageRootPath(generatorConfig.ToolConfig, package).ResolvedPath
        #packageBuildDirectory = GeneratorCMake._GetPackageBuildDir(generatorConfig, cmakeConfig, package)
        buildSourceDirectory = GeneratorCMake._GetProjectDirectoryName(cmakeBuildPackageDir, package.ProjectContext)

        return GeneratorOpenProjectReport(sourcePath, buildSourceDirectory)

    @staticmethod
    def TryGenerateGeneratorPackageReport(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig,
                                          cmakeBuildPackageDir: str, package: Package,
                                          configVariantOptions: List[str]) -> Optional[PackageGeneratorReport]:
        if package.IsVirtual and package.Type != PackageType.HeaderLibrary and package.Type != PackageType.TopLevel:
            return None

        isMasterBuild = False
        #preBuildReport = GeneratorCMake.TryGeneratePreBuildReport(log, generatorName, package)
        buildReport = GeneratorCMake._TryGenerateBuildReport(log, generatorConfig, cmakeConfig, package, isMasterBuild)
        executableReport = GeneratorCMake._TryGenerateExecutableReport(log, package)
        variableReport = GeneratorCMake._GenerateVariableReport(log, package, configVariantOptions, isMasterBuild)

        openProjectReport = GeneratorCMake.__TryGenerateOpenReport(log, generatorConfig, cmakeConfig, cmakeBuildPackageDir, package)
        return PackageGeneratorReport(buildReport, executableReport, variableReport, openProjectReport)

    @staticmethod
    def _ExtractRecipeInstallPaths(log: Log, package: Package) -> List[str]:
        res = []
        for depPackage in package.ResolvedBuildOrder:
            if (depPackage.ResolvedDirectExperimentalRecipe is not None and depPackage.ResolvedDirectExperimentalRecipe.AllowFind and
                    depPackage.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation is not None):
                newPath = depPackage.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation.ResolvedPath
                res.append(newPath)
                if ';' in newPath:
                    raise Exception("The recipe install path '{0}' can not contain a ';' ('{1}')".format(newPath, depPackage.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation.SourcePath))
                if ' ' in newPath:
                    raise Exception("The recipe install path '{0}' can not contain spaces ('{1}')".format(newPath, depPackage.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation.SourcePath))
        res.sort(key=lambda s: s.upper())
        return res

    @staticmethod
    def _GenerateConfigReport(log: Log, toolConfig: ToolConfig, platformName: str, cmakeConfig: GeneratorCMakeConfig,
                              cmakeBuildPackageDir: str, topLevelPackage: Package) -> GeneratorConfigReport:
        if topLevelPackage.Type != PackageType.TopLevel:
            raise Exception("Package is not a top level package")

        cmakeConfigureSettingsDict = {}
        cmakePrefixPathList = []

        if cmakeConfig.InstallPrefix is not None:
            cmakeConfigureSettingsDict["CMAKE_INSTALL_PREFIX"] = cmakeConfig.InstallPrefix

        # Configuration (Debug, Release) for the configurations that support configure time configuration
        if CMakeHelper.GetGeneratorMultiConfigCapabilities(cmakeConfig.GeneratorName) != CMakeGeneratorMultiConfigCapability.Yes:
            cmakeConfigureSettingsDict["CMAKE_BUILD_TYPE"] = "${{{0}}}".format(LocalMagicBuildVariants.CMakeBuildConfig)

        cmakeConfigureSettingsDict["CODE_COVERAGE"] = "${{{0}}}".format(LocalMagicBuildVariants.OptionCodeCoverage)

        # Normal variants
        for normalVariant in topLevelPackage.ResolvedNormalVariantNameList:
            key = "{0}".format(normalVariant)
            cmakeConfigureSettingsDict[key] = "${{{0}}}".format(normalVariant)

        recipePaths = GeneratorCMake._ExtractRecipeInstallPaths(log, topLevelPackage)
        if len(recipePaths) > 0:
            cmakeConfigureSettingsDict["CMAKE_PREFIX_PATH"] = "{0}".format(";".join(recipePaths))
            cmakePrefixPathList = list(recipePaths)

        # Top level package handling
        cmakeCommandArguments = []  # type: List[str]
        buildCommandArguments = []  # type: List[str]

        for key, value in cmakeConfigureSettingsDict.items():
            buildCommandArguments.append("-D{0}={1}".format(key, value))

        # we handle the prefix paths differently since vscode prefers to get it in a list
        if "CMAKE_PREFIX_PATH" in cmakeConfigureSettingsDict:
            del cmakeConfigureSettingsDict["CMAKE_PREFIX_PATH"]

        # Set the generator name
        buildCommandArguments.append("-G")
        buildCommandArguments.append(cmakeConfig.GeneratorName)

        # Add additional arguments
        additionalGeneratorArguments = cmakeConfig.CMakeConfigAppArguments
        if len(additionalGeneratorArguments) > 0:
            buildCommandArguments += additionalGeneratorArguments
            #cmakeCommandArguments += additionalGeneratorArguments

        # Add the path to the root cmake file
        #topToolProjectContext = toolConfig.ProjectInfo.TopProjectContext
        #projectAbsolutePath = topToolProjectContext.Location.ResolvedPath
        #buildCommandArguments.append(projectAbsolutePath)
        dstFilename = GeneratorCMake._GetProjectPackageBuildFileName(cmakeBuildPackageDir, toolConfig.ProjectInfo.TopProjectContext)
        buildCommandArguments.append(IOUtil.GetDirectoryName(dstFilename))

        # Set the build dir
        configCWD = cmakeConfig.BuildDir

        # FIX: See how the visual studio generator handles debug/release builds

        configCommand = "cmake"
        configCommandReport = GeneratorConfigCommandReport(configCommand, buildCommandArguments, configCWD)
        configCommandCMakeReport = GeneratorConfigCommandCMakeReport(cmakeConfigureSettingsDict, cmakeCommandArguments, configCWD, cmakePrefixPathList)

        return GeneratorConfigReport(configCommandReport, configCommandCMakeReport)

    @staticmethod
    def _GenerateConfigVariableReport(log: Log, topLevelPackage: Package, configVariantOptions: List[str]) -> GeneratorVariableReport:
        variableReport = GeneratorVariableReport(log, configVariantOptions=configVariantOptions)
        # Add all the package variants
        for variantEntry in topLevelPackage.ResolvedAllVariantDict.values():
            variantEntryOptions = [option.Name for option in variantEntry.Options]
            variableReport.Add(variantEntry.Name, variantEntryOptions)

        # CMake names for debug and release building
        variableReport.Add(LocalMagicBuildVariants.CMakeBuildConfig, ['Debug', 'Release', 'Debug'], ToolAddedVariant.CONFIG)
        # CMake settings for the code coverage option
        variableReport.Add(LocalMagicBuildVariants.OptionCodeCoverage, ['OFF', 'OFF', 'ON'], ToolAddedVariant.CONFIG)

        # make builds default to release
        variableReport.SetDefaultOption(ToolAddedVariant.CONFIG, ToolAddedVariantConfigOption.GetDefaultSetting())
        return variableReport


    @staticmethod
    def _GeneratedFileSet(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig, cmakeBuildPackageDir: str,
                          topLevelPackage: Package) -> Set[str]:
        """
        Generate a list of all build files that will be generated for the given topLevelPackage by this generator
        """
        fileSet = set() # type: Set[str]

        for toolProjectContext in generatorConfig.ToolConfig.ProjectInfo.Contexts:
            filename = GeneratorCMake._GetProjectPackageBuildFileName(cmakeBuildPackageDir, toolProjectContext)
            if IOUtil.IsFile(filename):
                fileSet.add(filename)

        for depPackage in topLevelPackage.ResolvedAllDependencies:
            filename = GeneratorCMake._GetPackageBuildFileName(generatorConfig.ToolConfig, cmakeBuildPackageDir, depPackage.Package)
            if IOUtil.IsFile(filename):
                fileSet.add(filename)

            # These should probably be added as "generated config files"
            # so we can detect if they are missing and use it as another reason to run configure (just as if a build file was modified)
            # But then again if people delete anything else from the "build" folder things might easily stop working
            #if dstPackage.Type == PackageType.Executable:
            #    buildPath = GeneratorCMake._GetPackageBuildDir(generatorConfig, cmakeConfig, dstPackage)
            #    fileSet.add(IOUtil.Join(buildPath,  ".fsl-build/config_Debug.json"))
            #    fileSet.add(IOUtil.Join(buildPath,  ".fsl-build/config_Release.json"))
        return fileSet


    @staticmethod
    def GenerateGeneratorBuildConfigReport(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig, cmakeBuildPackageDir: str,
                                           topLevelPackage: Package, configVariantOptions: List[str]) -> PackageGeneratorConfigReport:
        if topLevelPackage.Type != PackageType.TopLevel:
            raise Exception("Package is not a top level package")


        configReport = GeneratorCMake._GenerateConfigReport(log, generatorConfig.ToolConfig, generatorConfig.PlatformName,
                                                            cmakeConfig, cmakeBuildPackageDir, topLevelPackage)
        variableReport = GeneratorCMake._GenerateConfigVariableReport(log, topLevelPackage, configVariantOptions)

        generatedFileSet = GeneratorCMake._GeneratedFileSet(log, generatorConfig, cmakeConfig, cmakeBuildPackageDir, topLevelPackage)
        # Master build report
        isMasterBuild = True
        masterBuildReport = GeneratorCMake._TryGenerateBuildReport(log, generatorConfig, cmakeConfig, topLevelPackage, isMasterBuild)
        masterBuildVariableReport = GeneratorCMake._GenerateVariableReport(log, topLevelPackage, configVariantOptions, isMasterBuild)

        #PackageGeneratorConfigReport
        return PackageGeneratorConfigReport(configReport, variableReport, generatedFileSet, True, masterBuildReport, masterBuildVariableReport)


    @staticmethod
    def TryGetBuildExecutableInfo(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig, package: Package,
                                  generatorReport: PackageGeneratorReport, variantSettingsDict: Dict[str, str],
                                  configVariantOptions: List[str]) -> Optional[PackageGeneratorBuildExecutableInfo]:
        if package.Type != PackageType.Executable:
            return None

        variableReport = generatorReport.VariableReport
        executableReport = generatorReport.ExecutableReport
        if executableReport is None:
            return None
        if package.AbsolutePath is None:
            raise Exception("Invalid package: {0}".format(package.NameInfo.FullName))

        packageBuildPathFormatRoot = GeneratorCMake._GetPackageBuildDir(generatorConfig, cmakeConfig, package)
        packageBuildPathFormat = IOUtil.Join(packageBuildPathFormatRoot, ".fsl-build/config_${{{0}}}.json".format(LocalMagicBuildVariants.CMakeBuildConfig))

        configurationFilePath = ReportVariableFormatter.Format(packageBuildPathFormat, variableReport, variantSettingsDict,
                                                               executableReport.EnvironmentVariableResolveMethod)

        fileRunPath = ReportVariableFormatter.Format(packageBuildPathFormatRoot, variableReport, variantSettingsDict,
                                                     executableReport.EnvironmentVariableResolveMethod)

        configurationFileDict = GeneratorCMake._TryLoadConfigJson(configurationFilePath)
        buildExePath = IOUtil.NormalizePath(configurationFileDict["EXE_PATH"])
        buildExeCwdPath = fileRunPath
        return PackageGeneratorBuildExecutableInfo(buildExePath, buildExeCwdPath)

    @staticmethod
    def _TryLoadConfigJson(configFile: str) -> Dict[str, str]:
        strConfigJson = IOUtil.ReadFile(configFile)
        jsonDict = json.loads(strConfigJson)
        if not isinstance(jsonDict, dict):
            raise Exception("Incorrect configuration json file: '{0}'".format(configFile))
        finalDict = {} # type: Dict[str,str]
        for key, value in jsonDict.items():
            if not isinstance(key, str) or not isinstance(value, str):
                raise Exception("Incorrect configuration json file: '{0}'. Json decode failed".format(configFile))
            finalDict[key] = value
        return finalDict
