#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2019 NXP
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

from FslBuildGen import IOUtil
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.ToolConfig import ToolConfigPackageLocation


class PackagePath(object):
    def __init__(self, path: str, packageLocation: ToolConfigPackageLocation, normalize: bool = True) -> None:
        super().__init__()

        path = IOUtil.NormalizePath(path) if normalize else path

        if not isinstance(packageLocation, ToolConfigPackageLocation):
            raise UsageErrorException()


        if IOUtil.IsAbsolutePath(path):
            if not path.startswith(packageLocation.ResolvedPathEx):
                raise UsageErrorException("The path '{0}' does not belong to the supplied location '{1}'".format(path, packageLocation.ResolvedPathEx))
            rootRelativeDirPath = path[len(packageLocation.ResolvedPathEx):]
            absoluteDirPath = path
        else:
            rootRelativeDirPath = path
            absoluteDirPath = IOUtil.Join(packageLocation.ResolvedPath, path)

        self.RootRelativeDirPath = rootRelativeDirPath  # The root relative containing directory
        self.AbsoluteDirPath = absoluteDirPath          # type: str
        self.PackageRootLocation = packageLocation      # type: ToolConfigPackageLocation
