#!/usr/bin/env python

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

from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen import PackageConfig, PlatformUtil
from FslBuildGen.SharedGeneration import *
from FslBuildGen.PackageResolver import PackageResolver
from FslBuildGen.Generator import GenerateQuery
from FslBuildGen.Generator.GeneratorAndroid import GeneratorAndroid
from FslBuildGen.Generator.GeneratorAndroidGradleCMake import GeneratorAndroidGradleCMake
from FslBuildGen.Generator.GeneratorGNUmakefile import GeneratorGNUmakefile
from FslBuildGen.Generator.GeneratorVC import GeneratorVC
from FslBuildGen.Generator.GeneratorDot import GeneratorDot
from FslBuildGen.Generator.GeneratorQNXmakefile import GeneratorQNXmakefile
from FslBuildGen.Generator.GeneratorGitIgnore import GeneratorGitIgnore
from FslBuildGen.Generator.GeneratorCMake import GeneratorCMake
from FslBuildGen.PackageListUtil import GetTopLevelPackage

PLATFORM_ID_ALL = "all"
TYPE_DEFAULT = "default"
TYPE_UNIT_TEST = "UnitTest"

GENERATOR_TYPES = {
    "default" : GeneratorType.Default,
    "deprecated" : GeneratorType.Deprecated,
    "experimental" : GeneratorType.Experimental
}

class GeneratorVariant(object):
    def __init__(self, name, options, description, type):
        super(GeneratorVariant, self).__init__()
        self.Name = name
        self.Options = options
        self.Description = description.replace("##OPTIONS##", ",".join(options))
        self.Type = type


class GeneratorPlugin(object):
    def __init__(self, name):
        super(GeneratorPlugin, self).__init__()
        self.Name = name;
        self.Id = name.lower()
        self.OriginalName = self.Name
        self.OriginalId = self.Id
        self.InDevelopment = False
        self.DotEnabled = False
        # If this is set the native build file is expected to run FslBuildContent
        # If it is false FslBuild will run it during the build
        self.SupportContentBuild = False
        self.LastActiveGenerator = None
        self.GeneratorType = GeneratorType.Default
        self.GeneratorVariants = {}
        # Add the default 'config' variant        
        self.AddGeneratorVariant(GeneratorVariant(GEN_MAGIC_VARIANT, ["Debug", "Release"], "##OPTIONS##", BuildVariantType.Dynamic))
        self.SupportedPackageLanguages = [PackageLanguage.CPP]


    def SetCustomPlatformName(self, name):
        """ Change the platform name """
        self.Name = name;
        self.Id = name.lower()


    def SetGeneratorType(self, generatorType):
        if generatorType in GENERATOR_TYPES:
            self.GeneratorType = GENERATOR_TYPES[generatorType]
        else:
            raise Exception("Unsupported generator type: '%s'" % generatorType);


    def GetVariants(self):
        """ Get information about the variants introduced by this generator """
        return list(self.GeneratorVariants.values())


    def AddGeneratorVariant(self, generatorVariant):
        """ protected. intended for generators so they can add their variant """
        if generatorVariant.Name in self.GeneratorVariants:
            raise Exception("The variant name already exist '%s'" % generatorVariant.name)
        self.GeneratorVariants[generatorVariant.Name] = generatorVariant

    
    def Generate(self, config, packageLoader):
        """ General generate method, does a bit of processing the calles the plugin DoGenerate method """
        if not config.ToolConfig.DefaultPackageLanguage in self.SupportedPackageLanguages:
            raise UnsupportedException("The package language '{0}' is not supported by the generator '{1}'".format(PackageLanguage.ToString(config.ToolConfig.DefaultPackageLanguage), self.Name))
        return self.DoGenerate(config, packageLoader)

    def DoGenerate(self, config, packageLoader):
        pass


    def GenerateDone(self, config, packages, name, activeGenerator):
        if self.DotEnabled:
            GeneratorDot(config, packages, name)
        if config.IsQuery:
            GenerateQuery.Answer(config, packages, name)

        self.__GenerateGitIgnore(config, packages, name, activeGenerator)
        self.LastActiveGenerator = activeGenerator
        return packages


    def __GenerateGitIgnore(self, config, packages, name, activeGenerator):
        ignore = GeneratorGitIgnore(config, packages, name, activeGenerator)


class GeneratorPluginAndroid(GeneratorPlugin):
    def __init__(self):
        super(GeneratorPluginAndroid, self).__init__(PackageConfig.PLATFORM_ANDROID)
        options = [
            AndroidABIOption.All, 
            #AndroidABIOption.ArmeAbi, (does not support exception_ptr)
            AndroidABIOption.ArmeAbiV7a, 
            AndroidABIOption.Arm64V8a, 
            AndroidABIOption.X86, 
            AndroidABIOption.X86_64, 
            AndroidABIOption.Mips, 
            AndroidABIOption.Mips64
        ]
        self.VariantAndroidABI = GeneratorVariant(GEN_MAGIC_VARIANT_ANDROID_ABI, options, "##OPTIONS##", BuildVariantType.Static)
        self.OptionAndroidABI_all = self.__GetARMABIAllList(self.VariantAndroidABI.Options, AndroidABIOption.All)
        self.AddGeneratorVariant(self.VariantAndroidABI)


    def __GetARMABIAllList(self, options, blacklisted):
        options = list(options)
        options.remove(blacklisted)
        return options

    def __ContainsFeature(self, allFeatures, featureId):
        for feature in allFeatures:
            if feature.Id == featureId:
                return True
        return False

    def DoGenerate(self, config, packageLoader):
        packageResolver = PackageResolver(config, self.Name, packageLoader.GenFiles, False)
        generator = None

        topLevelPackage = GetTopLevelPackage(packageResolver.Packages)

        androidABIList = self.OptionAndroidABI_all
        if GEN_MAGIC_VARIANT_ANDROID_ABI in config.VariantsDict:
            androidABI = config.VariantsDict[GEN_MAGIC_VARIANT_ANDROID_ABI]
            if androidABI != 'all':
                if not androidABI in self.VariantAndroidABI.Options:
                    raise Exception("'%s' is not a valid option expected one of these %s" % (androidABI, self.VariantAndroidABI.Options) );
                androidABIList = [androidABI]
        # remove unsupported ABI's if there are others available to build
        if self.__ContainsFeature(topLevelPackage.ResolvedAllUsedFeatures, "vulkan"):
            if len(androidABIList) > 1 and AndroidABIOption.ArmeAbi in androidABIList:
                config.LogPrint("INFO: Vulkan does not support ANDROID_ABI '%s' removing the ABI and building the rest" % (AndroidABIOption.ArmeAbi));
                androidABIList.remove(AndroidABIOption.ArmeAbi)

        if self.GeneratorType == GeneratorType.Deprecated:
            config.LogPrint("Using the deprecated GeneratorAndroid by request");
            generator = GeneratorAndroid(config, packageResolver.Packages, self.Name)
        else:
            generator = GeneratorAndroidGradleCMake(config, packageResolver.Packages, self.Name, androidABIList)
        return self.GenerateDone(config, packageResolver.Packages, self.Name, generator)


class GeneratorPluginUbuntu(GeneratorPlugin):
    def __init__(self):
        super(GeneratorPluginUbuntu, self).__init__(PackageConfig.PLATFORM_UBUNTU)

    def DoGenerate(self, config, packageLoader):
        packageResolver = PackageResolver(config, self.Name, packageLoader.GenFiles, False)
        generator = GeneratorGNUmakefile(config, packageResolver.Packages, "GNUmakefile", "GNUmakefile_exe", "GNUmakefile_lib")
        # FslBuild handles this now so the scripts are not needed
        #GenerateBuildScript.GenerateBuildScriptAll(config, packageResolver.Packages)
        return self.GenerateDone(config, packageResolver.Packages, self.Name, generator)


class GeneratorPluginQNX(GeneratorPlugin):
    def __init__(self):
        super(GeneratorPluginQNX, self).__init__(PackageConfig.PLATFORM_QNX)
        self.InDevelopment = True

    def DoGenerate(self, config, packageLoader):
        packageResolver = PackageResolver(config, self.Name, packageLoader.GenFiles, False)
        generator = GeneratorQNXmakefile(config, packageResolver.Packages, "QNXmakefile", "QNXmakefile_exe", "QNXmakefile_lib")
        # FslBuild handles this now so the scripts are not needed
        # GenerateBuildScript.GenerateBuildScriptsQNX(config, packageResolver.Packages)
        return self.GenerateDone(config, packageResolver.Packages, self.Name, generator)


class GeneratorPluginYocto(GeneratorPlugin):
    def __init__(self):
        super(GeneratorPluginYocto, self).__init__(PackageConfig.PLATFORM_YOCTO)

    def DoGenerate(self, config, packageLoader):
        packageResolver = PackageResolver(config, self.Name, packageLoader.GenFiles, False)
        generator = GeneratorGNUmakefile(config, packageResolver.Packages, "GNUmakefile_Yocto", "GNUmakefile_Yocto_exe", "GNUmakefile_Yocto_lib")
        # FslBuild handles this now so the scripts are not needed
        #GenerateBuildScript.GenerateBuildScriptAll(config, packageResolver.Packages)
        return self.GenerateDone(config, packageResolver.Packages, self.Name, generator)


class GeneratorPluginWindows(GeneratorPlugin):
    def __init__(self):
        super(GeneratorPluginWindows, self).__init__(PackageConfig.PLATFORM_WINDOWS)
        self.InDevelopment = False
        self.VSVersion = VisualStudioVersion.Latest
        self.SupportContentBuild = True
        self.SupportedPackageLanguages.append(PackageLanguage.CSharp)

    def __PatchVisualStudioPackageIds(self, config, packageLoader, packageResolver):
        # We need the windows resolver to acquire the visual studio project id's
        windowsPackageResolver = PackageResolver(config, PackageConfig.PLATFORM_WINDOWS, packageLoader.GenFiles, True)

        # First we build a dict for quick lookup
        windowsPackageDict = dict()
        for package in windowsPackageResolver.Packages:
            windowsPackageDict[package.Name] = package

        # Then we patch all the packages that we can find in windows as well
        for package in packageResolver.Packages:
            if package.Name in windowsPackageDict:
                winPackage = windowsPackageDict[package.Name]
                if package.ResolvedPlatform and winPackage.ResolvedPlatform:
                    package.ResolvedPlatform.ProjectId = windowsPackageDict[package.Name].ResolvedPlatform.ProjectId


    def DoGenerate(self, config, packageLoader):
        packageResolver = PackageResolver(config, self.Name, packageLoader.GenFiles, True)
        if self.Name != PackageConfig.PLATFORM_WINDOWS:
            self.__PatchVisualStudioPackageIds(config, packageLoader, packageResolver)

        generator = GeneratorVC(config, packageResolver.Packages, self.Name, self.VSVersion)
        #GenerateBuildScript.GenerateWindowsScreenshotScriptAll(config, packageResolver.Packages)
        return self.GenerateDone(config, packageResolver.Packages, self.Name, generator)


class GeneratorPluginCMake(GeneratorPlugin):
    def __init__(self):
        super(GeneratorPluginCMake, self).__init__(PackageConfig.PLATFORM_CMAKE)
        self.InDevelopment = True

    def DoGenerate(self, config, packageLoader):
        packageResolver = PackageResolver(config, self.Name, packageLoader.GenFiles, True)
        generator = GeneratorCMake(config, packageResolver.Packages, self.Name)
        return self.GenerateDone(config, packageResolver.Packages, self.Name, generator)


__g_generatorPlugins = None
__g_generatorPlugins = [GeneratorPlugin(PLATFORM_ID_ALL), GeneratorPluginAndroid(), GeneratorPluginUbuntu(), GeneratorPluginYocto(), GeneratorPluginWindows(), GeneratorPluginQNX(), GeneratorPluginCMake()]
__g_generatorPluginDict = {}

for entry in __g_generatorPlugins:
    __g_generatorPluginDict[entry.Id] = entry


def __CreateCustomWindowGenerator(platformName):
    gen = GeneratorPluginWindows();
    gen.SetCustomPlatformName(platformName);
    return gen


def SetForceUseNativeGenerator(forceUseNativegenerator):
    if forceUseNativegenerator or PlatformUtil.DetectBuildPlatformType() != BuildPlatformType.Windows:
        return
    # This allows us to use the Visual C++ for linux development plugin on windows
    ubuntuGen = __CreateCustomWindowGenerator(PackageConfig.PLATFORM_UBUNTU)
    __g_generatorPluginDict[ubuntuGen.Id] = ubuntuGen
    PlatformUtil.AddExtraGenerators(ubuntuGen.Name)


def GetGeneratorPlugins(allowDevelopmentPlugins):
    if allowDevelopmentPlugins:
        return __g_generatorPlugins
    else:
        return [entry for entry in __g_generatorPlugins if not entry.InDevelopment]


def GetGeneratorPluginById(id):
    id = id.lower()
    if not id in __g_generatorPluginDict:
        raise UsageErrorException("Unknown platform: '%s'" % (id))
    return __g_generatorPluginDict[id]


def EnableGraph():
  for entry in __g_generatorPlugins:
      entry.DotEnabled = True;


def SetVSVersion(vsVersion):
    id = PackageConfig.PLATFORM_WINDOWS.lower()
    if id in __g_generatorPluginDict:
        value = int(vsVersion)
        __g_generatorPluginDict[id].VSVersion = value


def SetGeneratorType(generatorType):
    for entry in __g_generatorPluginDict.values():
        entry.SetGeneratorType(generatorType)
