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

from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommand import PackageRecipeValidateCommand
from FslBuildGen.DataTypes import BuildRecipeValidateCommand

class PackageRecipeValidateCommandFindFileInPath(PackageRecipeValidateCommand):
    def __init__(self, name: str, expectedPath: Optional[str], help: Optional[str]) -> None:
        super().__init__("FindFileInPath", BuildRecipeValidateCommand.FindFileInPath, help)

        if '\\' in name or '/' in name:
            raise Exception("A filename can not contain backslash '\\' or slash '/': '{0}'".format(name))
        if expectedPath is not None:
            if '\\' in expectedPath:
                raise Exception("A path can not contain backslash '\\': '{0}'".format(expectedPath))
            if expectedPath.startswith('/'):
                raise Exception("A path can not start with a slash '/': '{0}'".format(expectedPath))
            if expectedPath.endswith('/'):
                raise Exception("A path can not end with a slash '/': '{0}'".format(expectedPath))
            expectedPath = IOUtil.NormalizePath(expectedPath)

        self.Name = name
        self.ExpectedPath = expectedPath
