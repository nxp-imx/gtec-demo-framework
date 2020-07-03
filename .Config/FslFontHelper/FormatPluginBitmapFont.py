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

from typing import Any
from typing import Dict
from typing import List
import json
from FslFontHelper import FormatPluginFontBasicKerningUtil
from FslFontHelper import IOUtil
from FslFontHelper.BitmapFont import BitmapFont
from FslFontHelper.BitmapFont import BitmapFontChar
from FslFontHelper.BitmapFont import BitmapFontKerning
from FslFontHelper.FormatPlugin import FormatPlugin
from FslFontHelper.FormatPlugin import FormatPluginType

class FormatPluginBitmapFont(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("nbf", FormatPluginType.BitmapFont)

    def ProcessBitmapFont(self, bitmapFont: BitmapFont, outputFilename: str) -> None:
        dstBuffer = [] # type: List[int]

        offsetSize = self.__WriteHeader(dstBuffer)
        sizeOfHeader = len(dstBuffer)

        self.__WriteContent(dstBuffer, bitmapFont)

        content = bytearray(dstBuffer)
        finalOutputFilename = '{}.{}'.format(outputFilename, 'nbf')
        IOUtil.WriteBinaryFileIfChanged(finalOutputFilename, content)

    def __WriteHeader(self, dstBuffer: List[int]) -> int:
        FormatPluginFontBasicKerningUtil.AddUInt32(dstBuffer, 0x004E4246)
        FormatPluginFontBasicKerningUtil.AddUInt32(dstBuffer, 1)
        offset = len(dstBuffer)
        FormatPluginFontBasicKerningUtil.AddUInt32(dstBuffer, 0)
        return offset

    def __WriteContent(self, dstBuffer: List[int], bitmapFont: BitmapFont) -> None:
        FormatPluginFontBasicKerningUtil.AddString(dstBuffer, bitmapFont.Name)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt16(dstBuffer, bitmapFont.Size)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt16(dstBuffer, bitmapFont.LineSpacingPx)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt16(dstBuffer, bitmapFont.BaseLinePx)
        FormatPluginFontBasicKerningUtil.AddString(dstBuffer, bitmapFont.TextureName)
        FormatPluginFontBasicKerningUtil.AddEncodedBitmapFontType(dstBuffer, bitmapFont.FontType)

        self.__AddChars(dstBuffer, bitmapFont.Chars)
        self.__AddKernings(dstBuffer, bitmapFont.Kernings)

    def __AddChars(self, dstBuffer: List[int], chars: List[BitmapFontChar]) -> None:
        # Sort the chars by id to ensure we always write the data in the same order
        sortedChars = list(chars)
        sortedChars.sort(key=lambda entry: entry.Id)

        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, len(sortedChars))
        for ch in sortedChars:
            self.__AddChar(dstBuffer, ch)


    def __AddChar(self, dstBuffer: List[int], bitmapFontChar: BitmapFontChar) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontChar.Id)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontChar.X)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontChar.Y)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontChar.WidthPx)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontChar.HeightPx)
        FormatPluginFontBasicKerningUtil.AddEncodedInt32(dstBuffer, bitmapFontChar.XOffsetPx)
        FormatPluginFontBasicKerningUtil.AddEncodedInt32(dstBuffer, bitmapFontChar.YOffsetPx)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt16(dstBuffer, bitmapFontChar.XAdvancePx)

    def __AddKernings(self, dstBuffer: List[int], kernings: List[BitmapFontKerning]) -> None:
        # Sort the kernings by first to ensure we always write the data in the same order
        sortedKernings = list(kernings)
        sortedKernings.sort(key=lambda entry: entry.First)

        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, len(sortedKernings))
        for entry in sortedKernings:
            self.__AddKerning(dstBuffer, entry)

    def __AddKerning(self, dstBuffer: List[int], bitmapFontKerning: BitmapFontKerning) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontKerning.First)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dstBuffer, bitmapFontKerning.Second)
        FormatPluginFontBasicKerningUtil.AddEncodedInt32(dstBuffer, bitmapFontKerning.AmountPx)
