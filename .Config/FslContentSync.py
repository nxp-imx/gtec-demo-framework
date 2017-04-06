#!/usr/bin/env python
# -*- coding: utf-8 -*-

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

import argparse
import codecs
import datetime
import errno
import hashlib
import os
import shutil
import sys
import time


g_isVerbose = False
g_isInfo = False
GLOBAL_SEP = '\\'

g_cacheFilename = "_ContentSyncCache.fsl"

def GetTitle():
    return 'FslContentSync V0.2.7'

def GetCacheFormatVersion():
    return '1'

def ShowTitleIfNecessary():
    global g_isVerbose
    if not g_isVerbose:
        print(GetTitle())


def LogPrint(str):
    global g_isVerbose
    if g_isVerbose:
        print(str.encode(sys.getdefaultencoding(), 'replace'))


def LogPrintInfo(str):
    global g_isVerbose
    global g_isInfo
    if g_isVerbose or g_isInfo:
        print(str.encode(sys.getdefaultencoding(), 'replace'))


def PreviewPrint(str):
    print(str.encode(sys.getdefaultencoding(), 'replace'))

def EnsureUTF8(str):
    try:
        return unicode(str.encode('utf-8'))
    except ValueError:
        return str


def ReadFile(filename):
    content = None
    with codecs.open(filename, "r", "utf-8") as theFile:
        content = theFile.read()
    return content


def WriteFile(filename, content):
    with codecs.open(filename, "w", "utf-8") as theFile:
        theFile.write(content)


def WriteFileIfChanged(filename, content):
    existingContent = None
    if os.path.exists(filename):
        if os.path.isfile(filename):
            existingContent = ReadFile(filename)
        else:
            raise IOError("'%s' exist but it's not a file" % (filename))
    if content != existingContent:
        WriteFile(filename, content)


def FileModificationDate(filename):
    t = os.path.getmtime(filename)
    time = datetime.datetime.utcfromtimestamp(t)
    return "{:0>4d}-{:0>2d}-{:0>2d} {:0>2d}:{:0>2d}:{:0>2d}:{:0>4d}".format(time.year, time.month, time.day, time.hour, time.minute, time.second,time.microsecond / 1000)

def ToUnixStylePath(path):
    return path.replace("\\", "/")

def PathSafeJoin(path1, path2):
    return ToUnixStylePath(os.path.join(path1, path2))

def SafeRealPath(path):
    return ToUnixStylePath(os.path.realpath(path))

def SafeMakeDirs(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


class CacheState:
  Unmodified = 0
  Modified = 1
  New = 2


def GetCacheHeader():
    return "# FslContentSync cache file\n"

def GetCacheVersionString():
    return "# Format: %s\n" % (GetCacheFormatVersion())

def GetCacheToolVersion():
    return "# Created by %s\n" % (GetTitle())

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
    def __init__(self, folder):
        super(SyncState, self).__init__()
        self.Folder = folder
        self.Dirs = {}
        self.Entries = {}
        self.Removed = {}
        self.IsNew = True


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


    def Load(self, path):
        if not os.path.exists(path):
            return
        content = ReadFile(path)

        self.Dirs = {}
        self.Entries = {}
        self.Removed = {}
        self.IsNew = True
        cacheHeader = GetCacheHeader()
        # if the file starts with a invalid header, we ignore it
        if not content.startswith(cacheHeader):
            LogPrint("Cache header invalid, ignoring cache")
            return

        entries = []
        dirs = []
        lines = content.splitlines(False)
        for line in lines:
            line = line.strip()
            if not line.startswith('#'):
                elements = line.split(GLOBAL_SEP)
                if len(elements) != 4:
                    LogPrint("Cache entry invalid, ignoring cache")
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


    def Save(self, path):

        result = [GetCacheHeader(), GetCacheVersionString(), GetCacheToolVersion() ]

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
        WriteFileIfChanged(path, "".join(result))


def SDKDetectFolders(fromFolder):
    appFolder = fromFolder
    if not fromFolder.endswith("/Content"):
        fromFolder = PathSafeJoin(fromFolder, "Content")
    else:
        appFolder = os.path.dirname(fromFolder)
    if not os.path.isdir(fromFolder):
        raise Exception("Unable to locate the Content folder under path '%s'" % (fromFolder))
    androidFolder = PathSafeJoin(appFolder, "Android")
    if not os.path.isdir(androidFolder):
        raise Exception("Unable to locate the Android folder under path '%s'" % (appFolder))

    # validaton that we are in the correct dir
    androidJNIFolder = PathSafeJoin(androidFolder, "jni")
    if not os.path.isdir(androidJNIFolder):
        raise Exception("Unable to locate the Android 'jni' folder under path '%s'" % (androidFolder))
    androidRESFolder = PathSafeJoin(androidFolder, "res")
    if not os.path.isdir(androidRESFolder):
        raise Exception("Unable to locate the Android 'res' folder under path '%s'" % (androidFolder))
    androidSRCFolder = PathSafeJoin(androidFolder, "src")
    if not os.path.isdir(androidSRCFolder):
        raise Exception("Unable to locate the Android 'src' folder under path '%s'" % (androidFolder))

    # Build the content folder location
    androidAssetFolder = PathSafeJoin(androidFolder, "assets/fsl-content")
    EnsureFolderExist(androidAssetFolder)
    return (fromFolder, androidAssetFolder)


def ValidateFolder(folder, folderDesc, allowCreate):
    if not os.path.isdir(folder):
        if not allowCreate:
            raise Exception("%s does not exist '%s'" % (folderDesc, folder))
        else:
            SafeMakeDirs(folder)


def EnsureFolderExist(folder):
    if not os.path.isdir(folder):
        LogPrint("Creating directory '%s'" % (folder))
        SafeMakeDirs(folder)


def HashFile(filename, blocksize=65536):
    hasher = hashlib.sha256()
    with open(filename, "rb") as theFile:
        buf = theFile.read(blocksize)
        while len(buf) > 0:
            hasher.update(buf)
            buf = theFile.read(blocksize)
        return hasher.hexdigest()



def GetDirAndFilePaths(directory):
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
            dirpath = PathSafeJoin(root, dirname)
            dirPaths.append(ToUnixStylePath(dirpath))  # Add it to the list.
        for filename in files:
            filepath = PathSafeJoin(root, filename)
            filePaths.append(ToUnixStylePath(filepath))  # Add it to the list.
    filePaths.sort()
    dirPaths.sort()
    return (dirPaths, filePaths)

def GetCacheFilename(folder):
    return PathSafeJoin(folder, g_cacheFilename)


def BuildSyncState(folder, cachedSyncState, allowCaching):
    if cachedSyncState == None:
        LogPrint("Examining content of '%s'" % (folder))
        cachedSyncState = SyncState(folder)
        cachedSyncState.Load(GetCacheFilename(folder))
    else:
        LogPrint("Rescanning content of '%s'" % (folder))

    dirs, files = GetDirAndFilePaths(folder)


    absCacheFile = PathSafeJoin(folder, g_cacheFilename)
    if absCacheFile in files:
        files.remove(absCacheFile)

    syncState = SyncState(folder)

    allowNew = True #not cachedSyncState.IsNew

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
        syncState.AddDir(dirState)


    for file in files:
        if file.find('..') != -1:
            raise Exception("'..' is now allowed in content file names ('%s')" % (file))
        if file.find('\\') != -1:
            raise Exception("'\\' is now allowed in content file names ('%s')" % (file))
        fileState = ContentState()
        fileState.Name = file[len(folder)+1:]
        fileState.Length = os.path.getsize(file)
        fileState.ModifiedDate = FileModificationDate(file)

        cachedState = cachedSyncState.GetFileState(fileState)
        if allowCaching and cachedState and fileState.Length == cachedState.Length and fileState.ModifiedDate == cachedState.ModifiedDate:
            fileState.Checksum = cachedState.Checksum
            LogPrint("Using cached checksum for '%s'" % (fileState.Name))
        else:
            LogPrint("Calculating checksum for '%s'" % (fileState.Name))
            fileState.Checksum = HashFile(file)
        # Mark the entry as being new
        if not cachedState and allowNew:
            fileState.CacheState = CacheState.New
        elif cachedState and not fileState.IsSameState(cachedState):
            fileState.CacheState = CacheState.Modified
        else:
            fileState.CacheState = CacheState.Unmodified
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


def RemoveDirs(location, entries, isPreview):
    for entry in entries:
        LogPrint("Removing directory '%s'" % entry.Name)
        dir = PathSafeJoin(location, entry.Name)
        if not isPreview:
            os.rmdir(dir)
        else:
            PreviewPrint("Removing directory '%s'" % (dir))


def CreateDirs(location, entries, isPreview):
    for entry in entries:
        LogPrint("Creating directory '%s'" % entry.Name)
        dir = PathSafeJoin(location, entry.Name)
        if not isPreview:
            SafeMakeDirs(dir)
        else:
            PreviewPrint("Creating directory '%s'" % (dir))


def RemoveFiles(location, entries, isPreview):
    for entry in entries:
        LogPrint("Removing '%s'" % entry.Name)
        file = PathSafeJoin(location, entry.Name)
        if not isPreview:
            os.remove(file)
        else:
            PreviewPrint("Removing '%s'" % (file))


def CopyFiles(srcLocation, dstLocation, entries, isPreview):
    for entry in entries:
        srcFile = PathSafeJoin(srcLocation, entry.Name)
        dstFile = PathSafeJoin(dstLocation, entry.Name)
        if not isPreview:
            LogPrint("Updating '%s'" % entry.Name)
            dstDirName = os.path.dirname(dstFile)
            if not os.path.isdir(dstDirName):
                raise IOError("Target directory is missing '%s'" % (dstDirName))
            shutil.copy2(srcFile, dstFile)
        else:
            PreviewPrint("Copying '%s'->'%s'" % (srcFile, dstFile))


def PrepareDirSync(syncStateFrom, syncStateTo):
    # Build delete list
    deleteList = []
    createList = []
    for entry in syncStateTo.Dirs.values():
        if syncStateFrom.GetDirState(entry) == None:
            deleteList.append(entry)

    # locate all the entries that were modified or missing
    for entry in syncStateFrom.Dirs.values():
        toContentState = syncStateTo.GetDirState(entry)
        if not toContentState or not toContentState.IsSameState(entry):
            createList.append(entry)

    deleteList.sort(key=lambda s: s.Name.lower())
    createList.sort(key=lambda s: s.Name.lower())
    return (deleteList, createList)


def ExtractNames(list):
    names = []
    for entry in list:
        names.append(entry.Name)
    return names

def PrepareFileSync(syncStateFrom, syncStateTo):
    # Build delete list
    deleteList = []
    copyList = []
    for entry in syncStateTo.Entries.values():
        if syncStateFrom.GetFileState(entry) == None:
            deleteList.append(entry)

    # locate all the entries that were modified or missing
    for entry in syncStateFrom.Entries.values():
        toContentState = syncStateTo.GetFileState(entry)
        if not toContentState or not toContentState.IsSameState(entry):
            copyList.append(entry)
        elif toContentState:
            toContentState.CacheState = CacheState.Unmodified

    deleteList.sort(key=lambda s: s.Name.lower())
    copyList.sort(key=lambda s: s.Name.lower())
    return (deleteList, copyList)


def ScanForNewOrModifiedFiles(syncStateTo, allowForce):
    newFiles = []

    for file in syncStateTo.Entries.values():
        if file.CacheState != CacheState.Unmodified:
            newFiles.append(file)

    if len(newFiles) > 0:
        newFiles.sort(key=lambda s: s.Name.lower())
        if not allowForce:
            raise Exception("Found new or dirty files in the to folder '%s' files '%s'" % (syncStateTo.Folder, ", ".join(ExtractNames(newFiles))))
        else:
            for file in newFiles:
                LogPrintInfo("WARNING: new file '%s' ignored." % (file.Name))


def DoSynchronizeOneWay(syncStateFrom, syncStateTo, isPreview, allowForce):
    # Build sync lists for dirs
    dirDeleteList, dirCreateList = PrepareDirSync(syncStateFrom, syncStateTo)
    # Build sync lists for files
    fileDeleteList, fileCopyList = PrepareFileSync(syncStateFrom, syncStateTo)

    ScanForNewOrModifiedFiles(syncStateTo, allowForce)

    RemoveFiles(syncStateTo.Folder, fileDeleteList, isPreview)

    RemoveDirs(syncStateTo.Folder, dirDeleteList, isPreview)
    CreateDirs(syncStateTo.Folder, dirCreateList, isPreview)

    CopyFiles(syncStateFrom.Folder, syncStateTo.Folder, fileCopyList, isPreview)


def ValidateSyncState(syncState):
    for entry in syncState.Entries.values():
        if entry.CacheState != CacheState.Unmodified:
            raise Exception("Sync validation failed");

    if len(syncState.Removed) != 0:
        raise Exception("Sync validation failed");


def DoSynchronize(fromFolder, toFolder, isPreview, allowForce, allowCache):
    LogPrintInfo("Synchronizing\n  From '%s'\n  To   '%s'" % (fromFolder, toFolder))

    syncStateFrom = BuildSyncState(fromFolder, None, allowCache)
    syncStateTo = BuildSyncState(toFolder, None, allowCache)

    DoSynchronizeOneWay(syncStateFrom, syncStateTo, isPreview, allowForce)

    LogPrint("Refreshing to-folder's sync caches")
    syncStateTo = BuildSyncState(toFolder, syncStateFrom, True)

    # Update the sync caches
    if not isPreview:
        syncStateFrom.Save(GetCacheFilename(syncStateFrom.Folder))
        syncStateTo.Save(GetCacheFilename(syncStateTo.Folder))

        # We validate the cache after we saved it since it contains information
        # matching what is on disk even though its not what we expected after a sync.
        ValidateSyncState(syncStateTo)

    LogPrintInfo("Content sync completed")


def ValidatePaths(path1, path2):
    if path1.startswith(path2) or path2.startswith(path1):
        raise Exception("Paths are path of the same tree ('%s', '%s')" % (path1, path2))
    path1 = SafeRealPath(path1)
    path2 = SafeRealPath(path2)
    if path1.startswith(path2) or path2.startswith(path1):
        raise Exception("Real paths are path of the same tree ('%s', '%s')" % (path1, path2))


def CheckThatCacheFileExists(fromFolder, toFolder):
    if not os.path.isfile(GetCacheFilename(fromFolder)):
        raise Exception("Cache file '%s' not found under '%s'.\nUse --create to force sync or create empty cache file to allow sync." % (g_cacheFilename, toFolder))
    if not os.path.isfile(GetCacheFilename(toFolder)):
        raise Exception("Cache file '%s' not found under '%s'.\nUse --create to force sync or create a empty cache file to allow sync." % (g_cacheFilename, fromFolder))


def Synchronize(fromFolder, toFolder, isSDK, isPreview, createEnabled, allowForce, allowCache, ifExist):
    orgFromFolder = fromFolder
    if fromFolder == None:
        raise Exception("Missing input argument FromFolder")

    fromFolder = EnsureUTF8(fromFolder)

    fromFolder = ToUnixStylePath(os.path.abspath(fromFolder))

    if ifExist and not os.path.isdir(fromFolder):
        LogPrintInfo("The from folder '%s' doesnt exist, so nothing to sync." % orgFromFolder)
        return

    ValidateFolder(fromFolder, "from folder", False)

    if isSDK:
        if toFolder != None:
            raise Exception("ToFolder can not be supplied when using the --sdk option")
        fromFolder, toFolder = SDKDetectFolders(fromFolder)
    elif toFolder == None:
        raise Exception("Missing input argument ToFolder")
    else:
        toFolder = EnsureUTF8(toFolder)
        toFolder = ToUnixStylePath(os.path.abspath(toFolder))

    ValidateFolder(toFolder, "to folder", createEnabled)
    ValidatePaths(fromFolder, toFolder)

    if not createEnabled:
        CheckThatCacheFileExists(fromFolder, toFolder)

    DoSynchronize(fromFolder, toFolder, isPreview, allowForce, allowCache)


def Main():
    global g_isVerbose
    global g_isInfo

    parser = argparse.ArgumentParser(description="Synchronize the two 'Content' folders")
    parser.add_argument('-p', '--preview', action='store_true', help='If preview is enabled nothing is modified, the tool just lists what it intends to do')
    parser.add_argument('-i', '--info', action='store_true', help='Enable info mode')
    parser.add_argument('-v', '--verbose', action='store_true', help='Enable verbose mode')
    parser.add_argument('--create', action='store_true', help="Disable the requirements for the ToFolder to exist and for a '%s' file to exist in the sync folders" % (g_cacheFilename) )
    parser.add_argument('--force', action='store_true', help='if any new  or modified file is found in the toFolder they will be remove or overwritten' )
    parser.add_argument('--version', action='store_true', help='Show version')
    parser.add_argument('--cache', action='store_true', help='Allow the cache checksum to be used if the file is deemed likely to the same')
    parser.add_argument('--sdk', action='store_true', help='Enable sdk mode which allows you to just specify the apps content folder')
    parser.add_argument('--ifExist', action='store_true', help='On sync if the from folder exist')
    parser.add_argument('FromFolder', help='The primary content folder')
    parser.add_argument('ToFolder', nargs="?", help='The folder that needs to match the from folder')

    currentDir = EnsureUTF8(os.getcwd())
    try:
        args = parser.parse_args()
        g_isVerbose = args.verbose
        g_isInfo = args.info

        if args.version:
          print(GetTitle())
        if args.FromFolder:
            Synchronize(args.FromFolder, args.ToFolder, args.sdk, args.preview, args.create, args.force, args.cache, args.ifExist)
    except (Exception) as ex:
        ShowTitleIfNecessary()
        print("error: %s" % ex.message)
        if g_isVerbose:
            raise
        sys.exit(1)

Main()
