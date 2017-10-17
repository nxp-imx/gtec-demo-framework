#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from typing import Any
from typing import List
from typing import Optional
import argparse
from FslBuildGen import IOUtil
from FslBuildGen import Main as MainFlow
from FslBuildGen import ParseUtil
from FslBuildGen import PackageListUtil
from FslBuildGen import PlatformUtil
from FslBuildGen.Generator import PluginConfig
from FslBuildGen import PluginSharedValues
from FslBuildGen.BuildConfig import ScanSourceFiles
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildExternal import RecipeBuilder
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Log import Log
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig

class DefaultValue(object):
    DryRun = False
    PackageConfigurationType = PluginSharedValues.TYPE_DEFAULT
    Project = None  # type: Optional[str]
    Repair = False
    ScanSource = False


class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super(LocalToolConfig, self).__init__()
        self.DryRun = DefaultValue.DryRun
        self.PackageConfigurationType = DefaultValue.PackageConfigurationType
        self.Project = DefaultValue.Project
        self.Repair = DefaultValue.Repair
        self.ScanSource = DefaultValue.ScanSource
        #self.Validate = False


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowBuildCheck(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super(ToolFlowBuildCheck, self).__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function

        localToolConfig = LocalToolConfig()

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.DryRun = args.DryRun
        localToolConfig.PackageConfigurationType = args.type
        localToolConfig.Project = args.project
        localToolConfig.ScanSource = args.ScanSource
        localToolConfig.Repair = args.Repair

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def __CreatePackageProcess(self, config: Config, currentDirPath: str, plugin: GeneratorPlugin) -> MainFlow.PackageLoadAndResolveProcess:
        filePathList = MainFlow.DoGetFiles(config, currentDirPath)
        packageProcess = MainFlow.PackageLoadAndResolveProcess(config)
        packageProcess.Load(filePathList, plugin)
        return packageProcess


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:

        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)
        if localToolConfig.DryRun:
            config.ForceDisableAllWrite()

        if localToolConfig.ScanSource:
            config.IgnoreNotSupported = True


        packageFilters = localToolConfig.BuildPackageFilters

        # Get the platform and see if its supported
        platform = PluginConfig.GetGeneratorPluginById(localToolConfig.PlatformName, False)
        PlatformUtil.CheckBuildPlatform(platform.Name)
        generatorContext = GeneratorContext(config, config.ToolConfig.Experimental, platform)

        config.LogPrint("Active platform: {0}".format(platform.Name))

        packageProcess = None   # type: Optional[MainFlow.PackageLoadAndResolveProcess]
        packages = None
        discoverFeatureList = '*' in packageFilters.FeatureNameList
        if discoverFeatureList or localToolConfig.Project is None or localToolConfig.ScanSource:
            if discoverFeatureList:
                config.LogPrint("No features specified, so using package to determine them")
            if localToolConfig.ScanSource or discoverFeatureList:
                packageProcess = self.__CreatePackageProcess(config, currentDirPath, generatorContext.Platform)
                packageProcess.Resolve(generatorContext, packageFilters, False, False)
                packages = packageProcess.Packages
                topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
                if discoverFeatureList:
                    packageFilters.FeatureNameList = [entry.Name for entry in topLevelPackage.ResolvedAllUsedFeatures]

        if not localToolConfig.ScanSource:
            Validate.ValidatePlatform(config, localToolConfig.PlatformName, packageFilters.FeatureNameList)
            if packageProcess is None:
                packageProcess = self.__CreatePackageProcess(config, currentDirPath, generatorContext.Platform)
            if not packageProcess.IsFullResolve or packages is None:
                # For now this requires a full resolve (but basically it only requires basic + files)
                packages = packageProcess.Resolve(generatorContext, packageFilters, False, True)
            topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
            RecipeBuilder.ValidateInstallationForPackages(config, generatorContext, topLevelPackage.ResolvedBuildOrder)

        if localToolConfig.ScanSource:
            if packages is None or packageProcess is None:
                raise Exception("Packages can not be None when scanning source")
            if not packageProcess.IsFullResolve:
                # For now this requires a full resolve (but basically it only requires basic + files)
                packages = packageProcess.Resolve(generatorContext, packageFilters, False, True)

            thirdpartyExceptionDir = IOUtil.TryToUnixStylePath(currentDirPath) if not config.IsSDKBuild else None
            ScanSourceFiles.Scan(config, packages, localToolConfig.Repair, thirdpartyExceptionDir, CheckType.Normal)


class ToolAppFlowFactory(AToolAppFlowFactory):
    def __init__(self) -> None:
        pass


    def GetTitle(self) -> str:
        return 'FslBuildCheck'


    def GetShortDesc(self) -> Optional[str]:
        return "Basic sdk configuration validation"


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.SupportBuildTime = True
        argConfig.AddUseFeatures = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        parser.add_argument('--project', default=DefaultValue.Project, help='The name of the project')
        parser.add_argument('-t', '--type', default=DefaultValue.PackageConfigurationType, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')
        parser.add_argument('--ScanSource', action='store_true', help='Scan source and check for common issues. (Disabled the normal build environment configuration check)')
        parser.add_argument('--Repair', action='store_true', help='If used in combination with --ScanSource the tool will attempt to fix common mistakes, beware this modifies your source files so use it at your own risk!.')
        parser.add_argument('--DryRun', action='store_true', help='No files will be created')


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildCheck(toolAppContext)