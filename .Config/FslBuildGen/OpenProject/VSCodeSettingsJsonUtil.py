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

from typing import Optional
import json
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.OpenProject.OpenProjectCMakeInfo import OpenProjectCMakeInfo

# See https://vector-of-bool.github.io/docs/vscode-cmake-tools/settings.html

class VSCodeSettingsJsonUtil(object):

    @staticmethod
    def Patch(log: Log, jsonFilePath: str, cmakeInfo: OpenProjectCMakeInfo) -> None:
        strJson = IOUtil.TryReadFile(jsonFilePath)
        if strJson is not None:
            jsonDict = json.loads(strJson)
        else:
            jsonDict = {}

        if log.Verbosity >= 1:
            log.LogPrint("- cmake.buildDirectory: '{0}' ".format(cmakeInfo.BuildDirectory))
            log.LogPrint("- cmake.configureArgs: '{0}' ".format(cmakeInfo.ConfigureArgs))
            log.LogPrint("- cmake.sourceDirectory: '{0}' ".format(cmakeInfo.SourceDirectory))
            log.LogPrint("- cmake.generator: '{0}' ".format(cmakeInfo.Generator))
            log.LogPrint("- cmake.installPrefix: '{0}' ".format(cmakeInfo.InstallPrefix))
            if cmakeInfo.BuildThreads is not None:
                log.LogPrint("- cmake.parallelJobs: '{0}' ".format(cmakeInfo.BuildThreads))

        # Running build config command
        # cmake
        # -DCODE_COVERAGE=OFF
        # -DCMAKE_PREFIX_PATH=E:/Work/DFLibs/Windows/VS2019_X64_3_1/glm-0.9.8.5
        # -G "Visual Studio 16 2019"
        # -A x64
        # E:/Work/DemoFramework/build/Windows/VS2019_X64/_fsl_cmake/DF'

        # Specify the build directory (The root directory where CMakeCache.txt will be generated).
        jsonDict["cmake.buildDirectory"] = cmakeInfo.BuildDirectory

        # Directory where the root CMakeLists.txt will be found.
        jsonDict["cmake.sourceDirectory"] = cmakeInfo.SourceDirectory

        # Set to a string to override CMake Tools preferred generator logic. If this is set, CMake will unconditionally use it as the -G CMake generator command line argument.
        jsonDict["cmake.generator"] = cmakeInfo.Generator

        # By specifying a number, you can define how many jobs are run in parallel during the build.
        if cmakeInfo.BuildThreads is not None:
            jsonDict["cmake.parallelJobs"] = cmakeInfo.BuildThreads

        #If specified, sets a value for CMAKE_INSTALL_PREFIX when running CMake configure. If not, no value will be passed
        if len(cmakeInfo.InstallPrefix) > 0:
            jsonDict["cmake.installPrefix"] = cmakeInfo.InstallPrefix

        # Arguments to CMake that will be passed during the configure process.
        # WARNING: Always prefer to use cmake.configureSettings or CMake Variants. Never pass -D arguments using this setting.
        if len(cmakeInfo.ConfigureArgs) > 0:
            jsonDict["cmake.configureArgs"] = cmakeInfo.ConfigureArgs

        # cmake.configureSettings
        # An object containing key : value pairs, which will be passed onto CMake when configuring. It does the same thing as passing -DVAR_NAME=ON via cmake.configureArgs.
        if len(cmakeInfo.ConfigureSettingsDict) > 0:
            jsonDict["cmake.configureSettings"] = cmakeInfo.ConfigureSettingsDict

        # cmake.buildArgs
        # An array of additional arguments to pass to cmake --build.

        # cmake.buildToolArg
        # An array of additional arguments to pass to the underlying build tool.

        jsonText = json.dumps(jsonDict, ensure_ascii=False, sort_keys=True, indent=4)
        IOUtil.WriteFileIfChanged(jsonFilePath, jsonText)
