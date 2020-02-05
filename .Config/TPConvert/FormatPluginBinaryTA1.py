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
from TPConvert import IOUtil
from TPConvert.FormatPlugin import FormatPlugin
from TPConvert.TexturePackerObjects import TexturePackerAtlas
from TPConvert.FormatPluginBinaryTAUtil import *


class FormatPluginBinaryTA1(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("bta1")

    def Process(self, atlas: TexturePackerAtlas, outputFilename: str) -> None:
        dstList = [] # type: List[int]
        AddHeader(dstList, 1);
        # make room for a number of bytes written entry and store the offset where it was written so we can pacth it later
        offset = len(dstList)
        AddUInt32(dstList, 0);

        AddEncodedUInt32(dstList, len(atlas.Entries))
        for entry in atlas.Entries:
            rectX = entry.Frame.X - entry.SpriteSourceSize.X
            rectY = entry.Frame.Y - entry.SpriteSourceSize.Y
            rectWidth = entry.SourceSize.Width
            rectHeight = entry.SourceSize.Height
            self.__AddEntry(dstList, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height, entry.FullFilenameWithoutExt)

        # Write the number of bytes that were written to the extended header
        # -4 because we dont count the 'size' entry
        bytesWritten = len(dstList) - offset - 4
        SetUInt32(dstList, offset, bytesWritten)


        content = bytearray(dstList)
        outputFilename = '%s.%s' % (outputFilename, 'bta')
        IOUtil.WriteBinaryFileIfChanged(outputFilename, content)


    def __AddEntry(self, dstList: List[int], srcRectX: int, srcRectY: int, srcRectWidth: int, srcRectHeight: int,
                   trimmedRectX: int, trimmedRectY: int, trimmedRectWidth: int, trimmedRectHeight: int, name: str) ->None:
        AddRectangle(dstList, srcRectX, srcRectY, srcRectWidth, srcRectHeight)
        AddRectangle(dstList, trimmedRectX, trimmedRectY, trimmedRectWidth, trimmedRectHeight)
        AddString(dstList, name)

