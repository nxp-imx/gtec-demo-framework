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
from typing import Optional
from typing import Set
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.Order.Exceptions import FlavorExtensionCanNotBeAddedToFlavorOriginException
from FslBuildGen.Engine.Order.Exceptions import MustBeFlavorExtensionException
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorExtension import UnresolvedPackageFlavorExtension

class UnresolvedBasicPackage(object):
    def __init__(self, name: UnresolvedPackageName, packageType: PackageType, directDependencies: List[UnresolvedPackageDependency],
                 flavors: List[UnresolvedPackageFlavor], flavorExtensions: List[UnresolvedPackageFlavorExtension]) -> None:
        super().__init__()
        self.Name = name
        self.Type = packageType
        self.DirectDependencies = list(directDependencies)
        self.Flavors = list(flavors)
        self.FlavorExtensions = list(flavorExtensions)

        self.DirectDependencies.sort(key=lambda s: s.Name.Value.upper())
        self.Flavors.sort(key=lambda s: s.Name.Value.upper())
        self.FlavorExtensions.sort(key=lambda s: s.Name.Value.upper())

        UnresolvedBasicPackage.__SanityCheckDependencies(self.DirectDependencies)
        UnresolvedBasicPackage.__SanityCheckFlavor(self.Flavors, self.FlavorExtensions, self.Name)

    def __str__(self) -> str:
        return str(self.Name)

    def __repr__(self) -> str:
        return "Name:{0}".format(self.Name)

    @staticmethod
    def __SanityCheckDependencies(directDependencies: List[UnresolvedPackageDependency]) -> None:
        if len(directDependencies) <= 0:
            return
        uniqueNames = set() # type: Set[UnresolvedPackageName]
        for entry in directDependencies:
            if entry.Name in uniqueNames:
                raise Exception("Duplicate dependency '{0}'".format(entry.Name))
            uniqueNames.add(entry.Name)

    @staticmethod
    def __SanityCheckFlavor(packageFlavors: List[UnresolvedPackageFlavor], flavorExtensions: List[UnresolvedPackageFlavorExtension],
                            ownerPackageName: UnresolvedPackageName) -> None:
        if len(packageFlavors) <= 0 and len(flavorExtensions) <= 0:
            return
        uniqueNames = set() # type: Set[PackageFlavorName]
        # check flavors
        for entry in packageFlavors:
            if entry.Name in uniqueNames:
                raise Exception("Duplicate flavor '{0}'".format(entry.Name))
            uniqueNames.add(entry.Name)
            if ownerPackageName != entry.Name.OwnerPackageName:
                raise MustBeFlavorExtensionException(ownerPackageName, entry)

        # check flavor extensions
        for entry2 in flavorExtensions:
            if entry2.Name in uniqueNames:
                raise Exception("Duplicate flavor '{0}'".format(entry2.Name))
            uniqueNames.add(entry2.Name)
            if ownerPackageName == entry2.Name.OwnerPackageName:
                raise FlavorExtensionCanNotBeAddedToFlavorOriginException(entry2)


    def TryGetFlavor(self, flavorName: PackageFlavorName) -> Optional[UnresolvedPackageFlavor]:
        for entry in self.Flavors:
            if entry.Name == flavorName:
                return entry
        return None

    @staticmethod
    def Create2(packageName: UnresolvedPackageName, packageType: PackageType) -> 'UnresolvedBasicPackage':
        return UnresolvedBasicPackage(packageName, packageType, [], [], [])
