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
import multiprocessing
from FslBuildGen.DataTypes import BuildThreads
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString


class PlatformBuildUtil(object):
    @staticmethod
    def AddBuildThreads(log: Log, rArgumentList: List[str], generatorOriginalName: str, buildThreads: int, external: bool = False) -> None:
        platformName = generatorOriginalName
        if (platformName == PlatformNameString.QNX or
            platformName == PlatformNameString.YOCTO or
            platformName == PlatformNameString.UBUNTU or
            (external and platformName == PlatformNameString.ANDROID)):
            PlatformBuildUtil.__AppendMakeThreads(log, rArgumentList, buildThreads)
        elif platformName == PlatformNameString.WINDOWS:
            if buildThreads == BuildThreads.Auto:
                log.LogPrint("Builder using BuildThreads: auto")
                rArgumentList += ['/maxcpucount']
            elif buildThreads != BuildThreads.NotDefined and buildThreads > 0:
                log.LogPrint("Builder using BuildThreads: {0}".format(buildThreads))
                rArgumentList += ['/maxcpucount:{0}'.format(buildThreads)]
        else:
            if buildThreads != BuildThreads.NotDefined:
                log.LogPrintWarning("The builder ignored 'BuildThreads'")

    @staticmethod
    def __AppendMakeThreads(log: Log, rBuildCommandList: List[str], buildThreads: int) -> None:
        if buildThreads == BuildThreads.Auto:
             buildThreads = multiprocessing.cpu_count()

        if buildThreads != BuildThreads.NotDefined and buildThreads > 0:
            log.LogPrint("Builder using BuildThreads: {0}".format(buildThreads))
            rBuildCommandList += ['-j', str(buildThreads)]

