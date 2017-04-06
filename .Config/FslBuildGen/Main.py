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

import copy
import os
import os.path
from FslBuildGen import IOUtil, MakeFileHelper, PackageConfig, PluginConfig, Util
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.PackageLoader import PackageLoader
from FslBuildGen.XmlStuff import XmlGenFile
from FslBuildGen.XmlProjectRootConfigFile import XmlProjectRootConfigFile
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.Generator import GenerateQuery
from FslBuildGen.PackageResolver import PackageResolver

def DoGetFiles(config, currentDir):
    theFiles = []
    if not config.IsSDKBuild:
        theFile = IOUtil.Join(currentDir, config.GenFileName)
        if not os.path.isfile(theFile):
            raise Exception("File not found: '%s'" % (theFile))
        theFiles.append(theFile)
    return theFiles


def DoGetPackages(config, testFiles, platform):
    packageLoader = PackageLoader(config, testFiles, platform)
    packageResolver = PackageResolver(config, platform.Name, packageLoader.GenFiles, False)
    return packageResolver.Packages


def __GeneratePlatformBuild(config, testFiles, platform):
    packageLoader = PackageLoader(config, testFiles, platform)
    res = None
    found = False
    if platform.Id == PluginConfig.PLATFORM_ID_ALL:
        res = {}
        for entry in PluginConfig.GetGeneratorPlugins(config.AllowDevelopmentPlugins):
            packages = entry.Generate(config, copy.deepcopy(packageLoader))
            if config.IsTestMode:
                res[entry.Name] = packages
        found = True
    else:
        res = platform.Generate(config, copy.deepcopy(packageLoader))
        found = True

    if not found:
        raise UsageErrorException("Unknown platform '%s'" % platformName)
    return res


def DoGenerateBuildFiles(config, files, platformId): 
    platform = PluginConfig.GetGeneratorPluginById(platformId)
    config.LogPrint("- Generating build files")
    return __GeneratePlatformBuild(config, files, platform)


def ToUnitTestPath(config, path):
    return IOUtil.Join(config.TestPath, path)


def ToUnitTestPaths(config, paths):
    res = []
    for path in paths:
        res.append(ToUnitTestPath(config, path))
    return res


def GetDefaultConfigForTest(enableTestMode=False):
    toolConfigPath = IOUtil.GetEnvironmentVariableForDirectory("FSL_GRAPHICS_INTERNAL")
    toolConfigPath = IOUtil.Join(toolConfigPath, "Tools/FslBuildGen/FslBuildGen/FslBuildGen.xml")
    projectRootConfig = XmlProjectRootConfigFile(None)
    toolConfig = ToolConfig(toolConfigPath, projectRootConfig)
    config = Config(toolConfig, False, PluginConfig.TYPE_UNIT_TEST, None, True)
    config.ForceDisableAllWrite()
    if enableTestMode:
        config.SetTestMode()
    return config


def SimpleTestHookOneFile(theFile):
    config = GetDefaultConfigForTest()
    theFile = ToUnitTestPath(config, theFile)
    return DoGenerateBuildFiles(config, [theFile], PluginConfig.PLATFORM_ID_ALL)


def SimpleTestHookFiles(theFiles):
    config = GetDefaultConfigForTest()
    theFiles = ToUnitTestPaths(config, theFiles)
    return DoGenerateBuildFiles(config, theFiles, PluginConfig.PLATFORM_ID_ALL)


def SimpleTestHookOneFileEx(theFile, config):
    config.ForceDisableAllWrite()
    theFile = ToUnitTestPath(config, theFile)
    return DoGenerateBuildFiles(config, [theFile], PluginConfig.PLATFORM_ID_ALL)


def SimpleTestHookFilesEx(theFiles, config):
    config.ForceDisableAllWrite()
    theFiles = ToUnitTestPaths(config, theFiles)
    return DoGenerateBuildFiles(config, theFiles, PluginConfig.PLATFORM_ID_ALL)
