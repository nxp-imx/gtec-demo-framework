#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

from FslBuildGen import IOUtil, PluginConfig, ParseUtil, Util, PackageListUtil
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.Exceptions import *
from FslBuildGen.Main import *
from FslBuildGen.PlatformUtil import *
from FslBuildGen.PackageListUtil import GetTopLevelPackage
from FslBuildGen.Build import Builder
from FslBuildGen.Tool import ToolProgramFlow
from FslBuildGen.Tool.ToolProgramFlowConfig import ProgramFlowConfig
from FslBuildGen.BuildConfig import ScanSourceFiles


def __GetTitle():
    return 'FslBuildCheck V0.4.6 - Basic sdk configuration validation'


def ToolMain(funcGetTitle, args, currentDir, toolConfig, userTag):
    featureList = ParseUtil.ParseFeatureList(args.UseFeatures)

    allowDevelopmentPlugins = True if args.dev else False
    config = Config(toolConfig, args.verbosity, args.type, None, allowDevelopmentPlugins)
    if args.DryRun:
        config.ForceDisableAllWrite()

    config.LogPrint(funcGetTitle())

    # Get the platform and see if its supported
    platform = PluginConfig.GetGeneratorPluginById(args.platform)
    CheckBuildPlatform(platform.Name)

    config.LogPrint("Active platform: %s" % (platform.Name))

    packages = None
    discoverFeatureList = '*' in featureList
    if discoverFeatureList or args.project == None or args.ScanSource:
        if discoverFeatureList:
            config.LogPrint("No features specified, so using package to determine them")
        if args.ScanSource or discoverFeatureList:
            theFiles = DoGetFiles(config, currentDir)
            packages = DoGetPackages(config, theFiles, platform)
            topLevelPackage = GetTopLevelPackage(packages)
            if discoverFeatureList:
                featureList = [entry.Name for entry in topLevelPackage.ResolvedAllUsedFeatures]

    if not args.ScanSource:
        Validate.ValidatePlatform(config, args.platform, featureList)
    if args.ScanSource:
        thirdpartyExceptionDir = IOUtil.ToUnixStylePath(currentDir) if not config.IsSDKBuild else None
        ScanSourceFiles.Scan(config, packages, args.Repair, thirdpartyExceptionDir)



def __ParserAddCustomArguments(parser, userTag):
    parser.add_argument('-u', '--UseFeatures', default="*", required=False, help='The feature list to verify build settings for or * to use the current package settings')
    parser.add_argument('--project', default=None,  help='The name of the project')
    parser.add_argument('-t', '--type', default=PluginConfig.TYPE_DEFAULT, choices=[PluginConfig.TYPE_DEFAULT, 'sdk'], help='Select generator type')
    parser.add_argument('--ScanSource', action='store_true',  help='Scan source and check for common issues. (Disabled the normal build environment configuration check)')
    parser.add_argument('--Repair', action='store_true',  help='If used in combination with --ScanSource the tool will attempt to fix common mistakes, beware this modifies your source files so use it at your own risk!.')
    parser.add_argument('--DryRun', action='store_true',  help='No files will be created')


def Main():
    programFlowConfig = ProgramFlowConfig()
    programFlowConfig.AddPlatformArg = True
    ToolProgramFlow.ProgramMain(__GetTitle, __ParserAddCustomArguments, ToolMain, programFlowConfig)


Main()
