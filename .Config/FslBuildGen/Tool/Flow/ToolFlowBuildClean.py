#!/usr/bin/env python3

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

from typing import Any
#from typing import List
from typing import Optional
import argparse
import os
from FslBuildGen import IOUtil
from FslBuildGen import Main as MainFlow
from FslBuildGen import PackageListUtil
#from FslBuildGen.Generator import PluginConfig
#from FslBuildGen import ParseUtil
from FslBuildGen import PluginSharedValues
#from FslBuildGen.Build import Builder
from FslBuildGen.Build.BuildVariantConfigUtil import BuildVariantConfigUtil
#from FslBuildGen.BuildExternal.RecipeInfo import RecipeInfo
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
#from FslBuildGen.DataTypes import PackageType
#from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
#from FslBuildGen.Log import Log
#from FslBuildGen.PackageFilters import PackageFilters
#from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig
#from FslBuildGen.Info import InfoSaver
from FslBuildGen.VariableContextHelper import VariableContextHelper

class DefaultValue(object):
    IgnoreNotSupported = False
    PackageConfigurationType = PluginSharedValues.TYPE_DEFAULT
    ForceYes = False

class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super().__init__()
        self.ForceYes = DefaultValue.ForceYes
        self.IgnoreNotSupported = DefaultValue.IgnoreNotSupported
        self.PackageConfigurationType = DefaultValue.PackageConfigurationType


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowBuildInfo(AToolAppFlow):
    #def __init__(self, toolAppContext: ToolAppContext) -> None:
    #    super().__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:

        localToolConfig = LocalToolConfig()

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.ForceYes = args.y
        localToolConfig.IgnoreNotSupported = args.IgnoreNotSupported
        localToolConfig.PackageConfigurationType = args.type

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)

        # Disable downloads and writes
        if config.ToolConfig.Experimental is not None:
            config.ToolConfig.Experimental.AllowDownloads = False
        config.ForceDisableAllWrite()
        if localToolConfig.IgnoreNotSupported:
            config.IgnoreNotSupported = True

        self.Log.PrintTitle()

        if not localToolConfig.ForceYes and not self.__AskYesNo("Delete all build directories"):
            return

        packageFilters = localToolConfig.BuildPackageFilters

        buildVariantConfig = BuildVariantConfigUtil.GetBuildVariantConfig(localToolConfig.BuildVariantsDict)
        variableContext = VariableContextHelper.Create(toolConfig, localToolConfig.UserSetVariables)
        generator = self.ToolAppContext.PluginConfigContext.GetGeneratorPluginById(localToolConfig.PlatformName, localToolConfig.Generator,
                                                                                   buildVariantConfig, variableContext.UserSetVariables,
                                                                                   config.ToolConfig.DefaultPackageLanguage,
                                                                                   config.ToolConfig.CMakeConfiguration,
                                                                                   localToolConfig.GetUserCMakeConfig(), False)

        theFiles = MainFlow.DoGetFiles(config, toolConfig.GetMinimalConfig(generator.CMakeConfig), currentDirPath, localToolConfig.Recursive)
        generatorContext = GeneratorContext(config, self.ErrorHelpManager, packageFilters.RecipeFilterManager, config.ToolConfig.Experimental,
                                            generator, variableContext)
        packages = MainFlow.DoGetPackages(generatorContext, config, theFiles, packageFilters, autoAddRecipeExternals=False)

        topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
        #requestedFiles = None if config.IsSDKBuild else theFiles

        self.Log.LogPrint("Deleting package build directories")
        for package in topLevelPackage.ResolvedBuildOrder:
            if package.AbsoluteBuildPath is not None:
                # While the path is most likely normalized we force it here
                removePath = IOUtil.NormalizePath(package.AbsoluteBuildPath)
                if IOUtil.IsDirectory(removePath):
                    self.Log.LogPrint("- Deleting '{0}'".format(removePath))
                    if IOUtil.IsDriveRootPath(removePath):
                        raise Exception("Invalid path format '{0}'".format(removePath))
                    IOUtil.SafeRemoveDirectoryTree(removePath)

    def __AskYesNo(self, question: str, default: str = "no") -> bool:
        valid = {"yes": True, "y": True, "no": False, "n": False}
        if default is None:
            prompt = " [y/n] "
        elif default == "yes":
            prompt = " [Y/n] "
        elif default == "no":
            prompt = " [y/N] "
        else:
            raise ValueError("invalid default answer: '{0}'".format(default))

        while True:
            print(question + prompt)
            choice = input().lower().strip()
            if default is not None and choice == '':
                return valid[default]
            elif choice in valid:
                return valid[choice]
            else:
                print("Please answer yes, no, y or n.")


class ToolAppFlowFactory(AToolAppFlowFactory):
    #def __init__(self) -> None:
    #    pass


    def GetTitle(self) -> str:
        return 'FslBuildClean'


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.ProcessRemainingArgs = False
        argConfig.SupportBuildTime = True
        argConfig.AddBuildFiltering = True
        argConfig.AddBuildThreads = False
        argConfig.AddBuildVariants = True
        argConfig.AllowRecursive = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        parser.add_argument('--IgnoreNotSupported', action='store_true', help='try to build things that are marked as not supported')
        parser.add_argument('-t', '--type', default=DefaultValue.PackageConfigurationType, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')
        parser.add_argument('-y', action='store_true', help='answer yes to all prompts')


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildInfo(toolAppContext)
