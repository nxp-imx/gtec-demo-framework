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
from FslBuildGen.Config import Config
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.DataTypes import BuildVariantType
from FslBuildGen.DataTypes import LegacyGeneratorType
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorDot import GeneratorDot
from FslBuildGen.Generator.GeneratorGitIgnore import GeneratorGitIgnore
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorVariant
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorPluginBase2
from FslBuildGen.Log import Log
from FslBuildGen.PackageLoader import PackageLoader
from FslBuildGen.Packages.Package import Package
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import ToolAddedVariantOptions


GENERATOR_TYPES = {
    "default" : LegacyGeneratorType.Default,
    "deprecated" : LegacyGeneratorType.Deprecated,
    "experimental" : LegacyGeneratorType.Experimental
}



class GeneratorPlugin(GeneratorPluginBase2):
    def __init__(self, log: Log, platformName: str) -> None:
        super().__init__(platformName)
        self.Log = log
        self.OriginalPlatformId = self.PlatformId
        self.InDevelopment = False
        self.DotEnabled = False
        # If this is set the native build file is expected to run FslBuildContent
        # If it is false FslBuild will run it during the build
        self.SupportContentBuild = False
        self.SupportCommandClean = False
        self.SupportCommandInstall = False
        self.SupportCommandOpen = False
        self.SupportCommandOpenHintMessage = ""
        self.LegacyGeneratorType = LegacyGeneratorType.Default
        self.PackageResolveConfig_MarkExternalLibFirstUse = False
        # Add the default 'config' variant
        self.AddGeneratorVariant(GeneratorVariant(ToolAddedVariant.CONFIG, ToolAddedVariantOptions.CONFIG, "##OPTIONS##", BuildVariantType.Dynamic))
        self.SupportedPackageLanguages = [PackageLanguage.CPP]

    #def SetCustomPlatformName(self, name):
    #    """ Change the platform name """
    #    self.Name = name;
        #self.Id = name.lower()


    def SetLegacyGeneratorType(self, legacyGeneratorType: str) -> None:
        if legacyGeneratorType in GENERATOR_TYPES:
            self.LegacyGeneratorType = GENERATOR_TYPES[legacyGeneratorType]
        else:
            raise Exception("Unsupported generator type: '{0}'".format(legacyGeneratorType))


    def AddGeneratorVariant(self, generatorVariant: GeneratorVariant) -> None:
        """ protected. intended for generators so they can add their variant """
        if generatorVariant.Name in self.GeneratorVariants:
            raise Exception("The variant name already exist '{0}'".format(generatorVariant.Name))
        self.GeneratorVariants[generatorVariant.Name] = generatorVariant

    def AddGeneratorVariantConfigOption(self, optionName: str) -> None:
        variant = self.GeneratorVariants[ToolAddedVariant.CONFIG]
        variant.Options.append(optionName)
        if len(variant.Description) > 0:
            variant.Description = "{0},{1}".format(variant.Description, optionName)
        else:
            variant.Description = optionName



    def Generate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        """ General generate method, does a bit of processing then calls the plugin DoGenerate method """
        if not config.ToolConfig.DefaultPackageLanguage in self.SupportedPackageLanguages:
            raise UnsupportedException("The package language '{0}' is not supported by the generator '{1}'".format(PackageLanguage.ToString(config.ToolConfig.DefaultPackageLanguage), self.PlatformName))
        return self.DoGenerate(platformContext, config, packages)


    def DoGenerate(self, platformContext: PlatformContext, config: Config, packages: List[Package]) -> List[Package]:
        return []


    def GenerateDone(self, config: Config, packages: List[Package], name: str, activeGenerator: GeneratorBase) -> List[Package]:
        toolConfig = config.ToolConfig
        configSDKConfigTemplatePath = config.SDKConfigTemplatePath
        configDisableWrite = config.DisableWrite

        if self.DotEnabled:
            GeneratorDot(self.Log, toolConfig, packages, name)
        #if config.IsQuery:
        #    GenerateQuery.Answer(config, packages, name)

        self.__GenerateGitIgnore(configSDKConfigTemplatePath, configDisableWrite, packages, name, activeGenerator)
        self.LastActiveGenerator = activeGenerator
        return packages


    def __GenerateGitIgnore(self, configSDKConfigTemplatePath: str, configDisableWrite: bool, packages: List[Package], name: str, activeGenerator: GeneratorBase) -> None:
        ignore = GeneratorGitIgnore(configSDKConfigTemplatePath, configDisableWrite, packages, name, activeGenerator)


