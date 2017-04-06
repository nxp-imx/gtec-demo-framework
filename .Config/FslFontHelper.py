#!/usr/bin/env python
# -*- coding: utf-8 -*-

#****************************************************************************************************************************************************
# Copyright (c) 2015 Freescale Semiconductor, Inc.
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
import json
import os
from FslFontHelper.FormatPluginBasicKerning import FormatPluginBasicKerning
from FslFontHelper.FormatPluginFontBasicKerning import FormatPluginFontBasicKerning
from FslFontHelper import IOUtil
from FslFontHelper.BasicKerningObjects import BasicKerning


__g_verbosityLevel = 0
__g_debugEnabled = False
__g_allowDevelopmentPlugins = False


def GetTitle():
    return 'FslFontHelper V0.1.1 alpha'


def ShowTitleIfNecessary():
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print(GetTitle())


def GetEntryByName(list, name):
    for entry in list:
        if entry.Name == name:
            return entry
    raise Exception("Unknown plugin '%s'" % (name))


def Process(formatPlugins, inputFilename, outputFilename, formatName):
    if outputFilename == None:
        outputFilename = IOUtil.GetFileNameWithoutExtension(inputFilename)

    formatPlugin = GetEntryByName(formatPlugins, formatName)

    # Read the json file and convert it into python objects
    content = IOUtil.ReadFileBinary(inputFilename)
    jsonDict = json.loads(content)
    atlas = BasicKerning(jsonDict, outputFilename)

    formatPlugin.Process(atlas, outputFilename)


def AddDefaultOptions(parser):
    parser.add_argument('-v', '--verbosity', action='count', default=0, help='Set verbosity level')
    parser.add_argument('--debug', action='store_true',  help='Enable script debugging')
    parser.add_argument('--dev', action='store_true',  help='Allow plugins in development')


def EarlyArgumentParser():
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_allowDevelopmentPlugins
    ### Parse the initial options this allows us to use the required debug and verbosity levels while
    ### creating the actual command line argumnets.
    try:
        parser = argparse.ArgumentParser(add_help=False)
        AddDefaultOptions(parser)
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


def GetPluginList(allowDevPlugins):
    res = [ FormatPluginBasicKerning(), FormatPluginFontBasicKerning() ]
    return res;


def GetNames(list):
    return [entry.Name for entry in list];


def Main():
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_allowDevelopmentPlugins

    if not EarlyArgumentParser():
        return

    plugins = GetPluginList(__g_allowDevelopmentPlugins)

    ### Add the main command line arguments
    parser = argparse.ArgumentParser(description='Convert basic kerning Json(array) files into a custom native format.')
    AddDefaultOptions(parser)
    parser.add_argument('-f', '--format', default=plugins[0].Name,  help='Select output format: %s' % (", ".join(GetNames(plugins))))
    parser.add_argument("inputFile", help="the name of the input file")
    parser.add_argument("outputFile", nargs='?', default=None, help="the name of the output file")

    try:
        args = parser.parse_args()
        Process(plugins, args.inputFile, args.outputFile, args.format)
    except (Exception) as ex:
        ShowTitleIfNecessary()
        print("ERROR: %s" % ex.message)
        if __g_debugEnabled:
            raise
    return


Main();
