#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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

from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.PackagePath import PackagePath
from FslBuildGen.ToolConfig import ToolConfigPackageLocation


class PackageFile(PackagePath):
    def __init__(self, filename: str, strPackageName: Optional[str], packageLocation: ToolConfigPackageLocation) -> None:
        filename = IOUtil.NormalizePath(filename)
        if not IOUtil.IsAbsolutePath(filename):
            raise UsageErrorException()

        rootRelativePath = filename[len(packageLocation.ResolvedPathEx):]
        super().__init__(IOUtil.GetDirectoryName(rootRelativePath), packageLocation, False)

        self.Filename = IOUtil.GetFileName(filename)
        self.RootRelativeFilePath = rootRelativePath # The full root relative path to the file
        self.AbsoluteFilePath = filename  # type: str
        self.PackageName = self.__DeterminePackageNameFromRelativeName(self.RootRelativeDirPath) if strPackageName is None else strPackageName # type: str


    def __DeterminePackageNameFromRelativeName(self, relativeDirPath: str) -> str:
        return relativeDirPath.replace('/', '.')
