#!/usr/bin/env python3

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

from typing import Any
from typing import List
from typing import Optional
from typing import Dict
import argparse
import json
from FslBuildGen import IOUtil
#from FslBuildGen import Main as MainFlow
#from FslBuildGen.Log import Log
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig

# A very simple tool to write a basic JSON file from the command line

class DefaultValue(object):
    Save = None

class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super().__init__()
        self.Append = []  # type: List[str]
        self.Save = DefaultValue.Save


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()

class ToolFlowDumpEnv(AToolAppFlow):
    #def __init__(self, toolAppContext: ToolAppContext) -> None:
    #    super().__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function

        localToolConfig = LocalToolConfig()
        localToolConfig.Append = args.append
        localToolConfig.Save = args.save

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        self.Log.PrintTitle()

        envDict = {}  # type: Dict[str,str]
        if localToolConfig.Append is None:
            raise Exception("No arguments supplied")

        for entry in localToolConfig.Append:
            tokens = entry.split('=')
            if len(tokens) != 2:
                raise Exception("appended argument '{0}' was not in the correct key=value format".format(entry))
            envDict[tokens[0]] = tokens[1]

        jsonText = json.dumps(envDict, ensure_ascii=False, sort_keys=True, indent=2)

        if localToolConfig.Save:
            IOUtil.WriteFileIfChanged(localToolConfig.Save, jsonText)
        else:
            print(jsonText)



class ToolAppFlowFactory(AToolAppFlowFactory):
    #def __init__(self) -> None:
    #    pass

    def GetTitle(self) -> str:
        return 'FslBuildSave'

    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = False
        argConfig.ProcessRemainingArgs = False
        argConfig.SupportBuildTime = False
        argConfig.AddBuildFiltering = False
        argConfig.AddBuildThreads = False
        argConfig.AddBuildVariants = False
        return argConfig

    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        parser.add_argument('--append', action='append', help='Append a key value pair to the json dictionary')
        parser.add_argument('--save', default=DefaultValue.Save, help='The json filename to save the dictionary to')

    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowDumpEnv(toolAppContext)
