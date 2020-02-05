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

from typing import Any
from typing import Dict
from typing import List
import os
from TPConvert import IOUtil

class TexturePackerHeader(object):
    def __init__(self, jsonDict: Dict[str,Any]) -> None:
        super().__init__()

        if not "app" in jsonDict or not (jsonDict["app"] == "http://www.codeandweb.com/texturepacker" or jsonDict["app"] == "https://www.codeandweb.com/texturepacker"):
            raise Exception("Unknown json format")
        if not "version" in jsonDict or jsonDict["version"] != "1.0":
            raise Exception("Unknown json format")
        self.Image = jsonDict["image"]
        self.Format = jsonDict["format"]
        self.Size = TexturePackerSize(jsonDict["size"])
        self.Scale = jsonDict["scale"]


class TexturePackerSize(object):
    def __init__(self, jsonDict: Dict[str,Any]) -> None:
        super().__init__()
        self.Width = jsonDict["w"]
        self.Height = jsonDict["h"]


class TexturePackerVector2(object):
    def __init__(self, jsonDict: Dict[str,Any]) -> None:
        super().__init__()
        self.X = jsonDict["x"]
        self.Y = jsonDict["y"]


class TexturePackerRectangle(object):
    def __init__(self, jsonDict: Dict[str,Any]) -> None:
        super().__init__()
        self.X = jsonDict["x"]
        self.Y = jsonDict["y"]
        self.Width = jsonDict["w"]
        self.Height = jsonDict["h"]


class TexturePackerFrame(object):
    def __init__(self, jsonDict: Dict[str,Any], defaultDP: int) -> None:
        super().__init__()
        self.Filename = jsonDict["filename"]
        self.Frame = TexturePackerRectangle(jsonDict["frame"])
        self.Rotated = jsonDict["rotated"]
        self.Trimmed = jsonDict["trimmed"]
        self.SpriteSourceSize = TexturePackerRectangle(jsonDict["spriteSourceSize"])
        self.SourceSize = TexturePackerSize(jsonDict["sourceSize"])
#        self.Pivot = TexturePackerVector2(jsonDict["pivot"])

        self.FilenameWithoutExt = IOUtil.GetFileNameWithoutExtension(self.Filename)
        self.Path = IOUtil.ToUnixStylePath(os.path.dirname(self.Filename))
        self.FullFilenameWithoutExt = IOUtil.Join(self.Path, self.FilenameWithoutExt)

        self.DP = self.__ParseDPI_Tag(self.FilenameWithoutExt, defaultDP)

    def __ParseDPI_Tag(self, strFilename: str, defaultValue: int) -> int:
        strTag = "dp"
        if self.FilenameWithoutExt.endswith(strTag):
            index = self.FilenameWithoutExt.rfind('_')
            if index >= 0:
                strValue = self.FilenameWithoutExt[index+1:-len(strTag)]
                return self.__ParseInt(strValue, defaultValue)
        return defaultValue

    def __ParseInt(self, strVal: str, defaultValue: int) -> int:
        try:
            return int(strVal)
        except ValueError:
            return defaultValue

def ExtractEntries(frames: List[Dict[str,Any]], defaultDPI: int) -> List[TexturePackerFrame]:
    entries = []
    for entry in frames:
        entries.append(TexturePackerFrame(entry, defaultDPI))
    return entries

class TexturePackerAtlas(object):
    def __init__(self, jsonDict: Dict[str,Any], defaultDPI: int) -> None:
        super().__init__()

        if not "frames" in jsonDict or not "meta" in jsonDict:
            raise Exception("Unknown json format")

        self.Header = TexturePackerHeader(jsonDict["meta"]);
        self.Entries = ExtractEntries(jsonDict["frames"], defaultDPI) # type: List[TexturePackerFrame]
        self.Name = IOUtil.GetFileNameWithoutExtension(self.Header.Image)


