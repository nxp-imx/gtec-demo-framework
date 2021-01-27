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

from typing import Optional
from typing import List
from typing import Set
import subprocess
import shlex
from FslBuildGen import IOUtil
from FslBuildGen.BuildContent.ContentProcessor import ContentProcessor
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildContent.ToolFinder import ToolFinder
from FslBuildGen.Log import Log
from FslBuildGen.ToolConfig import ToolContentBuilder
from FslBuildGen.Xml.XmlToolConfigFile import XmlConfigContentBuilderAddExtension

# $(OutputFileName) = Absolute path to the output file
# $(InputFileName)  = Absolute path to the input file

class BasicContentProcessor(ContentProcessor):
    def __init__(self, log: Log, toolFinder: ToolFinder, contentBuilderConfig: ToolContentBuilder) -> None:
        super().__init__(contentBuilderConfig.Name, contentBuilderConfig.FeatureRequirements, self.__GetExtensionsSet(contentBuilderConfig.DefaultExtensions))
        self.BasedUpon = contentBuilderConfig
        self.DefaultExtensions = contentBuilderConfig.DefaultExtensions
        self.ToolArguments = self.__ProcessToolParameters(contentBuilderConfig.Parameters)
        self.ToolCommand = toolFinder.GetPlatformDependentExecuteableName(contentBuilderConfig.Executable)
        self.ToolDescription = contentBuilderConfig.Description
        self.__ToolFinder = toolFinder


    def __GetExtensionsSet(self, extensions: List[XmlConfigContentBuilderAddExtension]) -> Set[str]:
        return {extension.Name for extension in extensions}


    def __ProcessToolParameters(self, toolParameters: str) -> List[str]:
        res = shlex.split(toolParameters)
        return res


    def GetOutputFileName(self, log: Log, contentOutputPath: str, contentFileRecord: PathRecord, removeExtension: bool = False) -> str:
        outputFilename = super().GetOutputFileName(log, contentOutputPath, contentFileRecord, removeExtension)
        extension = self.__TryFindExtension(contentFileRecord.RelativePath)
        if extension is None or extension.PostfixedOutputExtension is None or len(extension.PostfixedOutputExtension) <= 0:
            return outputFilename
        return outputFilename + extension.PostfixedOutputExtension


    def __TryFindExtension(self, contentFile: str) -> Optional[XmlConfigContentBuilderAddExtension]:
        for entry in self.DefaultExtensions:
            if contentFile.endswith(entry.Name):
                return entry
        return None


    def __GetToolParameterList(self, outputFile: str, inputFile: str) -> List[str]:
        res = []
        for argument in self.ToolArguments:
            if argument == '$(OutputFileName)':
                res.append(outputFile)
            elif argument == '$(InputFileName)':
                res.append(inputFile)
            else:
                res.append(argument)
        return res


    def Process(self, log: Log, configDisableWrite: bool, contentBuildPath: str, contentOutputPath: str, contentFileRecord: PathRecord) -> None:
        # we ask the tool to write to a temporary file so that we can ensure that the output file is only modified
        # if the content was changed
        tmpOutputFileName = self.GetTempFileName(contentBuildPath, contentFileRecord)
        buildCommand = [self.ToolCommand]
        buildCommand += self.__GetToolParameterList(tmpOutputFileName, contentFileRecord.ResolvedPath)

        if configDisableWrite:
            # if write is disabled we do a "tool-command" check directly since the subprocess call can't fail
            # which would normally trigger the check
            self.__ToolFinder.CheckToolCommand(self.ToolCommand, self.ToolDescription)
            return

        outputFileName = self.GetOutputFileName(log, contentOutputPath, contentFileRecord)
        self.EnsureDirectoryExist(configDisableWrite, outputFileName)

        try:
            result = subprocess.call(buildCommand, cwd=contentBuildPath)
            if result != 0:
                self.__ToolFinder.CheckToolCommand(self.ToolCommand, self.ToolDescription)
                raise Exception("{0}: Failed to process file '{1}' ({2})".format(self.ToolCommand, contentFileRecord.ResolvedPath, self.ToolDescription))
            IOUtil.CopySmallFile(tmpOutputFileName, outputFileName)
        except:
            self.__ToolFinder.CheckToolCommand(self.ToolCommand, self.ToolDescription)
            raise
        finally:
            IOUtil.RemoveFile(tmpOutputFileName)
