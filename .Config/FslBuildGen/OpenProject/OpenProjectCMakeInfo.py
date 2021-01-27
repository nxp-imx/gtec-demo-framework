#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2020 NXP
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

from typing import Dict
from typing import List
from typing import Optional

class OpenProjectCMakeInfo(object):
    def __init__(self, buildDirectory: str, generator: str, sourceDirectory: str, configureArgs: List[str], configureSettingsDict: Dict[str,object],
                 installPrefix: str, buildThreads: Optional[int]) -> None:
        """
        configureArgs = Arguments to CMake that will be passed during the configure process. (Never pass -D arguments using this setting.)
        """

        for entry in configureArgs:
            if entry.strip().startswith("-D"):
                raise Exception("configureArgs can not contain entries that start with '-D' entry: '{0}'".format(entry))

        if "CMAKE_INSTALL_PREFIX" in configureSettingsDict:
            raise Exception("configureSettingsDict can not contain CMAKE_INSTALL_PREFIX")

        super().__init__()
        self.BuildDirectory = buildDirectory
        self.Generator = generator
        self.SourceDirectory = sourceDirectory
        self.ConfigureArgs = configureArgs
        self.ConfigureSettingsDict = configureSettingsDict
        self.InstallPrefix = installPrefix
        self.BuildThreads = buildThreads
