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

from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.BuildConfig.BuildVariables import BuildVariables
from FslBuildGen.BuildConfig.UserSetVariables import UserSetVariables
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageConfig import PlatformNameIdString
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.BuildExternal.CMakeTypes import CMakeGeneratorName
from FslBuildGen.BuildExternal.CMakeTypes import CMakeGeneratorMultiConfigCapability

class CMakeEmscriptenConfig(object):
    def __init__(self, configureCommand: str, buildCommand: str, runCommand: str) -> None:
        super().__init__()
        self.ConfigureCommand = configureCommand
        self.BuildCommand = buildCommand
        self.RunCommand = runCommand


def DetermineCMakeCommand(platformName: str) -> str:
    return PlatformUtil.GetExecutableName('cmake', platformName)

def DetermineEmscriptenCommands() -> CMakeEmscriptenConfig:
    scriptName = 'emcmake'
    buildScriptName = 'emmake'
    runName = 'emrun'
    if PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows:
        return CMakeEmscriptenConfig(scriptName + '.bat', buildScriptName + '.bat', runName + '.bat')
    return CMakeEmscriptenConfig(scriptName, buildScriptName, runName)


def TryGetPlatformDefaultCMakeGenerator(platformName: str, compilerVersion: int) -> Optional[str]:
    if platformName == PackageConfig.PlatformNameString.UBUNTU:
        return CMakeGeneratorName.Ninja
    elif platformName == PackageConfig.PlatformNameString.YOCTO:
        return CMakeGeneratorName.Ninja
    elif platformName == PackageConfig.PlatformNameString.FREERTOS:
        return CMakeGeneratorName.Ninja
    elif platformName == PackageConfig.PlatformNameString.QNX:
        return CMakeGeneratorName.Ninja
    elif platformName == PackageConfig.PlatformNameString.WINDOWS:
        return CMakeGeneratorName.FromVisualStudioVersion(compilerVersion)
    elif platformName == PackageConfig.PlatformNameString.ANDROID:
        return CMakeGeneratorName.Android
    elif platformName == PackageConfig.PlatformNameString.EMSCRIPTEN:
        return CMakeGeneratorName.Ninja
    return None

    #if generator.Name == PackageConfig.PlatformNameString.ANDROID:
    #    return "NotSupported"
    #elif generator.Name == PackageConfig.PlatformNameString.YOCTO:
    #    return "NotSupported"
    #elif generator.Name == PackageConfig.PlatformNameString.QNX:
    #    return "NotSupported"

def DetermineFinalCMakeGenerator(generatorName: str) -> str:
    if generatorName != CMakeGeneratorName.Android:
        return generatorName
    if PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows:
        return CMakeGeneratorName.Ninja
    return CMakeGeneratorName.UnixMakeFile


def GetPlatformDefaultCMakeGenerator(platformName: str, compilerVersion: int) -> str:
    result = TryGetPlatformDefaultCMakeGenerator(platformName, compilerVersion)
    if not result is None:
        return result
    raise Exception("CMake generator name could not be determined for this platform '{0}".format(platformName))




def GetCompilerShortIdFromGeneratorName(generatorName: str) -> str:
    if generatorName == CMakeGeneratorName.UnixMakeFile:
        return "Make"
    elif generatorName == CMakeGeneratorName.VisualStudio2015_X64:
        return "VS2015_X64"
    elif generatorName == CMakeGeneratorName.VisualStudio2017_X64:
        return "VS2017_X64"
    elif generatorName == CMakeGeneratorName.VisualStudio2019_X64:
        return "VS2019_X64"
    elif generatorName == CMakeGeneratorName.VisualStudio2022_X64:
        return "VS2022_X64"
    elif generatorName == CMakeGeneratorName.Android:
        # For android we utilize a combination of the SDK and NDK version for the unique 'toolchain' name
        theId = AndroidUtil.GetSDKNDKId()
        sdkVersion = AndroidUtil.GetSDKVersion()
        return "V{1}{0}".format(theId, sdkVersion)

    generatorName = generatorName.strip()
    generatorName = generatorName.replace(' ', '_')
    return generatorName


def DeterminePlatformArguments(platformName: str) -> List[str]:
    res = [] # type: List[str]
    if platformName != PackageConfig.PlatformNameString.ANDROID:
        return res

    androidToolchain = "clang"
    androidStlType = "c++_shared"
    if not AndroidUtil.UseNDKCMakeToolchain():
        # NDK before 19
        res.append("-DCMAKE_SYSTEM_NAME=Android")
        res.append("-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION={0}".format(androidToolchain))
        res.append("-DCMAKE_ANDROID_STL_TYPE={0}".format(androidStlType))
    else:
        # NDK from 19
        res.append("-DCMAKE_TOOLCHAIN_FILE={0}".format(IOUtil.Join(AndroidUtil.GetNDKPath(), "build/cmake/android.toolchain.cmake")))
        res.append("-DANDROID_STL={0}".format(androidStlType))
        res.append("-DANDROID_TOOLCHAIN={0}".format(androidToolchain))

    return res


def DetermineGeneratorArguments(cmakeGeneratorName: str, platformName: str) -> List[str]:
    res = DeterminePlatformArguments(platformName)
    if cmakeGeneratorName != CMakeGeneratorName.VisualStudio2019_X64 and cmakeGeneratorName != CMakeGeneratorName.VisualStudio2022_X64:
        return res
    # for now we always use x64
    res.append("-A")
    res.append("x64")
    return res

def DetermineVSToolsetVersion(log: Log, cmakeGeneratorName: str, platformName: str, userSetVariables: UserSetVariables) -> str:
    buildVariableName = BuildVariables.VS_TOOLSET_VERSION
    userToolsetVersion = userSetVariables.TryGet(buildVariableName)
    if userToolsetVersion is not None:
        log.LogPrint("DetermineVSToolsetVersion using user set '{0}={1}'".format(buildVariableName, userToolsetVersion))
        return userToolsetVersion

    if not CMakeGeneratorName.IsVisualStudio(cmakeGeneratorName):
        if platformName == PlatformNameString.WINDOWS:
            log.LogPrintVerbose(2, "Not a visual studio build so do not know the VSToolsetVersion, a best guess will be made but if necessary set it manually with '--set {0}=version'".format(buildVariableName))
            envVersion = IOUtil.TryGetEnvironmentVariable('VCToolsVersion')
            if envVersion is not None:
                envVersion = envVersion.strip()
                res = envVersion.split('.')
                if len(res) >= 2 and len(res[0]) >= 2 and len(res[1]) > 0:
                    bestGuess = res[0] + res[1][0]
                    log.LogPrintVerbose(2, "The VSToolsetVersion is likely to be '{0}' based on the environment variable VCToolsVersion being '{1}'".format(bestGuess, envVersion))
                    return bestGuess
        return "ErrorUnknownToolsetVersion"
    return CMakeGeneratorName.GetToolsetVersionString(cmakeGeneratorName)

def GetNativeBuildThreadArguments(cmakeGeneratorName: str, numBuildThreads: int) -> List[str]:
    if (cmakeGeneratorName == CMakeGeneratorName.UnixMakeFile or
            cmakeGeneratorName == CMakeGeneratorName.Ninja):
        return ['-j', str(numBuildThreads)]
    elif (cmakeGeneratorName == CMakeGeneratorName.VisualStudio2015_X64 or
          cmakeGeneratorName == CMakeGeneratorName.VisualStudio2017_X64 or
          cmakeGeneratorName == CMakeGeneratorName.VisualStudio2019_X64 or
          cmakeGeneratorName == CMakeGeneratorName.VisualStudio2022_X64):
        return ['/maxcpucount:{0}'.format(numBuildThreads)]
    return []


def GetGeneratorMultiConfigCapabilities(cmakeGeneratorName: str) -> CMakeGeneratorMultiConfigCapability:
    """
    Hardcode some knowledge about certain generators to avoid cmake warnings
    """
    if (cmakeGeneratorName == CMakeGeneratorName.UnixMakeFile or
            cmakeGeneratorName == CMakeGeneratorName.Ninja):
        return CMakeGeneratorMultiConfigCapability.No
    elif (cmakeGeneratorName == CMakeGeneratorName.VisualStudio2015_X64 or
          cmakeGeneratorName == CMakeGeneratorName.VisualStudio2017_X64 or
          cmakeGeneratorName == CMakeGeneratorName.VisualStudio2019_X64 or
          cmakeGeneratorName == CMakeGeneratorName.VisualStudio2022_X64):
        return CMakeGeneratorMultiConfigCapability.Yes
    # Since we dont know, we just return false
    return CMakeGeneratorMultiConfigCapability.Unknown
