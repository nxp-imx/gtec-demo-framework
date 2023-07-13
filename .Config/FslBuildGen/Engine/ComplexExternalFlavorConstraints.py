#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2023 NXP
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
from FslBuildGen.Engine.ExternalFlavorConstraints import ExternalFlavorConstraints

class ComplexExternalFlavorConstraints(object):
    def __init__(self, flavorConstraintsDict: ExternalFlavorConstraints,
                       packageFlavorDict: Dict[str, ExternalFlavorConstraints]) -> None:
        super().__init__()
        self.DefaultFlavorConstraints = flavorConstraintsDict
        self.PackageFlavorContraintDict = packageFlavorDict

    def GetFlavorConstraints(self, packageName: str) -> ExternalFlavorConstraints:
        return self.PackageFlavorContraintDict[packageName] if packageName in self.PackageFlavorContraintDict else self.DefaultFlavorConstraints

    def HasConstraints(self) -> bool:
        return self.DefaultFlavorConstraints.HasConstraints() or len(self.PackageFlavorContraintDict) > 0

    @staticmethod
    def Merge(flavorConstraints: 'ComplexExternalFlavorConstraints',
              newConstraints: Dict[str, ExternalFlavorConstraints]) -> 'ComplexExternalFlavorConstraints':
        mergedPackageFlavorDict = dict() # type: Dict[str, ExternalFlavorConstraints]
        for packageName, packageFlavorConstraints in newConstraints.items():
            if packageName in flavorConstraints.PackageFlavorContraintDict:
                mergedDict = dict() # type: Dict[str, str]
                for flavorConstraintName, flavorConstraintOptionName in packageFlavorConstraints.Dict.items():
                    mergedDict[flavorConstraintName.Value] = flavorConstraintOptionName.Value
                for flavorConstraintName, flavorConstraintOptionName in flavorConstraints.PackageFlavorContraintDict[packageName].Dict.items():
                    if flavorConstraintName.Value in mergedDict:
                        if mergedDict[flavorConstraintName.Value] != flavorConstraintOptionName:
                            raise Exception("internal error key exist in both location and the value is not the same")
                    mergedDict[flavorConstraintName.Value] = flavorConstraintOptionName.Value
                mergedPackageFlavorDict[packageName] = ExternalFlavorConstraints.ToExternalFlavorConstraints(mergedDict)
            else:
                mergedPackageFlavorDict[packageName] = packageFlavorConstraints


        return ComplexExternalFlavorConstraints(flavorConstraints.DefaultFlavorConstraints, mergedPackageFlavorDict)
