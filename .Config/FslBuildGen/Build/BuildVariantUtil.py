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

from typing import Dict
#from typing import List
from typing import Optional
#from FslBuildGen.Build.CaptureEnvironmentVariablesFromScript import CaptureEnvironmentVariablesFromScript
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackagePlatformVariant
from FslBuildGen.SharedGeneration import GEN_BUILD_ENV_VARIANT_SETTING
from FslBuildGen.SharedGeneration import ToolAddedVariant

class BuildVariantUtil(object):
    @staticmethod
    def BuildCompleteVariantDict(topLevelPackage: Package) -> Dict[str, PackagePlatformVariant]:
        variantDict = dict(topLevelPackage.ResolvedAllVariantDict) # type: Dict[str, PackagePlatformVariant]
        return variantDict

    @staticmethod
    def ValidateUserVariantSettings(log: Log,
                                    topLevelPackage: Package,
                                    userVariantSettingDict: Dict[str, str]) -> None:
        variantDict = BuildVariantUtil.BuildCompleteVariantDict(topLevelPackage)
        for key, value in list(userVariantSettingDict.items()):
            if key in variantDict:
                variant = variantDict[key]
                if not value in variant.OptionDict:
                    validValues = list(variant.OptionDict.keys())
                    validValues.sort()
                    raise Exception("Variant '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))
            elif key != ToolAddedVariant.CONFIG:
                log.LogPrintWarning("WARNING: Unused variant setting '{0}'".format(key))

    @staticmethod
    def LogVariantSettings(log: Log, variantSettingsDict: Dict[str, str]) -> None:
        if len(variantSettingsDict) <= 0:
            return
        names = list(variantSettingsDict.keys())
        names.sort()
        result = []
        for name in names:
            result.append("{0}={1}".format(name, variantSettingsDict[name]))
        log.LogPrint("Variant settings: {0}".format(", ".join(result)))

    @staticmethod
    def TryLocateVariant(package: Package, key: str) -> Optional[PackagePlatformVariant]:
        if key in package.ResolvedAllVariantDict:
            return package.ResolvedAllVariantDict[key]
        # try a manual search for 'virtual keys'
        for entry in list(package.ResolvedAllVariantDict.values()):
            if key in entry.PurifiedName:
                return entry
        return None

    @staticmethod
    def ExtendEnvironmentDictWithVariants(log: Log,
                                          buildEnv: Dict[str, str],
                                          package: Package,
                                          userVariantSettingDict: Dict[str, str]) -> None:
        for key, value in list(userVariantSettingDict.items()):
            variant = BuildVariantUtil.TryLocateVariant(package, key)
            if variant is not None:
                if variant.Type == VariantType.Virtual or (value in variant.OptionDict):
                    envName = "{0}{1}".format(GEN_BUILD_ENV_VARIANT_SETTING, key.upper())
                    if envName in buildEnv:
                        raise Exception("The environment variable {0} has allready been defined".format(envName))
                    buildEnv[envName] = value
                else:
                    validValues = list(variant.OptionDict.keys())
                    validValues.sort()
                    log.DoPrintWarning("Variant '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))

    #@staticmethod
    #def ExtractRelevantVariantSettingsDict(config: Config,
    #                                       package: Package,
    #                                       userVariantSettingDict: Dict[str, str]) -> Dict[str, str]:
    #    """ Filters the userVariantSettingsDict down into a dict containing only the entries that are relevant for this package.
    #        It also validates that the user-value is valid for the given variant.
    #    """
    #    dictVariantSettings = {}  # type: Dict[str, str]
    #    for key, value in userVariantSettingDict.items():
    #        if key in package.ResolvedAllVariantDict:
    #            variant = package.ResolvedAllVariantDict[key]
    #            if value in variant.OptionDict:
    #                dictVariantSettings[key] = value
    #            else:
    #                validValues = list(variant.OptionDict.keys())
    #                validValues.sort()
    #                config.DoPrintWarning("Variant '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))
    #    return dictVariantSettings

    @staticmethod
    def CreateCompleteStaticVariantSettings(resolvedAllVariantDict: Dict[str, PackagePlatformVariant],
                                            variantsSelectionDict: Dict[str, str]) -> Dict[str, str]:
        """
        Create a settings dict variant options in the resolvedAllVariantDict.
        We will use the supplied settings from variantsSelectionDict when available and chose a default option when not.
        :param resolvedAllVariantDict: all resolved variants from the top level package
        :param variantsSelectionDict: the variant settings currently set
        :return: a selection dict containing values for all non dynamic variants
        """
        resultDict = {}  # type: Dict[str,str]
        for variant in resolvedAllVariantDict.values():
            if variant.Type == VariantType.Normal:
                value = ""
                if variant.Name in variantsSelectionDict:
                    value = variantsSelectionDict[variant.Name]
                else:
                    if len(variant.Options) <= 0:
                        raise Exception("Invalid variant, no options available")
                    value = variant.Options[0].Name
                resultDict[variant.Name] = value
        return resultDict

    #@staticmethod
    #def CreateCompleteVirtualVariantSettings(log: Log, resolvedAllVariantDict: Dict[str, PackagePlatformVariant]) -> Dict[str,str]:
    #    """
    #    Create a settings dict variant options in the resolvedAllVariantDict.
    #    We will use the supplied settings from variantsSelectionDict when available and chose a default option when not.
    #    :param resolvedAllVariantDict: all resolved variants from the top level package
    #    :param variantsSelectionDict: the variant settings currently set
    #    :return: a selection dict containing values for all non dynamic variants
    #    """
    #    virtualVariants = []  # type: List[PackagePlatformVariant]
    #    for variant in resolvedAllVariantDict.values():
    #        if variant.Type == VariantType.Virtual:
    #            virtualVariants.append(variant)

    #    resultDict = {}  # type: Dict[str,str]
    #    if len(virtualVariants) > 0:
    #        log.LogPrint("Trying to capture variants {0}".format([entry.Name for entry in virtualVariants]))

    #        environmentVariableList = []   # type: List[str]
    #        for variant in virtualVariants:
    #            if len(variant.Options) != 1:
    #                raise Exception("unsupported virtual variant option length")
    #            for externalDependency in variant.Options[0].ExternalDependencies:
    #                if externalDependency.Include is not None:
    #                    environmentVariableList.append(externalDependency.Include)
    #        if len(environmentVariableList) > 0:
    #            envDict = CaptureEnvironmentVariablesFromScript.Capture(log, environmentVariableList)
    #            print(envDict)

    #    return resultDict

    #@staticmethod
    #def CreateCompleteVariantSettings(log: Log, resolvedAllVariantDict: Dict[str, PackagePlatformVariant],
    #                                  variantsSelectionDict: Dict[str, str]) -> Dict[str,str]:
    #    """
    #    Create a settings dict variant options in the resolvedAllVariantDict.
    #    We will use the supplied settings from variantsSelectionDict when available and chose a default option when not.
    #    Virtual variants will be captured.
    #    :param resolvedAllVariantDict: all resolved variants from the top level package
    #    :param variantsSelectionDict: the variant settings currently set
    #    :return: a selection dict containing values for all non dynamic variants
    #    """
    #    # check for unsupported types
    #    for variant in resolvedAllVariantDict.values():
    #        if variant.Type != VariantType.Normal and variant.Type != VariantType.Virtual:
    #            raise Exception("Unsupported VariantType: {0}".format(variant.Type))

    #    finalDict = BuildVariantUtil.CreateCompleteStaticVariantSettings(resolvedAllVariantDict, variantsSelectionDict)
    #    dynamicDict = BuildVariantUtil.CreateCompleteVirtualVariantSettings(log, resolvedAllVariantDict)
    #    for key, value in dynamicDict:
    #        if key in finalDict:
    #            raise Exception("variant error")
    #        finalDict[key] = value
    #    return finalDict
