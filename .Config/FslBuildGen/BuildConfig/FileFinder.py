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

from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.ToolConfig import ToolConfig

class FileFinder(object):
    @staticmethod
    def LocateFileInParentTree(log: Log, startDirectory: str, findFilename: str) -> str:
        try:
            res = IOUtil.TryFindFileInCurrentOrParentDir(startDirectory, findFilename)
            if res is not None:
                return res
        except (Exception) as ex:
            log.DoPrintError(str(ex))
        raise Exception("Could not find a '{0}' file in the parent tree of '{1}'".format(findFilename, startDirectory))

    @staticmethod
    def FindClosestFileInRoot(log: Log, toolConfig: ToolConfig, startDirectory: str, findFilename: str) -> str:
        foundPath = FileFinder.LocateFileInParentTree(log, startDirectory, findFilename)
        foundDir  = IOUtil.GetDirectoryName(foundPath)

        rootDir = toolConfig.TryFindRootDirectory(foundDir)
        if rootDir is None:
            raise Exception("Could not find a '{0}' file inside the project root".format(findFilename))
        return foundDir

    @staticmethod
    def TryFindClosestFileInRoot(log: Log, toolConfig: ToolConfig, startDirectory: str, findFilename: str) -> Optional[str]:
        foundPath = IOUtil.TryFindFileInCurrentOrParentDir(startDirectory, findFilename)
        if foundPath is None:
            return None

        foundDir  = IOUtil.GetDirectoryName(foundPath)
        rootDir = toolConfig.TryFindRootDirectory(foundDir)
        return foundDir if rootDir is not None else None
