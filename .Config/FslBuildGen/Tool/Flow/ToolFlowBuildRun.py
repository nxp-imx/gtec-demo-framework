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
from typing import Dict
from typing import List
from typing import Optional
import argparse
import subprocess
import sys
from FslBuildGen import IOUtil
from FslBuildGen import PackageListUtil
from FslBuildGen.Generator import PluginConfig
from FslBuildGen import ParseUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator.Report.ReportVariableFormatter import ReportVariableFormatter
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Info.AppInfo import AppInfoPackage
from FslBuildGen.Info.AppInfoLoader import AppInfoLoader
from FslBuildGen.Info.AppInfoRequirementTree import AppInfoRequirementTree
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.Flow import ToolFlowBuild
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppConfig import DefaultValue as ToolAppConfigDefaultValue
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig

g_InfoFilename = ".AppInfo.json"

class AppInfoChoice(object):
    File = 0
    Recursive = 1

    @staticmethod
    def GetAll() -> List[int]:
        return [AppInfoChoice.File, AppInfoChoice.Recursive]

    @staticmethod
    def TryToString(value: int) -> Optional[str]:
        if value == AppInfoChoice.File:
            return 'file'
        elif value == AppInfoChoice.Recursive:
            return 'recursive'
        return None

    @staticmethod
    def ToString(value: int) -> str:
        result = AppInfoChoice.TryToString(value)
        if result is not None:
            return result
        raise Exception("Unknown AppInfoChoice: {0}".format(value))

    @staticmethod
    def TryFromString(value: str) -> Optional[int]:
        if value == 'file':
            return AppInfoChoice.File
        elif value == 'recursive':
            return AppInfoChoice.Recursive
        return None

    @staticmethod
    def FromString(value: str) -> int:
        result = AppInfoChoice.TryFromString(value)
        if result is not None:
            return result
        raise Exception("Unknown AppInfoChoice: {0}".format(value))


class DefaultValue(object):
    PackageConfigurationType = PluginSharedValues.TYPE_DEFAULT
    AppInfo = None  # type: Optional[str]


class DefaultValueStandalone(object):
    AppInfo = AppInfoChoice.ToString(AppInfoChoice.File)  # type: str


class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super(LocalToolConfig, self).__init__()

        self.PackageConfigurationType = DefaultValue.PackageConfigurationType


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowBuildRun(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super(ToolFlowBuildRun, self).__init__(toolAppContext)

    def ProcessFromStandaloneCommandLine(self, args: Any, currentDirPath: str,  userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function
        localToolConfig = LocalToolConfig()
        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)
        # Configure the local part

        self.ProcessAppInfo(currentDirPath, localToolConfig, AppInfoChoice.FromString(args.AppInfo))


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function
        localToolConfig = LocalToolConfig()
        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)
        # Configure the local part
        localToolConfig.PackageConfigurationType = args.type

        if args.AppInfo is None:
            self.Process(currentDirPath, toolConfig, localToolConfig)
        else:
            self.ProcessAppInfo(currentDirPath, localToolConfig, AppInfoChoice.FromString(args.AppInfo))



    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)

        # Take advantage of the --ForAllExe option in the builder
        toolAppContext = self.ToolAppContext
        toolFlowConfig = ToolFlowBuild.GetDefaultLocalConfig()
        toolFlowConfig.SetToolAppConfigValues(toolAppContext.ToolAppConfig)
        toolFlowConfig.RemainingArgs = ToolAppConfigDefaultValue.RemainingArgs
        toolFlowConfig.ForAllExe = '(EXE) {0}'.format(" ".join(localToolConfig.RemainingArgs))
        toolFlowConfig.PackageConfigurationType = localToolConfig.PackageConfigurationType

        buildFlow = ToolFlowBuild.ToolFlowBuild(toolAppContext)
        buildFlow.Process(currentDirPath, toolConfig, toolFlowConfig)



    def ProcessAppInfo(self, currentDirPath: str, localToolConfig: LocalToolConfig, appInfoChoice: int) -> None:
        requestedFileList = [] # type: List[str]
        localFile = IOUtil.Join(currentDirPath, g_InfoFilename)
        if IOUtil.IsFile(localFile):
            requestedFileList.append(localFile)

        activePlatformNameId = localToolConfig.PlatformName.lower()

        isRecursive = appInfoChoice == AppInfoChoice.Recursive
        loader = AppInfoLoader(self.Log, g_InfoFilename, requestedFileList, isRecursive, currentDirPath, activePlatformNameId)
        if loader.IsEmpty():
            self.Log.LogPrint("No '{0}' files found".format(g_InfoFilename))
            return

        requirementTree = AppInfoRequirementTree(self.Log, loader.GetDict(), activePlatformNameId)

        if localToolConfig.BuildPackageFilters is None or localToolConfig.BuildPackageFilters.ExtensionNameList is None:
            raise Exception("BuildPackageFilters.ExtensionNameList is invalid")

        # Now we basically need to apply the same filters as the other build tools
        completePackageList = [AppInfoPackage(self.Log, appInfo, filename) for filename, appInfo in loader.GetDict().items()]
        completePackageList.sort(key=lambda s: s.Name.lower())
        filteredPackageList = PackageFilter.FilterAppInfo(self.Log, completePackageList,
                                                          requirementTree.GlobalTree,
                                                          localToolConfig.BuildPackageFilters)

        # Then its time to run the the packages that are left
        self.__RunPackages(filteredPackageList, localToolConfig.RemainingArgs, localToolConfig.BuildVariantsDict)


    def __RunPackages(self, packages: List[AppInfoPackage], userArgs: List[str],
                      buildVariantsDict: Dict[str, str]) -> None:
        for package in packages:
            self.__RunPackage(package, userArgs, buildVariantsDict)


    def __RunPackage(self, package: AppInfoPackage, userArgs: List[str],
                     buildVariantsDict: Dict[str, str]) -> None:
        if package.GeneratorReport is None or package.GeneratorReport.ExecutableReport is None or package.GeneratorReport.VariableReport is None:
            raise Exception("Could not run {0}, as we dont have the required information".format(package.Name))

        self.Log.LogPrint("Running {0}".format(package.Name))

        executableReport = package.GeneratorReport.ExecutableReport
        variableReport = package.GeneratorReport.VariableReport

        runCommandList =  []

        packagePath = package.AbsolutePath
        exePath = ReportVariableFormatter.Format(executableReport.ExeFormatString, variableReport, buildVariantsDict, executableReport.EnvironmentVariableResolveMethod)
        if not executableReport.UseAsRelative:
            exePath = IOUtil.Join(packagePath, exePath)

        runCommandList.append(exePath)

        if executableReport.RunScript is not None:
            runScript = executableReport.RunScript
            if not executableReport.UseAsRelative:
                runScript = IOUtil.Join(packagePath, runScript)
            runCommandList.insert(0, runScript)

        currentWorkingDirectory = package.AbsolutePath
        runCommandList = runCommandList + userArgs
        try:
            result = subprocess.call(runCommandList, cwd=currentWorkingDirectory)
            if result != 0:
                self.Log.LogPrintWarning("The run command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(runCommandList), result, currentWorkingDirectory))
                sys.exit(result)
        except FileNotFoundError:
            self.Log.LogPrintWarning("The run command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(runCommandList), currentWorkingDirectory))
            raise



class ToolAppFlowFactory(AToolAppFlowFactory):
    def __init__(self) -> None:
        pass


    def GetTitle(self) -> str:
        return 'FslBuildRun'


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.ProcessRemainingArgs = False
        argConfig.SupportBuildTime = True
        argConfig.AddBuildFiltering = True
        argConfig.AddBuildThreads = True
        argConfig.AddBuildVariants = True
        argConfig.ProcessRemainingArgs = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        allAppInfoChoices = [AppInfoChoice.ToString(entry) for entry in AppInfoChoice.GetAll()]

        parser.add_argument('-t', '--type', default=DefaultValue.PackageConfigurationType, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')
        parser.add_argument('--AppInfo', default=DefaultValue.AppInfo, choices=allAppInfoChoices, help='Run via appinfo: {0}.'.format(", ".join(allAppInfoChoices)))


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildRun(toolAppContext)


    def AddCustomStandaloneArguments(self, parser: argparse.ArgumentParser, userTag: Optional[object]) -> None:
        allAppInfoChoices = [AppInfoChoice.ToString(entry) for entry in AppInfoChoice.GetAll()]
        parser.add_argument('--AppInfo', default=DefaultValueStandalone.AppInfo, choices=allAppInfoChoices, help='Run via appinfo: {0}.'.format(", ".join(allAppInfoChoices)))
