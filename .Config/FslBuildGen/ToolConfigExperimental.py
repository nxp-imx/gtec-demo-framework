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

from typing import List
from typing import Optional
#import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory
from FslBuildGen.ToolConfigExperimentalDefaultThirdPartyInstallDirectory import ToolConfigExperimentalDefaultThirdPartyInstallDirectory
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlExperimental
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory


class ToolConfigExperimental(object):
    def __init__(self, log: Log, rootDirs: List[ToolConfigRootDirectory],
                 basedUponXML: XmlExperimental, configFileName: str, projectRootDirectory: str) -> None:
        super().__init__()
        self.BasedOn = basedUponXML

        self.DefaultThirdPartyInstallReadonlyCacheDirectory = None  # type: Optional[ToolConfigExperimentalDefaultThirdPartyInstallDirectory]

        self.AllowDownloads = basedUponXML.AllowDownloads
        self.DisableDownloadEnv = basedUponXML.DisableDownloadEnv
        self.DefaultThirdPartyInstallDirectory = ToolConfigExperimentalDefaultThirdPartyInstallDirectory(log, basedUponXML.DefaultThirdPartyInstallDirectory, "DefaultThirdPartyInstallDirectory", False)
        self.DefaultThirdPartyInstallReadonlyCacheDirectory = self.__TryCreateReadonlyCache(log, basedUponXML.DefaultThirdPartyInstallReadonlyCacheDirectory)

        if log.Verbosity > 1:
            log.LogPrint("DefaultThirdPartyInstallDirectory: {0} which resolves to '{1}'".format(self.DefaultThirdPartyInstallDirectory.Name, self.DefaultThirdPartyInstallDirectory.ResolvedPath))
            if not self.DefaultThirdPartyInstallReadonlyCacheDirectory is None:
                log.LogPrint("DefaultThirdPartyInstallReadonlyCacheDirectory: {0} which resolves to '{1}'".format(self.DefaultThirdPartyInstallReadonlyCacheDirectory.Name, self.DefaultThirdPartyInstallReadonlyCacheDirectory.ResolvedPath))


    def __TryCreateReadonlyCache(self, log: Log, basedUponXML: Optional[XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory]) -> Optional[ToolConfigExperimentalDefaultThirdPartyInstallDirectory]:
        entryName = "DefaultThirdPartyInstallReadonlyCacheDirectory"
        if basedUponXML is None:
            raise Exception("No '{0}' was defined in the xml".format(entryName))

        variableProcessor = VariableProcessor(log)
        env = variableProcessor.TryExtractLeadingEnvironmentVariableName(basedUponXML.Name, False)
        if env is None:
            raise Exception("The {0} is expected to contain a environment variable '{1}'".format(entryName, basedUponXML.Name))

        resolvedPath = IOUtil.TryGetEnvironmentVariable(env)
        if resolvedPath is None:
            log.LogPrintVerbose(2, "Read only cache environment variable {0} not set, disabling cache".format(env))
            return None

        return ToolConfigExperimentalDefaultThirdPartyInstallDirectory(log, basedUponXML, entryName, True)
