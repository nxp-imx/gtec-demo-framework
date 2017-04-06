#!/usr/bin/env python
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

import codecs
import datetime
import errno
import hashlib
import os
import shutil
import sys
import time
from FslBuildGen import IOUtil, Config


g_isVerbose = False
g_isInfo = False
GLOBAL_SEP = '\\'


class CacheState:
  Unmodified = 0
  Modified = 1
  New = 2


def __GetCacheFormatVersion():
    return '1'




def GetCacheHeader():
    return "# Resource builder cache file\n"


def GetCacheVersionString():
    return "# Format: %s\n" % (__GetCacheFormatVersion())


class ContentState(object):
    def __init__(self):
        super(ContentState, self).__init__()
        self.Name = ""
        self.Length = 0
        self.ModifiedDate = ""
        self.Checksum = ""
        self.CacheState = CacheState.New

    def IsSameState(self, entry):
        return self.Name == entry.Name and self.Length == entry.Length and self.ModifiedDate == entry.ModifiedDate and self.Checksum == entry.Checksum


class SyncState(object):
    def __init__(self, absoluteCacheFileName, folder):
        super(SyncState, self).__init__()
        self.AbsoluteCacheFileName = absoluteCacheFileName
        self.Folder = folder
        self.Dirs = {}
        self.Entries = {}
        self.Removed = {}
        self.IsNew = True

    def __FileModificationDate(self, filename):
        t = os.path.getmtime(filename)
        time = datetime.datetime.utcfromtimestamp(t)
        return "{:0>4d}-{:0>2d}-{:0>2d} {:0>2d}:{:0>2d}:{:0>2d}:{:0>4d}".format(time.year, time.month, time.day, time.hour, time.minute, time.second,time.microsecond / 1000)


    def Add(self, syncState):
        self.Entries[syncState.Name] = syncState


    def AddDir(self, syncState):
        self.Dirs[syncState.Name] = syncState


    def AddRemoved(self, syncState):
        self.Removed[syncState.Name] = syncState



    def GetDirState(self, syncState):
        if self.Dirs.has_key(syncState.Name):
            return self.Dirs[syncState.Name]
        return None


    def GetFileState(self, syncState):
        if self.Entries.has_key(syncState.Name):
            return self.Entries[syncState.Name]
        return None

    def GetFileStateByFileName(self, fileName):
        if self.Entries.has_key(fileName):
            return self.Entries[fileName]
        return None

    def Load(self, config):
        self.__Load(config, self.AbsoluteCacheFileName)

    def __Load(self, config, path):
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

        entries = []
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

    def Save(self):
        self.__Save(self.AbsoluteCacheFileName)

    def __Save(self, path):
        result = [GetCacheHeader(), GetCacheVersionString()]

        sortedList = list(self.Dirs.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Dirs[name]
            str = "%s%s%s%s%s%s%s\n" % (entry.Name, GLOBAL_SEP,entry.Length, GLOBAL_SEP, entry.ModifiedDate, GLOBAL_SEP, entry.Checksum)
            result.append(str)

        sortedList = list(self.Entries.keys())
        sortedList.sort()
        for name in sortedList:
            entry = self.Entries[name]
            str = "%s%s%s%s%s%s%s\n" % (entry.Name, GLOBAL_SEP,entry.Length, GLOBAL_SEP, entry.ModifiedDate, GLOBAL_SEP, entry.Checksum)
            result.append(str)
        IOUtil.WriteFileIfChanged(path, "".join(result))

    
    def BuildContentState(self, config, file, allowCaching, allowNew, cachedSyncState = None):
        if file.find('..') != -1:
            raise Exception("'..' is now allowed in content file names ('%s')" % (file))
        if file.find('\\') != -1:
            raise Exception("'\\' is now allowed in content file names ('%s')" % (file))
        fileState = ContentState()
        fileState.Name = file[len(self.Folder)+1:]
        fileState.Length = os.path.getsize(file)
        fileState.ModifiedDate = self.__FileModificationDate(file)

        cachedState = cachedSyncState.GetFileState(fileState) if cachedSyncState else None
        if allowCaching and cachedState and fileState.Length == cachedState.Length and fileState.ModifiedDate == cachedState.ModifiedDate:
            fileState.Checksum = cachedState.Checksum
            config.LogPrint("Using cached checksum for '%s'" % (fileState.Name))
        else:
            config.LogPrint("Calculating checksum for '%s'" % (fileState.Name))
            fileState.Checksum = self.__HashFile(file)
        # Mark the entry as being new
        if not cachedState and allowNew:
            fileState.CacheState = CacheState.New
        elif cachedState and not fileState.IsSameState(cachedState):
            fileState.CacheState = CacheState.Modified
        else:
            fileState.CacheState = CacheState.Unmodified
        return fileState


    def __HashFile(self, filename, blocksize=65536):
        hasher = hashlib.sha256()
        with open(filename, "rb") as theFile:
            buf = theFile.read(blocksize)
            while len(buf) > 0:
                hasher.update(buf)
                buf = theFile.read(blocksize)
            return hasher.hexdigest()



def __GetDirAndFilePaths(directory):
    """
    This function will generate the file names in a directory
    tree by walking the tree either top-down or bottom-up. For each
    directory in the tree rooted at directory top (including top itself),
    it yields a 3-tuple (dirpath, dirnames, filenames).
    """
    filePaths = []  # List which will store all of the full filepaths.
    dirPaths = []  # List which will store all of the full dirpaths.

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
    filePaths.sort()
    dirPaths.sort()
    return (dirPaths, filePaths)



def __BuildSyncState(config, absoluteCacheFileName, folder, cachedSyncState, allowCaching, allowNew, addNewFilesAndDirs):
    """ absoluteCacheFileName must be a absolute filename """
    if cachedSyncState == None:
        config.LogPrint("Examining content of '%s'" % (folder))
        cachedSyncState = SyncState(absoluteCacheFileName, folder)
        cachedSyncState.Load(config)
    else:
        config.LogPrint("Rescanning content of '%s'" % (folder))

    dirs, files = __GetDirAndFilePaths(folder)

    if absoluteCacheFileName in files:
        files.remove(absoluteCacheFileName)

    syncState = SyncState(absoluteCacheFileName, folder)


    for dir in dirs:
        if dir.find('..') != -1:
            raise Exception("'..' is now allowed in content file names ('%s')" % (dir))
        dirState = ContentState()
        dirState.Name = dir[len(folder)+1:]
        dirState.Length = -1
        dirState.ModifiedDate = "0"
        dirState.Checksum = "0"
        cachedState = cachedSyncState.GetDirState(dirState)
        if not cachedState and allowNew:
            dirState.CacheState = CacheState.New
        if addNewFilesAndDirs or dirState.CacheState != CacheState.New:
            syncState.AddDir(dirState)


    for file in files:
        fileState = syncState.BuildContentState(config, file, allowCaching, allowNew, cachedSyncState)
        if addNewFilesAndDirs or fileState.CacheState != CacheState.New:
            syncState.Add(fileState)


    # Tag the sync state with information about which files were removed compared the the last time we synced
    for entry in cachedSyncState.Entries.values():
        if not syncState.Entries.has_key(entry.Name):
            syncState.AddRemoved(entry)

    # Tag the sync state with information about which dirs were removed compared the the last time we synced
    for entry in cachedSyncState.Dirs.values():
        if not syncState.Dirs.has_key(entry.Name):
            syncState.AddRemoved(entry)
    return syncState



def GenerateSyncState(config, absoluteCacheFileName, path, allowCache):
    return __BuildSyncState(config, absoluteCacheFileName, path, None, allowCache, True, True)

def GenerateOutputSyncState(config, absoluteCacheFileName, path, allowCache):
    return __BuildSyncState(config, absoluteCacheFileName, path, None, allowCache, True, False)
