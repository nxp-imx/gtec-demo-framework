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
from FslBuildGen import Util
from FslBuildGen.Log import Log
from FslBuildGen.Exceptions import InvalidPackageNameException

class LocalVerbosityLevel(object):
    Info = 3
    Debug = 4
    Trace = 5

class JsonProjectIdCache(object):
    CURRENT_VERSION = 1

    def __init__(self, projectIdDict: Dict[str, str]) -> None:
        super().__init__()
        self.Version = JsonProjectIdCache.CURRENT_VERSION
        self.ProjectIdDict = projectIdDict

        projectIdToNameDict = {} # type: Dict[str, str]
        for packageName, packageProjectId in projectIdToNameDict.items():
            if not Util.IsValidPackageName(packageName):
                raise InvalidPackageNameException(packageName)
            if packageProjectId in projectIdDict:
                raise Exception("The package project id '{0}' is registered for multiple package names. First '{1}' Second '{2}'".format(packageProjectId, projectIdToNameDict[packageName], packageName))
            projectIdToNameDict[packageProjectId] = packageName

    def Add(self, packageName: str, packageProjectId: str) -> None:
        if not Util.IsValidPackageName(packageName):
            raise InvalidPackageNameException(packageName)
        self.ProjectIdDict[packageName] = packageProjectId

    def Remove(self, packageName: str) -> None:
        self.ProjectIdDict.pop(packageName)

    @staticmethod
    def TryLoad(log: Log, cacheFilename: str) -> Optional['JsonProjectIdCache']:
        try:
            strJson = IOUtil.TryReadFile(cacheFilename)
            if strJson is None:
                return None
            jsonDict = json.loads(strJson)
            if jsonDict["Version"] != JsonProjectIdCache.CURRENT_VERSION:
                raise Exception("Unsupported version")

            jsonProjectIdDict = jsonDict["ProjectIdDict"]
            finalDict = {} # type: Dict[str,str]

            for key, value in jsonProjectIdDict.items():
                if not isinstance(key, str) or not isinstance(value, str):
                    raise Exception("json decode failed")
                finalDict[key] = value

            return JsonProjectIdCache(finalDict)
        except:
            log.DoPrintWarning("Failed to decode cache file '{0}'".format(cacheFilename))
            return None

    @staticmethod
    def Save(log: Log, cacheFilename: str, JsonProjectIdCache: 'JsonProjectIdCache') -> None:
        log.LogPrintVerbose(LocalVerbosityLevel.Trace, "- Saving cache '{0}'".format(cacheFilename))
        jsonText = json.dumps(JsonProjectIdCache.__dict__, ensure_ascii=False, sort_keys=True, indent=2)
        IOUtil.WriteFileIfChanged(cacheFilename, jsonText)


    @staticmethod
    def IsEqual(lhs: 'JsonProjectIdCache', rhs: 'JsonProjectIdCache') -> bool:
        if lhs.Version != rhs.Version or len(lhs.ProjectIdDict) != len(rhs.ProjectIdDict):
            return False
        for key, value in lhs.ProjectIdDict.items():
            if key not in rhs.ProjectIdDict or value != rhs.ProjectIdDict[key]:
                return False
        return True
