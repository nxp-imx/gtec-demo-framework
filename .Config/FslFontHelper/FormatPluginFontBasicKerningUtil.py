#!/usr/bin/env python3
#****************************************************************************************************************************************************
#* BSD 3-Clause License
#*
#* Copyright (c) 2015, Mana Battery
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
from FslFontHelper.BitmapFont import BitmapFontType

def AddHeader(dst: List[int], version: int) -> None:
    # Magic
    # Version
    AddUInt32(dst, 0x00415442)
    AddUInt32(dst, version)


def AddByteToList(dst: List[int], value: int) -> None:
    if( value < 0 or value > 255 ):
        raise Exception("Invalid value")
    dst.append(value & 0xFF)


def SetByteInList(dst: List[int], index: int, value: int) -> None:
    if( value < 0 or value > 255 ):
        raise Exception("Invalid value")
    dst[index] = value & 0xFF


def SetUInt32(dst: List[int], index: int, value: int) -> None:
    SetByteInList(dst, index, value & 0xFF)
    SetByteInList(dst, index + 1, (value & 0xFF00) >> 8)
    SetByteInList(dst, index + 2, (value & 0xFF0000) >> 16)
    SetByteInList(dst, index + 3, (value & 0xFF000000) >> 24)

def AddUInt32(dst: List[int], value: int) -> None:
    AddByteToList(dst, value & 0xFF)
    AddByteToList(dst, (value & 0xFF00) >> 8)
    AddByteToList(dst, (value & 0xFF0000) >> 16)
    AddByteToList(dst, (value & 0xFF000000) >> 24)


def AddInt32(dst: List[int], value: int) -> None:
    AddByteToList(dst, value & 0xFF)
    AddByteToList(dst, (value & 0xFF00) >> 8)
    AddByteToList(dst, (value & 0xFF0000) >> 16)
    AddByteToList(dst, (value & 0xFF000000) >> 24)

def AddEncodedInt32(dst: List[int], value: int) -> int:
    # ZigZag encode signed numbers
   if value >= 0:
        return AddEncodedUInt32(dst, value << 1);
   else:
        return AddEncodedUInt32(dst, (value << 1) ^ (~0))

# Encodes a integer into a variable length encoding where the length can be determined from the first byte.
# in a way that favors small values.
def AddEncodedUInt16(dst: List[int], value: int) -> int:
    if value < 0 or value >= 0x10000:
        raise Exception("value out of bounds {0}".format(value))
    if (value <= 0x7F):
        # <=7 bits value
        AddByteToList(dst, value)
        return 1
    elif (value <= 0x3FFF):
        # <=14 bits value
        AddByteToList(dst, 0x80 | (value & 0x3F))
        AddByteToList(dst, (value & 0x3FC0) >> 6)
        return 2
    else:
        # <=21 bits value
        AddByteToList(dst, 0xC0 | (value & 0x1F))
        AddByteToList(dst, (value & 0x001FE0) >> 5)
        AddByteToList(dst, (value & 0x1FE000) >> 13)
        return 3


# Encodes a integer into a variable length encoding where the length can be determined from the first byte.
# in a way that favors small values.
def AddEncodedUInt32(dst: List[int], value: int) -> int:
    if value < 0:
        raise Exception("value out of bounds {0}".format(value))
    if (value <= 0x7F):
        # <=7 bits value
        AddByteToList(dst, value)
        return 1
    elif (value <= 0x3FFF):
        # <=14 bits value
        AddByteToList(dst, 0x80 | (value & 0x3F))
        AddByteToList(dst, (value & 0x3FC0) >> 6)
        return 2
    elif (value <= 0x1FFFFF):
        # <=21 bits value
        AddByteToList(dst, 0xC0 | (value & 0x1F))
        AddByteToList(dst, (value & 0x001FE0) >> 5)
        AddByteToList(dst, (value & 0x1FE000) >> 13)
        return 3
    elif (value <= 0xFFFFFFF):
        # <=28 bits value
        AddByteToList(dst, 0xE0 | (value & 0x0F))
        AddByteToList(dst, (value & 0x00000FF0) >> 4)
        AddByteToList(dst, (value & 0x000FF000) >> 12)
        AddByteToList(dst, (value & 0x0FF00000) >> 20)
        return 4
    else:
        # >28 bits value
        AddByteToList(dst, 0xF0 | (value & 0x07))
        AddByteToList(dst, (value & 0x000007F8) >> 3)
        AddByteToList(dst, (value & 0x0007F800) >> 11)
        AddByteToList(dst, (value & 0x07F80000) >> 19)
        AddByteToList(dst, (value & 0xF8000000) >> 27)
        return 5;


def AddEncodedBitmapFontType(dst: List[int], value: BitmapFontType) -> int:
     return AddEncodedUInt32(dst, value)

def AddString(dst: List[int], value: str) -> None:
    val = bytearray(value.encode('utf-8'))
    AddEncodedUInt32(dst, len(val))
    for entry in val:
        AddByteToList(dst, entry)

