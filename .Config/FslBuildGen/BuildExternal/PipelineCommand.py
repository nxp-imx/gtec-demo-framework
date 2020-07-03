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

from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildExternal.PipelineBasicCommand import PipelineBasicCommand
from FslBuildGen.BuildExternal.PipelineInfo import PipelineInfo
from FslBuildGen.BuildExternal.PipelineJoinCommand import PipelineJoinCommand
from FslBuildGen.BuildExternal.PipelineJoinCommandCopy import PipelineJoinCommandCopy
from FslBuildGen.BuildExternal.PipelineJoinCommandDelete import PipelineJoinCommandDelete
from FslBuildGen.BuildExternal.PipelineJoinCommandGitApply import PipelineJoinCommandGitApply
from FslBuildGen.DataTypes import BuildRecipePipelineCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandCopy
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandDelete
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandGitApply
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandUnpack


class PipelineCommand(PipelineBasicCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: Optional[XmlRecipePipelineCommand], pipelineInfo: PipelineInfo) -> None:
        super().__init__(basicConfig, sourceCommand, pipelineInfo)
        self.Skip = False
        self.AutoCreateDstDirectory = True
        if sourceCommand is None or sourceCommand.OutputPath is None or len(sourceCommand.OutputPath) <= 0:
            self.FinalDstPath = self.Info.CombinedDstRootPath
        else:
            self.FinalDstPath = IOUtil.Join(self.Info.CombinedDstRootPath, sourceCommand.OutputPath)
        self.JoinCommandList = [] if sourceCommand is None else self.__CreateJoinCommandList(sourceCommand.JoinCommandList)


    def Execute(self) -> None:
        if self.AutoCreateDstDirectory:
            self._CreateDirectory(self.Info.DstRootPath)

        if not self.Skip:
            self.DoExecute()
            self.__ExcuteJoin()
        elif self.SourceCommand is not None:
            self.LogPrint("Skipping command '{0}'".format(self.SourceCommand.CommandName))
        else:
            self.LogPrint("Skipping command")


    def DoExecute(self) -> None:
        """ The base implementation does nothing """
        pass


    def __ExcuteJoin(self) -> None:
        for joinCommand in self.JoinCommandList:
            joinCommand.Execute()


    def __CreateJoinCommandList(self, sourceJoinCommandList: List[XmlRecipePipelineJoinCommand]) -> List[PipelineJoinCommand]:
        commandList = []  # type: List[PipelineJoinCommand]
        for sourceJoinCommand in sourceJoinCommandList:
            command = self.__JoinCommand(sourceJoinCommand)
            commandList.append(command)
        return commandList


    def __JoinCommand(self, sourceCommand: XmlRecipePipelineJoinCommand) -> PipelineJoinCommand:
        if sourceCommand.CommandType == BuildRecipePipelineCommand.JoinCopy:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandCopy):
                raise Exception("Invalid command type")
            return PipelineJoinCommandCopy(self.BasicConfig, sourceCommand, self.Info, self.FinalDstPath)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.JoinUnpack:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandUnpack):
                raise Exception("Invalid command type")
            # TODO: implement
            raise Exception("JoinUnpack not implemented")
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.JoinGitApply:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandGitApply):
                raise Exception("Invalid command type")
            return PipelineJoinCommandGitApply(self.BasicConfig, sourceCommand, self.Info, self.FinalDstPath, self.Info.Tasks.TaskGitApply)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.JoinDelete:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandDelete):
                raise Exception("Invalid command type")
            return PipelineJoinCommandDelete(self.BasicConfig, sourceCommand, self.Info, self.FinalDstPath)
        raise Exception("Unsupported join command '{0}'({1}) in '{2}'".format(sourceCommand.CommandName, sourceCommand.CommandType, self.Info.SourceRecipe.FullName))
