#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2024 NXP
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

from typing import Optional
from typing import List

class SemanticVersion2(object):

    def __init__(self, major: int, minor: int = -1, patch: int = -1, revision: int = -1, suffix: Optional[str] = None) -> None:
        if (minor < 0 and (patch >= 0 or revision >= 0)) or (patch < 0 and revision >= 0):
            raise Exception("Invalid version")

        self.Major = major
        self.Minor = minor if minor >= 0 else 0
        self.Patch = patch if patch >= 0 else 0
        self.Revision = revision if revision >= 0 else 0
        self.Suffix = suffix

        self.__Set = 8
        self.__Set |= 4 if minor >= 0 else 0
        self.__Set |= 2 if patch >= 0 else 0
        self.__Set |= 1 if revision >= 0 else 0

    def IsCompatible(self, other: 'SemanticVersion2') -> bool:
        if self.Major != other.Major:
            return False
        if self.Minor > other.Minor:
            return False or ((self.__Set & 4) == 0)
        if self.Patch > other.Patch:
            return False or ((self.__Set & 2) == 0)
        if self.Revision > other.Revision:
            return False or ((self.__Set & 1) == 0)
        return True

    def __str__(self) -> str:
        if self.Suffix is None:
            if self.__Set == (8|4|2|1):
                return "{0}.{1}.{2}.{3}".format(self.Major, self.Minor, self.Patch, self.Revision)
            if self.__Set == (8|4|2):
                return "{0}.{1}.{2}".format(self.Major, self.Minor, self.Patch)
            if self.__Set == (8|4):
                return "{0}.{1}".format(self.Major, self.Minor)
            return "{0}".format(self.Major)
        if self.__Set == (8|4|2|1):
            return "{0}.{1}.{2}.{3}-{4}".format(self.Major, self.Minor, self.Patch, self.Revision, self.Suffix)
        if self.__Set == (8|4|2):
            return "{0}.{1}.{2}-{3}".format(self.Major, self.Minor, self.Patch, self.Suffix)
        if self.__Set == (8|4):
            return "{0}.{1}-{2}".format(self.Major, self.Minor, self.Suffix)
        return "{0}-{1}".format(self.Major, self.Suffix)

    def ToMajorMinorPatchString(self) -> str:
        return "{0}.{1}.{2}".format(self.Major, self.Minor, self.Patch)

    def ToMajorMinorString(self) -> str:
        return "{0}.{1}".format(self.Major, self.Minor)

    @staticmethod
    def IsValid(values: List[int]) -> bool:
        if len(values) < 1 or values[0] < 0:
            return False
        hasNegative = False
        for entry in values:
            if not hasNegative:
                hasNegative = (entry < 0)
            elif entry >= 0:
                return False
        return True

    @staticmethod
    def TryFromString(strValue: str) -> Optional['SemanticVersion2']:
        suffix = None # type: Optional[str]
        if '-' in strValue:
            strValue, suffix = strValue.split('-', 1)
            if suffix is None or len(suffix) <= 0:
                return None

        entries = strValue.split('.')
        if len(entries) < 1 or len(entries) > 4:
            return None

        values = [-1, -1, -1, -1]
        try:
            for index, entry in enumerate(entries):
                values[index] = int(entry)
            if not SemanticVersion2.IsValid(values):
                return None
            return SemanticVersion2(values[0], values[1], values[2], values[3], suffix)
        except ValueError:
            return None

    @staticmethod
    def FromString(strValue: str) -> 'SemanticVersion2':
        result = SemanticVersion2.TryFromString(strValue)
        if result is None:
            raise Exception("Version string '{0} was not of the expected '1.2.3.4' or '1.2.3.4-suffix' format".format(strValue))
        return result
