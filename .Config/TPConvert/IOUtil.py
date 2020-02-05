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

from typing import Optional
import os

def ReadFile(filename: str) -> str:
    content = None
    with open(filename, "r") as theFile:
        content = theFile.read()
    return content


def ReadBinaryFile(filename: str) -> bytes:
    content = None
    with open(filename, "rb") as theFile:
        content = theFile.read()
    return content


def WriteFile(filename: str, content: str) -> None:
    with open(filename, "w") as theFile:
        theFile.write(content)


def WriteBinaryFile(filename: str, content: bytes) -> None:
    with open(filename, "wb") as theFile:
        theFile.write(content)


def WriteFileIfChanged(filename: str, content: str) -> None:
    existingContent = None
    if os.path.exists(filename):
        if os.path.isfile(filename):
            existingContent = ReadFile(filename)
        else:
            raise IOError("'{0}' exist but it's not a file".format(filename))
    if content != existingContent:
        WriteFile(filename, content)


def WriteBinaryFileIfChanged(filename: str, content: bytes) -> None:
    existingContent = None
    if os.path.exists(filename):
        if os.path.isfile(filename):
            existingContent = ReadBinaryFile(filename)
        else:
            raise IOError("'{0}' exist but it's not a file".format(filename))
    if content != existingContent:
        WriteBinaryFile(filename, content)


def GetFileNameWithoutExtension(path: str) -> str:
    return os.path.splitext(os.path.basename(path))[0]


def ToUnixStylePath(path: str) -> str:
    return path.replace("\\", "/")

def TryToUnixStylePath(path: Optional[str]) -> Optional[str]:
    if path is None:
        return None
    return path.replace("\\", "/")


def Join(path1: str, path2: str) -> str:
    return ToUnixStylePath(os.path.join(path1, path2))
