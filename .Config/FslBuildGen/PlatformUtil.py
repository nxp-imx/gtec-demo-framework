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

import os
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.PackageConfig import PlatformNameString
#from FslBuildGen import PluginSharedValues

__g_posixPlatforms = [PlatformNameString.ANDROID, PlatformNameString.UBUNTU, PlatformNameString.YOCTO, PlatformNameString.QNX]
__g_ntPlatforms = [PlatformNameString.ANDROID, PlatformNameString.WINDOWS]


def AddExtraGenerators(platform: str) -> None:
    if DetectBuildPlatformType() != BuildPlatformType.Windows:
        return
    __g_ntPlatforms.append(platform)


def DetectBuildPlatform() -> str:
    sdkPlatformName = IOUtil.TryGetEnvironmentVariable('FSL_PLATFORM_NAME')
    if os.name == 'posix':
        if not sdkPlatformName:
            raise EnvironmentError("Please make sure that the environment variable FSL_PLATFORM_NAME is set")
        if sdkPlatformName not in __g_posixPlatforms:
            raise EnvironmentError("Please make sure that the environment variable FSL_PLATFORM_NAME is set to one of these {0}".format(__g_posixPlatforms))
    elif os.name == 'nt':
        if not sdkPlatformName:
            return PlatformNameString.WINDOWS
        if sdkPlatformName not in __g_ntPlatforms:
            raise EnvironmentError("Please make sure that the environment variable FSL_PLATFORM_NAME is set to one of these {0}".format(__g_ntPlatforms))
    else:
        raise EnvironmentError("Unsupported build environment")
    return sdkPlatformName


def DetectBuildPlatformType() -> int:
    if os.name == 'posix':
        return BuildPlatformType.Unix
    elif os.name == 'nt':
        return BuildPlatformType.Windows
    return BuildPlatformType.Unknown


def TryCheckBuildPlatform(platform: str) -> bool:
    buildPlatformType = DetectBuildPlatformType()
    if buildPlatformType == BuildPlatformType.Unix and platform in __g_posixPlatforms:
        return True
    elif buildPlatformType == BuildPlatformType.Windows and platform in __g_ntPlatforms:
        return True
    return False


def CheckBuildPlatform(platform: str) -> None:
    if TryCheckBuildPlatform(platform):
        return
    raise EnvironmentError("Unsupported build environment for '{0}'".format(platform))
