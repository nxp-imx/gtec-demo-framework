#!/usr/bin/env python3
#****************************************************************************************************************************************************
#* BSD 3-Clause License
#*
#* Copyright (c) 2014, Mana Battery
#* All rights reserved.
#*
#* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#*
#* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
#*    documentation and/or other materials provided with the distribution.
#* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
#*    software without specific prior written permission.
#*
#* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
#* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#****************************************************************************************************************************************************

from typing import List
import argparse
import json
import os
from TPConvert.FormatPlugin import FormatPlugin
from TPConvert.FormatPluginCSharp import FormatPluginCSharp
from TPConvert.FormatPluginCSharp2 import FormatPluginCSharp2
from TPConvert.FormatPluginCSharp3 import FormatPluginCSharp3
from TPConvert.FormatPluginFslAtlas import FormatPluginFslAtlas
from TPConvert.FormatPluginBinaryTA1 import FormatPluginBinaryTA1
from TPConvert.FormatPluginBinaryTA2 import FormatPluginBinaryTA2
from TPConvert.IOUtil import *
from TPConvert.TexturePackerObjects import *

__g_verbosityLevel = 0
__g_debugEnabled = False
__g_allowDevelopmentPlugins = False


def GetTitle() -> str:
    return 'TPConvert V0.3.0 alpha'


def ShowTitleIfNecessary() -> None:
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print(GetTitle())


def GetEntryByName(entries: List[FormatPlugin], name: str) -> FormatPlugin:
    for entry in entries:
        if entry.Name == name:
            return entry
    raise Exception("Unknown plugin '{0}'".format(name))


def Process(formatPlugins: List[FormatPlugin], inputFilename: str, outputFilename: str, formatName: str, dpi: str) -> None:
    if outputFilename == None:
        outputFilename = GetFileNameWithoutExtension(inputFilename)

    formatPlugin = GetEntryByName(formatPlugins, formatName)

    # Read the json file and convert it into python objects
    content = ReadBinaryFile(inputFilename)
    jsonDict = json.loads(content)
    atlas = TexturePackerAtlas(jsonDict, int(dpi))

    formatPlugin.Process(atlas, outputFilename)


def AddDefaultOptions(parser: argparse.ArgumentParser) -> None:
    parser.add_argument('-v', '--verbosity', action='count', default=0, help='Set verbosity level')
    parser.add_argument('--debug', action='store_true',  help='Enable script debugging')
    parser.add_argument('--dev', action='store_true',  help='Allow plugins in development')


def EarlyArgumentParser() -> bool:
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
        print("ERROR: {0}".format(ex))
        if __g_debugEnabled:
            raise
        else:
            return False
    return True


def GetPluginList(allowDevPlugins: bool) -> List[FormatPlugin]:
    return [FormatPluginCSharp(), FormatPluginCSharp(True), FormatPluginCSharp2(), FormatPluginCSharp3(), FormatPluginFslAtlas(), FormatPluginBinaryTA1(), FormatPluginBinaryTA2()]


def GetNames(entries: List[FormatPlugin]) -> List[str]:
    return [entry.Name for entry in entries];


def Main() -> None:
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_allowDevelopmentPlugins

    if not EarlyArgumentParser():
        return

    plugins = GetPluginList(__g_allowDevelopmentPlugins)

    ### Add the main command line arguments
    parser = argparse.ArgumentParser(description='Convert TexturePacker JSON(array) files into a custom native format.')
    AddDefaultOptions(parser)
    parser.add_argument('-f', '--format', default=plugins[0].Name,  help='Select format {0}'.format(", ".join(GetNames(plugins))))
    parser.add_argument('--dpi', default=160,  help='Select the base DPI for the graphics (defaults to 160) ')
    parser.add_argument("inputFile", help="the name of the input file")
    parser.add_argument("outputFile", nargs='?', default=None, help="the name of the output file")

    try:
        args = parser.parse_args()
        Process(plugins, args.inputFile, args.outputFile, args.format, args.dpi)
    except (IOError) as ex:
        ShowTitleIfNecessary()
        print("ERROR: {0}".format(ex.strerror))
        if __g_debugEnabled:
            raise
    except (Exception) as ex:
        ShowTitleIfNecessary()
        print("ERROR: {0}".format(ex))
        if __g_debugEnabled:
            raise
    return


Main();
