#!/usr/bin/env python

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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
from FslBuildGen import IOUtil, PlatformUtil, Util
from FslBuildGen.DataTypes import *


class Config(object):
    def __init__(self, toolConfig, verbosity, type, variantsDict, allowDevelopmentPlugins):
        super(Config, self).__init__()
        self.Verbosity = verbosity
        self.IsVerbose = verbosity > 0
        sdkPath = IOUtil.GetEnvironmentVariableForDirectory('FSL_GRAPHICS_SDK')
        sdkPathAndroidProjectDir = IOUtil.GetEnvironmentVariableForAbsolutePath('FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR')

        if toolConfig == None:
            raise Exception("Missing param");

        self.IsTestMode = False
        self.Type = type;
        self.ToolConfig = toolConfig
        self.SDKPath = sdkPath
        self.SDKPathAndroidProjectDir = sdkPathAndroidProjectDir
        #self.SDKConfigPath = IOUtil.Join(sdkPath, ".Config")
        self.SDKConfigTemplatePath = toolConfig.TemplateFolder.ResolvedPath
        self.TemplateImportDirectories = toolConfig.TemplateImportDirectories
        self.IsQuery = True if type == 'query' else False
        self.IsSDKBuild = True if type == 'sdk' or self.IsQuery else False
        self.TestPath = toolConfig.UnitTestPath
        self.DisableWrite = self.IsQuery
        self.DisableQueryWrite = False
        self.SubPackageSupport = SubPackageSupport.Enabled #SubPackageSupport.ExecutableOnly if toolConfig.DefaultPackageLanguage != PackageLanguage.CSharp else SubPackageSupport.Enabled
        self.GroupException = True
        # Variant extension is getting closer to working, so lets enable it
        self.AllowVariantExtension = True
        self.GenFileName = toolConfig.GenFileName

        self.AllowDevelopmentPlugins = allowDevelopmentPlugins

        self.DisableIncludeDirCheck = False
        self.DisableSourceDirCheck = False
        self.IgnoreNotSupported = False
        self.IsDryRun = False
        self.VariantsDict = variantsDict if variantsDict else {}

        if self.IsQuery:
            self.Type = "sdk"

        #if not os.path.isdir(self.SDKConfigPath):
        #    raise EnvironmentError("Config path '%s' does not point to a directory" % (self.SDKConfigPath))
        if not os.path.isdir(self.SDKConfigTemplatePath):
            raise EnvironmentError("Config template path '%s' does not point to a directory" % (self.SDKConfigTemplatePath))

        buildPlatformType = PlatformUtil.DetectBuildPlatformType()
        if buildPlatformType == BuildPlatformType.Windows:
            self.__ResolvedToCurrentOSPathMethod = self.ToDosPath
            self.__ResolvedToCurrentOSPathDirectConversionMethod = self.ToDosPathDirectConversion
        else:
            self.__ResolvedToCurrentOSPathMethod = self.ToBashPath
            self.__ResolvedToCurrentOSPathDirectConversionMethod = self.ToBashPathDirectConversion


    def ForceDisableAllWrite(self):
        self.DisableWrite = True
        self.DisableQueryWrite = True
        self.IsDryRun = True

    def ToPath(self, path):
        return self.ToolConfig.ToPath(path)

    def ToBashPath(self, path):
        return self.ToolConfig.ToBashPath(path)

    def ToBashPathDirectConversion(self, path):
        return self.ToolConfig.ToBashPathDirectConversion(path)

    def ToDosPath(self, path):
        return self.ToolConfig.ToDosPath(path)

    def ToDosPathDirectConversion(self, path):
        return self.ToolConfig.ToDosPathDirectConversion(path)

    def ToCurrentOSPath(self, path):
        """ Resolve the path to how it would look on the current OS """
        return self.__ResolvedToCurrentOSPathMethod(path)

    def ToCurrentOSPathDirectConversion(self, path):
        """ Resolve the path to how it would look on the current OS """
        return self.__ResolvedToCurrentOSPathDirectConversionMethod(path)

    def SetTestMode(self):
        self.IsTestMode = True
        self.DisableIncludeDirCheck = True
        self.DisableSourceDirCheck = True

    def GetBuildDir(self):
        return "build"

    def GetTemplateCopyPath(self, platform):
        return IOUtil.Join(self.SDKConfigTemplatePath, "%s/Copy" % (platform));

    def GetTemplateModifyPath(self, platform):
        return IOUtil.Join(self.SDKConfigTemplatePath, "%s/Modify" % (platform));

    def LogPrint(self, str):
        if self.IsVerbose:
            print(str)
            sys.stdout.flush()

    def DoPrint(self, str):
        print(str)
        sys.stdout.flush()

