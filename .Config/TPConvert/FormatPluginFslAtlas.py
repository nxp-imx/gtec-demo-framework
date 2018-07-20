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

from TPConvert.FormatPlugin import FormatPlugin
from TPConvert import IOUtil

class FormatPluginFslAtlas(FormatPlugin):
    def __init__(self):
        super(FormatPluginFslAtlas, self).__init__("fsl")

    def Process(self, atlas, outputFilename):
        hppOutputFilename = '{0}.{1}'.format(outputFilename, 'hpp')
        cppOutputFilename = '{0}.{1}'.format(outputFilename, 'cpp')

        className = IOUtil.GetFileNameWithoutExtension(outputFilename)

        hppContent = self.__BuildHPPContent(atlas, className)
        cppContent = self.__BuildCPPContent(atlas, className)
        IOUtil.WriteFileIfChanged(hppOutputFilename, hppContent)
        IOUtil.WriteFileIfChanged(cppOutputFilename, cppContent)


    def __BuildHPPContent(self, atlas, className):
        defineName = className.upper()

        list = []
        list.append('#ifndef FSL_%s_HPP' % (defineName))
        list.append('#define FSL_%s_HPP' % (defineName))
        self.__AppendHeader(list)
        list.append('')
        list.append('#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>')
        list.append('')
        list.append('namespace Fsl')
        list.append('{')
        list.append('  class %s : public ITextureAtlas' % (className))
        list.append('  {')
        list.append('  public:')
        list.append('    virtual std::string GetName() const;')
        list.append('    virtual int32_t Count() const;')
        list.append('    virtual NamedAtlasTexture GetEntry(const int32_t index) const;')
        list.append('  };')
        list.append('}')
        list.append('#endif')
        list.append('')
        return '\n'.join(list)


    def __BuildCPPContent(self, atlas, className):
        list = []

        self.__AppendHeader(list)
        list.append('')
        list.append('#include <%s.hpp>' % className)
        list.append('#include <cassert>')
        list.append('')
        list.append('namespace Fsl')
        list.append('{')
        list.append('  namespace')
        list.append('  {')
        list.append('    NamedAtlasTexture g_entries[] =')
        list.append('    {')

        for entry in atlas.Entries:
            rectX = entry.Frame.X - entry.SpriteSourceSize.X
            rectY = entry.Frame.Y - entry.SpriteSourceSize.Y
            rectWidth = entry.SourceSize.Width
            rectHeight = entry.SourceSize.Height
            list.append('      NamedAtlasTexture("%s", AtlasTextureInfo(Rectangle(%s, %s, %s, %s), Rectangle(%s, %s, %s, %s))),' % (entry.FullFilenameWithoutExt, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height))

        list.append('    };')
        list.append('  }')
        list.append('')
        list.append('')
        list.append('  std::string %s::GetName() const' % (className))
        list.append('  {')
        list.append('    return std::string("%s");' % (atlas.Name))
        list.append('  }')
        list.append('')
        list.append('')
        list.append('  int32_t %s::Count() const' % (className))
        list.append('  {')
        list.append('    return %s;' % (len(atlas.Entries)))
        list.append('  }')
        list.append('')
        list.append('')
        list.append('  NamedAtlasTexture %s::GetEntry(const int32_t index) const' % (className))
        list.append('  {')
        list.append('    assert(index >= 0 && index <= Count());')
        list.append('    return g_entries[index];')
        list.append('  }')
        list.append('}')
        list.append('')
        return '\n'.join(list)


    def __AppendHeader(self, list):
        list.append('/****************************************************************************************************************************************************')
        list.append(' * Copyright (c) 2015 Freescale Semiconductor, Inc.')
        list.append(' * All rights reserved.')
        list.append(' *')
        list.append(' * Redistribution and use in source and binary forms, with or without')
        list.append(' * modification, are permitted provided that the following conditions are met:')
        list.append(' *')
        list.append(' *    * Redistributions of source code must retain the above copyright notice,')
        list.append(' *      this list of conditions and the following disclaimer.')
        list.append(' *')
        list.append(' *    * Redistributions in binary form must reproduce the above copyright notice,')
        list.append(' *      this list of conditions and the following disclaimer in the documentation')
        list.append(' *      and/or other materials provided with the distribution.')
        list.append(' *')
        list.append(' *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of')
        list.append(' *      its contributors may be used to endorse or promote products derived from')
        list.append(' *      this software without specific prior written permission.')
        list.append(' *')
        list.append(' * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND')
        list.append(' * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED')
        list.append(' * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.')
        list.append(' * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,')
        list.append(' * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,')
        list.append(' * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,')
        list.append(' * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF')
        list.append(' * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE')
        list.append(' * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF')
        list.append(' * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.')
        list.append(' *')
        list.append(' ****************************************************************************************************************************************************/')
