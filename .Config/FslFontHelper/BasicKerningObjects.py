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

from FslFontHelper import IOUtil
import os

def ExtractRanges(entries):
    list = []
    for entry in entries:
        list.append(BasicGlyphRange(entry))
    return list


def ExtractBasicGlyphKerning(entries):
    list = []
    for entry in entries:
        list.append(BasicGlyphKerning(entry))
    return list

class BasicPoint2(object):
    def __init__(self, dict):
        super(BasicPoint2, self).__init__()
        self.X = dict["X"]
        self.Y = dict["Y"]


class BasicKerningHeader(object):
    def __init__(self, dict):
        super(BasicKerningHeader, self).__init__()

        if not "App" in dict or dict["App"] != "SpriteFontCreator2":
            raise Exception("Unknown json format")
        if not "Version" in dict or dict["Version"] != "1.0":
            raise Exception("Unknown json format")
        self.PathName = dict["PathName"]
        self.LineSpacing = dict["LineSpacing"]
        self.BaseLine = dict["BaseLine"]
        self.MaxGlyphLeadingOverdrawArea = BasicPoint2(dict["MaxGlyphLeadingOverdrawArea"])


class BasicGlyphRange(object):
    def __init__(self, dict):
        super(BasicGlyphRange, self).__init__()
        self.From = dict["From"]
        self.Length = dict["Length"]
        self.Offset = dict["Offset"]


class BasicGlyphKerning(object):
    def __init__(self, dict):
        super(BasicGlyphKerning, self).__init__()
        self.Id = dict["Id"]
        self.OffsetX = dict["OffsetX"]
        self.OffsetY = dict["OffsetY"]
        self.LayoutWidth = dict["LayoutWidth"]


class BasicKerning(object):
    def __init__(self, dict, name):
        super(BasicKerning, self).__init__()

        if not "BasicGlyphKerning" in dict or not "Ranges" in dict or not "Meta" in dict:
            raise Exception("Unknown json format")

        self.Name = name
        self.Header = BasicKerningHeader(dict["Meta"]);
        self.Ranges = ExtractRanges(dict["Ranges"]);
        self.BasicGlyphKerning = ExtractBasicGlyphKerning(dict["BasicGlyphKerning"]);


