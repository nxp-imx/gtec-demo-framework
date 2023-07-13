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
from typing import Set
from FslBuildGen import Util
from FslBuildGen.Engine.Resolver.PackageName import PackageName
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.PackageFlavorSelection import PackageFlavorSelection
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorExtension import UnresolvedPackageFlavorExtension
#from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorOption import UnresolvedPackageFlavorOption

class ExtendingFlavorCanNotIntroduceNewOptionsException(Exception):
    def __init__(self, flavorDefinition: UnresolvedPackageFlavor, extendingFlavor: UnresolvedPackageFlavorExtension, newOptions: List[PackageFlavorOptionName]) -> None:
        msg = "Flavor: '{0}' can not extend the options defined by '{1}' with '{2}'".format(extendingFlavor.Name, flavorDefinition.Name, ", ".join(Util.ExtractValues(newOptions)))
        super().__init__(msg)

class FlavorExtendParentUndefinedException(Exception):
    def __init__(self, flavor: UnresolvedPackageFlavorExtension) -> None:
        msg = "Can not extend flavor: '{0}' as its undefined".format(flavor)
        super().__init__(msg)

class FlavorNameCollisionException(Exception):
    def __init__(self, flavor0: UnresolvedPackageFlavor, flavor1: UnresolvedPackageFlavor) -> None:
        msg = "Flavor: '{0}' names collides with flavor '{1}'".format(flavor0.Name, flavor1.Name)
        super().__init__(msg)

class PackageDependencyNotFoundException(Exception):
    def __init__(self, sourcePackageName: str, dependencyPackageName: str, candidateList: str) -> None:
        super().__init__("'{0}' dependency '{1}' not found, did you mean: '{2}'".format(sourcePackageName, dependencyPackageName, candidateList))

class CircularDependencyInDependentPackageException(Exception):
    def __init__(self, message: str) -> None:
        # pylint: disable=useless-super-delegation
        super().__init__(message)

class PackageFlavorOptionDependencyNotFoundException(Exception):
    def __init__(self, message: str) -> None:
        # pylint: disable=useless-super-delegation
        super().__init__(message)

class PackageFlavorExtensionOptionDependencyNotFoundException(Exception):
    def __init__(self, message: str) -> None:
        # pylint: disable=useless-super-delegation
        super().__init__(message)

class PackageFlavorDependencyConstraintInvalidException(Exception):
    def __init__(self, packageName: UnresolvedPackageName, depConstraint: PackageFlavorSelection, targetFlavor: UnresolvedPackageFlavor) -> None:
        # pylint: disable=useless-super-delegation
        super().__init__("Package '{0}' has dependency to unknown flavor option '{1}' flavor '{2}' in package '{3}', valid options are '{4}': ".format(packageName, depConstraint.Option, depConstraint.Name, depConstraint.Name.OwnerPackageName, targetFlavor.Description))

class FlavorExtensionCanNotBeAddedToFlavorOriginException(Exception):
    def __init__(self, flavor: UnresolvedPackageFlavorExtension) -> None:
        super().__init__("Package '{0}' flavor: '{1}' can not extend its own flavor.".format(flavor.Name.OwnerPackageName, flavor.Name))

class MustBeFlavorExtensionException(Exception):
    def __init__(self, extendingPackageName: UnresolvedPackageName, extendingFlavor: UnresolvedPackageFlavor) -> None:
        super().__init__("Package '{0}' flavor: '{1}' must be marked as a flavor extension".format(extendingPackageName, extendingFlavor.Name))

class FlavorCanNotExtendPackageItsNotDependentUponException(Exception):
    def __init__(self, message: str) -> None:
        super().__init__(message)

    @staticmethod
    def CreateSimple(packageName: PackageName, flavor: UnresolvedPackageFlavor) -> 'FlavorCanNotExtendPackageItsNotDependentUponException':
        return FlavorCanNotExtendPackageItsNotDependentUponException("Package '{0}' can not extend flavor: '{1}' as no dependency to the source package '{2}' exist".format(packageName, flavor, flavor.Name.OwnerPackageName))

    @staticmethod
    def CreateComplex(packageToUnusedFlavorExtensionDict: Dict[str, Set[str]]) -> 'FlavorCanNotExtendPackageItsNotDependentUponException':
        res = '';
        for key, valueSet in packageToUnusedFlavorExtensionDict.items():
            for flavor in valueSet:
                if len(res) > 0:
                    res += "\n";
                res += "Package '{0}' can not extend flavor: '{1}' as no dependency to the source package exist".format(key, flavor);
        return FlavorCanNotExtendPackageItsNotDependentUponException(res)


class PackageExternalFlavorConstraintInvalidException(Exception):
    def __init__(self, flavorName: PackageFlavorName, validFlavors: List[UnresolvedPackageFlavor]) -> None:
        super().__init__("Invalid external flavor constraint for flavor '{0}', valid flavors are '{1}'".format(flavorName, PackageExternalFlavorConstraintInvalidException.ToString(validFlavors)))

    @staticmethod
    def ToString(validFlavors: List[UnresolvedPackageFlavor]) -> str:
        res = "";
        for flavor in validFlavors:
            if len(res) > 0:
                res += ", ";
            res += flavor.Name.Value;
        return res;

class PackageExternalFlavorConstraintOptionInvalidException(Exception):
    def __init__(self, flavorName: PackageFlavorName, optionName: PackageFlavorOptionName, targetFlavor: UnresolvedPackageFlavor) -> None:
        super().__init__("Invalid external flavor constraint option '{0}' for flavor '{1}', valid options are '{2}'".format(optionName, flavorName, targetFlavor.Description))

class PackageExternalFlavorConstraintMustBeSetException(Exception):
    def __init__(self, flavorsThatMustBeConstrained: str) -> None:
        super().__init__("Please specify exactly which flavor you desire to build: {0}".format(flavorsThatMustBeConstrained))
