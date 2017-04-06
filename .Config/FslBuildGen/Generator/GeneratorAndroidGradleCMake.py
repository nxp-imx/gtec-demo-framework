#!/usr/bin/env python

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

import itertools
import os
import os.path
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen.Generator import AndroidGeneratorUtil
from FslBuildGen.Generator import CMakeGeneratorUtil
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.SharedGeneration import *
from FslBuildGen.PackageBuildReport import *

def GetVCBuildConfigurationName(entry):
    return "-".join(entry)

# Status
# - External libs with special debug libraries are not handled
# - FslBuild things dont work
# Note:
# - The package headers might not completely follow the FslBuildGen standards
#   Meaning some packages might have access to more than their Fsl.gen file 
#   allows them access to


# This generator does not work if there are multiple source roots :(
class GeneratorAndroidGradleCMake(object):
    def __init__(self, config, packages, platformName, androidABIList):
        super(GeneratorAndroidGradleCMake, self).__init__()

        if config.SDKPathAndroidProjectDir == None:
            raise EnvironmentError("Android environment variable FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR not defined")

        strAppTemplatePath = "AndroidGradleCMake"
        appTemplateManager = AndroidGeneratorUtil.AppTemplateManager(config, strAppTemplatePath)

        strTemplatePath = IOUtil.Join(strAppTemplatePath, "CMake")
        extTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Ext", False)
        libTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Lib", False)
        exeTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Exe", False)

        templatePath = IOUtil.Join(config.SDKConfigTemplatePath, strAppTemplatePath)
        exeFileList = self.__ParseExeFileList(IOUtil.Join(templatePath, "ExeFiles.txt"))

        totalExeCount = 0
        for mainPackage in packages:
            if mainPackage.Type == PackageType.Executable:
                totalExeCount = totalExeCount + 1
                appPackageTemplateInfo = AndroidGeneratorUtil.AppPackageTemplateInfo(mainPackage)
                androidProjectDir = IOUtil.Join(config.SDKPathAndroidProjectDir, appPackageTemplateInfo.ProjectPathName)
                androidProjectCMakeDir = IOUtil.Join(androidProjectDir, ".FslCMake")

                for package in mainPackage.ResolvedBuildOrder:
                    if package.Type == PackageType.ExternalLibrary or package.Type == PackageType.HeaderLibrary:
                        self.__GenerateCMakeFile(config, package, platformName, extTemplate, androidProjectDir, androidProjectCMakeDir)
                    elif package.Type == PackageType.Library:
                        self.__GenerateCMakeFile(config, package, platformName, libTemplate, androidProjectDir, androidProjectCMakeDir)
                    elif package.Type == PackageType.Executable:
                        self.__GenerateExecutable(config, package, platformName, exeTemplate, appTemplateManager, appPackageTemplateInfo, androidProjectDir, androidProjectCMakeDir, exeFileList, androidABIList)

        # For now we only support doing 'exe' builds using full source for everything (like the old builder)
        if totalExeCount <= 0:
            config.DoPrint("No executables provided, nothing to build.");


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}


    def GetAndroidProjectDir(self, config, package, appPackageTemplateInfo = None):
        """ Get the android project dir of the package, this is the dir that the gradle build reside in """
        appPackageTemplateInfo = appPackageTemplateInfo if appPackageTemplateInfo else AndroidGeneratorUtil.AppPackageTemplateInfo(package)
        return IOUtil.Join(config.SDKPathAndroidProjectDir, appPackageTemplateInfo.ProjectPathName)


    def __GenerateCMakeFile(self, config, package, platformName, template, androidProjectDir, androidProjectCMakeDir):
        packageName = CMakeGeneratorUtil.GetPackageName(package)

        # this ignore is a workaround allowing us to build using the same info as the old Android builder
        ignoreLibs = ["android_native_app_glue"]
        
        aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName)
        targetIncludeDirectories = CMakeGeneratorUtil.BuildTargetIncludeDirectories(config, package, template.PackageTargetIncludeDirectories, template.PackageTargetIncludeDirEntry, True)
        includeFiles = self.__ExpandPathAndJoin(config, package, package.ResolvedBuildAllIncludeFiles)
        sourceFiles = self.__ExpandPathAndJoin(config, package, package.ResolvedBuildSourceFiles)
        linkLibrariesDirectDependencies = CMakeGeneratorUtil.BuildTargetLinkLibrariesForDirectDependencies(config, package, template.PackageDependencyTargetLinkLibraries, ignoreLibs)
        directDefinitions = CMakeGeneratorUtil.BuildDirectDefinitions(config, package, template.PackageDependencyTargetCompileDefinitions)
        findDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(config, package, template.PackageDependencyFindPackage)


        buildCMakeFile = template.Master
        
        if package.Type == PackageType.Executable:
            packageContentPath = CMakeGeneratorUtil.GetSDKBasedPathUsingCMakeVariable(config, package.AbsoluteContentPath)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_CONTENT_PATH##", packageContentPath)
            buildCMakeFile = buildCMakeFile.replace("##PACKAGE_ANDROID_PROJECT_PATH##", androidProjectDir)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_INCLUDE_FILES##", includeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_SOURCE_FILES##", sourceFiles)
        buildCMakeFile = buildCMakeFile.replace("##TARGET_INCLUDE_DIRECTORIES##", targetIncludeDirectories)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##", linkLibrariesDirectDependencies)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##", directDefinitions)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", findDirectExternalDependencies)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##", template.SnippetDefaultTargetCompileOptions)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##", template.SnippetDefaultTargetCompileFeatures)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_NAME!##", packageName.upper())
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_NAME##", packageName)
        buildCMakeFile = buildCMakeFile.replace("##ALIAS_PACKAGE_NAME##", aliasPackageName)

        if not config.DisableWrite:
            # We store all cmake build files in their own dir inside the 'android' exe-project's folder
            # to prevent collision with other more generic CMake builders
            packageCMakeDir = self.__GetPackageCMakeDir(androidProjectCMakeDir, package)
            IOUtil.SafeMakeDirs(packageCMakeDir)
            dstFileCMakeFile = self.__GetPackageCMakeFileName(androidProjectCMakeDir, package)
            IOUtil.WriteFileIfChanged(dstFileCMakeFile, buildCMakeFile)


    def __GetPackageCMakeDir(self, androidProjectCMakeDir, package):
            """ Get the directory that the CMake CMakeLists.txt file reside in for this package """
            return IOUtil.Join(androidProjectCMakeDir, package.Name)


    def __GetPackageCMakeFileName(self, androidProjectCMakeDir, package):
            """ Get the full path of the CMakeLists.txt file for the package """
            packageCMakeDir = self.__GetPackageCMakeDir(androidProjectCMakeDir, package)
            dstFileCMakeFile = IOUtil.Join(packageCMakeDir, "CMakeLists.txt")
            #dstFileCMakeFile = IOUtil.Join(package.AbsolutePath, "CMakeLists.txt")
            return dstFileCMakeFile


    def __GenerateExecutable(self, config, package, platformName, template, appTemplateManager, appPackageTemplateInfo, androidProjectDir, androidProjectCMakeDir, exeFileList, androidABIList):
        if not config.DisableWrite:
            # create folder structure
            appTemplateManager.CreateTemplate(androidProjectDir, package)
            # copy files that need to be modified
            self.__CopyAndModifyFiles(config, androidProjectDir, package, appTemplateManager.FilesToModify, appPackageTemplateInfo, template, androidProjectCMakeDir, androidABIList)
            # mark files as executable
            for entry in exeFileList:
                IOUtil.SetFileExecutable(IOUtil.Join(androidProjectDir, entry))


        self.__GenerateCMakeFile(config, package, platformName, template, androidProjectDir, androidProjectCMakeDir)


    def __CopyAndModifyFiles(self, config, dstPath, package, filesToModify, appPackageTemplateInfo, template, androidProjectCMakeDir, androidABIList):
        androidHome = IOUtil.GetEnvironmentVariableForDirectory("ANDROID_HOME")
        androidNDK = IOUtil.GetEnvironmentVariableForDirectory("ANDROID_NDK")
        androidNDKForProp = self.__ToPropPath(androidNDK)
        androidHomeForProp = self.__ToPropPath(androidHome)

        androidABIList = self.__patchABIList(config, package, androidABIList)
        self.__CheckABI(package, androidABIList)

        packageVariantGradleAndroidABIList = self.__CreateGradleAndroidABIList(androidABIList)

        packageName = CMakeGeneratorUtil.GetPackageName(package)
        cmakePackageExeLib = CMakeGeneratorUtil.GetAliasName(packageName)
        cmakePackageFindDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(config, package, template.PackageDependencyFindPackage)
        cmakePackageDirectDependenciesAndSubDirectories = self.__BuildCMakeAddSubDirectoriesForDirectDependencies(config, package, template, androidProjectCMakeDir)
        cmakeAssimpLib = self.__FindAssimpLib(package)
        cmakeAssimpSubDir = ""
        if len(cmakeAssimpLib) > 0:
            cmakeAssimpSubDir = "add_subdirectory(${FSL_GRAPHICS_SDK}/ThirdParty/AssimpSource ${CMAKE_BINARY_DIR}/AssimpSource)"


        for file in filesToModify:
            dst = IOUtil.Join(dstPath, file.RelativeDestPath)
            content = IOUtil.ReadFile(file.AbsoluteSourcePath)
            content = content.replace("##FSL_PACKAGE_GLES_VERSION##", appPackageTemplateInfo.MinGLESVersion)
            content = content.replace("##FSL_PACKAGE_MIN_ANDROID_SDK_VERSION##", appPackageTemplateInfo.MinSDKVersion.VersionString)
            content = content.replace("##FSL_PACKAGE_TARGET_ANDROID_SDK_VERSION##", appPackageTemplateInfo.TargetSDKVersion.VersionString)
            content = content.replace("##PACKAGE_APP_PLATFORM##", appPackageTemplateInfo.MinSDKVersion.AppPlatform)
            content = content.replace("##PACKAGE_LOCATION##", package.PackageLocation.Name)
            content = content.replace("##PACKAGE_TARGET_NAME##", package.ShortName)
            content = content.replace("##PREFIXED_PROJECT_NAME##", appPackageTemplateInfo.PrefixedProjectName)
            content = content.replace("##PREFIXED_PROJECT_NAME_L##", appPackageTemplateInfo.PrefixedProjectName.lower())
            content = content.replace("##ENV_ANDROID_NDK_FOR_PROP##", androidNDKForProp)
            content = content.replace("##ENV_ANDROID_HOME_FOR_PROP##", androidHomeForProp)
            content = content.replace("##CMAKE_PACKAGE_EXE_LIB##", cmakePackageExeLib)
            content = content.replace("##CMAKE_ASSIMP_LIB##", cmakeAssimpLib)
            content = content.replace("##CMAKE_ASSIMP_SUBDIR##", cmakeAssimpSubDir)
            content = content.replace("##CMAKE_PACKAGE_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", cmakePackageFindDirectExternalDependencies)
            content = content.replace("##CMAKE_PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##", cmakePackageDirectDependenciesAndSubDirectories)
            content = content.replace("##PACKAGE_VARIANT_ANDROID_ABIS##", packageVariantGradleAndroidABIList)

            dst = appPackageTemplateInfo.UpdateFileName(dst)
            dirName = IOUtil.GetDirectoryName(dst)
            IOUtil.SafeMakeDirs(dirName)
            IOUtil.WriteFileIfChanged(dst, content)


    def __BuildCMakeAddSubDirectoriesForDirectDependencies(self, config, package, template, androidProjectCMakeDir):       
        if len(package.ResolvedBuildOrder) <= 0:
            return ""
        content = "" 
        snippet = template.PackageDependencyAddSubdirectories
        for depPackage in package.ResolvedBuildOrder:
            #sdkPackagePath = CMakeGeneratorUtil.GetSDKBasedPathUsingCMakeVariable(config, depPackage.AbsolutePath)
            sdkPackagePath = self.__GetPackageCMakeDir(androidProjectCMakeDir, depPackage)
            # We take advantage of the fact that the full package name is unique
            cmakeBinPackagePath = "${CMAKE_BINARY_DIR}/%s" % (depPackage.Name)
            path = "%s %s" % (sdkPackagePath, cmakeBinPackagePath)
            content += snippet.replace("##PACKAGE_PATH##", path)
        return content


    def __ToPropPath(self, path):
        return path


    def __ExpandPathAndJoin(self, config, package, list):
        if not list or len(list) <= 0:
            return ''
        expandedList = [CMakeGeneratorUtil.GetSDKBasedPathUsingCMakeVariable(config, IOUtil.Join(package.AbsolutePath, entry)) for entry in list]
        return "\n  " + "\n  ".join(expandedList)

    def __ParseExeFileList(self, path):
        lines = IOUtil.ReadFile(path).split('\n')
        result = []
        for line in lines:
            line = line.strip()
            if len(line) > 0 and not line.startswith("#"):
                result.append(line)
        return result


    def __CheckFeatureVulkanVsABI(self, package, androidABIList):
        # AndroidABIOption.ArmeAbi does not currently work with Vulkan
        if not AndroidABIOption.ArmeAbi in androidABIList:
            return

        for feature in package.ResolvedAllUsedFeatures:
            if feature.Id == "vulkan":
                raise Exception("Android ABI '%s' does not currently work with Vulkan, please select another using the %s variant." % (AndroidABIOption.ArmeAbi, GEN_MAGIC_VARIANT_ANDROID_ABI));

    # Assimp support 
    # - armeabi
    # X armeabi-v7a,
    # X arm64-v8a,
    # X x86,
    # X x86_64,
    # - mips,
    # - mips64
    def __CheckAssimpVsABI(self, package, androidABIList):
        # AndroidABIOption.ArmeAbi does not currently work with Vulkan
        if not AndroidABIOption.ArmeAbi in androidABIList and not AndroidABIOption.Mips in androidABIList and not AndroidABIOption.Mips64 in androidABIList:
            return

        for depPackage in package.ResolvedBuildOrder:
            if depPackage.Name.lower() == "assimp":
                raise Exception("Android ABI '%s' does not currently work with Assimp, please select another using the %s variant." % ([AndroidABIOption.ArmeAbi, AndroidABIOption.Mips, AndroidABIOption.Mips64], GEN_MAGIC_VARIANT_ANDROID_ABI));


    def __CheckABI(self, package, androidABIList):
        self.__CheckFeatureVulkanVsABI(package, androidABIList)
        self.__CheckAssimpVsABI(package, androidABIList)


    def __patchABIList(self, config, package, androidABIList):
        if not AndroidABIOption.ArmeAbi in androidABIList and not AndroidABIOption.Mips in androidABIList and not AndroidABIOption.Mips64 in androidABIList:
            return androidABIList

        removed = []
        result = list(androidABIList)
        for depPackage in package.ResolvedBuildOrder:
            if depPackage.Name.lower() == "assimp":
                if AndroidABIOption.ArmeAbi in androidABIList:
                    result.remove(AndroidABIOption.ArmeAbi)
                    removed.append(AndroidABIOption.ArmeAbi)
                if AndroidABIOption.Mips in androidABIList:
                    result.remove(AndroidABIOption.Mips)
                    removed.append(AndroidABIOption.Mips)
                if AndroidABIOption.Mips64 in androidABIList:
                    result.remove(AndroidABIOption.Mips64)
                    removed.append(AndroidABIOption.Mips64)
        
        # If all would be removed by this patch, dont remove anything and let the checker catch incompatibility issue so its reported
        if len(result) <= 0:
            return androidABIList

        if len(removed) > 0:
            config.LogPrint("Removed incompatible assimp ABI's: %s" % (removed))
        return result


    def __CreateGradleAndroidABIList(self, androidABIList):
        result = ["'%s'" % (abi) for abi in androidABIList]
        return ", ".join(result)


    def __FindAssimpLib(self, package):
        for depPackage in package.ResolvedBuildOrder:
            if depPackage.Name.lower() == 'assimp':
                return "assimp"

        return ""
