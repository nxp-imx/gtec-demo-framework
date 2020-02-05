#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2019 NXP
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
import json
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log

class BuildConfigureCache(object):
    def __init__(self, fileHashDict: Dict[str,str], commandList: List[str], platformName: str) -> None:
        super().__init__()
        self.Version = 2
        self.FileHashDict = fileHashDict
        self.CommandList = commandList
        self.PlatformName = platformName;

    @staticmethod
    def TryLoad(log: Log, cacheFilename: str) -> Optional['BuildConfigureCache']:
        try:
            strJson = IOUtil.TryReadFile(cacheFilename)
            if strJson is None:
                return None
            jsonDict = json.loads(strJson)
            if jsonDict["Version"] != 2:
                raise Exception("Unsupported version")

            jsonFileHashDict = jsonDict["FileHashDict"]
            finalDict = {} # type: Dict[str,str]
            for key, value in jsonFileHashDict.items():
                if not isinstance(key, str) or not isinstance(value, str):
                    raise Exception("json decode failed")
                finalDict[key] = value

            finalCommandList = [] # type: List[str]
            jsonCommandList = jsonDict["CommandList"]
            for value in jsonCommandList:
                if not isinstance(value, str):
                    raise Exception("json decode failed")
                finalCommandList.append(value)

            platformName = jsonDict["PlatformName"] # type: str
            return BuildConfigureCache(finalDict, finalCommandList, platformName)
        except:
            log.DoPrintWarning("Failed to decode cache file '{0}'".format(cacheFilename))
            return None

    @staticmethod
    def Save(log: Log, cacheFilename: str, buildConfigureCache: 'BuildConfigureCache') -> None:
        log.LogPrintVerbose(4, "- Saving generated file hash cache")
        jsonText = json.dumps(buildConfigureCache.__dict__, ensure_ascii=False, sort_keys=True, indent=2)
        IOUtil.WriteFileIfChanged(cacheFilename, jsonText)


    @staticmethod
    def IsEqual(lhs: 'BuildConfigureCache', rhs: 'BuildConfigureCache') -> bool:
        if lhs.Version != rhs.Version or len(lhs.FileHashDict) != len(rhs.FileHashDict) or len(lhs.CommandList) != len(rhs.CommandList):
            return False
        for key, value in lhs.FileHashDict.items():
            if key not in rhs.FileHashDict or value != rhs.FileHashDict[key]:
                return False

        for index, value in enumerate(lhs.CommandList):
            if value != rhs.CommandList[index]:
                return False

        return lhs.PlatformName == rhs.PlatformName