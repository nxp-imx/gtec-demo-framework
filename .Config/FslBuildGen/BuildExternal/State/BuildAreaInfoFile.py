#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2018 NXP
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
from FslBuildGen import IOUtil
from FslBuildGen.BuildExternal.State.JsonDictType import JsonDictType


class BuildAreaInfoFileElements(object):
    FileFormatVersion = "FileFormatVersion"
    SDKPath = "SDKPath"

    CURRENT_VERSION = "1"


class BuildAreaInfoFile(object):
    def __init__(self, jsonDict: JsonDictType) -> None:
        super().__init__()
        self.FileFormatVersion = jsonDict[BuildAreaInfoFileElements.FileFormatVersion]
        self.SDKPath = IOUtil.NormalizePath(jsonDict[BuildAreaInfoFileElements.SDKPath])

    @staticmethod
    def IsDictValid(srcDict: JsonDictType) -> bool:
        if not BuildAreaInfoFileElements.FileFormatVersion in srcDict or not BuildAreaInfoFileElements.SDKPath in srcDict:
            return False
        if not isinstance(srcDict[BuildAreaInfoFileElements.FileFormatVersion], str):
            return False
        if not isinstance(srcDict[BuildAreaInfoFileElements.SDKPath], str):
            return False

        return cast(bool, srcDict[BuildAreaInfoFileElements.FileFormatVersion] == BuildAreaInfoFileElements.CURRENT_VERSION)

    @staticmethod
    def CreateDict(sdkPath: str) -> JsonDictType:
        jsonRootDict = {}
        jsonRootDict[BuildAreaInfoFileElements.FileFormatVersion] = BuildAreaInfoFileElements.CURRENT_VERSION
        jsonRootDict[BuildAreaInfoFileElements.SDKPath] = sdkPath
        return jsonRootDict
