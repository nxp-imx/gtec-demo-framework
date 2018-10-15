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
from typing import Union
import copy
import os
import os.path
from FslBuildGen import IOUtil
from FslBuildGen import PackageListUtil
from FslBuildGen import PackageUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen import Util
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.Build.Filter import LocalUtil
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Generator import PluginConfig
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Log import Log
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.PackageLoader import PackageLoader
from FslBuildGen.PackageResolver import PackageResolver
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Tool import ToolAppMain
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfig import ToolConfigPackageConfigurationAddLocationType
from FslBuildGen.ToolConfig import ToolConfigPackageConfigurationLocationSetup
from FslBuildGen.ToolConfigPackageRootUtil import ToolConfigPackageRootUtil
from FslBuildGen.ToolMinimalConfig import ToolMinimalConfig
from FslBuildGen.Xml.XmlGenFile import XmlGenFile

MultiPlatformPackageResultType = Dict[str, Tuple[List[Package], GeneratorPlugin]]

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
                 plugin: Optional[GeneratorPlugin] = None) -> None:
        self.Config = config
        self.Log = config
        self.MarkExternalLibFirstUse = False
        self.SourceFiles = []         # type: List[str]
        self.FoundInputFiles = []     # type: List[PackageFile]
        self.LoadedGenFiles = None    # type: Optional[List[XmlGenFile]]
        self.Packages = []            # type: List[Package]
        self.IsFullResolve = True

        if packageLoader is not None:
            if plugin is None:
                raise Exception("A package loader also requires a plugin parameter")
            self.__ExtractFromPackageLoader(packageLoader, plugin)

    def __ExtractFromPackageLoader(self, packageLoader: PackageLoader, plugin: GeneratorPlugin) -> None:
        self.SourceFiles = packageLoader.SourceFiles
        self.FoundInputFiles = packageLoader.FoundInputFiles
        self.LoadedGenFiles = packageLoader.GenFiles
        self.MarkExternalLibFirstUse = plugin.PackageResolveConfig_MarkExternalLibFirstUse


    def Load(self, filePathList: List[str], plugin: GeneratorPlugin, forceImportPackageNames: Optional[List[str]] = None) -> None:
        if self.LoadedGenFiles is not None:
            raise Exception("Load has already been called")
        packageLoader = PackageLoader(self.Config, filePathList, plugin, forceImportPackageNames)
        self.__ExtractFromPackageLoader(packageLoader, plugin)


    def Resolve(self, platformContext: PlatformContext,
                packageFilters: PackageFilters,
                autoAddRecipeExternals: bool = True,
                fullResolve: bool = True) -> List[Package]:
        sourceGenFiles = self.LoadedGenFiles
        if sourceGenFiles is None:
            raise Exception("Load has not been called")

        sourceGenFiles = self.__Filter(platformContext, packageFilters, autoAddRecipeExternals, sourceGenFiles)
        packageResolver = PackageResolver(platformContext, self.Config, sourceGenFiles,
                                          autoAddRecipeExternals, fullResolve,
                                          self.MarkExternalLibFirstUse,
                                          packageFilters.RecipeFilterManager)
        self.IsFullResolve = fullResolve
        self.Packages = packageResolver.Packages
        return self.Packages


    def __Filter(self, platformContext: PlatformContext,
                 packageFilters: PackageFilters,
                 autoAddRecipeExternals: bool,
                 sourceGenFiles: List[XmlGenFile]) -> List[XmlGenFile]:

        if not packageFilters.ContainsFilters():
            return sourceGenFiles

        self.Log.LogPrint("- Filtering")
        try:
            self.Log.PushIndent()
            packageResolver = PackageResolver(platformContext, self.Config, sourceGenFiles,
                                              autoAddRecipeExternals, False,
                                              self.MarkExternalLibFirstUse,
                                              packageFilters.RecipeFilterManager)
            packages = packageResolver.Packages

            topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
            requestedFiles = self.SourceFiles
            requestedPackages = PackageUtil.TryGetPackageListFromFilenames(topLevelPackage, requestedFiles)
            resolvedBuildOrder = PackageFilter.Filter(self.Log, topLevelPackage, requestedPackages, packageFilters)

            # Now do a lookup of package -> Genfile to provide a filtered gen file list
            genFileSet = set(sourceGenFiles)
            return [package.GenFile for package in resolvedBuildOrder if package.GenFile is not None and package.GenFile in genFileSet]
        finally:
            self.Log.PopIndent()


def DoGetPackages(generatorContext: GeneratorContext, config: Config, filePathList: List[str],
                  packageFilters: PackageFilters, autoAddRecipeExternals: bool = True,
                  forceImportPackageNames: Optional[List[str]] = None) -> List[Package]:

    process = PackageLoadAndResolveProcess(config)
    process.Load(filePathList, generatorContext.Platform, forceImportPackageNames)
    process.Resolve(generatorContext, packageFilters, autoAddRecipeExternals)
    return process.Packages


def __ResolveAndGenerate(config: Config, platformGeneratorPlugin: GeneratorPlugin, packageLoader: PackageLoader,
                         packageFilters: PackageFilters, isSDKBuild: bool) -> List[Package]:
    generatorContext = GeneratorContext(config, config.ToolConfig.Experimental, platformGeneratorPlugin)


    process = PackageLoadAndResolveProcess(config, packageLoader, platformGeneratorPlugin)
    process.Resolve(generatorContext, packageFilters)

    if not isSDKBuild:
        for package in process.Packages:
            if package.ResolvedPlatformNotSupported and package.Type != PackageType.TopLevel:
                notSupported = LocalUtil.BuildListOfDirectlyNotSupported(package)
                notSupportedNames = Util.ExtractNames(notSupported)
                config.DoPrintWarning("{0} was marked as not supported on this platform by package: {1}".format(package.Name, notSupportedNames))

    return platformGeneratorPlugin.Generate(generatorContext, config, process.Packages)


# TODO: fix the bad return type
def DoGenerateBuildFiles(config: Config,
                         files: List[str],
                         platformGeneratorPlugin: GeneratorPlugin,
                         packageFilters: PackageFilters) -> Union[List[Package], MultiPlatformPackageResultType]:
    config.LogPrint("- Generating build files")

    isSDKBuild = len(files) <= 0
    packageLoader = PackageLoader(config, files, platformGeneratorPlugin)
    res = []  # type: Union[List[Package], MultiPlatformPackageResultType]
    if platformGeneratorPlugin.Id == PluginSharedValues.PLATFORM_ID_ALL:
        resDict = {} # type: MultiPlatformPackageResultType
        for entry in PluginConfig.GetGeneratorPlugins(config.AllowDevelopmentPlugins):
            if not config.IsTestMode or not entry.InDevelopment:
                packages = __ResolveAndGenerate(config, entry, copy.deepcopy(packageLoader), packageFilters, isSDKBuild)
            resDict[entry.Name] = (packages, entry)
        res = resDict
    else:
        res = __ResolveAndGenerate(config, platformGeneratorPlugin, packageLoader, packageFilters, isSDKBuild)

    return res


def DoGenerateBuildFilesNoAll(config: Config,
                              files: List[str],
                              platformGeneratorPlugin: GeneratorPlugin,
                              packageFilters: PackageFilters) -> List[Package]:
    config.LogPrint("- Generating build files")

    if platformGeneratorPlugin.Id == PluginSharedValues.PLATFORM_ID_ALL:
        raise Exception("Can not use PLATFORM_ID_ALL")

    isSDKBuild = len(files) <= 0
    packageLoader = PackageLoader(config, files, platformGeneratorPlugin)
    return __ResolveAndGenerate(config, platformGeneratorPlugin, packageLoader, packageFilters, isSDKBuild)


def DoGenerateBuildFiles3(config: Config, files: List[str], platformGeneratorPlugin: GeneratorPlugin,
                          packageFilters: PackageFilters) -> MultiPlatformPackageResultType:
    config.LogPrint("- Generating build files")

    isSDKBuild = len(files) <= 0
    packageLoader = PackageLoader(config, files, platformGeneratorPlugin)
    if platformGeneratorPlugin.Id != PluginSharedValues.PLATFORM_ID_ALL:
        raise Exception("This requires: PLATFORM_ID_ALL")
    resDict = {} # type: MultiPlatformPackageResultType
    for entry in PluginConfig.GetGeneratorPlugins(config.AllowDevelopmentPlugins):
        if not config.IsTestMode or not entry.InDevelopment:
            packages = __ResolveAndGenerate(config, entry, copy.deepcopy(packageLoader), packageFilters, isSDKBuild)
            resDict[entry.Name] = (packages, entry)
    return resDict


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


def GetDefaultConfigForTest(enableTestMode: bool = False, customUnitTestRoots: Optional[ToolConfigPackageConfigurationAddLocationType] = None) -> Config:
    strToolAppTitle = "UnitTest"
    log = Log(strToolAppTitle, 0)
    currentDir = IOUtil.GetEnvironmentVariableForDirectory("FSL_GRAPHICS_INTERNAL")
    basicConfig = BasicConfig(log)
    localToolConfig = ToolAppMain.LowLevelToolConfig(log.Verbosity, False, False, False, False)
    projectRootConfig = ToolAppMain.GetProjectRootConfig(localToolConfig, basicConfig, currentDir)
    toolConfig = ToolConfig(basicConfig, projectRootConfig.ToolConfigFile, projectRootConfig)
    config = Config(log, toolConfig, PluginSharedValues.TYPE_UNIT_TEST, None, True)
    config.ForceDisableAllWrite()
    if enableTestMode:
        config.SetTestMode()
    if customUnitTestRoots is not None:
        TEST_AddPackageRoots(config, customUnitTestRoots, True)
    return config


def __TestGenerateBuildFiles(config: Config, files: List[str], platformId: str) -> Dict[str, List[Package]]:
    packageFilters = PackageFilters()
    platform = PluginConfig.GetGeneratorPluginById(platformId, True)
    buildFilesDict = DoGenerateBuildFiles3(config, files, platform, packageFilters)
    return {dictKey: dictValue[0] for dictKey, dictValue in buildFilesDict.items()}


def __TestGetPackageLoader(config: Config, files: List[str], platformId: str) -> PackageLoader:
    packageFilters = PackageFilters()
    platformGeneratorPlugin = PluginConfig.GetGeneratorPluginById(platformId, True)
    return PackageLoader(config, files, platformGeneratorPlugin)


def TEST_AddPackageRoots(config: Config, customUnitTestRoots: ToolConfigPackageConfigurationAddLocationType, replaceExistingLocations: bool = False) -> None:
    unitTestRootList = CustomUnitTestRootsToUnitTestPaths(config, customUnitTestRoots)
    activePackageConfiguration = config.ToolConfig.PackageConfiguration[PluginSharedValues.TYPE_UNIT_TEST]
    if replaceExistingLocations:
        activePackageConfiguration.ClearLocations()
    activePackageConfiguration.AddLocations(unitTestRootList)


def SimpleTestHookOneFile(theFile: str) -> Dict[str, List[Package]]:
    config = GetDefaultConfigForTest()
    theFile = ToUnitTestPath(config, theFile)
    return __TestGenerateBuildFiles(config, [theFile], PluginSharedValues.PLATFORM_ID_ALL)


def SimpleTestHookFiles(theFiles: List[str]) -> Dict[str, List[Package]]:
    config = GetDefaultConfigForTest()
    theFiles = ToUnitTestPaths(config, theFiles)
    return __TestGenerateBuildFiles(config, theFiles, PluginSharedValues.PLATFORM_ID_ALL)


def SimpleTestHookFilesWithCustomPackageRoot(theFiles: List[str], customUnitTestRoots: List[str]) -> Dict[str, List[Package]]:
    config = GetDefaultConfigForTest()
    TEST_AddPackageRoots(config, customUnitTestRoots)
    theFiles = ToUnitTestPaths(config, theFiles)
    return __TestGenerateBuildFiles(config, theFiles, PluginSharedValues.PLATFORM_ID_ALL)


def SimpleTestHookOneFileEx(theFile: str, config: Config) -> Dict[str, List[Package]]:
    config.ForceDisableAllWrite()
    theFile = ToUnitTestPath(config, theFile)
    return __TestGenerateBuildFiles(config, [theFile], PluginSharedValues.PLATFORM_ID_ALL)


def SimpleTestHookFilesEx(theFiles: List[str], config: Config) -> Dict[str, List[Package]]:
    config.ForceDisableAllWrite()
    theFiles = ToUnitTestPaths(config, theFiles)
    return __TestGenerateBuildFiles(config, theFiles, PluginSharedValues.PLATFORM_ID_ALL)


def SimpleTestHookGetPackageLoaderOneFileEx(file: str, config: Config) -> PackageLoader:
    config.ForceDisableAllWrite()
    theFiles = ToUnitTestPaths(config, [file])
    return __TestGetPackageLoader(config, theFiles, PluginSharedValues.PLATFORM_ID_ALL)
