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
from typing import Union
import json
from FslBuildGen import IOUtil
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.BuildExternal import CMakeTypes
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.Generator import CMakeGeneratorUtil
from FslBuildGen.Generator import GitIgnoreHelper
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorCommandReport import GeneratorCommandReport
from FslBuildGen.Generator.Report.GeneratorConfigCommandReport import GeneratorConfigCommandReport
from FslBuildGen.Generator.Report.GeneratorConfigReport import GeneratorConfigReport
from FslBuildGen.Generator.Report.GeneratorExecutableReport import GeneratorExecutableReport
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

def GetVCBuildConfigurationName(entry: List[str]) -> str:
    return "-".join(entry)

class LocalMagicBuildVariants(object):
    CMakeBuildConfig = "FSL_GENERATOR_CMAKE_BUILD_CONFIG"
    GeneratorExeFileExtension = "FSLGEN_GENERATOR_EXE_FILE_EXTENSION"

# Status
# - Variants are not handled
# - Install target does not work due to the way external libs are handled :(
# - Version tags and handling?
# - All apps must be options
# - All source install must be a option
# - Ensure that the 'lib' depenendencies get set correctly (currently everything is public)
# - Root CMakeList should work with 'Extended' repos
# - Need to handle FEATURES as options
# - cmake projects currently only handle one platform at a time. A final version should contain support for all platforms.
# - Using the 'root' CMakeLists.txt is kind of a 'work around' to allow us to re-use libraries
#   It would have been better to have a unique build file for each package with its own 'build' dir
#   However that would be more complex to implement and might make it impossible to have 'all'
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

class ProjectContextCacheRecord(object):
    def __init__(self) -> None:
        super().__init__()

class GeneratorCMake(GeneratorBase):
    def __init__(self, config: Config, packages: List[Package], platformName: str, templateName: str, overrideTemplateName: Optional[str]) -> None:
        super().__init__()

        self.__DisableWrite = config.DisableWrite

        # Create a quick lookup table
        toolProjectContextsDict = {projectContext.ProjectId : projectContext for projectContext in config.ToolConfig.ProjectInfo.Contexts}

        strTemplatePath = templateName
        extTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Ext", False, overrideTemplateName)
        libTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Lib", False, overrideTemplateName)
        exeTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Exe", False, overrideTemplateName)
        rootTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Root", False, overrideTemplateName)

        useExtendedProjectHack = True
        for package in packages:
            if package.Type == PackageType.ExternalLibrary or package.Type == PackageType.HeaderLibrary:
                self.__GenerateCMakeFile(config, package, platformName, extTemplate, toolProjectContextsDict, useExtendedProjectHack)
            elif package.Type == PackageType.Library:
                self.__GenerateCMakeFile(config, package, platformName, libTemplate, toolProjectContextsDict, useExtendedProjectHack)
            elif package.Type == PackageType.Executable:
                self.__GenerateCMakeFile(config, package, platformName, exeTemplate, toolProjectContextsDict, useExtendedProjectHack)
            elif package.Type == PackageType.TopLevel:
                self.__GenerateRootCMakeFile(config, package, platformName, rootTemplate, toolProjectContextsDict, useExtendedProjectHack)

    def __GenerateCMakeFile(self, config: Config, package: Package, platformName: str, template: CMakeGeneratorUtil.CodeTemplateCMake,
                            toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext],
                            useExtendedProjectHack: bool) -> None:
        if package.Type == PackageType.TopLevel:
            raise Exception("Usage error")
        #if package.IsVirtual:
        #    return
        if package.AbsolutePath is None or package.ResolvedBuildPath is None:
            raise Exception("Invalid package")

        packageName = CMakeGeneratorUtil.GetPackageName(package)

        aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName, package.ProjectContext.ProjectName)

        targetIncludeDirectories = CMakeGeneratorUtil.BuildTargetIncludeDirectories(config, package, template.PackageTargetIncludeDirectories,
                                                                                    template.PackageTargetIncludeDirEntry, template.PackageTargetIncludeDirVirtualEntry,
                                                                                    CMakeGeneratorUtil.CMakePathType.LocalRelative)

        publicIncludeFiles = self.__Join(package.ResolvedBuildPublicIncludeFiles)
        privateIncludeFiles = self.__Join(package.ResolvedBuildPrivateIncludeFiles)
        includeFiles = self.__Join(package.ResolvedBuildAllIncludeFiles)
        sourceFiles = self.__Join(package.ResolvedBuildSourceFiles)

        linkLibrariesDirectDependencies = CMakeGeneratorUtil.BuildTargetLinkLibrariesForDirectDependencies(config, package,
                                                                                                           template.PackageDependencyTargetLinkLibraries,
                                                                                                           template.PackageDependencyFindPackage)
        directDefinitions = CMakeGeneratorUtil.BuildDirectDefinitions(config, package, template.PackageDependencyTargetCompileDefinitions)
        findDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(config, package, template.PackageDependencyFindPackage)
        installInstructions = CMakeGeneratorUtil.BuildInstallInstructions(config, package, template.PackageInstall,
                                                                          template.PackageInstallTargets,
                                                                          template.PackageInstallHeaders,
                                                                          template.PackageInstallContent,
                                                                          template.PackageInstallDLL,
                                                                          template.PackageInstallAppInfo)
        targetCompileFeatures = CMakeGeneratorUtil.BuildCompileFeatures(config, package, template.SnippetTargetCompileFeaturesDefault,
                                                                        template.SnippetTargetCompileFeaturesInterface)
        targetCompileOptions = CMakeGeneratorUtil.BuildCompileOptions(config, package, template.SnippetTargetCompileOptionsDefault)
        targetFileCopy = CMakeGeneratorUtil.BuildFileCopy(config, package, template.PackageTargetCopyFile, template.PackageTargetCopyFilePath)

        cacheVariants = CMakeGeneratorUtil.GetCacheVariants(package, template.SnippetCacheVariant)

        contentInBinaryDirectory = True

        packageContentBuilder = CMakeGeneratorUtil.GetContentBuilder(config, package, platformName, template.PackageContentBuilder, contentInBinaryDirectory)
        #packageContentBuilderOutputFiles = CMakeGeneratorUtil.GetContentBuilderOutputFiles(config, package, contentInBinaryDirectory)

        packageContentSection = CMakeGeneratorUtil.GetContentSection(config, package, platformName, template.PackageContent, template.PackageContentFile, contentInBinaryDirectory)
        #packageContentSectionOutputFiles = CMakeGeneratorUtil.GetContentSectionOutputFiles(config, package, contentInBinaryDirectory)

        packageContentDep = CMakeGeneratorUtil.GetContentDepSection(config, package, platformName, template.PackageContentDep, contentInBinaryDirectory)
        packageContentDepOutputFiles = CMakeGeneratorUtil.GetContentDepOutputFile(config, package, contentInBinaryDirectory)

        packageCompilerSpecificFileDependencies = CMakeGeneratorUtil.CompilerSpecificFileDependencies(config, package,
                                                                                                      template.PackageCompilerConditional,
                                                                                                      template.PackageTargetSourceFiles,
                                                                                                      template.PackageCompilerFileDict)

        packageVariantSettings = CMakeGeneratorUtil.GetVariantSettings(config, package, template.PackageVariantSettings,
                                                                       template.PackageDependencyTargetCompileDefinitions,
                                                                       template.PackageDependencyTargetLinkLibraries)

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

        toolProjectContext = toolProjectContextsDict[package.ProjectContext.ProjectId]
        sectionDefinePathEnvAsVariables = CMakeGeneratorUtil.CreateDefineRootDirectoryEnvironmentAsVariables(config.ToolConfig, toolProjectContext,
                                                                                                             useExtendedProjectHack, template.PathEnvToVariable)
        buildCMakeFile = self.__CommonReplace(buildCMakeFile, package.ProjectContext, packageName, aliasPackageName, cacheVariants,
                                              sectionDefinePathEnvAsVariables, config.ToolConfig.CMakeConfiguration.MinimumVersion,
                                              template)

        self.__SaveFile(GeneratorCMake._GetPackageBuildFileName(package), buildCMakeFile)
        GitIgnoreHelper.SafeAddEntry(self.GitIgnoreDict, package, "CMakeLists.txt")


    def __GenerateRootCMakeFile(self, config: Config, package: Package, platformName: str, template: CMakeGeneratorUtil.CodeTemplateCMake,
                                toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext],
                                useExtendedProjectHack: bool) -> None:
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

            addSubDirectoriesDirectDependencies = self.__BuildAddSubDirectoriesForTopLevelDirectDependencies(config, package, projectAbsolutePath, template)
            #findExternalDependencies = self.__BuildFindExternalDependencies(config, package, template)

            aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName, projectContext.ProjectName)

            findDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(config, package, template.PackageDependencyFindPackage)

            allPackageNames = CMakeGeneratorUtil.GetAllPackageNames(package, projectContext if not useExtendedProjectHack else None)

            cacheVariants = CMakeGeneratorUtil.GetCacheVariants(package, template.SnippetCacheVariant)

            extendedProjectParent = CMakeGeneratorUtil.GetAddExtendedPackageParent(config.ToolConfig, toolProjectContext, template.AddExtendedPackage)

            buildCMakeFile = template.Master
            buildCMakeFile = buildCMakeFile.replace("##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", findDirectExternalDependencies)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##", addSubDirectoriesDirectDependencies)
            buildCMakeFile = buildCMakeFile.replace("##ALL_PACKAGE_NAMES##", "\n  ".join(allPackageNames))
            buildCMakeFile = buildCMakeFile.replace("##EXTENDED_PROJECT_PARENT##", extendedProjectParent)

            sectionDefinePathEnvAsVariables = CMakeGeneratorUtil.CreateDefineRootDirectoryEnvironmentAsVariables(config.ToolConfig, toolProjectContext,
                                                                                                                    useExtendedProjectHack, template.PathEnvToVariable)
            buildCMakeFile = self.__CommonReplace(buildCMakeFile, projectContext, packageName, aliasPackageName, cacheVariants,
                                                  sectionDefinePathEnvAsVariables, config.ToolConfig.CMakeConfiguration.MinimumVersion,
                                                  template)

            dstFilename = GeneratorCMake._GetProjectPackageBuildFileName(toolProjectContext.Location)
            self.__SaveFile(dstFilename, buildCMakeFile)


    def __CommonReplace(self, content: str, projectContext: Union[PackageProjectContext,ToolConfigProjectContext], packageName: str, aliasPackageName: str,
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

    @staticmethod
    def _GetProjectPackageBuildFileName(location: ResolvedPath) -> str:
        return GeneratorCMake._GetBuildFileName(location.ResolvedPath)

    @staticmethod
    def _GetPackageBuildFileName(package: Package) -> str:
        if package.AbsolutePath is None:
            raise Exception("Invalid package")
        return GeneratorCMake._GetBuildFileName(package.AbsolutePath)



    def __Join(self, srcList: Optional[List[str]]) -> str:
        if srcList is None or len(srcList) <= 0:
            return ''
        return "\n  " + "\n  ".join(srcList)


    def __BuildAddSubDirectoriesForTopLevelDirectDependencies(self, config: Config, package: Package, projectAbsolutePath: str,
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
                    path = CMakeGeneratorUtil.GetRelativePath(config, projectAbsolutePath, entry.AbsolutePath)
                    content += template.PackageDependencyAddSubdirectories.replace("##PACKAGE_PATH##", path)
        return content


    #def __BuildFindExternalDependencies(self, config, package, template):

    #    dictExternal = {}
    #    for subPackage in package.ResolvedBuildOrder:
    #        for externalDep in subPackage.ResolvedDirectExternalDependencies:
    #            dictExternal[externalDep.Name] = externalDep

    #    externalDeps = []
    #    for externalDep in dictExternal.values():
    #        if externalDep.Type == ExternalDependencyType.Find:
    #            externalDeps.append(externalDep)

    #    if len(externalDeps) <= 0:
    #        return ""


    #    snippet = template.PackageDependencyFindPackage
    #    content = ""
    #    for externalDep in externalDeps:
    #        findParams = "%s REQUIRED" % (externalDep.Name)
    #        contentEntry = snippet
    #        contentEntry = contentEntry.replace("##FIND_PARAMS##", findParams)
    #        content += contentEntry
    #    return content

    #@staticmethod
    #def TryGeneratePreBuildReport(log: Log, generatorName: str, package: Package) -> Optional[GeneratorPreBuildReport]:
    #    if package.Type != PackageType.TopLevel:
    #        return None

    #    # preBuildCommand = ['cmake', '-G', 'Visual Studio 15 2017 Win64'] + buildConfig.BuildArgs

    #    buildCommandArguments = ['-G', 'Visual Studio 15 2017 Win64']  # type: List[str]

    #    buildCommand = "cmake"
    #    buildCommandReport = GeneratorCommandReport(True, buildCommand, buildCommandArguments, [])
    #    return GeneratorPreBuildReport(buildCommandReport)

    @staticmethod
    def _TryGenerateBuildReport(log: Log, generatorConfig: GeneratorConfig, generatorName: str, cmakeConfig: GeneratorCMakeConfig,
                                package: Package, isMasterBuild: bool) -> Optional[GeneratorBuildReport]:
        if package.IsVirtual and not isMasterBuild:
            return None

        # preBuildCommand = ['cmake', '-G', 'Visual Studio 15 2017 Win64'] + buildConfig.BuildArgs
        # buildCommand = ['cmake', '--build', '.', '--config', 'Debug'] + buildConfig.BuildArgs

        buildCommandArguments = ['--build', '.']  # type: List[str]
        buildCommandNativeArguments = [] # type: List[str]

        # Configuration (Debug, Release) for the configurations that support build time configuration switching
        if CMakeTypes.GetGeneratorMultiConfigCapabilities(cmakeConfig.GeneratorName) != CMakeTypes.CMakeGeneratorMultiConfigCapability.No:
            buildCommandArguments.append("--config")
            buildCommandArguments.append("${{{0}}}".format(LocalMagicBuildVariants.CMakeBuildConfig))

        # Do a fallback solution for a few generators that we know how work
        nativeArguments = CMakeTypes.GetNativeBuildThreadArguments(cmakeConfig.GeneratorName, generatorConfig.NumBuildThreads)
        if len(nativeArguments) > 0:
            buildCommandNativeArguments += nativeArguments
        elif cmakeConfig.CMakeVersion.Major >= 3 and cmakeConfig.CMakeVersion.Minor >= 12:
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
    def _TryGenerateExecutableReport(log: Log, generatorName: str, package: Package) -> Optional[GeneratorExecutableReport]:
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
    def _GenerateVariableReport(log: Log, generatorName: str, package: Package, configVariantOptions: List[str],
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
        variableReport.Add(LocalMagicBuildVariants.CMakeBuildConfig, ['Debug', 'Release'], ToolAddedVariant.CONFIG)

        # make builds default to release
        variableReport.SetDefaultOption(ToolAddedVariant.CONFIG, ToolAddedVariantConfigOption.Release)
        return variableReport

    @staticmethod
    def TryGenerateGeneratorPackageReport(log: Log, generatorConfig: GeneratorConfig, generatorName: str, cmakeConfig: GeneratorCMakeConfig,
                                          package: Package, configVariantOptions: List[str]) -> Optional[PackageGeneratorReport]:
        if package.IsVirtual and package.Type != PackageType.HeaderLibrary and package.Type != PackageType.TopLevel:
            return None

        isMasterBuild = False
        #preBuildReport = GeneratorCMake.TryGeneratePreBuildReport(log, generatorName, package)
        buildReport = GeneratorCMake._TryGenerateBuildReport(log, generatorConfig, generatorName, cmakeConfig, package, isMasterBuild)
        executableReport = GeneratorCMake._TryGenerateExecutableReport(log, generatorName, package)
        variableReport = GeneratorCMake._GenerateVariableReport(log, generatorName, package, configVariantOptions, isMasterBuild)
        return PackageGeneratorReport(buildReport, executableReport, variableReport)

    @staticmethod
    def _GenerateConfigReport(log: Log, toolConfig: ToolConfig, generatorName: str, platformName: str,
                              cmakeConfig: GeneratorCMakeConfig,
                              topLevelPackage: Package) -> GeneratorConfigReport:
        if topLevelPackage.Type != PackageType.TopLevel:
            raise Exception("Package is not a top level package")

        # Top level package handling
        buildCommandArguments = []  # type: List[str]

        if cmakeConfig.InstallPrefix is not None:
             buildCommandArguments.append("-DCMAKE_INSTALL_PREFIX={0}".format(cmakeConfig.InstallPrefix))

        # Configuration (Debug, Release) for the configurations that support configure time configuration
        if CMakeTypes.GetGeneratorMultiConfigCapabilities(cmakeConfig.GeneratorName) != CMakeTypes.CMakeGeneratorMultiConfigCapability.Yes:
            buildCommandArguments.append("-DCMAKE_BUILD_TYPE=${{{0}}}".format(LocalMagicBuildVariants.CMakeBuildConfig))

        # Normal variants
        for normalVariant in topLevelPackage.ResolvedNormalVariantNameList:
            buildCommandArguments.append("-D{0}=${{{0}}}".format(normalVariant))

        # Set the generator name
        buildCommandArguments.append("-G")
        buildCommandArguments.append(cmakeConfig.GeneratorName)

        # Add additional arguments
        additionalGeneratorArguments = CMakeTypes.DetermineGeneratorArguments(cmakeConfig.GeneratorName, platformName)
        if len(additionalGeneratorArguments) > 0:
            buildCommandArguments += additionalGeneratorArguments

        # Add the path to the root cmake file
        topToolProjectContext = toolConfig.ProjectInfo.TopProjectContext
        projectAbsolutePath = topToolProjectContext.Location.ResolvedPath
        buildCommandArguments.append(projectAbsolutePath)

        # Set the build dir
        configCWD = cmakeConfig.BuildDir

        # FIX: See how the visual studio generator handles debug/release builds

        configCommand = "cmake"
        configCommandReport = GeneratorConfigCommandReport(configCommand, buildCommandArguments, configCWD)

        return GeneratorConfigReport(configCommandReport)

    @staticmethod
    def _GenerateConfigVariableReport(log: Log, generatorName: str, topLevelPackage: Package, configVariantOptions: List[str]) -> GeneratorVariableReport:
        variableReport = GeneratorVariableReport(log, configVariantOptions=configVariantOptions)
        # Add all the package variants
        for variantEntry in topLevelPackage.ResolvedAllVariantDict.values():
            variantEntryOptions = [option.Name for option in variantEntry.Options]
            variableReport.Add(variantEntry.Name, variantEntryOptions)

        # CMake names for debug and release building
        variableReport.Add(LocalMagicBuildVariants.CMakeBuildConfig, ['Debug', 'Release'], ToolAddedVariant.CONFIG)

        # make builds default to release
        variableReport.SetDefaultOption(ToolAddedVariant.CONFIG, ToolAddedVariantConfigOption.Release)
        return variableReport


    @staticmethod
    def _GeneratedFileSet(log: Log, generatorConfig: GeneratorConfig, cmakeConfig: GeneratorCMakeConfig, topLevelPackage: Package) -> Set[str]:
        """
        Generate a list of all build files that will be generated for the given topLevelPackage by this generator
        """
        fileSet = set() # type: Set[str]

        for toolProjectContext in generatorConfig.ToolConfig.ProjectInfo.Contexts:
            filename = GeneratorCMake._GetProjectPackageBuildFileName(toolProjectContext.Location)
            if IOUtil.IsFile(filename):
                fileSet.add(filename)

        for depPackage in topLevelPackage.ResolvedAllDependencies:
            filename = GeneratorCMake._GetPackageBuildFileName(depPackage.Package)
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
    def GenerateGeneratorBuildConfigReport(log: Log, generatorConfig: GeneratorConfig, generatorName: str,
                                           cmakeConfig: GeneratorCMakeConfig, topLevelPackage: Package,
                                           configVariantOptions: List[str]) -> PackageGeneratorConfigReport:
        if topLevelPackage.Type != PackageType.TopLevel:
            raise Exception("Package is not a top level package")

        configReport = GeneratorCMake._GenerateConfigReport(log, generatorConfig.ToolConfig, generatorName, generatorConfig.PlatformName,
                                                            cmakeConfig, topLevelPackage)
        variableReport = GeneratorCMake._GenerateConfigVariableReport(log, generatorName, topLevelPackage, configVariantOptions)

        generatedFileSet = GeneratorCMake._GeneratedFileSet(log, generatorConfig, cmakeConfig, topLevelPackage)
        # Master build report
        isMasterBuild = True
        masterBuildReport = GeneratorCMake._TryGenerateBuildReport(log, generatorConfig, generatorName, cmakeConfig, topLevelPackage, isMasterBuild)
        masterBuildVariableReport = GeneratorCMake._GenerateVariableReport(log, generatorName, topLevelPackage, configVariantOptions, isMasterBuild)

        #PackageGeneratorConfigReport
        return PackageGeneratorConfigReport(configReport, variableReport, generatedFileSet, True, masterBuildReport, masterBuildVariableReport)


    @staticmethod
    def TryGetBuildExecutableInfo(log: Log, generatorConfig: GeneratorConfig, generatorName: str,
                                  cmakeConfig: GeneratorCMakeConfig, package: Package,
                                  generatorReport: PackageGeneratorReport, variantSettingsDict: Dict[str, str],
                                  configVariantOptions: List[str]) -> Optional[PackageGeneratorBuildExecutableInfo]:
        if package.Type != PackageType.Executable:
            return None

        variableReport = generatorReport.VariableReport
        executableReport = generatorReport.ExecutableReport
        if executableReport is None:
            return None

        packageBuildPathFormat = GeneratorCMake._GetPackageBuildDir(generatorConfig, cmakeConfig, package)
        packageBuildPathFormat = IOUtil.Join(packageBuildPathFormat, ".fsl-build/config_${{{0}}}.json".format(LocalMagicBuildVariants.CMakeBuildConfig))

        configurationFilePath = ReportVariableFormatter.Format(packageBuildPathFormat, variableReport, variantSettingsDict,
                                                               executableReport.EnvironmentVariableResolveMethod)

        configurationFileDict = GeneratorCMake._TryLoadConfigJson(configurationFilePath)
        buildExePath = IOUtil.NormalizePath(configurationFileDict["EXE_PATH"])
        return PackageGeneratorBuildExecutableInfo(buildExePath)

    @staticmethod
    def _TryLoadConfigJson(configFile: str) -> Dict[str,str]:
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

