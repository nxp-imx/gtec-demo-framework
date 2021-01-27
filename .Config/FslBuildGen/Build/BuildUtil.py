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
from typing import Optional
from enum import Enum
import multiprocessing
from FslBuildGen.BuildExternal.CMakeTypes import CMakeGeneratorName
from FslBuildGen.DataTypes import BuildThreads
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.CMakeUtil import CMakeVersion

class PlatformBuildTypeInfo(Enum):
    Legacy = 0
    CMake = 1
    CMakeCustom = 2


class PlatformBuildUtil(object):
    @staticmethod
    def AddBuildThreads(log: Log, rArgumentList: List[str], generatorOriginalName: str, buildThreads: int,
                        buildTypeInfo: PlatformBuildTypeInfo, cmakeConfig: Optional[GeneratorCMakeConfig], external: bool) -> int:
        numBuildThreads = PlatformBuildUtil.GetRecommendedBuildThreads(buildThreads)
        if PlatformBuildUtil.TryAddBuildThreads(log, rArgumentList, generatorOriginalName, numBuildThreads, buildTypeInfo, cmakeConfig, external):
            log.LogPrint("Builder using BuildThreads: {0}".format(numBuildThreads))
        else:
            log.LogPrintWarning("The builder ignored 'BuildThreads'")
        return numBuildThreads

    @staticmethod
    def TryAddBuildThreads(log: Log, rArgumentList: List[str], generatorOriginalName: str, numBuildThreads: int,
                           buildTypeInfo: PlatformBuildTypeInfo, cmakeConfig: Optional[GeneratorCMakeConfig], external: bool) -> bool:

        if buildTypeInfo == PlatformBuildTypeInfo.Legacy:
            # This is the old add thread arguments code from a simpler time when we only supported one build system per platform
            platformName = generatorOriginalName
            if (platformName == PlatformNameString.QNX or
                    platformName == PlatformNameString.YOCTO or
                    platformName == PlatformNameString.UBUNTU or
                    platformName == PlatformNameString.FREERTOS or
                    (external and platformName == PlatformNameString.ANDROID)):
                PlatformBuildUtil.AddMakeThreadArgument(rArgumentList, numBuildThreads)
                return True
            elif platformName == PlatformNameString.WINDOWS:
                PlatformBuildUtil.AddVisualStudioThreadArgument(rArgumentList, numBuildThreads)
                return True
            return False

        # We are doing somekind of cmake based build.
        if cmakeConfig is None:
            raise Exception("buildTypeInfo {0} requires a cmakeConfig to be supplied".format(buildTypeInfo))

        if buildTypeInfo == PlatformBuildTypeInfo.CMake:
            return PlatformBuildUtil.TryAddCMakeThreadArgument(rArgumentList, numBuildThreads, cmakeConfig)
        elif buildTypeInfo == PlatformBuildTypeInfo.CMakeCustom:
            return PlatformBuildUtil.TryAddCustomCMakeThreadArgument(rArgumentList, numBuildThreads, cmakeConfig)
        log.DoPrintWarning("Unknown build type info {0}".format(buildTypeInfo))
        return False

    @staticmethod
    def AddVisualStudioThreadArgument(rArgumentList: List[str], numBuildThreads: int) -> None:
        rArgumentList += ['/maxcpucount:{0}'.format(numBuildThreads)]

    @staticmethod
    def AddMakeThreadArgument(rArgumentList: List[str], numBuildThreads: int) -> None:
        rArgumentList += ['-j', str(numBuildThreads)]

    @staticmethod
    def TryAddCMakeThreadArgument(rArgumentList: List[str], numBuildThreads: int, cmakeConfig: GeneratorCMakeConfig) -> bool:
        if cmakeConfig.CMakeVersion >= CMakeVersion(3, 13, 0):
            # This is a cmake build, so we use the standard cmake parallel command
            rArgumentList += ['--parallel', str(numBuildThreads)]
            return True
        # This is a cmake based build but the cmake command is so old that it does not support '--parallel'
        # FIX: patch the command so it can supply commands that get added after '--' or whatever the build system use
        return False

    @staticmethod
    def TryAddCustomCMakeThreadArgument(rArgumentList: List[str], numBuildThreads: int, cmakeConfig: GeneratorCMakeConfig) -> bool:
        generatorName = cmakeConfig.CMakeFinalGeneratorName
        if CMakeGeneratorName.IsVisualStudio(generatorName):
            PlatformBuildUtil.AddVisualStudioThreadArgument(rArgumentList, numBuildThreads)
            return True
        if generatorName == CMakeGeneratorName.Ninja or generatorName == CMakeGeneratorName.UnixMakeFile:
            PlatformBuildUtil.AddMakeThreadArgument(rArgumentList, numBuildThreads)
            return True
        return False

    @staticmethod
    def GetRecommendedBuildThreads(buildThreads: int) -> int:
        if buildThreads == BuildThreads.Auto:
            return multiprocessing.cpu_count() + 2
        elif buildThreads > 0:
            return buildThreads
        return 1
