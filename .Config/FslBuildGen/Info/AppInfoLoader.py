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

from typing import Dict
from typing import List
from typing import Set
from typing import Union
from FslBuildGen import IOUtil
from FslBuildGen.Info.AppInfo import AppInfo
from FslBuildGen.Info import AppInfoJson
from FslBuildGen.Log import Log


class AppInfoLoader(object):
    def __init__(self, log: Log, appInfoFilename: str, requestedFiles: Union[List[str],Set[str]],
                 scanSubdirectories: bool, currentPath: str, activePlatformNameId: str) -> None:
        self.Log = log

        requestedFileSet = set(requestedFiles)
        if scanSubdirectories:
            foundFiles = self.__ScanSubdirectories(currentPath, appInfoFilename)
            for foundFile in foundFiles:
                if foundFile not in requestedFileSet:
                    requestedFileSet.add(foundFile)

        requestedFiles = list(requestedFileSet)
        requestedFiles.sort()

        self.CacheDict = {} # type: Dict[str, AppInfo]
        self.__CacheFiles(self.CacheDict, requestedFiles, activePlatformNameId)


    def IsEmpty(self) -> bool:
        return len(self.CacheDict) <= 0


    def GetDict(self) -> Dict[str, AppInfo]:
        return self.CacheDict


    def __ScanSubdirectories(self, scanPath: str, appInfoFilename: str) -> List[str]:
        filesFound = IOUtil.GetFilePaths(scanPath, appInfoFilename)
        return [entry for entry in filesFound if IOUtil.GetFileName(entry) == appInfoFilename]


    def __CacheFiles(self, rCacheDict: Dict[str, AppInfo], fileList: List[str], activePlatformNameId: str) -> None:
        for filePath in fileList:
            appInfo = self.__Load(filePath)
            if appInfo.PlatformName.lower() != activePlatformNameId:
                raise Exception("The file '{0}' contained information for '{1}' not '{2}'".format(filePath, appInfo.PlatformName, activePlatformNameId))
            rCacheDict[filePath] = appInfo


    def __Load(self, filename: str) -> AppInfo:
        appInfo = AppInfoJson.TryLoad(self.Log, filename)
        if appInfo is None:
            raise Exception("Failed to load requested file: {0}".format(filename))
        return appInfo
