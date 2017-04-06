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

import argparse
import os
import sys
import time
from datetime import timedelta
from FslBuildGen import IOUtil, PluginConfig, ParseUtil, Util
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.Exceptions import *
from FslBuildGen.Main import *
from FslBuildGen.PlatformUtil import *
from FslBuildGen.Build import Builder
from FslBuildGen.Tool.ToolProgramFlowConfig import ProgramFlowConfig
from FslBuildGen.PluginConfig import SetForceUseNativeGenerator
from FslBuildGen.XmlProjectRootConfigFile import XmlProjectRootConfigFile


__g_verbosityLevel = 0
__g_debugEnabled = False
__g_allowDevelopmentPlugins = False


def __ShowTitleIfNecessary(funcGetTitle):
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print(funcGetTitle())


def __Print(text):
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print(text)


def __AddDefaultOptions(parser):
    parser.add_argument('-v', '--verbosity', action='count', default=0, help='Set verbosity level')
    parser.add_argument('--debug', action='store_true',  help='Enable script debugging')
    parser.add_argument('--dev', action='store_true',  help='Allow plugins in development')
    parser.add_argument('--configFile', default=None,  help='The tool config file to load')


def __EarlyArgumentParser():
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_allowDevelopmentPlugins
    ### Parse the initial options this allows us to use the required debug and verbosity levels while
    ### creating the actual command line argumnets.
    try:
        parser = argparse.ArgumentParser(add_help=False)
        __AddDefaultOptions(parser)
        args, unknown = parser.parse_known_args()
        __g_verbosityLevel = args.verbosity
        __g_debugEnabled = True if args.debug else False;
        __g_allowDevelopmentPlugins = True if args.dev else False;
    except (Exception) as ex:
        print("ERROR: %s" % ex.message)
        if __g_debugEnabled:
            raise
        else:
            return False
    return True



def __ProcessRemainingArgs(args):
    if args == None or len(args) <= 0:
        return []

    if args[0] !=  '--':
        raise Exception("Expected '--' before additional build options not '%s'" % (args[0]))

    return args[1:]


class BuildTimer(object):
    def __init__(self):
        super(BuildTimer, self).__init__()
        self.StartTime = time.time()

    def GetTimePassed(self):
        return time.time() - self.StartTime


def PrintBuildTiming(timing):
    timeString = str(timedelta(seconds = timing.GetTimePassed()))
    print("Build time: %s" % (timeString))


def __TryGetProjectRootConfigPath(currentDir, projectRootFileName):
    try:
        return IOUtil.TryFindFileInCurrentOrParentDir(currentDir, projectRootFileName)
    except (Exception) as ex:
        print("ERROR: %s" % ex)
        if __g_debugEnabled:
            raise
    return None


def __GetProjectRootConfig(currentDir):
    """ Try to locate a instance of the 'Project.gen' file in the current or parent directories """
    path = __TryGetProjectRootConfigPath(currentDir, "Project.gen")
    if not path:
        __Print("Using project root config file: '{0}'".format(path))
    return XmlProjectRootConfigFile(path)


def __GetToolConfigPath(defaultPath):
    """ Retrieve the full path of the tool config file """
    toolConfigPath = defaultPath
    if not toolConfigPath:
        toolConfigPath = IOUtil.GetExecutablePath()
        toolConfigPath = IOUtil.Join(toolConfigPath, "FslBuildGen.xml")
    return toolConfigPath


def ProgramMain(funcGetTitle, funcAddCustomArguments, funcMain, programFlowConfig, funcCreateUserTag = None):
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_allowDevelopmentPlugins

    if not __EarlyArgumentParser():
        return

    generatorPlugins = PluginConfig.GetGeneratorPlugins(__g_allowDevelopmentPlugins)
    generatorIds = [entry.Id for entry in generatorPlugins]
    if not programFlowConfig.AllowPlaformAll:
        generatorIds.remove(PluginConfig.PLATFORM_ID_ALL)

    defaultPlatform = DetectBuildPlatform().lower()

    ### Do the actual command line parsing
    parser = argparse.ArgumentParser(description='Update the build files and build')
    __AddDefaultOptions(parser)
    if programFlowConfig.AddPlatformArg:
        parser.add_argument('-p', '--platform', default=defaultPlatform,  help='Select build platform: %s' % (", ".join(generatorIds)))
    if programFlowConfig.SupportBuildTime:
        parser.add_argument('--BuildTime', action='store_true',  help='Time the build')
    if programFlowConfig.ProcessRemainingArgs:
        parser.add_argument('RemainingArgs', nargs=argparse.REMAINDER)
    #parser.add_argument('--NativeGen', action='store_true',  help='Force use the native build generator')

    buildTiming = None
    try:
        userTag = None
        if funcCreateUserTag:
            userTag = funcCreateUserTag()
        funcAddCustomArguments(parser, userTag)

        currentDir = Util.EnsureUTF8(os.getcwd())

        args = parser.parse_args()

        if programFlowConfig.AddPlatformArg and args.platform.lower() != PluginConfig.PLATFORM_ID_ALL:
            PluginConfig.SetForceUseNativeGenerator(True)
            #PluginConfig.SetForceUseNativeGenerator(args.NativeGen)
            

        if programFlowConfig.ProcessRemainingArgs:
            args.RemainingArgs = __ProcessRemainingArgs(args.RemainingArgs)
        if programFlowConfig.SupportBuildTime and args.BuildTime:
            buildTiming = BuildTimer()

        # If no specific config file was specified we try to locate a project root configuration file
        toolConfigFile = args.configFile
        projectRootConfig = None
        if toolConfigFile == None:
            projectRootConfig = __GetProjectRootConfig(currentDir)
            toolConfigFile = projectRootConfig.ToolConfigFile
        else:
            projectRootConfig = XmlProjectRootConfigFile(None)

        # Get the path to the toolconfig file if necessary and load the tool config file
        toolConfigPath = __GetToolConfigPath(toolConfigFile)
        toolConfig = ToolConfig(toolConfigPath, projectRootConfig)

        funcMain(funcGetTitle, args, currentDir, toolConfig, userTag)
        if buildTiming:
            PrintBuildTiming(buildTiming)
    except GroupedException as ex:
        __ShowTitleIfNecessary(funcGetTitle)
        if buildTiming:
            PrintBuildTiming(buildTiming)
        for entry in ex.ExceptionList:
            print("ERROR: %s" % entry)
        if __g_debugEnabled:
            raise
        sys.exit(1)
    except (Exception) as ex:
        if buildTiming:
            PrintBuildTiming(buildTiming)
        print("ERROR: %s" % ex)
        if __g_debugEnabled:
            raise
        sys.exit(1)
