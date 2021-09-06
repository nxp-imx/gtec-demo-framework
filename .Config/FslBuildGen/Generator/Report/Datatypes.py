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

from typing import cast
from enum import Enum
from FslBuildGen.Exceptions import UnknownTypeException

class FormatStringEnvironmentVariableResolveMethod(Enum):
    # Do a environment variable lookup right away
    Lookup = 0
    # Convert the OS shell specific environment variable format
    OSShellEnvironmentVariable = 1

    @staticmethod
    def ToInt(value: 'FormatStringEnvironmentVariableResolveMethod') -> int:
        return value.value

    @staticmethod
    def Convert(value: int) -> 'FormatStringEnvironmentVariableResolveMethod':
        if value == FormatStringEnvironmentVariableResolveMethod.Lookup.value:
            return FormatStringEnvironmentVariableResolveMethod.Lookup
        elif value == FormatStringEnvironmentVariableResolveMethod.OSShellEnvironmentVariable.value:
            return FormatStringEnvironmentVariableResolveMethod.OSShellEnvironmentVariable
        raise UnknownTypeException("Unknown FormatStringEnvironmentVariableResolveMethod: {0}".format(value))
