#!/usr/bin/env python
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

from TPConvert.FormatPlugin import FormatPlugin
from TPConvert import IOUtil


class FormatPluginCSharp(FormatPlugin):
    def __init__(self, enableScaleMod=False):
        super(FormatPluginCSharp, self).__init__("C#" if not enableScaleMod else "C#Scale")
        self.__EnableScaleMod = enableScaleMod

    def Process(self, atlas, outputFilename):
        enableScale = self.__EnableScaleMod
        list = []
        list.append('using System;')
        list.append('using System.Collections.Generic;')
        list.append('using Microsoft.Xna.Framework;')
        list.append('using MB.Base.Xna;')
        list.append('')
        list.append('namespace MB')
        list.append('{')
        list.append('  class %sTextureAtlas : ITextureAtlasInfo' % (atlas.Name))
        list.append('  {')
        list.append('    public const string TheTextureName = "%s";' % (atlas.Name))
        if enableScale:
            list.append('    public const int SCALE_X = 1;')
            list.append('    public const int SCALE_Y = 1;')
        list.append('')
        list.append('    public static readonly Dictionary<string,AtlasTextureInfo> TheTextureInfo = new Dictionary<string,AtlasTextureInfo>()')
        list.append('    {')

        for entry in atlas.Entries:
            rectX = entry.Frame.X - entry.SpriteSourceSize.X
            rectY = entry.Frame.Y - entry.SpriteSourceSize.Y
            rectWidth = entry.SourceSize.Width
            rectHeight = entry.SourceSize.Height
            if not enableScale:
                list.append('      { "%s", new AtlasTextureInfo(new Rectangle(%s, %s, %s, %s), new Rectangle(%s, %s, %s, %s)) },' % (entry.FullFilenameWithoutExt, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height))
            else:
                list.append('      { "%s", new AtlasTextureInfo(new Rectangle(%s * SCALE_X, %s * SCALE_Y, %s * SCALE_X, %s * SCALE_Y), new Rectangle(%s * SCALE_X, %s * SCALE_Y, %s * SCALE_X, %s * SCALE_Y)) },' % (entry.FullFilenameWithoutExt, rectX, rectY, rectWidth, rectHeight, entry.Frame.X, entry.Frame.Y, entry.Frame.Width, entry.Frame.Height))

        list.append('    };')
        list.append('')
        list.append('    public string TextureName { get { return TheTextureName; } }')
        list.append('    public Dictionary<string,AtlasTextureInfo> TextureInfo { get { return TheTextureInfo; } }')
        list.append('  }')
        list.append('}')

        content = '\n'.join(list)

        outputFilename = '%s.%s' % (outputFilename, 'cs')
        IOUtil.WriteBinaryFileIfChanged(outputFilename, content)

