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

from typing import Any
from typing import Dict
from typing import List
import json
from FslBuildGen.BuildExternal.State.JsonDictType import JsonDictType
from FslBuildGen.BuildExternal.State.JsonRecipePackageContentState import JsonRecipePackageContentState
from FslBuildGen.BuildExternal.State.JsonRecipePackageFileState import JsonRecipePackageFileState

class BuildInfoComplexJsonDecoder(object):
    @staticmethod
    def DecodeJson(jsonDict: JsonDictType) -> JsonRecipePackageContentState:
        directories = jsonDict["Directories"] # type: List[str]
        files       = jsonDict["Files"]       # type: List[Dict[str,str]]

        result = JsonRecipePackageContentState()
        for entry in directories:
            result.Directories.append(entry)
        for fileEntry in files:
             result.Files.append(BuildInfoComplexJsonDecoder.DecodeJsonFileState(fileEntry))
        return result


    @staticmethod
    def DecodeJsonFileState(jsonDict: Dict[str,str]) -> JsonRecipePackageFileState:
        name = jsonDict["Name"]
        length = int(jsonDict["Length"])
        modifiedDate = jsonDict["ModifiedDate"]
        checksum = jsonDict["Checksum"]
        tagChecksum = jsonDict["TagChecksum"]

        result = JsonRecipePackageFileState()
        result.Set(name, length, modifiedDate, checksum, tagChecksum)
        return result
