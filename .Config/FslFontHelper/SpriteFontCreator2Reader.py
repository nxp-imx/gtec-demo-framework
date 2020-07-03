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
import os
from FslFontHelper import TypeConverter
from FslFontHelper.BasicFont import BasicFont
from FslFontHelper.BasicFont import BasicFontGlyphKerning
from FslFontHelper.BasicFont import BasicFontGlyphRange
from FslFontHelper.BasicFont import BasicFontHeader
from FslFontHelper.BasicFont import BasicFontPoint2
from FslFontHelper.BitmapFont import BitmapFont
from FslFontHelper.BitmapFont import BitmapFontChar
from FslFontHelper.BitmapFont import BitmapFontKerning

class BasicGlyphRange(object):
    def __init__(self, dict: Dict[str,int]) -> None:
        super().__init__()
        self.From = dict["From"] # type: int
        self.Length = dict["Length"] # type: int
        self.Offset = dict["Offset"] # type: int


def ExtractRanges(entries: List[Dict[str,int]]) -> List[BasicGlyphRange]:
    list = []
    for entry in entries:
        list.append(BasicGlyphRange(entry))
    return list


class BasicGlyphKerning(object):
    def __init__(self, dict: Dict[str,int]) -> None:
        super().__init__()
        self.Id = dict["Id"] # type: int
        self.OffsetX = dict["OffsetX"] # type: int
        self.OffsetY = dict["OffsetY"] # type: int
        self.LayoutWidth = dict["LayoutWidth"]


def ExtractBasicGlyphKerning(entries: List[Dict[str,int]]) -> List[BasicGlyphKerning]:
    list = []
    for entry in entries:
        list.append(BasicGlyphKerning(entry))
    return list


class BasicPoint2(object):
    def __init__(self, dict: Dict[str,int]) -> None:
        super().__init__()
        self.X = dict["X"] # type: int
        self.Y = dict["Y"] # type: int


class BasicKerningHeader(object):
    def __init__(self, dict: Dict[str,Any]) -> None:
        super().__init__()

        if not "App" in dict or dict["App"] != "SpriteFontCreator2":
            raise Exception("Unknown json format")
        if not "Version" in dict or dict["Version"] != "1.0":
            raise Exception("Unknown json format")
        self.PathName = dict["PathName"] # type: str
        self.LineSpacing = dict["LineSpacing"] # type: int
        self.BaseLine = dict["BaseLine"]  # type: int
        self.MaxGlyphLeadingOverdrawArea = BasicPoint2(dict["MaxGlyphLeadingOverdrawArea"])


class BasicKerning(object):
    def __init__(self, dict: Dict[str,Any], name: str) -> None:
        super().__init__()

        if not "BasicGlyphKerning" in dict or not "Ranges" in dict or not "Meta" in dict:
            raise Exception("Unknown json format")

        self.Name = name
        self.Header = BasicKerningHeader(dict["Meta"]);
        self.Ranges = ExtractRanges(dict["Ranges"]);
        self.BasicGlyphKerning = ExtractBasicGlyphKerning(dict["BasicGlyphKerning"]);


def _ToBasicFontGlyphKerning(value: BasicGlyphKerning) -> BasicFontGlyphKerning:
    return BasicFontGlyphKerning(value.Id, value.OffsetX, value.OffsetY, value.LayoutWidth)

def _ToBasicFontGlyphRange(value: BasicGlyphRange) -> BasicFontGlyphRange:
    return BasicFontGlyphRange(value.From, value.Length, value.Offset)

def _ToBasicFontPoint2(value: BasicPoint2) -> BasicFontPoint2:
    return BasicFontPoint2(value.X, value.Y)

def _ToBasicFontGlyphKernings(entries: List[BasicGlyphKerning]) -> List[BasicFontGlyphKerning]:
    result = [] # type: List[BasicFontGlyphKerning]
    for entry in entries:
        result.append(_ToBasicFontGlyphKerning(entry))
    return result

def _ToBasicFontGlyphRanges(entries: List[BasicGlyphRange]) -> List[BasicFontGlyphRange]:
    result = [] # type: List[BasicFontGlyphRange]
    for entry in entries:
        result.append(_ToBasicFontGlyphRange(entry))
    return result

def _ToBasicFontHeader(value: BasicKerningHeader) -> BasicFontHeader:
    maxGlyphLeadingOverdrawArea = _ToBasicFontPoint2(value.MaxGlyphLeadingOverdrawArea)
    return BasicFontHeader(value.PathName, value.LineSpacing, value.BaseLine, maxGlyphLeadingOverdrawArea)

def _ToBasicFont(basicKerning: BasicKerning) -> BasicFont:
    header = _ToBasicFontHeader(basicKerning.Header)
    ranges = _ToBasicFontGlyphRanges(basicKerning.Ranges)
    glyphKernings = _ToBasicFontGlyphKernings(basicKerning.BasicGlyphKerning)
    return BasicFont(basicKerning.Name, header, ranges, glyphKernings)


def Decode(content: str, name: str) -> BasicFont:
    jsonDict = json.loads(content)
    return _ToBasicFont(BasicKerning(jsonDict, name))


def DecodeAsBitmapFont(content: str, name: str) -> BitmapFont:
    font = Decode(content, name)
    return TypeConverter.ToBitmapFont(font)

