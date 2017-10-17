#!/usr/bin/env python3

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
import io
import os
import os.path
import shutil
import stat
import sys
from typing import Any
from typing import List
from typing import Optional
from typing import Tuple
from typing import Union
#from FslBuildGen.Exceptions import *
#from FslBuildGen import Util

def ReadFile(filename: str, newline: Optional[str] = None) -> str:
    content = None
    with io.open(filename, "r", newline=newline) as theFile:
        content = str(theFile.read())
    return content


def TryReadFile(filename: str) -> Optional[str]:
    try:
        return ReadFile(filename)
    except IOError:
        return None


def WriteFile(filename: str, content: str, newline: Optional[str] = None) -> None:
    with io.open(filename, "w", newline=newline) as theFile:
        theFile.write(content)


def WriteFileIfChanged(filename: str, content: str, newline: Optional[str] = None) -> None:
    existingContent = None
    if os.path.exists(filename):
        if os.path.isfile(filename):
            existingContent = ReadFile(filename)
        else:
            raise IOError("'{0}' exist but it's not a file".format(filename))
    if content != existingContent:
        WriteFile(filename, content, newline=newline)


def ReadBinaryFile(filename: str) -> bytes:
    content = None
    with open(filename, "rb") as theFile:
        content = theFile.read()
    return content


def TryReadBinaryFile(filename: str) -> Optional[bytes]:
    try:
        return ReadBinaryFile(filename)
    except IOError:
        return None


def WriteBinaryFile(filename: str, content: bytes) -> None:
    with open(filename, "wb") as theFile:
        theFile.write(content)


def WriteBinaryFileIfChanged(filename: str, content: bytes) -> None:
    existingContent = None
    if os.path.exists(filename):
        if os.path.isfile(filename):
            existingContent = ReadBinaryFile(filename)
        else:
            raise IOError("'%s' exist but it's not a file" % (filename))
    if content != existingContent:
        WriteBinaryFile(filename, content)


def SetFileExecutable(filename: str) -> None:
    st = os.stat(filename)
    os.chmod(filename, st.st_mode | stat.S_IEXEC)
    st = os.stat(filename)


def Exists(path: str) -> bool:
    return os.path.exists(path)


def IsFile(filename: str) -> bool:
    return os.path.isfile(filename)


def IsDirectory(path: str) -> bool:
    return os.path.isdir(path)


def RemoveFile(filename: str) -> None:
    try:
        os.remove(filename)
    except OSError:
        pass


def IsAbsolutePath(sourcePath: str) -> bool:
    return os.path.isabs(sourcePath)


def RemoveAllContent(pathDir: str, directoryMustExist: bool = True) -> None:
    """ Removes all files and directories from the given directory """
    if not IsDirectory(pathDir):
        if directoryMustExist:
            raise Exception("Usage error '{0}' is not a directory".format(pathDir))
        return

    for item in os.listdir(pathDir):
        fullPath = os.path.join(pathDir, item)
        if IsFile(fullPath):
            os.remove(fullPath)
        elif IsDirectory(fullPath):
            shutil.rmtree(fullPath)


def TryGetEnvironmentVariable(name: str) -> Optional[str]:
    return os.environ.get(name)

def GetEnvironmentVariable(name: str) -> Optional[str]:
    result = os.environ.get(name)
    if result is None:
        raise EnvironmentError("'{0}' environment variable not set".format(name))
    return result


def GetEnvironmentVariableForAbsolutePath(name: str) -> str:
    path = TryGetEnvironmentVariable(name)
    if path is None:
        raise EnvironmentError("'{0}' environment variable not set".format(name))
    path = NormalizePath(path)
    if path is None:
        raise EnvironmentError("'{0}' environment variable not set".format(name))
    if not os.path.isabs(path):
        raise EnvironmentError("'{0}' environment path '{1}' is not absolute".format(name, path))
    if path.endswith("/"):
        raise EnvironmentError("'{0}' environment path '{1}' not allowed to end with '/' or '\'".format(name, path))
    return path


def GetEnvironmentVariableForDirectory(name: str) -> str:
    path = TryGetEnvironmentVariable(name)
    if path is None:
        raise EnvironmentError("{0} environment variable not set".format(name))
    path = NormalizePath(path)
    if path is None:
        raise EnvironmentError("{0} environment variable not set".format(name))
    if not os.path.isabs(path):
        raise EnvironmentError("{0} environment path '{1}' is not absolute".format(name, path))
    if path.endswith("/"):
        raise EnvironmentError("{0} environment path '{1}' not allowed to end with '/' or '\'".format(name, path))
    if not os.path.isdir(path):
        raise EnvironmentError("The {0} environment variable content '{1}' does not point to a valid directory".format(name, path))
    return path


def SafeMakeDirs(path: str) -> None:
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

# TODO: find the correct type for the excinfo
def __OnRMError(func: object, path: str, excinfo: Any) -> None:
    # Deal with read only files
    if IsFile(path):
        os.chmod(path, stat.S_IWRITE)
        os.remove(path)
    elif IsDirectory(path):
        os.chmod(path, stat.S_IWRITE)
        os.removedirs(path)
    else:
        raise excinfo[0](excinfo[1]).with_traceback(excinfo[2])


def SafeRemoveDirectoryTree(path: str, logExceptionAsWarningButContinue: bool = False) -> None:
    """ Beware this also removes read only files """
    try:
        if IsDirectory(path):
            shutil.rmtree(path, onerror=__OnRMError)
    except Exception as ex:
        if not logExceptionAsWarningButContinue:
            raise
        print(("WARNING: Could not remove the directory at '{0}' because {1}".format(path, str(ex))))



def CopySmallFile(srcFilename: str, dstFilename: str) -> None:
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

    if srcContent is None:
        raise IOError("'%s' not found" % (srcFilename))

    if srcContent != dstContent:
        if dstContent is None:
            dstDirName = os.path.dirname(dstFilename)
            if not os.path.exists(dstDirName):
                SafeMakeDirs(dstDirName)
        WriteBinaryFileIfChanged(dstFilename, srcContent)



def ToUnixStylePath(path: str) -> str:
    # Workaround the fact that paths on windows sometimes come with a uppercase drive letter and sometimes a lowercase
    if len(path) > 2 and (path[1] == ':' and (path[0] >= 'a' and path[0] <= 'z')):
        path = path[0].upper() + path[1:]
    return path.replace("\\", "/")


def TryToUnixStylePath(path: Optional[str]) -> Optional[str]:
    if path is None:
        return None
    return path.replace("\\", "/")


def NormalizePath(path: str) -> str:
    return ToUnixStylePath(os.path.normpath(path))


def Join(path1: str, path2: str) -> str:
    return ToUnixStylePath(os.path.join(path1, path2))


def GetFileName(path: str) -> str:
    return os.path.basename(path)


def GetFileNameWithoutExtension(path: str) -> str:
    return os.path.splitext(os.path.basename(path))[0]


def GetFileNameExtension(path: str) -> str:
    return os.path.splitext(os.path.basename(path))[1]


def GetFilePaths(directory: str, endswithFilter: Optional[Union[str, Tuple[str, ...]]]) -> List[str]:
    """
    This function will generate the file names in a directory
    tree by walking the tree either top-down or bottom-up. For each
    directory in the tree rooted at directory top (including top itself),
    it yields a 3-tuple (dirpath, dirnames, filenames).
    """
    filePaths = []  # type: List[str]   # List which will store all of the full filepaths.

    try:
        # Walk the tree.
        for root, directories, files in os.walk(directory):
            for filename in files:
                if endswithFilter is None or filename.endswith(endswithFilter):
                    # Join the two strings in order to form the full filepath.
                    filepath = os.path.join(root, filename)
                    filePaths.append(ToUnixStylePath(filepath))  # Add it to the list.
    except StopIteration: # Python >2.5
        pass
    return filePaths


def GetFilesAt(directory: str, absolutePaths: bool) -> List[str]:
    if absolutePaths:
        return [Join(directory, f) for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    return [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]


def GetDirectoriesAt(directory: str, absolutePaths: bool) -> List[str]:
    """
    This function will generate the file names in a directory
    tree by walking the tree either top-down or bottom-up. For each
    directory in the tree rooted at directory top (including top itself),
    it yields a 3-tuple (dirpath, dirnames, filenames).
    """

    #file_paths: List[str] = []  # List which will store all of the full filepaths.

    # Walk the tree.
    res = []  # type: List[str]
    try:
        root, directories, files = next(os.walk(directory))

        for path in directories:
            dirpath = path
            if absolutePaths:
                dirpath = os.path.join(root, path)
            res.append(ToUnixStylePath(dirpath))
    except StopIteration: # Python >2.5
        pass
    return res


def GetExecutablePath() -> str:
    return NormalizePath(os.path.dirname(sys.argv[0]))


def GetDirectoryName(path: str) -> str:
    directoryName = NormalizePath(os.path.dirname(path))
    return directoryName if directoryName != '.' else ''


def TryFindFileInCurrentOrParentDir(path: str, filename: str) -> Optional[str]:
    oldPath = None
    while path != oldPath:
        fullPath = os.path.join(path, filename)
        if os.path.isfile(fullPath):
            return NormalizePath(fullPath)
        oldPath = path
        path = os.path.dirname(path)
    return None


def TryFindExecutable(program: str) -> Optional[str]:
    """ Try to locate the given executable """
    def IsExe(fpath: str) -> bool:
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


def TryFindFileInPath(filename: str) -> Optional[str]:
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


def GetCurrentWorkingDirectory() -> str:
    return NormalizePath(os.getcwd())
