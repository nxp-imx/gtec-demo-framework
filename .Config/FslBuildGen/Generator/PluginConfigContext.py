#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen.BuildConfig.CMakeConfiguration import CMakeConfiguration
from FslBuildGen.BuildConfig.UserSetVariables import UserSetVariables
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import GeneratorType
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Tool.UserCMakeConfig import UserCMakeConfig

class PluginConfigContext(object):
    def __init__(self) -> None:
        super().__init__()

    def GetGeneratorPlugins(self) -> List[GeneratorPlugin]:
        raise Exception("GetGeneratorPlugins not implemented")

    def GetGeneratorPluginById(self, pluginId: str, generatorType: GeneratorType, buildVariantConfig: BuildVariantConfig,
                               userSetVariables: UserSetVariables, defaultPackageLanguage: PackageLanguage, cmakeConfiguration: CMakeConfiguration,
                               userCMakeConfig: Optional[UserCMakeConfig], isCheckMode: bool) -> GeneratorPlugin:
        raise Exception("GetGeneratorPluginById not implemented")

    def EnableGraph(self) -> None:
        raise Exception("EnableGraph not implemented")

    def SetVSVersion(self, vsVersion: str) -> None:
        raise Exception("SetVSVersion not implemented")

    def SetLegacyGeneratorType(self, legacyGeneratorType: str) -> None:
        raise Exception("SetLegacyGeneratorType not implemented")

