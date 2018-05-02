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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen import PackageConfig
from FslBuildGen import PackageListUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.Config import Config
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.DataTypes import BuildVariantType
from FslBuildGen.DataTypes import GeneratorType
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import VisualStudioVersion
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorAndroidGradleCMake import GeneratorAndroidGradleCMake
from FslBuildGen.Generator.GeneratorAndroidGradleCMake import GeneratorAndroidGradleCMakeUtil
from FslBuildGen.Generator.GeneratorGNUmakefile import GeneratorGNUmakefile
from FslBuildGen.Generator.GeneratorGNUmakefile import GeneratorGNUmakefileUtil
from FslBuildGen.Generator.GeneratorCMake import GeneratorCMake
from FslBuildGen.Generator.GeneratorQNXmakefile import GeneratorQNXmakefile
from FslBuildGen.Generator.GeneratorVC import GeneratorVC
from FslBuildGen.Generator.GeneratorVC import GeneratorVCUtil
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorVariant
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.PackageLoader import PackageLoader
from FslBuildGen.PackageResolver import PackageResolver
from FslBuildGen.SharedGeneration import AndroidABIOption
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import GEN_MAGIC_VARIANT_ANDROID_ABI


class GeneratorPluginAndroid(GeneratorPlugin):
    def __init__(self) -> None:
        super(GeneratorPluginAndroid, self).__init__(PackageConfig.PlatformNameString.ANDROID)
        options = AndroidUtil.GetKnownABIList(True)
        self.VariantAndroidABI = GeneratorVariant(GEN_MAGIC_VARIANT_ANDROID_ABI, options, "##OPTIONS##", BuildVariantType.Static)
        self.OptionAndroidABI_all = AndroidUtil.GetKnownABIList(False)
        self.AddGeneratorVariant(self.VariantAndroidABI)


    def __ContainsFeature(self, allFeatures: List[PackageRequirement], featureId: str) -> bool:
        for feature in allFeatures:
            if feature.Id == featureId:
                return True
        return False


    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = None # type: Optional[GeneratorBase]

        topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)

        androidABIList = self.OptionAndroidABI_all
        if GEN_MAGIC_VARIANT_ANDROID_ABI in config.VariantsDict:
            androidABI = config.VariantsDict[GEN_MAGIC_VARIANT_ANDROID_ABI]
            if androidABI != 'all':
                if not androidABI in self.VariantAndroidABI.Options:
                    raise Exception("'{0}' is not a valid option expected one of these {1}".format(androidABI, self.VariantAndroidABI.Options))
                androidABIList = [androidABI]
        # remove unsupported ABI's if there are others available to build
        if self.__ContainsFeature(topLevelPackage.ResolvedAllUsedFeatures, "vulkan"):
            if len(androidABIList) > 1 and AndroidABIOption.DeprecatedArmeAbi in androidABIList:
                config.LogPrint("INFO: Vulkan does not support ANDROID_ABI '{0}' removing the ABI and building the rest".format(AndroidABIOption.DeprecatedArmeAbi))
                androidABIList.remove(AndroidABIOption.DeprecatedArmeAbi)

        generator = GeneratorAndroidGradleCMake(config, packages, self.Name, androidABIList)
        return self.GenerateDone(config, packages, self.Name, generator)


    def _DoGenerateReport(self, log: Log, packageList: List[Package]) -> Dict[Package, PackageGeneratorReport]:
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorAndroidGradleCMakeUtil.TryGenerateGeneratorPackageReport(log, self.Name, package)
            if buildReport is not None:
                resultDict[package] = buildReport
        return resultDict


class GeneratorPluginUbuntu(GeneratorPlugin):
    def __init__(self) -> None:
        super(GeneratorPluginUbuntu, self).__init__(PackageConfig.PlatformNameString.UBUNTU)

    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = GeneratorGNUmakefile(config, packages, "GNUmakefile", "GNUmakefile_exe", "GNUmakefile_lib", self.Name)
        # FslBuild handles this now so the scripts are not needed
        #GenerateBuildScript.GenerateBuildScriptAll(config, packages)
        return self.GenerateDone(config, packages, self.Name, generator)


    def _DoGenerateReport(self, log: Log, packageList: List[Package]) -> Dict[Package, PackageGeneratorReport]:
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorGNUmakefileUtil.TryGenerateGeneratorPackageReport(log, self.Name, package)
            if buildReport is not None:
                resultDict[package] = buildReport
        return resultDict


class GeneratorPluginQNX(GeneratorPlugin):
    def __init__(self) -> None:
        super(GeneratorPluginQNX, self).__init__(PackageConfig.PlatformNameString.QNX)
        self.InDevelopment = True

    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = GeneratorQNXmakefile(config, packages, "QNXmakefile", "QNXmakefile_exe", "QNXmakefile_lib")
        # FslBuild handles this now so the scripts are not needed
        # GenerateBuildScript.GenerateBuildScriptsQNX(config, packages)
        return self.GenerateDone(config, packages, self.Name, generator)


class GeneratorPluginYocto(GeneratorPlugin):
    def __init__(self) -> None:
        super(GeneratorPluginYocto, self).__init__(PackageConfig.PlatformNameString.YOCTO)


    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = GeneratorGNUmakefile(config, packages, "GNUmakefile_Yocto", "GNUmakefile_Yocto_exe", "GNUmakefile_Yocto_lib", self.Name)
        # FslBuild handles this now so the scripts are not needed
        #GenerateBuildScript.GenerateBuildScriptAll(config, packages)
        return self.GenerateDone(config, packages, self.Name, generator)


    def _DoGenerateReport(self, log: Log, packageList: List[Package]) -> Dict[Package, PackageGeneratorReport]:
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorGNUmakefileUtil.TryGenerateGeneratorPackageReport(log, self.Name, package)
            if buildReport is not None:
                resultDict[package] = buildReport
        return resultDict


class GeneratorPluginWindows(GeneratorPlugin):
    def __init__(self) -> None:
        super(GeneratorPluginWindows, self).__init__(PackageConfig.PlatformNameString.WINDOWS)
        self.InDevelopment = False
        self.ToolVersion = VisualStudioVersion.DEFAULT
        self.SupportContentBuild = True
        self.SupportedPackageLanguages.append(PackageLanguage.CSharp)
        self.PackageResolveConfig_MarkExternalLibFirstUse = True

    #def __PatchVisualStudioPackageIds(self, config, packageLoader, packageResolver):
    #    # We need the windows resolver to acquire the visual studio project id's
    #    windowsPackageResolver = PackageResolver(config, PackageConfig.PlatformNameString.WINDOWS, packageLoader.GenFiles, True)

    #    # First we build a dict for quick lookup
    #    windowsPackageDict = dict()
    #    for package in windowsPackageResolver.Packages:
    #        windowsPackageDict[package.Name] = package

    #    # Then we patch all the packages that we can find in windows as well
    #    for package in packageResolver.Packages:
    #        if package.Name in windowsPackageDict:
    #            winPackage = windowsPackageDict[package.Name]
    #            if package.ResolvedPlatform and winPackage.ResolvedPlatform:
    #                package.ResolvedPlatform.ProjectId = windowsPackageDict[package.Name].ResolvedPlatform.ProjectId


    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        #if self.Name != PackageConfig.PlatformNameString.WINDOWS:
        #    self.__PatchVisualStudioPackageIds(config, packageLoader, packageResolver)

        if platformContext.RecipePathBuilder is None:
            raise Exception("Invalid package")

        activeThirdPartyLibsDir = platformContext.RecipePathBuilder.InstallRootPath
        generator = GeneratorVC(config, packages, self.Name, self.ToolVersion, activeThirdPartyLibsDir)
        #GenerateBuildScript.GenerateWindowsScreenshotScriptAll(config, packages)
        return self.GenerateDone(config, packages, self.Name, generator)


    def _DoGenerateReport(self, log: Log, packageList: List[Package]) -> Dict[Package, PackageGeneratorReport]:
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorVCUtil.TryGenerateGeneratorPackageReport(log, self.Name, package)
            if buildReport is not None:
                resultDict[package] = buildReport
        return resultDict


class GeneratorPluginCMake(GeneratorPlugin):
    def __init__(self) -> None:
        super(GeneratorPluginCMake, self).__init__(PackageConfig.PlatformNameString.CMAKE)
        self.InDevelopment = True
        self.PackageResolveConfig_MarkExternalLibFirstUse = True

    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = GeneratorCMake(config, packages, self.Name)
        return self.GenerateDone(config, packages, self.Name, generator)


#__g_generatorPlugins = [GeneratorPlugin(PluginSharedValues.PLATFORM_ID_ALL), GeneratorPluginAndroid(), GeneratorPluginUbuntu(), GeneratorPluginYocto(), GeneratorPluginWindows(), GeneratorPluginQNX(), GeneratorPluginCMake()]
__g_generatorPlugins = [GeneratorPluginAndroid(), GeneratorPluginUbuntu(), GeneratorPluginYocto(), GeneratorPluginWindows(), GeneratorPluginQNX(), GeneratorPluginCMake()]
__g_generatorPluginDict = {}  # type: Dict[str, GeneratorPlugin]

for _entry in __g_generatorPlugins:
    __g_generatorPluginDict[_entry.Id] = _entry
__g_generatorPluginDict[PluginSharedValues.PLATFORM_ID_ALL] = GeneratorPlugin(PluginSharedValues.PLATFORM_ID_ALL)


#def __CreateCustomWindowGenerator(platformName):
#    gen = GeneratorPluginWindows();
#    gen.SetCustomPlatformName(platformName);
#    return gen


#def SetForceUseNativeGenerator(forceUseNativegenerator):
#    if forceUseNativegenerator or PlatformUtil.DetectBuildPlatformType() != BuildPlatformType.Windows:
#        return
#    # This allows us to use the Visual C++ for linux development plugin on windows
#    ubuntuGen = __CreateCustomWindowGenerator(PackageConfig.PlatformNameString.UBUNTU)
#    __g_generatorPluginDict[ubuntuGen.Id] = ubuntuGen
#    PlatformUtil.AddExtraGenerators(ubuntuGen.Name)


def GetGeneratorPlugins(allowDevelopmentPlugins: bool) -> List[GeneratorPlugin]:
    if allowDevelopmentPlugins:
        return __g_generatorPlugins
    return [entry for entry in __g_generatorPlugins if not entry.InDevelopment]


def GetGeneratorPluginById(pluginId: str, allowAll: bool) -> GeneratorPlugin:
    pluginId = pluginId.lower()
    if not pluginId in __g_generatorPluginDict:
        raise UsageErrorException("Unknown platform: '{0}'".format(pluginId))
    if pluginId == PluginSharedValues.PLATFORM_ID_ALL and not allowAll:
        raise UsageErrorException("Platform '{0}' is not allowed".format(pluginId))
    return __g_generatorPluginDict[pluginId]


def EnableGraph() -> None:
    for entry in __g_generatorPlugins:
        entry.DotEnabled = True


def SetVSVersion(vsVersion: str) -> None:
    nameId = PackageConfig.PlatformNameString.WINDOWS.lower()
    if nameId in __g_generatorPluginDict:
        value = int(vsVersion)
        __g_generatorPluginDict[nameId].ToolVersion = value


def SetGeneratorType(generatorType: str) -> None:
    for entry in list(__g_generatorPluginDict.values()):
        entry.SetGeneratorType(generatorType)
