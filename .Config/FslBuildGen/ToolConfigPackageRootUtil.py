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
from FslBuildGen.Log import Log
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory
from FslBuildGen import Util

class ToolConfigPackageRootUtil(object):
    @staticmethod
    def TryFindRootDirectory(rootDirectories: List[ToolConfigRootDirectory], path: Optional[str]) -> Optional[ToolConfigRootDirectory]:
        """
        Try to find the nearest root directory
        """
        if path is None:
            return None
        for rootDir in rootDirectories:
            if path.startswith(rootDir.ResolvedPathEx) or path == rootDir.ResolvedPath:
                return rootDir
        return None

    @staticmethod
    def ToPath(rootDirectories: List[ToolConfigRootDirectory], path: str) -> str:
        """
        convert to a path that we know reside in one of the package roots
        """
        foundPath = ToolConfigPackageRootUtil.TryToPath(rootDirectories, path)
        if foundPath is not None:
            return foundPath
        raise UsageErrorException("the folder '{0}' does not reside inside one of the root dirs".format(path))

    @staticmethod
    def TryToPath(rootDirectories: List[ToolConfigRootDirectory], path: str) -> Optional[str]:
        """
        convert to a path that we know reside in one of the package roots
        """
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '{0}'".format(path))
        for rootDir in rootDirectories:
            if path.startswith(rootDir.ResolvedPathEx):
                lenRootPath = len(rootDir.ResolvedPathEx)
                path = path[lenRootPath:]
                return rootDir.Name + "/" + Util.UTF8ToAscii(path)
            elif path == rootDir.ResolvedPath:
                return rootDir.Name + "/"
        return None

