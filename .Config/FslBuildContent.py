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
from FslBuildGen.Build import Builder
from FslBuildGen.Tool import ToolProgramFlow
from FslBuildGen.Tool.ToolProgramFlowConfig import ProgramFlowConfig
from FslBuildGen.BuildContent.SharedValues import CONFIG_FSLBUILDCONTENT_ENABLED
from FslBuildGen.PackageListUtil import GetTopLevelPackage


def __GetTitle():
    return 'FslBuildContent V0.4.1 - Build pipeline for content'


def ToolMain(funcGetTitle, args, currentDir, toolConfig, userTag):
    # Check if a environment variable has been set to disable this tool
    # This is for example done by FslBuild to prevent multiple executions of content building.
    toolEnabled = IOUtil.TryGetEnvironmentVariable(CONFIG_FSLBUILDCONTENT_ENABLED)

    featureList = ParseUtil.ParseFeatureList(args.UseFeatures)

    allowDevelopmentPlugins = True if args.dev else False
    config = Config(toolConfig, args.verbosity, PluginConfig.TYPE_DEFAULT, None, allowDevelopmentPlugins)

    config.LogPrint(funcGetTitle())

    # Get the platform and see if its supported
    platform = PluginConfig.GetGeneratorPluginById(args.platform)
    CheckBuildPlatform(platform.Name)

    config.LogPrint("Active platform: %s" % (platform.Name))

    discoverFeatureList = '*' in featureList
    if discoverFeatureList or args.project == None:
        if discoverFeatureList:
            config.LogPrint("No features specified, so using package to determine them")
        theFiles = DoGetFiles(config, currentDir)
        packages = DoGetPackages(config, theFiles, platform)
        topLevelPackage = GetTopLevelPackage(packages)
        if discoverFeatureList:
            featureList = [entry.Name for entry in topLevelPackage.ResolvedAllUsedFeatures]
        #if args.project == None:
        #    executeablePackage = PackageListUtil.GetExecutablePackage(packages)
        #    args.project = executeablePackage.ShortName

    if args.Validate:
        Validate.ValidatePlatform(config, args.platform, featureList)

    if toolEnabled != None and not ParseUtil.ParseBool(toolEnabled):
        if args.verbosity > 0:
            print("FslBuildContent has been disabled by environment variable %s set to %s" % (CONFIG_FSLBUILDCONTENT_ENABLED, toolEnabled))
        return

    ContentBuilder.Build(config, currentDir, featureList)


def __ParserAddCustomArguments(parser, userTag):
    parser.add_argument('-u', '--UseFeatures', default="*",  help='Dont use this unless you know what you are doing. The list of available features to build for. For example [OpenGLES2,OpenGLES3.1] or * for everything the package needs.')
    parser.add_argument('--project', default=None,  help='The name of the project')
    parser.add_argument('--Validate', action='store_true',  help='Do build config validation, like running FslBuildCheck')



def Main():
    programFlowConfig = ProgramFlowConfig()
    programFlowConfig.AddPlatformArg = True
    ToolProgramFlow.ProgramMain(__GetTitle, __ParserAddCustomArguments, ToolMain, programFlowConfig)


Main()
