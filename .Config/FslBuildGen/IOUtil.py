#!/usr/bin/env python

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

import errno
import sys
import os
import stat
import os.path
from FslBuildGen.Exceptions import *
from FslBuildGen import Util

def ReadFile(filename):
    content = None
    with open(filename, "r") as theFile:
        content = theFile.read()
    return content


def TryReadFile(filename):
    try:
        return ReadFile(filename)
    except IOError:
        return None


def WriteFile(filename, content):
    with open(filename, "w") as theFile:
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


def ReadBinaryFile(filename):
    content = None
    with open(filename, "rb") as theFile:
        content = theFile.read()
    return content


def TryReadBinaryFile(filename):
    try:
        return ReadBinaryFile(filename)
    except IOError:
        return None


def WriteBinaryFile(filename, content):
    with open(filename, "wb") as theFile:
        theFile.write(content)


def WriteBinaryFileIfChanged(filename, content):
    existingContent = None
    if os.path.exists(filename):
        if os.path.isfile(filename):
            existingContent = ReadBinaryFile(filename)
        else:
            raise IOError("'%s' exist but it's not a file" % (filename))
    if content != existingContent:
        WriteBinaryFile(filename, content)

def SetFileExecutable(filename):
    st = os.stat(filename)
    os.chmod(filename, st.st_mode | stat.S_IEXEC)
    st = os.stat(filename)


def IsFile(filename):
    return os.path.isfile(filename)


def IsDirectory(path):
    return os.path.isdir(path)


def RemoveFile(filename):
    try:
        os.remove(filename)
    except OSError:
        pass


def TryGetEnvironmentVariable(name):
    return Util.EnsureUTF8(os.environ.get(name))


def GetEnvironmentVariableForAbsolutePath(name):
    path = TryGetEnvironmentVariable(name)
    if path == None:
        raise EnvironmentError("%s environment variable not set" % name)
    path = NormalizePath(path)
    if path == None:
        raise EnvironmentError("%s environment variable not set" % name)
    if not os.path.isabs(path):
        raise EnvironmentError("%s environment path '%s' is not absolute" % (name, path))
    if path.endswith("/"):
        raise EnvironmentError("%s environment path not allowed to end with '/' or '\'" % (name, path))
    return path


def GetEnvironmentVariableForDirectory(name):
    path = TryGetEnvironmentVariable(name)
    if path == None:
        raise EnvironmentError("%s environment variable not set" % name)
    path = NormalizePath(path)
    if path == None:
        raise EnvironmentError("%s environment variable not set" % name)
    if not os.path.isdir(path):
        raise EnvironmentError("%s environment variable does not point to a valid directory" % name)
    if not os.path.isabs(path):
        raise EnvironmentError("%s environment path '%s' is not absolute" % (name, path))
    if path.endswith("/"):
        raise EnvironmentError("%s environment path not allowed to end with '/' or '\'" % (name, path))
    return path


def SafeMakeDirs(path):
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def CopySmallFile(srcFilename, dstFilename):
    srcContent = None
    dstContent = None
    if os.path.exists(srcFilename):
        if os.path.isfile(srcFilename):
            srcContent = ReadBinaryFile(srcFilename)
        else:
            raise IOError("'%s' exist but it's not a file" % (srcFilename))

    if os.path.exists(dstFilename):
        if os.path.isfile(dstFilename):
            dstContent = ReadBinaryFile(dstFilename)
        else:
            raise IOError("'%s' exist but it's not a file" % (dstFilename))

    if srcContent == None:
            raise IOError("'%s' not found" % (srcFilename))

    if srcContent != dstContent:
        if dstContent == None:
            dstDirName = os.path.dirname(dstFilename);
            if not os.path.exists(dstDirName):
                SafeMakeDirs(dstDirName)
        WriteBinaryFileIfChanged(dstFilename, srcContent)


def ToUnixStylePath(path):
    if path == None:
        return None
    return path.replace("\\", "/")


def NormalizePath(path):
    return ToUnixStylePath(os.path.normpath(path))


def Join(path1, path2):
    return ToUnixStylePath(os.path.join(path1, path2))


def GetFileName(path):
    return os.path.basename(path)

def GetFileNameWithoutExtension(path):
    return os.path.splitext(os.path.basename(path))[0]

def GetFileNameExtension(path):
    return os.path.splitext(os.path.basename(path))[1]


def GetFilePaths(directory, filter):
    """
    This function will generate the file names in a directory
    tree by walking the tree either top-down or bottom-up. For each
    directory in the tree rooted at directory top (including top itself),
    it yields a 3-tuple (dirpath, dirnames, filenames).
    """
    file_paths = []  # List which will store all of the full filepaths.

    try:
        # Walk the tree.
        for root, directories, files in os.walk(directory):
            for filename in files:
                if filter == None or filename.endswith(filter):
                    # Join the two strings in order to form the full filepath.
                    filepath = os.path.join(root, filename)
                    file_paths.append(ToUnixStylePath(filepath))  # Add it to the list.
    except StopIteration as exc: # Python >2.5
        return file_paths
    return file_paths


def GetFilesAt(directory, absolutePaths):
    if absolutePaths:
        return [ Join(directory,f) for f in os.listdir(directory) if os.path.isfile(os.path.join(directory,f)) ]
    else:
        return [ f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory,f)) ]


def GetDirectoriesAt(directory, absolutePaths):
    """
    This function will generate the file names in a directory
    tree by walking the tree either top-down or bottom-up. For each
    directory in the tree rooted at directory top (including top itself),
    it yields a 3-tuple (dirpath, dirnames, filenames).
    """

    file_paths = []  # List which will store all of the full filepaths.

    # Walk the tree.
    res = []
    try:
        root, directories, files = next(os.walk(directory))

        for path in directories:
            dirpath = path
            if absolutePaths:
                dirpath = os.path.join(root, path)
            res.append(ToUnixStylePath(dirpath))
    except StopIteration as exc: # Python >2.5
        pass
    return res


def GetExecutablePath():
    return NormalizePath(os.path.dirname(sys.argv[0]))


def GetDirectoryName(path):
    return NormalizePath(os.path.dirname(path))


def TryFindFileInCurrentOrParentDir(path, filename):
    oldPath = None
    while path != oldPath:
        fullPath = os.path.join(path, filename)
        if os.path.isfile(fullPath): 
            return NormalizePath(fullPath)
        oldPath = path
        path = os.path.dirname(path)
    return None


def FindExecutable(program):
    """ Try to locate the given executable """
    def IsExe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if IsExe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exeFile = os.path.join(path, program)
            if IsExe(exeFile):
                return exeFile
    return None


def FindFileInPath(filename):
    fpath, fname = os.path.split(filename)
    if fpath:
        if os.path.isfile(filename):
            return filename
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exeFile = os.path.join(path, filename)
            if os.path.isfile(exeFile):
                return exeFile
    return None
