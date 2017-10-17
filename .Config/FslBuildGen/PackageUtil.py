#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2017 NXP
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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen.Packages.Package import Package

def GetPackageFromFilename(topLevelPackage: Package, filename: str) -> Package:
    for entry in topLevelPackage.ResolvedAllDependencies:
        if entry.Package.GenFile and entry.Package.GenFile.SourcePackageFile and entry.Package.GenFile.SourcePackageFile.AbsoluteFilePath == filename:
            return entry.Package
    raise Exception("Could not find package for '{0}'".format(filename))


def TryGetPackageListFromFilenames(topLevelPackage: Package, requestedFiles: Optional[List[str]]) -> Optional[List[Package]]:
    if requestedFiles is None:
        return None
    uniqueDict = {} # type: Dict[str, Package]
    for file in requestedFiles:
        filePackage = GetPackageFromFilename(topLevelPackage, file)
        if not filePackage.Name in uniqueDict:
            uniqueDict[filePackage.Name] = filePackage
    return list(uniqueDict.values())


def GetPackageListFromFilenames(topLevelPackage: Package, requestedFiles: Optional[List[str]]) -> List[Package]:
    result = TryGetPackageListFromFilenames(topLevelPackage, requestedFiles)
    return [] if result is None else result
