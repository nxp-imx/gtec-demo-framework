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
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.PackageFlavorSelection import PackageFlavorSelection

class PackageFlavorSelections(object):
    def __init__(self, selections: List[PackageFlavorSelection]) -> None:
        super().__init__()
        if len(selections) > 0:
          selections = list(selections)
          selections.sort(key=lambda s: s.Name.Value.upper())
        self.Selections = selections
        self.Description = ", ".join([str(entry) for entry in selections])
        self.__SanityCheck(self.Selections)

    def __str__(self) -> str:
        return self.Description

    def Contains(self, name: PackageFlavorName) -> bool:
        return self.IndexOf(name) >= 0

    def IndexOf(self, name: PackageFlavorName) -> int:
        for i, selection in enumerate(self.Selections):
            res = selection.Name.CompareTo(name)
            if res >= 0:
                return i if res == 0 else -1
        return -1

    def __SanityCheck(self, selections: List[PackageFlavorSelection]) -> None:
        if len(selections) > 0:
            uniqueNames = set() # type: Set[PackageFlavorName]
            for entry in selections:
                if entry.Name in uniqueNames:
                    raise Exception("Duplicate flavor '{0}'".format(entry.Name))
                uniqueNames.add(entry.Name)

class PackageFlavorSelectionsEmpty(object):
    Empty = PackageFlavorSelections([])
