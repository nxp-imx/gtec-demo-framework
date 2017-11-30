#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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

from typing import List
from typing import Optional
from typing import Tuple
import os
from FslBuildGen.Log import Log
from FslBuildGen import IOUtil
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildContent.ContentRootRecord import ContentRootRecord
from FslBuildGen.BuildContent.Processor.ContentFileRecord import ContentFileRecord


class Content(object):
    def __init__(self, log: Log, sourcePath: str, includeSourcePathContent: bool, additionalFiles: Optional[List[PathRecord]] = None) -> None:
        super(Content, self).__init__()

        folderRoot = ContentRootRecord(log, sourcePath)

        pathDirRecords = []  # type: List[PathRecord]
        pathFileRecords = []  # type: List[PathRecord]
        if includeSourcePathContent:
            dirs, files = self.__GetDirAndFilePaths(sourcePath)

            #rootDict = {}
            #rootDict[folderRoot.Id] = folderRoot

        #    if additionalFiles != None:
        #        for entry in additionalFiles:
        #            if not entry.SourceRoot.Id in rootDict:
        #                rootDict[entry.SourceRoot.Id] = entry.SourceRoot
        #            files.append(entry)

            # We sort it so that the longest paths come first meaning we will always find the most exact match first
            # if searching from the front to the end of the list and comparing to 'startswith'
            #uniqueRootNames = list(rootDict.values())
            #uniqueRootNames.sort(key=lambda s: -len(s.Id))

            for dirEntry in dirs:
                pathDirRecords.append(PathRecord(log, folderRoot, dirEntry[len(folderRoot.ResolvedPath)+1:]))

            for file in files:
                pathFileRecords.append(PathRecord(log, folderRoot, file[len(folderRoot.ResolvedPath)+1:]))

        self.__AppendAdditional(log, pathDirRecords, pathFileRecords, additionalFiles)

        self.PrimaryFolder = sourcePath
        self.Dirs = pathDirRecords
        self.Files = pathFileRecords

        self.Dirs.sort(key=lambda s: s.Id)
        self.Files.sort(key=lambda s: s.Id)


    def RemoveFileByResolvedSourcePath(self, resolvedSourcePath: str) -> None:
        entry = self.TryFindByResolvedSourcePath(resolvedSourcePath)
        if entry is not None:
            self.Files.remove(entry)


    def TryFindByResolvedSourcePath(self, resolvedSourcePath: str) -> Optional[PathRecord]:
        for entry in self.Files:
            if entry.ResolvedPath == resolvedSourcePath:
                return entry
        return None


    def __GetDirAndFilePaths(self, directory: str) -> Tuple[List[str], List[str]]:
        """
        This function will generate the file names in a directory
        tree by walking the tree either top-down or bottom-up. For each
        directory in the tree rooted at directory top (including top itself),
        it yields a 3-tuple (dirpath, dirnames, filenames).
        """
        filePaths = [] # type: List[str]   # List which will store all of the full filepaths.
        dirPaths = []  # type: List[str]   # List which will store all of the full dirpaths.

        # Walk the tree (skipping hidden files and directories).
        for root, directories, files in os.walk(directory):
            files = [f for f in files if not f[0] == '.']
            directories[:] = [d for d in directories if not d[0] == '.']
            for dirname in directories:
                dirpath = IOUtil.Join(root, dirname)
                dirPaths.append(IOUtil.ToUnixStylePath(dirpath))  # Add it to the list.
            for filename in files:
                filepath = IOUtil.Join(root, filename)
                filePaths.append(IOUtil.ToUnixStylePath(filepath))  # Add it to the list.
        return (dirPaths, filePaths)


    def __AppendAdditional(self, log: Log,
                           rPathDirRecords: List[PathRecord],
                           pathFileRecords: List[PathRecord],
                           additionalFiles: Optional[List[PathRecord]]) -> None:
        if additionalFiles is None:
            return

        uniqueDirs = set()
        for dirEntry in rPathDirRecords:
            uniqueDirs.add(dirEntry.RelativeId)

        uniqueFiles = {}
        for entry in pathFileRecords:
            uniqueFiles[entry.RelativeId] = entry

        for entry in additionalFiles:
            dirName = IOUtil.GetDirectoryName(entry.RelativePath)
            if len(dirName) > 0:
                dirId = dirName.lower()
                if not dirId in uniqueDirs:
                    uniqueDirs.add(dirId)
                    rPathDirRecords.append(PathRecord(log, entry.SourceRoot, dirName))
            if entry.RelativeId in uniqueFiles:
                raise Exception("The relative file name '{0}' has already been added by '{1}' and '{2}' tried to add it again".format(uniqueFiles[entry.RelativeId].RelativePath, uniqueFiles[entry.RelativeId].ResolvedPath, entry.ResolvedPath))

            pathFileRecords.append(entry)
            uniqueFiles[entry.RelativeId] = entry
