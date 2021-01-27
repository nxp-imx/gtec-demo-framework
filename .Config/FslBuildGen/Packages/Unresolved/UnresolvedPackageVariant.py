#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
from typing import List
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Exceptions import VariantOptionNameCollisionException
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariantOption import UnresolvedPackageVariantOption

class UnresolvedPackageVariant(object):
    def __init__(self, name: str, introducedByPackageName: str, allowExtend: bool, variantType: VariantType,
                 options: List[UnresolvedPackageVariantOption]) -> None:
        super().__init__()
        self.Name = name
        self.IntroducedByPackageName = introducedByPackageName
        self.AllowExtend = allowExtend
        self.Options = options
        self.Type = variantType
        self.OptionDict = UnresolvedPackageVariant.__BuildOptionDict(options)  # type: Dict[str, UnresolvedPackageVariantOption]
        # TODO: verify
        #self.__ValidateVariantName()
        #self.__ValidateOptionNames()

    @staticmethod
    def __BuildOptionDict(options: List[UnresolvedPackageVariantOption]) -> Dict[str, UnresolvedPackageVariantOption]:
        optionDict = {} # type: Dict[str, UnresolvedPackageVariantOption]
        optionNameSet = {}  # type: Dict[str, str]
        for option in options:
            optionDict[option.Name] = option
            key = option.Name.upper()
            if not key in optionNameSet:
                optionNameSet[key] = option.Name
            else:
                raise VariantOptionNameCollisionException(optionNameSet[key], option.Name)
        return optionDict
