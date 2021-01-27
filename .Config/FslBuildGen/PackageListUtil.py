#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
from typing import overload
from typing import Set
from typing import TypeVar
from typing import Union
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Engine.Resolver.PreResolvePackageResult import PreResolvePackageResult

CommonPackage = TypeVar('CommonPackage', Package, PreResolvePackageResult)


def TryFindFirstExecutablePackage(packages: List[Package]) -> Optional[Package]:
    """ Given a list of packages locate the first marked as PackageType.Executable """
    for package in packages:
        if package.Type == PackageType.Executable:
            return package
    return None


def FindFirstExecutablePackage(packages: List[Package]) -> Package:
    package = TryFindFirstExecutablePackage(packages)
    if package is not None:
        return package
    raise Exception("No executable package found")


def GetExecutablePackages(packages: List[Package]) -> List[Package]:
    """ Given a list of packages marked as PackageType.Executable """
    return [package for package in packages if package.Type == PackageType.Executable]

def GetTopLevelPackage(packages: List[CommonPackage]) -> CommonPackage:
    """ Given a list of packages locate the TopLevel one or raise a exception if not found"""
    for package in packages:
        if package.Type == PackageType.TopLevel:
            return package
    raise Exception("No TopLevel package")


def BuildReferencedPackageSet(packageList: List[CommonPackage]) -> Set[CommonPackage]:
    """ build a set of packages that is referenced by the packages in packageList.
        This is basically all packages in the package list and any packages that they depend upon.
    """
    referencedPackageSet = set()
    for package in packageList:
        referencedPackageSet.add(package)
        for dependency in package.ResolvedBuildOrder:
            referencedPackageSet.add(dependency)
    return referencedPackageSet


def GetRequiredPackagesInSourcePackageListOrder(packageList: List[CommonPackage], sourcePackageList: List[CommonPackage]) -> List[CommonPackage]:
    """ Generate a list of all the packages that are required to build the packages in packageList in resolved build order.
    """

    # From the packageList build a set of all packages required to build
    allRequiredPackageSet = BuildReferencedPackageSet(packageList)
    # Then remove all packages from the sourcePackageList that isn't part of the required packages.

    filteredResolvedBuildOrder = []
    for package in sourcePackageList:
        if package in allRequiredPackageSet:
            filteredResolvedBuildOrder.append(package)
    return filteredResolvedBuildOrder
