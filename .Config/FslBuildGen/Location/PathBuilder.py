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

from FslBuildGen.Log import Log
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Vars.VariableProcessor import VariableProcessor

class PathBuilder(object):
    def __init__(self, log: Log, variableProcessor: VariableProcessor, platformName: str) -> None:
        super().__init__()

        self.__Log = log                             # type: Log
        self.__VariableProcessor = variableProcessor # type: VariableProcessor


    def ResolveFilePath(self, sourcePath: str) -> ResolvedPath:
        if sourcePath.find('..') != -1:
            raise Exception("'..' is now allowed in file paths ('{0}')".format(sourcePath))
        if sourcePath.find('\\') != -1:
            raise Exception("'\\' is now allowed in file paths ('{0}')".format(sourcePath))

        resolvedPath = self.__VariableProcessor.ResolveAbsolutePathWithLeadingEnvironmentVariablePathAsFile(sourcePath)
        return ResolvedPath(sourcePath, resolvedPath)


    def ResolveDirectoryPath(self, sourcePath: str, checkExists: bool = True) -> ResolvedPath:
        if sourcePath.find('..') != -1:
            raise Exception("'..' is now allowed in directory paths ('{0}')".format(sourcePath))
        if sourcePath.find('\\') != -1:
            raise Exception("'\\' is now allowed in directory paths ('{0}')".format(sourcePath))

        resolvedPath = self.__VariableProcessor.ResolveAbsolutePathWithLeadingEnvironmentVariablePathAsDir(sourcePath, checkExists=checkExists)
        return ResolvedPath(sourcePath, resolvedPath)


    def ResolveFilenameWithVariables(self, sourcePath: str) -> str:
        if sourcePath.find('..') != -1:
            raise Exception("'..' is now allowed in directory paths ('{0}')".format(sourcePath))
        if sourcePath.find('\\') != -1:
            raise Exception("'\\' is now allowed in directory paths ('{0}')".format(sourcePath))
        if sourcePath.find('$') == -1:
            return sourcePath
        resolvedPath = self.__VariableProcessor.ResolveFilenameWithVariables(sourcePath)
        return resolvedPath

