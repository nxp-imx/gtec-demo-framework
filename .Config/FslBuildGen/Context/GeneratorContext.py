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
from typing import Optional
from FslBuildGen import IOUtil
#from FslBuildGen.BuildExternal import CMakeTypes
from FslBuildGen.BuildExternal.RecipeBuilderSetup import RecipeBuilderSetup
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.ErrorHelpManager import ErrorHelpManager
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
#from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.RecipeFilterManager import RecipeFilterManager
from FslBuildGen.ToolConfigExperimental import ToolConfigExperimental

class GeneratorContext(PlatformContext):
    def __init__(self, log: Log, errorHelpManager: ErrorHelpManager, recipeFilterManager: RecipeFilterManager,
                 experimental: Optional[ToolConfigExperimental], generator: GeneratorPlugin) -> None:
        if generator.CMakeConfig is None:
            raise Exception("Invalid generator")

        recipeBuilderSetup = None
        allowDownloads = False
        if experimental is not None:
            targetLocation = ResolvedPath(experimental.DefaultThirdPartyInstallDirectory.DynamicName,
                                          experimental.DefaultThirdPartyInstallDirectory.ResolvedPath)
            installReadonlySource = experimental.DefaultThirdPartyInstallReadonlyCacheDirectory
            readonlyCachePath = None if installReadonlySource is None else installReadonlySource.ResolvedPath  # type: Optional[str]
            recipeBuilderSetup = RecipeBuilderSetup(targetLocation, readonlyCachePath)
            if IOUtil.TryGetEnvironmentVariable(experimental.DisableDownloadEnv) is not None:
                allowDownloads = False
                log.LogPrint("Downloads disabled since the environment variable {0} was defined".format(experimental.DisableDownloadEnv))
            elif experimental.AllowDownloads:
                allowDownloads = True
            else:
                log.LogPrint("Downloads disabled since the project has it disabled by default")

        validVariableDict = {}      # type: Dict[str, object]
        validVariableDict['PlatformName'] = generator.PlatformName
        validVariableDict['IsCMakeBuild'] = generator.IsCMake

        generatorInfo = GeneratorInfo(generator.IsCMake, generator.CMakeConfig.AllowFindPackage, validVariableDict)
        super().__init__(log, errorHelpManager, generator.PlatformName, generator.PlatformName, generatorInfo, generator.CMakeConfig,
                         recipeBuilderSetup)

        #allowDownload=True, disableDownloadEnv=None

        self.Log = log
        self.Generator = generator
        # for now the generator is also the platform and the existing code use that name so maintain compatibility for now
        self.Platform = generator
        self.AllowDownloads = allowDownloads
        self.RecipeFilterManager = recipeFilterManager
