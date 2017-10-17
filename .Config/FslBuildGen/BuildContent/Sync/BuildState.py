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
import hashlib
import os
from FslBuildGen import IOUtil
from FslBuildGen.BuildContent.Sync.Content import Content
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.Config import Config


g_isVerbose = False
g_isInfo = False
GLOBAL_SEP = '\\'


class CacheState:
    Unmodified = 0
    Modified = 1
    New = 2


def __GetCacheFormatVersion() -> str:
    return '1'


def GetCacheHeader() -> str:
    return "# Resource builder cache file\n"


def GetCacheVersionString() -> str:
    return "# Format: %s\n" % (__GetCacheFormatVersion())


class ContentState(object):
    def __init__(self) -> None:
        super(ContentState, self).__init__()
        self.Name = ""
        self.Length = 0
        self.ModifiedDate = ""
        self.Checksum = ""
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
        return res


    def __AddToString(self, srcStr: str, strNew: str) -> str:
        return strNew if len(srcStr) == 0 else "{0}, {1}".format(srcStr, strNew)


class SyncState(object):
    def __init__(self, absoluteCacheFileName: str) -> None:
        super(SyncState, self).__init__()
        self.AbsoluteCacheFileName = absoluteCacheFileName
        self.Dirs = {}  # type: Dict[str, ContentState]
        self.Entries = {}  # type: Dict[str, ContentState]
        self.Removed = {}  # type: Dict[str, ContentState]
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



    def GetDirState(self, syncState: ContentState) -> Optional[ContentState]:
        if syncState.Name in self.Dirs:
            return self.Dirs[syncState.Name]
        return None


    def GetFileState(self, syncState: ContentState) -> Optional[ContentState]:
        if syncState.Name in self.Entries:
            return self.Entries[syncState.Name]
        return None

    def GetFileStateByFileName(self, fileName: str) -> Optional[ContentState]:
        if fileName in self.Entries:
            return self.Entries[fileName]
        return None


    def Load(self, config: Config) -> None:
        self.__Load(config, self.AbsoluteCacheFileName)


    def __Load(self, config: Config, path: str) -> None:
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
            config.LogPrint("Cache header invalid, ignoring cache")
            return

        entries = []  # type: List[ContentState]
        dirs = []
        lines = content.splitlines(False)
        for line in lines:
            line = line.strip()
            if not line.startswith('#'):
                elements = line.split(GLOBAL_SEP)
                if len(elements) != 4:
                    config.LogPrint("Cache entry invalid, ignoring cache")
                    return
                contentState = ContentState()
                contentState.Name = elements[0]
                contentState.Length = int(elements[1])
                contentState.ModifiedDate = elements[2]
                contentState.Checksum = elements[3]
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


    def __Save(self, path: str) -> None:
        result = [GetCacheHeader(), GetCacheVersionString()]

        sortedList = list(self.Dirs.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Dirs[name]
            strContent = "%s%s%s%s%s%s%s\n" % (entry.Name, GLOBAL_SEP, entry.Length, GLOBAL_SEP, entry.ModifiedDate, GLOBAL_SEP, entry.Checksum)
            result.append(strContent)

        sortedList = list(self.Entries.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Entries[name]
            strContent = "%s%s%s%s%s%s%s\n" % (entry.Name, GLOBAL_SEP, entry.Length, GLOBAL_SEP, entry.ModifiedDate, GLOBAL_SEP, entry.Checksum)
            result.append(strContent)
        IOUtil.WriteFileIfChanged(path, "".join(result))


    def BuildContentState(self, config: Config,
                          pathFileRecord: PathRecord,
                          allowCaching: bool, allowNew: bool,
                          cachedSyncState: Optional['SyncState'] = None) -> ContentState:
        fileState = ContentState()
        fileState.Name = pathFileRecord.RelativePath
        fileState.Length = os.path.getsize(pathFileRecord.ResolvedPath)
        fileState.ModifiedDate = self.__FileModificationDate(pathFileRecord.ResolvedPath)

        cachedState = cachedSyncState.GetFileState(fileState) if cachedSyncState else None
        if allowCaching and cachedState and fileState.Length == cachedState.Length and fileState.ModifiedDate == cachedState.ModifiedDate:
            fileState.Checksum = cachedState.Checksum
            config.LogPrint("Using cached checksum for '{0}'".format(fileState.Name))
        else:
            config.LogPrint("Calculating checksum for '{0}'".format(fileState.Name))
            fileState.Checksum = self.__HashFile(pathFileRecord.ResolvedPath)
        # Mark the entry as being new
        if not cachedState and allowNew:
            fileState.CacheState = CacheState.New
        elif cachedState and not fileState.IsSameState(cachedState):
            fileState.CacheState = CacheState.Modified
            fileState.ModificationComment = fileState.GetDifferenceString(cachedState)
        else:
            fileState.CacheState = CacheState.Unmodified
        return fileState


    def __HashFile(self, filename: str, blocksize: int = 65536) -> str:
        hasher = hashlib.sha256()
        with open(filename, "rb") as theFile:
            buf = theFile.read(blocksize)
            while len(buf) > 0:
                hasher.update(buf)
                buf = theFile.read(blocksize)
            return hasher.hexdigest()



def __BuildSyncState(config: Config, absoluteCacheFileName: str,
                     content: Content,
                     cachedSyncState: Optional[SyncState],
                     allowCaching: bool, allowNew: bool, addNewFilesAndDirs: bool) -> SyncState:
    """ absoluteCacheFileName must be a absolute filename """
    content.RemoveFileByResolvedSourcePath(absoluteCacheFileName)

    if cachedSyncState is None:
        config.LogPrint("Examining content of '{0}'".format(content.PrimaryFolder))
        cachedSyncState = SyncState(absoluteCacheFileName)
        cachedSyncState.Load(config)
    else:
        config.LogPrint("Rescanning content of '{0}'".format(content.PrimaryFolder))


    syncState = SyncState(absoluteCacheFileName)

    for pathDirRecord in content.Dirs:
        dirState = ContentState()
        dirState.Name = pathDirRecord.RelativePath
        dirState.Length = -1
        dirState.ModifiedDate = "0"
        dirState.Checksum = "0"
        cachedState = cachedSyncState.GetDirState(dirState)
        if not cachedState and allowNew:
            dirState.CacheState = CacheState.New
        if addNewFilesAndDirs or dirState.CacheState != CacheState.New:
            syncState.AddDir(dirState)


    for pathFileRecord in content.Files:
        fileState = syncState.BuildContentState(config, pathFileRecord, allowCaching, allowNew, cachedSyncState)
        if addNewFilesAndDirs or fileState.CacheState != CacheState.New:
            syncState.Add(fileState)


    # Tag the sync state with information about which files were removed compared the the last time we synced
    for entry in list(cachedSyncState.Entries.values()):
        if not entry.Name in syncState.Entries:
            syncState.AddRemoved(entry)

    # Tag the sync state with information about which dirs were removed compared the the last time we synced
    for entry in list(cachedSyncState.Dirs.values()):
        if not entry.Name in syncState.Dirs:
            syncState.AddRemoved(entry)
    return syncState



def GenerateSyncState(config: Config, absoluteCacheFileName: str, content: Content, allowCache: bool) -> SyncState:
    return __BuildSyncState(config, absoluteCacheFileName, content, None, allowCache, True, True)


def GenerateOutputSyncState(config: Config, absoluteCacheFileName: str, path: str, allowCache: bool) -> SyncState:
    content = Content(config, path)
    return __BuildSyncState(config, absoluteCacheFileName, content, None, allowCache, True, True)
