#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2023 NXP
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
from FslBuildGen.Log import Log
from FslBuildGen.Build.BuildVariantUtil import BuildVariantUtil
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageFlavor
from FslBuildGen.ExternalVariantConstraints import ExternalVariantConstraints
from FslBuildGen.Packages.Package import Package
#from FslBuildGen.Packages.Package import PackagePlatformVariant
#from FslBuildGen.SharedGeneration import GEN_BUILD_ENV_VARIANT_SETTING
#from FslBuildGen.SharedGeneration import ToolAddedVariant

class BuildFlavorUtil(object):
    @staticmethod
    def BuildCompleteFlavorDict(topLevelPackage: Package, preferQuickNames: bool) -> Dict[str, ResolvedPackageFlavor]:
        flavorQuickNameDict = dict() # type: Dict[str, Optional[ResolvedPackageFlavor]]
        flavorDict = dict() # type: Dict[str, ResolvedPackageFlavor]
        for srcPackage in topLevelPackage.ResolvedBuildOrder:
            for flavor in srcPackage.ResolvedFlavorTemplate.PackageFlavors:
                if flavor.Name.Value not in flavorDict:
                    flavorDict[flavor.Name.Value] = flavor
                if preferQuickNames and flavor.QuickName is not None:
                    if flavor.QuickName.Value not in flavorQuickNameDict:
                        flavorQuickNameDict[flavor.QuickName.Value] = flavor
                    else:
                        # Quick name collision -> so the quick name can not be used
                        flavorQuickNameDict[flavor.QuickName.Value] = None

        if preferQuickNames and len(flavorQuickNameDict) > 0:
            # ensure that we dont insert any quick names that collide with the fully qualified names
            for key in flavorQuickNameDict:
                if key in flavorDict:
                    flavorQuickNameDict[key] = None
            # replace any 'full name' with the quick name
            for key, value in flavorQuickNameDict.items():
                if value is not None:
                    del flavorDict[value.Name.Value]
                    flavorDict[key] = value
        return flavorDict

    @staticmethod
    def ValidateUserFlavorSettings(log: Log,
                                   topLevelPackage: Package,
                                   externalVariantConstraints: ExternalVariantConstraints) -> None:
    #    flavorDict = BuildFlavorUtil.BuildCompleteFlavorDict(topLevelPackage)
    #    for key, value in list(userFlavorSettingDict.items()):
    #        if key in flavorDict:
    #            flavor = flavorDict[key]
    #            if not value in flavor.OptionDict:
    #                validValues = list(flavor.OptionDict.keys())
    #                validValues.sort()
    #                raise Exception("Flavor '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))
    #        elif key != ToolAddedFlavor.CONFIG:
    #            log.LogPrintWarning("WARNING: Unused flavor setting '{0}'".format(key))
        BuildVariantUtil.ValidateUserVariantSettings(log, topLevelPackage, externalVariantConstraints)

    @staticmethod
    def LogFlavorSettings(log: Log, externalVariantConstraints: ExternalVariantConstraints) -> None:
        if not externalVariantConstraints.HasConstraints():
            return
        names = list(externalVariantConstraints.Dict.keys())
        names.sort()
        result = []
        for name in names:
            result.append("{0}={1}".format(name.Value, externalVariantConstraints.Dict[name].Value))
        log.LogPrint("Flavor settings: {0}".format(", ".join(result)))

    #@staticmethod
    #def TryLocateFlavor(package: Package, key: str) -> Optional[PackagePlatformFlavor]:
    #    if key in package.ResolvedAllFlavorDict:
    #        return package.ResolvedAllFlavorDict[key]
    #    # try a manual search for 'virtual keys'
    #    for entry in list(package.ResolvedAllFlavorDict.values()):
    #        if key in entry.PurifiedName:
    #            return entry
    #    return None

    #@staticmethod
    #def ExtendEnvironmentDictWithFlavors(log: Log,
    #                                      buildEnv: Dict[str, str],
    #                                      package: Package,
    #                                      userFlavorSettingDict: Dict[str, str]) -> None:
    #    for key, value in list(userFlavorSettingDict.items()):
    #        flavor = BuildFlavorUtil.TryLocateFlavor(package, key)
    #        if flavor is not None:
    #            if flavor.Type == FlavorType.Virtual or (value in flavor.OptionDict):
    #                envName = "{0}{1}".format(GEN_BUILD_ENV_FLAVOR_SETTING, key.upper())
    #                if envName in buildEnv:
    #                    raise Exception("The environment variable {0} has allready been defined".format(envName))
    #                buildEnv[envName] = value
    #            else:
    #                validValues = list(flavor.OptionDict.keys())
    #                validValues.sort()
    #                log.DoPrintWarning("Flavor '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))

    #@staticmethod
    #def CreateCompleteStaticFlavorSettings(resolvedAllFlavorDict: Dict[str, PackagePlatformFlavor],
    #                                       flavorSelectionDict: Dict[str, str]) -> Dict[str, str]:
    #    """
    #    Create a settings dict flavor options in the resolvedAllFlavorDict.
    #    We will use the supplied settings from flavorSelectionDict when available and chose a default option when not.
    #    :param resolvedAllFlavorDict: all resolved flavors from the top level package
    #    :param flavorSelectionDict: the flavor settings currently set
    #    :return: a selection dict containing values for all non dynamic flavors
    #    """
    #    resultDict = {}  # type: Dict[str,str]
    #    for flavor in resolvedAllFlavorDict.values():
    #        value = ""
    #        if flavor.Name in flavorSelectionDict:
    #            value = flavorSelectionDict[flavor.Name]
    #        else:
    #            if len(flavor.Options) <= 0:
    #                raise Exception("Invalid flavor, no options available")
    #            value = flavor.Options[0].Name
    #        resultDict[flavor.Name] = value
    #    return resultDict

