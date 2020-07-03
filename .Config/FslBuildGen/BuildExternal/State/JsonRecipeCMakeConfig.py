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
from FslBuildGen.BuildExternal.State.JsonRecipePackageFileState import JsonRecipePackageFileState

class JsonRecipeCMakeVersion(object):
    def __init__(self) -> None:
        super().__init__()
        self.Major = 0      # type: int
        self.Minor = 0      # type: int

    def Set(self, major: int, minor: int) -> None:
        self.Major = major
        self.Minor = minor

    def __eq__(self, rhs: object) -> bool:
        if not isinstance(rhs, JsonRecipeCMakeVersion):
            return NotImplemented
        return (self.Major == rhs.Major and self.Minor == rhs.Minor)

    def __ne__(self, rhs: object) -> bool:
        if not isinstance(rhs, JsonRecipeCMakeVersion):
            return NotImplemented
        return (self.Major != rhs.Major or self.Minor != rhs.Minor)

    def __str__(self) -> str:
        return "{0}.{1}".format(self.Major, self.Minor)


class JsonRecipeCMakeConfig(object):
    def __init__(self) -> None:
        super().__init__()
        self.GeneratorName = ""                         # type: str
        self.CMakeVersion = JsonRecipeCMakeVersion()    # type: JsonRecipeCMakeVersion
        self.ConfigInternalArgs = []                    # type: List[str]
        self.ConfigUserArgs     = []                    # type: List[str]

    def Set(self, generatorName: str, cmakeVersion: JsonRecipeCMakeVersion, configInternalArgs: List[str], configUserArgs: List[str]) -> None:
        self.GeneratorName = generatorName
        self.CMakeVersion = cmakeVersion
        self.ConfigInternalArgs = configInternalArgs
        self.ConfigUserArgs = configUserArgs

    def __eq__(self, rhs: object) -> bool:
        if not isinstance(rhs, JsonRecipeCMakeConfig):
            return NotImplemented
        return (self.GeneratorName == rhs.GeneratorName and self.CMakeVersion == rhs.CMakeVersion and
                self.ConfigInternalArgs == rhs.ConfigInternalArgs and self.ConfigUserArgs == rhs.ConfigUserArgs)

    def __ne__(self, rhs: object) -> bool:
        if not isinstance(rhs, JsonRecipeCMakeConfig):
            return NotImplemented
        return (self.GeneratorName != rhs.GeneratorName or self.CMakeVersion != rhs.CMakeVersion or
                self.ConfigInternalArgs != rhs.ConfigInternalArgs or self.ConfigUserArgs != rhs.ConfigUserArgs)


    @staticmethod
    def GetDiff(lhs: 'JsonRecipeCMakeConfig', rhs: 'JsonRecipeCMakeConfig') -> str:
        res = []   # type: List[str]
        if lhs.GeneratorName != rhs.GeneratorName:
            res.append("GeneratorName: '{0}'!='{1}'".format(lhs.GeneratorName, rhs.GeneratorName))
        if lhs.CMakeVersion != rhs.CMakeVersion:
            res.append("CMakeVersion: {0}!={1}".format(lhs.CMakeVersion, rhs.CMakeVersion))
        if lhs.ConfigInternalArgs != rhs.ConfigInternalArgs:
            res.append("ConfigInternalArgs: {0}!={1}".format(lhs.ConfigInternalArgs, rhs.ConfigInternalArgs))
        if lhs.ConfigUserArgs != rhs.ConfigUserArgs:
            res.append("ConfigUserArgs: {0}!={1}".format(lhs.ConfigUserArgs, rhs.ConfigUserArgs))
        return ", ".join(res)
