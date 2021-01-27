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
from FslBuildGen.Exceptions import CircularDependencyException
from FslBuildGen.Exceptions import GroupedException
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.Unresolved.UnresolvedBasicPackage import UnresolvedBasicPackage
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName

class FlavorInfo(object):
    def __init__(self, flavorName: PackageFlavorName, flavorOption: PackageFlavorOptionName) -> None:
        super().__init__()
        self.FlavorName = flavorName
        self.FlavorOption = flavorOption

class EvaluationPackage(object):

    class DependencyRecord(object):
        def __init__(self, package: 'EvaluationPackage', flavorInfo: Optional[FlavorInfo]) -> None:
            super().__init__()
            self.Package = package
            self.FlavorInfo = flavorInfo


    def __init__(self, name: UnresolvedPackageName, source: UnresolvedBasicPackage, directDependencies: List[DependencyRecord]) -> None:
        super().__init__()
        self.Name = name
        self.SourcePackage = source
        self.DirectDependencies = directDependencies
        self.IsSealed = False

    def Seal(self) -> None:
        if self.IsSealed:
            raise Exception("already sealed")
        self.IsSealed = True
        EvaluationPackage.__SanityCheckDependencies(self.DirectDependencies, self.Name)

    def __str__(self) -> str:
        return str(self.Name)

    def __repr__(self) -> str:
        return "EvaluationPackage({0})".format(self.Name)

    @staticmethod
    def __SanityCheckDependencies(directDependencies: List[DependencyRecord], name: UnresolvedPackageName) -> None:
        if len(directDependencies) <= 0:
            return

        uniquePackageDict = dict() # type: Dict[UnresolvedPackageName, List[EvaluationPackage.DependencyRecord]]
        for record in directDependencies:
            if record.Package.Name == name:
                raise CircularDependencyException("Can not add dependency to self '{0}'".format(record.Package.Name))
            if record.Package.Name not in uniquePackageDict:
                recordList = []   # type: List[EvaluationPackage.DependencyRecord]
                uniquePackageDict[record.Package.Name] = recordList
            else:
                recordList = uniquePackageDict[record.Package.Name]
            recordList.append(record)

        exceptionList = None # type: Optional[List[Exception]]
        for pairKey, pairValue in uniquePackageDict.items():
            if EvaluationPackage.__IsDuplicatedDependency(pairValue):
                if exceptionList is None:
                    exceptionList = []

                helpStr = EvaluationPackage.__ToDuplicatedDependencyHelp(name, pairKey, pairValue)

                exceptionList.append(Exception("Package '{0}' has duplicate dependency to '{1}': [{2}]'".format(name, pairKey, helpStr)))

        if exceptionList is not None and len(exceptionList) > 0:
            if len(exceptionList) > 1:
                raise GroupedException(exceptionList)
            else:
                raise exceptionList[0]

    @staticmethod
    def __IsDuplicatedDependency(entries: List['EvaluationPackage.DependencyRecord']) -> bool:
        if len(entries) <= 1:
            return False

        # Check if all the collisions occur from a flavor (which is allowed)
        flavorHitCount = 0 # type: int
        for entry in entries:
            if entry.FlavorInfo is not None:
                flavorHitCount = flavorHitCount + 1
        return (flavorHitCount != len(entries))

    @staticmethod
    def __ToDuplicatedDependencyHelp(fromPackageName: UnresolvedPackageName, toPackageName: UnresolvedPackageName, entries: List['EvaluationPackage.DependencyRecord']) -> str:
        res = [] # type: List[str]
        for entry in entries:
            desc = "{0}<{1}={2}>->{3}".format(fromPackageName, entry.FlavorInfo.FlavorName, entry.FlavorInfo.FlavorOption, toPackageName) if entry.FlavorInfo is not None else "{0}->{1}".format(fromPackageName, toPackageName)
            res.append(desc)
        return ", ".join(res)
