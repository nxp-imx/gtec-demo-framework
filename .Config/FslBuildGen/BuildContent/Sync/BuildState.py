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

from typing import Dict
from typing import List
from typing import Optional
import datetime
import os
from FslBuildGen import IOUtil
from FslBuildGen.BuildContent.Sync.Content import Content
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.Log import Log


g_isVerbose = False
g_isInfo = False
GLOBAL_SEP = '\\'


class CacheState:
    Unmodified = 0
    Modified = 1
    New = 2


def __GetCacheFormatVersion() -> str:
    return '2'


def GetCacheHeader() -> str:
    return "# Resource builder cache file\n"


def GetCacheVersionString() -> str:
    return "# Format: %s\n" % (__GetCacheFormatVersion())


class BasicContentState(object):
    def __init__(self) -> None:
        super().__init__()
        self.Name = ""
        self.Length = 0
        self.ModifiedDate = ""
        self.Checksum = ""
        self.TagChecksum = "0"

    def Set(self, name: str, length: int, modifiedDate: str, checksum: str, tagChecksum: str) -> None:
        self.Name = name
        self.Length = length
        self.ModifiedDate = modifiedDate
        self.Checksum = checksum
        self.TagChecksum = tagChecksum


class ContentState(BasicContentState):
    def __init__(self) -> None:
        super().__init__()
        self.CacheState = CacheState.New
        self.ModificationComment = ""

    def IsSameState(self, entry: 'ContentState') -> bool:
        return self.Name == entry.Name and self.Length == entry.Length and self.ModifiedDate == entry.ModifiedDate and self.Checksum == entry.Checksum


    def GetDifferenceString(self, entry: 'ContentState')-> str:
        res = ""
        if self.Name != entry.Name:
            res = self.__AddToString(res, "Name: {0} != {1}".format(self.Name, entry.Name))
        if self.Length != entry.Length:
            res = self.__AddToString(res, "Length: {0} != {1}".format(self.Length, entry.Length))
        if self.ModifiedDate != entry.ModifiedDate:
            res = self.__AddToString(res, "ModifiedDate: {0} != {1}".format(self.ModifiedDate, entry.ModifiedDate))
        if self.Checksum != entry.Checksum:
            res = self.__AddToString(res, "Checksum: {0} != {1}".format(self.Checksum, entry.Checksum))
        if self.TagChecksum != entry.TagChecksum:
            res = self.__AddToString(res, "TagChecksum: {0} != {1}".format(self.TagChecksum, entry.TagChecksum))
        return res


    def __AddToString(self, srcStr: str, strNew: str) -> str:
        return strNew if len(srcStr) == 0 else "{0}, {1}".format(srcStr, strNew)


def CreateDirEntry(path: str) -> ContentState:
    dirState = ContentState()
    dirState.Name = path
    dirState.Length = -1
    dirState.ModifiedDate = "0"
    dirState.Checksum = "0"
    dirState.TagChecksum = "0"
    return dirState


class BasicContent(object):
    def __init__(self) -> None:
        super().__init__()
        self.Directories = []  # type: List[str]
        self.Files = []  # type: List[BasicContentState]

    def AddFile(self, name: str, length: int, modifiedDate: str, checksum: str, tagChecksum: str) -> None:
        fileState = BasicContentState()
        fileState.Set(name, length, modifiedDate, checksum, tagChecksum)
        self.Files.append(fileState)

class SyncState(object):
    def __init__(self, absoluteCacheFileName: str) -> None:
        super().__init__()
        self.AbsoluteCacheFileName = absoluteCacheFileName
        self.__Clear()

    def __Clear(self) -> None:
        self.Dirs = {}      # type: Dict[str, ContentState]
        self.Entries = {}   # type: Dict[str, ContentState]
        self.Removed = {}   # type: Dict[str, ContentState]
        self.IsNew = True

    def __FileModificationDate(self, filename: str) -> str:
        t = os.path.getmtime(filename)
        currentTime = datetime.datetime.utcfromtimestamp(t)
        return currentTime.isoformat()


    def Add(self, syncState: ContentState) -> None:
        self.Entries[syncState.Name] = syncState


    def AddDir(self, syncState: ContentState) -> None:
        self.Dirs[syncState.Name] = syncState


    def AddRemoved(self, syncState: ContentState) -> None:
        self.Removed[syncState.Name] = syncState



    def TryGetDirState(self, syncState: ContentState) -> Optional[ContentState]:
        if syncState.Name in self.Dirs:
            return self.Dirs[syncState.Name]
        return None


    def TryGetFileState(self, syncState: ContentState) -> Optional[ContentState]:
        if syncState.Name in self.Entries:
            return self.Entries[syncState.Name]
        return None

    def TryGetFileStateByFileName(self, fileName: str) -> Optional[ContentState]:
        if fileName in self.Entries:
            return self.Entries[fileName]
        return None


    def Load(self, log: Log) -> None:
        self.__Load(log, self.AbsoluteCacheFileName)


    def __Load(self, log: Log, path: str) -> None:
        if not os.path.exists(path):
            return
        content = IOUtil.ReadFile(path)

        self.Dirs = {}
        self.Entries = {}
        self.Removed = {}
        self.IsNew = True
        cacheHeader = GetCacheHeader()
        # if the file starts with a invalid header, we ignore it
        if not content.startswith(cacheHeader):
            log.LogPrint("Cache header invalid, ignoring cache")
            return

        entries = []  # type: List[ContentState]
        dirs = []
        lines = content.splitlines(False)
        if len(lines) < 3:
            log.LogPrintWarning("Cache at '{0}' is invalid, ignoring it.".format(path))
            return
        for line in lines:
            line = line.strip()
            if not line.startswith('#'):
                elements = line.split(GLOBAL_SEP)
                if len(elements) != 5:
                    log.LogPrint("Cache entry invalid, ignoring cache")
                    return
                contentState = ContentState()
                contentState.Name = elements[0]
                contentState.Length = int(elements[1])
                contentState.ModifiedDate = elements[2]
                contentState.Checksum = elements[3]
                contentState.TagChecksum = elements[4]
                if contentState.Length >= 0:
                    entries.append(contentState)
                else:
                    dirs.append(contentState)

        for entry in entries:
            self.Add(entry)
        for entry in dirs:
            self.AddDir(entry)
        self.IsNew = False


    def Save(self) -> None:
        self.__Save(self.AbsoluteCacheFileName)

    def ExtractContent(self) -> BasicContent:
        res = BasicContent()

        # Directories
        sortedList = list(self.Dirs.keys())
        sortedList.sort()
        for name in sortedList:
            res.Directories.append(name)

        # Files
        sortedList = list(self.Entries.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Entries[name]
            state = BasicContentState()
            state.Name = entry.Name
            state.Length = entry.Length
            state.ModifiedDate = entry.ModifiedDate
            state.Checksum = entry.Checksum
            state.TagChecksum = entry.TagChecksum
            res.Files.append(state)
        return res


    def RestoreContent(self, content: BasicContent) -> None:
        self.__Clear()
        self.IsNew = False
        # Directories
        for entry in content.Directories:
            dirEntry = CreateDirEntry(entry)
            self.AddDir(dirEntry)
        # Files
        for contentFileEntry in content.Files:
            fileEntry = ContentState()
            fileEntry.Name = contentFileEntry.Name
            fileEntry.Length = contentFileEntry.Length
            fileEntry.ModifiedDate = contentFileEntry.ModifiedDate
            fileEntry.Checksum = contentFileEntry.Checksum
            fileEntry.TagChecksum = contentFileEntry.TagChecksum
            self.Add(fileEntry)


    def __Save(self, path: str) -> None:
        result = [GetCacheHeader(), GetCacheVersionString()]

        sortedList = list(self.Dirs.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Dirs[name]
            strContent = "{0}{1}{2}{3}{4}{5}{6}{7}{8}\n".format(entry.Name, GLOBAL_SEP, entry.Length, GLOBAL_SEP, entry.ModifiedDate, GLOBAL_SEP, entry.Checksum, GLOBAL_SEP, entry.TagChecksum)
            result.append(strContent)

        sortedList = list(self.Entries.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Entries[name]
            strContent = "{0}{1}{2}{3}{4}{5}{6}{7}{8}\n".format(entry.Name, GLOBAL_SEP, entry.Length, GLOBAL_SEP, entry.ModifiedDate, GLOBAL_SEP, entry.Checksum, GLOBAL_SEP, entry.TagChecksum)
            result.append(strContent)
        IOUtil.WriteFileIfChanged(path, "".join(result))


    def BuildContentState(self, log: Log,
                          pathFileRecord: PathRecord,
                          allowCaching: bool, allowNew: bool,
                          cachedSyncState: Optional['SyncState'] = None) -> ContentState:
        fileState = ContentState()
        fileState.Name = pathFileRecord.RelativePath
        fileState.Length = os.path.getsize(pathFileRecord.ResolvedPath)
        fileState.ModifiedDate = self.__FileModificationDate(pathFileRecord.ResolvedPath)
        fileState.TagChecksum = '0'

        cachedState = cachedSyncState.TryGetFileState(fileState) if cachedSyncState is not None else None
        if allowCaching and cachedState is not None and fileState.Length == cachedState.Length and fileState.ModifiedDate == cachedState.ModifiedDate:
            fileState.Checksum = cachedState.Checksum
            fileState.TagChecksum = cachedState.TagChecksum
            log.LogPrintVerbose(2, "Using cached checksum for '{0}'".format(fileState.Name))
        else:
            log.LogPrintVerbose(2, "Calculating checksum for '{0}'".format(fileState.Name))
            fileState.Checksum = IOUtil.HashFile(pathFileRecord.ResolvedPath)
        # Mark the entry as being new
        #if (cachedState is None or CacheState.New) and allowNew:
        if cachedState is None and allowNew:
            fileState.CacheState = CacheState.New
        elif cachedState is not None and not fileState.IsSameState(cachedState):
            fileState.CacheState = CacheState.Modified
            fileState.ModificationComment = fileState.GetDifferenceString(cachedState)
        else:
            fileState.CacheState = CacheState.Unmodified
        return fileState



def __BuildSyncState(log: Log, absoluteCacheFileName: str,
                     content: Content,
                     cachedSyncState: Optional[SyncState],
                     allowCaching: bool, allowNew: bool, addNewFilesAndDirs: bool,
                     allowCacheLoad: bool) -> SyncState:
    """ absoluteCacheFileName must be a absolute filename """
    content.RemoveFileByResolvedSourcePath(absoluteCacheFileName)

    if cachedSyncState is None and allowCacheLoad:
        log.LogPrintVerbose(2, "Examining content of '{0}'".format(content.PrimaryFolder))
        cachedSyncState = SyncState(absoluteCacheFileName)
        cachedSyncState.Load(log)
    else:
        log.LogPrintVerbose(2, "Rescanning content of '{0}'".format(content.PrimaryFolder))


    syncState = SyncState(absoluteCacheFileName)

    for pathDirRecord in content.Dirs:
        dirState = CreateDirEntry(pathDirRecord.RelativePath)
        cachedState = cachedSyncState.TryGetDirState(dirState) if cachedSyncState is not None else None
        if cachedState is None and allowNew:
            dirState.CacheState = CacheState.New
        if addNewFilesAndDirs or dirState.CacheState != CacheState.New:
            syncState.AddDir(dirState)


    for pathFileRecord in content.Files:
        fileState = syncState.BuildContentState(log, pathFileRecord, allowCaching, allowNew, cachedSyncState)
        if addNewFilesAndDirs or fileState.CacheState != CacheState.New:
            syncState.Add(fileState)


    # Tag the sync state with information about which files were removed compared the the last time we synced
    if cachedSyncState is not None:
        for entry in list(cachedSyncState.Entries.values()):
            if not entry.Name in syncState.Entries:
                syncState.AddRemoved(entry)

        # Tag the sync state with information about which dirs were removed compared the the last time we synced
        for entry in list(cachedSyncState.Dirs.values()):
            if not entry.Name in syncState.Dirs:
                syncState.AddRemoved(entry)
    return syncState



def GenerateSyncState(log: Log, absoluteCacheFileName: str, content: Content, allowCache: bool) -> SyncState:
    return __BuildSyncState(log, absoluteCacheFileName, content, None, allowCache, True, True, True)


def GenerateSyncState2(log: Log, absoluteCacheFileName: str, content: Content, cachedSyncState: Optional[SyncState], allowCache: bool) -> SyncState:
    return __BuildSyncState(log, absoluteCacheFileName, content, cachedSyncState, allowCache, True, True, False)


def GenerateOutputSyncState(log: Log, absoluteCacheFileName: str, path: str, allowCache: bool) -> SyncState:
    content = Content(log, path, True)
    return __BuildSyncState(log, absoluteCacheFileName, content, None, allowCache, True, True, True)
