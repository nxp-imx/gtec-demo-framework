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
from FslBuildGen.PackageConfig import PlatformNameIdString
#from FslBuildGen import PluginSharedValues

g_posixPlatforms = [PlatformNameString.ANDROID, PlatformNameString.UBUNTU, PlatformNameString.YOCTO, PlatformNameString.FREERTOS, PlatformNameString.QNX]
g_ntPlatforms = [PlatformNameString.ANDROID, PlatformNameString.WINDOWS, PlatformNameString.FREERTOS]

g_posixPlatformIds = [name.lower() for name in g_posixPlatforms]
g_ntPlatformIds = [name.lower() for name in g_ntPlatforms]

class PlatformUtil(object):
    @staticmethod
    def AddExtraGenerators(platform: str) -> None:
        if PlatformUtil.DetectBuildPlatformType() != BuildPlatformType.Windows:
            return
        global g_ntPlatforms
        g_ntPlatforms.append(platform)

    @staticmethod
    def DetectBuildPlatform() -> str:
        sdkPlatformName = IOUtil.TryGetEnvironmentVariable('FSL_PLATFORM_NAME')
        if os.name == 'posix':
            if not sdkPlatformName:
                raise EnvironmentError("Please make sure that the environment variable FSL_PLATFORM_NAME is set")
            if sdkPlatformName not in g_posixPlatforms:
                raise EnvironmentError("Please make sure that the environment variable FSL_PLATFORM_NAME is set to one of these {0}".format(g_posixPlatforms))
        elif os.name == 'nt':
            if not sdkPlatformName:
                return PlatformNameString.WINDOWS
            if sdkPlatformName not in g_ntPlatforms:
                raise EnvironmentError("Please make sure that the environment variable FSL_PLATFORM_NAME is set to one of these {0}".format(g_ntPlatforms))
        else:
            raise EnvironmentError("Unsupported build environment")
        return sdkPlatformName


    @staticmethod
    def DetectBuildPlatformType() -> BuildPlatformType:
        if os.name == 'posix':
            return BuildPlatformType.Unix
        elif os.name == 'nt':
            return BuildPlatformType.Windows
        return BuildPlatformType.Unknown


    @staticmethod
    def TryCheckBuildPlatform(platform: str) -> bool:
        platformId = platform.lower()
        buildPlatformType = PlatformUtil.DetectBuildPlatformType()
        if buildPlatformType == BuildPlatformType.Unix and platformId in g_posixPlatformIds:
            return True
        elif buildPlatformType == BuildPlatformType.Windows and platformId in g_ntPlatformIds:
            return True
        return False


    @staticmethod
    def CheckBuildPlatform(platform: str) -> None:
        if PlatformUtil.TryCheckBuildPlatform(platform):
            return
        raise EnvironmentError("Unsupported build environment for '{0}'".format(platform))


    @staticmethod
    def GetPlatformDependentExecuteableName(exeName: str, buildPlatformType: BuildPlatformType) -> str:
        exeName = exeName if not exeName.lower().endswith(".exe") else IOUtil.GetFileNameWithoutExtension(exeName)
        if buildPlatformType == BuildPlatformType.Windows:
            return exeName + ".exe"
        return exeName

    @staticmethod
    def GetExecutableName(exeName: str, platformName: str) -> str:
        exeName = IOUtil.GetFileNameWithoutExtension(exeName)
        platformId = platformName.lower()
        if platformId == PlatformNameIdString.WINDOWS:
            return exeName + '.exe'
        return exeName
