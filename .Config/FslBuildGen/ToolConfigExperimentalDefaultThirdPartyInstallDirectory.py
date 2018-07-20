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
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlExperimentalDefaultThirdPartyInstallDirectory
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory


class ToolConfigExperimentalDefaultThirdPartyInstallDirectory(object):
    def __init__(self, log: Log,
                 basedUponXML: Optional[Union[XmlExperimentalDefaultThirdPartyInstallDirectory, XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory]],
                 entryName: str, isReadonlyCache: bool) -> None:
        super(ToolConfigExperimentalDefaultThirdPartyInstallDirectory, self).__init__()

        if basedUponXML is None:
            raise Exception("No '{0}' was defined in the xml".format(entryName))

        self.__Log = log # type: Log
        self.BasedOn = basedUponXML
        self.Name = basedUponXML.Name  # type: str
        self.DynamicName = basedUponXML.Name  # type: str
        self.IsReadonlyCache = isReadonlyCache  # type: bool

        variableProcessor = VariableProcessor(log)

        # NOTE: workaround Union of tuples not being iterable bug in mypy https://github.com/python/mypy/issues/1575
        tupleResult = variableProcessor.TryExtractLeadingEnvironmentVariableNameAndPath(self.DynamicName, False)
        env = tupleResult[0]
        remainingPath = tupleResult[1]
        if env is None:
            raise Exception("The {0} is expected to contain a environment variable '{1}'".format(entryName, self.DynamicName))

        resolvedPath = self.__GetEnvironmentVariable(env)

        self.__EnvironmentVariableName = env  # type: str
        self.BashName = '${0}{1}'.format(env, remainingPath)  # type: str
        self.DosName = '%{0}%{1}'.format(env, remainingPath)  # type: str
        self.ResolvedPath = IOUtil.ToUnixStylePath(resolvedPath)  # type: str
        self.ResolvedPathEx = "{0}/".format(self.ResolvedPath) if len(self.ResolvedPath) > 0 else ""  # type: str


    def __GetEnvironmentVariable(self, name: str) -> str:
        # For cache entries we allow the variable to not be defined, but if it is defned we retrieve is as normal
        value = IOUtil.TryGetEnvironmentVariable(name)
        if value is None:
            raise EnvironmentError("{0} environment variable not set".format(name))

        value = IOUtil.NormalizePath(value)
        if value is None:
            raise EnvironmentError("{0} environment variable not set".format(name))

        if not IOUtil.IsAbsolutePath(value):
            raise EnvironmentError("{0} environment path '{1}' is not absolute".format(name, value))

        if value.endswith("/"):
            raise EnvironmentError("{0} environment path '{1}' not allowed to end with '/' or '\'".format(name, value))

        # Create the directory if it didnt exist
        if not IOUtil.IsDirectory(value) and not IOUtil.Exists(value):
            self.__Log.LogPrint("The directory '{0}' did not exist, creating it".format(value))
            IOUtil.SafeMakeDirs(value)

        if not IOUtil.IsDirectory(value):
            raise EnvironmentError("The {0} environment variable content '{1}' does not point to a valid directory".format(name, value))
        return value


    def TryGetEnvironmentVariableName(self) -> str:
        return self.__EnvironmentVariableName
