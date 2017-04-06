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

#import cProfile
#import pstats
import argparse
import sys
from FslBuildGen import IOUtil, PluginConfig, ParseUtil, Util
from FslBuildGen.Config import Config
from FslBuildGen.Main import *
from FslBuildGen.PlatformUtil import *
from FslBuildGen.PackageListUtil import GetTopLevelPackage
from FslBuildGen.Build import Builder
from FslBuildGen.Tool import ToolProgramFlow
from FslBuildGen.Tool.ToolProgramFlowConfig import ProgramFlowConfig


def __GetTitle():
    return 'FslBuild V0.7.3 beta'


def ToolMain(funcGetTitle, args, currentDir, toolConfig, userTag):
    args.UseFeatures = ParseUtil.ParseFeatureList(args.UseFeatures)
    args.Variants = ParseUtil.ParseVariantDict(args.Variants)
    args.RequireFeatures = ParseUtil.ParseFeatureList(args.RequireFeatures)

    enableContentBuilder = ParseUtil.ParseBool(args.ContentBuilder)

    typeSdk = 'sdk'

    allowDevelopmentPlugins = True if args.dev else False
    config = Config(toolConfig, args.verbosity, args.type, args.Variants, allowDevelopmentPlugins)

    if args.DryRun:
        config.ForceDisableAllWrite()
    if args.IgnoreNotSupported:
        config.IgnoreNotSupported = True

    config.LogPrint(funcGetTitle())

    # Get the platform and see if its supported
    platform = PluginConfig.GetGeneratorPluginById(args.platform)
    CheckBuildPlatform(platform.Name)

    config.LogPrint("Active platform: %s" % (platform.Name))

    theFiles = DoGetFiles(config, currentDir)

    PluginConfig.SetVSVersion(args.VSVersion)
    PluginConfig.SetGeneratorType(args.GenType)

    packages = DoGenerateBuildFiles(config, theFiles, args.platform)

    topLevelPackage = GetTopLevelPackage(packages)

    requestedFiles = None if config.IsSDKBuild else theFiles

    # We need the generator to be able to examine its support
    generator = PluginConfig.GetGeneratorPluginById(args.platform)

    if args.ListFeatures or args.ListVariants:
        if args.ListFeatures:
            Builder.ShowFeatureList(topLevelPackage, requestedFiles)
        if args.ListVariants:
            Builder.ShowVariantList(topLevelPackage, requestedFiles, generator)
    else:
        Builder.BuildPackages(config, packages, platform.Name, requestedFiles, args.UseFeatures, args.RequireFeatures, args.Variants, args.RemainingArgs, args.ForAllExe, generator, enableContentBuilder)


def __ParserAddCustomArguments(parser, userTag):
    parser.add_argument('-t', '--type', default=PluginConfig.TYPE_DEFAULT, choices=[PluginConfig.TYPE_DEFAULT, 'sdk'], help='Select generator type')
    parser.add_argument('--VSVersion', default=VisualStudioVersion.VS2015,  help='Choose a specific visual studio version (2013,2015,2017)')
    parser.add_argument('--GenType', default="default",  help='Chose the generator type to use (%s)' % (", ".join(PluginConfig.GENERATOR_TYPES.keys())))
    parser.add_argument('-u', '--UseFeatures', default="*",  help='The list of available features to build for. For example [OpenGLES2,OpenGLES3.1] or * for everything')
    parser.add_argument('-l', '--ListFeatures', action='store_true',  help='List all features supported by build and exit')
    parser.add_argument('-r', '--RequireFeatures', default="*",  help='The list of features that are required for a executable to be build. For example [OpenGLES2] to build all executables that use OpenGLES2.')
    parser.add_argument('--Variants',  help='Configure the variants you wish to use for the build [WindowSystem=X11]')
    parser.add_argument('--ListVariants', action='store_true',  help='List all variants supported by build and exit')
    parser.add_argument('--DryRun', action='store_true',  help='Nothing will be build')
    parser.add_argument('--IgnoreNotSupported', action='store_true',  help='try to build things that are marked as not supported')
    parser.add_argument('--ContentBuilder', default='on',  help='Enable/disable the content builder')
    parser.add_argument('--ForAllExe', default=None,  help='For each executable run the given command. (EXE) = the full path to the executable. (EXE_NAME) = name of the executable. (EXE_PATH) = the executables dir. (PACKAGE_PATH) = full path to package (CONTENT_PATH) = full path to package content directory. *Experimental*')


def Main():
    programFlowConfig = ProgramFlowConfig()
    programFlowConfig.AddPlatformArg = True
    programFlowConfig.ProcessRemainingArgs = True
    programFlowConfig.SupportBuildTime = True
    ToolProgramFlow.ProgramMain(__GetTitle, __ParserAddCustomArguments, ToolMain, programFlowConfig)


Main()
#cProfile.run('Main()', 'restats')
#p = pstats.Stats('restats')
#p.strip_dirs().sort_stats(-1).print_stats()
#p.sort_stats('cumulative').print_stats(10)
#p.sort_stats('time').print_stats(10)
#p.sort_stats('tottime').print_stats(100)
