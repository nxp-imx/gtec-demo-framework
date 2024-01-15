#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
from FslBuildGen.Engine.Resolver.PreResolvePackageResult import PreResolvePackageResult
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName

#def GetPackageFromFilename(topLevelPackage: Package, filename: str) -> Package:
#    for entry in topLevelPackage.ResolvedAllDependencies:
#        if entry.Package.GenFile is not None and entry.Package.GenFile.PackageFile is not None and entry.Package.GenFile.PackageFile.AbsoluteFilePath == filename:
#            return entry.Package
#    raise Exception("Could not find package for '{0}'".format(filename))

def TryGetPackageListFromFilenames(allPackages: List[PreResolvePackageResult], requestedFiles: Optional[List[str]], ignoreNotFound: bool) -> Optional[List[PreResolvePackageResult]]:
    if requestedFiles is None:
        return None

    filenameToPackagesDict = {}  # type: Dict [str,List[PreResolvePackageResult]]
    for entry in allPackages:
        if entry.SourcePackage.TraceContext.PackageFile is not None:
            sourcePackageFile = entry.SourcePackage.TraceContext.PackageFile.AbsoluteFilePath
            if sourcePackageFile not in filenameToPackagesDict:
                filenameToPackagesDict[sourcePackageFile] = [entry]
            else:
                filenameToPackagesDict[sourcePackageFile].append(entry)

    uniqueDict = {} # type: Dict[PackageInstanceName, PreResolvePackageResult]
    for file in requestedFiles:
        if file in filenameToPackagesDict:
            filePackageList = filenameToPackagesDict[file]
            for filePackage in filePackageList:
                if not filePackage.SourcePackage.NameInfo.FullName in uniqueDict:
                    uniqueDict[filePackage.SourcePackage.NameInfo.FullName] = filePackage
        elif not ignoreNotFound:
            raise Exception("Could not find package for '{0}'".format(file))
    return list(uniqueDict.values())
