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
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.Packages.Package import Package
from FslBuildGen.ToolConfig import ToolConfigPackageLocation


class PackageDepByLocation(object):
    def __init__(self, location: ToolConfigPackageLocation) -> None:
        super().__init__()
        self.Location = location
        self.Packages = []  # type: List[Package]


class AndroidSDKVersion(object):
    def __init__(self, version: int) -> None:
        super().__init__()
        self.Version = version
        self.VersionString = "{0}".format(version)  # type: str
        self.AppPlatform = "android-{0}".format(version)  # type: str


def GetTargetSDKVersion(package: Package) -> AndroidSDKVersion:
    return AndroidSDKVersion(AndroidUtil.GetTargetSDKVersion())


def DetermineMinSDKVersion(package: Package) -> AndroidSDKVersion:
    namesOnly = [entry.Name for entry in package.ResolvedAllUsedFeatures]
    if "Vulkan" in namesOnly:
        return AndroidSDKVersion(max(24, AndroidUtil.GetMinimumSDKVersion()))
    elif "OpenGLES3.1" in namesOnly:
        return AndroidSDKVersion(AndroidUtil.GetMinimumSDKVersion())
    return AndroidSDKVersion(AndroidUtil.GetMinimumSDKVersion())


def DetermineMinGLESVersion(package: Package) -> str:
    namesOnly = [entry.Name for entry in package.ResolvedAllUsedFeatures]
    if "OpenGLES3.2" in namesOnly:
        return "0x00030002"
    elif "OpenGLES3.1" in namesOnly:
        return "0x00030001"
    elif "OpenGLES3" in namesOnly:
        return "0x00030000"
    return "0x00020000"


class AppPackageTemplateInfo(object):
    def __init__(self, package: Package) -> None:
        super().__init__()
        self.ProjectPathName = package.Name.replace('.', '/')
        self.PrefixedProjectName = package.Name.replace('.', '_')
        self.TargetSDKVersion = GetTargetSDKVersion(package)
        self.MinSDKVersion = DetermineMinSDKVersion(package)
        self.MinGLESVersion = DetermineMinGLESVersion(package)


    def UpdateFileName(self, fileName: str) -> str:
        fileName = fileName.replace("##PREFIXED_PROJECT_NAME##", self.PrefixedProjectName)
        fileName = fileName.replace("##PREFIXED_PROJECT_NAME_L##", self.PrefixedProjectName.lower())
        return fileName
