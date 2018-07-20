#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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
from FslBuildGen import IOUtil
from FslBuildGen.PlatformUtil import PlatformUtil

class CMakeVersion(object):
    def __init__(self, major: int, minor: int) -> None:
        super().__init__()
        self.Major = major
        self.Minor = minor


def RunCommand(cmd: List[str]) -> str:
    try:
        with subprocess.Popen(cmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, universal_newlines=True) as proc:
            verStr = proc.stdout.read().strip()
            proc.stdout.close()
            if proc.wait() != 0:
                raise Exception("Failed to get CMake version (1).")
            if not isinstance(verStr, str):
                raise Exception("Failed to get CMake version (2).")
            return verStr
    except OSError as e:
        raise Exception("Failed to get CMake version (3).")


class CMakeUtil(object):
    @staticmethod
    def GetVersion() -> CMakeVersion:
        exeName = PlatformUtil.GetPlatformDependentExecuteableName("cmake", PlatformUtil.DetectBuildPlatformType())
        path = IOUtil.TryFindExecutable(exeName)
        if path is None:
            raise Exception("Could not locate cmake in path")
        cmd = [path, "--version"]
        version = RunCommand(cmd)
        versionString = "cmake version "
        if not version.startswith(versionString):
            raise Exception("Failed to parse cmake version string '{0}'".format(versionString))
        version = version[len(versionString):]
        indexEnd = version.find('\n')
        indexEnd = indexEnd if indexEnd >= 0 else len(version)
        version = version[0:indexEnd]
        indexEndMajor = version.index('.')
        indexEndMinor = version.index('.', indexEndMajor+1)
        versionMajor = version[0:indexEndMajor]
        versionMinor = version[indexEndMajor+1:indexEndMinor]
        return CMakeVersion(int(versionMajor), int(versionMinor))
