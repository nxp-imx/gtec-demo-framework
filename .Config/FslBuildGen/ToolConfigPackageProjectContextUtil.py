#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from typing import List
from typing import Optional
#from FslBuildGen.Log import Log
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext

class ToolConfigPackageProjectContextUtil(object):
    @staticmethod
    def TryFindToProjectContext(contexts: List[ToolConfigProjectContext], path: Optional[str]) -> Optional[ToolConfigProjectContext]:
        """
        Try to find the project context
        """
        if path is None:
            return None
        foundContext = None # type: Optional[ToolConfigProjectContext]
        lenPath = 0
        # Scan all contexts and find the best match
        for context in contexts:
            if path == context.Location.ResolvedPath:
                return context
            if path.startswith(context.Location.ResolvedPathEx) and len(context.Location.ResolvedPathEx) > lenPath:
                foundContext = context
                lenPath = len(context.Location.ResolvedPathEx)
        return foundContext

    @staticmethod
    def FindProjectContext(contexts: List[ToolConfigProjectContext], path: str) -> ToolConfigProjectContext:
        context = ToolConfigPackageProjectContextUtil.TryFindToProjectContext(contexts, path)
        if context is None:
            raise UsageErrorException("Could not locate a context for path '{0}'".format(path))
        return context
