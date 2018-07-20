#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2018 NXP
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
import fnmatch;
from FslBuildGen import IOUtil
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package

class CustomPackageFileFilter(object):
    def __init__(self, userRequestedFNMatchPattern: str) -> None:
        super().__init__()
        userRequestedFNMatchPattern = IOUtil.NormalizePath(userRequestedFNMatchPattern)
        #self.FilterDirPath = IOUtil.NormalizePath(filterDirPath)
        self.UserRequestedFNMatchPattern = userRequestedFNMatchPattern
        self.IsAbsolutePattern = IOUtil.IsAbsolutePath(userRequestedFNMatchPattern)
        self.PatternFileName = IOUtil.GetFileName(userRequestedFNMatchPattern)
        patternDirectory = IOUtil.GetDirectoryName(userRequestedFNMatchPattern)
        if not self.IsAbsolutePattern and len(patternDirectory) > 0 and not patternDirectory.startswith('/'):
            patternDirectory = '/' + patternDirectory
        self.PatternDirectory = patternDirectory

    def TryLocateFilePatternInPackage(self, log: Log, package: Package, validExtensionList: List[str]) -> Optional[List[str]]:
        allPackageFileList = self.__GetAllPackageFilesList(package, None)

        filenameDict = self.__CreateFilenameLookupDict(allPackageFileList)
        # pattern match the filenames
        matchedFilenames = fnmatch.filter(filenameDict.keys(), self.PatternFileName)

        matchedFiles = self.__MatchPaths(matchedFilenames, filenameDict, self.PatternDirectory)

        # strip all files with unsupported extensions
        matchedFiles = [filepath for filepath in matchedFiles if PerformClangUtil.IsValidExtension(filepath, validExtensionList)]
        matchedFiles.sort()
        return matchedFiles if len(matchedFiles) > 0 else None

    def __MatchPaths(self, filenameList: List[str], filenameToPathsDict: Dict[str,List[str]], directoryPattern: str) -> List[str]:
        """
        Match
        """
        result = [] # type: List[str]
        for filename in filenameList:
            if filename in filenameToPathsDict:
                for filepath in filenameToPathsDict[filename]:
                    filedirectory = IOUtil.GetDirectoryName(filepath)
                    if filedirectory.endswith(directoryPattern):
                        result.append(filepath)
        return result

    def __CreateFilenameLookupDict(self, filenameList: List[str]) -> Dict[str,List[str]]:
        """
        Create a lookup dict that matches a filename to all full paths where that filename exist
        """
        filenameDict = dict()  # type: Dict[str,List[str]]
        for filepath in filenameList:
            filename = IOUtil.GetFileName(filepath)
            if not filename in filenameDict:
                filenameDict[filename] = [filepath]
            else:
                filenameDict[filename].append(filepath)
        return filenameDict

    def __GetAllPackageFilesList(self, package: Package, additionalPackageFiles: Optional[List[str]]) -> List[str]:
        """
        Get a list containing the full pathname of all include and source files + any additionalPackageFiles supplied.
        """
        if package.AbsolutePath is None or package.ResolvedBuildAllIncludeFiles is None or package.ResolvedBuildSourceFiles is None:
            raise Exception("Internal error")

        allFiles = [] # type: List[str]
        for filename in package.ResolvedBuildAllIncludeFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, filename)
            allFiles.append(fullPath)
        for filename in package.ResolvedBuildSourceFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, filename)
            allFiles.append(fullPath)
        if additionalPackageFiles is not None:
            allFiles += additionalPackageFiles
        return allFiles

    def __str__(self) -> str:
        #return "'{0}' at '{1}'".format(self.UserRequestedFNMatchPattern, self.FilterDirPath)
        return "'{0}'".format(self.UserRequestedFNMatchPattern)


