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
from enum import Enum
from FslFontHelper.BasicFont import BasicFont
from FslFontHelper.BasicFont import BasicFontHeader
from FslFontHelper.BasicFont import BasicFontPoint2
from FslFontHelper.BasicFont import BasicFontGlyphKerning
from FslFontHelper.BasicFont import BasicFontGlyphRange
from FslFontHelper.BitmapFont import BitmapFont
from FslFontHelper.BitmapFont import BitmapFontChar
from FslFontHelper.BitmapFont import BitmapFontKerning
from FslFontHelper.BitmapFont import BitmapFontType

#-----------------------------------------------------------------------------------------------------------------------------------------------------

class ExtractRangeState(Enum):
    FindBegin = 1
    FindEnd = 2


def _ExtractHeader(font: BitmapFont) -> BasicFontHeader:
    return BasicFontHeader(font.TextureName, font.LineSpacingPx, font.BaseLinePx, BasicFontPoint2(0, 0))

def _AddRange(dstList: List[BasicFontGlyphRange], newEntry: BasicFontGlyphRange) -> None:
    if newEntry.From < 32 and (newEntry.From + newEntry.Length) <= 32:
        return
    dstList.append(newEntry)

def _ExtractRanges(font: BitmapFont) -> List[BasicFontGlyphRange]:
    result = [] # type: List[BasicFontGlyphRange]
    state = ExtractRangeState.FindBegin
    rangeStartId = 0
    expectedRangeId = 0

    index = 0
    while index < len(font.Chars):
        fontChar = font.Chars[index]
        if state == ExtractRangeState.FindBegin:
            state = ExtractRangeState.FindEnd
            rangeStartId = fontChar.Id
            expectedRangeId = rangeStartId + 1
        elif state == ExtractRangeState.FindEnd:
            if fontChar.Id != expectedRangeId:
                state = ExtractRangeState.FindBegin
                _AddRange(result, BasicFontGlyphRange(rangeStartId, expectedRangeId - rangeStartId, rangeStartId))
                index -= 1
            else:
                expectedRangeId += 1
        index += 1

    if state == ExtractRangeState.FindEnd:
        _AddRange(result, BasicFontGlyphRange(rangeStartId, expectedRangeId - rangeStartId, rangeStartId))
    return result

def _IsInRange(validRanges: List[BasicFontGlyphRange], charId: int) -> bool:
    for range in validRanges:
        if charId >= range.From and charId < (range.From + range.Length):
          return True;
    return False

def _ExtractKernings(font: BitmapFont, validRanges: List[BasicFontGlyphRange]) -> List[BasicFontGlyphKerning]:
    result = [] # type: List[BasicFontGlyphKerning]
    for fontChar in font.Chars:
        if _IsInRange(validRanges, fontChar.Id):
            result.append(BasicFontGlyphKerning(fontChar.Id, fontChar.XOffsetPx, fontChar.YOffsetPx, fontChar.XAdvancePx))
    return result

def ToBasicFont(font: BitmapFont) -> BasicFont:
    name = font.Name
    header = _ExtractHeader(font)
    fontGlyphRanges = _ExtractRanges(font)
    fontGlyphKernings = _ExtractKernings(font, fontGlyphRanges)
    return BasicFont(name, header, fontGlyphRanges, fontGlyphKernings)

#-----------------------------------------------------------------------------------------------------------------------------------------------------

def _FindBasicGlyphKerning(basicKernings: List[BasicFontGlyphKerning], id: int) -> BasicFontGlyphKerning:
    for entry in basicKernings:
        if entry.Id == id:
            return entry
    raise Exception("Could not find information for {}".format(id))

def _ToBitmapFontChars(ranges: List[BasicFontGlyphRange], basicKernings: List[BasicFontGlyphKerning]) -> List[BitmapFontChar]:
    # To get the full information we will need information from the texture atlas
    print("FIX: implement support for the texture rectangle")
    result = [] # type: List[BitmapFontChar]
    for basicGlyphRange in ranges:
        for id in range(basicGlyphRange.From, basicGlyphRange.From + basicGlyphRange.Length):
            basicGlyphKerning = _FindBasicGlyphKerning(basicKernings, id)
            rectTexX = 0
            rectTexY = 0
            rectTexWidth = 0
            rectTexHeight = 0
            xOffsetPx = basicGlyphKerning.OffsetX
            yOffsetPx = basicGlyphKerning.OffsetY
            xAdvancePx = basicGlyphKerning.LayoutWidth
            result.append(BitmapFontChar(id, rectTexX, rectTexY, rectTexWidth, rectTexHeight, xOffsetPx, yOffsetPx, xAdvancePx))
    return result

def ToBitmapFont(font: BasicFont) -> BitmapFont:
    chars = _ToBitmapFontChars(font.Ranges, font.BasicGlyphKerning)
    kernings = [] # type: List[BitmapFontKerning]
    return BitmapFont(font.Name, 1, font.Header.LineSpacing, font.Header.BaseLine, font.Header.PathName, BitmapFontType.Bitmap, chars, kernings)

#-----------------------------------------------------------------------------------------------------------------------------------------------------

