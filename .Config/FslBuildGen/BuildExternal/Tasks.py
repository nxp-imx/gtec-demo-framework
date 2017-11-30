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
from typing import List
from typing import Optional
import subprocess
import time
import urllib.request
import urllib.parse
import urllib.error
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen.BuildExternal.FileUnpacker import FileUnpack
from FslBuildGen.BuildExternal import CMakeTypes
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import CMakeTargetType


class BasicTask(object):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super(BasicTask, self).__init__()
        self.Context = generatorContext
        self.BasicConfig = generatorContext.BasicConfig

    def LogPrint(self, message: str) -> None:
        self.BasicConfig.LogPrint(message)

    def DoPrint(self, message: str) -> None:
        self.BasicConfig.DoPrint(message)

    def CreateDirectory(self, path: str) -> None:
        if not IOUtil.IsDirectory(path):
            self.BasicConfig.LogPrint("Creating '{0}' as it was missing".format(path))
            IOUtil.SafeMakeDirs(path)


class DownloadTask(BasicTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super(DownloadTask, self).__init__(generatorContext)

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
        super(GitBaseTask, self).__init__(generatorContext)
        self.__ConfigureForPlatform(generatorContext)


    def __ConfigureForPlatform(self, generatorContext: GeneratorContext) -> None:
        if generatorContext.Platform.Name == PackageConfig.PlatformNameString.WINDOWS:
            self.GitCommand = 'git.exe'
        else:
            self.GitCommand = 'git'


class GitCloneTask(GitBaseTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super(GitCloneTask, self).__init__(generatorContext)


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


    def GetCurrentHash(self, path: str) -> str:
        command = [self.GitCommand, "rev-parse", "HEAD"]
        strVersion = None
        proc = subprocess.Popen(command, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, universal_newlines=True, cwd=path)
        try:
            (strVersion, err) = proc.communicate()
            proc.wait()
            strVersion = strVersion.strip()
        finally:
            proc.stdout.close()
        return str(strVersion)


    def __RunGitClone(self, sourcePath: str, targetPath: str, branch: str) -> None:
        buildCommand = [self.GitCommand, 'clone', sourcePath, targetPath]
        # for faster checkout
        ##--single-branch --depth 1
        buildCommand += ['--single-branch']
        if branch != None and len(branch) > 0:
            buildCommand += ['-b', branch]
        result = subprocess.call(buildCommand)
        if result != 0:
            raise Exception("git clone failed {0}".format(buildCommand))


class GitApplyTask(GitBaseTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super(GitApplyTask, self).__init__(generatorContext)


    def RunGitApply(self, sourcePatchFile: str, targetPath: str) -> None:
        self.LogPrint("Running git apply {0} in {1}".format(sourcePatchFile, targetPath))

        buildCommand = [self.GitCommand, 'apply', sourcePatchFile, "--whitespace=fix"]
        result = subprocess.call(buildCommand, cwd=targetPath)
        if result != 0:
            raise Exception("git apply failed {0}".format(buildCommand))


class UnpackAndRenameTask(BasicTask):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super(UnpackAndRenameTask, self).__init__(generatorContext)

    def RunUnpack(self, packedFilePath: str, dstPath: str) -> None:
        if not IOUtil.IsDirectory(dstPath):
            self.__RunUnpack(packedFilePath, dstPath)
        else:
            self.LogPrint("Unpacked directory found at '{0}', skipping unpack.".format(dstPath))

    def __RunUnpack(self, packedFilePath: str, dstPath: str) -> None:
        self.LogPrint("* Unpacking archive '{0}' to '{1}'".format(packedFilePath, dstPath))
        FileUnpack.UnpackFile(packedFilePath, dstPath)


class CMakeBuilder(object):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super(CMakeBuilder, self).__init__()
        self.Context = generatorContext
        self.BasicConfig = generatorContext.BasicConfig
        # Builders like ninja and make only contains a single configuration
        self.IsSingleConfiguration = False
#        #self.__ConfigureForPlatform(generatorContext)

    def Execute(self, path: str, target: int, cmakeProjectName: str, configuration: int) -> None:
        pass


class CMakeBuilderMake(CMakeBuilder):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super(CMakeBuilderMake, self).__init__(generatorContext)
        # The cmake make files only support one configuration
        self.IsSingleConfiguration = True

    def Execute(self, path: str, target: int, cmakeProjectName: str, configuration: int) -> None:
        projectFile = "Makefile"

        self.BasicConfig.LogPrint("* Running make at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, BuildVariantConfig.ToString(configuration)))
        buildCommand = ['make', '-f', projectFile]
        if target == CMakeTargetType.Install:
            buildCommand.append('install')
        result = subprocess.call(buildCommand, cwd=path)
        if result != 0:
            raise Exception("make failed {0}".format(buildCommand))


class CMakeBuilderMSBuild(CMakeBuilder):
    #def __init__(self, generatorContext: GeneratorContext) -> None:
    #    super(CMakeBuilderMSBuild, self).__init__(generatorContext)

    # msbuild INSTALL.vcxproj /p:Configuration=Debug
    # msbuild INSTALL.vcxproj /p:Configuration=Release
    def Execute(self, path: str, target: int, cmakeProjectName: str, configuration: int) -> None:
        projectFile = self.__GetMSBuildFilename(target, cmakeProjectName)
        configurationString = self.__GetMSBuildBuildVariantConfigString(configuration)
        self.BasicConfig.LogPrint("* Running msbuild at '{0}' for project '{1}' and configuration '{2}'".format(path, projectFile, configurationString))
        configurationString = "/p:Configuration={0}".format(configurationString)
        buildCommand = ['msbuild.exe', projectFile, configurationString]
        result = subprocess.call(buildCommand, cwd=path)
        if result != 0:
            raise Exception("msbuild failed {0}".format(buildCommand))


    def __GetMSBuildFilename(self, target: int, cmakeProjectName: str) -> str:
        if target == CMakeTargetType.Install:
            return "Install.vcxproj"
        return "{0}.sln".format(cmakeProjectName)


    def __GetMSBuildBuildVariantConfigString(self, configuration: int) -> str:
        if configuration == BuildVariantConfig.Debug:
            return "Debug"
        elif configuration == BuildVariantConfig.Release:
            return "Release"
        else:
            raise Exception("Unsupported BuildVariantConfig: {0}".format(configuration))



class CMakeAndBuildTask(BasicTask):
    def __init__(self, generatorContext: GeneratorContext) -> None:
        super(CMakeAndBuildTask, self).__init__(generatorContext)
        self.__ConfigureForPlatform(generatorContext)

    # cmake -G "Visual Studio 14 2015 Win64"
    # -DCMAKE_INSTALL_PREFIX="e:\Work\Down\Windows\final\zlib-1.2.11"
    def RunCMakeAndBuild(self, sourcePath: str, installPath: str, tempBuildPath: str, target: int, cmakeProjectName: str, configurationList: List[int], cmakeOptionList: List[str]) -> None:
        if IOUtil.IsDirectory(installPath):
            self.LogPrint("Running cmake and build on source '{0}' and installing to '{1}' was skipped since install directory exist.".format(sourcePath, installPath))
            return

        self.LogPrint("Running cmake and build on source '{0}' and installing to '{1}'".format(sourcePath, installPath))
        try:
            self.CreateDirectory(tempBuildPath)

            if not self.Builder.IsSingleConfiguration:
                self.RunCMake(tempBuildPath, sourcePath, installPath, cmakeOptionList)

                for config in configurationList:
                    self.Builder.Execute(tempBuildPath, target, cmakeProjectName, config)
            else:
                for config in configurationList:
                    self.RunCMake(tempBuildPath, sourcePath, installPath, cmakeOptionList, config)
                    self.Builder.Execute(tempBuildPath, target, cmakeProjectName, config)
        except Exception:
            # A error occurred remove the install dir
            self.LogPrint("* A error occurred removing '{0}' to be safe.".format(installPath))
            IOUtil.SafeRemoveDirectoryTree(installPath, True)
            raise


    def RunCMake(self, path: str, sourcePath: str, cmakeInstallPrefix: str, cmakeOptionList: List[str], buildVariantConfig: Optional[int] = None) -> None:
        defineCMakeInstallPrefix = "-DCMAKE_INSTALL_PREFIX={0}".format(cmakeInstallPrefix)

        defineBuildType = self.__TryGetBuildTypeString(buildVariantConfig)

        self.LogPrint("* Running cmake at '{0}' for source '{1}' with prefix {2} and options {3}".format(path, sourcePath, defineCMakeInstallPrefix, cmakeOptionList))
        buildCommand = [self.CMakeCommand, '-G', self.CMakeGeneratorName, defineCMakeInstallPrefix, sourcePath]
        if not defineBuildType is None:
            buildCommand.append("-D{0}".format(defineBuildType))

        if cmakeOptionList != None:
            buildCommand += cmakeOptionList
        result = subprocess.call(buildCommand, cwd=path)
        if result != 0:
            raise Exception("CMake failed {0}".format(buildCommand))


    def __TryGetBuildTypeString(self, buildVariantConfig: Optional[int]) -> Optional[str]:
        if buildVariantConfig is None:
            return None
        buildType = CMakeTypes.CMakeBuildType.FromBuildVariantConfig(buildVariantConfig)
        return "CMAKE_BUILD_TYPE={0}".format(buildType)


    def __ConfigureForPlatform(self, generatorContext: GeneratorContext) -> None:
        self.CMakeCommand = CMakeTypes.DetermineCMakeCommand(generatorContext.Platform.Name)
        self.CMakeGeneratorName = CMakeTypes.DetermineCMakeGenerator(generatorContext.Platform)
        self.CompilerShortId = CMakeTypes.GetCompilerShortIdFromGeneratorName(self.CMakeGeneratorName)
        self.Builder = self.__DetermineBuilder(self.CMakeGeneratorName, generatorContext)


    def __DetermineBuilder(self, generatorName: str, generatorContext: GeneratorContext) -> CMakeBuilder:
        if generatorName == CMakeTypes.CMakeGeneratorName.UnixMakeFile:
            return CMakeBuilderMake(generatorContext)
        elif generatorName == CMakeTypes.CMakeGeneratorName.VisualStudio2015_X64 or generatorName == CMakeTypes.CMakeGeneratorName.VisualStudio2017_X64:
            return CMakeBuilderMSBuild(generatorContext)
        raise Exception("No Builder defined for the cmake generator '{0}' on platform '{1}'".format(generatorName, generatorContext.PlatformName))
