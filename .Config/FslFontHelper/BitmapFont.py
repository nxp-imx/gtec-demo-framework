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
from enum import IntEnum

class BitmapFontChar(object):
    def __init__(self, id: int, x: int, y: int, widthPx: int, heightPx: int, xOffsetPx: int, yOffsetPx: int, xAdvancePx: int) -> None:
        if id < 0:
            raise Exception("BitmapFontChar id can not be negative {}".format(id))
        if x < 0:
            raise Exception("BitmapFontChar id={} x can not be negative {}".format(id, x))
        if y < 0:
            raise Exception("BitmapFontChar id={} y can not be negative {}".format(id, y))
        if widthPx < 0:
            raise Exception("BitmapFontChar id={} widthPx can not be negative {}".format(id, widthPx))
        if heightPx < 0:
            raise Exception("BitmapFontChar id={} heightPx can not be negative {}".format(id, heightPx))

        self.Id = id                        # The character id.
        self.X = x                          # The left position of the character image in the texture.
        self.Y = y                          # The top position of the character image in the texture.
        self.WidthPx = widthPx                # The width of the character image in the texture.
        self.HeightPx = heightPx              # The height of the character image in the texture.
        self.XOffsetPx = xOffsetPx            # How much the current position should be offset when copying the image from the texture to the screen.
        self.YOffsetPx = yOffsetPx            # How much the current position should be offset when copying the image from the texture to the screen.
        self.XAdvancePx = xAdvancePx        # How much the current position should be advanced after drawing the character.

class BitmapFontKerning(object):
    def __init__(self, first: int, second: int, amountPx: int) -> None:
        if first < 0:
            raise Exception("BitmapFontKerning first can not be negative {}".format(first))
        if second < 0:
            raise Exception("BitmapFontKerning second can not be negative {}".format(second))

        self.First = first                  # The first character id.
        self.Second = second                # The second character id.
        self.AmountPx = amountPx            # How much the x position should be adjusted when drawing the second character immediately following the first.


class BitmapFontType(IntEnum):
    Bitmap = 0
    SDF = 1

    @staticmethod
    def FromString(value: str) -> "BitmapFontType":
        if value == "bitmap":
            return BitmapFontType.Bitmap
        elif value == "sdf":
            return BitmapFontType.SDF
        raise Exception("Unknown '{0}' font type. Valid types: bitmap, sdf".format(value))


class BitmapFont(object):
    def __init__(self, name: str, size: int, lineSpacingPx: int, baseLinePx: int, textureName: str, fontType: BitmapFontType, chars: List[BitmapFontChar],
                 kernings: List[BitmapFontKerning]) -> None:
        if size < 0:
            raise Exception("BitmapFont Size can not be negative {}".format(size))
        if lineSpacingPx < 0:
            raise Exception("BitmapFont lineSpacingPx can not be negative {}".format(lineSpacingPx))
        if baseLinePx < 0:
            raise Exception("BitmapFont baseLinePx can not be negative {}".format(baseLinePx))

        self.Name = name                    # This is the name of the true type font.
        self.Size = size                    # The size of the true type font.
        self.LineSpacingPx = lineSpacingPx  # This is the distance in pixels between each line of text.
        self.BaseLinePx = baseLinePx        # The number of pixels from the absolute top of the line to the base of the characters.
        self.TextureName = textureName      # The name of the texture
        self.FontType = fontType            # The type of the font
        self.Chars = chars
        self.Kernings = kernings
