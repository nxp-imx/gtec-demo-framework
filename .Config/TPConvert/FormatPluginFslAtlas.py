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
from TPConvert.FormatPlugin import FormatPlugin
from TPConvert.TexturePackerObjects import TexturePackerAtlas
from TPConvert import IOUtil

class FormatPluginFslAtlas(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("fsl")

    def Process(self, atlas: TexturePackerAtlas, outputFilename: str) -> None:
        hppOutputFilename = '{0}.{1}'.format(outputFilename, 'hpp')
        cppOutputFilename = '{0}.{1}'.format(outputFilename, 'cpp')

        className = IOUtil.GetFileNameWithoutExtension(outputFilename)

        hppContent = self.__BuildHPPContent(atlas, className)
        cppContent = self.__BuildCPPContent(atlas, className)
        IOUtil.WriteFileIfChanged(hppOutputFilename, hppContent)
        IOUtil.WriteFileIfChanged(cppOutputFilename, cppContent)


    def __BuildHPPContent(self, atlas: TexturePackerAtlas, className: str) -> str:
        defineName = className.upper()

        dstList = []
        dstList.append('#ifndef FSL_%s_HPP' % (defineName))
        dstList.append('#define FSL_%s_HPP' % (defineName))
        self.__AppendHeader(dstList)
        dstList.append('')
        dstList.append('#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>')
        dstList.append('')
        dstList.append('namespace Fsl')
        dstList.append('{')
        dstList.append('  class %s : public ITextureAtlas' % (className))
        dstList.append('  {')
        dstList.append('  public:')
        dstList.append('    virtual std::string GetName() const;')
        dstList.append('    virtual int32_t Count() const;')
        dstList.append('    virtual NamedAtlasTexture GetEntry(const int32_t index) const;')
        dstList.append('  };')
        dstList.append('}')
        dstList.append('#endif')
        dstList.append('')
        return '\n'.join(dstList)


    def __BuildCPPContent(self, atlas: TexturePackerAtlas, className: str) -> str:
        dstList = [] # type: List[str]

        self.__AppendHeader(dstList)
        dstList.append('')
        dstList.append('#include <%s.hpp>' % className)
        dstList.append('#include <cassert>')
        dstList.append('')
        dstList.append('namespace Fsl')
        dstList.append('{')
        dstList.append('  namespace')
        dstList.append('  {')
        dstList.append('    NamedAtlasTexture g_entries[] =')
        dstList.append('    {')

        for entry in atlas.Entries:
            rectX = entry.Frame.X - entry.SpriteSourceSize.X
            rectY = entry.Frame.Y - entry.SpriteSourceSize.Y
            rectWidth = entry.SourceSize.Width
            rectHeight = entry.SourceSize.Height
            dstList.append('      NamedAtlasTexture("%s", AtlasTextureInfo(Rectangle(%s, %s, %s, %s), Rectangle(%s, %s, %s, %s))),' % (entry.FullFilenameWithoutExt, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height))

        dstList.append('    };')
        dstList.append('  }')
        dstList.append('')
        dstList.append('')
        dstList.append('  std::string %s::GetName() const' % (className))
        dstList.append('  {')
        dstList.append('    return std::string("%s");' % (atlas.Name))
        dstList.append('  }')
        dstList.append('')
        dstList.append('')
        dstList.append('  int32_t %s::Count() const' % (className))
        dstList.append('  {')
        dstList.append('    return %s;' % (len(atlas.Entries)))
        dstList.append('  }')
        dstList.append('')
        dstList.append('')
        dstList.append('  NamedAtlasTexture %s::GetEntry(const int32_t index) const' % (className))
        dstList.append('  {')
        dstList.append('    assert(index >= 0 && index <= Count());')
        dstList.append('    return g_entries[index];')
        dstList.append('  }')
        dstList.append('}')
        dstList.append('')
        return '\n'.join(dstList)


    def __AppendHeader(self, dstList: List[str]) -> None:
        dstList.append('/****************************************************************************************************************************************************')
        dstList.append(' * Copyright (c) 2015 Freescale Semiconductor, Inc.')
        dstList.append(' * All rights reserved.')
        dstList.append(' *')
        dstList.append(' * Redistribution and use in source and binary forms, with or without')
        dstList.append(' * modification, are permitted provided that the following conditions are met:')
        dstList.append(' *')
        dstList.append(' *    * Redistributions of source code must retain the above copyright notice,')
        dstList.append(' *      this dstList of conditions and the following disclaimer.')
        dstList.append(' *')
        dstList.append(' *    * Redistributions in binary form must reproduce the above copyright notice,')
        dstList.append(' *      this dstList of conditions and the following disclaimer in the documentation')
        dstList.append(' *      and/or other materials provided with the distribution.')
        dstList.append(' *')
        dstList.append(' *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of')
        dstList.append(' *      its contributors may be used to endorse or promote products derived from')
        dstList.append(' *      this software without specific prior written permission.')
        dstList.append(' *')
        dstList.append(' * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND')
        dstList.append(' * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED')
        dstList.append(' * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.')
        dstList.append(' * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,')
        dstList.append(' * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,')
        dstList.append(' * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,')
        dstList.append(' * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF')
        dstList.append(' * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE')
        dstList.append(' * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF')
        dstList.append(' * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.')
        dstList.append(' *')
        dstList.append(' ****************************************************************************************************************************************************/')
