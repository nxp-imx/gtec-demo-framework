#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2021 NXP
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
from FslBuildGen import Util
from FslBuildGen.BannedCommands import BannedCommands
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommand import PackageRecipeValidateCommand
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning import PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning
from FslBuildGen.DataTypes import BuildRecipeValidateCommand

class PackageRecipeValidateCommandFindExecutableFileInPath(PackageRecipeValidateCommand):
    def __init__(self, name: str, alternatives: List[str], expectedPath: Optional[str], minVersion: Optional[str], versionCommand: Optional[str], versionRegEx: Optional[str],
                 addOnErrorWarning: List[PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning], help: Optional[str]) -> None:
        super().__init__("FindFileInPath", BuildRecipeValidateCommand.FindExecutableFileInPath, help)

        self.__ValidateName(name)

        if expectedPath is not None:
            if '\\' in expectedPath:
                raise Exception("A path can not contain backslash '\\': '{0}'".format(expectedPath))
            if expectedPath.startswith('/'):
                raise Exception("A path can not start with a slash '/': '{0}'".format(expectedPath))
            if expectedPath.endswith('/'):
                raise Exception("A path can not end with a slash '/': '{0}'".format(expectedPath))
            expectedPath = IOUtil.NormalizePath(expectedPath)


        self.Name = name
        self.Alternatives = alternatives
        self.ExpectedPath = expectedPath
        self.MinVersion = minVersion
        self.VersionCommand = versionCommand
        self.VersionRegEx = versionRegEx

        self.AddOnErrorWarning = addOnErrorWarning

        self.__ValidateVersionCheck()


    def __ValidateName(self, name: str) -> None:
        if '\\' in name or '/' in name:
            raise Exception("A filename can not contain backslash '\\' or slash '/': '{0}'".format(name))
        trimmed = name.strip()
        if trimmed != name:
            raise Exception("Name contained leading or ending whitespaces'{0}'".format(name))
        if len(name) <= 0:
            raise Exception("Name length must be greater than zero")
        if not Util.IsValidCommandName(name):
            raise Exception("Name must start with a a-z or A-Z and can only contain a-z,A-Z,0-9,_ and - '{0}'".format(name))
        if name.lower() in BannedCommands.Commands:
            raise Exception("The command '{0}' is banned".format(name))

    def __ValidateVersionCheck(self) -> None:
        if self.MinVersion is None and self.VersionCommand is None and self.VersionRegEx is None:
            return
        if (self.MinVersion is None and len(self.AddOnErrorWarning) == 0) or self.VersionCommand is None or self.VersionRegEx is None:
            missingAttribs = []
            if self.MinVersion is None:
                missingAttribs.append("MinVersion")
            if self.VersionCommand is None:
                missingAttribs.append("VersionCommand")
            if self.VersionRegEx is None:
                missingAttribs.append("VersionRegEx")
            raise Exception("{0} are not defined".format(", ".join(missingAttribs)))
        if self.MinVersion is not None:
            trimmed = self.MinVersion.strip()
            if trimmed != self.MinVersion:
                raise Exception("MinVersion contained leading or ending whitespaces")
        trimmed = self.VersionCommand.strip()
        if trimmed != self.VersionCommand:
            raise Exception("VersionCommand contained leading or ending whitespaces")
        trimmed = self.VersionRegEx.strip()
        if trimmed != self.VersionRegEx:
            raise Exception("VersionRegEx contained leading or ending whitespaces")
