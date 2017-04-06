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

#import cProfile
#import pstats
import argparse
import sys
from FslBuildGen import IOUtil, PluginConfig, ParseUtil
from FslBuildGen import Util
from FslBuildGen.Config import Config
from FslBuildGen.Build import Builder
from FslBuildGen.Main import *
from FslBuildGen.PackageListUtil import GetTopLevelPackage
from FslBuildGen.PlatformUtil import *
from FslBuildGen.Tool import ToolProgramFlow
from FslBuildGen.Tool.ToolProgramFlowConfig import ProgramFlowConfig


def __GetTitle():
    return 'FslBuildGen V1.4.3 beta'


def ToolMain(funcGetTitle, args, currentDir, toolConfig, userTag):
    args.Variants = ParseUtil.ParseVariantDict(args.Variants)
    typeSdk = 'sdk'

    allowDevelopmentPlugins = True if args.dev else False
    config = Config(toolConfig, args.verbosity, args.type, args.Variants, allowDevelopmentPlugins)

    if args.DryRun:
        config.ForceDisableAllWrite()
    if args.IgnoreNotSupported:
        config.IgnoreNotSupported = True

    config.LogPrint(funcGetTitle())

    if args.graph:
        PluginConfig.EnableGraph();

    theFiles = DoGetFiles(config, currentDir)

    PluginConfig.SetVSVersion(args.VSVersion)
    PluginConfig.SetGeneratorType(args.GenType)

    packages = DoGenerateBuildFiles(config, theFiles, args.platform)

    if args.ListVariants or args.ListBuildVariants:
        if args.ListVariants:
            topLevelPackage = GetTopLevelPackage(packages)
            requestedFiles = None if config.IsSDKBuild else theFiles
            generator = PluginConfig.GetGeneratorPluginById(args.platform)
            Builder.ShowVariantList(topLevelPackage, requestedFiles, generator)
        if args.ListBuildVariants:
            generator = PluginConfig.GetGeneratorPluginById(args.platform)
            Builder.ShowBuildVariantList(generator)


def __ParserAddCustomArguments(parser, userTag):
    parser.add_argument('-t', '--type', default=PluginConfig.TYPE_DEFAULT, choices=[PluginConfig.TYPE_DEFAULT, 'sdk'], help='Select generator type')
    parser.add_argument('--graph', action='store_true',  help='Generate a dependency graph using dot (requires the graphviz dot executable in path)')
    parser.add_argument('--DryRun', action='store_true',  help='No files will be created')
    parser.add_argument('--IgnoreNotSupported', action='store_true',  help='try to build things that are marked as not supported')
    parser.add_argument('--VSVersion', default=VisualStudioVersion.VS2015,  help='Choose a specific visual studio version (2013,2015,2017)')
    parser.add_argument('--GenType', default="default",  help='Chose the generator type to use (%s)' % (", ".join(PluginConfig.GENERATOR_TYPES.keys())))
    parser.add_argument('--ListBuildVariants', action='store_true',  help='List all build-variants supported by build and exit')
    parser.add_argument('--ListVariants', action='store_true',  help='List all variants supported by build and exit')
    parser.add_argument('--Variants',  help='Configure the build-variants you wish to use for the build, these affect the generated build files [ANDROID_ABI=all] this is mostly a android thing')


def Main():
    programFlowConfig = ProgramFlowConfig()
    programFlowConfig.AddPlatformArg = True
    programFlowConfig.AllowPlaformAll = True
    ToolProgramFlow.ProgramMain(__GetTitle, __ParserAddCustomArguments, ToolMain, programFlowConfig)


Main()
#cProfile.run('Main()', 'restats')
#p = pstats.Stats('restats')
#p.strip_dirs().sort_stats(-1).print_stats()
#p.sort_stats('cumulative').print_stats(10)
#p.sort_stats('time').print_stats(10)
#p.sort_stats('tottime').print_stats(100)
