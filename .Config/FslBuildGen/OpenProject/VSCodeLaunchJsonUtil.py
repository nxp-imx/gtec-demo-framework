#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2020 NXP
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

from typing import Dict
import json
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import BuildPlatformType


class LocalStrings(object):
    Version = "version"
    VersionStr = "0.2.0"
    Configurations = "configurations"
    ConfigKeyProgram = "program"
    ConfigKeyCwd = "cwd"
    ConfigKeyName = "name"
    ConfigKeySetupCommands = "setupCommands"

    UbuntuLaunchName = "(FslBuild-Ubuntu) Launch"
    WindowsLaunchName = "(FslBuild-Windows) Launch"

# https://github.com/microsoft/vscode-cmake-tools/blob/master/docs/debug-launch.md#debug-using-a-launchjson-file
class VSCodeLaunchJsonUtil(object):

    @staticmethod
    def TryPatch(jsonFilePath: str, buildPlatformType: BuildPlatformType, executable: str, currentWorkingDirectory: str) -> bool:
        strJson = IOUtil.TryReadFile(jsonFilePath)
        if strJson is not None:
            strJson = VSCodeLaunchJsonUtil.__StripComments(strJson)
            jsonDict = json.loads(strJson)
        else:
            jsonDict = {}
            jsonDict[LocalStrings.Version] = LocalStrings.VersionStr

        if LocalStrings.Version not in jsonDict or jsonDict[LocalStrings.Version] != LocalStrings.VersionStr:
            return False

        configurationDictList = jsonDict[LocalStrings.Configurations] if LocalStrings.Configurations in jsonDict else []

        # Patch/Create windows configuration
        launchWindowsFound = buildPlatformType != BuildPlatformType.Windows  # this basically ensures we only generate the windows launch on windows
        launchUbuntuFound = buildPlatformType == BuildPlatformType.Windows   # this basically ensures we only generate the ubuntu launch on non windows platforms
        for configDict in configurationDictList:
            if configDict[LocalStrings.ConfigKeyName] == LocalStrings.WindowsLaunchName:
                VSCodeLaunchJsonUtil.__PacthConfigurationDict(configDict, executable, currentWorkingDirectory, False)
                launchWindowsFound = True
            elif configDict[LocalStrings.ConfigKeyName] == LocalStrings.UbuntuLaunchName:
                VSCodeLaunchJsonUtil.__PacthConfigurationDict(configDict, executable, currentWorkingDirectory, True)
                launchUbuntuFound = True
        if not launchWindowsFound:
            configDict = VSCodeLaunchJsonUtil.__CreateWindowsLaunchConfigurationDict()
            VSCodeLaunchJsonUtil.__PacthConfigurationDict(configDict, executable, currentWorkingDirectory, False)
            configurationDictList.append(configDict)
        if not launchUbuntuFound:
            configDict = VSCodeLaunchJsonUtil.__CreateUbuntuLaunchConfigurationDict()
            VSCodeLaunchJsonUtil.__PacthConfigurationDict(configDict, executable, currentWorkingDirectory, True)
            configurationDictList.append(configDict)


        jsonDict[LocalStrings.Configurations] = configurationDictList

        jsonText = json.dumps(jsonDict, ensure_ascii=False, sort_keys=True, indent=4)
        IOUtil.WriteFileIfChanged(jsonFilePath, jsonText)
        return True


    @staticmethod
    def __PacthConfigurationDict(configDict: Dict[str, object], executable: str, currentWorkingDirectory: str, isUnix: bool) -> None:
        #configDict[LocalStrings.ConfigKeyProgram] = executable
        configDict[LocalStrings.ConfigKeyProgram] = "${command:cmake.launchTargetPath}"
        configDict[LocalStrings.ConfigKeyCwd] = currentWorkingDirectory

        if isUnix:
            configDict["MIMode"] = "gdb"
            setupCommandsDict = {}   # type: Dict[str, object]
            setupCommandsDict["description"] = "Enable pretty-printing for gdb"
            setupCommandsDict["text"] =  "-enable-pretty-printing"
            setupCommandsDict["ignoreFailures"] = True
            configDict[LocalStrings.ConfigKeySetupCommands] = [setupCommandsDict]


    @staticmethod
    def __StripComments(json: str) -> str:
        lines = json.split("\n")
        finalLines = []
        for line in lines:
            strippedLine = line.strip()
            if not strippedLine.startswith("//"):
                finalLines.append(line)
        return "\n".join(finalLines)

    @staticmethod
    def __CreateWindowsLaunchConfigurationDict() -> Dict[str, object]:
        launchDict = {} # type: Dict[str, object]
        launchDict[LocalStrings.ConfigKeyName] = LocalStrings.WindowsLaunchName
        launchDict["type"] = "cppvsdbg"
        launchDict["request"] = "launch"
        launchDict["args"] = []
        launchDict["stopAtEntry"] = False
        launchDict["environment"] = []
        launchDict["externalConsole"] = False
        return launchDict

    @staticmethod
    def __CreateUbuntuLaunchConfigurationDict() -> Dict[str, object]:
        launchDict = {} # type: Dict[str, object]
        launchDict[LocalStrings.ConfigKeyName] = LocalStrings.UbuntuLaunchName
        launchDict["type"] = "cppdbg"
        launchDict["request"] = "launch"
        launchDict["args"] = []
        launchDict["stopAtEntry"] = False
        launchDict["environment"] = []
        launchDict["externalConsole"] = False
        return launchDict
