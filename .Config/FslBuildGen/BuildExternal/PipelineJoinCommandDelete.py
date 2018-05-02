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

from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandDelete
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildExternal.PipelineInfo import PipelineInfo
from FslBuildGen.BuildExternal.PipelineJoinCommand import PipelineJoinCommand


class PipelineJoinCommandDelete(PipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig,
                 sourceCommand: XmlRecipePipelineJoinCommandDelete,
                 pipelineInfo: PipelineInfo, finalDstPath: str) -> None:
        super().__init__(basicConfig, sourceCommand, pipelineInfo, finalDstPath)
        self.__SourceCommand = sourceCommand


    def DoExecute(self) -> None:
        """ Delete a file or directory if it exist """
        sourcePath = self.__SourceCommand.Path
        path = sourcePath
        if  '../' in path or '/..' in path or path.startswith('/') or '\\' in path or path == '..' or len(path) <= 0:
            raise Exception("Invalid path format '{0}'".format(path))
        path = IOUtil.Join(self.FinalDstPath, path)
        if IOUtil.IsDirectory(path):
            self.LogPrint("Deleting directory '{0}'".format(sourcePath))
            IOUtil.SafeRemoveDirectoryTree(path)
        elif IOUtil.IsFile(path):
            self.LogPrint("Deleting file '{0}'".format(sourcePath))
            IOUtil.RemoveFile(path)
