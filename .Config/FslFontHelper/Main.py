#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
#    * Neither the name of the NXP. nor the names of
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

from typing import List
import argparse
import os
from FslFontHelper import AngelCodeFontReader
from FslFontHelper import IOUtil
from FslFontHelper import SpriteFontCreator2Reader
from FslFontHelper import TypeConverter
from FslFontHelper.BasicFont import BasicFont
from FslFontHelper.BitmapFont import BitmapFont
from FslFontHelper.BitmapFont import BitmapFontType
from FslFontHelper.FormatPlugin import FormatPlugin
from FslFontHelper.FormatPlugin import FormatPluginType
from FslFontHelper.FormatPluginBasicKerning import FormatPluginBasicKerning
from FslFontHelper.FormatPluginBitmapFont import FormatPluginBitmapFont
from FslFontHelper.FormatPluginBitmapFontJson import FormatPluginBitmapFontJson
from FslFontHelper.FormatPluginFontBasicKerning import FormatPluginFontBasicKerning
from FslFontHelper.SpriteFontCreator2Reader import BasicKerning


__g_verbosityLevel = 0
__g_debugEnabled = False
__g_allowDevelopmentPlugins = False


def GetTitle() -> str:
    return 'FslFontHelper V0.2.3 alpha'


def ShowTitleIfNecessary() -> None:
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print((GetTitle()))


def GetEntryByName(formatPlugins: List[FormatPlugin], name: str) -> FormatPlugin:
    for entry in formatPlugins:
        if entry.Name == name:
            return entry
    raise Exception("Unknown plugin '{0}'".format(name))

def ProcessSpriteFontCreator2JsonFileAsBitmapFont(inputFilename: str, name: str) -> BitmapFont:
    # Read the json file and convert it into python objects
    content = IOUtil.ReadFile(inputFilename)
    return SpriteFontCreator2Reader.DecodeAsBitmapFont(content, name)

def ProcessAngelCodeFontFileAsBitmapFont(inputFilename: str, name: str, bitmapFontType: BitmapFontType) -> BitmapFont:
    content = IOUtil.ReadFile(inputFilename)
    return AngelCodeFontReader.DecodeAsBitmapFont(content, bitmapFontType)


def ProcessSpriteFontCreator2JsonFile(inputFilename: str, name: str) -> BasicFont:
    # Read the json file and convert it into python objects
    content = IOUtil.ReadFile(inputFilename)
    return SpriteFontCreator2Reader.Decode(content, name)

def ProcessAngelCodeFontFile(inputFilename: str, name: str, bitmapFontType: BitmapFontType) -> BasicFont:
    content = IOUtil.ReadFile(inputFilename)
    bitmapFont = AngelCodeFontReader.DecodeAsBitmapFont(content, bitmapFontType)
    return TypeConverter.ToBasicFont(bitmapFont)


def ProcessSourceFile(inputFilename: str, outputFilename: str, bitmapFontType: BitmapFontType) -> BasicFont:
    inputExt = IOUtil.GetFileNameExtension(inputFilename)
    if inputExt == '.json':
        return ProcessSpriteFontCreator2JsonFile(inputFilename, outputFilename)
    if inputExt == '.fnt':
        return ProcessAngelCodeFontFile(inputFilename, outputFilename, bitmapFontType)
    raise Exception("Unsupported input file extension: {}".format(inputExt))

def ProcessSourceFileAsBitmapFont(inputFilename: str, outputFilename: str, bitmapFontType: BitmapFontType) -> BitmapFont:
    inputExt = IOUtil.GetFileNameExtension(inputFilename)
    if inputExt == '.json':
        return ProcessSpriteFontCreator2JsonFileAsBitmapFont(inputFilename, outputFilename)
    if inputExt == '.fnt':
        return ProcessAngelCodeFontFileAsBitmapFont(inputFilename, outputFilename, bitmapFontType)
    raise Exception("Unsupported input file extension: {}".format(inputExt))


def ProcessV1(formatPlugins: List[FormatPlugin], inputFilename: str, outputFilename: str, bitmapFontType: BitmapFontType, formatPlugin: FormatPlugin) -> None:
    atlas = ProcessSourceFile(inputFilename, outputFilename, bitmapFontType)
    formatPlugin.Process(atlas, outputFilename)


def ProcessV2(formatPlugins: List[FormatPlugin], inputFilename: str, outputFilename: str, bitmapFontType: BitmapFontType, formatPlugin: FormatPlugin) -> None:
    bitmapFont = ProcessSourceFileAsBitmapFont(inputFilename, outputFilename, bitmapFontType)
    formatPlugin.ProcessBitmapFont(bitmapFont, outputFilename)


def Process(formatPlugins: List[FormatPlugin], inputFilename: str, outputFilename: str, formatName: str, bitmapFontType: BitmapFontType) -> None:
    formatPlugin = GetEntryByName(formatPlugins, formatName)
    if outputFilename is None:
        outputFilename = IOUtil.GetFileNameWithoutExtension(inputFilename)


    if formatPlugin.PluginType == FormatPluginType.BasicFont:
        ProcessV1(formatPlugins, inputFilename, outputFilename, bitmapFontType, formatPlugin)
    else:
        ProcessV2(formatPlugins, inputFilename, outputFilename, bitmapFontType, formatPlugin)


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
    res = [FormatPluginBasicKerning(), FormatPluginFontBasicKerning(), FormatPluginBitmapFont(), FormatPluginBitmapFontJson()]
    return res;


def GetNames(entries: List[FormatPlugin]) -> List[str]:
    return [entry.Name for entry in entries];


def Run() -> None:
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_allowDevelopmentPlugins

    if not EarlyArgumentParser():
        return

    plugins = GetPluginList(__g_allowDevelopmentPlugins)
    fontTypes = ['bitmap', 'sdf']

    ### Add the main command line arguments
    parser = argparse.ArgumentParser(description='Convert basic kerning Json(array) files into a custom native format.')
    AddDefaultOptions(parser)
    parser.add_argument('-f', '--format', default=plugins[0].Name,  help='Select output format: {}'.format(", ".join(GetNames(plugins))))
    parser.add_argument('-t', '--type', default=fontTypes[0],  help='Select font type: {}'.format(", ".join(fontTypes)))
    parser.add_argument("inputFile", help="the name of the input file. Supply a text based AngleCodeFont '.fnt' file or a SpriteFontCreator2 '.json' file")
    parser.add_argument("outputFile", nargs='?', default=None, help="the name of the output file")

    try:
        args = parser.parse_args()
        bitmapFontType = BitmapFontType.FromString(args.type)
        Process(plugins, args.inputFile, args.outputFile, args.format, bitmapFontType)
    except (Exception) as ex:
        ShowTitleIfNecessary()
        print("ERROR: {0}".format(ex))
        if __g_debugEnabled:
            raise
    return
