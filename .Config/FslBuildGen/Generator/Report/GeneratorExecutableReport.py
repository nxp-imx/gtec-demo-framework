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

#from typing import Callable
#from typing import Dict
#from typing import List
from typing import Optional
#from typing import Set
#from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.Generator.Report.Datatypes import FormatStringEnvironmentVariableResolveMethod


class GeneratorExecutableReport(object):
    def __init__(self, useAsRelative: bool, exeFormatString: str, runScript: Optional[str] = None,
                 environmentVariableResolveMethod: FormatStringEnvironmentVariableResolveMethod = FormatStringEnvironmentVariableResolveMethod.Lookup) -> None:
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
        if exeFormatString is None:
            raise Exception("exeFormatString can not be None")
        if exeFormatString.startswith('/') or ':' in exeFormatString:
            raise Exception("exeFormatString can not be absolute")

        # if this is true the exe should be run via as 'relative' command.
        # if this is false it will be run as a absolute path command relative to the package absolute path
        self.UseAsRelative = useAsRelative

        # Should be run with CWD equal to the package absolute path.
        self.ExeFormatString = exeFormatString

        # If this is set then it is the script that will be used to launch the executable
        # when we run using --ForAllExe.
        # The real executable is passed as the first paramter to the script followed by
        # all the paramaters intended for the exectuable
        # If this is None then the executable will be launched directly
        # Should be run with CWD equal to the package absolute path.
        self.RunScript = runScript

        # if set this is the method to use for resolving the exe format string when running
        self.EnvironmentVariableResolveMethod = environmentVariableResolveMethod  # type: FormatStringEnvironmentVariableResolveMethod
