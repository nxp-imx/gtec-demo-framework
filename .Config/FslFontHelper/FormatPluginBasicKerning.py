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
from FslFontHelper.BasicFont import BasicFont
from FslFontHelper.FormatPlugin import FormatPlugin
from FslFontHelper.FormatPlugin import FormatPluginType

class FormatPluginBasicKerning(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("c++", FormatPluginType.BasicFont)

    def Process(self, basicFont: BasicFont, outputFilename: str) -> None:
        hppOutputFilename = '{}.{}'.format(outputFilename, 'hpp')
        cppOutputFilename = '{}.{}'.format(outputFilename, 'cpp')

        className = IOUtil.GetFileNameWithoutExtension(outputFilename)

        hppContent = self.__BuildHPPContent(basicFont, className)
        cppContent = self.__BuildCPPContent(basicFont, className)
        IOUtil.WriteFileIfChanged(hppOutputFilename, hppContent)
        IOUtil.WriteFileIfChanged(cppOutputFilename, cppContent)


    def __BuildHPPContent(self, basicFont: BasicFont, className: str) -> str:
        defineName = className.upper()

        dst = []
        dst.append('#ifndef FSL_{}_HPP'.format(defineName))
        dst.append('#define FSL_{}_HPP'.format(defineName))
        self.__AppendHeader(dst)
        dst.append('')
        dst.append('#include <FslGraphics/Font/IFontBasicKerning.hpp>')
        dst.append('')
        dst.append('namespace Fsl')
        dst.append('{')
        dst.append('  class {} : public IFontBasicKerning'.format(className))
        dst.append('  {')
        dst.append('  public:')
        dst.append('    virtual UTF8String GetName() const;')
        dst.append('    virtual UTF8String GetPathName() const;')
        dst.append('    virtual FontDesc GetDesc() const;')
        dst.append('    virtual int32_t RangeCount() const;')
        dst.append('    virtual FontGlyphRange GetRange(const int32_t index) const;')
        dst.append('    virtual int32_t Count() const;')
        dst.append('    virtual FontGlyphBasicKerning Get(const int32_t index) const;')
        dst.append('  };')
        dst.append('}')
        dst.append('#endif')
        dst.append('')
        return '\n'.join(dst)


    def __BuildCPPContent(self, basicFont: BasicFont, className: str) -> str:
        dst = [] # type: List[str]

        self.__AppendHeader(dst)
        dst.append('')
        dst.append('#include <{}.hpp>'.format(className))
        dst.append('#include <cassert>')
        dst.append('')
        dst.append('namespace Fsl')
        dst.append('{')
        dst.append('  namespace')
        dst.append('  {')
        dst.append('    FontGlyphRange g_ranges[] =')
        dst.append('    {')

        for entry in basicFont.Ranges:
            dst.append('      FontGlyphRange({}, {}),'.format(entry.From, entry.Length))

        dst.append('    };')
        dst.append('')
        dst.append('')
        dst.append('    FontGlyphBasicKerning g_glyphKerning[] =')
        dst.append('    {')

        for entry2 in basicFont.BasicGlyphKerning:
            dst.append('      FontGlyphBasicKerning({}, {}, {}),'.format(entry2.OffsetX, entry2.OffsetY, entry2.LayoutWidth))

        dst.append('    };')

        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  UTF8String {}::GetName() const'.format(className))
        dst.append('  {')
        dst.append('    return UTF8String("{}");'.format(basicFont.Name))
        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  UTF8String {}::GetPathName() const'.format(className))
        dst.append('  {')
        dst.append('    return UTF8String("{}");'.format(basicFont.Header.PathName))
        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  FontDesc {}::GetDesc() const'.format(className))
        dst.append('  {')
        dst.append('    return FontDesc({}, {}, Point2({}, {}));'.format(basicFont.Header.LineSpacing, basicFont.Header.BaseLine, basicFont.Header.MaxGlyphLeadingOverdrawArea.X, basicFont.Header.MaxGlyphLeadingOverdrawArea.Y))
        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  int32_t {}::RangeCount() const'.format(className))
        dst.append('  {')
        dst.append('    return sizeof(g_ranges)/sizeof(FontGlyphRange);')
        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  FontGlyphRange {}::GetRange(const int32_t index) const'.format(className))
        dst.append('  {')
        dst.append('    assert(index >= 0 && index <= RangeCount());')
        dst.append('    return g_ranges[index];')
        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  int32_t {}::Count() const'.format(className))
        dst.append('  {')
        dst.append('    return sizeof(g_glyphKerning)/sizeof(FontGlyphBasicKerning);')
        dst.append('  }')
        dst.append('')
        dst.append('')
        dst.append('  FontGlyphBasicKerning {}::Get(const int32_t index) const'.format(className))
        dst.append('  {')
        dst.append('    assert(index >= 0 && index <= Count());')
        dst.append('    return g_glyphKerning[index];')
        dst.append('  }')
        dst.append('}')
        dst.append('')
        return '\n'.join(dst)


    def __AppendHeader(self, dst: List[str]) -> None:
        dst.append('/****************************************************************************************************************************************************')
        dst.append(' * Copyright 2020 NXP')
        dst.append(' * All rights reserved.')
        dst.append(' *')
        dst.append(' * Redistribution and use in source and binary forms, with or without')
        dst.append(' * modification, are permitted provided that the following conditions are met:')
        dst.append(' *')
        dst.append(' *    * Redistributions of source code must retain the above copyright notice,')
        dst.append(' *      this list of conditions and the following disclaimer.')
        dst.append(' *')
        dst.append(' *    * Redistributions in binary form must reproduce the above copyright notice,')
        dst.append(' *      this list of conditions and the following disclaimer in the documentation')
        dst.append(' *      and/or other materials provided with the distribution.')
        dst.append(' *')
        dst.append(' *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of')
        dst.append(' *      its contributors may be used to endorse or promote products derived from')
        dst.append(' *      this software without specific prior written permission.')
        dst.append(' *')
        dst.append(' * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND')
        dst.append(' * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED')
        dst.append(' * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.')
        dst.append(' * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,')
        dst.append(' * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,')
        dst.append(' * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,')
        dst.append(' * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF')
        dst.append(' * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE')
        dst.append(' * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF')
        dst.append(' * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.')
        dst.append(' *')
        dst.append(' ****************************************************************************************************************************************************/')
