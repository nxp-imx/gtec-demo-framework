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
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.Resolver.InstanceConfig import InstanceConfig
from FslBuildGen.Engine.Resolver.PackageName import PackageName
from FslBuildGen.Engine.Resolver.ResolvedPackage import ResolvedPackage


class ResolvedPackageTemplateDependency(object):
    def __init__(self, template: 'ResolvedPackageTemplate', flavorConstraints: PackageFlavorSelections) -> None:
        super().__init__()
        self.Template = template
        self.FlavorConstraints = flavorConstraints

    def __str__(self) -> str:
        return "Name:{0} Constraints:{1}".format(self.Template.Name, self.FlavorConstraints)


class ResolvedPackageFlavorOption(object):
    def __init__(self, name: PackageFlavorOptionName, directDependencies: List[ResolvedPackageTemplateDependency]) -> None:
        super().__init__()
        self.Name = name
        self.DirectDependencies = directDependencies

    def __str__(self) -> str:
        return "Name:{0} DirectDepCount:{1}".format(self.Name, len(self.DirectDependencies))


class ResolvedPackageFlavor(object):
    def __init__(self, name: PackageFlavorName, options: List[ResolvedPackageFlavorOption]) -> None:
        super().__init__()
        self.Name = name
        self.Options = options
        self.Description = ResolvedPackageFlavor.__OptionString(self.Options)

    def TryGetOptionByName(self, name: PackageFlavorOptionName) -> Optional[ResolvedPackageFlavorOption]:
        for flavorOption in self.Options:
            if flavorOption.Name == name:
                return flavorOption
        return None


    def __str__(self) -> str:
        return "{0}:{{{1}}}".format(self.Name, self.Description)

    @staticmethod
    def __OptionString(options: List[ResolvedPackageFlavorOption]) -> str:
        return ",".join([option.Name.Value for option in options])




class ResolvedPackageTemplate(ResolvedPackage):
    def __init__(self, name: PackageName, packageType: PackageType, directDependencies: List[ResolvedPackageTemplateDependency],
                 instanceConfigs: List[InstanceConfig], packageFlavors: List[ResolvedPackageFlavor]) -> None:
        super().__init__(name, packageType)

        if len(directDependencies) > 0:
          directDependencies = list(directDependencies)
          directDependencies.sort(key=lambda s: s.Template.Name.Value.upper())

        if len(instanceConfigs) > 0:
          instanceConfigs = list(instanceConfigs)
          instanceConfigs.sort(key=lambda s: s.Description.upper())

        self.DirectDependencies = directDependencies
        self.InstanceConfigs = instanceConfigs
        self.PackageFlavors = packageFlavors
        if len(self.InstanceConfigs) <= 0:
            raise Exception("No instance configs")

        ResolvedPackageTemplate.__SanityCheckDependencies(self.DirectDependencies, self.Name)
        ResolvedPackageTemplate.__SanityCheckInstanceConfigs(self.InstanceConfigs)

    def TryGetFlavor(self, name: PackageFlavorName) -> Optional[ResolvedPackageFlavor]:
        for flavor in self.PackageFlavors:
            if flavor.Name == name:
                return flavor
        return None

    def GetFlavor(self, name: PackageFlavorName) -> ResolvedPackageFlavor:
        flavor = self.TryGetFlavor(name)
        if flavor is None:
            raise Exception("Package '{0}' unknown flavor name: {1} ({2})".format(self.Name, name, self.PackageFlavors))
        return flavor

    @staticmethod
    def __SanityCheckDependencies(directDependencies: List[ResolvedPackageTemplateDependency], name: PackageName) -> None:
        if len(directDependencies) <= 0:
            return
        uniqueNames = set()  # type: Set[PackageName]
        for entry in directDependencies:
            if entry.Template.Name == name:
                raise Exception("Can not add dependency to self '{0}'".format(entry.Template.Name))
            if entry.Template.Name in uniqueNames:
                raise Exception("Duplicate dependency '{0}'".format(entry.Template.Name))
            uniqueNames.add(entry.Template.Name)

    @staticmethod
    def __SanityCheckInstanceConfigs(instanceConfigs: List[InstanceConfig]) -> None:
        if len(instanceConfigs) <= 0:
            return
        uniqueNames = set()  # type: Set[str]
        for entry in instanceConfigs:
            if entry.Description in uniqueNames:
                raise Exception("Duplicate instance config '{0}'".format(entry.Description))
            uniqueNames.add(entry.Description)
