#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from typing import Optional
#from FslBuildGen import Util
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorUnqualifiedName import UnresolvedPackageFlavorUnqualifiedName
#from FslBuildGen.Exceptions import InvalidPackageFlavorNameException

class PackageFlavorName(object):
    def __init__(self, ownerPackageName: UnresolvedPackageName, name: UnresolvedPackageFlavorUnqualifiedName) -> None:
        super().__init__()

        self.OwnerPackageName = ownerPackageName
        self.Value = "{0}.{1}".format(ownerPackageName.Value, name) if len(name.Value) > 0 else ownerPackageName.Value
        self.Id = self.Value.upper()

    def CompareTo(self, other: 'PackageFlavorName') -> int:
        if self.Value < other.Value:
            return -1
        return 0 if self.Value == other.Value else 1

    def __hash__(self) -> int:
        return self.Value.__hash__()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, PackageFlavorName):
            return NotImplemented
        return self.Value == other.Value

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, PackageFlavorName):
            return NotImplemented
        return self.Value < other.Value

    def __le__(self, other: object) -> bool:
        if not isinstance(other, PackageFlavorName):
            return NotImplemented
        return self.Value <= other.Value

    def __gt__(self, other: object) -> bool:
        if not isinstance(other, PackageFlavorName):
            return NotImplemented
        return self.Value > other.Value

    def __ge__(self, other: object) -> bool:
        if not isinstance(other, PackageFlavorName):
            return NotImplemented
        return self.Value > other.Value

    def __str__(self) -> str:
        return self.Value

    def __repr__(self) -> str:
        return "PackageFlavorName({0})".format(self.Value)

    @staticmethod
    def FromString(value: str) -> 'PackageFlavorName':
        res = PackageFlavorName.TryFromString(value)
        if res is None:
            raise Exception("Invalid package flavor name '{0}', package flavor names must contain atleast one '.'".format(value))
        return res

    @staticmethod
    def TryFromString(value: str) -> Optional['PackageFlavorName']:
        lastDotIndex = value.rfind('.')
        if lastDotIndex < 0:
            return None
        return PackageFlavorName(UnresolvedPackageName(value[:lastDotIndex]), UnresolvedPackageFlavorUnqualifiedName(value[lastDotIndex+1:]))
