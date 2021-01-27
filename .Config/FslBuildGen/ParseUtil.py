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
#from typing import Union
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.RecipeFilterManager import RecipeFilterManager
from FslBuildGen.RecipeFilterName import RecipeFilterName
from FslBuildGen.QualifiedRequirementExtensionName import QualifiedRequirementExtensionName
from FslBuildGen import Util


def ParseList(strSrcList: str, message: str, allowWildcard: bool) -> List[str]:
    if not strSrcList:
        return []
    if allowWildcard and strSrcList == '*':
        return ['*']
    if not strSrcList.startswith('[') and not strSrcList.endswith('['):
        raise Exception("Expected a {0} list in the format '[{0},{0}]' not '{1}'".format(message, strSrcList))

    strSrcList = strSrcList[1:-1]
    if len(strSrcList) == 0:
        return []
    parsedList = strSrcList.split(',')
    return parsedList if not '*' in parsedList else ['*']

def ParseFilterList(strSrcList: str, message: str, allowWildcard: bool) -> List[str]:
    if not strSrcList:
        return []
    if allowWildcard and strSrcList == '*':
        return ['*']
    if not strSrcList.startswith('[') and not strSrcList.endswith('['):
        raise Exception("Expected a {0} list in the format '[{0},{0}]' not '{1}'".format(message, strSrcList))

    strSrcList = strSrcList[1:-1]
    if len(strSrcList) == 0:
        return []
    parsedList = strSrcList.split(',')
    return parsedList


# Do some minimal basic validation of the requirement name input
def __ValidateRequirementList(requirementNameList: List[str], strHelpListName: str, strHelpEntryName: str) -> None:
    if len(requirementNameList) <= 0 or requirementNameList[0] == '*':
        return

    for entry in requirementNameList:
        if not Util.IsValidRequirementName(entry):
            raise Exception("The {0} must be valid, the {1} name '{2}' is not a valid {1} name in list {3}".format(strHelpListName, strHelpEntryName, entry, requirementNameList))


def ParseExtensionList(strExtensionList: str) -> ExtensionListManager:
    parsedList = ParseList(strExtensionList, "extension", True)
    if not '*' in parsedList:
        newParsedList = []
        for entry in parsedList:
            # Do some minimal basic validation of the input
            # All extensions has to be qualified with a feature "featureName:extensionName
            values = entry.split(':')
            if not len(values) == 2:
                raise Exception("The extension list must be valid, the extension '{0}' did not follow the expected '<FeatureName>:<ExtensionName>' format".format(entry))
            if not Util.IsValidRequirementName(values[0]):
                raise Exception("The extension list must be valid, the extension '{0}' did not contain a valid feature name '{1}'".format(entry, values[0]))
            if not Util.IsValidRequirementName(values[1]):
                raise Exception("The extension list must be valid, the extension '{0}' did not contain a valid extension name '{1}'".format(entry, values[1]))
            newParsedList.append(QualifiedRequirementExtensionName(values[0], values[1]))
        return ExtensionListManager(False, newParsedList)
    return ExtensionListManager(True, [])


def ParseRecipeList(strRecipeFilterList: str) -> RecipeFilterManager:
    parsedList = ParseFilterList(strRecipeFilterList, "recipe", True)
    containsWildCard = '*' in parsedList
    newParsedList = []
    for entry in parsedList:
        # Do some minimal basic validation of the input
        if entry != '*':
            newParsedList.append(RecipeFilterName(entry))
    return RecipeFilterManager(containsWildCard, newParsedList)


def ParsePackageTypeList(strPackageTypeList: str) -> List[str]:
    parsedList = ParseList(strPackageTypeList, "packageType", True)
    if not '*' in parsedList:
        validPackageTypes = PackageType.AllStrings()
        for entry in parsedList:
            if entry not in validPackageTypes:
                raise Exception("The package type list must be valid, the package type '{0}' is not, valid types {1}".format(entry, validPackageTypes))
    return parsedList


def ParseFeatureList(features: str) -> List[str]:
    parsedList = ParseList(features, "feature", True)
    __ValidateRequirementList(parsedList, "feature list", "feature")
    return parsedList


def ParseBool(value: str) -> bool:
    if value is None:
        return False
    value = value.lower()
    if value == '1' or value == 'true' or value == 'on':
        return True
    elif value == '0' or value == 'false' or value == 'off':
        return False
    else:
        raise Exception("Unsupported bool value '{0}'".format(value))


def ParseVariantDict(variants: Optional[str]) -> Dict[str, str]:
    if not variants:
        return {}
    if not variants.startswith('[') and not variants.endswith('['):
        raise Exception("Expected a variant list in the format '[variant=value,variant=value]' not '%s'" % (variants))

    variants = variants[1:-1]
    if len(variants) == 0:
        return {}
    entries = variants.split(',')
    variantDict = {}  # type: Dict[str, str]
    for entry in entries:
        pair = entry.split('=')
        if len(pair) != 2:
            raise Exception("Expected the variant config to be in the format 'key=value' not '{0}'".format(entry))
        if len(pair[0]) <= 0:
            raise Exception("The variant name must be valid not empty '{0}'".format(entry))
        if len(pair[1]) <= 0:
            raise Exception("The variant value must be valid not empty '{0}'".format(entry))
        if pair[0] in variantDict:
            raise Exception("The variant '{0}' has already been configured ('{1}')".format(pair[0], entry))
        variantDict[pair[0]] = pair[1]
    return variantDict
