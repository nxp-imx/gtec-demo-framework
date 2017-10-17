#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
from typing import Union
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen.Xml.XmlProjectRootConfigFile import XmlConfigFileAddRootDirectory


class ToolConfigRootDirectory(object):
    def __init__(self, log: Log,
                 basedUponXML: Optional[XmlConfigFileAddRootDirectory],
                 dynamicSourceRootDir: Union[Optional[XmlConfigFileAddRootDirectory], Optional['ToolConfigRootDirectory']] = None,
                 dynamicRootName: Optional[str] = None,
                 dynamicPath: Optional[str] = None) -> None:
        super(ToolConfigRootDirectory, self).__init__()
        if basedUponXML is not None:
            self.BasedOn = basedUponXML  # type: Union[XmlConfigFileAddRootDirectory, 'ToolConfigRootDirectory']
            self.Name = basedUponXML.Name # type: str
            self.DynamicName = basedUponXML.Name # type: str
        else:
            if dynamicSourceRootDir is None:
                raise Exception("dynamicSourceRootDir can not be none")
            if dynamicRootName is None:
                raise Exception("dynamicRootName can not be none")
            if dynamicPath is None:
                raise Exception("dynamicPath can not be none")
            self.BasedOn = dynamicSourceRootDir
            self.Name = dynamicRootName
            self.DynamicName = dynamicPath

        variableProcessor = VariableProcessor(log)
        # NOTE: workaround Union of tuples not being iterable bug in mypy https://github.com/python/mypy/issues/1575
        tupleResult = variableProcessor.TryExtractLeadingEnvironmentVariableNameAndPath(self.DynamicName, dynamicSourceRootDir != None)
        env = tupleResult[0]
        remainingPath = tupleResult[1]
        if env is None:
            raise Exception("Root dirs are expected to contain environment variables '{0}'".format(self.DynamicName))

        resolvedPath = IOUtil.GetEnvironmentVariableForDirectory(env) + remainingPath
        self.BashName = '${0}{1}'.format(env, remainingPath)  # type: str
        self.DosName = '%{0}%{1}'.format(env, remainingPath)  # type: str
        self.ResolvedPath = IOUtil.ToUnixStylePath(resolvedPath)  # type: str
        self.ResolvedPathEx = "{0}/".format(self.ResolvedPath) if len(self.ResolvedPath) > 0 else ""  # type: str
        self.__EnvironmentVariableName = env  # type: str


    def TryGetEnvironmentVariableName(self) -> str:
        return self.__EnvironmentVariableName
