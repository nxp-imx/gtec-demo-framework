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

from TPConvert import IOUtil
import os

def ExtractEntries(frames):
    list = []
    for entry in frames:
        list.append(TexturePackerFrame(entry))
    return list


class TexturePackerHeader(object):
    def __init__(self, dict):
        super(TexturePackerHeader, self).__init__()

        if not "app" in dict or not (dict["app"] == "http://www.codeandweb.com/texturepacker" or dict["app"] == "https://www.codeandweb.com/texturepacker"):
            raise Exception("Unknown json format")
        if not "version" in dict or dict["version"] != "1.0":
            raise Exception("Unknown json format")
        self.Image = dict["image"]
        self.Format = dict["format"]
        self.Size = TexturePackerSize(dict["size"])
        self.Scale = dict["scale"]


class TexturePackerSize(object):
    def __init__(self, dict):
        super(TexturePackerSize, self).__init__()
        self.Width = dict["w"]
        self.Height = dict["h"]


class TexturePackerVector2(object):
    def __init__(self, dict):
        super(TexturePackerVector2, self).__init__()
        self.X = dict["x"]
        self.Y = dict["y"]


class TexturePackerRectangle(object):
    def __init__(self, dict):
        super(TexturePackerRectangle, self).__init__()
        self.X = dict["x"]
        self.Y = dict["y"]
        self.Width = dict["w"]
        self.Height = dict["h"]


class TexturePackerFrame(object):
    def __init__(self, dict):
        super(TexturePackerFrame, self).__init__()
        self.Filename = dict["filename"]
        self.Frame = TexturePackerRectangle(dict["frame"])
        self.Rotated = dict["rotated"]
        self.Trimmed = dict["trimmed"]
        self.SpriteSourceSize = TexturePackerRectangle(dict["spriteSourceSize"])
        self.SourceSize = TexturePackerSize(dict["sourceSize"])
#        self.Pivot = TexturePackerVector2(dict["pivot"])

        self.FilenameWithoutExt = IOUtil.GetFileNameWithoutExtension(self.Filename)
        self.Path = IOUtil.ToUnixStylePath(os.path.dirname(self.Filename))
        self.FullFilenameWithoutExt = IOUtil.Join(self.Path, self.FilenameWithoutExt)


class TexturePackerAtlas(object):
    def __init__(self, dict):
        super(TexturePackerAtlas, self).__init__()

        if not "frames" in dict or not "meta" in dict:
            raise Exception("Unknown json format")

        self.Header = TexturePackerHeader(dict["meta"]);
        self.Entries = ExtractEntries(dict["frames"]);
        self.Name = IOUtil.GetFileNameWithoutExtension(self.Header.Image)


