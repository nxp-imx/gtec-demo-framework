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
from FslFontHelper import IOUtil
from FslFontHelper.BitmapFont import BitmapFont
from FslFontHelper.BitmapFont import BitmapFontChar
from FslFontHelper.BitmapFont import BitmapFontKerning
from FslFontHelper.FormatPlugin import FormatPlugin
from FslFontHelper.FormatPlugin import FormatPluginType

class FormatPluginBitmapFontJson(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("json", FormatPluginType.BitmapFont)

    def ProcessBitmapFont(self, bitmapFont: BitmapFont, outputFilename: str) -> None:
        jsonRootDict = self.__ToJsonDict(bitmapFont)
        jsonText = str(json.dumps(jsonRootDict, ensure_ascii=False, sort_keys=True, indent=2))
        IOUtil.WriteFileIfChanged(outputFilename, jsonText)

    def __BitmapFontCharToJson(self, entry: BitmapFontChar) -> Dict[str, Any]:
        result = {} # Dict[str, Any]
        result['Id'] = entry.Id
        result['X'] = entry.X
        result['Y'] = entry.Y
        result['Width'] = entry.WidthPx
        result['Height'] = entry.HeightPx
        result['XOffsetPx'] = entry.XOffsetPx
        result['YOffsetPx'] = entry.YOffsetPx
        result['XAdvancePx'] = entry.XAdvancePx
        return result

    def __BitmapFontCharsToJson(self, entries: List[BitmapFontChar]) -> List[Dict[str, Any]]:
        result = [] # type: List[Dict[str, Any]]
        for entry in entries:
            result.append(self.__BitmapFontCharToJson(entry))
        return result

    def __BitmapFontKerningToJson(self, entry: BitmapFontKerning) -> Dict[str, Any]:
        result = {} # Dict[str, Any]
        result['First'] = entry.First
        result['Second'] = entry.Second
        result['AmountPx'] = entry.AmountPx
        return result

    def __BitmapFontKerningsToJson(self, entries: List[BitmapFontKerning]) -> List[Dict[str, Any]]:
        result = [] # type: List[Dict[str, Any]]
        for entry in entries:
            result.append(self.__BitmapFontKerningToJson(entry))
        return result

    def __ToJsonDict(self, bitmapFont: BitmapFont) -> Dict[str, Any]:
        result = {} # type: Dict[str, Any]
        result['Format'] = "BitmapFont"
        result['Version'] = 1
        result['Name'] = bitmapFont.Name
        result['Size'] = bitmapFont.Size
        result['LineSpacingPx'] = bitmapFont.LineSpacingPx
        result['BaseLinePx'] = bitmapFont.BaseLinePx
        result['TextureName'] = bitmapFont.TextureName
        result['Chars'] = self.__BitmapFontCharsToJson(bitmapFont.Chars)
        result['Kernings'] = self.__BitmapFontKerningsToJson(bitmapFont.Kernings)
        return result
