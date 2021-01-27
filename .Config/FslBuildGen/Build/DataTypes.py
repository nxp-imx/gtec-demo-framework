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

from typing import List
from typing import Optional
from enum import Enum

class CommandType(Enum):
    Build = 0
    Clean = 1
    Install = 2
    Open = 3
    Open2 = 4
    Config = 5
    ConfigIfChanged = 6

    @staticmethod
    def FromString(value: str) -> 'CommandType':
        if value == "build":
            return CommandType.Build
        elif value == "clean":
            return CommandType.Clean
        elif value == "install":
            return CommandType.Install
        elif value == "open":
            return CommandType.Open
        elif value == "open2":
            return CommandType.Open2
        elif value == "config":
            return CommandType.Config
        elif value == "configIfChanged":
            return CommandType.ConfigIfChanged
        raise Exception("Unsupported Command '{0}'".format(value))

    @staticmethod
    def ToString(value: 'CommandType') -> str:
        result = CommandType.TryToString(value)
        if result is not None:
            return result
        raise Exception("Unsupported Command '{0}'".format(value))


    @staticmethod
    def TryToString(value: 'CommandType') -> Optional[str]:
        if value == CommandType.Build:
            return "build"
        elif value == CommandType.Clean:
            return "clean"
        elif value == CommandType.Install:
            return "install"
        elif value == CommandType.Open:
            return "open"
        elif value == CommandType.Open2:
            return "open2"
        elif value == CommandType.Config:
            return "config"
        elif value == CommandType.ConfigIfChanged:
            return "configIfChanged"
        return None

    @staticmethod
    def AllStrings() -> List[str]:
        return  [CommandType.ToString(CommandType.Build), CommandType.ToString(CommandType.Clean), CommandType.ToString(CommandType.Install),
                 CommandType.ToString(CommandType.Open), CommandType.ToString(CommandType.Open2), CommandType.ToString(CommandType.Config)]
