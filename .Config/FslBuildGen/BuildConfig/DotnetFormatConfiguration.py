#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2022 NXP
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
#    * Neither the name of the NXP. nor the names of
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
from FslBuildGen import IOUtil
from FslBuildGen.BuildConfig.ClangConfiguration import ClangConfiguration

class DotnetFormatConfiguration(ClangConfiguration):
    def __init__(self, fileExtensions: List[str], recipePackageName: str, ninjaRecipePackageName: str) -> None:
        super().__init__()
        self.CustomFormatFile = ".editorconfig"
        self.FileExtensions = fileExtensions
        self.RecipePackageName = recipePackageName
        self.NinjaRecipePackageName = ninjaRecipePackageName

        # Additional user supplied command line arguments (FIX: should be moved)
        self.AdditionalUserArguments = []  # type: List[str]

        self.__ValidateFileExtensions(fileExtensions)

    def __ValidateFileExtensions(self, fileExtensions: List[str]) -> None:
        for fileExt in fileExtensions:
            normalizedFileExt = IOUtil.NormalizePath(fileExt)
            directory = IOUtil.GetDirectoryName(normalizedFileExt)
            if len(directory) > 0:
                raise Exception("DotnetFormatConfiguration: File extension '{0}' can not contain a directory '{1}'".format(fileExt, directory))
            if fileExt.startswith(".."):
                raise Exception("DotnetFormatConfiguration: File extension can not start with '..'")
            if not fileExt.startswith("."):
                raise Exception("DotnetFormatConfiguration: File extension '{0}' does not start with '.'")
