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

from typing import List
from FslBuildGen import Util
from FslBuildGen.Engine.PackageFlavorSelection import PackageFlavorSelection
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelectionsEmpty
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorConfig import UnresolvedPackageFlavorConfig
from FslBuildGen.Exceptions import InvalidPackageNameException

class PackageName(object):
    def __init__(self, name: str, smartName: str, baseName: UnresolvedPackageName, flavorSelections: PackageFlavorSelections) -> None:
        super().__init__()
        if not Util.IsValidPackageName(name):
            raise InvalidPackageNameException(name)

        self.Value = name
        self.SmartValue = smartName
        self.Unresolved = baseName
        self.FlavorSelections = flavorSelections

    def CompareTo(self, other: 'PackageName') -> int:
        if self.Value < other.Value:
            return -1
        return 0 if self.Value == other.Value else 1

    def __hash__(self) -> int:
        return self.Value.__hash__()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, PackageName):
            return NotImplemented
        return self.Value == other.Value

    def __lt__(self, other: object) -> bool:
        if not isinstance(other, PackageName):
            return NotImplemented
        return self.Value < other.Value

    def __le__(self, other: object) -> bool:
        if not isinstance(other, PackageName):
            return NotImplemented
        return self.Value <= other.Value

    def __gt__(self, other: object) -> bool:
        if not isinstance(other, PackageName):
            return NotImplemented
        return self.Value > other.Value

    def __ge__(self, other: object) -> bool:
        if not isinstance(other, PackageName):
            return NotImplemented
        return self.Value > other.Value

    def __str__(self) -> str:
        return self.Value

    def __repr__(self) -> str:
        return "PackageName({0})".format(self.Value)


    @staticmethod
    def __CreateFullName2(packageName: 'PackageName', flavorSelections: PackageFlavorSelections) -> str:
        return PackageName.__CreateFullName(packageName.Unresolved, flavorSelections)

    @staticmethod
    def __CreateFullName(packageName: UnresolvedPackageName, flavorSelections: PackageFlavorSelections) -> str:
        if flavorSelections is not None and len(flavorSelections.Selections) >= 1:
            return "{0}___{1}".format(packageName, PackageName.__GetFlavorConfigName(flavorSelections.Selections))
        return packageName.Value

    @staticmethod
    def __GetFlavorConfigName(flavorSelections: List[PackageFlavorSelection]) -> str:
        tmp = []  # type: List[str]
        for flavorSelection in flavorSelections:
            tmp.append(flavorSelection.Option.Value)
        return "__".join(tmp)

    @staticmethod
    def __ToPackageFlavorSelections(flavorConfigs: List[UnresolvedPackageFlavorConfig]) -> PackageFlavorSelections:
        if len(flavorConfigs) <= 0:
            return PackageFlavorSelectionsEmpty.Empty

        res = [] # type: List[PackageFlavorSelection]
        for flavorConfig in flavorConfigs:
            res.append(PackageFlavorSelection(flavorConfig.Name, flavorConfig.Option.Name))
        return PackageFlavorSelections(res)

    @staticmethod
    def CreateName(packageName: UnresolvedPackageName) -> 'PackageName':
        return PackageName(packageName.Value, packageName.Value, packageName, PackageFlavorSelectionsEmpty.Empty)

    @staticmethod
    def CreateUnresolvedNameAndSelection(packageName: UnresolvedPackageName, flavorSelections: PackageFlavorSelections) -> 'PackageName':
        name = PackageName.__CreateFullName(packageName, flavorSelections)
        return PackageName(name, name, packageName, flavorSelections)

    @staticmethod
    def CreateNameAndSelection(packageName: 'PackageName', flavorSelections: PackageFlavorSelections, isFlavorSingleton: bool) -> 'PackageName':
        name = PackageName.__CreateFullName2(packageName, flavorSelections)
        if isFlavorSingleton:
            return PackageName(name, packageName.Value, packageName.Unresolved, flavorSelections)
        return PackageName(name, name, packageName.Unresolved, flavorSelections)
