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

from typing import Any
from typing import Dict
from typing import List
from typing import Optional
import argparse
from FslBuildGen.Generator import GeneratorPlugin
from FslBuildGen import Main as MainFlow
from FslBuildGen import PackageListUtil
from FslBuildGen import ParseUtil
from FslBuildGen.Generator import PluginConfig
from FslBuildGen import PluginSharedValues
from FslBuildGen.Build import Builder
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Log import Log
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig


class DefaultValue(object):
    DryRun = False
    EnableContentBuilder = True
    ForAllExe = None
    GenType = "default"
    IgnoreNotSupported = False
    ListExtensions = False
    ListFeatures = False
    ListRequirements = False
    ListVariants = False
    Variants = None
    Type = PluginSharedValues.TYPE_DEFAULT
    Command = CommandType.ToString(CommandType.Build)


class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super(LocalToolConfig, self).__init__()

        self.DryRun = DefaultValue.DryRun
        self.EnableContentBuilder = DefaultValue.EnableContentBuilder
        self.ForAllExe = DefaultValue.ForAllExe  # type: Optional[str]
        self.GenType = DefaultValue.GenType
        self.IgnoreNotSupported = DefaultValue.IgnoreNotSupported
        self.ListExtensions = DefaultValue.ListExtensions
        self.ListFeatures = DefaultValue.ListFeatures
        self.ListRequirements = DefaultValue.ListRequirements
        self.ListVariants = DefaultValue.ListVariants
        self.PackageConfigurationType = DefaultValue.Type
        self.Command = CommandType.FromString(DefaultValue.Command)


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowBuild(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super(ToolFlowBuild, self).__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the inpurt arguments here, before calling the real work function
        localToolConfig = LocalToolConfig()

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.DryRun = args.DryRun
        localToolConfig.EnableContentBuilder = ParseUtil.ParseBool(args.ContentBuilder)
        localToolConfig.ForAllExe = args.ForAllExe
        localToolConfig.GenType = args.GenType
        localToolConfig.IgnoreNotSupported = args.IgnoreNotSupported
        localToolConfig.ListExtensions = args.ListExtensions
        localToolConfig.ListFeatures = args.ListFeatures
        localToolConfig.ListRequirements = args.ListRequirements
        localToolConfig.ListVariants = args.ListVariants
        localToolConfig.PackageConfigurationType = args.type
        localToolConfig.Command = CommandType.FromString(args.Command)

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)

        if localToolConfig.DryRun:
            config.ForceDisableAllWrite()
        if localToolConfig.IgnoreNotSupported:
            config.IgnoreNotSupported = True


        # Get the platform and see if its supported
        platformGeneratorPlugin = PluginConfig.GetGeneratorPluginById(localToolConfig.PlatformName, False)
        PlatformUtil.CheckBuildPlatform(platformGeneratorPlugin.Name)

        config.LogPrint("Active platform: {0}".format(platformGeneratorPlugin.Name))

        theFiles = MainFlow.DoGetFiles(config, toolConfig.GetMinimalConfig(), currentDirPath, localToolConfig.Recursive)

        generatorContext = GeneratorContext(config, config.ToolConfig.Experimental, platformGeneratorPlugin)
        PluginConfig.SetGeneratorType(localToolConfig.GenType)

        packageFilters = localToolConfig.BuildPackageFilters
        packages = MainFlow.DoGenerateBuildFilesNoAll(config, theFiles, platformGeneratorPlugin, packageFilters)

        topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)

        requestedFiles = None if config.IsSDKBuild else theFiles

        # We need the generator to be able to examine its support

        if localToolConfig.ListFeatures or localToolConfig.ListVariants or localToolConfig.ListExtensions or localToolConfig.ListRequirements:
            if localToolConfig.ListFeatures:
                Builder.ShowFeatureList(self.Log, config, topLevelPackage, requestedFiles)
            if localToolConfig.ListVariants:
                Builder.ShowVariantList(self.Log, topLevelPackage, requestedFiles, platformGeneratorPlugin)
            if localToolConfig.ListExtensions:
                Builder.ShowExtensionList(self.Log, topLevelPackage, requestedFiles)
            if localToolConfig.ListRequirements:
                Builder.ShowRequirementList(self.Log, config, topLevelPackage, requestedFiles)
        else:
            if localToolConfig.BuildPackageFilters is None or localToolConfig.BuildPackageFilters.ExtensionNameList is None:
                raise Exception("localToolConfig.BuildPackageFilters.ExtensionNameList not set")
            Builder.BuildPackages(generatorContext, config, packages,
                                  localToolConfig.BuildVariantsDict, localToolConfig.RemainingArgs, localToolConfig.ForAllExe,
                                  platformGeneratorPlugin, localToolConfig.EnableContentBuilder, localToolConfig.ForceClaimInstallArea,
                                  localToolConfig.BuildThreads, localToolConfig.Command)



class ToolAppFlowFactory(AToolAppFlowFactory):
    def __init__(self) -> None:
        pass


    def GetTitle(self) -> str:
        return 'FslBuild'


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.ProcessRemainingArgs = True
        #argConfig.AllowVSVersion = True
        argConfig.AllowForceClaimInstallArea = True
        argConfig.SupportBuildTime = True
        argConfig.AddBuildFiltering = True
        argConfig.AddBuildThreads = True
        argConfig.AddBuildVariants = True
        argConfig.AllowRecursive = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        defaultContentBuilder = "on" if DefaultValue.EnableContentBuilder else "off"
        parser.add_argument('-t', '--type', default=DefaultValue.Type, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')
        parser.add_argument('--GenType', default=DefaultValue.GenType, help='Chose the generator type to use ({0})'.format(", ".join(list(GeneratorPlugin.GENERATOR_TYPES.keys()))))
        parser.add_argument('--ListFeatures', action='store_true', help='List all features supported by build and exit')
        #parser.add_argument('--RequireExtensions', default=DefaultValue.RequireExtensions, help='The list of extensions that are required for a executable to be build. For example [OpenGLES3.1:EXT_geometry_shader] to build all executables that use OpenGLES3.1:EXT_geometry_shader beware this allows OpenGLES3.2 apps that use EXT_geometry_shader since OpenGLES3.2 extends OpenGLES3.1.')
        parser.add_argument('--ListExtensions', action='store_true', help='List all extensions supported by build and exit')
        parser.add_argument('--ListRequirements', action='store_true', help='List all requirements supported by build and exit')
        parser.add_argument('--ListVariants', action='store_true', help='List all variants supported by build and exit')
        parser.add_argument('--DryRun', action='store_true', help='Nothing will be build')
        parser.add_argument('--IgnoreNotSupported', action='store_true', help='try to build things that are marked as not supported')
        parser.add_argument('--ContentBuilder', default=defaultContentBuilder, help='Enable/disable the content builder')
        parser.add_argument('--ForAllExe', default=DefaultValue.ForAllExe, help='For each executable run the given command. (EXE) = the full path to the executable. (EXE_NAME) = name of the executable. (EXE_PATH) = the executables dir. (PACKAGE_PATH) = full path to package (CONTENT_PATH) = full path to package content directory. *Experimental*')
        parser.add_argument('-c', "--Command", default=DefaultValue.Command, help='The build command, defaults to build. Choices: build, clean')


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuild(toolAppContext)
