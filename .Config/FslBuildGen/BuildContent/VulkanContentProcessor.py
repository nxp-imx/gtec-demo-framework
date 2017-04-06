#!/usr/bin/env python

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

from FslBuildGen.BuildContent.ContentProcessor import ContentProcessor
from FslBuildGen import IOUtil
import subprocess

# .vert   for a vertex shader
# .tesc   for a tessellation control shader
# .tese   for a tessellation evaluation shader
# .geom   for a geometry shader
# .frag   for a fragment shader
# .comp   for a compute shader
g_vulkanFileExtensionSet = {"vert", "tesc", "tese", "geom", "frag", "comp"}
g_vulkanFeatureName = "Vulkan"

class VulkanContentProcessor(ContentProcessor):
    def __init__(self):
        super(VulkanContentProcessor, self).__init__("VulkanContentProcessor", g_vulkanFeatureName, g_vulkanFileExtensionSet)


    def GetOutputFileName(self, config, contentBuildPath, contentOutputPath, contentFile):
        outputFilename = super(VulkanContentProcessor, self).GetOutputFileName(config, contentBuildPath, contentOutputPath, contentFile, False)
        return outputFilename + ".spv"



    def Process(self, config, contentBuildPath, contentOutputPath, contentFile, toolFinder):
        # we ask the tool to write to a temporary file so that we can ensure that the output file is only modified
        # if the content was changed
        tmpOutputFileName = self.GetTempFileName(contentBuildPath, contentFile)
        buildCommand = [toolFinder.VulkanShaderCompiler, '-t', '-o', tmpOutputFileName, '-V', contentFile ]
        #if config.Verbosity == 0:
        #    buildCommand += ['-s']

        if config.DisableWrite:
            # if write is disabled we do a vulkan shader compiler check directly since the subprocess call can't fail
            # which would normally trigger the check
            toolFinder.CheckVulkanShaderCompiler()
            return

        outputFileName = self.GetOutputFileName(config, contentBuildPath, contentOutputPath, contentFile)
        self.EnsureDirectoryExist(config, outputFileName)

        try:
            result = subprocess.call(buildCommand, cwd=contentBuildPath)
            if result != 0:
                toolFinder.CheckVulkanShaderCompiler()
                raise Exception("VulkanContentProcessor: Failed to compile file '%s' to SPIR-V binary" % (contentFile));
            IOUtil.CopySmallFile(tmpOutputFileName, outputFileName)
        except:
            toolFinder.CheckVulkanShaderCompiler()
            raise
        finally:
            IOUtil.RemoveFile(tmpOutputFileName)
            pass
