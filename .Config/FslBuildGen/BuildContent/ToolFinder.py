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

from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.Log import Log
from FslBuildGen.PlatformUtil import PlatformUtil


class ToolFinder(object):
    def __init__(self, log: Log) -> None:
        super().__init__()
        self.BuildPlatform = PlatformUtil.DetectBuildPlatformType()
        self.VulkanShaderCompiler = self.GetPlatformDependentExecuteableName("glslangValidator")

    def GetPlatformDependentExecuteableName(self, exeName: str) -> str:
        return PlatformUtil.GetPlatformDependentExecuteableName(exeName, self.BuildPlatform)

    def CheckVulkanShaderCompiler(self) -> None:
        if IOUtil.TryFindExecutable(self.VulkanShaderCompiler) is None:
            raise EnvironmentError("Could not locate the Vulkan shader compiler: '{0}'".format(self.VulkanShaderCompiler))

    def CheckToolCommand(self, toolCommand: str, toolDescription: str) -> None:
        if IOUtil.TryFindExecutable(toolCommand) is None:
            raise EnvironmentError("Could not locate the content builder '{0}' ({1})".format(toolCommand, toolDescription))
