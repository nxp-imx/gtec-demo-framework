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
from typing import Tuple
import itertools
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackagePlatformVariant

class VariantHelper(object):
    def __init__(self, package: Package) -> None:
        self.Package = package
        allVariants = list(package.ResolvedAllVariantDict.values())
        self.VariantTypeDict = self.__FilterVariants(allVariants)


        if VariantType.Normal in self.VariantTypeDict:
            self.NormalVariants = self.VariantTypeDict[VariantType.Normal]
            # We sort this to enforce a normalized 'ordering' of the variants
            self.NormalVariants.sort(key=lambda s: s.Name.lower())
            # build a list like this [ variant0-option-list, variant1-option-list ]
            optionsPerVariantList = []  # type: List[List[str]]
            for variant in self.NormalVariants:
                variantOptionList = []  # type: List[str]
                for option in variant.Options:
                    variantOptionList.append(option.Name)
                optionsPerVariantList.append(variantOptionList)
            #Cartesian product of all possible configurations (yes variants are dangerous)
            self.CartesianProduct = list(itertools.product(*optionsPerVariantList))  # type: List[Tuple[str, ...]]
            #self.CartesianProduct = self.__SortCartesianProduct(cartesianProduct)
        else:
            self.CartesianProduct = []
            self.NormalVariants = []

        # The normal variant format string each variant is inserted as a ${VARIANT_NAME} in the string
        self.ResolvedNormalVariantNameHint = self.__GetNormalVariantsVariableFormatString()  # type: str


    def __GetNormalVariantsVariableFormatString(self) -> str:
        variantVariableNameList = []
        for platformVariant in self.NormalVariants:
            variantVariableNameList.append("${{{0}}}".format(platformVariant.Name))
        return "".join(variantVariableNameList)


    def __FilterVariants(self, variants: List[PackagePlatformVariant]) -> Dict[int, List[PackagePlatformVariant]]:
        resDict = {}  # type: Dict[int, List[PackagePlatformVariant]]
        for entry in variants:
            if not entry.Type in resDict:
                resDict[entry.Type] = []
            resDict[entry.Type].append(entry)
        return resDict

    #def __SortCartesianProduct(self, cartesianProduct):

    #    return cartesianProduct
