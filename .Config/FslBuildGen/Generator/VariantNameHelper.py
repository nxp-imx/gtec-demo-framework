#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2017 NXP
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

from typing import List
from typing import Optional
import itertools
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package

class VariantNameHelper(object):
    def __init__(self, log: Log) -> None:
        super().__init__()
        self.Log = log


    def __BuildAllVariantStrings(self, package: Package, variantFormattingString: str, variantNames: List[str]) -> List[str]:
        lists = []
        for name in variantNames:
            if not name in package.ResolvedAllVariantDict:
                raise Exception("Failed to locate the variant '{0}' for package '{1}'".format(name, package.Name))
            variant = package.ResolvedAllVariantDict[name]
            lists.append(list(variant.OptionDict.keys()))

        cartesianProduct = list(itertools.product(*lists))

        variantNamesAsKey = ["$({0})".format(entry) for entry in variantNames]

        finalNames = []
        for variantCombination in cartesianProduct:
            result = variantFormattingString
            for index, entry in enumerate(variantCombination):
                asKey = variantNamesAsKey[index]
                result = result.replace(asKey, entry)
            finalNames.append(result)

        # sort to make it reproducible given the same input
        finalNames.sort()
        return finalNames


    def TryGetPackageVariantNameList(self, package: Package, allowVirtualVariants: bool = False) -> Optional[List[str]]:
        variantFormattingString = package.ResolvedMakeVariantNameHint
        if variantFormattingString is None:
            return None

        if len(variantFormattingString) <= 0:
            return []

        if not allowVirtualVariants and len(package.ResolvedVirtualVariantNameList) > 0:
            self.Log.DoPrintWarning("Could not GetPackageVariantNameList for package '{0}' as it contains virtual variants: {1}".format(package.Name, package.ResolvedVirtualVariantNameList))
            return None

        if len(package.ResolvedVirtualVariantNameList) > 0:
            return None

        return self.__BuildAllVariantStrings(package, variantFormattingString, package.ResolvedNormalVariantNameList)


    def GetPackageVariantNameList(self, package: Package, allowVirtualVariants: bool = False) -> List[str]:
        variantFormattingString = package.ResolvedMakeVariantNameHint
        if variantFormattingString is None:
            raise Exception("Invalid package")

        if len(variantFormattingString) <= 0:
            return []

        if not allowVirtualVariants and len(package.ResolvedVirtualVariantNameList) > 0:
            raise Exception("Could not GetPackageVariantNameList for package '{0}' as it contains virtual variants: {1}".format(package.Name, package.ResolvedVirtualVariantNameList))

        if len(package.ResolvedVirtualVariantNameList) > 0:
            raise Exception("Virtual variants are not supported")

        return self.__BuildAllVariantStrings(package, variantFormattingString, package.ResolvedNormalVariantNameList)
