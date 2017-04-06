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
import os.path
import shutil
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *

class PackageDepByLocation(object):
    def __init__(self, location):
        super(PackageDepByLocation, self).__init__()
        self.Location = location;
        self.Packages = []

class AndroidSDKVersion(object):
    def __init__(self, version):
        super(AndroidSDKVersion, self).__init__()
        self.Version = version
        self.VersionString = "%s" % version
        self.AppPlatform = "android-%s" % (version)


class FileName(object):
    def __init__(self, filename, srcPath):
        super(FileName, self).__init__()
        self.AbsoluteSourcePath = IOUtil.Join(srcPath, filename)
        if not filename.endswith(".__template__"):
            self.RelativeDestPath = filename
        else:
            self.RelativeDestPath = filename[:-len(".__template__")]


def GetTargetSDKVersion(package):
    return AndroidSDKVersion(24)


def DetermineMinSDKVersion(package):
    namesOnly = [entry.Name for entry in package.ResolvedAllUsedFeatures]
    if "Vulkan" in namesOnly:
        return AndroidSDKVersion(24)
    elif "OpenGLES3.1" in namesOnly:
        return AndroidSDKVersion(21)
    else:
        return AndroidSDKVersion(18)


def DetermineMinGLESVersion(package):
    namesOnly = [entry.Name for entry in package.ResolvedAllUsedFeatures]
    if "OpenGLES3.1" in namesOnly:
        return "0x00030001"
    elif "OpenGLES3" in namesOnly:
        return "0x00030000"
    else:
        return "0x00020000"


class AppPackageTemplateInfo(object):
    def __init__(self, package):
        super(AppPackageTemplateInfo, self).__init__()
        self.ProjectPathName = package.Name.replace('.', '/');
        self.PrefixedProjectName = package.Name.replace('.', '_');
        self.TargetSDKVersion = GetTargetSDKVersion(package)
        self.MinSDKVersion = DetermineMinSDKVersion(package)
        self.MinGLESVersion = DetermineMinGLESVersion(package)

    def UpdateFileName(self, fileName):
        fileName = fileName.replace("##PREFIXED_PROJECT_NAME##", self.PrefixedProjectName)
        fileName = fileName.replace("##PREFIXED_PROJECT_NAME_L##", self.PrefixedProjectName.lower())
        return fileName


class AppTemplateManager(object):
    def __init__(self, config, templateName):
        super(AppTemplateManager, self).__init__()

        self.FilesToCopy = self.__GetTemplateFiles(config, config.GetTemplateCopyPath(templateName))
        self.FilesToModify = self.__GetTemplateFiles(config, config.GetTemplateModifyPath(templateName))

    def CreateTemplate(self, dstPath, package):
        # create folder structure
        IOUtil.SafeMakeDirs(dstPath)
        # do the actual copy
        self.__CopyFiles(dstPath, self.FilesToCopy)


    def __CopyFiles(self, dstPath, filesToCopy):
        for file in filesToCopy:
            dst = IOUtil.Join(dstPath, file.RelativeDestPath)
            dirName = IOUtil.GetDirectoryName(dst)
            IOUtil.SafeMakeDirs(dirName)
            IOUtil.CopySmallFile(file.AbsoluteSourcePath, dst)

    def __GetTemplateFiles(self, config, copyFrom):
        skip = len(copyFrom)+1
        files = []
        for file in IOUtil.GetFilePaths(copyFrom, None):
            files.append(FileName(file[skip:], copyFrom))
        return files
