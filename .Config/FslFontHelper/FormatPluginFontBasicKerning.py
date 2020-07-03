#!/usr/bin/env python3
#****************************************************************************************************************************************************
# Copyright (c) 2015 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
from FslFontHelper import IOUtil
from FslFontHelper import FormatPluginFontBasicKerningUtil
from FslFontHelper.BasicFont import BasicFont
from FslFontHelper.BasicFont import BasicFontGlyphKerning
from FslFontHelper.BasicFont import BasicFontGlyphRange
from FslFontHelper.FormatPlugin import FormatPlugin
from FslFontHelper.FormatPlugin import FormatPluginType

class FormatPluginFontBasicKerning(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("fbk", FormatPluginType.BasicFont)

    def Process(self, basicFont: BasicFont, outputFilename: str) -> None:

        dst = [] # type: List[int]
        offsetSize = self.__WriteHeader(dst)
        sizeOfHeader = len(dst)

        self.__WriteRanges(dst, basicFont.Ranges)
        self.__WriteRangeGlyphKernings(dst, basicFont.BasicGlyphKerning)
        self.__WriteDescription(dst, basicFont)
        FormatPluginFontBasicKerningUtil.AddString(dst, basicFont.Name)
        FormatPluginFontBasicKerningUtil.AddString(dst, basicFont.Header.PathName)

        # Write the number of bytes that were written to the extended header
        bytesWritten = len(dst) - sizeOfHeader
        FormatPluginFontBasicKerningUtil.SetUInt32(dst, offsetSize, bytesWritten)

        content = bytearray(dst)
        finalOutputFilename = '{}.{}'.format(outputFilename, 'fbk')
        IOUtil.WriteBinaryFileIfChanged(finalOutputFilename, content)


    def __WriteHeader(self, dst: List[int]) -> int:
        FormatPluginFontBasicKerningUtil.AddUInt32(dst, 0x004B4246)
        FormatPluginFontBasicKerningUtil.AddUInt32(dst, 1)
        offset = len(dst)
        FormatPluginFontBasicKerningUtil.AddUInt32(dst, 0)
        return offset


    def __WriteDescription(self, dst: List[int], basicFont: BasicFont) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, basicFont.Header.LineSpacing)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, basicFont.Header.BaseLine)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, basicFont.Header.MaxGlyphLeadingOverdrawArea.X)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, basicFont.Header.MaxGlyphLeadingOverdrawArea.Y)


    def __WriteRangeGlyphKernings(self, dst: List[int], kernings: List[BasicFontGlyphKerning]) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, len(kernings))
        for kerning in kernings:
            self.__WriteGlyphKerning(dst, kerning)


    def __WriteGlyphKerning(self, dst: List[int], kerning: BasicFontGlyphKerning) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedInt32(dst, kerning.OffsetX)
        FormatPluginFontBasicKerningUtil.AddEncodedInt32(dst, kerning.OffsetY)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, kerning.LayoutWidth)


    def __WriteRanges(self, dst: List[int], ranges: List[BasicFontGlyphRange]) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, len(ranges))
        for range in ranges:
            self.__WriteRange(dst, range)


    def __WriteRange(self, dst: List[int], range: BasicFontGlyphRange) -> None:
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, range.From)
        FormatPluginFontBasicKerningUtil.AddEncodedUInt32(dst, range.Length)

