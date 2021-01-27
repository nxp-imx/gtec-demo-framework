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

from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.BuildExternal.PipelineInfo import PipelineInfo
from FslBuildGen.Log import Log
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineBasicCommand

class PipelineBasicCommand(object):
    def __init__(self, log: Log,
                 sourceCommand: Optional[XmlRecipePipelineBasicCommand],
                 pipelineInfo: PipelineInfo, finalDstPath: Optional[str] = None) -> None:
        super().__init__()
        self.Log = log
        self.SourceCommand = sourceCommand
        self.Info = pipelineInfo
        self.FinalDstPath = finalDstPath if not finalDstPath is None else self.Info.CombinedDstRootPath


    def LogPrint(self, message: str) -> None:
        self.Log.LogPrint(message)


    def _CreateDirectory(self, path: str) -> None:
        if not IOUtil.IsDirectory(path):
            self.LogPrint("- Creating '{0}' as it was missing".format(path))
            IOUtil.SafeMakeDirs(path)


    def TryResolveSrcPathString(self, path: str) -> Optional[str]:
        if path.startswith('{RECIPE_PATH}/'):
            srcPath = path[len('{RECIPE_PATH}/'):]
            if self.Info.RecipeAbsolutePath is None:
                raise Exception("{RECIPE_PATH} could not be resolved as RecipeAbsolutePath was None")
            return IOUtil.Join(self.Info.RecipeAbsolutePath, srcPath)
        elif path.startswith('{DST_PATH}/'):
            srcPath = path[len('{DST_PATH}/'):]
            return IOUtil.Join(self.Info.DstRootPath, srcPath)
        elif path.startswith('{SRC_PATH}/'):
            srcPath = path[len('{SRC_PATH}/'):]
            return IOUtil.Join(self.Info.SrcRootPath, srcPath)
        elif path.startswith('{OUTPUT_PATH}/'):
            srcPath = path[len('{OUTPUT_PATH}/'):]
            return IOUtil.Join(self.FinalDstPath, srcPath)
        return None


    def TryResolveDstPathString(self, path: str) -> Optional[str]:
        if path.startswith('{DST_PATH}/'):
            srcPath = path[len('{DST_PATH}/'):]
            return IOUtil.Join(self.Info.DstRootPath, srcPath)
        elif path.startswith('{OUTPUT_PATH}/'):
            srcPath = path[len('{OUTPUT_PATH}/'):]
            return IOUtil.Join(self.FinalDstPath, srcPath)
        return None
