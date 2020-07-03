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

from typing import Dict
from typing import List
from typing import Tuple
from enum import Enum
from FslFontHelper.BitmapFont import BitmapFont
from FslFontHelper.BitmapFont import BitmapFontChar
from FslFontHelper.BitmapFont import BitmapFontKerning
from FslFontHelper.BitmapFont import BitmapFontType


# Decode Angle Font Format  http://www.angelcode.com/products/bmfont/doc/file_format.html

class AngleEntryAttribute(object):
    def __init__(self, name: str, value: str) -> None:
        self.Name = name
        self.Value = value

    def __str__(self) -> str:
        return "{}={}".format(self.Name, self.Value)

class IntArray2(object):
    def __init__(self, value: List[int]) -> None:
        if len(value) != 2:
            raise Exception("Value array must contain 2 entries not {}".format(len(value)))
        self.Value = value

    def __str__(self) -> str:
        return str(self.Value)

class IntArray4(object):
    def __init__(self, value: List[int]) -> None:
        if len(value) != 4:
            raise Exception("Value array must contain 4 entries not {}".format(len(value)))
        self.Value = value

    def __str__(self) -> str:
        return str(self.Value)

# This tag holds information on how the font was generated.
class AngleFontInfo(object):
    def __init__(self, face: str, size: int, bold: bool, italic: bool, charset: str, unicode: bool, stretchH: int, smooth: bool, aa: int,
                 padding: IntArray4, spacing: IntArray2) -> None:
        self.Face = face                    # This is the name of the true type font.
        self.Size = size                    # The size of the true type font.
        self.Bold = bold                    # The font is bold
        self.Italic = italic                # The font is italic
        self.Charset = charset              # The name of the OEM charset used (when not unicode).
        self.Unicode = unicode              # Set to true if it is the unicode charset
        self.StretchH = stretchH            # The font height stretch in percentage. 100% means no stretch
        self.Smooth = smooth                # Set to true if smoothing was turned on.
        self.Aa = aa                        # The supersampling level used. 1 means no supersampling was used.
        self.Padding = padding              # The padding for each character (up, right, down, left).
        self.Spacing = spacing              # The spacing for each character (horizontal, vertical).

# This tag holds information common to all characters.
class AngleFontCommon(object):
    def __init__(self, lineHeight: int, base: int, scaleW: int, scaleH: int, pages: int, packed: bool) -> None:
        self.LineHeight = lineHeight        # This is the distance in pixels between each line of text.
        self.Base = base                    # The number of pixels from the absolute top of the line to the base of the characters.
        self.ScaleW = scaleW                # The width of the texture, normally used to scale the x pos of the character image.
        self.ScaleH = scaleH                # The height of the texture, normally used to scale the y pos of the character image.
        self.Pages = pages                  # The number of texture pages included in the font.
        self.Packed = packed                # Set to true if the monochrome characters have been packed into each of the texture channels. In this case alphaChnl describes what is stored in each channel.

# This tag gives the name of a texture file. There is one for each page in the font.
class AngleFontPage(object):
    def __init__(self, id: int, file: str) -> None:
        self.Id = id                        # The page id.
        self.File = file                    # The texture file name.

# This tag describes on character in the font. There is one for each included character in the font.
class AngleFontChar(object):
    def __init__(self, id: int, x: int, y: int, width: int, height: int, xOffset: int, yOffset: int, xAdvance: int, page: int, chnl: int) -> None:
        self.Id = id                        # The character id.
        self.X = x                          # The left position of the character image in the texture.
        self.Y = y                          # The top position of the character image in the texture.
        self.Width = width                  # The width of the character image in the texture.
        self.Height = height                # The height of the character image in the texture.
        self.XOffset = xOffset              # How much the current position should be offset when copying the image from the texture to the screen.
        self.YOffset = yOffset              # How much the current position should be offset when copying the image from the texture to the screen.
        self.XAdvance = xAdvance            # How much the current position should be advanced after drawing the character.
        self.Page = page                    # The texture page where the character image is found.
        self.Chnl = chnl                    # The texture channel where the character image is found (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels).

# The kerning information is used to adjust the distance between certain characters, e.g. some characters should be placed closer to each other than others.
class AngleFontKerning(object):
    def __init__(self, first: int, second: int, amount: int) -> None:
        self.First = first                  # The first character id.
        self.Second = second                # The second character id.
        self.Amount = amount                # How much the x position should be adjusted when drawing the second character immediately following the first.


class AngleFont(object):
    def __init__(self, info: AngleFontInfo, common: AngleFontCommon, pages: List[AngleFontPage], chars: List[AngleFontChar],
                 kernings: List[AngleFontKerning]) -> None:

        pages.sort(key=lambda entry: entry.Id)
        chars.sort(key=lambda entry: entry.Id)
        kernings.sort(key=lambda entry: entry.First)

        self.Info = info
        self.Common = common
        self.Pages = pages
        self.Chars = chars
        self.Kernings = kernings

        self.__ValidatePages(pages)
        self.__ValidateChars(chars)

    def __ValidatePages(self, pages: List[AngleFontPage]) -> None:
        currentPage = -1
        for page in pages:
            if page.Id < 0 or page.Id >= len(pages):
                raise Exception("Page id {} is out of bounds".format(page.Id))
            if page.Id <= currentPage:
                raise Exception("Duplicated page id {} found".format(page.Id))
            currentPage = page.Id

    def __ValidateChars(self, chars: List[AngleFontChar]) -> None:
        currentChar = -1
        for char in chars:
            if char.Id < 0:
                raise Exception("Char id {} is out of bounds".format(char.Id))
            if char.Id <= currentChar:
                raise Exception("Duplicated char id {} found".format(char.Id))
            if char.X < 0:
                raise Exception("char.Id={} X can not be negative ({})".format(char.Id, char.X))
            if char.Y < 0:
                raise Exception("char.Id={} Y can not be negative ({})".format(char.Id, char.Y))
            if char.Width < 0:
                raise Exception("char.Id={} Width can not be negative ({})".format(char.Id, char.Width))
            if char.Height < 0:
                raise Exception("char.Id={} Height can not be negative ({})".format(char.Id, char.Height))
            currentChar = char.Id

class ParseLineState(Enum):
    FindNextAttribute = 0
    FindAttributeEnd = 1
    FindValueStart = 2
    FindValueEnd = 3
    FindComplexValueEnd = 4

def _IsWhiteSpace(ch: str) -> bool:
    return ch == ' ' or ch == '\t'

def _ParseLine(line: str, lineHeader: str) -> Dict[str, AngleEntryAttribute]:
    if not line.startswith(lineHeader + ' '):
        raise Exception("Unexpected line start '{}' expected '{}'".format(line, lineHeader))
    currentIndex = len(lineHeader) + 1

    state = ParseLineState.FindNextAttribute

    attributeStartIndex = 0
    attributeEndIndex = 0
    attributeValueStartIndex = 0

    result = {} # type: Dict[str, AngleEntryAttribute]
    while(currentIndex < len(line)):
        ch = line[currentIndex]
        if state == ParseLineState.FindNextAttribute:
            if not _IsWhiteSpace(ch):
                attributeStartIndex = currentIndex
                state = ParseLineState.FindAttributeEnd
        elif state == ParseLineState.FindAttributeEnd:
            if ch == '=':
                attributeEndIndex = currentIndex - 1
                state = ParseLineState.FindValueStart
            elif _IsWhiteSpace(ch):
                raise Exception("Found unexpected whitespace")
        elif state == ParseLineState.FindValueStart:
            if ch == '"':
                state = ParseLineState.FindComplexValueEnd
                attributeValueStartIndex = currentIndex + 1
            else:
                state = ParseLineState.FindValueEnd
                attributeValueStartIndex = currentIndex
        elif state == ParseLineState.FindValueEnd:
            if _IsWhiteSpace(ch):
                state = ParseLineState.FindNextAttribute
                name = line[attributeStartIndex:attributeEndIndex+1]
                result[name] = AngleEntryAttribute(name, line[attributeValueStartIndex:currentIndex])
        elif state == ParseLineState.FindComplexValueEnd:
            if ch == '"':
                state = ParseLineState.FindNextAttribute
                name = line[attributeStartIndex:attributeEndIndex+1]
                result[name] = AngleEntryAttribute(name, line[attributeValueStartIndex:currentIndex])
        currentIndex += 1

    if state == ParseLineState.FindValueEnd:
        name = line[attributeStartIndex:attributeEndIndex+1]
        result[name] = AngleEntryAttribute(name, line[attributeValueStartIndex:])
    elif state != ParseLineState.FindNextAttribute:
        raise Exception("Failed to parse line '{}'".format(line))
    return result

def _GetAttribute(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> AngleEntryAttribute:
    if not name in attributeDict:
        raise Exception("Attribute not found '{}'".format(name))
    return attributeDict[name]

def _GetAttributeValueAsInt(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> int:
    return int(_GetAttribute(attributeDict, name).Value)

def _GetAttributeValueAsBool(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> bool:
    return _GetAttributeValueAsInt(attributeDict, name) != 0

def _GetAttributeValueAsStr(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> str:
    return _GetAttribute(attributeDict, name).Value

def _GetAttributeValueAsIntArray(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> List[int]:
    strValue = _GetAttribute(attributeDict, name).Value
    entries = strValue.split(',')
    result =  [] # type: List[int]
    for entry in entries:
        result.append(int(entry))
    return result

def _GetAttributeValueAsIntArray2(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> IntArray2:
    return IntArray2(_GetAttributeValueAsIntArray(attributeDict, name))

def _GetAttributeValueAsIntArray4(attributeDict: Dict[str, AngleEntryAttribute], name: str) -> IntArray4:
    return IntArray4(_GetAttributeValueAsIntArray(attributeDict, name))


def _ParseInfo(line: str) -> AngleFontInfo:
    attributeDict = _ParseLine(line, "info")
    face = _GetAttributeValueAsStr(attributeDict, 'face')
    size = _GetAttributeValueAsInt(attributeDict, 'size')
    bold = _GetAttributeValueAsBool(attributeDict, 'bold')
    italic = _GetAttributeValueAsBool(attributeDict, 'italic')
    charset = _GetAttributeValueAsStr(attributeDict, 'charset')
    unicode = _GetAttributeValueAsBool(attributeDict, 'unicode')
    stretchH = _GetAttributeValueAsInt(attributeDict, 'stretchH')
    smooth = _GetAttributeValueAsBool(attributeDict, 'smooth')
    aa = _GetAttributeValueAsInt(attributeDict, 'aa')
    padding = _GetAttributeValueAsIntArray4(attributeDict, 'padding')
    spacing = _GetAttributeValueAsIntArray2(attributeDict, 'spacing')
    return AngleFontInfo(face, size, bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing)

def _ParseCommon(line: str) -> AngleFontCommon:
    attributeDict = _ParseLine(line, "common")
    lineHeight = _GetAttributeValueAsInt(attributeDict, 'lineHeight')
    base = _GetAttributeValueAsInt(attributeDict, 'base')
    scaleW = _GetAttributeValueAsInt(attributeDict, 'scaleW')
    scaleH = _GetAttributeValueAsInt(attributeDict, 'scaleH')
    pages = _GetAttributeValueAsInt(attributeDict, 'pages')
    packed = _GetAttributeValueAsBool(attributeDict, 'packed')
    return AngleFontCommon(lineHeight, base, scaleW, scaleH, pages, packed)

def _ParsePage(line: str) -> AngleFontPage:
    attributeDict = _ParseLine(line, "page")
    id = _GetAttributeValueAsInt(attributeDict, 'id')
    file = _GetAttributeValueAsStr(attributeDict, 'file')
    return AngleFontPage(id, file)

def _ParsePages(lines: List[str], lineIndex: int) -> Tuple[List[AngleFontPage], int]:
    result = [] # type: List[AngleFontPage]
    while(lineIndex < len(lines) and lines[lineIndex].startswith("page ")):
        result.append(_ParsePage(lines[lineIndex]))
        lineIndex += 1
    return (result, lineIndex)

def _ParseChar(line: str) -> AngleFontChar:
    attributeDict = _ParseLine(line, "char")
    id = _GetAttributeValueAsInt(attributeDict, 'id')
    x = _GetAttributeValueAsInt(attributeDict, 'x')
    y = _GetAttributeValueAsInt(attributeDict, 'y')
    width = _GetAttributeValueAsInt(attributeDict, 'width')
    height = _GetAttributeValueAsInt(attributeDict, 'height')
    xoffset = _GetAttributeValueAsInt(attributeDict, 'xoffset')
    yoffset = _GetAttributeValueAsInt(attributeDict, 'yoffset')
    xadvance = _GetAttributeValueAsInt(attributeDict, 'xadvance')
    page = _GetAttributeValueAsInt(attributeDict, 'page')
    chnl = _GetAttributeValueAsInt(attributeDict, 'chnl')
    return AngleFontChar(id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl)

def _ParseChars(lines: List[str], lineIndex: int) -> Tuple[List[AngleFontChar], int]:
    attributeDict = _ParseLine(lines[lineIndex], "chars")
    charCount = _GetAttributeValueAsInt(attributeDict, 'count')
    lineIndex += 1

    result = [] # type: List[AngleFontChar]
    while(lineIndex < len(lines) and lines[lineIndex].startswith("char ")):
        result.append(_ParseChar(lines[lineIndex]))
        lineIndex += 1

    if len(result) != charCount:
        raise Exception("File did not contain the expected amount of chars. Expected: {} Found: {}".format(charCount, len(result)))
    return (result, lineIndex)


def _ParseKerning(line: str) -> AngleFontKerning:
    attributeDict = _ParseLine(line, "kerning")
    first = _GetAttributeValueAsInt(attributeDict, 'first')
    second = _GetAttributeValueAsInt(attributeDict, 'second')
    amount = _GetAttributeValueAsInt(attributeDict, 'amount')
    return AngleFontKerning(first, second, amount)

def _ParseKernings(lines: List[str], lineIndex: int) -> Tuple[List[AngleFontKerning], int]:
    attributeDict = _ParseLine(lines[lineIndex], "kernings")
    kerningCount = _GetAttributeValueAsInt(attributeDict, 'count')
    lineIndex += 1

    result = [] # type: List[AngleFontKerning]
    while(lineIndex < len(lines) and lines[lineIndex].startswith("kerning ")):
        result.append(_ParseKerning(lines[lineIndex]))
        lineIndex += 1

    if len(result) != kerningCount:
        raise Exception("File did not contain the expected amount of chars. Expected: {} Found: {}".format(kerningCount, len(result)))
    return (result, lineIndex)


def Decode(content: str) -> AngleFont:
    if not content.startswith("info "):
        raise Exception("Unsupported format")

    content = content.replace('\r', '')
    lines = content.split('\n')

    info = _ParseInfo(lines[0])
    common = _ParseCommon(lines[1])
    pages, currentLineIndex = _ParsePages(lines, 2)
    chars, currentLineIndex = _ParseChars(lines, currentLineIndex)
    kernings, currentLineIndex = _ParseKernings(lines, currentLineIndex)

    return AngleFont(info, common, pages, chars, kernings)


def _ToBitmapFontChars(entries: List[AngleFontChar]) -> List[BitmapFontChar]:
    return [BitmapFontChar(entry.Id, entry.X, entry.Y, entry.Width, entry.Height, entry.XOffset, entry.YOffset, entry.XAdvance) for entry in entries]

def _ToBitmapFontKernings(entries: List[AngleFontKerning]) -> List[BitmapFontKerning]:
    return [BitmapFontKerning(entry.First, entry.Second, entry.Amount) for entry in entries]


def ToBitmapFont(font: AngleFont, fontType: BitmapFontType) -> BitmapFont:
    if len(font.Pages) != 1:
        raise Exception("BitmapFont only support one page, not {} pages".format(len(font.Pages)))
    if font.Common.Packed:
        raise Exception("BitmapFont does not support font packing (common.Packed=true)")

    chars = _ToBitmapFontChars(font.Chars)
    kernings = _ToBitmapFontKernings(font.Kernings)
    return BitmapFont(font.Info.Face, font.Info.Size, font.Common.LineHeight, font.Common.Base, font.Pages[0].File, fontType, chars, kernings)


def DecodeAsBitmapFont(content: str, fontType: BitmapFontType) -> BitmapFont:
    font = Decode(content)
    return ToBitmapFont(font, fontType)

