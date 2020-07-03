#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2019 NXP
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
from typing import List
from FslBuildGen import PathUtil
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.BuildExternal import CMakeTypes
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Version import Version

class GeneratorCMakeConfig(object):
    """
    The cmake context for the current platform
    """
    def __init__(self, toolVersion: Version, platformName: str, buildVariantConfig: BuildVariantConfig, buildDir: str, buildDirSetByUser: bool,
                 checkDir: str, generatorName: str, installPrefix: Optional[str], cmakeVersion: CMakeVersion,
                 additionalGlobalConfigArguments: List[str], additionalAppConfigArguments: List[str], allowFindPackage: bool) -> None:
        super().__init__()

        PathUtil.ValidateIsNormalizedPath(buildDir, "BuildDir")
        if installPrefix is not None:
            PathUtil.ValidateIsNormalizedPath(installPrefix, "DefaultInstallPrefix")

        finalGeneratorName = CMakeTypes.DetermineFinalCMakeGenerator(generatorName)
        # its important that we use the  generatorName for the GetCompilerShortIdFromGeneratorName to get the proper android name
        generatorShortName = CMakeTypes.GetCompilerShortIdFromGeneratorName(generatorName)

        # Check if we should use a 'build variant temp dir'
        if not buildDirSetByUser:
            buildDir = IOUtil.Join(buildDir, generatorShortName)
            if CMakeTypes.GetGeneratorMultiConfigCapabilities(finalGeneratorName) == CMakeTypes.CMakeGeneratorMultiConfigCapability.No:
                buildDir = IOUtil.Join(buildDir, BuildVariantConfig.ToString(buildVariantConfig))

        self.BuildDir = buildDir
        # If this is true the user specified the directory
        self.BuildDirSetByUser = buildDirSetByUser
        self.CheckDir = checkDir

        # the active build variant
        self.BuildVariantConfig = buildVariantConfig
        self.GeneratorName = generatorName
        self.InstallPrefix = installPrefix
        self.CMakeVersion = cmakeVersion

        self.CMakeCommand = CMakeTypes.DetermineCMakeCommand(platformName)
        # The tool arguments
        self.CMakeInternalArguments = CMakeTypes.DetermineGeneratorArguments(finalGeneratorName, platformName)

        # This contains only the user arguments for both recipe and apps
        self.CMakeConfigUserGlobalArguments = additionalGlobalConfigArguments
        self.CMakeConfigUserAppArguments = additionalAppConfigArguments

        # The combined arguments that should be supplied to cmake
        self.CMakeConfigRecipeArguments = self.CMakeInternalArguments + additionalGlobalConfigArguments

        # The combined arguments that should be supplied to cmake
        self.CMakeConfigAppArguments = self.CMakeInternalArguments + additionalGlobalConfigArguments + additionalAppConfigArguments

        self.CMakeFinalGeneratorName = finalGeneratorName
        self.GeneratorShortName = generatorShortName
        self.GeneratorRecipeShortName = "{0}_{1}".format(generatorShortName, toolVersion.ToMajorMinorString().replace('.','_'))
        self.AllowFindPackage = allowFindPackage
