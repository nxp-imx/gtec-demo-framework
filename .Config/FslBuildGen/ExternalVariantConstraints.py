#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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
from typing import Optional
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorName import UnresolvedPackageFlavorName

class ExternalVariantConstraints(object):
    def __init__(self, constraintsDict: Dict[UnresolvedPackageFlavorName, PackageFlavorOptionName]) -> None:
        super().__init__()
        self.Dict = constraintsDict
        self.__LookupDict = ExternalVariantConstraints.__ToLookupDict(constraintsDict)

    def AsString(self) -> str:
        res = ""
        for key, value in self.Dict.items():
            if len(res) > 0:
                res = res + ","
            res = "{0}={1}".format(key, value)
        return res

    def TryGetByNameString(self, name: str) -> Optional[PackageFlavorOptionName]:
        return self.__LookupDict[name] if name in self.__LookupDict else None

    def TryGetOptionStringByNameString(self, name: str) -> Optional[str]:
        return self.__LookupDict[name].Value if name in self.__LookupDict else None

    def HasConstraints(self) -> bool:
        return len(self.Dict) > 0

    @staticmethod
    def ToExternalVariantConstraints(srcDict: Dict[str,str]) -> 'ExternalVariantConstraints':
        constraintsDict = dict() # type: Dict[UnresolvedPackageFlavorName, PackageFlavorOptionName]
        for key, value in srcDict.items():
            constraintsDict[UnresolvedPackageFlavorName(key)] = PackageFlavorOptionName(value)
        return ExternalVariantConstraints(constraintsDict)

    @staticmethod
    def __ToLookupDict(srcDict: Dict[UnresolvedPackageFlavorName, PackageFlavorOptionName]) -> Dict[str, PackageFlavorOptionName]:
        resDict = dict() # type: Dict[str, PackageFlavorOptionName]
        for key, value in srcDict.items():
            resDict[key.Value] = value
        return resDict
