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
from FslBuildGen import PackageConfig
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import VisualStudioVersion
from FslBuildGen.Generator.GeneratorPluginBase import GeneratorPluginBase
from FslBuildGen.PlatformUtil import PlatformUtil

class CMakeGeneratorName(object):
    UnixMakeFile = "Unix Makefiles"
    VisualStudio2015_X64 = "Visual Studio 14 2015 Win64"
    VisualStudio2017_X64 = "Visual Studio 15 2017 Win64"
    Ninja = "Ninja"
    Android = "AndroidDummy"

    @staticmethod
    def FromVisualStudioVersion(version: int) -> str:
        if version == VisualStudioVersion.VS2015:
            return CMakeGeneratorName.VisualStudio2015_X64
        elif version == VisualStudioVersion.VS2017:
            return CMakeGeneratorName.VisualStudio2017_X64
        raise Exception("Unsupported visual studio version: {0}".format(version))


class CMakeBuildType(object):
    Release = "RELEASE"
    Debug = "DEBUG"

    @staticmethod
    def FromBuildVariantConfig(buildVariantConfig: int) -> str:
        if BuildVariantConfig.Release:
            return CMakeBuildType.Release
        elif BuildVariantConfig.Debug:
            return CMakeBuildType.Debug
        raise Exception("Unsupported BuildVariantConfig '{0}'".format(BuildVariantConfig.ToString(buildVariantConfig)))


def DetermineCMakeCommand(platformName: str) -> str:
    return PlatformUtil.GetExecutableName('cmake', platformName)

def TryDetermineCMakeGenerator(platform: GeneratorPluginBase) -> Optional[str]:
    if platform.Name == PackageConfig.PlatformNameString.UBUNTU:
        return CMakeGeneratorName.UnixMakeFile
    elif platform.Name == PackageConfig.PlatformNameString.YOCTO:
        return CMakeGeneratorName.UnixMakeFile
    elif platform.Name == PackageConfig.PlatformNameString.QNX:
        return CMakeGeneratorName.UnixMakeFile
    elif platform.Name == PackageConfig.PlatformNameString.WINDOWS:
        return CMakeGeneratorName.FromVisualStudioVersion(platform.ToolVersion)
    elif platform.Name == PackageConfig.PlatformNameString.ANDROID:
        return CMakeGeneratorName.Android
    return None

    #if platform.Name == PackageConfig.PlatformNameString.ANDROID:
    #    return "NotSupported"
    #elif platform.Name == PackageConfig.PlatformNameString.YOCTO:
    #    return "NotSupported"
    #elif platform.Name == PackageConfig.PlatformNameString.QNX:
    #    return "NotSupported"


def DetermineFinalCMakeGenerator(generatorName: str) -> str:
    if generatorName != CMakeGeneratorName.Android:
        return generatorName
    if PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows:
        return CMakeGeneratorName.Ninja
    return CMakeGeneratorName.UnixMakeFile


def DetermineCMakeGenerator(platform: GeneratorPluginBase) -> str:
    result = TryDetermineCMakeGenerator(platform)
    if not result is None:
        return result
    raise Exception("CMake generator name could not be determined for this platform '{0}".format(platform.Name))


def TryGetCompilerShortIdFromGeneratorName(generatorName: str) -> Optional[str]:
    if generatorName == CMakeGeneratorName.UnixMakeFile:
        return "Make"
    elif generatorName == CMakeGeneratorName.VisualStudio2015_X64:
        return "VS2015_X64"
    elif generatorName == CMakeGeneratorName.VisualStudio2017_X64:
        return "VS2017_X64"
    elif generatorName == CMakeGeneratorName.VisualStudio2017_X64:
        return "VS2017_X64"
    elif generatorName == CMakeGeneratorName.Android:
        # For android we utilize a combination of the SDK and NDK version for the unique 'toolchain' name
        id = AndroidUtil.GetSDKNDKId()
        sdkVersion = AndroidUtil.GetMinimumSDKVersion()
        return "V{1}{0}".format(id, sdkVersion)
    return None


def GetCompilerShortIdFromGeneratorName(generatorName: str) -> str:
    shortName = TryGetCompilerShortIdFromGeneratorName(generatorName)
    if not shortName is None:
        return shortName
    raise Exception("No short name has been defined for generator '{0}'".format(generatorName))


def DeterminePlatformArguments(platformName: str) -> List[str]:
    res = [] # type: List[str]
    if platformName != PackageConfig.PlatformNameString.ANDROID:
        return res

    androidToolchain = "clang"
    androidStlType = "c++_shared"
    res.append("-DCMAKE_SYSTEM_NAME=Android")
    res.append("-DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION={0}".format(androidToolchain))
    res.append("-DCMAKE_ANDROID_STL_TYPE={0}".format(androidStlType))
    return res

