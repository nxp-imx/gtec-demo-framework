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

from typing import Optional
from FslBuildGen import PathUtil

class UserCMakeConfig(object):
    """
    The cmake settings that can be overriden by the user
    """
    def __init__(self, buildDir: Optional[str], buildDirId: Optional[int], generatorName: Optional[str], installPrefix: Optional[str], configUserAppArgs: Optional[str],
                 configGlobalArgs: Optional[str], allowFindPackage: Optional[bool]) -> None:
        super().__init__()

        if buildDir is not None:
            PathUtil.ValidateIsNormalizedPath(buildDir, "BuildDir")
        if installPrefix is not None:
            PathUtil.ValidateIsNormalizedPath(installPrefix, "DefaultInstallPrefix")

        self.BuildDir = buildDir
        self.BuildDirId = buildDirId
        self.GeneratorName = generatorName
        self.InstallPrefix = installPrefix
        self.ConfigAppArgs = "" if configUserAppArgs is None else configUserAppArgs
        self.ConfigGlobalArgs = "" if configGlobalArgs is None else configGlobalArgs
        self.AllowFindPackage = allowFindPackage
