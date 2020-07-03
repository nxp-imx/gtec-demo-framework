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
from FslBuildGen import PackageListUtil
from FslBuildGen import ParseUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen.Build.BuildVariantConfigUtil import BuildVariantConfigUtil
from FslBuildGen.BuildExternal import RecipeBuilder
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.BuildContent.SharedValues import CONFIG_FSLBUILDCONTENT_ENABLED
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator import PluginConfig
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.PackagePath import PackagePath
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfig import ToolConfigPackageLocation
from FslBuildGen.ToolMinimalConfig import ToolMinimalConfig

class DefaultValue(object):
    Output = None  # type: Optional[str]
    PackageConfigurationType = PluginSharedValues.TYPE_DEFAULT
    Project = None  # type: Optional[str]
    Validate = False


class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super().__init__()
        self.Output = DefaultValue.Output
        self.PackageConfigurationType = DefaultValue.PackageConfigurationType
        self.Project = DefaultValue.Project
        self.Validate = DefaultValue.Validate


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowBuildContent(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super().__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function
        localToolConfig = LocalToolConfig()

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.Output = IOUtil.NormalizePath(args.output) if args.output is not None else None
        localToolConfig.PackageConfigurationType = args.type
        localToolConfig.Project = args.project
        localToolConfig.Validate = args.Validate

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        # Check if a environment variable has been set to disable this tool
        # This is for example done by FslBuild to prevent multiple executions of content building.
        toolEnabled = IOUtil.TryGetEnvironmentVariable(CONFIG_FSLBUILDCONTENT_ENABLED)

        featureList = localToolConfig.BuildPackageFilters.FeatureNameList

        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)

        # Get the platform and see if its supported
        buildVariantConfig = BuildVariantConfigUtil.GetBuildVariantConfig(localToolConfig.BuildVariantsDict)
        generator = self.ToolAppContext.PluginConfigContext.GetGeneratorPluginById(localToolConfig.PlatformName, localToolConfig.Generator,
                                                                                   buildVariantConfig, False, config.ToolConfig.CMakeConfiguration,
                                                                                   localToolConfig.GetUserCMakeConfig())
        PlatformUtil.CheckBuildPlatform(generator.PlatformName)
        generatorContext = GeneratorContext(config, self.ErrorHelpManager, localToolConfig.BuildPackageFilters.RecipeFilterManager, config.ToolConfig.Experimental, generator)

        config.LogPrint("Active platform: {0}".format(generator.PlatformName))

        discoverFeatureList = '*' in featureList
        topLevelPackage = None
        if discoverFeatureList or localToolConfig.Project is None:
            if discoverFeatureList:
                config.LogPrint("No features specified, so using package to determine them")
            topLevelPackage = self.__ResolveAndGetTopLevelPackage(generatorContext, config, currentDirPath, toolConfig.GetMinimalConfig(),
                                                                  localToolConfig.Recursive)
            if discoverFeatureList:
                featureList = [entry.Name for entry in topLevelPackage.ResolvedAllUsedFeatures]
            #if localToolConfig.Project is None:
            #    executeablePackage = PackageListUtil.FindFirstExecutablePackage(packages)
            #    localToolConfig.Project = executeablePackage.ShortName

        if localToolConfig.Validate:
            Validate.ValidatePlatform(config, localToolConfig.PlatformName, featureList)
            if topLevelPackage is None:
                topLevelPackage = self.__ResolveAndGetTopLevelPackage(generatorContext, config, currentDirPath, toolConfig.GetMinimalConfig(),
                                                                      localToolConfig.Recursive)
            RecipeBuilder.ValidateInstallationForPackages(config, generatorContext, topLevelPackage.ResolvedBuildOrder)

        if toolEnabled is not None and not ParseUtil.ParseBool(toolEnabled):
            if self.Log.Verbosity > 0:
                print("FslBuildContent has been disabled by environment variable {0} set to {1}".format(CONFIG_FSLBUILDCONTENT_ENABLED, toolEnabled))
            return

        locations = toolConfig.PackageConfiguration[localToolConfig.PackageConfigurationType].Locations
        if not localToolConfig.Recursive or topLevelPackage is None:
            location = self.__TryFindLocation(locations, currentDirPath)
            if location is None:
                raise Exception("Could not locate location for {0}".format(currentDirPath))
            packagePath = PackagePath(currentDirPath, location)
            ContentBuilder.Build(config, packagePath, featureList, localToolConfig.Output)
        else:
            # Location not found, but its ok since '-r' was specified and we have a top level package
            for foundPackage in topLevelPackage.ResolvedBuildOrder:
                if foundPackage.Type == PackageType.Executable:
                    foundFeatureList = [entry.Name for entry in foundPackage.ResolvedAllUsedFeatures]
                    if foundPackage.Path is None:
                        raise Exception("Invalid package")
                    ContentBuilder.Build(config, foundPackage.Path, foundFeatureList)



    def __TryFindLocation(self, locations: List[ToolConfigPackageLocation], fullProjectPath: str) -> Optional[ToolConfigPackageLocation]:
        for location in locations:
            if fullProjectPath.startswith(location.ResolvedPathEx) or fullProjectPath == location.ResolvedPath:
                return location
        return None

    def __ResolveAndGetTopLevelPackage(self, generatorContext: GeneratorContext, config: Config, currentDir: str,
                                       toolMiniConfig: ToolMinimalConfig, recursive: bool) -> Package:
        # Since we use this to discover filters, we just use a empty one
        noPackageFilters = PackageFilters()

        theFiles = MainFlow.DoGetFiles(config, toolMiniConfig, currentDir, recursive)
        packages = MainFlow.DoGetPackages(generatorContext, config, theFiles, noPackageFilters)
        return PackageListUtil.GetTopLevelPackage(packages)


class ToolAppFlowFactory(AToolAppFlowFactory):
    def __init__(self) -> None:
        pass


    def GetTitle(self) -> str:
        return 'FslBuildContent'


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.SupportBuildTime = True
        argConfig.AddUseFeatures = True
        argConfig.AddBuildThreads = True
        argConfig.AllowRecursive = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        parser.add_argument('-t', '--type', default=DefaultValue.PackageConfigurationType, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')
        parser.add_argument('--project', default=DefaultValue.Project, help='The name of the project')
        parser.add_argument('--Validate', action='store_true', help='Do build config validation, like running FslBuildCheck')
        parser.add_argument('--output', default=DefaultValue.Output, help='Set the build output directory, overriding the default "<package path>/Content" directory (experimental). Only allowed for single package builds')


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildContent(toolAppContext)