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

def AddHeader(list, version):
    # Magic
    # Version
    AddUInt32(list, 0x00415442)
    AddUInt32(list, version)


def AddByteToList(list, value):
    if( value < 0 or value > 255 ):
        raise Exception("Invalid value")
    list.append(value & 0xFF)


def SetByteInList(list, index, value):
    if( value < 0 or value > 255 ):
        raise Exception("Invalid value")
    list[index] = value & 0xFF


def SetUInt32(list, index, value):
    SetByteInList(list, index, value & 0xFF)
    SetByteInList(list, index + 1, (value & 0xFF00) >> 8)
    SetByteInList(list, index + 2, (value & 0xFF0000) >> 16)
    SetByteInList(list, index + 3, (value & 0xFF000000) >> 24)


def AddUInt32(list, value):
    AddByteToList(list, value & 0xFF)
    AddByteToList(list, (value & 0xFF00) >> 8)
    AddByteToList(list, (value & 0xFF0000) >> 16)
    AddByteToList(list, (value & 0xFF000000) >> 24)


def AddInt32(list, value):
    AddByteToList(list, value & 0xFF)
    AddByteToList(list, (value & 0xFF00) >> 8)
    AddByteToList(list, (value & 0xFF0000) >> 16)
    AddByteToList(list, (value & 0xFF000000) >> 24)

def AddEncodedInt32(list, value):
    # ZigZag encode signed numbers
    # While this "(value << 1) ^ (value >> 31)" might be more optimal its very dependent on how the language handles sign extension when downshifting
    # so a 'safe' implementation was chosen
   if value >= 0:
        return AddEncodedUInt32(list, value << 1);
   else:
        return AddEncodedUInt32(list, (value << 1) ^ (~0))

# Encodes a integer into a variable length encoding where the length can be determined from the first byte.
# in a way that favors small values. 
def AddEncodedUInt32(list, value):
    if (value <= 0x7F):
        # <=7 bits value
        AddByteToList(list, value)
        return 1
    elif (value <= 0x3FFF):
        # <=14 bits value
        AddByteToList(list, 0x80 | (value & 0x3F))
        AddByteToList(list, (value & 0x3FC0) >> 6)
        return 2
    elif (value <= 0x1FFFFF):
        # <=21 bits value
        AddByteToList(list, 0xC0 | (value & 0x1F))
        AddByteToList(list, (value & 0x001FE0) >> 5)
        AddByteToList(list, (value & 0x1FE000) >> 13)
        return 3
    elif (value <= 0xFFFFFFF):
        # <=28 bits value
        AddByteToList(list, 0xE0 | (value & 0x0F))
        AddByteToList(list, (value & 0x00000FF0) >> 4)
        AddByteToList(list, (value & 0x000FF000) >> 12)
        AddByteToList(list, (value & 0x0FF00000) >> 20)
        return 4
    else:
        # >28 bits value
        AddByteToList(list, 0xF0 | (value & 0x07))
        AddByteToList(list, (value & 0x000007F8) >> 3)
        AddByteToList(list, (value & 0x0007F800) >> 11)
        AddByteToList(list, (value & 0x07F80000) >> 19)
        AddByteToList(list, (value & 0xF8000000) >> 27)
        return 5;


def AddString(list, value):
    value = bytearray(value.encode('utf-8'))
    AddEncodedUInt32(list, len(value))
    for entry in value:
        AddByteToList(list, entry)


def AddRectangle(list, valueX, valueY, valueWidth, valueHeight):
    if( valueWidth < 0 or valueHeight < 0):
        raise Exception("Unsupported rect")
    AddEncodedInt32(list, valueX)
    AddEncodedInt32(list, valueY)
    AddEncodedUInt32(list, valueWidth)
    AddEncodedUInt32(list, valueHeight)
