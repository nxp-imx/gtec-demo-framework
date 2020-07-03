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
import argparse
from FslBuildGen import Main as MainFlow
from FslBuildGen import PackageListUtil
from FslBuildGen import ParseUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen.Build import Builder
from FslBuildGen.Build.BuildVariantConfigUtil import BuildVariantConfigUtil
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.BuildExternal.RecipeInfo import RecipeInfo
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Generator import PluginConfig
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.Info import InfoSaver

class DefaultValue(object):
    PackageTypeList = "*"
    RequireFeaturesList = "*"
    UseExtensionsList = "*"
    UseFeaturesList =  "*"
    #RequireExtensionsList = "*"

    #DryRun = False
    IgnoreNotSupported = False
    ListBuildVariants = False
    ListExtensions = False
    ListFeatures = False
    ListRecipes = False
    ListRequirements = False
    ListVariants = False
    Stats = False
    PackageConfigurationType = PluginSharedValues.TYPE_DEFAULT
    SaveJson = None # type: Optional[str]
    IncludeGeneratorReport = False


class LocalToolConfig(ToolAppConfig):
    def __init__(self, packageTypeList: List[str]) -> None:
        super().__init__()

        self.PackageTypeList = packageTypeList

        #self.DryRun = DefaultValue.DryRun
        self.IgnoreNotSupported = DefaultValue.IgnoreNotSupported
        self.ListBuildVariants = DefaultValue.ListBuildVariants
        self.ListExtensions = DefaultValue.ListExtensions
        self.ListFeatures = DefaultValue.ListFeatures
        self.ListRecipes = DefaultValue.ListRecipes
        self.ListRequirements = DefaultValue.ListRequirements
        self.ListVariants = DefaultValue.ListVariants
        self.Stats = DefaultValue.Stats
        self.PackageConfigurationType = DefaultValue.PackageConfigurationType
        self.SaveJson = DefaultValue.SaveJson
        self.IncludeGeneratorReport = DefaultValue.IncludeGeneratorReport


def GetDefaultLocalConfig() -> LocalToolConfig:
    parsedPackageType = ParseUtil.ParsePackageTypeList(DefaultValue.PackageTypeList)
    return LocalToolConfig(parsedPackageType)


class ToolFlowBuildInfo(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super().__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function

        parsedPackageType = ParseUtil.ParsePackageTypeList(args.PackageType)

        localToolConfig = LocalToolConfig(parsedPackageType)

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.IgnoreNotSupported = args.IgnoreNotSupported
        localToolConfig.ListBuildVariants = args.ListBuildVariants
        localToolConfig.ListExtensions = args.ListExtensions
        localToolConfig.ListFeatures = args.ListFeatures
        localToolConfig.ListRecipes = args.ListRecipes
        localToolConfig.ListRequirements = args.ListRequirements
        localToolConfig.ListVariants = args.ListVariants
        localToolConfig.Stats = args.stats
        localToolConfig.PackageConfigurationType = args.type
        localToolConfig.SaveJson = args.SaveJson
        localToolConfig.IncludeGeneratorReport = args.IncludeGeneratorReport

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)

        #if localToolConfig.DryRun:
        #    config.ForceDisableAllWrite()
        if localToolConfig.IgnoreNotSupported:
            config.IgnoreNotSupported = True

        self.Log.PrintTitle()

        packageFilters = localToolConfig.BuildPackageFilters

        buildVariantConfig = BuildVariantConfigUtil.GetBuildVariantConfig(localToolConfig.BuildVariantsDict)
        generator = self.ToolAppContext.PluginConfigContext.GetGeneratorPluginById(localToolConfig.PlatformName, localToolConfig.Generator,
                                                                                   buildVariantConfig, False, config.ToolConfig.CMakeConfiguration,
                                                                                   localToolConfig.GetUserCMakeConfig())

        theFiles = MainFlow.DoGetFiles(config, toolConfig.GetMinimalConfig(), currentDirPath, localToolConfig.Recursive)
        generatorContext = GeneratorContext(config, self.ErrorHelpManager, packageFilters.RecipeFilterManager, config.ToolConfig.Experimental, generator)
        packages = MainFlow.DoGetPackages(generatorContext, config, theFiles, packageFilters, autoAddRecipeExternals=False)

        topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
        requestedFiles = None if config.IsSDKBuild else theFiles

        if localToolConfig.SaveJson is not None:
            if localToolConfig.BuildPackageFilters.ExtensionNameList is None:
                raise Exception("Invalid config missing ExtensionNameList filters")
            config.LogPrint("Saving to json file '{0}'".format(localToolConfig.SaveJson))


            generatorConfig = GeneratorConfig(generator.PlatformName, config.SDKConfigTemplatePath, config.ToolConfig, 1, CommandType.Build)
            InfoSaver.SavePackageMetaDataToJson(generatorContext,
                                                generatorConfig,
                                                localToolConfig.SaveJson,
                                                config,
                                                topLevelPackage,
                                                localToolConfig.PackageTypeList,
                                                localToolConfig.IncludeGeneratorReport)

        if localToolConfig.ListFeatures:
            Builder.ShowFeatureList(self.Log, config, topLevelPackage, requestedFiles)
        if localToolConfig.ListVariants:
            requestedFiles = None if config.IsSDKBuild else theFiles
            Builder.ShowVariantList(self.Log, topLevelPackage, requestedFiles, generator)
        if localToolConfig.ListBuildVariants:
            Builder.ShowBuildVariantList(self.Log, generator)
        if localToolConfig.ListExtensions:
            Builder.ShowExtensionList(self.Log, topLevelPackage, requestedFiles)
        if localToolConfig.ListRequirements:
            Builder.ShowRequirementList(self.Log, config, topLevelPackage, requestedFiles)
        if localToolConfig.ListRecipes:
            RecipeInfo.ShowRecipeList(self.Log, topLevelPackage, requestedFiles)
        if localToolConfig.Stats:
            self.__ShowStats(topLevelPackage)

    def __ShowStats(self, topLevelPackage: Package) -> None:
        exeCount = 0
        libCount = 0
        extLibCount = 0
        headerLibCount = 0
        toolRecipe = 0
        for dep in topLevelPackage.ResolvedAllDependencies:
            if dep.Package.Type == PackageType.Executable:
                exeCount += 1
            elif dep.Package.Type == PackageType.Library:
                libCount += 1
            elif dep.Package.Type == PackageType.ExternalLibrary:
                extLibCount += 1
            elif dep.Package.Type == PackageType.HeaderLibrary:
                headerLibCount += 1
            elif dep.Package.Type == PackageType.ToolRecipe:
                toolRecipe += 1

        self.Log.DoPrint("Total packages: {}".format(len(topLevelPackage.ResolvedAllDependencies)))
        self.Log.DoPrint("- Exe:        {}".format(exeCount))
        self.Log.DoPrint("- Lib:        {}".format(libCount))
        self.Log.DoPrint("- ExtLib:     {}".format(extLibCount))
        self.Log.DoPrint("- HeaderLib:  {}".format(headerLibCount))
        self.Log.DoPrint("- ToolRecipe: {}".format(toolRecipe))

class ToolAppFlowFactory(AToolAppFlowFactory):
    def __init__(self) -> None:
        pass


    def GetTitle(self) -> str:
        return 'FslBuildInfo'


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.AddGeneratorSelection = True
        argConfig.ProcessRemainingArgs = False
        argConfig.SupportBuildTime = True
        argConfig.AddBuildFiltering = True
        argConfig.AddBuildThreads = True
        argConfig.AddBuildVariants = True
        argConfig.AllowRecursive = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        packageTypes = PackageType.AllStrings()
        packageTypes.sort()

        #parser.add_argument('--graph', action='store_true', help='Generate a dependency graph using dot (requires the graphviz dot executable in path)')
        parser.add_argument('--IgnoreNotSupported', action='store_true', help='try to build things that are marked as not supported')

        parser.add_argument('--ListBuildVariants', action='store_true', help='List all build-variants')
        parser.add_argument('--ListExtensions', action='store_true', help='List all extensions')
        parser.add_argument('--ListFeatures', action='store_true', help='List all features supported by build')
        parser.add_argument('--ListVariants', action='store_true', help='List all used variants')
        parser.add_argument('--ListRecipes', action='store_true', help='List all known recipes')
        parser.add_argument('--ListRequirements', action='store_true', help='List all requirements')

        parser.add_argument('--stats', action='store_true', help='Show stats')

        parser.add_argument('-t', '--type', default=DefaultValue.PackageConfigurationType, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')

        parser.add_argument('--SaveJson', default=DefaultValue.SaveJson, help='Save package information to the given json output file')
        parser.add_argument('--IncludeGeneratorReport', action='store_true', help='If set we include the generator report if available when saving to json')

        # filtering
        parser.add_argument('--PackageType', default=DefaultValue.PackageTypeList, help="The list of package types that will be saved [{0}]. For example [Executable] to save all packages of the 'Executable' type.".format(', '.join(packageTypes)))


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildInfo(toolAppContext)
