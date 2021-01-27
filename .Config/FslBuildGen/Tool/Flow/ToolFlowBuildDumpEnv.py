#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2017 NXP
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
import os
#from FslBuildGen import Main as MainFlow
from FslBuildGen import ParseUtil
#from FslBuildGen.Log import Log
from FslBuildGen.Build.CaptureEnvironmentBlock import CaptureEnvironmentBlock
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig

class DefaultValue(object):
    Env = None # type: Optional[List[str]]
    Enclose = False
    AllowNotFound = False

class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super().__init__()
        self.Env = DefaultValue.Env
        self.Enclose = DefaultValue.Enclose
        self.AllowNotFound = DefaultValue.AllowNotFound


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowDumpEnv(AToolAppFlow):
    #def __init__(self, toolAppContext: ToolAppContext) -> None:
    #    super().__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function

        localToolConfig = LocalToolConfig()

        envList = None   # type: Optional[List[str]]
        if args.Env is not None:
            envList = ParseUtil.ParseList(args.Env, "environment variable list", False)

        localToolConfig.Env = envList
        localToolConfig.AllowNotFound = args.AllowNotFound
        localToolConfig.Enclose = args.Enclose

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        self.Log.PrintTitle()

        envDict = {}  # type: Dict[str,str]
        if localToolConfig.Env is None:
            for key, value in os.environ.items():
                envDict[key] = value
        else:
            for entry in localToolConfig.Env:
                if entry not in os.environ:
                    strError = "Could not locate environment variable: {0}".format(entry)
                    if not localToolConfig.AllowNotFound:
                        raise Exception(strError)
                    else:
                        self.Log.DoPrintWarning(strError)
                else:
                    envDict[entry] = os.environ[entry]

        jsonText = json.dumps(envDict, ensure_ascii=False, sort_keys=True, indent=2)

        if localToolConfig.Enclose:
            print(CaptureEnvironmentBlock.Begin)

        print(jsonText)

        if localToolConfig.Enclose:
            print(CaptureEnvironmentBlock.End)

class ToolAppFlowFactory(AToolAppFlowFactory):
    #def __init__(self) -> None:
    #    pass


    def GetTitle(self) -> str:
        return 'FslBuildDumpEnv'


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
        parser.add_argument('--Env', default=DefaultValue.Env, help='Dump the given environment variables')
        parser.add_argument('--Enclose', action='store_true', help='Add begin and end statements around the json')
        parser.add_argument('--AllowNotFound', action='store_true', help='Not finding a environment variable does not cause a error')

    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowDumpEnv(toolAppContext)
