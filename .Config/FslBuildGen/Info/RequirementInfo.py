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

from typing import List
from typing import Optional
from enum import Enum

class RequirementType(object):
    Undefined = 0
    Feature = 1
    Extension = 2

    @staticmethod
    def FromString(value: str) -> int:
        if value == "feature":
            return RequirementType.Feature
        elif value == "extension":
            return RequirementType.Extension
        else:
            raise Exception("Unknown value: {0}".format(value))


    @staticmethod
    def ToString(value: int) -> str:
        if value == RequirementType.Feature:
            return "feature"
        elif value == RequirementType.Extension:
            return "extension"
        else:
            raise Exception("Unknown value {0}".format(value))


class RequirementInfo(object):
    def __init__(self, name: str, requirementType: int, version: Optional[str], extends: str = "") -> None:
        super().__init__()
        self.Name = name
        self.Type = requirementType
        self.Version = version
        self.Extends = extends


    def IsContentEqual(self, src: 'RequirementInfo') -> bool:
        return self.Name == src.Name and self.Type == src.Type and self.Version == src.Version and self.Extends == src.Extends


    def DebugGetDifferenceString(self, src: 'RequirementInfo') -> str:
        strDiff = ""
        if self.Name != src.Name:
            strDiff = self.__AppendStr(strDiff, "Name '{0}' != '{1}'".format(self.Name, src.Name))
        if self.Type != src.Type:
            strDiff = self.__AppendStr(strDiff, "Type '{0}' != '{1}'".format(self.Type, src.Type))
        if self.Version != src.Version:
            strDiff = self.__AppendStr(strDiff, "Version '{0}' != '{1}'".format(self.Version, src.Version))
        if self.Extends != src.Extends:
            strDiff = self.__AppendStr(strDiff, "Extends '{0}' != '{1}'".format(self.Extends, src.Extends))
        return strDiff

    def __AppendStr(self, src1: str, src2: str) -> str:
        if len(src1) > 0:
            return "{0}, {1}".format(src1, src2)
        return "{0}".format(src2)

