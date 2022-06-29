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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen.BuildConfig.UserSetVariables import UserSetVariables
from FslBuildGen.DataTypes import BuildThreads
from FslBuildGen.DataTypes import GeneratorType
from FslBuildGen.PackageConfig import PlatformNameString
#from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Tool.UserCMakeConfig import UserCMakeConfig

class DefaultValue(object):
    UseFeatures = "*"
    RequireFeatures = "*"
    UseExtensions = "*"
    RequireExtensions = "*"
    Recipes = "*"

    AllowDevelopmentPlugins = False
    ForceClaimInstallArea = False
    PlatformName = PlatformNameString.INVALID
    Generator = GeneratorType.Default
    RemainingArgs = []  # type: List[str]
    BuildThreads = BuildThreads.Auto  # Not set
    CMakeBuildDir = None # type: Optional[str]
    CMakeBuildDirId = None # type: Optional[int]
    CMakeInstallPrefix = None # type: Optional[str]
    CMakeGeneratorName = None # type: Optional[str]
    CMakeConfigArgs = None # type: Optional[str]               # applied to app cmake config only
    CMakeConfigGlobalArgs = None # type: Optional[str]         # applied to both recipe and app config.
    CMakeAllowFindPackage = None # type: Optional[bool]        #


class ToolAppConfig(object):
    def __init__(self) -> None:
        self.AllowDevelopmentPlugins = DefaultValue.AllowDevelopmentPlugins
        self.BuildThreads = DefaultValue.BuildThreads
        self.DefaultPlatformName = DefaultValue.PlatformName
        self.ForceClaimInstallArea = DefaultValue.ForceClaimInstallArea
        self.PlatformName = DefaultValue.PlatformName
        self.Generator = DefaultValue.Generator
        self.RemainingArgs = DefaultValue.RemainingArgs
        self.VSVersion = None  # type: Optional[int]

        self.BuildPackageFilters = PackageFilters()
        self.BuildVariantsDict = {}  # type: Dict[str, str]
        self.Recursive = False

        self.CMakeBuildDir = DefaultValue.CMakeBuildDir
        self.CMakeBuildDirId = DefaultValue.CMakeBuildDirId
        self.CMakeInstallPrefix = DefaultValue.CMakeInstallPrefix
        self.CMakeGeneratorName = DefaultValue.CMakeGeneratorName
        self.CMakeConfigArgs = DefaultValue.CMakeConfigArgs
        self.CMakeConfigGlobalArgs = DefaultValue.CMakeConfigGlobalArgs
        self.CMakeAllowFindPackage = DefaultValue.CMakeAllowFindPackage
        self.UserSetVariables = UserSetVariables(dict())


    def SetToolAppConfigValues(self, toolAppConfig: 'ToolAppConfig') -> None:
        if toolAppConfig.PlatformName is None:
            raise Exception("Platform name can not be None")

        self.AllowDevelopmentPlugins = toolAppConfig.AllowDevelopmentPlugins
        self.BuildThreads = toolAppConfig.BuildThreads
        self.DefaultPlatformName = toolAppConfig.DefaultPlatformName
        self.ForceClaimInstallArea = toolAppConfig.ForceClaimInstallArea
        self.PlatformName = toolAppConfig.PlatformName
        self.Generator = toolAppConfig.Generator
        self.RemainingArgs = toolAppConfig.RemainingArgs
        self.VSVersion = toolAppConfig.VSVersion

        self.BuildPackageFilters = toolAppConfig.BuildPackageFilters
        self.BuildVariantsDict = toolAppConfig.BuildVariantsDict
        self.Recursive = toolAppConfig.Recursive

        self.CMakeBuildDir = toolAppConfig.CMakeBuildDir
        self.CMakeBuildDirId = toolAppConfig.CMakeBuildDirId
        self.CMakeInstallPrefix = toolAppConfig.CMakeInstallPrefix
        self.CMakeGeneratorName = toolAppConfig.CMakeGeneratorName
        self.CMakeConfigArgs = toolAppConfig.CMakeConfigArgs
        self.CMakeConfigGlobalArgs = toolAppConfig.CMakeConfigGlobalArgs
        self.CMakeAllowFindPackage = toolAppConfig.CMakeAllowFindPackage
        self.UserSetVariables = toolAppConfig.UserSetVariables

    def GetUserCMakeConfig(self) -> UserCMakeConfig:
        return UserCMakeConfig(self.CMakeBuildDir, self.CMakeBuildDirId, self.CMakeGeneratorName, self.CMakeInstallPrefix, self.CMakeConfigArgs,
                               self.CMakeConfigGlobalArgs, self.CMakeAllowFindPackage)
