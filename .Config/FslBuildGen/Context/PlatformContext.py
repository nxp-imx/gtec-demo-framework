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

from typing import Optional
from FslBuildGen.Log import Log
from FslBuildGen.BuildConfig.BuildVariables import BuildVariables
from FslBuildGen.BuildExternal.RecipeBuilderSetup import RecipeBuilderSetup
from FslBuildGen.BuildExternal.RecipePathBuilder import RecipePathBuilder
from FslBuildGen.Context.Context import Context
from FslBuildGen.Context.VariableContext import VariableContext
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.ErrorHelpManager import ErrorHelpManager
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Location.PathBuilder import PathBuilder
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Vars.VariableEnvironment import VariableEnvironment
from FslBuildGen.Vars.VariableProcessor import VariableProcessor


class PlatformContext(Context):
    def __init__(self, log: Log, errorHelpManager: ErrorHelpManager, platformName: str, generatorName: str,
                 generatorInfo: GeneratorInfo, cmakeConfig: GeneratorCMakeConfig, recipeBuilderSetup: Optional[RecipeBuilderSetup]) -> None:
        super().__init__(log)

        self.ErrorHelpManager = errorHelpManager
        self.HostPlatformName = self.__DetermineHostPlatformName(platformName)
        self.PlatformName = platformName # type: str
        self.GeneratorName = generatorName # type: str
        self.GeneratorInfo = generatorInfo
        self.CMakeConfig = cmakeConfig
        self.VariableProcessor = VariableProcessor(log, self.__CreateVariables(log, cmakeConfig, generatorInfo.VariableContext))

        self.PathBuilder = PathBuilder(log, self.VariableProcessor, platformName)
        self.RecipePathBuilder = RecipePathBuilder(log, self.VariableProcessor, recipeBuilderSetup, platformName, cmakeConfig)


    def __DetermineHostPlatformName(self, platformName: str) -> str:
        if platformName != PlatformNameString.ANDROID:
            return platformName
        if PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows:
            return PlatformNameString.WINDOWS
        return PlatformNameString.UBUNTU

    def __CreateVariables(Self, log: Log, cmakeConfig: GeneratorCMakeConfig, variableContext: VariableContext) -> VariableEnvironment:
        variables = VariableEnvironment(log)
        variables.Set(BuildVariables.VS_TOOLSET_VERSION, cmakeConfig.VsToolsetVersionStr)
        return variables

