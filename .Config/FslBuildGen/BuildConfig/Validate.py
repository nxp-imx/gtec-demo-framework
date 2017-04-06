#!/usr/bin/env python

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

import os
import sys
from FslBuildGen import IOUtil, PackageConfig

# TODO: update this code to just pull the paths from the package files instead since we now have full access to them
#       Problem: due to performance we dont always scan all the package files :(
#                but maybe we could get away with scanning the third party files or just a select few if no packages were processed

def IsEnvironmentVariableSet(variable):
    return os.environ.get(variable) != None


def DoesPathEndsWithSlash(path):
    return path.endswith("/") or path.endswith("\\")


def ValidateEnvironmentVariableAsDirectory(name):
    envValue = os.environ.get(name);
    if envValue == None:
        raise EnvironmentError("%s environment variable is not set" % (name));

    if DoesPathEndsWithSlash(envValue):
        raise EnvironmentError("%s environment variable path can not end with a slash or backslash" % (name));

    if not IOUtil.IsDirectory(envValue):
        raise EnvironmentError("Path '%s' pointed to by environment variable %s is not a directory" % (envValue, name));


def CheckCommon(config):
    config.LogPrint("- Common SDK environment");
    ValidateEnvironmentVariableAsDirectory("FSL_GRAPHICS_SDK")


def CheckWindowsGLESCommon(config):
    ValidateEnvironmentVariableAsDirectory("FSL_GLES_EMULATOR_PATH")
    ValidateEnvironmentVariableAsDirectory("FSL_GLES_INCLUDE_PATH")
    ValidateEnvironmentVariableAsDirectory("FSL_GLES_LIB_PATH")


def CheckWindowsGLESArm(config):
    CheckWindowsGLESCommon(config)


def CheckWindowsGLESPowerVR(config):
    CheckWindowsGLESCommon(config)


def CheckWindowsGLESMesa(config):
    CheckWindowsGLESCommon(config)


def CheckWindowsGLESQualcomm(config):
    CheckWindowsGLESCommon(config)


def CheckWindowsGLESVivante(config):
    CheckWindowsGLESCommon(config)


def CheckWindowsGLES(config):
    config.LogPrint("- OpenGL ES environment");
    try:
        emulator = os.environ.get("FSL_GLES_NAME");
        if not emulator:
            raise EnvironmentError("OpenGLES emulation 'FSL_GLES_NAME' environment variable is not set.");

        emulator = emulator.lower();
        if( emulator == 'arm' or emulator == 'default' ):
            CheckWindowsGLESArm(config)
        elif( emulator == 'mesa' ):
            CheckWindowsGLESPowerVR(config)
        elif( emulator == 'powervr' ):
            CheckWindowsGLESMesa(config)
        elif( emulator == 'qualcomm' ):
            CheckWindowsGLESQualcomm()
        elif( emulator == 'vivante' ):
            CheckWindowsGLESVivante(config)
        else:
            config.LogPrint("WARNING: Unknown emulator '{0}'".format(emulator));
    except:
        print("ERROR: CheckWindowsGLES failed.\nThis indicates that the OpenGLES emulation environment was not correctly setup and the code wont be able to locate the necessary OpenGLES header files. Possible causes:\n- Are you using the right emulator type: x64 or x86 see documentation.\n- Did you run the required setup scripts?\n- If building from visual studio did you launch the project via .StartProject.bat\n")
        raise

def CheckWindowsOpenCV2_4(config):
    config.LogPrint("- OpenCV 2.4 environment");
    name = "OPENCV2_4_DIR"
    rootPath = os.environ.get(name);
    if not rootPath:
        raise EnvironmentError("OpenCV '%s' environment variable is not set." % (name));
    if not IOUtil.IsDirectory(rootPath):
        raise EnvironmentError("Path '%s' pointed to by environment variable %s is not a directory" % (rootPath, name));
    rootPath = IOUtil.NormalizePath(rootPath)
    includePath = IOUtil.Join(rootPath, 'include')
    libPath = IOUtil.Join(rootPath, 'x86/vc12/lib')

    if not IOUtil.IsDirectory(includePath):
        raise EnvironmentError("Include path '%s' does not exist" % (includePath));
    if not IOUtil.IsDirectory(libPath):
        raise EnvironmentError("Lib path '%s' does not exist" % (libPath));

    # apply additional knowledge
    includePathHeaders = IOUtil.Join(includePath, 'opencv')
    if not IOUtil.IsDirectory(includePathHeaders):
        raise EnvironmentError("Include path '%s' does not exist" % (includePathHeaders));
    includePathHeaders = IOUtil.Join(includePath, 'opencv2')
    if not IOUtil.IsDirectory(includePathHeaders):
        raise EnvironmentError("Include path '%s' does not exist" % (includePathHeaders));


def CheckWindowsOpenCV3_1(config):
    config.LogPrint("- OpenCV 3.1 environment");
    name = "OPENCV_DIR"
    rootPath = os.environ.get(name);
    if not rootPath:
        raise EnvironmentError("OpenCV '%s' environment variable is not set." % (name));
    if not IOUtil.IsDirectory(rootPath):
        raise EnvironmentError("Path '%s' pointed to by environment variable %s is not a directory" % (rootPath, name));
    rootPath = IOUtil.NormalizePath(rootPath)
    includePath = IOUtil.Join(rootPath, 'include')
    libPath = IOUtil.Join(rootPath, 'x64/vc14/lib')

    if not IOUtil.IsDirectory(includePath):
        raise EnvironmentError("Include path '%s' does not exist" % (includePath));
    if not IOUtil.IsDirectory(libPath):
        raise EnvironmentError("Lib path '%s' does not exist" % (libPath));

    # apply additional knowledge
    includePathHeaders = IOUtil.Join(includePath, 'opencv')
    if not IOUtil.IsDirectory(includePathHeaders):
        raise EnvironmentError("Include path '%s' does not exist" % (includePathHeaders));
    includePathHeaders = IOUtil.Join(includePath, 'opencv2')
    if not IOUtil.IsDirectory(includePathHeaders):
        raise EnvironmentError("Include path '%s' does not exist" % (includePathHeaders));

    # Scan path for DLL files
    dllFiles = [ "opencv_world310.dll", "opencv_world310d.dll" ]
    for dllFile in dllFiles:
        if IOUtil.FindFileInPath(dllFile) == None:
            raise EnvironmentError("{0} is not present in the path. Are you sure that the {1} environment variable was configured before you ran prepare.bat".format(dllFile, name));


def CheckWindowsOpenCL(config):
    config.LogPrint("- OpenCL environment");
    name = "AMDAPPSDKROOT"
    rootPath = os.environ.get(name);
    if not rootPath:
        raise EnvironmentError("Amd OpenCL '%s' environment variable is not set." % (name));
    if not IOUtil.IsDirectory(rootPath):
        raise EnvironmentError("Path '%s' pointed to by environment variable %s is not a directory" % (rootPath, name));
    rootPath = IOUtil.NormalizePath(rootPath)
    includePath = IOUtil.Join(rootPath, 'include')
    libPath = IOUtil.Join(rootPath, 'lib/x86')

    if not IOUtil.IsDirectory(includePath):
        raise EnvironmentError("Include path '%s' does not exist" % (includePath));
    if not IOUtil.IsDirectory(libPath):
        raise EnvironmentError("Lib path '%s' does not exist" % (libPath));

    # apply additional knowledge
    includePathHeaders = IOUtil.Join(includePath, 'CL')
    mainIncludeFile = IOUtil.Join(includePathHeaders, 'cl.h')
    if not IOUtil.IsDirectory(includePathHeaders):
        raise EnvironmentError("Include path '%s' does not exist" % (includePathHeaders));
    if not IOUtil.IsFile(mainIncludeFile):
        raise EnvironmentError("Main include file does not exist '%s'" % (mainIncludeFile));



def CheckWindowsOpenVX(config):
    config.LogPrint("- OpenVX environment");
    #This check is no longer needed as we ship a precompiled version
    pass
#    name = "FSL_AMD_OPENVX_PATH"
#    rootPath = os.environ.get(name);
#    if not rootPath:
#        raise EnvironmentError("OpenVX '%s' environment variable is not set." % (name));
#    if not IOUtil.IsDirectory(rootPath):
#        raise EnvironmentError("Path '%s' pointed to by environment variable %s is not a directory" % (rootPath, name));
#    rootPath = IOUtil.NormalizePath(rootPath)
#    includePath = IOUtil.Join(rootPath, 'include')
#    libPath = IOUtil.Join(rootPath, 'Release')
#
#    if not IOUtil.IsDirectory(includePath):
#        raise EnvironmentError("Include path '%s' does not exist" % (includePath));
#    if not IOUtil.IsDirectory(libPath):
#        raise EnvironmentError("Lib path '%s' does not exist" % (libPath));
#
#    # apply additional knowledge
#    includePathHeaders = IOUtil.Join(includePath, 'VX')
#    mainIncludeFile = IOUtil.Join(includePathHeaders, 'vx.h')
#    if not IOUtil.IsDirectory(includePathHeaders):
#        raise EnvironmentError("Include path '%s' does not exist" % (includePathHeaders));
#    if not IOUtil.IsFile(mainIncludeFile):
#        raise EnvironmentError("Main include file does not exist '%s'" % (mainIncludeFile));


def CheckVulkan(config):
    config.LogPrint("- Vulkan environment");
    name = "VULKAN_SDK"
    rootPath = os.environ.get(name);
    if not rootPath:
        raise EnvironmentError("Vulkan sdk '%s' environment variable is not set." % (name));
    if not IOUtil.IsDirectory(rootPath):
        raise EnvironmentError("Path '%s' pointed to by environment variable %s is not a directory" % (rootPath, name));
    rootPath = IOUtil.NormalizePath(rootPath)
    includePath = IOUtil.Join(rootPath, 'include')
    if not IOUtil.IsDirectory(includePath):
        raise EnvironmentError("Include path '%s' does not exist" % (includePath));


def CheckWindowsVulkan(config):
    CheckVulkan(config)
    ValidateEnvironmentVariableAsDirectory("VK_SDK_PATH")


def CheckUbuntuVulkan(config):
    CheckVulkan(config)


def CheckYoctoVulkan(config):
    #CheckVulkan(config)
    pass


def CheckWindows(config, features):
    config.LogPrint("Running Windows checks");
    CheckCommon(config)

    checkedGLES = False
    checkedG2D = False
    checkedOpenVG = False
    checkedOpenCV2_4 = False
    checkedOpenCV3_1 = False
    checkedOpenCL = False
    checkedOpenVX = False
    checkedVulkan = False
    for feature in features:
        if not checkedGLES and (feature == "EGL" or feature == 'OpenGLES2' or feature == 'OpenGLES3' or feature == 'OpenGLES3.1'):
            CheckWindowsGLES(config)
            checkedGLES = True
        elif not checkedG2D and feature == 'G2D':
            checkedG2D = True
        elif not checkedOpenVG and feature == 'OpenVG':
            checkedOpenVG = True
        elif not checkedOpenCV2_4 and feature == 'OpenCV2.4':
            CheckWindowsOpenCV2_4(config)
            checkedOpenCV2_4 = True
        elif not checkedOpenCV3_1 and feature == 'OpenCV3.1':
            CheckWindowsOpenCV3_1(config)
            checkedOpenCV3_0 = True
        elif not checkedOpenCL and (feature == 'OpenCL' or feature == 'OpenCL1.1' or feature == 'OpenCL1.2'):
            CheckWindowsOpenCL(config)
            checkedOpenCL = True
        elif not checkedOpenVX and feature == 'OpenVX':
            CheckWindowsOpenVX(config)
            checkedOpenVX = True
        elif not checkedVulkan and feature == 'Vulkan':
            CheckWindowsVulkan(config)
            checkedVulkan = True


def CheckUbuntu(config, features):
    config.LogPrint("Running Ubuntu checks");
    CheckCommon(config)

    checkedGLES = False
    checkedG2D = False
    checkedOpenVG = False
    checkedOpenCV2_4 = False
    checkedOpenCV3_1 = False
    checkedOpenCL = False
    checkedOpenVX = False
    checkedVulkan = False
    for feature in features:
        if not checkedGLES and (feature == "EGL" or feature == 'OpenGLES2' or feature == 'OpenGLES3' or feature == 'OpenGLES3.1'):
            #CheckUbuntuGLES(config)
            checkedGLES = True
        elif not checkedG2D and feature == 'G2D':
            checkedG2D = True
        elif not checkedOpenVG and feature == 'OpenVG':
            checkedOpenVG = True
        elif not checkedOpenCV2_4 and feature == 'OpenCV2.4':
            #CheckUbuntuOpenCV2_4(config)
            checkedOpenCV2_4 = True
        elif not checkedOpenCV3_1 and feature == 'OpenCV3.1':
            #CheckUbuntuOpenCV3_1(config)
            checkedOpenCV3_0 = True
        elif not checkedOpenCL and (feature == 'OpenCL' or feature == 'OpenCL1.1' or feature == 'OpenCL1.2'):
            #CheckUbuntuOpenCL(config)
            checkedOpenCL = True
        elif not checkedOpenVX and feature == 'OpenVX':
            #CheckUbuntuOpenVX(config)
            checkedOpenVX = True
        elif not checkedVulkan and feature == 'Vulkan':
            CheckUbuntuVulkan(config)
            checkedVulkan = True


def CheckYocto(config, features):
    config.LogPrint("Running Yocto checks");
    CheckCommon(config)

    checkedGLES = False
    checkedG2D = False
    checkedOpenVG = False
    checkedOpenCV2_4 = False
    checkedOpenCV3_1 = False
    checkedOpenCV3_2 = False
    checkedOpenCL = False
    checkedOpenVX = False
    checkedVulkan = False
    for feature in features:
        if not checkedGLES and (feature == "EGL" or feature == 'OpenGLES2' or feature == 'OpenGLES3' or feature == 'OpenGLES3.1'):
            #CheckYoctoGLES(config)
            checkedGLES = True
        elif not checkedG2D and feature == 'G2D':
            checkedG2D = True
        elif not checkedOpenVG and feature == 'OpenVG':
            checkedOpenVG = True
        elif not checkedOpenCV2_4 and feature == 'OpenCV2.4':
            #CheckYoctoOpenCV2_4(config)
            checkedOpenCV2_4 = True
        elif not checkedOpenCV3_1 and feature == 'OpenCV3.1':
            #CheckYoctoOpenCV3_1(config)
            checkedOpenCV3_0 = True
        elif not checkedOpenCV3_2 and feature == 'OpenCV3.2':
            #CheckYoctoOpenCV3_2(config)
            checkedOpenCV3_2= True
        elif not checkedOpenCL and (feature == 'OpenCL' or feature == 'OpenCL1.1' or feature == 'OpenCL1.2'):
            #CheckYoctoOpenCL(config)
            checkedOpenCL = True
        elif not checkedOpenVX and feature == 'OpenVX':
            #CheckYoctoOpenVX(config)
            checkedOpenVX = True
        elif not checkedVulkan and feature == 'Vulkan':
            CheckYoctoVulkan(config)
            checkedVulkan = True


def ValidatePlatform(config, platform, features):
    if platform.lower() == PackageConfig.PLATFORM_WINDOWS.lower():
        CheckWindows(config, features)
    elif platform.lower() == PackageConfig.PLATFORM_UBUNTU.lower():
        CheckUbuntu(config, features)
    elif platform.lower() == PackageConfig.PLATFORM_YOCTO.lower():
        CheckYocto(config, features)
    else:
       config.LogPrint("No configuration checks available for this platform");
    config.LogPrint("Configuration check completed successfully.");

