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
from FslBuildGen import CMakeConfigUtil
from FslBuildGen import PackageConfig
from FslBuildGen import PackageListUtil
from FslBuildGen import PluginSharedValues
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.BuildConfig.CMakeConfiguration import CMakeConfiguration
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.Config import Config
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import BuildVariantType
from FslBuildGen.DataTypes import GeneratorType
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import VisualStudioVersion
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Generator.GeneratorAndroidGradleCMake import GeneratorAndroidGradleCMake
from FslBuildGen.Generator.GeneratorAndroidGradleCMake import GeneratorAndroidGradleCMakeUtil
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.GeneratorGNUmakefile import GeneratorGNUmakefile
from FslBuildGen.Generator.GeneratorGNUmakefile import GeneratorGNUmakefileUtil
from FslBuildGen.Generator.GeneratorCMake import GeneratorCMake
from FslBuildGen.Generator.GeneratorQNXmakefile import GeneratorQNXmakefile
from FslBuildGen.Generator.GeneratorVC import GeneratorVC
from FslBuildGen.Generator.GeneratorVC import GeneratorVCUtil
from FslBuildGen.Generator.GeneratorVC import GeneratorVSConfig
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.GeneratorPlugin import GeneratorPlugin
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorVariant
from FslBuildGen.Generator.GeneratorVSTemplateInfo import GeneratorVSTemplateInfo
from FslBuildGen.Generator.PluginConfigContext import PluginConfigContext
from FslBuildGen.Generator.Report.GeneratorConfigReport import GeneratorConfigReport
from FslBuildGen.Generator.Report.PackageGeneratorBuildExecutableInfo import PackageGeneratorBuildExecutableInfo
from FslBuildGen.Generator.Report.PackageGeneratorConfigReport import PackageGeneratorConfigReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.Report.TheGeneratorBuildReport import TheGeneratorBuildReport
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.Version import Version
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageLoader import PackageLoader
from FslBuildGen.PackageResolver import PackageResolver
from FslBuildGen.SharedGeneration import AndroidABIOption
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import ToolAddedVariantConfigOption
from FslBuildGen.SharedGeneration import GEN_MAGIC_VARIANT_ANDROID_ABI
from FslBuildGen.Tool.UserCMakeConfig import UserCMakeConfig


class GeneratorPluginAndroid(GeneratorPlugin):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.ANDROID)
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

        generator = GeneratorAndroidGradleCMake(config, packages, self.PlatformName, androidABIList)
        return self.GenerateDone(config, packages, self.PlatformName, generator)


    def _DoGenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> TheGeneratorBuildReport:
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorAndroidGradleCMakeUtil.TryGenerateGeneratorPackageReport(log, self.PlatformName, package)
            if buildReport is not None:
                resultDict[package] = buildReport
        return TheGeneratorBuildReport(resultDict)


class GeneratorPluginMakefile(GeneratorPlugin):
    def __init__(self, platformName: str, makeFilename: str, addCoverageVariant: bool) -> None:
        super().__init__(platformName)
        self.SupportCommandClean = True
        self.SupportCommandInstall = True
        self.MakeFilename = makeFilename
        if addCoverageVariant:
            self.AddGeneratorVariantConfigOption(ToolAddedVariantConfigOption.Coverage)

    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        dstMakeFilename = self.MakeFilename
        templateExe = "{0}_exe".format(self.MakeFilename)
        templateLib = "{0}_lib".format(self.MakeFilename)

        configVariant = self.GeneratorVariants[ToolAddedVariant.CONFIG]
        generator = GeneratorGNUmakefile(config, packages, dstMakeFilename, templateExe, templateLib, self.PlatformName, configVariant.Options)
        return self.GenerateDone(config, packages, self.PlatformName, generator)


    def _DoGenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> TheGeneratorBuildReport:
        configVariant = self.GeneratorVariants[ToolAddedVariant.CONFIG]
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorGNUmakefileUtil.TryGenerateGeneratorPackageReport(log, generatorConfig, self.PlatformName, package, configVariant.Options)
            if buildReport is not None:
                resultDict[package] = buildReport
        return TheGeneratorBuildReport(resultDict)


class GeneratorPluginUbuntuLegacy(GeneratorPluginMakefile):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.UBUNTU, "GNUmakefile", True)


class GeneratorPluginYoctoLegacy(GeneratorPluginMakefile):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.YOCTO, "GNUmakefile_Yocto", False)


class GeneratorPluginQNXLegacy(GeneratorPlugin):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.QNX)
        self.InDevelopment = True

    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = GeneratorQNXmakefile(config, packages, "QNXmakefile", "QNXmakefile_exe", "QNXmakefile_lib")
        return self.GenerateDone(config, packages, self.PlatformName, generator)


class GeneratorPluginWindows(GeneratorPlugin):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.WINDOWS)
        self.InDevelopment = False
        self.ToolVersion = VisualStudioVersion.DEFAULT
        self.SupportContentBuild = True
        self.SupportCommandClean = True
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

        installRootLocation = platformContext.RecipePathBuilder.InstallRootLocation
        activeThirdPartyLibsDir = installRootLocation.ResolvedPath if installRootLocation is not None else None
        generatorConfig = GeneratorVSConfig(self.PlatformName, self.ToolVersion)
        generator = GeneratorVC(config, packages, generatorConfig, activeThirdPartyLibsDir)
        return self.GenerateDone(config, packages, self.PlatformName, generator)


    def _DoGenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> TheGeneratorBuildReport:
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]

        configVariant = self.GeneratorVariants[ToolAddedVariant.CONFIG]

        # vsVersion: int, platformName: str
        generatorVSConfig = GeneratorVSConfig(self.PlatformName, self.ToolVersion)
        generatorTemplateInfo = GeneratorVSTemplateInfo(generatorConfig.ToolConfig.ProjectRootConfig.DefaultTemplate,
                                                        generatorConfig.ToolConfig.TemplateFolder,
                                                        generatorConfig.SDKConfigTemplatePath)

        for package in packageList:
            buildReport = GeneratorVCUtil.TryGenerateGeneratorPackageReport(log, generatorConfig, self.PlatformName, package,
                                                                            generatorVSConfig, generatorTemplateInfo,
                                                                            configVariant.Options)
            if buildReport is not None:
                resultDict[package] = buildReport
        return TheGeneratorBuildReport(resultDict)


class GeneratorPluginCMakeBase(GeneratorPlugin):
    def __init__(self, platformName: str, templateName: Optional[str] = None, overrideTemplateName: Optional[str] = None) -> None:
        super().__init__(platformName)
        self.AddGeneratorVariantConfigOption(ToolAddedVariantConfigOption.Coverage)
        self.InDevelopment = False
        self.IsCMake = True
        self.SupportCommandClean = True
        self.SupportCommandInstall = True
        self.SupportCommandOpen = False
        self.SupportCommandOpenHintMessage = "Please upgrade to cmake 3.11+"
        self.PackageResolveConfig_MarkExternalLibFirstUse = True
        self.__CMakeTemplateName = templateName if templateName is not None else "CMake"
        self.__CMakeOverrideTemplateName = overrideTemplateName

    def SYS_SetCMakeConfig(self, cmakeConfig: Optional[GeneratorCMakeConfig]) -> None:
        super().SYS_SetCMakeConfig(cmakeConfig)
        if cmakeConfig is not None and cmakeConfig.CMakeVersion >= CMakeVersion(3, 11, 0):
            self.SupportCommandOpen = True
            self.SupportCommandOpenHintMessage = ""

    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        generator = GeneratorCMake(config, packages, self.PlatformName, self.__CMakeTemplateName, self.__CMakeOverrideTemplateName)
        return self.GenerateDone(config, packages, self.PlatformName, generator)

    def _DoGenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> TheGeneratorBuildReport:
        if self.CMakeConfig is None:
            raise Exception("internal error generator not configured")

        configVariant = self.GeneratorVariants[ToolAddedVariant.CONFIG]
        resultDict = {} # type: Dict[Package, PackageGeneratorReport]
        for package in packageList:
            buildReport = GeneratorCMake.TryGenerateGeneratorPackageReport(log, generatorConfig, self.CMakeConfig, package, configVariant.Options)
            if buildReport is not None:
                resultDict[package] = buildReport
        return TheGeneratorBuildReport(resultDict)

    def _DoTryGenerateConfigReport(self, log: Log, generatorConfig: GeneratorConfig, topLevelPackage: Package) -> Optional[PackageGeneratorConfigReport]:
        if self.CMakeConfig is None:
            raise Exception("internal error generator not configured")

        configVariant = self.GeneratorVariants[ToolAddedVariant.CONFIG]
        return GeneratorCMake.GenerateGeneratorBuildConfigReport(log, generatorConfig, self.CMakeConfig, topLevelPackage, configVariant.Options)

    def _DoTryGetBuildExecutableInfo(self, log: Log, generatorConfig: GeneratorConfig, package: Package,
                                     generatorReport: PackageGeneratorReport,
                                     variantSettingsDict: Dict[str, str]) -> Optional[PackageGeneratorBuildExecutableInfo]:
        if self.CMakeConfig is None:
            raise Exception("internal error generator not configured")
        configVariant = self.GeneratorVariants[ToolAddedVariant.CONFIG]
        return GeneratorCMake.TryGetBuildExecutableInfo(log, generatorConfig, self.CMakeConfig, package, generatorReport, variantSettingsDict,
                                                        configVariant.Options)


class GeneratorPluginUbuntu(GeneratorPluginCMakeBase):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.UBUNTU)

class GeneratorPluginYocto(GeneratorPluginCMakeBase):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.YOCTO)

class GeneratorPluginQNX(GeneratorPluginCMakeBase):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.QNX)

class GeneratorPluginCMake(GeneratorPluginCMakeBase):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.CMAKE)


class GeneratorPluginFreeRTOS(GeneratorPluginCMakeBase):
    def __init__(self) -> None:
        super().__init__(PackageConfig.PlatformNameString.FREERTOS, None, "CMakeFreeRTOS")
        self.InDevelopment = True


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

class ActualPluginConfigContext(PluginConfigContext):
    def __init__(self, log: Log, toolVersion: Version, allowDevelopmentPlugins: bool) -> None:
        self.DotEnabled = False
        self.VSVersion = 0
        self.LegacyGeneratorType = "default"
        self.__Log = log
        self.__ToolVersion = toolVersion
        # prepare plugins
        self.__GeneratorPlugins = [GeneratorPluginAndroid(), GeneratorPluginUbuntu(),
                                   GeneratorPluginYocto(), GeneratorPluginWindows(),
                                   GeneratorPluginFreeRTOS(), GeneratorPluginQNX(),
                                   GeneratorPluginCMake()]
        if not allowDevelopmentPlugins:
            self.__GeneratorPlugins = [entry for entry in self.__GeneratorPlugins if not entry.InDevelopment]

        self.__GeneratorPluginDict = {} # Dict[str, GeneratorPlugin]
        for _entry in self.__GeneratorPlugins:
            self.__GeneratorPluginDict[_entry.PlatformId] = _entry
        self.__GeneratorPluginDict[PluginSharedValues.PLATFORM_ID_ALL] = GeneratorPlugin(PluginSharedValues.PLATFORM_ID_ALL)

    def GetGeneratorPlugins(self) -> List[GeneratorPlugin]:
        return self.__GeneratorPlugins

    def GetGeneratorPluginById(self, pluginId: str, generatorType: GeneratorType, buildVariantConfig: BuildVariantConfig,
                               allowAll: bool, cmakeConfiguration: CMakeConfiguration, userCMakeConfig: Optional[UserCMakeConfig]) -> GeneratorPlugin:
        generator = self.__GetGenerator(pluginId, generatorType, allowAll)
        # Patch the generator with the global context variables
        generator.DotEnabled =  self.DotEnabled
        generator.ToolVersion = self.VSVersion
        generator.SetLegacyGeneratorType(self.LegacyGeneratorType)

        if isinstance(generator, GeneratorPlugin):
            # patch the GeneratorPlugin generators with cmake configuration data
            generator.SYS_SetCMakeConfig(CMakeConfigUtil.BuildGeneratorCMakeConfig(self.__Log, self.__ToolVersion, generator.PlatformName,
                                                                                   buildVariantConfig, userCMakeConfig, cmakeConfiguration,
                                                                                   generator.ToolVersion))
        return generator

    def EnableGraph(self) -> None:
        for entry in self.__GeneratorPlugins:
            entry.DotEnabled = True
        self.DotEnabled = True

    def SetVSVersion(self, vsVersion: str) -> None:
        nameId = PackageConfig.PlatformNameString.WINDOWS.lower()
        if nameId in self.__GeneratorPluginDict:
            value = int(vsVersion)
            self.__GeneratorPluginDict[nameId].ToolVersion = value
            self.VSVersion = value

    def SetLegacyGeneratorType(self, legacyGeneratorType: str) -> None:
        self.LegacyGeneratorType = legacyGeneratorType

    def __GetGenerator(self, pluginId: str, generatorType: GeneratorType, allowAll: bool) -> GeneratorPlugin:
        pluginId = pluginId.lower()
        generatorPluginDict = self.__GeneratorPluginDict
        if not pluginId in generatorPluginDict:
            raise UsageErrorException("Unknown platform: '{0}'".format(pluginId))
        if pluginId == PluginSharedValues.PLATFORM_ID_ALL and not allowAll:
            raise UsageErrorException("Platform '{0}' is not allowed".format(pluginId))
        if  generatorType == GeneratorType.CMake:
            platformName = generatorPluginDict[pluginId].PlatformName
            return GeneratorPluginCMakeBase(platformName)
        elif  generatorType == GeneratorType.Legacy:
            platformName = generatorPluginDict[pluginId].PlatformName
            if platformName == PlatformNameString.UBUNTU:
                legacyUbuntuGenerator = GeneratorPluginUbuntuLegacy()
                return legacyUbuntuGenerator
            elif platformName == PlatformNameString.YOCTO:
                legacyYoctoGenerator = GeneratorPluginYoctoLegacy()
                return legacyYoctoGenerator
            elif platformName == PlatformNameString.QNX:
                legacyQnxGenerator = GeneratorPluginQNXLegacy()
                return legacyQnxGenerator
        return generatorPluginDict[pluginId]


__g_globalContext = None # type ActualPluginConfigContext


def InitPluginConfigContext(log: Log, toolVersion: Version, allowDevelopmentPlugins: bool) -> PluginConfigContext:
    global __g_globalContext
    if __g_globalContext is None:
        __g_globalContext = ActualPluginConfigContext(log, toolVersion, allowDevelopmentPlugins)
    return __g_globalContext
