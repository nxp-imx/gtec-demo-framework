#!/usr/bin/env python
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

from FslFontHelper.FormatPlugin import FormatPlugin
from FslFontHelper import IOUtil
from FslFontHelper.FormatPluginFontBasicKerningUtil import *

class FormatPluginFontBasicKerning(FormatPlugin):
    def __init__(self):
        super(FormatPluginFontBasicKerning, self).__init__("fbk")

    def Process(self, basicKerning, outputFilename):

        list = []
        offsetSize = self.__WriteHeader(list)
        sizeOfHeader = len(list)

        self.__WriteRanges(list, basicKerning.Ranges)
        self.__WriteRangeGlyphKernings(list, basicKerning.BasicGlyphKerning)
        self.__WriteDescription(list, basicKerning)
        AddString(list, basicKerning.Name)
        AddString(list, basicKerning.Header.PathName)

        # Write the number of bytes that were written to the extended header
        bytesWritten = len(list) - sizeOfHeader
        SetUInt32(list, offsetSize, bytesWritten)

        content = bytearray(list)
        finalOutputFilename = '%s.%s' % (outputFilename, 'fbk')
        IOUtil.WriteBinaryFileIfChanged(finalOutputFilename, content)


    def __WriteHeader(self, list):
        AddUInt32(list, 0x004B4246)
        AddUInt32(list, 1)
        offset = len(list)
        AddUInt32(list, 0)
        return offset


    def __WriteDescription(self, list, basicKerning):
        AddEncodedUInt32(list, basicKerning.Header.LineSpacing)
        AddEncodedUInt32(list, basicKerning.Header.BaseLine)
        AddEncodedUInt32(list, basicKerning.Header.MaxGlyphLeadingOverdrawArea.X)
        AddEncodedUInt32(list, basicKerning.Header.MaxGlyphLeadingOverdrawArea.Y)


    def __WriteRangeGlyphKernings(self, list, kernings):
        AddEncodedUInt32(list, len(kernings))
        for kerning in kernings:
            self.__WriteGlyphKerning(list, kerning)


    def __WriteGlyphKerning(self, list, kerning):
        AddEncodedInt32(list, kerning.OffsetX)
        AddEncodedInt32(list, kerning.OffsetY)
        AddEncodedUInt32(list, kerning.LayoutWidth)


    def __WriteRanges(self, list, ranges):
        AddEncodedUInt32(list, len(ranges))
        for range in ranges:
            self.__WriteRange(list, range)


    def __WriteRange(self, list, range):
        AddEncodedUInt32(list, range.From)
        AddEncodedUInt32(list, range.Length)

