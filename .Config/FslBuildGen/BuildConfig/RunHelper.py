#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2018 NXP
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
import subprocess
from FslBuildGen.Log import Log
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.Exceptions import ExitException


class RunHelper(object):
    @staticmethod
    def RunNinja(log: Log, ninjaExeInfo: ClangExeInfo, ninjaFile: str, currentWorkingDirectory: str, numBuildThreads: int, logOutput: bool) -> None:
        buildCommand = [ninjaExeInfo.Command, "-f", ninjaFile]
        #buildCommand += ["-d", "explain"]
        if numBuildThreads > 0:
            buildCommand += ["-j", str(numBuildThreads)]
        try:
            if log.Verbosity >= 4:
                log.LogPrint("Running command '{0}' in cwd: {1}".format(buildCommand, currentWorkingDirectory))
            result = RunHelper.RunNow(log, buildCommand, currentWorkingDirectory, logOutput)
            if result != 0:
                log.LogPrintWarning("The command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
                raise ExitException(result)
        except FileNotFoundError:
            log.DoPrintWarning("The command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(buildCommand), currentWorkingDirectory))
            raise

    @staticmethod
    def RunNow(log: Log, buildCommand: List[str], currentWorkingDirectory: str, logOutput: bool) -> int:
        if not logOutput:
            return subprocess.call(buildCommand, cwd=currentWorkingDirectory)
        try:
            with subprocess.Popen(buildCommand, cwd=currentWorkingDirectory, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, universal_newlines=True) as proc:
                output = proc.stdout.read().strip() if proc.stdout is not None else ""
                if proc.stdout is not None:
                    proc.stdout.close()
                return proc.wait()
        finally:
            if output is not None and len(output) > 0:
                log.DoPrint(output)
