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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.PackageFlavorSelection import PackageFlavorSelection
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelectionsEmpty
from FslBuildGen.Engine.Resolver.InstanceConfig import InstanceConfig
from FslBuildGen.Engine.Resolver.PackageDependency import PackageDependency
from FslBuildGen.Engine.Resolver.PackageName import PackageName
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageFlavor
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageFlavorExtension
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageFlavorOption
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageTemplate
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageTemplateDependency
from FslBuildGen.Engine.Unresolved.UnresolvedBasicPackage import UnresolvedBasicPackage
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency
from FslBuildGen.Log import Log
#from FslBuildGen.Resolver.PackageFlavorSelections import PackageFlavorSelections

class LocalVerbosityLevel(object):
    Trace = 6

class Record(object):
    def __init__(self, packageTemplate: ResolvedPackageTemplate) -> None:
        super().__init__()
        self.PackageTemplate = packageTemplate


class PackageResolver(object):
    def __init__(self, log: Log) -> None:
        super().__init__()
        self.__PackageTemplateDict = dict() # type: Dict[str, Record]
        self.__Log = log


    def Resolve(self, unresolvedPackage: UnresolvedBasicPackage) -> ResolvedPackageTemplate:
        # Since we are resolving this package, it should not be present in the dict yet
        if unresolvedPackage.Name.Value in self.__PackageTemplateDict:
            raise Exception("Internal error package '{0}' already resolved".format(unresolvedPackage.Name))

        directDependencies = self.__ResolveDirectDependencies(unresolvedPackage)
        allInstancesConfigurations = self.__GenerateInstancesConfigurations(unresolvedPackage)

        packageFlavors = PackageResolver.__ResolveFlavors(unresolvedPackage, self.__PackageTemplateDict) # type: List[ResolvedPackageFlavor]
        packageFlavorExtensions = PackageResolver.__ResolveFlavorExtensions(unresolvedPackage, self.__PackageTemplateDict) # type: List[ResolvedPackageFlavorExtension]

        newTemplateName = PackageName.CreateName(unresolvedPackage.Name)
        newTemplate = ResolvedPackageTemplate(newTemplateName, unresolvedPackage.Type, directDependencies, allInstancesConfigurations,
                                              packageFlavors, packageFlavorExtensions)
        self.__PackageTemplateDict[newTemplate.Name.Value] = Record(newTemplate)

        if self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
            self.__Log.LogPrint("- Name: {0} Combinations: {1}".format(newTemplate.Name, len(newTemplate.InstanceConfigs)))
            self.__Log.PushIndent()
            try:
                for instanceConfig in newTemplate.InstanceConfigs:
                    strDirectDependency = ", ".join([str(dep) for dep in instanceConfig.DirectDependencies]) # type: str
                    self.__Log.LogPrint("- {0}<{1}> directDependencies: [{2}]".format(unresolvedPackage.Name, instanceConfig.Description, strDirectDependency))
            finally:
                self.__Log.PopIndent()
        return newTemplate

    @staticmethod
    def __ResolveFlavors(unresolvedPackage: UnresolvedBasicPackage, packageTemplateDict: Dict[str, Record]) -> List[ResolvedPackageFlavor]:
        packageFlavors = [] # type: List[ResolvedPackageFlavor]
        for flavor in unresolvedPackage.Flavors:
            options = [] # type: List[ResolvedPackageFlavorOption]
            for flavorOption in flavor.Options:
                if len(flavorOption.DirectDependencies) > 0:
                    resolvedDependencies = [] # type List[ResolvedPackageTemplateDependency]
                    for srcDep in flavorOption.DirectDependencies:
                        if srcDep.Name.Value not in packageTemplateDict:
                            raise Exception("Unknown dependency '{0}".format(srcDep.Name))
                        depRecord = packageTemplateDict[srcDep.Name.Value]
                        resolvedDependencies.append(ResolvedPackageTemplateDependency(depRecord.PackageTemplate, srcDep.FlavorConstraints))
                else:
                    resolvedDependencies = []

                options.append(ResolvedPackageFlavorOption(flavorOption.Name, resolvedDependencies, flavorOption.Supported))
            packageFlavors.append(ResolvedPackageFlavor(flavor.Name, flavor.QuickName, options))
        return packageFlavors

    @staticmethod
    def __ResolveFlavorExtensions(unresolvedPackage: UnresolvedBasicPackage, packageTemplateDict: Dict[str, Record]) -> List[ResolvedPackageFlavorExtension]:
        packageFlavorExtensions = [] # type: List[ResolvedPackageFlavorExtension]
        for flavor in unresolvedPackage.FlavorExtensions:
            options = [] # type: List[ResolvedPackageFlavorOption]
            for flavorOption in flavor.Options:
                if len(flavorOption.DirectDependencies) > 0:
                    resolvedDependencies = [] # type List[ResolvedPackageTemplateDependency]
                    for srcDep in flavorOption.DirectDependencies:
                        if srcDep.Name.Value not in packageTemplateDict:
                            raise Exception("Unknown dependency '{0}".format(srcDep.Name))
                        depRecord = packageTemplateDict[srcDep.Name.Value]
                        resolvedDependencies.append(ResolvedPackageTemplateDependency(depRecord.PackageTemplate, srcDep.FlavorConstraints))
                else:
                    resolvedDependencies = []

                options.append(ResolvedPackageFlavorOption(flavorOption.Name, resolvedDependencies, flavorOption.Supported))
            packageFlavorExtensions.append(ResolvedPackageFlavorExtension(flavor.Name, options))
        return packageFlavorExtensions

    def __GenerateFlavorPermutations(self, instanceConfigs: List[InstanceConfig], permutation: List[PackageFlavorSelection],
                                     permutationDirectDependencies: List[PackageDependency], flavors: List[UnresolvedPackageFlavor],
                                     flavorIndex: int, unresolvedPackage: UnresolvedBasicPackage, flavorConstraints: PackageFlavorSelections) -> None:
        if flavorIndex >= len(flavors):
            flavorSelections = PackageFlavorSelections(list(permutation))
            if PackageResolver.__IsAllowed(flavorSelections, flavorConstraints):
                instanceConfig = InstanceConfig(flavorSelections, list(permutationDirectDependencies))
                strDependencies = ", ".join([str(dep) for dep in instanceConfig.DirectDependencies])
                if self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
                    self.__Log.LogPrint("- Package {0} InstanceConfig: '{1}' DirectDependencies: [{2}]".format(unresolvedPackage.Name, instanceConfig.Description, strDependencies))
                instanceConfigs.append(instanceConfig)
            elif self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
                self.__Log.LogPrint("- Package {0} Permutation: '{1}' rejected due to constraints: {2}".format(unresolvedPackage.Name, flavorSelections.Description, flavorConstraints))
            return

        flavor = flavors[flavorIndex]
        for flavorOption in flavor.Options:
            if self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
                self.__Log.LogPrint("- Flavor {0}={1}".format(flavor.Name, flavorOption.Name))
            self.__Log.PushIndent()
            try:

                permutation.append(PackageFlavorSelection(flavor.Name, flavorOption.Name))

                if len(flavorOption.DirectDependencies) <= 0:
                    self.__GenerateFlavorPermutations(instanceConfigs, permutation, permutationDirectDependencies, flavors, flavorIndex + 1, unresolvedPackage,
                                                        flavorConstraints)
                else:
                    self.__GenerateFlavorPermutations2(instanceConfigs, permutation, permutationDirectDependencies, flavors, flavorIndex, unresolvedPackage,
                                                        flavorConstraints, flavorOption.DirectDependencies, 0)
                permutation.pop()
            finally:
                self.__Log.PopIndent()


    def __GenerateFlavorPermutations2(self, instanceConfigs: List[InstanceConfig], permutation: List[PackageFlavorSelection],
                                      permutationDirectDependencies: List[PackageDependency], flavors: List[UnresolvedPackageFlavor],
                                      flavorIndex: int, unresolvedPackage: UnresolvedBasicPackage, flavorConstraints: PackageFlavorSelections,
                                      directDependencies: List[UnresolvedPackageDependency], depIndex: int) -> None:
        if depIndex >= len(directDependencies):
            self.__GenerateFlavorPermutations(instanceConfigs, permutation, permutationDirectDependencies, flavors, flavorIndex + 1,
                                                unresolvedPackage, flavorConstraints)
            return

        dependency = directDependencies[depIndex] # type: UnresolvedPackageDependency
        if dependency.Name.Value not in self.__PackageTemplateDict:
            raise Exception("Unknown dependency '{0}".format(dependency.Name))
        depRecord = self.__PackageTemplateDict[dependency.Name.Value]

        #permutationHitCount = 0 # type: int
        #var rejectionReasons = new List<string>();
        for combination in depRecord.PackageTemplate.InstanceConfigs:
            if self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
                self.__Log.LogPrint("- Dependency {0} combination: '{1}'".format(dependency.Name, combination.Description))

            variantFlavorConstraints = self.__TryCombineConstraints(flavorConstraints, dependency.FlavorConstraints)
            if variantFlavorConstraints is not None:
                currentPermutation = self.__TryCombine(permutation, combination)
                if currentPermutation is not None:
                    #permutationHitCount = permutationHitCount + 1
                    permutationDirectDependencies.append(PackageDependency(PackageName.CreateUnresolvedNameAndSelection(dependency.Name, combination.FlavorSelections), dependency))

                    self.__GenerateFlavorPermutations2(instanceConfigs, currentPermutation, permutationDirectDependencies, flavors, flavorIndex, unresolvedPackage,
                                                        variantFlavorConstraints, directDependencies, depIndex + 1)
                    permutationDirectDependencies.pop()
                else:
                    strDescPermutation = ", ".join([str(entry) for entry in permutation])
                    if self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
                        self.__Log.LogPrint("  - Package '{0}' flavor rejected due to merge failure of '{1}' and '{2}' Dependency: {3}".format(unresolvedPackage.Name, strDescPermutation, combination.Description, dependency.Name))
            elif self.__Log.Verbosity >= LocalVerbosityLevel.Trace:
                self.__Log.LogPrint("-  Package '{0}' flavor rejected due to constraint conflict: '{1}' and '{2}' Dependency: {3}".format(unresolvedPackage.Name, flavorConstraints, dependency.FlavorConstraints.Description, dependency.Name))

        #if permutationHitCount <= 0:
        #  raise Exception(f"Package '{unresolvedPackage.Name}' unable to generate any valid permutations due to:\n{string.Join("\n  ", rejectionReasons)}")

    def __TryCombine(self, permutation: List[PackageFlavorSelection], combination: InstanceConfig) -> Optional[List[PackageFlavorSelection]]:
        newPermutation = list(permutation) # type: List[PackageFlavorSelection]

        for entry in combination.FlavorSelections.Selections:
            index = PackageResolver.__IndexOf(newPermutation, entry.Name)
            if index < 0:
                # The flavor was not part of the permutation, so just add it
                newPermutation.append(entry)
            elif newPermutation[index].Option != entry.Option:
                # The flavor was part of the permutation but its not compatible -> abort
                return None
        return newPermutation


    def __ResolveDirectDependencies(self, unresolvedPackage: UnresolvedBasicPackage) -> List[ResolvedPackageTemplateDependency]:
        if len(unresolvedPackage.DirectDependencies) > 0:
            resolvedDependencies = [] # type: List[ResolvedPackageTemplateDependency]
            for dependency in unresolvedPackage.DirectDependencies:
                if dependency.Name.Value not in self.__PackageTemplateDict:
                    raise Exception("Unknown dependency '{0}".format(dependency.Name))
                depRecord = self.__PackageTemplateDict[dependency.Name.Value]
                # Combine
                resolvedDependencies.append(ResolvedPackageTemplateDependency(depRecord.PackageTemplate, dependency.FlavorConstraints))
        else:
            resolvedDependencies = []
        return resolvedDependencies


    def __GenerateInstancesConfigurations(self, unresolvedPackage: UnresolvedBasicPackage) -> List[InstanceConfig]:
        instanceConfigs = []   # type: List[InstanceConfig]
        if len(unresolvedPackage.DirectDependencies) <= 0:
            self.__GenerateFlavorPermutations(instanceConfigs, [], [], unresolvedPackage.Flavors, 0, unresolvedPackage, PackageFlavorSelectionsEmpty.Empty)
        else:
            self.__GenerateFlavorPermutations2(instanceConfigs, [], [], unresolvedPackage.Flavors, -1, unresolvedPackage, PackageFlavorSelectionsEmpty.Empty,
                                               unresolvedPackage.DirectDependencies, 0)

        # We started with combinations but ended with none, so we have a impossible to satisfy constraint
        if len(instanceConfigs) <= 0 and (len(unresolvedPackage.DirectDependencies) > 0 or len(unresolvedPackage.Flavors) > 0):
            raise Exception("Package '{0}' internal error: no valid configurations. This is a constraints violation which should have been caught earlier".format(unresolvedPackage.Name))

        if len(instanceConfigs) <= 0:
            instanceConfigs.append(InstanceConfig(PackageFlavorSelectionsEmpty.Empty, []))

        return instanceConfigs


    @staticmethod
    def __TryCombineConstraints(flavorConstraints: PackageFlavorSelections, depFlavorConstraints: PackageFlavorSelections) -> Optional[PackageFlavorSelections]:
        if len(flavorConstraints.Selections) <= 0:
            return depFlavorConstraints
        if len(depFlavorConstraints.Selections) <= 0:
            return flavorConstraints

        constraintDict = dict() # type: Dict[PackageFlavorName, PackageFlavorOptionName]
        for entry in flavorConstraints.Selections:
            constraintDict[entry.Name] = entry.Option

        if not PackageResolver.__TryCombineConstraintsIntoDict(constraintDict, depFlavorConstraints.Selections):
            return None
        return PackageFlavorSelections([PackageFlavorSelection(name, option) for name, option in constraintDict.items()])


    @staticmethod
    def __TryCombineConstraintsIntoDict(constraintDict: Dict[PackageFlavorName, PackageFlavorOptionName], constraintArray: List[PackageFlavorSelection]) -> bool:
        for entry in constraintArray:
            if entry.Name not in constraintDict:
                constraintDict[entry.Name] = entry.Option
            elif entry.Option != constraintDict[entry.Name]:
                return False
        return True

    @staticmethod
    def __IsAllowed(flavorSelections: PackageFlavorSelections, requirements: PackageFlavorSelections) -> bool:
        if len(requirements.Selections) <= 0:
            return True

        for entry in flavorSelections.Selections:
            index = requirements.IndexOf(entry.Name)
            if index >= 0 and entry.Option != requirements.Selections[index].Option:
                return False
        return True

    @staticmethod
    def __IndexOf(entries: List[PackageFlavorSelection], entryName: PackageFlavorName) -> int:
        for i, entry in enumerate(entries):
            if entry.Name == entryName:
                return i
        return -1
