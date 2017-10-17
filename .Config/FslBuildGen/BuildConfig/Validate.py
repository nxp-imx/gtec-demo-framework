#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
import os
from FslBuildGen.Config import Config
from FslBuildGen.Log import Log
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen.SharedGeneration import ToolEnvironmentVariableName

# TODO: update this code to just pull the paths from the package files instead since we now have full access to them
#       Problem: due to performance we dont always scan all the package files :(
#                but maybe we could get away with scanning the third party files or just a select few if no packages were processed

def IsEnvironmentVariableSet(variable: str) -> bool:
    return os.environ.get(variable) is not None


def CheckCommon(log: Log, verbosityLevel: int) -> None:
    log.LogPrintVerbose(verbosityLevel, "- Common SDK environment")
    IOUtil.GetEnvironmentVariableForDirectory(ToolEnvironmentVariableName.FSL_GRAPHICS_SDK)


def CheckWindowsGLESCommon(log: Log) -> None:
    IOUtil.GetEnvironmentVariableForDirectory("FSL_GLES_EMULATOR_PATH")
    IOUtil.GetEnvironmentVariableForDirectory("FSL_GLES_INCLUDE_PATH")
    IOUtil.GetEnvironmentVariableForDirectory("FSL_GLES_LIB_PATH")


def CheckWindowsGLESArm(log: Log) -> None:
    CheckWindowsGLESCommon(log)


def CheckWindowsGLESPowerVR(log: Log) -> None:
    CheckWindowsGLESCommon(log)


def CheckWindowsGLESMesa(log: Log) -> None:
    CheckWindowsGLESCommon(log)


def CheckWindowsGLESQualcomm(log: Log) -> None:
    CheckWindowsGLESCommon(log)


def CheckWindowsGLESVivante(log: Log) -> None:
    CheckWindowsGLESCommon(log)


def CheckWindowsGLES(log: Log) -> None:
    log.LogPrint("- OpenGL ES environment")
    try:
        emulator = os.environ.get("FSL_GLES_NAME")
        if not emulator:
            raise EnvironmentError("OpenGLES emulation 'FSL_GLES_NAME' environment variable is not set.")

        emulator = emulator.lower()
        if(emulator == 'arm' or emulator == 'default'):
            CheckWindowsGLESArm(log)
        elif(emulator == 'mesa'):
            CheckWindowsGLESPowerVR(log)
        elif(emulator == 'powervr'):
            CheckWindowsGLESMesa(log)
        elif(emulator == 'qualcomm'):
            CheckWindowsGLESQualcomm(log)
        elif(emulator == 'vivante'):
            CheckWindowsGLESVivante(log)
        else:
            log.LogPrint("WARNING: Unknown emulator '{0}'".format(emulator))
    except:
        print("ERROR: CheckWindowsGLES failed.\nThis indicates that the OpenGLES emulation environment was not correctly setup and the code wont be able to locate the necessary OpenGLES header files. Possible causes:\n- Are you using the right emulator type: x64 or x86 see documentation.\n- Did you run the required setup scripts?\n- If building from visual studio did you launch the project via .StartProject.bat\n")
        raise




def CheckWindows(log: Log, features: List[str], verbosityLevel: int) -> None:
    log.LogPrintVerbose(verbosityLevel, "Running Windows checks")
    CheckCommon(log, verbosityLevel)

    checkedGLES = False
    for feature in features:
        if not checkedGLES and (feature == "EGL" or feature == 'OpenGLES2' or feature == 'OpenGLES3' or feature == 'OpenGLES3.1' or feature == 'OpenGLES3.2'):
            CheckWindowsGLES(log)
            checkedGLES = True


def CheckUbuntu(log: Log, features: List[str], verbosityLevel: int) -> None:
    log.LogPrintVerbose(verbosityLevel, "Running Ubuntu checks")
    CheckCommon(log, verbosityLevel)

    checkedGLES = False
    for feature in features:
        if not checkedGLES and (feature == "EGL" or feature == 'OpenGLES2' or feature == 'OpenGLES3' or feature == 'OpenGLES3.1') or feature == 'OpenGLES3.2':
            #CheckUbuntuGLES(log)
            checkedGLES = True


def CheckYocto(log: Log, features: List[str], verbosityLevel: int) -> None:
    log.LogPrintVerbose(verbosityLevel, "Running Yocto checks")
    CheckCommon(log, verbosityLevel)

    checkedGLES = False
    for feature in features:
        if not checkedGLES and (feature == "EGL" or feature == 'OpenGLES2' or feature == 'OpenGLES3' or feature == 'OpenGLES3.1'):
            #CheckYoctoGLES(log)
            checkedGLES = True


def ValidatePlatform(log: Log, platformName: str, features: List[str], verbosityLevel: int = 1) -> None:
    if platformName.lower() == PackageConfig.PlatformNameString.WINDOWS.lower():
        CheckWindows(log, features, verbosityLevel)
    elif platformName.lower() == PackageConfig.PlatformNameString.UBUNTU.lower():
        CheckUbuntu(log, features, verbosityLevel)
    elif platformName.lower() == PackageConfig.PlatformNameString.YOCTO.lower():
        CheckYocto(log, features, verbosityLevel)
    else:
        log.LogPrintVerbose(verbosityLevel, "No configuration checks available for this platform")
    log.LogPrintVerbose(verbosityLevel, "Configuration check completed successfully.")
