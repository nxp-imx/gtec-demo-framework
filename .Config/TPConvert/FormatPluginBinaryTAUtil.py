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

def AddHeader(entries: List[int], version: int) -> None:
    # Magic
    # Version
    AddUInt32(entries, 0x00415442)
    AddUInt32(entries, version)


def AddByteToList(entries: List[int], value: int) -> None:
    if( value < 0 or value > 255 ):
        raise Exception("Invalid value")
    entries.append(value & 0xFF)


def SetByteInList(entries: List[int], index: int, value: int) -> None:
    if( value < 0 or value > 255 ):
        raise Exception("Invalid value")
    entries[index] = value & 0xFF


def SetUInt32(entries: List[int], index: int, value: int) -> None:
    SetByteInList(entries, index, value & 0xFF)
    SetByteInList(entries, index + 1, (value & 0xFF00) >> 8)
    SetByteInList(entries, index + 2, (value & 0xFF0000) >> 16)
    SetByteInList(entries, index + 3, (value & 0xFF000000) >> 24)


def AddUInt32(entries: List[int], value: int) -> None:
    AddByteToList(entries, value & 0xFF)
    AddByteToList(entries, (value & 0xFF00) >> 8)
    AddByteToList(entries, (value & 0xFF0000) >> 16)
    AddByteToList(entries, (value & 0xFF000000) >> 24)


def AddInt32(entries: List[int], value: int) -> None:
    AddByteToList(entries, value & 0xFF)
    AddByteToList(entries, (value & 0xFF00) >> 8)
    AddByteToList(entries, (value & 0xFF0000) >> 16)
    AddByteToList(entries, (value & 0xFF000000) >> 24)

def AddEncodedInt32(entries: List[int], value: int) -> int:
    # ZigZag encode signed numbers
    # While this "(value << 1) ^ (value >> 31)" might be more optimal its very dependent on how the language handles sign extension when downshifting
    # so a 'safe' implementation was chosen
   if value >= 0:
        return AddEncodedUInt32(entries, value << 1);
   else:
        return AddEncodedUInt32(entries, (value << 1) ^ (~0))

# Encodes a integer into a variable length encoding where the length can be determined from the first byte.
# in a way that favors small values.
def AddEncodedUInt32(entries: List[int], value: int) -> int:
    if (value <= 0x7F):
        # <=7 bits value
        AddByteToList(entries, value)
        return 1
    elif (value <= 0x3FFF):
        # <=14 bits value
        AddByteToList(entries, 0x80 | (value & 0x3F))
        AddByteToList(entries, (value & 0x3FC0) >> 6)
        return 2
    elif (value <= 0x1FFFFF):
        # <=21 bits value
        AddByteToList(entries, 0xC0 | (value & 0x1F))
        AddByteToList(entries, (value & 0x001FE0) >> 5)
        AddByteToList(entries, (value & 0x1FE000) >> 13)
        return 3
    elif (value <= 0xFFFFFFF):
        # <=28 bits value
        AddByteToList(entries, 0xE0 | (value & 0x0F))
        AddByteToList(entries, (value & 0x00000FF0) >> 4)
        AddByteToList(entries, (value & 0x000FF000) >> 12)
        AddByteToList(entries, (value & 0x0FF00000) >> 20)
        return 4
    else:
        # >28 bits value
        AddByteToList(entries, 0xF0 | (value & 0x07))
        AddByteToList(entries, (value & 0x000007F8) >> 3)
        AddByteToList(entries, (value & 0x0007F800) >> 11)
        AddByteToList(entries, (value & 0x07F80000) >> 19)
        AddByteToList(entries, (value & 0xF8000000) >> 27)
        return 5;


def AddString(entries: List[int], value: str) -> None:
    encodedValue = bytearray(value.encode('utf-8'))
    AddEncodedUInt32(entries, len(encodedValue))
    for entry in encodedValue:
        AddByteToList(entries, entry)


def AddRectangle(entries: List[int], valueX: int, valueY: int, valueWidth: int, valueHeight: int) -> None:
    if( valueWidth < 0 or valueHeight < 0):
        raise Exception("Unsupported rect")
    AddEncodedInt32(entries, valueX)
    AddEncodedInt32(entries, valueY)
    AddEncodedUInt32(entries, valueWidth)
    AddEncodedUInt32(entries, valueHeight)
