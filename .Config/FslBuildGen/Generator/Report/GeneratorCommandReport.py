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

class GeneratorCommandReport(object):
    def __init__(self, useAsRelative: bool, commandFormatString: str,
                 arguments: List[str], nativeArguments: List[str],
                 currentWorkingDirectoryFormatString: Optional[str] = None,
                 runInEnvScript: Optional[str] = None,
                 nativeArgumentSeparator: Optional[str] = None) -> None:
        """
            The information stored in a format string can contain both variables and environment variables and
            it need to be formatted/converted using the ReportVariableFormatter before being used.

            Some notes:
            Each variant name string as used by the generator.
            - Each normal variant is represented as a ${VARIANT_NAME} variable.
            - Each virtual variant is represented as a $(VARIANT_NAME) variable.
            Please note there can be other variables and environment variables as required by the generator
        """
        super().__init__()
        if commandFormatString is None:
            raise Exception("commandFormatString can not be None")
        if commandFormatString.startswith('/') or ':' in commandFormatString:
            raise Exception("commandFormatString can not be absolute")
        if currentWorkingDirectoryFormatString is not None and (currentWorkingDirectoryFormatString.startswith('/') or ':' in currentWorkingDirectoryFormatString):
            raise Exception("currentWorkingDirectoryFormatString can not be absolute")
        if runInEnvScript is not None and (runInEnvScript.startswith('/') or ':' in runInEnvScript):
            raise Exception("runInEnvScript can not be absolute")
        if len(nativeArguments) != 0 and nativeArgumentSeparator is None:
            raise Exception("When native args are supplied there should be a native argument separator")

        # if this is true the exe should be run via as 'relative' command.
        # if this is false it will be run as a absolute path command relative to the package absolute path
        self.UseAsRelative = useAsRelative

        # The command that should be run with the cwdFormatString as CWD
        # This normally represents a absolute path, but for build-in commands it can be relative.
        # Relative paths are considered relative to the cwdFormatString
        self.CommandFormatString = commandFormatString
        self.Arguments = arguments

        # The argument that indicates that the rest of the arguments are native arguments
        self.NativeArgumentSeparator = nativeArgumentSeparator;
        # Arguments that should be send to the native command
        self.NativeArguments = nativeArguments

        # if this is None, the package absolute path should be used instead
        self.CurrentWorkingDirectoryFormatString = currentWorkingDirectoryFormatString

        # A optional script that can be used to run things with the 'dynamic' environment used for virtual variants
        self.RunInEnvScript = runInEnvScript
