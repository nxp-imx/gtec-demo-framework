#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2018 NXP
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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen import PathUtil
from FslBuildGen import IOUtil
from FslBuildGen.BuildConfig.CMakeConfigurationPlatform import CMakeConfigurationPlatform
from FslBuildGen.CMakeUtil import CMakeVersion

class CMakeConfiguration(object):
    def __init__(self, defaultBuildDir: str, defaultInstallPrefix: Optional[str], minimumVersion: CMakeVersion,
                 platforms: List[CMakeConfigurationPlatform], ninjaRecipePackageName: str) -> None:
        super().__init__()

        PathUtil.ValidateIsNormalizedPath(defaultBuildDir, "DefaultBuildDir")
        if defaultInstallPrefix is not None:
            PathUtil.ValidateIsNormalizedPath(defaultInstallPrefix, "DefaultInstallPrefix")

        self.DefaultBuildDir = defaultBuildDir
        self.DefaultInstallPrefix = defaultInstallPrefix
        self.MinimumVersion = minimumVersion
        self.NinjaRecipePackageName = ninjaRecipePackageName;


        platformDict = {}                                                # type: Dict[str,CMakeConfigurationPlatform]
        for entry in platforms:
            platformDict[entry.Name.lower()] = entry
        self.__PlatformDict = platformDict

    def TryGetPlatformConfig(self, platformName: str) -> Optional[CMakeConfigurationPlatform]:
        platformId = platformName.lower()
        return self.__PlatformDict[platformId] if platformId in self.__PlatformDict else None

    def SetAllowFindPackage(self, enabled: bool) -> None:
        for entry in self.__PlatformDict.values():
            entry.AllowFindPackage = enabled
