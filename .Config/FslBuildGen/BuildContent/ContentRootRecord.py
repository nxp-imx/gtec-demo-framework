#!/usr/bin/env python3

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

from typing import Optional
from FslBuildGen.Log import Log
from FslBuildGen.BuildContent.PathVariables import PathVariables

class ContentRootRecord(object):
    def __init__(self, log: Log, sourcePath: str, pathVariables: Optional[PathVariables] = None) -> None:
        super(ContentRootRecord, self).__init__()
        expandedSourcePath = self.__ExpandSourcePath(sourcePath, pathVariables) if pathVariables is not None else sourcePath

        self.Id = expandedSourcePath.lower()
        self.SourcePath = sourcePath
        self.ResolvedPath = expandedSourcePath


    def __ExpandSourcePath(self, sourcePath: str, pathVariables: PathVariables) -> str:
        result = self.__TryExpandSourcePath(sourcePath, pathVariables.ProjectRoot, "${ProjectRoot}")
        if result is not None:
            return result
        result = self.__TryExpandSourcePath(sourcePath, pathVariables.Content, "${Content}")
        if result is not None:
            return result
        result = self.__TryExpandSourcePath(sourcePath, pathVariables.ContentBuild, "${Content.bld}")
        if result is not None:
            return result
        return sourcePath

    def __TryExpandSourcePath(self, sourcePath: str, root: str, rootName: str) -> Optional[str]:
        if sourcePath.startswith(rootName):
            return sourcePath.replace(rootName, root)
        return None
