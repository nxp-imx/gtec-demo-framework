#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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

from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple
#from typing import Union
import copy
import os
import os.path
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
#from FslBuildGen import PackageListUtil
#from FslBuildGen import PackageUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen import Util
from FslBuildGen.BasicConfig import BasicConfig
#from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.Build.Filter import LocalUtil
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import GeneratorType
from FslBuildGen.ErrorHelpManager import ErrorHelpManager
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Generator import PluginConfig
#from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Generator.PluginConfigContext import PluginConfigContext
from FslBuildGen.Log import Log
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.PackageLoader import PackageLoader
from FslBuildGen.PackageManager import PackageManagerFilter
from FslBuildGen.PackageResolver import PackageResolver
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Version import Version
from FslBuildGen.Tool import ToolAppMain
from FslBuildGen.Tool.LowLevelToolConfig import LowLevelToolConfig
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfig import ToolConfigPackageConfigurationAddLocationType
from FslBuildGen.ToolConfig import ToolConfigPackageConfigurationLocationSetup
from FslBuildGen.ToolConfigPackageRootUtil import ToolConfigPackageRootUtil
from FslBuildGen.ToolMinimalConfig import ToolMinimalConfig
from FslBuildGen.Xml.XmlGenFile import XmlGenFile

def DoGetFiles(config: Config, toolMiniConfig: ToolMinimalConfig,
               currentDir: str, allowRecursiveScan: bool = False) -> List[str]:
    """
    :param currentDir: currentDir must be part of a package root
    :param allowRecursiveScan: if True and not a sdk build all subdirectories will be scanned
    """
    if allowRecursiveScan and config.IsSDKBuild:
        config.DoPrintWarning("recursive is ignored for sdk builds")

    if ToolConfigPackageRootUtil.TryFindRootDirectory(toolMiniConfig.RootDirectories, currentDir) is None:
        raise UsageErrorException("the folder '{0}' does not reside inside one of the root dirs".format(currentDir))

    theFiles = [] # type: List[str]
    if not config.IsSDKBuild:
        if allowRecursiveScan:
            theFiles += IOUtil.FindFileByName(currentDir, config.GenFileName, toolMiniConfig.IgnoreDirectories)
        else:
            theFile = IOUtil.Join(currentDir, config.GenFileName)
            if not os.path.isfile(theFile):
                raise Exception("File not found: '{0}'".format(theFile))
            theFiles.append(theFile)
    return theFiles


class PackageLoadAndResolveProcess(object):
    def __init__(self, config: Config,
                 packageLoader: Optional[PackageLoader] = None,
                 plugin: Optional[GeneratorPlugin] = None,
                 writeGraph: bool = False) -> None:
        self.Config = config
        self.Log = config
        self.MarkExternalLibFirstUse = False
        self.SourceFiles = []         # type: List[str]
        self.FoundInputFiles = []     # type: List[PackageFile]
        self.LoadedGenFiles = []      # type: List[XmlGenFile]
        self.Packages = []            # type: List[Package]
        self.IsFullResolve = True
        self.__loadCalled = False
        self.__writeGraph = writeGraph

        if packageLoader is not None:
            if plugin is None:
                raise Exception("A package loader also requires a plugin parameter")
            self.__ExtractFromPackageLoader(packageLoader, plugin)

    def __ExtractFromPackageLoader(self, packageLoader: PackageLoader, plugin: GeneratorPlugin) -> None:
        self.SourceFiles = packageLoader.SourceFiles
        self.FoundInputFiles = packageLoader.FoundInputFiles
        self.LoadedGenFiles = packageLoader.GenFiles
        self.MarkExternalLibFirstUse = plugin.PackageResolveConfig_MarkExternalLibFirstUse
        self.__loadCalled = True


    def Load(self, filePathList: List[str], plugin: GeneratorPlugin, forceImportPackageNames: Optional[List[str]] = None) -> None:
        if self.__loadCalled:
            raise Exception("Load has already been called")
        packageLoader = PackageLoader(self.Config, filePathList, plugin, forceImportPackageNames)
        self.__ExtractFromPackageLoader(packageLoader, plugin)


    def Resolve(self, platformContext: PlatformContext,
                packageFilters: PackageFilters,
                autoAddRecipeExternals: bool = True,
                fullResolve: bool = True) -> List[Package]:
        sourceGenFiles = self.LoadedGenFiles
        if not self.__loadCalled:
            raise Exception("Load has not been called")

        packageManagerFilter = PackageManagerFilter(self.SourceFiles, packageFilters)

        log = self.Config # type: Log
        configBuildDir = self.Config.GetBuildDir()
        configIsDryRun = self.Config.IsDryRun
        configIgnoreNotSupported = self.Config.IgnoreNotSupported
        configAllowVariantExtension = self.Config.AllowVariantExtension
        configGroupException = self.Config.GroupException
        toolConfig = self.Config.ToolConfig

        packageResolver = PackageResolver(log, configBuildDir, configIsDryRun, configIgnoreNotSupported, configAllowVariantExtension,
                                          configGroupException, toolConfig, platformContext, sourceGenFiles, autoAddRecipeExternals, fullResolve,
                                          self.MarkExternalLibFirstUse,
                                          packageFilters.RecipeFilterManager, packageManagerFilter, self.__writeGraph)
        self.IsFullResolve = fullResolve
        self.Packages = packageResolver.Packages
        return self.Packages


def DoGetPackages(generatorContext: GeneratorContext, config: Config, filePathList: List[str],
                  packageFilters: PackageFilters, autoAddRecipeExternals: bool = True,
                  forceImportPackageNames: Optional[List[str]] = None) -> List[Package]:

    process = PackageLoadAndResolveProcess(config)
    process.Load(filePathList, generatorContext.Platform, forceImportPackageNames)
    process.Resolve(generatorContext, packageFilters, autoAddRecipeExternals)
    return process.Packages


def __ResolveAndGenerate(config: Config, errorHelpManager: ErrorHelpManager, platformGeneratorPlugin: GeneratorPlugin, packageLoader: PackageLoader,
                         packageFilters: PackageFilters, isSDKBuild: bool, writeGraph: bool) -> List[Package]:
    generatorContext = GeneratorContext(config, errorHelpManager, packageFilters.RecipeFilterManager, config.ToolConfig.Experimental,
                                        platformGeneratorPlugin)


    process = PackageLoadAndResolveProcess(config, packageLoader, platformGeneratorPlugin, writeGraph=writeGraph)
    process.Resolve(generatorContext, packageFilters)

    if not isSDKBuild:
        for package in process.Packages:
            if not package.ResolvedPlatformSupported and package.Type != PackageType.TopLevel:
                notSupported = LocalUtil.BuildListOfDirectlyNotSupported(package)
                notSupportedNames = Util.ExtractNames(notSupported)
                config.DoPrintWarning("{0} was marked as not supported on this platform by package: {1}".format(package.Name, notSupportedNames))

    return platformGeneratorPlugin.Generate(generatorContext, config, process.Packages)


def DoGenerateBuildFiles(pluginConfigContext: PluginConfigContext, config: Config, errorHelpManager: ErrorHelpManager,
                         files: List[str], platformGeneratorPlugin: GeneratorPlugin,
                         packageFilters: PackageFilters, writeGraph: bool = False) -> List[Package]:
    config.LogPrint("- Generating build files")

    isSDKBuild = len(files) <= 0
    packageLoader = PackageLoader(config, files, platformGeneratorPlugin)
    return __ResolveAndGenerate(config, errorHelpManager, platformGeneratorPlugin, packageLoader, packageFilters,
                                isSDKBuild, writeGraph)


def DoGenerateBuildFilesNoAll(config: Config, errorHelpManager: ErrorHelpManager,
                              files: List[str],
                              platformGeneratorPlugin: GeneratorPlugin,
                              packageFilters: PackageFilters) -> List[Package]:
    config.LogPrint("- Generating build files")
    isSDKBuild = len(files) <= 0
    packageLoader = PackageLoader(config, files, platformGeneratorPlugin)
    return __ResolveAndGenerate(config, errorHelpManager, platformGeneratorPlugin, packageLoader, packageFilters, isSDKBuild, False)


def DoGenerateBuildFilesNow(pluginConfigContext: PluginConfigContext, config: Config, errorHelpManager: ErrorHelpManager, files: List[str],
                            platformGeneratorPlugin: GeneratorPlugin, packageFilters: PackageFilters) -> Optional[Tuple[List[Package], GeneratorPlugin]]:
    config.LogPrint("- Generating build files")

    isSDKBuild = len(files) <= 0
    packageLoader = PackageLoader(config, files, platformGeneratorPlugin)
    res = None # type: Optional[Tuple[List[Package], GeneratorPlugin]]
    for entry in pluginConfigContext.GetGeneratorPlugins():
        if entry.PlatformName.lower() == platformGeneratorPlugin.OriginalPlatformId and (not entry.InDevelopment):
            packages = __ResolveAndGenerate(config, errorHelpManager, entry, copy.deepcopy(packageLoader), packageFilters, isSDKBuild, False)
            res = (packages, entry)
    return res


def ToUnitTestPath(config: Config, path: str) -> str:
    if config.TestPath is None:
        raise Exception("config.TestPath not configured")
    return IOUtil.Join(config.TestPath, path)


def ToUnitTestPaths(config: Config, paths: List[str]) -> List[str]:
    res = []
    for path in paths:
        res.append(ToUnitTestPath(config, path))
    return res


def ToolConfigPackageConfigurationLocationSetupToUnitTestPaths(config: Config, location: ToolConfigPackageConfigurationLocationSetup) -> ToolConfigPackageConfigurationLocationSetup:
    newName = ToUnitTestPath(config, location.Name)
    return ToolConfigPackageConfigurationLocationSetup(newName, location.ScanMethod, location.Blacklist)


def CustomUnitTestRootsToUnitTestPaths(config: Config, paths: ToolConfigPackageConfigurationAddLocationType) -> ToolConfigPackageConfigurationAddLocationType:
    if isinstance(paths, str):
        return ToUnitTestPath(config, paths)
    elif isinstance(paths, ToolConfigPackageConfigurationLocationSetup):
        return ToolConfigPackageConfigurationLocationSetupToUnitTestPaths(config, paths)

    if not isinstance(paths, list):
        raise Exception("Not supported")

    if len(paths) <= 0:
        return paths

    if isinstance(paths[0], str):
        paths1 = cast(List[str], paths)
        res1 = []
        for path1 in paths1:
            res1.append(ToUnitTestPath(config, path1))
        return res1

    paths2 = cast(List[ToolConfigPackageConfigurationLocationSetup], paths)
    res2 = []
    for path2 in paths2:
        res2.append(ToolConfigPackageConfigurationLocationSetupToUnitTestPaths(config, path2))
    return res2


def GetDefaultConfigForTest(enableTestMode: bool = False, customUnitTestRoots: Optional[List[str]] = None) -> Config:
    strToolAppTitle = "UnitTest"
    log = Log(strToolAppTitle, 0)
    currentDir = IOUtil.GetEnvironmentVariableForDirectory("FSL_GRAPHICS_INTERNAL")
    basicConfig = BasicConfig(log)
    localToolConfig = LowLevelToolConfig(log.Verbosity, False, False, False, False, currentDir)
    projectRootConfig = ToolAppMain.GetProjectRootConfig(localToolConfig, basicConfig, currentDir)
    buildPlatformType = PlatformUtil.DetectBuildPlatformType()
    toolConfig = ToolConfig(buildPlatformType, Version(1, 3, 3, 7), basicConfig, projectRootConfig.ToolConfigFile, projectRootConfig)
    config = Config(log, toolConfig, PluginSharedValues.TYPE_UNIT_TEST, None, True)
    config.ForceDisableAllWrite()
    if enableTestMode:
        config.SetTestMode()
    if customUnitTestRoots is not None:
        TEST_AddPackageRoots(config, customUnitTestRoots, True)
    return config

#def __GetTestGeneratorCMakeConfig() -> GeneratorCMakeConfig:
#    generatorCMakeConfig = GeneratorCMakeConfig()
#    return generatorCMakeConfig

def __TestGenerateBuildFilesAllPlatforms(config: Config, files: List[str]) -> Dict[str, List[Package]]:
    res = {} # type: Dict[str, List[Package]]
    for platformId in PackageConfig.APPROVED_PLATFORM_NAMES:
        errorHelpManager = ErrorHelpManager()
        packageFilters = PackageFilters()
        log = config # type: Log
        #generatorCMakeConfig = __GetTestGeneratorCMakeConfig()
        pluginConfigContext = PluginConfig.InitPluginConfigContext(log, config.ToolConfig.ToolVersion, allowDevelopmentPlugins=True)
        pluginConfigContext.SetVSVersion(str(config.ToolConfig.GetVisualStudioDefaultVersion()))

        buildVariantConfig = BuildVariantConfig.Debug
        platform = pluginConfigContext.GetGeneratorPluginById(platformId, GeneratorType.Default, buildVariantConfig,
                                                              config.ToolConfig.DefaultPackageLanguage, config.ToolConfig.CMakeConfiguration,
                                                              None, False)
        resultTuple = DoGenerateBuildFilesNow(pluginConfigContext, config, errorHelpManager, files, platform, packageFilters)
        if resultTuple is not None:
            res[platformId] = resultTuple[0]
    return res


def __TestGetPackageLoader(config: Config, files: List[str], platformId: str) -> PackageLoader:
    #packageFilters = PackageFilters()
    #generatorCMakeConfig = __GetTestGeneratorCMakeConfig()
    log = config # type: Log
    pluginConfigContext = PluginConfig.InitPluginConfigContext(log, config.ToolConfig.ToolVersion, allowDevelopmentPlugins=True)
    buildVariantConfig = BuildVariantConfig.Debug
    platformGeneratorPlugin = pluginConfigContext.GetGeneratorPluginById(platformId, GeneratorType.Default, buildVariantConfig,
                                                                         config.ToolConfig.DefaultPackageLanguage,
                                                                         config.ToolConfig.CMakeConfiguration, None, False)
    return PackageLoader(config, files, platformGeneratorPlugin)


def TEST_AddPackageRoots(config: Config, customUnitTestRoots: ToolConfigPackageConfigurationAddLocationType, replaceExistingLocations: bool = False) -> None:
    unitTestRootList = CustomUnitTestRootsToUnitTestPaths(config, customUnitTestRoots)
    activePackageConfiguration = config.ToolConfig.PackageConfiguration[PluginSharedValues.TYPE_UNIT_TEST]
    if replaceExistingLocations:
        activePackageConfiguration.ClearLocations("$(FSL_GRAPHICS_SDK)/ThirdParty")
    activePackageConfiguration.AddLocations(unitTestRootList)


def SimpleTestHookOneFile(theFile: str) -> Dict[str, List[Package]]:
    config = GetDefaultConfigForTest()
    theFile = ToUnitTestPath(config, theFile)
    return __TestGenerateBuildFilesAllPlatforms(config, [theFile])


def SimpleTestHookFiles(theFiles: List[str]) -> Dict[str, List[Package]]:
    config = GetDefaultConfigForTest()
    theFiles = ToUnitTestPaths(config, theFiles)
    return __TestGenerateBuildFilesAllPlatforms(config, theFiles)


def SimpleTestHookFilesWithCustomPackageRoot(theFiles: List[str], customUnitTestRoots: List[str]) -> Dict[str, List[Package]]:
    config = GetDefaultConfigForTest()
    TEST_AddPackageRoots(config, customUnitTestRoots)
    theFiles = ToUnitTestPaths(config, theFiles)
    return __TestGenerateBuildFilesAllPlatforms(config, theFiles)


def SimpleTestHookOneFileEx(theFile: str, config: Config) -> Dict[str, List[Package]]:
    config.ForceDisableAllWrite()
    theFile = ToUnitTestPath(config, theFile)
    return __TestGenerateBuildFilesAllPlatforms(config, [theFile])


def SimpleTestHookFilesEx(theFiles: List[str], config: Config) -> Dict[str, List[Package]]:
    config.ForceDisableAllWrite()
    theFiles = ToUnitTestPaths(config, theFiles)
    return __TestGenerateBuildFilesAllPlatforms(config, theFiles)


def SimpleTestHookGetPackageLoaderOneFileEx(file: str, config: Config) -> List[PackageLoader]:
    config.ForceDisableAllWrite()
    theFiles = ToUnitTestPaths(config, [file])
    res = [] # type: List[PackageLoader]
    for platformId in PackageConfig.APPROVED_PLATFORM_NAMES:
        res.append(__TestGetPackageLoader(config, theFiles, platformId))
    return res
