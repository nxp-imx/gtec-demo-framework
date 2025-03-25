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

from typing import cast
from typing import Callable
from typing import List
from typing import Optional
import os
from FslBuildGen import IOUtil
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.State.PackageRecipeUtil import PackageRecipeUtil
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import BuildRecipePipelineCommand
from FslBuildGen.DataTypes import BuildRecipeValidateCommand
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator import AndroidGeneratorUtil
from FslBuildGen.Generator import CMakeGeneratorUtil
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorUtil import GeneratorUtil
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorCommandReport import GeneratorCommandReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.ExternalVariantConstraints import ExternalVariantConstraints
from FslBuildGen.LibUtil import LibUtil
from FslBuildGen.Log import Log
#from FslBuildGen.Exceptions import *
from FslBuildGen.Packages.Package import Package
from FslBuildGen.SharedGeneration import AndroidABIOption
from FslBuildGen.SharedGeneration import GEN_MAGIC_VARIANT_ANDROID_ABI
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import ToolEnvironmentVariableName
from FslBuildGen.Template.TemplateFileRecordManager import TemplateFileRecordManager
from FslBuildGen.Template.TemplateFileProcessor import TemplateFileProcessor
from FslBuildGen.Vars.Variable import Variable
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddLib

#from FslBuildGen.PackageGeneratorReport import *


class LocalMagicBuildVariants(object):
    GradleBuildConfig = "FSL_GENERATOR_GRADLE_BUILD_CONFIG"


def GetVCBuildConfigurationName(entry: List[str]) -> str:
    return "-".join(entry)

# Status
# - External libs with special debug libraries are not handled
# - FslBuild things dont work
# Note:
# - The package headers might not completely follow the FslBuildGen standards
#   Meaning some packages might have access to more than their Fsl.gen file
#   allows them access to

class AndroidGradleCMakeSnippets(object):
    def __init__(self, templatePath: str) -> None:
        super().__init__()
        fileEnvironmentBasedRootVariable = IOUtil.Join(templatePath, "CMakeAndroid/DefineEnvironmentBasedRootVariable.txt")
        self.DefineEnvironmentBasedRootVariable = IOUtil.ReadFile(fileEnvironmentBasedRootVariable)


class AndroidCMakeLibRecord(object):
    def __init__(self, name: str, path: str) -> None:
        super().__init__()
        self.Name = name
        self.Path = path

class AndroidCMakeLib(object):
    def __init__(self, path: str, staticLibs: List[AndroidCMakeLibRecord]) -> None:
        super().__init__()
        self.Path = path
        self.StaticLibs = staticLibs


# This generator does not work if there are multiple source roots :(
class GeneratorAndroidGradleCMake(GeneratorBase):
    def __init__(self, config: Config, packages: List[Package], platformName: str, androidABIList: List[str],
                 externalVariantConstraints: ExternalVariantConstraints) -> None:
        super().__init__()

        cmakeMinimumVersion = config.ToolConfig.CMakeConfiguration.MinimumVersion

        if config.SDKPathAndroidProjectDir is None:
            raise EnvironmentError("Android environment variable {0} not defined".format(ToolEnvironmentVariableName.FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR))

        strAppTemplatePath = "AndroidGradleCMake"
        templateFilePath = IOUtil.Join(config.SDKConfigTemplatePath, strAppTemplatePath)
        templateFileRecordManager = TemplateFileRecordManager(templateFilePath)
        templateFileProcessor = TemplateFileProcessor(config, platformName)

        sdkConfigTemplatePath = config.SDKConfigTemplatePath

        strTemplatePath = IOUtil.Join(strAppTemplatePath, "CMake")
        extTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Ext", False, None)
        libTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Lib", False, None)
        exeTemplate = CMakeGeneratorUtil.CodeTemplateCMake(sdkConfigTemplatePath, strTemplatePath, "Exe", False, None)

        templatePath = IOUtil.Join(config.SDKConfigTemplatePath, strAppTemplatePath)
        exeFileList = self.__ParseExeFileList(IOUtil.Join(templatePath, "ExeFiles.txt"))

        localSnippets = AndroidGradleCMakeSnippets(templatePath)

        cmakePackageRootVariables = self.__GenerateCmakePackageRootVariables(config, localSnippets)

        totalExeCount = 0
        for mainPackage in packages:
            if mainPackage.Type == PackageType.Executable:
                totalExeCount = totalExeCount + 1
                appPackageTemplateInfo = AndroidGeneratorUtil.AppPackageTemplateInfo(mainPackage)
                androidProjectDir = IOUtil.Join(config.SDKPathAndroidProjectDir, appPackageTemplateInfo.ProjectPathName)
                androidProjectCMakeDir = IOUtil.Join(androidProjectDir, ".fsl")

                for package in mainPackage.ResolvedBuildOrder:
                    if package.ResolvedPlatformSupported:
                        if package.Type == PackageType.ExternalLibrary or package.Type == PackageType.HeaderLibrary:
                            self.__GenerateCMakeFile(config, package, platformName, extTemplate, androidProjectDir, androidProjectCMakeDir, cmakeMinimumVersion)
                        elif package.Type == PackageType.Library:
                            self.__GenerateCMakeFile(config, package, platformName, libTemplate, androidProjectDir, androidProjectCMakeDir, cmakeMinimumVersion)
                        elif package.Type == PackageType.Executable:
                            self.__GenerateExecutable(config, package, platformName, exeTemplate, templateFileRecordManager, templateFileProcessor,
                                                      appPackageTemplateInfo, androidProjectDir, androidProjectCMakeDir,
                                                      exeFileList, androidABIList, cmakePackageRootVariables, cmakeMinimumVersion,
                                                      externalVariantConstraints)

        # For now we only support doing 'exe' builds using full source for everything (like the old builder)
        if totalExeCount <= 0 and not config.IsTestMode:
            config.DoPrint("No executables provided, nothing to build.")


    # TODO: Remove
    def GetAndroidProjectDir(self, config: Config,
                             package: Package,
                             appPackageTemplateInfo: Optional[AndroidGeneratorUtil.AppPackageTemplateInfo]=None) -> str:
        """ Get the android project dir of the package, this is the dir that the gradle build reside in """
        appPackageTemplateInfo = AndroidGeneratorUtil.AppPackageTemplateInfo(package) if appPackageTemplateInfo is None else appPackageTemplateInfo
        return IOUtil.Join(config.SDKPathAndroidProjectDir, appPackageTemplateInfo.ProjectPathName)


    def __GenerateCmakePackageRootVariables(self, config: Config, localSnippets: AndroidGradleCMakeSnippets) -> str:
        cmakePackageRootVariables = ''
        rootDirectories = config.ToolConfig.RootDirectories

        for rootDir in rootDirectories:
            environmentVariableName = rootDir.TryGetEnvironmentVariableName()
            if environmentVariableName is not None:
                content = localSnippets.DefineEnvironmentBasedRootVariable
                content = content.replace("##ROOT_ENVIRONEMNT_VARIABLE_NAME##", environmentVariableName)
                cmakePackageRootVariables += '\n' + content
            else:
                raise Exception("Unsupported package root type")
        return cmakePackageRootVariables


    def __GenerateCMakeFile(self, config: Config,
                            package: Package,
                            platformName: str,
                            template: CMakeGeneratorUtil.CodeTemplateCMake,
                            androidProjectDir: str,
                            androidProjectCMakeDir: str,
                            cmakeMinimumVersion: CMakeVersion) -> None:

        toolConfig = config.ToolConfig

        pathType = CMakeGeneratorUtil.CMakePathType.Relative

        packageName = CMakeGeneratorUtil.GetPackageName(package)

        # this ignore is a workaround allowing us to build using the same info as the old Android builder
        ignoreLibs = ["android_native_app_glue"]

        aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName, package.ProjectContext.ProjectName)
        targetIncludeDirectories = CMakeGeneratorUtil.BuildTargetIncludeDirectories(toolConfig, package, template.PackageTargetIncludeDirectories,
                                                                                    template.PackageTargetIncludeDirEntry, template.PackageTargetIncludeDirVirtualEntry, pathType)
        targetIncludeDirectories = targetIncludeDirectories.replace(Variable.RecipeVariant, "${ANDROID_ABI}")

        publicIncludeFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildPublicIncludeFiles)
        privateIncludeFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildPrivateIncludeFiles)
        includeFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildAllIncludeFiles)
        sourceFiles = CMakeGeneratorUtil.ExpandPathAndJoin(toolConfig, package, package.ResolvedBuildSourceFiles)
        linkLibrariesDirectDependencies = CMakeGeneratorUtil.BuildTargetLinkLibrariesForDirectDependencies(config, package,
                                                                                                           template.PackageDependencyTargetLinkLibraries,
                                                                                                           template.PackageDependencyFindPackageInternal,
                                                                                                           ignoreLibs)
        linkLibrariesDirectDependencies = linkLibrariesDirectDependencies.replace(Variable.RecipeVariant, "${ANDROID_ABI}")
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

        buildCMakeFile = template.Master

        cmakeMinimumVersionStr = "{0}.{1}.{2}".format(cmakeMinimumVersion.Major, cmakeMinimumVersion.Minor, cmakeMinimumVersion.Build)

        if package.Type == PackageType.Executable:
            if package.ContentPath is None or package.AbsolutePath is None:
                raise Exception("Invalid package")
            packagePath = CMakeGeneratorUtil.GetSDKBasedPathUsingCMakeVariable(toolConfig, package.AbsolutePath)
            packageContentPath = CMakeGeneratorUtil.GetSDKBasedPathUsingCMakeVariable(toolConfig, package.ContentPath.AbsoluteDirPath)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_PATH##", packagePath)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENT_PATH##", packageContentPath)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_ANDROID_PROJECT_PATH##", androidProjectDir)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_INCLUDE_FILES##", includeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_PUBLIC_INCLUDE_FILES##", publicIncludeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_PRIVATE_INCLUDE_FILES##", privateIncludeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_SOURCE_FILES##", sourceFiles)
        buildCMakeFile = buildCMakeFile.replace("##TARGET_INCLUDE_DIRECTORIES##", targetIncludeDirectories)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##", linkLibrariesDirectDependencies)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##", directDefinitions)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", findDirectExternalDependencies)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##", targetCompileOptions)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##", template.SnippetTargetCompileFeaturesDefault)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_GENERATE_INSTALL_INSTRUCTIONS##", installInstructions)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_NAME!##", packageName.upper())
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_NAME##", packageName)
        buildCMakeFile = buildCMakeFile.replace("##ALIAS_PACKAGE_NAME##", aliasPackageName)
        buildCMakeFile = buildCMakeFile.replace("##PROJECT_NAME##", package.ProjectContext.ProjectName)
        buildCMakeFile = buildCMakeFile.replace("##PROJECT_VERSION##", str(package.ProjectContext.ProjectVersion))
        buildCMakeFile = buildCMakeFile.replace("##CMAKE_MINIMUM_VERSION##", cmakeMinimumVersionStr)

        if not config.DisableWrite:
            # We store all cmake build files in their own dir inside the 'android' exe-project's folder
            # to prevent collision with other more generic CMake builders
            packageCMakeDir = self.__GetPackageCMakeDir(androidProjectCMakeDir, package)
            IOUtil.SafeMakeDirs(packageCMakeDir)
            dstFileCMakeFile = self.__GetPackageCMakeFileName(androidProjectCMakeDir, package)
            IOUtil.WriteFileIfChanged(dstFileCMakeFile, buildCMakeFile)


    def __GetPackageCMakeDir(self, androidProjectCMakeDir: str, package: Package) -> str:
        """ Get the directory that the CMake CMakeLists.txt file reside in for this package """
        return IOUtil.Join(androidProjectCMakeDir, package.Name)


    def __GetPackageCMakeFileName(self, androidProjectCMakeDir: str, package: Package) -> str:
        """ Get the full path of the CMakeLists.txt file for the package """
        packageCMakeDir = self.__GetPackageCMakeDir(androidProjectCMakeDir, package)
        dstFileCMakeFile = IOUtil.Join(packageCMakeDir, "CMakeLists.txt")
        #dstFileCMakeFile = IOUtil.Join(package.AbsolutePath, "CMakeLists.txt")
        return dstFileCMakeFile


    def __GenerateExecutable(self, config: Config,
                             package: Package,
                             platformName: str,
                             template: CMakeGeneratorUtil.CodeTemplateCMake,
                             templateFileRecordManager: TemplateFileRecordManager,
                             templateFileProcessor: TemplateFileProcessor,
                             appPackageTemplateInfo: AndroidGeneratorUtil.AppPackageTemplateInfo,
                             androidProjectDir: str,
                             androidProjectCMakeDir: str,
                             exeFileList: List[str],
                             androidABIList: List[str],
                             cmakePackageRootVariables: str, cmakeMinimumVersion: CMakeVersion,
                             externalVariantConstraints: ExternalVariantConstraints) -> None:
        # copy files that need to be modified
        dstFilenameModifier = self.__GetDstFilenameModifier(config, androidProjectDir, package, appPackageTemplateInfo,
                                                            template, androidProjectCMakeDir, androidABIList,
                                                            templateFileProcessor, cmakePackageRootVariables, cmakeMinimumVersion)

        templateFileProcessor.Process(config, templateFileRecordManager, androidProjectDir, package, externalVariantConstraints, dstFilenameModifier)

        if not config.DisableWrite:
            # mark files as executable
            for entry in exeFileList:
                IOUtil.SetFileExecutable(IOUtil.Join(androidProjectDir, entry))

        self.__GenerateCMakeFile(config, package, platformName, template, androidProjectDir, androidProjectCMakeDir, cmakeMinimumVersion)


    def __GetDstFilenameModifier(self, config: Config,
                                 dstPath: str,
                                 package: Package,
                                 appPackageTemplateInfo: AndroidGeneratorUtil.AppPackageTemplateInfo,
                                 template: CMakeGeneratorUtil.CodeTemplateCMake,
                                 androidProjectCMakeDir: str,
                                 androidABIList: List[str],
                                 templateFileProcessor: TemplateFileProcessor,
                                 cmakePackageRootVariables: str,
                                 cmakeMinimumVersion: CMakeVersion) -> Callable[[str], str]:
        androidHome = AndroidUtil.GetSDKPath()
        androidNDK = AndroidUtil.GetNDKPath()
        ndkVersion  = AndroidUtil.GetNDKVersion()
        androidNDKForProp = self.__ToPropPath(androidNDK)
        androidHomeForProp = self.__ToPropPath(androidHome)

        androidABIList = self.__patchABIList(config, package, androidABIList)
        self.__CheckABI(package, androidABIList)

        packageVariantGradleAndroidABIList = self.__CreateGradleAndroidABIList(androidABIList)

        packageName = CMakeGeneratorUtil.GetPackageName(package)
        cmakePackageExeLib = CMakeGeneratorUtil.GetAliasName(packageName, package.ProjectContext.ProjectName)
        cmakePackageFindDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(config, package, template.PackageDependencyFindPackage)
        cmakePackageDirectDependenciesAndSubDirectories = self.__BuildCMakeAddSubDirectoriesForDirectDependencies(config, package, template, androidProjectCMakeDir)

        addCMakeLibsList = self.__AddCMakeLibs(package)

        cmakeMinimumVersionStr = "{0}.{1}.{2}".format(cmakeMinimumVersion.Major, cmakeMinimumVersion.Minor, cmakeMinimumVersion.Build)

        thirdPartyLibsList = []  # type: List[str]
        for entry in addCMakeLibsList:
            for staticLib in entry.StaticLibs:
                thirdPartyLibsList.append(staticLib.Name)

        thirdPartyAddLibs = ""
        for entry in addCMakeLibsList:
            for staticLib in entry.StaticLibs:
                content = template.AddImportedLibrary
                content = content.replace("##LIBRARY_NAME##", staticLib.Name)
                content = content.replace("##LIBRARY_TYPE##", "STATIC")
                content = content.replace("##LIBRARY_PATH##", staticLib.Path)
                thirdPartyAddLibs += "\n" + content

        # thirdPartyLibs is a space seperated list of third party lib names
        thirdPartyLibs = " ".join(thirdPartyLibsList)

        templateFileProcessor.Environment.Set("##CMAKE_THIRD_PARTY_LIBS##", thirdPartyLibs)
        templateFileProcessor.Environment.Set("##CMAKE_THIRD_PARTY_ADD_LIBS##", thirdPartyAddLibs)
        templateFileProcessor.Environment.Set("##CMAKE_PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##", cmakePackageDirectDependenciesAndSubDirectories)
        templateFileProcessor.Environment.Set("##CMAKE_PACKAGE_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", cmakePackageFindDirectExternalDependencies)
        templateFileProcessor.Environment.Set("##CMAKE_PACKAGE_EXE_LIB##", cmakePackageExeLib)
        templateFileProcessor.Environment.Set("##CMAKE_DEFINE_PACKAGE_ROOT_VARIABLES##", cmakePackageRootVariables)

        templateFileProcessor.Environment.Set("##ENV_ANDROID_HOME_FOR_PROP##", androidHomeForProp)
        templateFileProcessor.Environment.Set("##ENV_ANDROID_NDK_FOR_PROP##", androidNDKForProp)
        templateFileProcessor.Environment.Set("##FSL_PACKAGE_GLES_VERSION##", appPackageTemplateInfo.MinGLESVersion)
        templateFileProcessor.Environment.Set("##FSL_PACKAGE_MIN_ANDROID_SDK_VERSION##", appPackageTemplateInfo.MinSDKVersion.VersionString)
        templateFileProcessor.Environment.Set("##FSL_PACKAGE_TARGET_ANDROID_SDK_VERSION##", appPackageTemplateInfo.TargetSDKVersion.VersionString)
        templateFileProcessor.Environment.Set("##FSL_NDK_VERSION##", ndkVersion)
        templateFileProcessor.Environment.Set("##PACKAGE_APP_PLATFORM##", appPackageTemplateInfo.MinSDKVersion.AppPlatform)
        templateFileProcessor.Environment.Set("##PACKAGE_VARIANT_ANDROID_ABIS##", packageVariantGradleAndroidABIList)
        templateFileProcessor.Environment.Set("##PREFIXED_PROJECT_NAME##", appPackageTemplateInfo.PrefixedProjectName)
        templateFileProcessor.Environment.Set("##PREFIXED_PROJECT_NAME_L##", appPackageTemplateInfo.PrefixedProjectName.lower())
        templateFileProcessor.Environment.Set("##CMAKE_MINIMUM_VERSION##", cmakeMinimumVersionStr)

        return appPackageTemplateInfo.UpdateFileName


    def __BuildCMakeAddSubDirectoriesForDirectDependencies(self, config: Config,
                                                           package: Package,
                                                           template: CMakeGeneratorUtil.CodeTemplateCMake,
                                                           androidProjectCMakeDir: str) -> str:
        if len(package.ResolvedBuildOrder) <= 0:
            return ""
        content = ""
        snippet = template.PackageDependencyAddSubdirectories
        for depPackage in package.ResolvedBuildOrder:
            if depPackage.Type != PackageType.ToolRecipe:
                #sdkPackagePath = CMakeGeneratorUtil.GetSDKBasedPathUsingCMakeVariable(config, depPackage.AbsolutePath)
                sdkPackagePath = self.__GetPackageCMakeDir(androidProjectCMakeDir, depPackage)
                # We take advantage of the fact that the full package name is unique
                cmakeBinPackagePath = "${{CMAKE_BINARY_DIR}}/{0}".format(depPackage.Name)
                path = "{0} {1}".format(sdkPackagePath, cmakeBinPackagePath)
                content += snippet.replace("##PACKAGE_PATH##", path)
        return content


    def __ToPropPath(self, path: str) -> str:
        return path


    def __ParseExeFileList(self, path: str) -> List[str]:
        lines = IOUtil.ReadFile(path).split('\n')
        result = []
        for line in lines:
            line = line.strip()
            if len(line) > 0 and not line.startswith("#"):
                result.append(line)
        return result


    def __CheckFeatureVulkanVsABI(self, package: Package, androidABIList: List[str]) -> None:
        # AndroidABIOption.DeprecatedArmeAbi does not currently work with Vulkan
        if not AndroidABIOption.DeprecatedArmeAbi in androidABIList:
            return

        for feature in package.ResolvedAllUsedFeatures:
            if feature.Id == "vulkan":
                raise Exception("Android ABI '{0}' does not currently work with Vulkan, please select another using the {1} variant.".format(AndroidABIOption.DeprecatedArmeAbi, GEN_MAGIC_VARIANT_ANDROID_ABI))

    # Assimp support
    # - armeabi
    # X armeabi-v7a,
    # X arm64-v8a,
    # X x86,
    # X x86_64,
    # - mips,
    # - mips64
    def __CheckAssimpVsABI(self, package: Package, androidABIList: List[str]) -> None:
        # AndroidABIOption.DeprecatedArmeAbi does not currently work with Vulkan
        if not AndroidABIOption.DeprecatedArmeAbi in androidABIList and not AndroidABIOption.DeprecatedMips in androidABIList and not AndroidABIOption.DeprecatedMips64 in androidABIList:
            return

        for depPackage in package.ResolvedBuildOrder:
            if depPackage.Name.lower() == "assimp":
                raise Exception("Android ABI '{0}' does not currently work with Assimp, please select another using the {1} variant.".format([AndroidABIOption.DeprecatedArmeAbi, AndroidABIOption.DeprecatedMips, AndroidABIOption.DeprecatedMips64], GEN_MAGIC_VARIANT_ANDROID_ABI))


    def __CheckABI(self, package: Package, androidABIList: List[str]) -> None:
        self.__CheckFeatureVulkanVsABI(package, androidABIList)
        self.__CheckAssimpVsABI(package, androidABIList)


    def __patchABIList(self, config: Config, package: Package, androidABIList: List[str]) -> List[str]:
        if not AndroidABIOption.DeprecatedArmeAbi in androidABIList and not AndroidABIOption.DeprecatedMips in androidABIList and not AndroidABIOption.DeprecatedMips64 in androidABIList:
            return androidABIList

        removed = []
        result = list(androidABIList)
        for depPackage in package.ResolvedBuildOrder:
            if depPackage.Name.lower() == "assimp":
                if AndroidABIOption.DeprecatedArmeAbi in androidABIList:
                    result.remove(AndroidABIOption.DeprecatedArmeAbi)
                    removed.append(AndroidABIOption.DeprecatedArmeAbi)
                if AndroidABIOption.DeprecatedMips in androidABIList:
                    result.remove(AndroidABIOption.DeprecatedMips)
                    removed.append(AndroidABIOption.DeprecatedMips)
                if AndroidABIOption.DeprecatedMips64 in androidABIList:
                    result.remove(AndroidABIOption.DeprecatedMips64)
                    removed.append(AndroidABIOption.DeprecatedMips64)

        # If all would be removed by this patch, dont remove anything and let the checker catch incompatibility issue so its reported
        if len(result) <= 0:
            return androidABIList

        if len(removed) > 0:
            config.LogPrint("Removed incompatible assimp ABI's: %s" % (removed))
        return result


    def __CreateGradleAndroidABIList(self, androidABIList: List[str]) -> str:
        result = ["'{0}'".format(abi) for abi in androidABIList]
        return ", ".join(result)


    def __AddCMakeLibs(self, package: Package) -> List[AndroidCMakeLib]:
        result = []   # type: List[AndroidCMakeLib]
        for depPackage in package.ResolvedBuildOrder:
            recipe = depPackage.ResolvedDirectExperimentalRecipe
            addCommand = self.__TryAddAsCMakeLib(recipe, depPackage)
            if addCommand is not None:
                result.append(addCommand)
        return result


    def __TryAddAsCMakeLib(self, recipe: Optional[PackageExperimentalRecipe], package: Package) -> Optional[AndroidCMakeLib]:
        if recipe is None or recipe.ResolvedInstallLocation is None or recipe.Pipeline is None:
            return None
        if not PackageRecipeUtil.CommandListContainsBuildCMake(recipe.Pipeline.CommandList):
            return None

        path = "{0}".format(recipe.ResolvedInstallLocation)
        staticLibs = [] # type: List[AndroidCMakeLibRecord]
        if recipe.ValidateInstallation is not None and recipe.ValidateInstallation.CommandList is not None:
            for command in recipe.ValidateInstallation.CommandList:
                if command.CommandType == BuildRecipeValidateCommand.AddLib:
                    commandEx = cast(XmlRecipeValidateCommandAddLib, command)
                    libName = LibUtil.ToUnixLibName(IOUtil.GetFileName(commandEx.Name))
                    libPath = IOUtil.Join(path, "${ANDROID_ABI}")
                    libPath = IOUtil.Join(libPath, commandEx.Name)
                    staticLibs.append(AndroidCMakeLibRecord(libName, libPath))
#                elif command.CommandType == BuildRecipeValidateCommand.AddDLL:
#                    dynamicLibs.append(LibUtil.ToUnixLibName(IOUtil.GetFileName(command.Name)))

        return AndroidCMakeLib(path, staticLibs)


class GeneratorAndroidGradleCMakeUtil(object):
    @staticmethod
    def GetAndroidProjectDir(package: Package) -> str:
        """ Get the android project dir of the package, this is the dir that the gradle build reside in """
        appPackageTemplateInfo = AndroidGeneratorUtil.AppPackageTemplateInfo(package)
        # The old code used the "config.SDKPathAndroidProjectDir"
        # but now we resolve it from the environment variable name
        environmentVariable = ToolEnvironmentVariableName.FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR
        environmentVariable = "$({0})".format(environmentVariable)
        return IOUtil.Join(environmentVariable, appPackageTemplateInfo.ProjectPathName)


    @staticmethod
    def GetPlatformGradleCommand() -> str:
        if os.name == 'posix':
            return 'gradlew'
        elif os.name == 'nt':
            return 'gradlew.bat'
        else:
            raise EnvironmentError("Unsupported build environment for OS '{0}'".format(os.name))


    @staticmethod
    def GenerateVariableReport(log: Log, generatorName: str, package: Package) -> GeneratorVariableReport:
        variableReport = GeneratorVariableReport(log)
        # Add all the package variants
        for variantEntry in package.ResolvedAllVariantDict.values():
            variantEntryOptions = [option.Name for option in variantEntry.Options]
            variableReport.Add(variantEntry.Name, variantEntryOptions)

        # Add all the package flavor selections
        GeneratorUtil.AddFlavors(variableReport, package)

        # Gradle names for debug and release building
        variableReport.Add(LocalMagicBuildVariants.GradleBuildConfig, ['assembleDebug', 'assembleRelease'], ToolAddedVariant.CONFIG)

        # make builds default to release
        #variableReport.SetDefaultOption(ToolAddedVariant.CONFIG, ToolAddedVariantConfigOption.Release)
        return variableReport


    @staticmethod
    def TryGenerateBuildReport(log: Log, generatorName: str, package: Package) -> Optional[GeneratorBuildReport]:
        if package.IsVirtual:
            return None
        if package.Type != PackageType.Executable:
            return GeneratorBuildReport(None)

        commandCWD = GeneratorAndroidGradleCMakeUtil.GetAndroidProjectDir(package)

        gradleBuildConfigVariable = "${{{0}}}".format(LocalMagicBuildVariants.GradleBuildConfig)
        buildCommandArguments = [gradleBuildConfigVariable]
        buildCommand = GeneratorAndroidGradleCMakeUtil.GetPlatformGradleCommand()
        buildCommand = IOUtil.Join(commandCWD, buildCommand)
        buildCommandReport = GeneratorCommandReport(False, buildCommand, buildCommandArguments, [], commandCWD)
        return GeneratorBuildReport(buildCommandReport)


    @staticmethod
    def TryGenerateGeneratorPackageReport(log: Log, generatorName: str, package: Package) -> Optional[PackageGeneratorReport]:
        if package.IsVirtual and package.Type != PackageType.HeaderLibrary:
            return None
        #
        buildReport = GeneratorAndroidGradleCMakeUtil.TryGenerateBuildReport(log, generatorName, package)
        executableReport = None  # We dont currently support running android apps
        variableReport = GeneratorAndroidGradleCMakeUtil.GenerateVariableReport(log, generatorName, package)
        return PackageGeneratorReport(buildReport, executableReport, variableReport, None)

