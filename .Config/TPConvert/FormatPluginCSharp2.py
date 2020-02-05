#!/usr/bin/env python3
#****************************************************************************************************************************************************
#* BSD 3-Clause License
#*
#* Copyright (c) 2014, Mana Battery
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

import datetime
from TPConvert.FormatPlugin import FormatPlugin
from TPConvert.TexturePackerObjects import TexturePackerAtlas
from TPConvert import IOUtil


class FormatPluginCSharp2(FormatPlugin):
    def __init__(self) -> None:
        super().__init__("C#2")

    def Process(self, atlas: TexturePackerAtlas, outputFilename: str) -> None:
        dateNow = datetime.datetime.now()
        list = []
        list.append('//****************************************************************************************************************************************************')
        list.append('//* File Description')
        list.append('//* ----------------')
        list.append('//*')
        list.append('//* (c) {0} company_name_here'.format(dateNow.year))
        list.append('//****************************************************************************************************************************************************')
        list.append('')
        list.append('using MB.Base.MathEx;')
        list.append('using MB.Graphics2.TextureAtlas;')
        list.append('using System.Collections.Generic;')
        list.append('')
        list.append('namespace MB')
        list.append('{')
        list.append('  class {0} : TextureAtlasBase'.format(atlas.Name))
        list.append('  {')
        list.append('    public const string SourceTextureName = "{0}";'.format(atlas.Name))
        list.append('')
        list.append('    public static readonly Dictionary<string, TextureAtlasImageInfo> SourceTextureInfo = new Dictionary<string, TextureAtlasImageInfo>()')
        list.append('    {')

        for entry in atlas.Entries:
            rectX = entry.Frame.X - entry.SpriteSourceSize.X
            rectY = entry.Frame.Y - entry.SpriteSourceSize.Y
            rectWidth = entry.SourceSize.Width
            rectHeight = entry.SourceSize.Height
            srcDP = entry.DP
            if srcDP == 160:
                list.append('      {{ "{0}", new TextureAtlasImageInfo(new Rectangle({1}, {2}, {3}, {4}), new Rectangle({5}, {6}, {7}, {8})) }},'.format(entry.FullFilenameWithoutExt, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height))
            else:
                list.append('      {{ "{0}", new TextureAtlasImageInfo(new Rectangle({1}, {2}, {3}, {4}), new Rectangle({5}, {6}, {7}, {8}), {9}) }},'.format(entry.FullFilenameWithoutExt, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height, srcDP))

        list.append('    };')
        list.append('')
        list.append('    public {0}()'.format(atlas.Name))
        list.append('      : base(SourceTextureName, SourceTextureInfo)')
        list.append('    {')
        list.append('    }')
        list.append('  }')
        list.append('}')
        list.append('')
        list.append('//****************************************************************************************************************************************************')

        content = '\n'.join(list)

        outputFilename = '{0}.{1}'.format(outputFilename, 'cs')
        IOUtil.WriteFileIfChanged(outputFilename, content)

