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
from typing import Set
from enum import Enum
from FslBuildGen.DataTypes import PackageInstanceType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.Resolver.PackageName import PackageName
from FslBuildGen.Engine.Resolver.ResolvedPackage import ResolvedPackage
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageTemplate
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency

class ResolvedPackageInstanceDependency(object):
    def __init__(self, package: 'ResolvedPackageInstance', sourceDependency: UnresolvedPackageDependency) -> None:
        super().__init__()
        self.Package = package
        self.SourceDependency = sourceDependency

    def __str__(self) -> str:
        return "Name:{0}".format(self.Package.Name)

    def __repr__(self) -> str:
        return "ResolvedPackageInstanceDependency({0})".format(self.Package)


class ResolvedPackageInstance(ResolvedPackage):
    def __init__(self, name: PackageName, packageType: PackageType, directDependencies: List[ResolvedPackageInstanceDependency],
                 flavorSelections: PackageFlavorSelections, flavorTemplate: ResolvedPackageTemplate, instanceType: PackageInstanceType) -> None:
        super().__init__(name, packageType)

        if len(directDependencies) > 0:
            directDependencies = list(directDependencies)
            directDependencies.sort(key=lambda s: s.Package.Name.Value.upper())

        self.DirectDependencies = directDependencies
        self.FlavorSelections = flavorSelections
        self.FlavorTemplate = flavorTemplate
        self.InstanceType = instanceType

        ResolvedPackageInstance.__SanityCheckDependencies(directDependencies, self.Name)

    def IsSupported(self) -> bool:
        for flavorSelection in self.FlavorSelections.Selections:
            flavor = self.FlavorTemplate.TryGetFlavor(flavorSelection.Name)
            if flavor is not None:
                flavorOption = flavor.GetOptionByName(flavorSelection.Option)
                if not flavorOption.Supported:
                    return False
            else:
                flavorExtension = self.FlavorTemplate.TryGetFlavorExtension(flavorSelection.Name)
                if flavorExtension is not None:
                    flavorExtensionOption = flavorExtension.TryGetOptionByName(flavorSelection.Option)
                    if flavorExtensionOption is not None and not flavorExtensionOption.Supported:
                        return False
        return True

    @staticmethod
    def __SanityCheckDependencies(directDependencies: List[ResolvedPackageInstanceDependency], name: PackageName) -> None:
        if len(directDependencies) <= 0:
            uniqueNames = set() # type: Set[PackageName]
            for entry in directDependencies:
                if entry.Package.Name == name:
                    raise Exception("Can not add dependency to self '{0}'".format(entry.Package.Name))
                if entry.Package.Name in uniqueNames:
                    raise Exception("Duplicate dependency '{0}'".format(entry.Package.Name))
                uniqueNames.add(entry.Package.Name)
