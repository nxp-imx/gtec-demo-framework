#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2017 NXP
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
#    * Neither the name of the NXP. nor the names of
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

from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
import os
import subprocess
import time
import urllib.request
import urllib.parse
import urllib.error
from FslBuildGen import IOUtil
#from FslBuildGen import PackageConfig
from FslBuildGen.Build.BuildUtil import PlatformBuildTypeInfo
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.BuildExternal import CMakeHelper
from FslBuildGen.BuildExternal.CMakeTypes import CMakeBuildType
from FslBuildGen.BuildExternal.CMakeTypes import CMakeGeneratorMultiConfigCapability
from FslBuildGen.BuildExternal.CMakeTypes import CMakeGeneratorName
from FslBuildGen.BuildExternal.FileUnpacker import FileUnpack
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import BuildPlatformType
#from FslBuildGen.DataTypes import BuildThreads
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import CMakeTargetType
#from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.GitUtil import GitUtil
from FslBuildGen.PackageToolFinder import PackageToolFinder
from FslBuildGen.PlatformUtil import PlatformUtil


class BasicTask(object):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super().__init__()
        self.Context = generatorContext
        self.Log = generatorContext.Log
        #self._IsAndroid = generatorContext.PlatformName == PlatformNameString.ANDROID

    def LogPrint(self, message: str) -> None:
        self.Log.LogPrint(message)

    def LogPrintWarning(self, message: str) -> None:
        self.Log.LogPrintWarning(message)

    def DoPrint(self, message: str) -> None:
        self.Log.DoPrint(message)

    def CreateDirectory(self, path: str) -> None:
        if not IOUtil.IsDirectory(path):
            self.Log.LogPrint("Creating '{0}' as it was missing".format(path))
            IOUtil.SafeMakeDirs(path)


class DownloadTask(BasicTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super().__init__(generatorContext)

    @staticmethod
    def __DownloadReport(fnLogPrint: Callable[[str], None], startTime: float, shortFileName: str,
                         count: int, blockSize: int, totalSize: int) -> None:
        if totalSize <= 0:
            return

        duration = time.time() - startTime
        progressSize = int(count * blockSize)
        speed = int(progressSize / (1024 * duration)) if duration > 0 else 0
        percent = min(int(count * blockSize * 100 / totalSize), 100)
        fnLogPrint("* {0}: {1:3d}% of {2} MB, {3} KB/s, {4:.2f} seconds passed.".format(shortFileName, percent, totalSize / (1024 * 1024), speed, duration))


    @staticmethod
    def __MakeDownReporter(fnLogPrint: Callable[[str], None], shortFileName: str) -> Callable[[int, int, int], None]:
        startTime = time.time()
        return lambda x, y, z: DownloadTask.__DownloadReport(fnLogPrint, startTime, shortFileName, x, y, z)


    def DownloadFromUrl(self, url: str, dstPath: str) -> None:
        if IOUtil.IsFile(dstPath):
            self.LogPrint("Downloaded archive found at '{0}', skipping download.".format(dstPath))
            return

        self.DoPrint("Downloading '{0}' to '{1}'".format(url, dstPath))
        reporter = DownloadTask.__MakeDownReporter(self.DoPrint, IOUtil.GetFileName(dstPath))
        urllib.request.urlretrieve(url, dstPath, reporthook=reporter)


class GitBaseTask(BasicTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super().__init__(generatorContext)
        self.__ConfigureForPlatform(generatorContext)

    def __ConfigureForPlatform(self, generatorContext: GeneratorContext) -> None:
        self.GitCommand = GitUtil.GetExecutableName(generatorContext.Generator.PlatformName)


class GitCloneTask(GitBaseTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super().__init__(generatorContext)


    def RunGitClone(self, sourcePath: str, branch: str, targetPath: str) -> None:
        if IOUtil.IsDirectory(targetPath):
            self.LogPrint("Running git clone {0} {1}, skipped since it exist.".format(sourcePath, targetPath))
            return

        self.DoPrint("Running git clone {0} {1}".format(sourcePath, targetPath))
        try:
            self.__RunGitClone(sourcePath, targetPath, branch)
        except Exception:
            # A error occurred removing the targetPath
            self.LogPrint("* A error occurred removing '{0}' to be safe.".format(targetPath))
            IOUtil.SafeRemoveDirectoryTree(targetPath, True)
            raise

    def RunGitCheckout(self, sourcePath: str, branch: str) -> None:
        self.DoPrint("Running git checkout {0} at {1}".format(branch, sourcePath))
        try:
            self.__RunGitCheckout(sourcePath, branch)
        except Exception:
            # A error occurred removing the targetPath
            raise


    def GetCurrentHash(self, path: str) -> str:
        return GitUtil.GetCurrentHash(self.GitCommand, path)


    def __RunGitClone(self, sourcePath: str, targetPath: str, branch: str) -> None:
        buildCommand = [self.GitCommand, 'clone', sourcePath, targetPath]
        # for faster checkout
        ##--single-branch --depth 1
        buildCommand += ['--single-branch']
        if branch is not None and len(branch) > 0:
            buildCommand += ['-b', branch]
        result = subprocess.call(buildCommand)
        if result != 0:
            raise Exception("git clone failed {0}".format(buildCommand))


    def __RunGitCheckout(self, sourcePath: str, branch: str) -> None:
        if len(branch) <= 0:
            raise Exception("A git checkout branch name can not be empty")

        currentWorkingDirectory = sourcePath
        buildCommand = [self.GitCommand, 'checkout', branch]
        result = subprocess.call(buildCommand, cwd=currentWorkingDirectory)
        if result != 0:
            self.LogPrintWarning("The command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
            raise Exception("git clone failed {0}".format(buildCommand))


class GitApplyTask(GitBaseTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super().__init__(generatorContext)


    def RunGitApply(self, sourcePatchFile: str, targetPath: str) -> None:
        self.LogPrint("Running git apply {0} in {1}".format(sourcePatchFile, targetPath))
        buildCommand = [self.GitCommand, 'apply', sourcePatchFile, "--whitespace=fix"]
        if self.Log.Verbosity > 0:
            buildCommand.append("-v")
        result = subprocess.call(buildCommand, cwd=targetPath)
        if result != 0:
            raise Exception("git apply failed {0}".format(buildCommand))


class UnpackAndRenameTask(BasicTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super().__init__(generatorContext)

    def RunUnpack(self, packedFilePath: str, dstPath: str) -> None:
        if not IOUtil.IsDirectory(dstPath):
            self.__RunUnpack(packedFilePath, dstPath)
        else:
            self.LogPrint("Unpacked directory found at '{0}', skipping unpack.".format(dstPath))

    def __RunUnpack(self, packedFilePath: str, dstPath: str) -> None:
        self.LogPrint("* Unpacking archive '{0}' to '{1}'".format(packedFilePath, dstPath))
        FileUnpack.UnpackFile(packedFilePath, dstPath)


class CMakeBuilder(object):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int,
                 buildTypeInfo: PlatformBuildTypeInfo) -> None:
        super().__init__()
        self.Context = generatorContext
        self.Log = generatorContext.Log
        # Builders like ninja and make only contains a single configuration
        self.IsSingleConfiguration = False
#        #self.__ConfigureForPlatform(generatorContext)
        self.BuilderThreadArguments = [] # type: List[str]
        self.NumBuildThreads = PlatformBuildUtil.AddBuildThreads(generatorContext.Log, self.BuilderThreadArguments, generatorContext.PlatformName,
                                                                 buildThreads, buildTypeInfo, generatorContext.CMakeConfig, True)

    def Execute(self, toolFinder: PackageToolFinder, path: str, target: CMakeTargetType, cmakeProjectName: str, configuration: BuildVariantConfig,
                buildEnv: Dict[str, str], parentPath: str) -> None:
        pass


class CMakeBuilderDummy(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads, PlatformBuildTypeInfo.CMakeCustom)
        self.IsSingleConfiguration = False
    def Execute(self, toolFinder: PackageToolFinder, path: str, target: CMakeTargetType, cmakeProjectName: str, configuration: BuildVariantConfig,
                buildEnv: Dict[str, str], parentPath: str) -> None:
        raise Exception("This builder's Execute method is not supposed to be called")


class CMakeBuilderGeneric(CMakeBuilder):
    MINIMUM_VERSION = CMakeVersion(3, 16, 0)

    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads, PlatformBuildTypeInfo.CMake)

        cmakeConfig = generatorContext.CMakeConfig
        if cmakeConfig.CMakeVersion < CMakeBuilderGeneric.MINIMUM_VERSION:
            raise Exception("The chosen CMake generator '{0}' requires cmake version {1} or newer".format(cmakeConfig.CMakeFinalGeneratorName, CMakeBuilderGeneric.MINIMUM_VERSION))

        # The cmake make files only support one configuration
        self.IsSingleConfiguration = (CMakeHelper.GetGeneratorMultiConfigCapabilities(cmakeConfig.CMakeFinalGeneratorName) != CMakeGeneratorMultiConfigCapability.Yes)
        self.CMakeConfig = cmakeConfig


    def Execute(self, toolFinder: PackageToolFinder, path: str, target: CMakeTargetType, cmakeProjectName: str, configuration: BuildVariantConfig,
                buildEnv: Dict[str, str], parentPath: str) -> None:

        self.Log.LogPrint("* Running make at '{0}' for project '{1}' and configuration '{2}'".format(path, cmakeProjectName,
                                                                                                     BuildVariantConfig.ToString(configuration)))

        cmakeCommand = self.CMakeConfig.CMakeCommand
        cmakeConfig = CMakeBuildType.FromBuildVariantConfig(configuration)
        buildCommand = [cmakeCommand, '--build', path, '--config', cmakeConfig]
        if self.CMakeConfig.EmscriptenEnabled:
            buildCommand.insert(0, self.CMakeConfig.EmscriptenBuildCommand)

        if self.NumBuildThreads > 0:
            buildCommand += ['--parallel', str(self.NumBuildThreads)]

        try:
            result = subprocess.call(buildCommand, cwd=parentPath, env=buildEnv)
            if result != 0:
                raise Exception("cmake failed {0}".format(buildCommand))
        except Exception:
            self.Log.LogPrint("* cmake failed '{0}'".format(buildCommand))
            raise

        if target == CMakeTargetType.Install:
            buildCommand = [cmakeCommand, '--install', path, '--config', cmakeConfig]
            if self.CMakeConfig.EmscriptenEnabled:
                buildCommand.insert(0, self.CMakeConfig.EmscriptenBuildCommand)
            try:
                result = subprocess.call(buildCommand, cwd=parentPath, env=buildEnv)
                if result != 0:
                    raise Exception("cmake failed {0}".format(buildCommand))
            except Exception:
                self.Log.LogPrint("* cmake failed '{0}'".format(buildCommand))
                raise


class CMakeBuilderNinja(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int, useRecipe: bool = True) -> None:
        super().__init__(generatorContext, buildThreads, PlatformBuildTypeInfo.CMakeCustom)
        self.IsSingleConfiguration = True
        self.__CommandName = PlatformUtil.GetPlatformDependentExecuteableName("ninja", PlatformUtil.DetectBuildPlatformType())
        self.__UseRecipe = useRecipe

    def Execute(self, toolFinder: PackageToolFinder, path: str, target: CMakeTargetType, cmakeProjectName: str, configuration: BuildVariantConfig,
                buildEnv: Dict[str, str], parentPath: str) -> None:
        projectFile = "rules.ninja"

        if self.__UseRecipe:
            toolPackage = toolFinder.GetToolPackageByToolName('ninja')
            commandName = IOUtil.Join(toolPackage.AbsoluteToolPath, self.__CommandName)
        else:
            commandName = self.__CommandName

        self.Log.LogPrint("* Running ninja at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, BuildVariantConfig.ToString(configuration)))
        buildCommand = [commandName]
        if target == CMakeTargetType.Install:
            buildCommand.append('install')

        buildCommand += self.BuilderThreadArguments

        try:
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                raise Exception("ninja failed with {0} command {1}".format(result, buildCommand))
        except Exception:
            self.Log.LogPrint("* ninja failed '{0}'".format(buildCommand))
            raise


class CMakeBuilderMake(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads, PlatformBuildTypeInfo.CMakeCustom)
        # The cmake make files only support one configuration
        self.IsSingleConfiguration = True


    def Execute(self, toolFinder: PackageToolFinder, path: str, target: CMakeTargetType, cmakeProjectName: str, configuration: BuildVariantConfig,
                buildEnv: Dict[str, str], parentPath: str) -> None:
        projectFile = "Makefile"

        self.Log.LogPrint("* Running make at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, BuildVariantConfig.ToString(configuration)))
        buildCommand = ['make', '-f', projectFile]
        buildCommand += self.BuilderThreadArguments
        if target == CMakeTargetType.Install:
            buildCommand.append('install')
        try:
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                raise Exception("make failed {0}".format(buildCommand))
        except Exception:
            self.Log.LogPrint("* make failed '{0}'".format(buildCommand))
            raise


class CMakeBuilderMSBuild(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext, buildThreads, PlatformBuildTypeInfo.CMakeCustom)

    # msbuild INSTALL.vcxproj /p:Configuration=Debug
    # msbuild INSTALL.vcxproj /p:Configuration=Release
    def Execute(self, toolFinder: PackageToolFinder, path: str, target: CMakeTargetType, cmakeProjectName: str, configuration: BuildVariantConfig,
                buildEnv: Dict[str, str], parentPath: str) -> None:
        projectFile = self.__GetMSBuildFilename(target, cmakeProjectName)
        configurationString = self.__GetMSBuildBuildVariantConfigString(configuration)
        self.Log.LogPrint("* Running msbuild at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, configurationString))
        configurationString = "/p:Configuration={0}".format(configurationString)
        buildCommand = ['msbuild.exe', projectFile, configurationString]
        buildCommand += self.BuilderThreadArguments
        try:
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                raise Exception("msbuild failed {0}".format(buildCommand))
        except Exception:
            self.Log.LogPrint("* msbuild failed '{0}'".format(buildCommand))
            raise


    def __GetMSBuildFilename(self, target: CMakeTargetType, cmakeProjectName: str) -> str:
        if target == CMakeTargetType.Install:
            return "Install.vcxproj"
        return "{0}.sln".format(cmakeProjectName)


    def __GetMSBuildBuildVariantConfigString(self, configuration: BuildVariantConfig) -> str:
        if configuration == BuildVariantConfig.Debug or configuration == BuildVariantConfig.Coverage:
            return "Debug"
        elif configuration == BuildVariantConfig.Release:
            return "Release"
        else:
            raise Exception("Unsupported BuildVariantConfig: {0}".format(configuration))




class CMakeAndBuildTask(BasicTask):
    def __init__(self, generatorContext: GeneratorContext, buildThreads: int) -> None:
        super().__init__(generatorContext)
        self.CMakeConfig = generatorContext.CMakeConfig
        self.Builder = self.__DetermineBuilder(self.CMakeConfig.GeneratorName, generatorContext, buildThreads)

    # cmake -G "Visual Studio 14 2015 Win64"
    # -DCMAKE_INSTALL_PREFIX="e:\Work\Down\Windows\final\zlib-1.2.11"
    def RunCMakeAndBuild(self, toolFinder: PackageToolFinder, sourcePath: str, installPath: str, tempBuildPath: str, target: CMakeTargetType,
                         cmakeProjectName: str, configurationList: List[BuildVariantConfig], cmakeOptionList: List[str],
                         allowSkip: bool) -> None:
        if allowSkip and IOUtil.IsDirectory(installPath):
            self.LogPrint("Running cmake and build on source '{0}' and installing to '{1}' was skipped since install directory exist.".format(sourcePath, installPath))
            return

        self.LogPrint("Running cmake and build on source '{0}' and installing to '{1}'".format(sourcePath, installPath))
        try:
            self.CreateDirectory(tempBuildPath)

            # Add platform specific commands
            if len(self.CMakeConfig.CMakeConfigRecipeArguments) > 0:
                cmakeOptionList += self.CMakeConfig.CMakeConfigRecipeArguments

            buildEnv = os.environ.copy()  # type: Dict[str, str]
            self.__ApplyPath(buildEnv, toolFinder.ToolPaths)

            self.__DoBuildNow(toolFinder, sourcePath, installPath, tempBuildPath, target, cmakeProjectName, configurationList, cmakeOptionList,
                              buildEnv)
        except Exception:
            # A error occurred remove the install dir
            self.LogPrint("* A error occurred removing '{0}' to be safe.".format(installPath))
            IOUtil.SafeRemoveDirectoryTree(installPath, True)
            raise


    def __DoBuildNow(self, toolFinder: PackageToolFinder, sourcePath: str, installPath: str, tempBuildPath: str, target: CMakeTargetType,
                     cmakeProjectName: str, configurationList: List[BuildVariantConfig], cmakeOptionList: List[str],
                     buildEnv: Dict[str, str]) -> None:

        if not self.Builder.IsSingleConfiguration:
            self.RunCMake(tempBuildPath, sourcePath, installPath, cmakeOptionList, buildEnv)

            for config in configurationList:
                self.Builder.Execute(toolFinder, tempBuildPath, target, cmakeProjectName, config, buildEnv, sourcePath)
        else:
            for config in configurationList:
                self.RunCMake(tempBuildPath, sourcePath, installPath, cmakeOptionList, buildEnv, config)
                self.Builder.Execute(toolFinder, tempBuildPath, target, cmakeProjectName, config, buildEnv, sourcePath)

    def RunCMake(self, path: str, sourcePath: str, cmakeInstallPrefix: str, cmakeOptionList: List[str],
                 buildEnv: Dict[str, str], buildVariantConfig: Optional[BuildVariantConfig] = None) -> None:
        defineCMakeInstallPrefix = "-DCMAKE_INSTALL_PREFIX={0}".format(cmakeInstallPrefix)

        # Add user state
       # self.SaveStateManager.CMakeState.Add(buildVariantConfig)
       #= CMakeAndBuildTaskCMakeSaveState(self.CMakeConfig.CMakeUserArguments)

        defineBuildType = self.__TryGetBuildTypeString(buildVariantConfig)

        self.LogPrint("* Running cmake at '{0}' for source '{1}' with prefix {2} and options {3}".format(path, sourcePath, defineCMakeInstallPrefix, cmakeOptionList))

        buildCommand = [self.CMakeConfig.CMakeCommand, '-G', self.CMakeConfig.CMakeFinalGeneratorName, defineCMakeInstallPrefix, sourcePath]
        if self.CMakeConfig.EmscriptenEnabled:
            buildCommand.insert(0, self.CMakeConfig.EmscriptenConfigureCommand)

        if defineBuildType is not None:
            buildCommand.append("-D{0}".format(defineBuildType))

        if len(cmakeOptionList) > 0:
            buildCommand += cmakeOptionList

        self.Log.LogPrintVerbose(4, "Build commands '{0}'".format(buildCommand))

        result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
        if result != 0:
            raise Exception("CMake failed {0}".format(buildCommand))

    #def __AddToolDependencies()

    def __ApplyPath(self, env: Dict[str, str], paths: List[str]) -> None:
        if len(paths) <= 0:
            return
        res = ";{0}".format(";".join(paths))
        if 'PATH' in env:
            env['PATH'] += res
        else:
            env['PATH'] = res


    def __TryGetBuildTypeString(self, buildVariantConfig: Optional[BuildVariantConfig]) -> Optional[str]:
        if buildVariantConfig is None:
            return None
        buildType = CMakeBuildType.FromBuildVariantConfig(buildVariantConfig)
        return "CMAKE_BUILD_TYPE={0}".format(buildType)



    def __DetermineBuilder(self, generatorName: str, generatorContext: GeneratorContext, buildThreads: int) -> CMakeBuilder:
        if generatorName == CMakeGeneratorName.Android:
            if PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows:
                return CMakeBuilderNinja(generatorContext, buildThreads)
            return CMakeBuilderMake(generatorContext, buildThreads)
        isMSVC = (generatorName == CMakeGeneratorName.VisualStudio2015_X64 or generatorName == CMakeGeneratorName.VisualStudio2017_X64 or
                  generatorName == CMakeGeneratorName.VisualStudio2019_X64 or generatorName == CMakeGeneratorName.VisualStudio2022_X64)
        # The generic handler does not really apply proper threaded builds, so we use the old one for MSVC
        if generatorContext.CMakeConfig.CMakeVersion < CMakeBuilderGeneric.MINIMUM_VERSION or isMSVC:
            if isMSVC:
                return CMakeBuilderMSBuild(generatorContext, buildThreads)
            if generatorName == CMakeGeneratorName.Ninja:
                return CMakeBuilderNinja(generatorContext, buildThreads, False)
            if generatorName == CMakeGeneratorName.UnixMakeFile:
                return CMakeBuilderMake(generatorContext, buildThreads)
        return CMakeBuilderGeneric(generatorContext, buildThreads)
