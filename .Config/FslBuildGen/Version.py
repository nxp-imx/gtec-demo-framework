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

from typing import Optional
from typing import List

class Version(object):
    def __init__(self, major: int, minor: int = -1, patch: int = -1, tweak: int = -1) -> None:
        if (minor < 0 and (patch >= 0 or tweak >= 0)) or (patch < 0 and tweak >= 0):
            raise Exception("Invalid version")

        self.Major = major
        self.Minor = minor if minor >= 0 else 0
        self.Patch = patch if patch >= 0 else 0
        self.Tweak = tweak if tweak >= 0 else 0

        self.__Set = 8
        self.__Set |= 4 if minor >= 0 else 0
        self.__Set |= 2 if patch >= 0 else 0
        self.__Set |= 1 if tweak >= 0 else 0

    def IsCompatible(self, other: 'Version') -> bool:
        if self.Major != other.Major:
            return False
        if self.Minor > other.Minor:
            return False or ((self.__Set & 4) == 0)
        if self.Patch > other.Patch:
            return False or ((self.__Set & 2) == 0)
        if self.Tweak > other.Tweak:
            return False or ((self.__Set & 1) == 0)
        return True

    def __str__(self) -> str:
        if self.__Set == (8|4|2|1):
            return "{0}.{1}.{2}.{3}".format(self.Major, self.Minor, self.Patch, self.Tweak)
        if self.__Set == (8|4|2):
            return "{0}.{1}.{2}".format(self.Major, self.Minor, self.Patch)
        if self.__Set == (8|4):
            return "{0}.{1}".format(self.Major, self.Minor)
        return "{0}".format(self.Major)

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
    def TryFromString(strValue: str) -> Optional['Version']:
        entries = strValue.split('.')
        if len(entries) < 1 or len(entries) > 4:
            return None

        values = [-1, -1, -1, -1]
        try:
            for index, entry in enumerate(entries):
                values[index] = int(entry)
            if not Version.IsValid(values):
                return None
            return Version(values[0], values[1], values[2], values[3])
        except ValueError:
            return None

    @staticmethod
    def FromString(strValue: str) -> 'Version':
        result = Version.TryFromString(strValue)
        if result is None:
            raise Exception("Version string '{0} was not of the expected '1.2.3.4' format".format(strValue))
        return result
