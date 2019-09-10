#!/usr/bin/env python3

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

from typing import Dict
from typing import Optional
import datetime
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.DataTypes import SubPackageSupport
from FslBuildGen.Log import Log
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.SharedGeneration import ToolEnvironmentVariableName
from FslBuildGen.ToolConfig import ToolConfig

class BaseConfig(BasicConfig):
    def __init__(self, log: Log, toolConfig: ToolConfig) -> None:
        super().__init__(log)

        if toolConfig is None:
            raise Exception("Missing param")

        sdkPath = IOUtil.GetEnvironmentVariableForDirectory(ToolEnvironmentVariableName.FSL_GRAPHICS_SDK)  # type: str
        sdkPathAndroidProjectDir = IOUtil.GetEnvironmentVariableForAbsolutePath(ToolEnvironmentVariableName.FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR)  # type: str
        dateNow = datetime.datetime.now()
        self.CurrentYearString = "{0}".format(dateNow.year)  # type: str

        self.SDKPath = sdkPath  # type: str
        self.SDKPathAndroidProjectDir = sdkPathAndroidProjectDir  # type: str
        self.SDKConfigTemplatePath = toolConfig.TemplateFolder.ResolvedPath
        self.TemplateImportDirectories = toolConfig.TemplateImportDirectories
        self.ToolConfig = toolConfig  # type: ToolConfig

        if not IOUtil.IsDirectory(self.SDKConfigTemplatePath):
            raise EnvironmentError("Config template path '{0}' does not point to a directory".format(self.SDKConfigTemplatePath))


class Config(BaseConfig):
    def __init__(self, log: Log,
                 toolConfig: ToolConfig, srcType: str,
                 variantsDict: Optional[Dict[str, str]], allowDevelopmentPlugins: bool) -> None:
        super().__init__(log, toolConfig)


        self.IsTestMode = False
        self.Type = srcType
        #self.SDKConfigPath = IOUtil.Join(sdkPath, ".Config")
        self.IsQuery = True if srcType == 'query' else False  # type: bool
        self.IsSDKBuild = True if srcType == 'sdk' or self.IsQuery else False  # type: bool
        self.TestPath = toolConfig.UnitTestPath
        self.DisableWrite = self.IsQuery
        self.DisableQueryWrite = False  # type: bool
        self.SubPackageSupport = SubPackageSupport.Enabled #SubPackageSupport.ExecutableOnly if toolConfig.DefaultPackageLanguage != PackageLanguage.CSharp else SubPackageSupport.Enabled
        self.GroupException = True  # type: bool
        # Variant extension is getting closer to working, so lets enable it
        self.AllowVariantExtension = True  # type: bool
        self.GenFileName = toolConfig.GenFileName

        self.AllowDevelopmentPlugins = allowDevelopmentPlugins

        self.DisableIncludeDirCheck = False  # type: bool
        self.DisableSourceDirCheck = False  # type: bool
        self.IgnoreNotSupported = False  # type: bool
        self.IsDryRun = False  # type: bool
        self.VariantsDict = variantsDict if variantsDict else {}

        if self.IsQuery:
            self.Type = "sdk"

        #if not os.path.isdir(self.SDKConfigPath):
        #    raise EnvironmentError("Config path '%s' does not point to a directory" % (self.SDKConfigPath))

        buildPlatformType = PlatformUtil.DetectBuildPlatformType()
        if buildPlatformType == BuildPlatformType.Windows:
            self.__ResolvedLegacyToCurrentOSPathMethod = self.TryLegacyToDosPath
            self.__ResolvedLegacyToCurrentOSPathDirectConversionMethod = self.TryLegacyToDosPathDirectConversion
            self.__ResolvedToCurrentOSPathMethod = self.ToDosPath
            self.__ResolvedToCurrentOSPathDirectConversionMethod = self.ToDosPathDirectConversion
        else:
            self.__ResolvedLegacyToCurrentOSPathMethod = self.TryLegacyToBashPath
            self.__ResolvedLegacyToCurrentOSPathDirectConversionMethod = self.TryLegacyToBashPathDirectConversion
            self.__ResolvedToCurrentOSPathMethod = self.ToBashPath
            self.__ResolvedToCurrentOSPathDirectConversionMethod = self.ToBashPathDirectConversion


    def ForceDisableAllWrite(self) -> None:
        self.DisableWrite = True
        self.DisableQueryWrite = True
        self.IsDryRun = True


    def TryToPath(self, path: Optional[str]) -> Optional[str]:
        return self.ToolConfig.TryToPath(path)

    def ToPath(self, path: str) -> str:
        return self.ToolConfig.ToPath(path)


    def TryLegacyToBashPath(self, path: Optional[str]) -> Optional[str]:
        return self.ToolConfig.TryLegacyToBashPath(path)


    def ToBashPath(self, path: str) -> str:
        return self.ToolConfig.ToBashPath(path)


    def TryLegacyToBashPathDirectConversion(self, path: Optional[str]) -> Optional[str]:
        return self.ToolConfig.TryLegacyToBashPathDirectConversion(path)


    def ToBashPathDirectConversion(self, path: str) -> str:
        return self.ToolConfig.ToBashPathDirectConversion(path)


    def TryLegacyToDosPath(self, path: Optional[str]) -> Optional[str]:
        return self.ToolConfig.TryLegacyToDosPath(path)


    def ToDosPath(self, path: str) -> str:
        return self.ToolConfig.ToDosPath(path)


    def TryLegacyToDosPathDirectConversion(self, path: Optional[str]) -> Optional[str]:
        return self.ToolConfig.TryLegacyToDosPathDirectConversion(path)


    def ToDosPathDirectConversion(self, path: str) -> str:
        return self.ToolConfig.ToDosPathDirectConversion(path)


    def TryLegacyToCurrentOSPath(self, path: Optional[str]) -> Optional[str]:
        """ Resolve the path to how it would look on the current OS """
        return self.__ResolvedLegacyToCurrentOSPathMethod(path)


    def ToCurrentOSPathDirectConversion(self, path: str) -> str:
        """ Resolve the path to how it would look on the current OS """
        return self.__ResolvedToCurrentOSPathDirectConversionMethod(path)

    def TryToCurrentOSPathDirectConversion(self, path: Optional[str]) -> Optional[str]:
        """ Resolve the path to how it would look on the current OS """
        return self.__ResolvedLegacyToCurrentOSPathDirectConversionMethod(path)


    def SetTestMode(self) -> None:
        self.IsTestMode = True
        self.DisableIncludeDirCheck = True
        self.DisableSourceDirCheck = True


    def GetBuildDir(self) -> str:
        return "build"
