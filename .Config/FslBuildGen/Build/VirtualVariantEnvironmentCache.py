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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.Build.CaptureEnvironmentVariablesFromScript import CaptureEnvironmentVariablesFromScript
from FslBuildGen.Log import Log


class VirtualVariantEnvironmentCache(object):
    def __init__(self, log: Log, pythonScriptRoot: str, allowCaching: bool) -> None:
        super().__init__()
        if not IOUtil.IsAbsolutePath(pythonScriptRoot):
            raise Exception("pythonScriptRoot '{0}' is not absolute".format(pythonScriptRoot))
        self.Log = log
        self.PythonScriptRoot = pythonScriptRoot    # type: str
        self.EnvDict = {}                           # type: Dict[str,str]
        self.AllowCaching = allowCaching

    def GetCachedValue(self, key: str) -> str:
        if key not in self.EnvDict:
            print("")
        return self.EnvDict[key]

    def TryGetCachedValue(self, key: str) -> Optional[str]:
        if key in self.EnvDict:
            return self.EnvDict[key]
        return None

    def CacheEnv(self, keys: List[str], runCommand: List[str]) -> None:
        #numFound = 0
        #for key in keys:
        #    if key in self.EnvDict:
        #        numFound += 1
        #if len(keys) == numFound:
        #    return

        if not self.AllowCaching:
            # we can't cache the environment variables as each package could modify them :(
            self.EnvDict.clear()

        keys = list(set(keys))
        unknownKeys = keys
        if self.AllowCaching:
            unknownKeys = []
            for entry in keys:
                if not (entry.startswith("$(") and entry.endswith(")")):
                    raise Exception("Environment variable not in the correct $(NAME) format {0}".format(entry))
                strippedEntry = entry[2:-1]
                if strippedEntry not in self.EnvDict:
                    unknownKeys.append(entry)
                else:
                    self.Log.LogPrintVerbose(4, "Using captured value for {0}".format(entry))

        if len(unknownKeys) > 0:
            captureDict = CaptureEnvironmentVariablesFromScript.Capture(self.Log, runCommand, self.PythonScriptRoot, unknownKeys)
            for key, value in captureDict.items():
                if not key in self.EnvDict:
                    self.EnvDict[key] = value
                elif self.EnvDict[key] != value:
                    raise Exception("newly cached value for key '{0} does not match cached value. Cached: {1}, new: {2}".format(key, self.EnvDict[key], value))

