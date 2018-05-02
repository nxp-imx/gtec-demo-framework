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

import shutil
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildExternal.PipelineInfo import PipelineInfo
from FslBuildGen.BuildExternal.PipelineJoinCommand import PipelineJoinCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandCopy


class PipelineJoinCommandCopy(PipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig,
                 sourceCommand: XmlRecipePipelineJoinCommandCopy,
                 pipelineInfo: PipelineInfo, finalDstPath: str) -> None:
        super().__init__(basicConfig, sourceCommand, pipelineInfo, finalDstPath)
        self.__Overwrite = sourceCommand.Overwrite
        self.__SourceCommand = sourceCommand


    def DoExecute(self) -> None:
        """ Copy a file or directory to the destination """
        # Try to do a lookup
        srcPath = self.TryResolveSrcPathString(self.__SourceCommand.From)
        if srcPath is None:
            srcPath = IOUtil.Join(self.Info.SrcRootPath, self.__SourceCommand.From)

        dstPath = self.TryResolveDstPathString(self.__SourceCommand.To)
        if dstPath is None:
            dstPath = IOUtil.Join(self.Info.DstRootPath, self.__SourceCommand.To) if len(self.__SourceCommand.To) > 0 else self.Info.DstRootPath

        fileExist = IOUtil.Exists(dstPath)
        if not IOUtil.Exists(dstPath) or self.__Overwrite:
            if fileExist:
                self.LogPrint("Copying from '{0}' to '{1}' overwriting the existing file".format(srcPath, dstPath))
            else:
                self.LogPrint("Copying from '{0}' to '{1}'".format(srcPath, dstPath))
            if IOUtil.IsFile(srcPath):
                self._CreateDirectory(IOUtil.GetDirectoryName(dstPath))
                shutil.copy2(srcPath, dstPath)
            elif IOUtil.IsDirectory(srcPath):
                self._CreateDirectory(IOUtil.GetDirectoryName(dstPath))
                shutil.copytree(srcPath, dstPath)
            else:
                raise Exception("Copy source '{0}' not found".format(srcPath))
        else:
            self.LogPrint("Copying from '{0}' to '{1}' skipped as target exist".format(srcPath, dstPath))
