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

from enum import Enum
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import VisualStudioVersion

class CMakeGeneratorName(object):
    NotDefined = "NotDefined"
    UnixMakeFile = "Unix Makefiles"
    VisualStudio2015_X64 = "Visual Studio 14 2015 Win64"
    VisualStudio2017_X64 = "Visual Studio 15 2017 Win64"
    VisualStudio2019_X64 = "Visual Studio 16 2019"    # this needs a "-A x64" option to generate x64 code
    VisualStudio2022_X64 = "Visual Studio 17 2022"    # this needs a "-A x64" option to generate x64 code
    Ninja = "Ninja"
    Android = "AndroidDummy"

    @staticmethod
    def FromVisualStudioVersion(version: int) -> str:
        if version == VisualStudioVersion.VS2015:
            return CMakeGeneratorName.VisualStudio2015_X64
        elif version == VisualStudioVersion.VS2017:
            return CMakeGeneratorName.VisualStudio2017_X64
        elif version == VisualStudioVersion.VS2019:
            return CMakeGeneratorName.VisualStudio2019_X64
        elif version == VisualStudioVersion.VS2022:
            return CMakeGeneratorName.VisualStudio2022_X64
        raise Exception("Unsupported visual studio version: {0}".format(version))

    @staticmethod
    def IsVisualStudio(name: str) -> bool:
        return (name == CMakeGeneratorName.VisualStudio2015_X64 or name == CMakeGeneratorName.VisualStudio2017_X64 or
                name == CMakeGeneratorName.VisualStudio2019_X64 or name == CMakeGeneratorName.VisualStudio2022_X64)

    @staticmethod
    def GetToolsetVersionString(name: str) -> str:
        # from
        # https://devblogs.microsoft.com/cppblog/side-by-side-minor-version-msvc-toolsets-in-visual-studio-2019/
        if name == CMakeGeneratorName.VisualStudio2015_X64:
            return "140"
        if name == CMakeGeneratorName.VisualStudio2017_X64:
            return "141"
        if name == CMakeGeneratorName.VisualStudio2019_X64:
            return "142"
        if name == CMakeGeneratorName.VisualStudio2022_X64:
            return "143"
        return ""



class CMakeGeneratorMultiConfigCapability(Enum):
    No = 0
    Yes = 1
    Unknown = 2

class CMakeBuildType(object):
    Release = "RELEASE"
    Debug = "DEBUG"

    @staticmethod
    def FromBuildVariantConfig(buildVariantConfig: BuildVariantConfig) -> str:
        if buildVariantConfig == BuildVariantConfig.Release:
            return CMakeBuildType.Release
        elif buildVariantConfig == BuildVariantConfig.Debug or buildVariantConfig == BuildVariantConfig.Coverage:
            return CMakeBuildType.Debug
        raise Exception("Unsupported BuildVariantConfig '{0}'".format(BuildVariantConfig.ToString(buildVariantConfig)))
