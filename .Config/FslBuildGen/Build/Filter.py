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

from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from typing import overload
from typing import TypeVar
from typing import Union
from FslBuildGen import PackageListUtil
from FslBuildGen import Util
from FslBuildGen.Build.RequirementTree import RequirementTree
from FslBuildGen.Build.RequirementTreeNode import RequirementTreeNode
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Info.AppInfo import AppInfoPackage
from FslBuildGen.Info.RequirementInfo import RequirementInfo
from FslBuildGen.Info.AppInfoRequirementTree import AppInfoGlobalRequirementTree
from FslBuildGen.Info.AppInfoGlobalRequirementTreeNode import AppInfoGlobalRequirementTreeNode
from FslBuildGen.Log import Log
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.QualifiedRequirementExtensionName import QualifiedRequirementExtensionName


class LocalUtil:
    @staticmethod
    def BuildListOfDirectlyNotSupported(package: Package) -> List[Package]:
        notSupported = []  # type: List[Package]
        for dependency in package.ResolvedBuildOrder:
            if not dependency.ResolvedPlatformDirectSupported:
                notSupported.append(dependency)
        return notSupported


class RequirementFilter:
    @staticmethod
    def FilterRequirementsByType(requirements: List[PackageRequirement], requirementType: Optional[str]) -> List[PackageRequirement]:
        """ Filter the requirements by the supplied type.
            If type is none this returns the requirements list without modification
        """
        return requirements if requirementType is None else [requirement for requirement in requirements if requirement.Type == requirementType]


    @staticmethod
    def GetRequirementList(topLevelPackage: Package,
                           requestedPackages: Optional[List[Package]],
                           requirementType: Optional[str] = None) -> List[PackageRequirement]:
        """ Generate a requirement list based on input, the requirement list can be optionally filtered by requirementType.
            If requestedPackages are None then then all packages used by the top level package will be filtered.
            If a top level package is supplied then we return the 'ResolvedAllRequirements' for it (filtered as requested).
        """
        if topLevelPackage is None:
            raise UsageErrorException("topLevelPackage can not be None")

        if requestedPackages is None or len(requestedPackages) <= 0:
            # Since no files were supplied we use the topLevelPackage
            return RequirementFilter.FilterRequirementsByType(topLevelPackage.ResolvedAllRequirements, requirementType)
        return RequirementFilter.GetRequirementListFromPackages(requestedPackages, requirementType)


    @staticmethod
    def GetRequirementListFromPackages(requestedPackages: List[Package],
                                       requirementType: Optional[str] = None) -> List[PackageRequirement]:
        # extract the package requirements into a unique list while still respecting the filter
        requirementDict = {}  # type: Dict[str, PackageRequirement]
        for package in requestedPackages:
            requirementList = RequirementFilter.FilterRequirementsByType(package.ResolvedAllRequirements, requirementType)
            for requirement in requirementList:
                if not requirement.FullId in requirementDict:
                    requirementDict[requirement.FullId] = requirement
        return list(requirementDict.values())


T = TypeVar('T')

class PackageFilter:
    @staticmethod
    def __ContainsFeature(resolvedFeatureList: Union[List[PackageRequirement], List[RequirementInfo]], featureName: str) -> bool:
        for feature in resolvedFeatureList:
            if feature.Name == featureName:
                return True
        return False


    @staticmethod
    def __UsesFeatures(package: Union[Package, AppInfoPackage], requiredFeatureNameList: List[str]) -> bool:
        for featureName in requiredFeatureNameList:
            if not PackageFilter.__ContainsFeature(package.ResolvedAllUsedFeatures, featureName):
                return False
        return True


    @staticmethod
    def __FeaturesAvailable(package: Union[Package, AppInfoPackage], featureNameList: List[str]) -> bool:
        for feature in package.ResolvedAllUsedFeatures:
            if not feature.Name in featureNameList:
                return False
        return True


    @staticmethod
    def __IsExtensionAvailable(requirementTree: Union[RequirementTree, AppInfoGlobalRequirementTree], featureName: str, extensionName: str) -> bool:
        extensionNode = requirementTree.TryLocateExtensionNode(featureName, extensionName)
        return extensionNode is not None and extensionNode.Supported


    @staticmethod
    def __IsAllPackageExtensionAvailable(package: Union[Package, AppInfoPackage], requirementTree: Union[RequirementTree, AppInfoGlobalRequirementTree]) -> bool:
        for requirement in package.ResolvedAllRequirements:
            if requirement.Type == PackageRequirementTypeString.Extension and not PackageFilter.__IsExtensionAvailable(requirementTree, requirement.Extends, requirement.Name):
                return False
        return True

    @staticmethod
    def __GetCompleteMissingExtensionNames(package: Union[Package, AppInfoPackage], requirementTree: Union[RequirementTree, AppInfoGlobalRequirementTree]) -> List[str]:
        missing = []
        for requirement in package.ResolvedAllRequirements:
            if requirement.Type == PackageRequirementTypeString.Extension and not PackageFilter.__IsExtensionAvailable(requirementTree, requirement.Extends, requirement.Name):
                missing.append(QualifiedRequirementExtensionName.ToString(requirement.Extends, requirement.Name))
        return missing


    @staticmethod
    def __GetCompleteMissingFeatureNames(package: Union[Package, AppInfoPackage], featureNameList: List[str]) -> List[str]:
        missing = []
        for feature in package.ResolvedAllUsedFeatures:
            if not feature.Name in featureNameList:
                missing.append(feature.Name)
        return missing


    @overload
    @staticmethod
    def __FiltersPackagesByRequiredFeature(log: Log, packages: List[Package],
                                           requiredFeatureNameList: List[str]) -> List[T]:
        pass

    @overload
    @staticmethod
    def __FiltersPackagesByRequiredFeature(log: Log, packages: List[AppInfoPackage],
                                           requiredFeatureNameList: List[str]) -> List[T]:
        pass

    @staticmethod
    def __FiltersPackagesByRequiredFeature(log: Log, packages: Union[List[Package], List[AppInfoPackage]],
                                           requiredFeatureNameList: List[str]) -> List[T]:
        """ Filter packages to those that require the specified feature.
            - If '*' is contained in 'requiredFeatureNameList' then we return 'packages' and no filtering is done.
            - Will always return a new list
        """
        allowAllFeatures = '*' in requiredFeatureNameList
        if allowAllFeatures:
            return cast(List[T], list(packages))

        # Count and filter executables
        filteredPackageList = []
        for package in packages:
            if PackageFilter.__UsesFeatures(package, requiredFeatureNameList):
                filteredPackageList.append(package)
            else:
                log.LogPrint("Skipping '{0}' since it did not use the features '{1}'".format(package.Name, ", ".join(requiredFeatureNameList)))
        return cast(List[T], filteredPackageList)


    @staticmethod
    def __AddParentFeatures(log: Log, featureNameList: List[str],
                            requirementTree: Union[RequirementTree, AppInfoGlobalRequirementTree],
                            useStrictFeatureWarning: bool) -> List[str]:
        if '*' in featureNameList:
            return featureNameList
        featureNameList.sort()
        if log.Verbosity > 1:
            log.LogPrint("Automatically adding features to supplied feature list {0}".format(featureNameList))
        featureNameSet = set(featureNameList)
        for featureName in featureNameList:
            if featureName in requirementTree.FeatureToNodeDict:
                requirementNode = requirementTree.FeatureToNodeDict[featureName]
                currentNode = requirementNode  # type: Optional[Union[RequirementTreeNode, AppInfoGlobalRequirementTreeNode]]
                while currentNode is not None:
                    if currentNode.Content is not None:
                        if not currentNode.Content.Name in featureNameSet:
                            featureNameSet.add(currentNode.Content.Name)
                            if log.Verbosity > 1 and requirementNode.Content is not None:
                                log.LogPrint("- '{0}' because '{1}' depends on it".format(currentNode.Content.Name, requirementNode.Content.Name))
                    currentNode = currentNode.Parent
            else:
                featureNameSet.remove(featureName)
                if useStrictFeatureWarning:
                    log.DoPrintWarning("Unknown feature name '{0}' in filterNameList {1}".format(featureName, featureNameList))
                else:
                    # For now just log a warning
                    log.LogPrintVerbose(5, "Unknown feature name '{0}' in filterNameList {1}".format(featureName, featureNameList))
        resultList = list(featureNameSet)
        resultList.sort()
        return resultList



    @overload
    @staticmethod
    def __FiltersPackagesByFeatures(log: Log, packages: List[Package], featureNameList: List[str]) -> List[T]:
        pass


    @overload
    @staticmethod
    def __FiltersPackagesByFeatures(log: Log, packages: List[AppInfoPackage], featureNameList: List[str]) -> List[T]:
        pass


    @staticmethod
    def __FiltersPackagesByFeatures(log: Log, packages: Union[List[Package], List[AppInfoPackage]],
                                   featureNameList: List[str]) -> List[T]:
        """  Filter packages by features.
             If '*' is in the featureNameList a clone of 'packages' will be returned
             Else we return a list containing only the packages that can be build with the available features
        """

        if '*' in featureNameList:
            log.LogPrint("Filtering by features: All")
            return cast(List[T], list(packages))
        else:
            log.LogPrint("Filtering by features: {0}".format(", ".join(featureNameList)))

        filteredPackageList = []
        for package in packages:
            if PackageFilter.__FeaturesAvailable(package, featureNameList):
                filteredPackageList.append(package)
            elif package.Type == PackageType.Library or package.Type == PackageType.Executable or package.ResolvedDirectExperimentalRecipe is not None:
                missingFeatures = PackageFilter.__GetCompleteMissingFeatureNames(package, featureNameList)
                log.LogPrint("Could not build package '{0}' due to missing features '{1}'".format(package.Name, ", ".join(missingFeatures)))
        return cast(List[T], filteredPackageList)


    @overload
    @staticmethod
    def __FiltersPackagesByExtensions(log: Log,
                                      packages: List[Package],
                                      extensionNameList: ExtensionListManager,
                                      featureNameList: List[str],
                                      requirementTree: RequirementTree) -> List[T]:
        pass


    @overload
    @staticmethod
    def __FiltersPackagesByExtensions(log: Log,
                                      packages: List[AppInfoPackage],
                                      extensionNameList: ExtensionListManager,
                                      featureNameList: List[str],
                                      requirementTree: AppInfoGlobalRequirementTree) -> List[T]:
        pass


    @staticmethod
    def __FiltersPackagesByExtensions(log: Log,
                                      packages: Union[List[Package], List[AppInfoPackage]],
                                      extensionNameList: ExtensionListManager,
                                      featureNameList: List[str],
                                      requirementTree: Union[RequirementTree, AppInfoGlobalRequirementTree]) -> List[T]:
        """  Filter packages by extensions.
             If '*' is in the extensionNameList a clone of 'packages' will be returned
             Else we return a list containing only the packages that can be build with the available extensions
        """
        if extensionNameList.AllowAllExtensions:
            log.LogPrint("Filtering by extensions: All")
            return cast(List[T], list(packages))
        else:
            log.LogPrint("Filtering by extensions: {0}".format(", ".join([str(qualifiedName) for qualifiedName in extensionNameList.Content])))

        filteredPackageList = []
        for package in packages:
            if PackageFilter.__IsAllPackageExtensionAvailable(package, requirementTree):
                filteredPackageList.append(package)
            elif package.Type == PackageType.Library or package.Type == PackageType.Executable or package.ResolvedDirectExperimentalRecipe != None:
                missingNames = PackageFilter.__GetCompleteMissingExtensionNames(package, requirementTree)
                log.LogPrint("Could not build package '{0}' due to missing extension '{1}'".format(package.Name, ", ".join(missingNames)))
        return cast(List[T], filteredPackageList)


    @overload
    @staticmethod
    def __FiltersPackagesBySupported(log: Log, packages: List[Package]) -> List[T]:
        pass

    @overload
    @staticmethod
    def __FiltersPackagesBySupported(log: Log, packages: List[AppInfoPackage]) -> List[T]:
        pass

    @staticmethod
    def __FiltersPackagesBySupported(log: Log, packages: Union[List[Package], List[AppInfoPackage]]) -> List[T]:
        """ Remove packages that are marked as not supported by the platform
        """
        packageList = []
        for package in packages:
            if not package.ResolvedPlatformNotSupported:
                packageList.append(package)
            elif not package.Type == PackageType.TopLevel and log.IsVerbose and isinstance(package, Package):
                notSupported = LocalUtil.BuildListOfDirectlyNotSupported(package)
                notSupportedNames = Util.ExtractNames(notSupported)
                log.DoPrint("Skipping {0} since its marked as not supported on this platform by package: {1}".format(package.Name, notSupportedNames))
        return cast(List[T], packageList)

    @staticmethod
    def __ContainsExecutablePackage(packages: List[Package]) -> bool:
        if packages is None:
            return False
        for package in packages:
            if package.Type == PackageType.Executable:
                return True
        return False


    @staticmethod
    def PrintExecutableSkipReason(log: Log, fullPackageList: List[Package], filteredPackageList: List[Package]) -> None:
        for package in fullPackageList:
            if package.Type == PackageType.Executable:
                if package.ResolvedPlatformNotSupported:
                    notSupported = LocalUtil.BuildListOfDirectlyNotSupported(package)
                    notSupportedNames = Util.ExtractNames(notSupported)
                    log.DoPrint("{0} was marked as not supported on this platform by package: {1}".format(package.Name, notSupportedNames))


    @staticmethod
    def __FilterExtensionsByAvailableFeatures(log: Log, featureNameList: List[str],
                                              qualifiedExtensionNameList: ExtensionListManager) -> ExtensionListManager:
        if qualifiedExtensionNameList.AllowAllExtensions:
            return qualifiedExtensionNameList

        filteredList = []
        for qualifiedExtensionNameRecord in qualifiedExtensionNameList.Content:
            if qualifiedExtensionNameRecord.FeatureName in featureNameList:
                filteredList.append(qualifiedExtensionNameRecord)
            else:
                log.LogPrint("Removing extension '{0}' as the feature '{1}' is unavailable".format(qualifiedExtensionNameRecord, qualifiedExtensionNameRecord.FeatureName))
        return ExtensionListManager(False, filteredList)


    @staticmethod
    def __DetermineActualUserBuildRequest(allAvailablePackageListInResolvedBuildOrder: List[Package],
                                          requestedPackages: Optional[List[Package]]) -> List[Package]:
        if requestedPackages is not None and len(requestedPackages) > 0:
            return requestedPackages
        return allAvailablePackageListInResolvedBuildOrder


    @staticmethod
    def __FiltersRecipePackages(log: Log, resolvedPackageOrder: List[Package], requestedPackages: Optional[List[Package]]) -> List[Package]:
        return [package for package in resolvedPackageOrder if not package.ContainsRecipe() or (requestedPackages is not None and package in requestedPackages)]


    @staticmethod
    def FilterNotSupported(log: Log,
                           topLevelPackage: Package,
                           requestedPackages: Optional[List[Package]]) -> List[Package]:
        """ Filter the package list based
            - if they are supported on the platform
        """
        resolvedBuildOrder = topLevelPackage.ResolvedBuildOrder

        # Try to determine what the user is interested in building
        requestedPackagesInOrder = PackageFilter.__DetermineActualUserBuildRequest(resolvedBuildOrder, requestedPackages)

        # remove all the unsupported packages
        requestedPackagesInOrder = PackageFilter.__FiltersPackagesBySupported(log, requestedPackagesInOrder)
        # Now that we have a filtered list of desired packages, extend it to include all required packages
        return PackageListUtil.GetRequiredPackagesInSourcePackageListOrder(requestedPackagesInOrder, resolvedBuildOrder)


    @staticmethod
    def Filter(log: Log,
               topLevelPackage: Package,
               requestedPackages: Optional[List[Package]],
               packageFilters: PackageFilters) -> List[Package]:
        """ Filter the package list based
            - Required packages by the requested packages (if requestedPackages isnt None)
            - If there is executeables then chose those that implement the required features in requiredFeatureNameList
            - the available features from featureNameList
            - the available extensions from extensionNameList
            - if they are supported on the platform
        """
        resolvedBuildOrder = topLevelPackage.ResolvedBuildOrder
        requirements = RequirementFilter.GetRequirementList(topLevelPackage, None)
        requirementTree = RequirementTree(requirements)

        # Smart expand the input lists
        # - First we add all parent features automatically to ensure the feature list is complete
        useStrictFeatureWarning = requestedPackages is None or len(requestedPackages) <= 0
        featureNameList2 = PackageFilter.__AddParentFeatures(log, packageFilters.FeatureNameList, requirementTree, useStrictFeatureWarning)
        extensionNameList = packageFilters.ExtensionNameList
        if not packageFilters.ExtensionNameList.AllowAllExtensions:
            # Filter extensions by available features
            extensionNameList = PackageFilter.__FilterExtensionsByAvailableFeatures(log, featureNameList2, extensionNameList)
            requirementTree.SetExtensionSupport(log, extensionNameList)

        # Try to determine what the user is interested in building
        requestedPackagesInOrder = PackageFilter.__DetermineActualUserBuildRequest(resolvedBuildOrder, requestedPackages)

        # Remove recipe packages
        requestedPackagesInOrder = PackageFilter.__FiltersRecipePackages(log, requestedPackagesInOrder, requestedPackages)

        # Remove packages based on the users required features request
        requestedPackagesInOrder = PackageFilter.__FiltersPackagesByRequiredFeature(log, requestedPackagesInOrder, packageFilters.RequiredFeatureNameList)
        # Remove packages based on the available features (remove all packages that can't be build due to missing features)
        requestedPackagesInOrder = PackageFilter.__FiltersPackagesByFeatures(log, requestedPackagesInOrder, featureNameList2)
        # Remove packages based on the available extensions (remove all packages that can't be build due missing extensions)
        requestedPackagesInOrder = PackageFilter.__FiltersPackagesByExtensions(log, requestedPackagesInOrder, extensionNameList, featureNameList2, requirementTree)
        # Remove packages that are not supported on this platform
        requestedPackagesInOrder = PackageFilter.__FiltersPackagesBySupported(log, requestedPackagesInOrder)
        # Now that we have a filtered list of desired packages, extend it to include all required packages
        return PackageListUtil.GetRequiredPackagesInSourcePackageListOrder(requestedPackagesInOrder, resolvedBuildOrder)


    @staticmethod
    def FilterAppInfo(log: Log,
                      resolvedBuildOrder: List[AppInfoPackage],
                      appInfoRequirementTree: AppInfoGlobalRequirementTree,
                      packageFilters: PackageFilters) -> List[AppInfoPackage]:
        """ The goal of this filter is to follow the same rules as the 'Filter' method
            so we filter the AppInfoPackage in the same way we would the packages
        """
        useStrictFeatureWarning = True
        # Smart expand the input lists
        # - First we add all parent features automatically to ensure the feature list is complete
        featureNameList2 = PackageFilter.__AddParentFeatures(log, packageFilters.FeatureNameList, appInfoRequirementTree, useStrictFeatureWarning)
        extensionNameList = packageFilters.ExtensionNameList
        if not packageFilters.ExtensionNameList.AllowAllExtensions:
            # Filter extensions by available features
            extensionNameList = PackageFilter.__FilterExtensionsByAvailableFeatures(log, featureNameList2, extensionNameList)
            appInfoRequirementTree.SetExtensionSupport(log, extensionNameList)

        # Remove recipe packages (we dont have recipe packages in the app info)
        #requestedPackagesInOrder = PackageFilter.__FiltersRecipePackages(log, requestedPackagesInOrder)

        # Remove packages based on the users required features request (app must have feature, if no executables in resolvedBuildOrder no filtering is done!)
        resolvedBuildOrder = PackageFilter.__FiltersPackagesByRequiredFeature(log, resolvedBuildOrder, packageFilters.RequiredFeatureNameList)
        # Remove packages based on the available features (remove all packages that can't be build due to missing features)
        resolvedBuildOrder = PackageFilter.__FiltersPackagesByFeatures(log, resolvedBuildOrder, featureNameList2)
        # Remove packages based on the available extensions (remove all packages that can't be build due missing extensions)
        resolvedBuildOrder = PackageFilter.__FiltersPackagesByExtensions(log, resolvedBuildOrder, extensionNameList, featureNameList2, appInfoRequirementTree)
        # Remove packages that are not supported on this platform
        resolvedBuildOrder = PackageFilter.__FiltersPackagesBySupported(log, resolvedBuildOrder)

        # NOTE: this is probably not necessary for app info
        # Now that we have a filtered list of desired packages, extend it to include all required packages
        #return PackageListUtil.GetRequiredPackagesInSourcePackageListOrder(requestedPackagesInOrder, resolvedBuildOrder)
        return resolvedBuildOrder


    @staticmethod
    def WasThisAExecutableBuildAndAreThereAnyLeft(sourcePackageList: List[Package], packageList: List[Package]) -> bool:
        # If we require executables and had executables available to begin with and if there is no executables left -> exit
        if PackageFilter.__ContainsExecutablePackage(sourcePackageList) and not PackageFilter.__ContainsExecutablePackage(packageList):
            return False
        return True


    @staticmethod
    def FilterBuildablePackages(resolvedBuildOrder: List[Package]) -> List[Package]:
        """ Remove any package that aint buildable.
        """
        result = []
        for package in resolvedBuildOrder:
            if not package.IsVirtual and (package.Type == PackageType.Library or package.Type == PackageType.Executable):
                result.append(package)
        return result
