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
from typing import Tuple
import argparse
import shlex
from FslBuildGen import IOUtil
from FslBuildGen import Main as MainFlow
from FslBuildGen import ParseUtil
from FslBuildGen import PackageListUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen.Build.BuildVariantConfigUtil import BuildVariantConfigUtil
from FslBuildGen.BuildConfig import ScanSourceFiles
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.PerformClangFormat import PerformClangFormat
from FslBuildGen.BuildConfig.PerformClangTidy import PerformClangTidy
from FslBuildGen.BuildConfig.PerformClangTidyConfig import PerformClangTidyConfig
from FslBuildGen.BuildConfig.ToolPackageFiltering import ToolPackageFiltering
from FslBuildGen.BuildExternal import RecipeBuilder
from FslBuildGen.BuildExternal.BuilderConfig import BuilderConfig
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import ClangTidyProfile
from FslBuildGen.DataTypes import ClangTidyProfileString
from FslBuildGen.Generator import PluginConfig
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolMinimalConfig import ToolMinimalConfig

class DefaultValue(object):
    DryRun = False
    PackageConfigurationType = PluginSharedValues.TYPE_DEFAULT
    Project = None  # type: Optional[str]
    Repair = False
    ScanSource = False
    ScanDependencies = False
    ClangFormat = False
    ClangFormatArgs = []  # type: List[str]
    ClangTidy = False
    ClangTidyArgs = []  # type: List[str]
    ClangTidyProfile = ClangTidyProfile.Strict
    ClangTidyNoDynamicVariantCache = False
    ClangPostfixArgs = [] # type: List[str]
    Legacy = False
    TidyOverrideChecks = [] # type: List[str]
    File = None  # type: Optional[str]
    IgnoreNotSupported = False


class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super().__init__()
        self.DryRun = DefaultValue.DryRun
        self.PackageConfigurationType = DefaultValue.PackageConfigurationType
        self.Project = DefaultValue.Project
        self.Repair = DefaultValue.Repair
        self.ScanSource = DefaultValue.ScanSource
        self.ScanDependencies = DefaultValue.ScanDependencies
        self.ClangFormat = DefaultValue.ClangFormat
        self.ClangFormatArgs = DefaultValue.ClangFormatArgs
        self.ClangTidy = DefaultValue.ClangTidy
        self.ClangTidyArgs = DefaultValue.ClangTidyArgs
        self.ClangTidyPostfixArgs = DefaultValue.ClangPostfixArgs
        self.ClangTidyProfile = DefaultValue.ClangTidyProfile
        self.ClangTidyNoDynamicVariantCache = DefaultValue.ClangTidyNoDynamicVariantCache
        self.Legacy = DefaultValue.Legacy
        self.TidyOverrideChecks = DefaultValue.TidyOverrideChecks
        self.File = DefaultValue.File
        #self.Validate = False
        self.IgnoreNotSupported = DefaultValue.IgnoreNotSupported


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


def ForceCheckBuildTools(configToolCheck: Config, generatorContext: GeneratorContext, toolPackageNames: List[str]) -> PackageRecipeResultManager:
    configToolCheck.LogPrint("BuildTools check");
    plugin = generatorContext.Platform
    filePathList = [] # type: List[str]
    packageFilters = PackageFilters()
    packages = MainFlow.DoGetPackages(generatorContext, configToolCheck, filePathList, packageFilters, forceImportPackageNames=toolPackageNames)

    packageRecipeResultManager = PackageRecipeResultManager(configToolCheck);
    builderConfig = BuilderConfig()
    builderConfig.Settings.CheckBuildCommands = True
    RecipeBuilder.BuildPackages(configToolCheck, generatorContext, builderConfig, packages, packageRecipeResultManager)
    return packageRecipeResultManager


def ForceBuildExternals(config: Config, generatorContext: GeneratorContext, packages: List[Package]) -> None:
    builderConfig = BuilderConfig()
    RecipeBuilder.BuildPackages(config, generatorContext, builderConfig, packages)


class ToolFlowBuildCheck(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super().__init__(toolAppContext)

    def __SplitAndTrim(self, strInput: str) -> List[str]:
        entries = strInput.split(',')
        entries = [entry.strip() for entry in entries if len(entry.strip()) > 0]
        return entries

    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function

        localToolConfig = LocalToolConfig()

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.DryRun = args.dryRun
        localToolConfig.File = args.file
        localToolConfig.PackageConfigurationType = args.type
        localToolConfig.Project = args.project
        localToolConfig.ScanSource = args.scan
        localToolConfig.ScanDependencies = args.scanDeps
        localToolConfig.Repair = args.repair
        localToolConfig.IgnoreNotSupported = args.ignoreNotSupported
        if hasattr(args, 'format'):
            localToolConfig.ClangFormat = args.format
        if hasattr(args, 'formatArgs'):
            localToolConfig.ClangFormatArgs = shlex.split(args.formatArgs)
        if hasattr(args, 'tidy'):
            localToolConfig.ClangTidy = args.tidy
        if hasattr(args, 'tidyArgs'):
            localToolConfig.ClangTidyArgs = shlex.split(args.tidyArgs)
        if hasattr(args, 'tidyPostfixArgs'):
            localToolConfig.ClangTidyPostfixArgs = shlex.split(args.tidyPostfixArgs)
        if hasattr(args, 'tidyProfile'):
            localToolConfig.ClangTidyProfile = ClangTidyProfile.FromString(args.tidyProfile)
        if hasattr(args, 'tidyOverrideChecks'):
            localToolConfig.TidyOverrideChecks = self.__SplitAndTrim(args.tidyOverrideChecks)
        if hasattr(args, 'tidyNoDynamicVariantCache'):
            localToolConfig.ClangTidyNoDynamicVariantCache = args.tidyNoDynamicVariantCache

        localToolConfig.Legacy = args.legacy

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def __CreatePackageProcess(self, config: Config, miniToolConfig: ToolMinimalConfig, currentDirPath: str, allowRecursiveScan: bool,
                               plugin: GeneratorPlugin, forceImportPackageNames: List[str]) -> MainFlow.PackageLoadAndResolveProcess:
        filePathList = MainFlow.DoGetFiles(config, miniToolConfig, currentDirPath, allowRecursiveScan)
        packageProcess = MainFlow.PackageLoadAndResolveProcess(config)
        packageProcess.Load(filePathList, plugin, forceImportPackageNames)
        return packageProcess

    def __CheckUserArgs(self, clangArgs: List[str], argDesc: str) -> None:
        for entry in clangArgs:
            if entry.strip() == "--":
                raise Exception("{0} args can not contain '--'")

    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        config = Config(self.Log, toolConfig, localToolConfig.PackageConfigurationType,
                        localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)

        # create a config we control and that can be used to just build the tool recipe's
        configToolCheck = Config(self.Log, toolConfig, PluginSharedValues.TYPE_DEFAULT,
                                 localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)
        if localToolConfig.DryRun:
            config.ForceDisableAllWrite()
            configToolCheck.ForceDisableAllWrite()

        self.__CheckUserArgs(localToolConfig.ClangFormatArgs, "formatArgs")
        self.__CheckUserArgs(localToolConfig.ClangTidyArgs, "tidyArgs")
        self.__CheckUserArgs(localToolConfig.ClangTidyPostfixArgs, "tidyPostfixArgs")

        applyClangFormat = toolConfig.ClangFormatConfiguration is not None and localToolConfig.ClangFormat
        applyClangTidy = toolConfig.ClangTidyConfiguration is not None and localToolConfig.ClangTidy

        if localToolConfig.IgnoreNotSupported or ((localToolConfig.ScanSource or applyClangFormat) and not applyClangTidy):
            config.IgnoreNotSupported = True
            configToolCheck.IgnoreNotSupported = True

        packageFilters = localToolConfig.BuildPackageFilters

        # Get the platform and see if its supported
        buildVariantConfig = BuildVariantConfigUtil.GetBuildVariantConfig(localToolConfig.BuildVariantsDict)
        if applyClangTidy and config.ToolConfig.CMakeConfiguration is not None:
            # Disable allow find package for the build checks for now as we dont use cmake for those
            # We basically have to update the tidy pass to utilize ninja+cmake for the tidy pass so that find_package will work
            config.LogPrintVerbose(2, "Force disabling 'AllowFindPackage'")
            config.ToolConfig.CMakeConfiguration.SetAllowFindPackage(False)
        generator = self.ToolAppContext.PluginConfigContext.GetGeneratorPluginById(localToolConfig.PlatformName, localToolConfig.Generator,
                                                                                   buildVariantConfig, False, config.ToolConfig.CMakeConfiguration,
                                                                                   localToolConfig.GetUserCMakeConfig())
        PlatformUtil.CheckBuildPlatform(generator.PlatformName)
        generatorContext = GeneratorContext(config, self.ErrorHelpManager, packageFilters.RecipeFilterManager, config.ToolConfig.Experimental, generator)

        config.LogPrint("Active platform: {0}".format(generator.PlatformName))

        packageRecipeResultManager = None # type: Optional[PackageRecipeResultManager]
        toolPackageNamesSet = set()
        toolPackageNames = []
        if applyClangFormat or applyClangTidy:
            if applyClangFormat:
                if toolConfig.ClangFormatConfiguration is None:
                    raise Exception("internal error")
                toolPackageNamesSet.add(toolConfig.ClangFormatConfiguration.RecipePackageName)
                toolPackageNamesSet.add(toolConfig.ClangFormatConfiguration.NinjaRecipePackageName)
            if applyClangTidy:
                if toolConfig.ClangTidyConfiguration is None:
                    raise Exception("internal error")
                toolPackageNamesSet.add(toolConfig.ClangTidyConfiguration.ClangRecipePackageName)
                toolPackageNamesSet.add(toolConfig.ClangTidyConfiguration.ClangTidyRecipePackageName)
                toolPackageNamesSet.add(toolConfig.ClangTidyConfiguration.NinjaRecipePackageName)
            toolPackageNames = list(toolPackageNamesSet)
            packageRecipeResultManager = ForceCheckBuildTools(configToolCheck, generatorContext, toolPackageNames)

        searchDir = currentDirPath
        if localToolConfig.File is not None:
            localToolConfig.File = IOUtil.NormalizePath(localToolConfig.File)
            if IOUtil.IsAbsolutePath(localToolConfig.File):
                searchDir = IOUtil.GetDirectoryName(localToolConfig.File)
        closestGenFilePath = FileFinder.TryFindClosestFileInRoot(config, toolConfig, searchDir, config.GenFileName)
        if closestGenFilePath is None:
            closestGenFilePath = searchDir

        if( self.Log.Verbosity >= 4):
            self.Log.LogPrint("Closest '{0}' file path: '{1}'".format(toolConfig.GenFileName, closestGenFilePath))

        packageProcess = None   # type: Optional[MainFlow.PackageLoadAndResolveProcess]
        packages = None
        discoverFeatureList = '*' in packageFilters.FeatureNameList
        if discoverFeatureList or localToolConfig.Project is None or localToolConfig.ScanSource or applyClangFormat or applyClangTidy:
            if discoverFeatureList:
                config.LogPrint("No features specified, so using package to determine them")
            if localToolConfig.ScanSource or applyClangFormat or applyClangTidy or discoverFeatureList:
                packageProcess = self.__CreatePackageProcess(config, toolConfig.GetMinimalConfig(), closestGenFilePath, localToolConfig.Recursive,
                                                             generatorContext.Platform, toolPackageNames)
                packageProcess.Resolve(generatorContext, packageFilters, applyClangTidy, False)
                packages = packageProcess.Packages
                topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
                if discoverFeatureList:
                    packageFilters.FeatureNameList = [entry.Name for entry in topLevelPackage.ResolvedAllUsedFeatures]

        customPackageFileFilter = None   # type: Optional[CustomPackageFileFilter]
        if not localToolConfig.ScanSource and not applyClangFormat and not applyClangTidy:
            Validate.ValidatePlatform(config, localToolConfig.PlatformName, packageFilters.FeatureNameList)
            if packageProcess is None:
                packageProcess = self.__CreatePackageProcess(config, toolConfig.GetMinimalConfig(), closestGenFilePath, localToolConfig.Recursive,
                                                             generatorContext.Platform, toolPackageNames)
            if not packageProcess.IsFullResolve or packages is None:
                # For now this requires a full resolve (but basically it only requires basic + files)
                packages = packageProcess.Resolve(generatorContext, packageFilters, applyClangTidy, True)

            topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
            RecipeBuilder.ValidateInstallationForPackages(config, generatorContext, topLevelPackage.ResolvedBuildOrder)
        else:
            if localToolConfig.File is not None:
                # Delay extension validation
                customPackageFileFilter = CustomPackageFileFilter(localToolConfig.File)

        theTopLevelPackage = None  # type: Optional[Package]
        filteredPackageList = [] # type: List[Package]
        if applyClangTidy or applyClangFormat or localToolConfig.ScanSource:
            addExternals = applyClangTidy
            filteredPackageList, theTopLevelPackage = self.__PreparePackages(self.Log, localToolConfig,
                                                                             packageProcess, generatorContext, packageFilters, addExternals,
                                                                             packages, config.IsSDKBuild, applyClangTidy, config)
            if len(filteredPackageList) <= 0:
                self.Log.DoPrint("No supported packages left to process")
                return

        if applyClangTidy:
            self.__ApplyClangTidy(self.Log, toolConfig, localToolConfig, packageRecipeResultManager, theTopLevelPackage,
                                  filteredPackageList, generator, config, generatorContext, customPackageFileFilter)

        if applyClangFormat:
            self.__ApplyClangFormat(self.Log, toolConfig, localToolConfig, packageRecipeResultManager, filteredPackageList, customPackageFileFilter,
                                    generatorContext.CMakeConfig)

        # Scan source after 'format' to ensure we dont warn about stuff that has been fixed
        if localToolConfig.ScanSource:
            self.__ApplyScanSource(self.Log, localToolConfig, config.IsSDKBuild, config.DisableWrite, filteredPackageList, customPackageFileFilter)

    def __PreparePackages(self, log: Log, localToolConfig: LocalToolConfig,
                          packageProcess: Optional[MainFlow.PackageLoadAndResolveProcess],
                          generatorContext: GeneratorContext, packageFilters: PackageFilters, addExternals: bool,
                          packages: Optional[List[Package]], isSdkBuild: bool,
                          applyClangTidy: bool, config: Config) -> Tuple[List[Package],Package]:
        if packages is None or packageProcess is None:
            raise Exception("Packages can not be None")
        if not packageProcess.IsFullResolve:
            # For now this requires a full resolve (but basically it only requires basic + files)
            packages = packageProcess.Resolve(generatorContext, packageFilters, addExternals, True)

        if applyClangTidy:
            if packages is None:
                raise Exception("Packages can not be None")
            ForceBuildExternals(config, generatorContext, packages)

        specifiedPackages = [] # type: List[Package]
        if isSdkBuild:
            specifiedPackages = list(packages)
        else:
            packageDict = {}
            for package in packages:
                packageDict[package.Name] = package
            for entry in packageProcess.FoundInputFiles:
                packageName = entry.PackageName
                if packageName in packageDict:
                    specifiedPackages.append(packageDict[packageName])
        topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
        return (ToolPackageFiltering.FilterPackages(self.Log, topLevelPackage, specifiedPackages, localToolConfig.ScanDependencies), topLevelPackage)

    def __ApplyClangTidy(self, log: Log, toolConfig: ToolConfig, localToolConfig: LocalToolConfig,
                         packageRecipeResultManager: Optional[PackageRecipeResultManager],
                         topLevelPackage: Optional[Package], tidyPackageList: List[Package], generator: GeneratorPlugin,
                         config: Config, generatorContext: GeneratorContext, customPackageFileFilter: Optional[CustomPackageFileFilter]) -> None:
        """
        The only reason we take optionals here is because they are optional in the main program, so its just easier to do the check here
        :currentDirPath: is used to process the '--file' argument
        """
        if toolConfig.ClangTidyConfiguration is None or packageRecipeResultManager is None or topLevelPackage is None:
            raise Exception("internal error")

        # Generate the build files (so that the run scripts exist)
        log.LogPrint("Generating build files")
        generator.Generate(generatorContext, config, tidyPackageList)

        clangFormatFilename = None if toolConfig.ClangFormatConfiguration is None else toolConfig.ClangFormatConfiguration.CustomFormatFile

        pythonScriptRoot = IOUtil.Join(config.SDKPath, ".Config")

        performClangTidyConfig = PerformClangTidyConfig(toolConfig.ClangTidyConfiguration, localToolConfig.ClangTidyArgs,
                                                        localToolConfig.ClangTidyPostfixArgs, localToolConfig.TidyOverrideChecks,
                                                        localToolConfig.ClangTidyProfile, not localToolConfig.ClangTidyNoDynamicVariantCache,
                                                        localToolConfig.Repair)
        PerformClangTidy.Run(log, toolConfig, generatorContext.Generator.PlatformId, topLevelPackage, tidyPackageList,
                             localToolConfig.BuildVariantsDict, pythonScriptRoot, generatorContext, config.SDKConfigTemplatePath,
                             packageRecipeResultManager, performClangTidyConfig, customPackageFileFilter, clangFormatFilename,
                             localToolConfig.BuildThreads, localToolConfig.Legacy)


    def __ApplyClangFormat(self, log: Log, toolConfig: ToolConfig, localToolConfig: LocalToolConfig,
                           packageRecipeResultManager: Optional[PackageRecipeResultManager],
                           formatPackageList: List[Package], customPackageFileFilter: Optional[CustomPackageFileFilter],
                           cmakeConfig: GeneratorCMakeConfig) -> None:
        """
        The only reason we take optionals here is because they are optional in the main program, so its just easier to do the check here
        """
        if toolConfig.ClangFormatConfiguration is None or packageRecipeResultManager is None:
            raise Exception("internal error")

        # FIX: should really be done the same was as we do for tidy
        toolConfig.ClangFormatConfiguration.AdditionalUserArguments = localToolConfig.ClangFormatArgs

        PerformClangFormat.Run(log, toolConfig, formatPackageList, customPackageFileFilter, packageRecipeResultManager,
                               toolConfig.ClangFormatConfiguration, cmakeConfig, localToolConfig.Repair, localToolConfig.BuildThreads,
                               localToolConfig.Legacy)

    def __ApplyScanSource(self, log: Log, localToolConfig: LocalToolConfig, isSdkBuild: bool, disableWrite: bool,
                          scanPackageList: List[Package], customPackageFileFilter: Optional[CustomPackageFileFilter]) -> None:
        #thirdpartyExceptionDir = IOUtil.TryToUnixStylePath(currentDirPath) if not isSdkBuild else None
        thirdpartyExceptionDir = None
        ScanSourceFiles.Scan(log, scanPackageList, customPackageFileFilter, localToolConfig.Repair, thirdpartyExceptionDir, CheckType.Normal, disableWrite)

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
        argConfig.AddBuildFiltering = True
        argConfig.AddBuildThreads = True
        argConfig.AddBuildVariants = True
        argConfig.AllowRecursive = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        clangFormatConfiguration = toolConfig.ClangFormatConfiguration
        clangTidyConfiguration = toolConfig.ClangTidyConfiguration
        parser.add_argument('--project', default=DefaultValue.Project, help='The name of the project')
        parser.add_argument('-t', '--type', default=DefaultValue.PackageConfigurationType, choices=[PluginSharedValues.TYPE_DEFAULT, 'sdk'], help='Select generator type')
        if clangFormatConfiguration is not None:
            parser.add_argument('--format', action='store_true', help="Apply the clang format file '{0}' to all include and source files. Beware this modifies your source files so use it at your own risk!.".format(clangFormatConfiguration.CustomFormatFile))
            parser.add_argument('--formatArgs', default=" ".join(DefaultValue.ClangFormatArgs), help="The command line arguments to pass to clang-format before any '--'.")
        if clangTidyConfiguration is not None:
            tidyProfileNameList = ClangTidyProfileString.AllStrings()
            tidyDefaultProfileName = ClangTidyProfile.ToString(DefaultValue.ClangTidyProfile)
            parser.add_argument('--tidy', action='store_true', help="Apply the clang tidy file '{0}' to all include and source files. Beware this modifies your source files so use it at your own risk!.".format(clangTidyConfiguration.CustomTidyFile))
            parser.add_argument('--tidyArgs', default=" ".join(DefaultValue.ClangTidyArgs), help="The command line arguments to pass to clang-tidy before any '--' (to supply a command that starts with - use '=' like --tidyArgs=-cmd').")
            parser.add_argument('--tidyPostfixArgs', default=" ".join(DefaultValue.ClangPostfixArgs), help="The command line arguments to pass to clang-tidy after '--'. (to supply a command that starts with - use '=' like --tidyPostfixArgs=-cmd).")
            parser.add_argument('--tidyProfile', default=tidyDefaultProfileName, help="Select the checker profile to use (default is: {0}). Supported profiles: {1}".format(tidyDefaultProfileName, ", ".join(tidyProfileNameList)))
            parser.add_argument('--tidyOverrideChecks', default=" ".join(DefaultValue.TidyOverrideChecks), help="Supply the checks to clang-tidy via '-checks='. The tool will automatically prepend the input string with '-*,' to disable all other checks (beware this could conflict with --tidyArgs so be careful)")
            parser.add_argument('--tidyNoDynamicVariantCache', action='store_true', help="Disable the dynamic variant caching (caching of dynamic variants can lead to incorrect headers being included, but for tidy it should be good enough. We are also phasing out dynamic variants)")
        parser.add_argument('--legacy', action='store_true', help="enable legacy mode, to run the legacy processors.")
        parser.add_argument('--file', default=DefaultValue.File, help='The file to process, supports Unix shell-style wildcards')
        parser.add_argument('--scan', action='store_true', help='Scan source and check for common issues. (Disabled the normal build environment configuration check)')
        parser.add_argument('--scanDeps', action='store_true', help="Scan all package dependencies of the specified package)")
        parser.add_argument('--repair', action='store_true', help='If used in combination with --ScanSource the tool will attempt to fix common mistakes, beware this modifies your source files so use it at your own risk!.')
        parser.add_argument('--dryRun', action='store_true', help='No files will be created')
        parser.add_argument('--ignoreNotSupported', action='store_true', help='try to check things that are marked as not supported, this might not work!')


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildCheck(toolAppContext)