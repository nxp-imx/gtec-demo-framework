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

from typing import Any
from typing import Dict
from typing import List
from typing import Optional
from typing import Set
from typing import Tuple
import argparse
import errno
import hashlib
import io
import json
import os
import subprocess
import sys

__g_verbosityLevel = 0
__g_debugEnabled = False
__g_licenseFilename = "License.json"

__g_Image = "Image"
__g_Model = "Model"
__g_Video = "Video"
__g_extensions = [
    ('.bmp', __g_Image),
    ('.dds', __g_Image),
    ('.hdr', __g_Image),
    ('.jpg', __g_Image),
    ('.ktx', __g_Image),
    ('.png', __g_Image),
    ('.psd', __g_Image),
    ('.tga', __g_Image),
    ('.tiff', __g_Image),
    ('.3ds', __g_Model),
    ('.fbx', __g_Model),
    ('.fsf', __g_Model),
    ('.obj', __g_Model),
    ('.nff', __g_Model),
    # video
    ('.avi', __g_Video),
    ('.fsf', __g_Video),
    ('.mp4', __g_Video),
    ('.mpg', __g_Video),
    ('.mpeg', __g_Video),
    ('.mkv', __g_Video),
    ]
__g_ignore = ["example.jpg"]

__g_ignoreDir = [
    ".Config/Templates.gen/Android/Copy/res/drawable-hdpi",
    ".Config/Templates.gen/Android/Copy/res/drawable-ldpi",
    ".Config/Templates.gen/Android/Copy/res/drawable-mdpi",
    ".Config/Templates.gen/Android/Copy/res/drawable-xhdpi",
    ".Config/Templates.gen/Android/Copy/res/drawable-xxhdpi",
    ".Config/Templates.gen/Android/Copy/res/drawable-xxxhdpi",
    ".Config/Templates.gen/AndroidGradleCMake/Copy/app/src/main/res/mipmap-hdpi",
    ".Config/Templates.gen/AndroidGradleCMake/Copy/app/src/main/res/mipmap-ldpi",
    ".Config/Templates.gen/AndroidGradleCMake/Copy/app/src/main/res/mipmap-mdpi",
    ".Config/Templates.gen/AndroidGradleCMake/Copy/app/src/main/res/mipmap-xhdpi",
    ".Config/Templates.gen/AndroidGradleCMake/Copy/app/src/main/res/mipmap-xxhdpi",
    ".Config/Templates.gen/AndroidGradleCMake/Copy/app/src/main/res/mipmap-xxxhdpi",
    ]

class Config(object):
    def __init__(self, verbosityLevel: int) -> None:
        super().__init__()
        self.VerbosityLevel = verbosityLevel
        self.IsVerbose = verbosityLevel > 0

    def LogPrint(self, strMessage: str) -> None:
        if self.IsVerbose:
            print(strMessage)
            sys.stdout.flush()


class JsonBasicLicense(object):
    def __init__(self, sourceDict: Dict[str,str] = {}) -> None:
        super().__init__()
        self.Origin = ""
        self.License = ""
        self.Comment = ""
        self.Url = ""
        self.Tags = ""
        self.TagsIdList = []  # type: List[str]
        self.SourceDict = sourceDict

    def SetTags(self, tags: str) -> None:
        self.Tags = tags
        self.TagsIdList = [entry.lower() for entry in tags.split(';') if len(entry) > 0]

    def Compare(self, license: 'JsonBasicLicense') -> bool:
        return self.Origin == license.Origin and  self.License == license.License and self.Url == license.Url and self.Tags == license.Tags and self.SourceDict == license.SourceDict


class JsonComplexLicense(object):
    def __init__(self, licenses: List[JsonBasicLicense], comment: Optional[str]) -> None:
        super().__init__()
        self.Comment = comment if comment is not None else ""
        self.Licenses = list(licenses)

    def Compare(self, license: 'JsonComplexLicense') -> bool:
        return self.Comment == license.Comment and self.__IsConsideredEqual(license.Licenses)

    def __IsConsideredEqual(self, otherLicenses: List[JsonBasicLicense]) -> bool:
        for entry in otherLicenses:
            if not self.__IsMember(entry):
                return False
        return True

    def __IsMember(self, license: JsonBasicLicense) -> bool:
        for entry in self.Licenses:
            if entry.Compare(license):
                return True
        return False;

def GetExtensionList(extensions: List[Tuple[str,str]]) -> List[str]:
    return [extension[0] for extension in extensions]


def GetTitle() -> str:
    return 'FslResourceScan V0.1.3'


def ShowTitleIfNecessary() -> None:
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print((GetTitle()))


def ReadFile(filename: str, newline: Optional[str] = None) -> str:
    content = None
    with io.open(filename, "r", newline=newline) as theFile:
        content = str(theFile.read())
    return content

def ReadBinaryFile(filename: str) -> bytes:
    content = None
    with open(filename, "rb") as theFile:
        content = theFile.read()
    return content


def WriteFile(filename: str, content: str) -> None:
    with open(filename, "w") as theFile:
        theFile.write(content)


def ReadJsonFile(filename: str) -> Any: # nasty any return
    content = ReadFile(filename)
    return json.loads(content)


def WriteJsonFile(filename: str, dict: Dict[str,str]) -> None:
    with io.open(filename, 'w', encoding='utf-8') as currentFile:
        currentFile.write(str(json.dumps(dict, ensure_ascii=False, indent=2)))

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


def SafeMakeDirs(path: str) -> None:
    try:
        os.makedirs(path)
    except OSError as exc: # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

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


def TryToUnixStylePath(path: str) -> Optional[str]:
    if path is None:
        return None
    return path.replace("\\", "/")


def ToUnixStylePath(path: str) -> str:
    return path.replace("\\", "/")


def GetDirectoryName(path: str) -> str:
    return ToUnixStylePath(os.path.dirname(path))


def Join(path1: str, path2: str) -> str:
    return ToUnixStylePath(os.path.join(path1, path2))

class Resource(object):
    def __init__(self, sourcePath: str, relativeSkipChars: int) -> None:
        super().__init__()
        self.SourcePath = sourcePath
        self.SourceDirectory = GetDirectoryName(sourcePath)
        self.License = None  # type: Optional[JsonComplexLicense]
        self.RelativePath = sourcePath[relativeSkipChars:]


def ScanForFiles(path: str, extensionList: List[str], ignoreFiles: List[str]) -> List[str]:
    foundFiles = []
    for root, dirs, files in os.walk(path):
        for file in files:
            fileId = file.lower()
            for extension in extensionList:
                if fileId.endswith(extension) and not fileId in ignoreFiles:
                    foundFiles.append(ToUnixStylePath(os.path.join(root, file)))
                    break
    return foundFiles


def HashFile(filename: str, blocksize: int =65536) -> str:
    hasher = hashlib.sha256()
    with open(filename, "rb") as theFile:
        buf = theFile.read(blocksize)
        while len(buf) > 0:
            hasher.update(buf)
            buf = theFile.read(blocksize)
        return hasher.hexdigest()


def BuildFileLengthDict(files: List[str]) -> Dict[int,List[str]]:
    dict = {}  # type: Dict[int,List[str]]
    for file in files:
        fileLength = os.stat(file).st_size
        if fileLength in dict:
            dict[fileLength].append(file)
        else:
            dict[fileLength] = [file]
    return dict


def BuildFileContentHashDict(files: List[str]) -> Dict[str, List[str]]:
    dict = {} # type: Dict[str, List[str]]
    for file in files:
        hash = HashFile(file)
        if hash in dict:
            dict[hash].append(file)
        else:
            dict[hash] = [file]
    return dict


def BuildDuplicatedList(fileName: str, files: List[str]) -> List[str]:
    srcFilename = files[0]
    srcContentSet = set(ReadBinaryFile(srcFilename))
    matchingFiles = [fileName]
    for file in files:
        content = ReadBinaryFile(file)
        if len(srcContentSet.intersection(content)) == len(srcContentSet):
            matchingFiles.append(file)
    return matchingFiles


def BuildDuplicatedDict(config: Config, files: List[str], uniqueFiles: List[str]) -> Dict[str, List[str]]:
    dict = {} # type: Dict[str, List[str]]
    while(len(files) > 1):
        srcFile = files[0]
        remainingFiles = files[1:]
        matchingFiles = BuildDuplicatedList(srcFile, remainingFiles)
        if len(matchingFiles) > 1:
            dict[srcFile] = matchingFiles
        else:
            uniqueFiles.append(srcFile)

        # Remove all non duplicated files
        files.remove(srcFile)
        files = []
        for file in remainingFiles:
            if not file in matchingFiles:
                files.append(file)
    return dict


def BuildUniqueFileDictByContent(config: Config, files: List[str], uniqueFiles: List[str]) -> Dict[str, List[str]]:
    # we start by sorting files by their hash
    # this should limit the amount of files that have to be byte compared quite a bit
    duplicationDict = {} # type: Dict[str, List[str]]
    dictHash = BuildFileContentHashDict(files)
    for fileList in list(dictHash.values()):
        if len(fileList) > 1:
            newDuplicationDict = BuildDuplicatedDict(config, fileList, uniqueFiles)
            duplicationDict.update(newDuplicationDict)
        else:
            uniqueFiles.append(fileList[0])
    return duplicationDict


def BuildUniqueFileDict(config: Config, files: List[str], uniqueFiles: List[str]) -> Dict[str, List[str]]:
    # we start by sorting files by their size
    # this should limit the amount of files that have to be byte compared quite a bit
    dictFileLength = BuildFileLengthDict(files)

    #config.LogPrint("Initial bins {0}".format(len(dictFileLength)))

    duplicationDict = {}
    for fileList in list(dictFileLength.values()):
        if len(fileList) > 1:
            newDuplicationDict = BuildUniqueFileDictByContent(config, fileList, uniqueFiles)
            duplicationDict.update(newDuplicationDict)
        else:
            uniqueFiles.append(fileList[0])
    return duplicationDict


def GetFileExtension(filename: str) -> str:
    filename, fileExtension = os.path.splitext(filename)
    return fileExtension


def BuildExtensionDict(extensions: List[Tuple[str,str]]) -> Dict[str,str]:
    dict = {} # type: Dict[str,str]
    for extension in extensions:
        dict[extension[0]] = extension[1]
    return dict


def GetContentTypeByExtension(extensionDict: Dict[str,str], filename: str) -> str:
    filenameExtension = GetFileExtension(filename).lower()
    return extensionDict[filenameExtension] if filenameExtension in extensionDict else ""


def BuildResourceDirectorySet(uniqueFiles: List[str], duplicatedFilesDict: Dict[str, List[str]]) -> Set[str]:
    # build unique dir list
    resourceDirSet = set()  # type: Set[str]
    for entry in uniqueFiles:
        dirName = GetDirectoryName(entry)
        if not dirName in resourceDirSet:
            resourceDirSet.add(dirName)

    for fileList in list(duplicatedFilesDict.values()):
        for entry in fileList:
            dirName = GetDirectoryName(entry)
            if not dirName in resourceDirSet:
                resourceDirSet.add(dirName)
    return resourceDirSet


class LicenseManager(object):
    def __init__(self) -> None:
        super().__init__()
        self.KeyOrigin = "Origin"
        self.KeyLicense = "License"
        self.KeyComment = "Comment"
        self.KeyTags = "Tags"
        self.KeyURL = "URL"

        self.KeyComplexLicense = "ComplexLicense"
        self.KeyComplexLicenses = "Licenses"
        self.KeyComplexComment = "Comment"

    def TryReadLicense(self, config: Config, filename: str) -> Optional[JsonComplexLicense]:
        if not os.path.isfile(filename):
            return None

        content = None
        try:
            content = ReadJsonFile(filename)
        except (Exception) as ex:
            print("ERROR: Exception while parsing {0}".format(filename))
            raise

        if self.KeyComplexLicense in content:
            return self.__TryParseComplexLicense(config, content, filename)

        if not self.KeyOrigin in content:
            config.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyOrigin, filename));
            return None
        if not self.KeyLicense in content:
            config.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyLicense, filename));
            return None
        basicLicense = self.__TryParseBasicLicense(config, content, filename)
        return  JsonComplexLicense([basicLicense], None) if basicLicense is not None else None

    def __TryParseBasicLicense(self, config: Config, jsonDict: Any, debugFilename: str) -> Optional[JsonBasicLicense]:
        license = JsonBasicLicense(jsonDict)
        license.Origin = jsonDict[self.KeyOrigin]
        license.License = jsonDict[self.KeyLicense]
        license.Comment = jsonDict[self.KeyComment] if self.KeyComment in jsonDict else ""
        license.Url = jsonDict[self.KeyURL] if self.KeyURL in jsonDict else ""
        license.SetTags(jsonDict[self.KeyTags] if self.KeyTags in jsonDict else "")
        return  license

    def __TryParseComplexLicense(self, config: Config, jsonDict: Any, debugFilename: str) -> Optional[JsonComplexLicense]:
        jsonDict = jsonDict[self.KeyComplexLicense]
        if not self.KeyComplexLicenses in jsonDict:
            config.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyComplexLicenses, debugFilename));
            return None

        licenses = [] # type: List[JsonBasicLicense]
        for entry in jsonDict[self.KeyComplexLicenses]:
            basicLicense = self.__TryParseBasicLicense(config, entry, debugFilename)
            if basicLicense is None:
                config.LogPrint("ERROR: Failed to parse complex license in file '{0}'".format(debugFilename));
                return None
            licenses.append(basicLicense)
        comment = jsonDict[self.KeyComplexComment] if self.KeyComplexComment in jsonDict else ""
        return JsonComplexLicense(licenses, comment)

    #def SaveLicense(self, filename: str, license: JsonComplexLicense) -> None:
    #    if len(license.Comment) > 0 or len(license.Licenses) > 1:
    #        # save complex license
    #        pass
    #    else:
    #        #contentDict = {}
    #        #self.__AddKeyIfNeeded(contentDict, self.KeyOrigin, license.Origin)
    #        #self.__AddKeyIfNeeded(contentDict, self.KeyLicense, license.License)
    #        #self.__AddKeyIfNeeded(contentDict, self.KeyURL, license.URL)

    #        WriteJsonFile(filename, license.Licenses[0].SourceDict)


    #def __AddKeyIfNeeded(self, dict, key, value):
    #    if len(value) <= 0:
    #        return
    #    dict[key] = value


def BuildDirectoryLicenseDict(config: Config, resourceDirectories: Set[str], licenseFilename: str) -> Dict[str, JsonComplexLicense]:
    licenseManager = LicenseManager()
    licenseDict = {} # type: Dict[str, JsonComplexLicense]
    for dir in resourceDirectories:
        license = licenseManager.TryReadLicense(config, Join(dir, licenseFilename))
        if license is not None:
            licenseDict[dir] = license
    return licenseDict


def TagListWithLicenses(inputDirectory: str, files: List[str], directoryLicenseDict: Dict[str, JsonComplexLicense]) -> List[Resource]:
    inputDirectory = ToUnixStylePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = []
    for entry in files:
        resource = Resource(entry, skipChars)
        if resource.SourceDirectory in directoryLicenseDict:
            resource.License = directoryLicenseDict[resource.SourceDirectory]
        res.append(resource)
    return res;


def TagDictWithLicenses(inputDirectory: str, fileDict: Dict[str, List[str]], directoryLicenseDict: Dict[str, JsonComplexLicense]) -> Dict[str, List[Resource]]:
    inputDirectory = ToUnixStylePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = {} # type: Dict[str, List[Resource]]
    for key, value in fileDict.items():
        keyFilename = key[skipChars:]
        res[keyFilename] = TagListWithLicenses(inputDirectory, value, directoryLicenseDict)
    return res;

def _Flatten(entry: JsonComplexLicense) -> Tuple[str,str,str,str]:
    originList = []  # type: List[str]
    licenseList = []  # type: List[str]
    commentList = []  # type: List[str]
    urlList = []  # type: List[str]
    for licenseEntry in entry.Licenses:
        originList.append(licenseEntry.Origin)
        licenseList.append(licenseEntry.License)
        commentList.append(licenseEntry.Comment)
        urlList.append(licenseEntry.Url)
    strOrigin = "\\".join(originList)
    strLicense = "\\".join(licenseList)
    strComment = "\\".join(commentList)
    strUrl = "\\".join(urlList)
    return (strOrigin, strLicense, strComment, strUrl)



def WriteCSV(dstFilename: str, extensions: List[Tuple[str,str]], uniqueEntries: List[Resource], duplicatedEntryDict: Dict[str, List[Resource]]) -> None:
    #count = len(uniqueFiles)
    #for list in duplicatedFilesDict.values():
    #  count += len(list)
    #config.LogPrint("Found {0} resource files".format(count))

    uniqueEntries.sort(key=lambda s: s.SourcePath.lower())
    sortedDuplicatedFiles = list(duplicatedEntryDict.keys())
    sortedDuplicatedFiles.sort()
    for fileList in list(duplicatedEntryDict.values()):
        fileList.sort(key=lambda s: s.SourcePath.lower());

    extensionDict = BuildExtensionDict(extensions)

    lines = []
    lines.append("Unique files ({0});;Origin;License;Type;Comment;URL".format(len(uniqueEntries)))
    for entry in uniqueEntries:
        contentType = GetContentTypeByExtension(extensionDict, entry.RelativePath)
        if entry.License is None:
            lines.append("{0};;;;{1};;".format(entry.RelativePath, contentType))
        else:
            strOrigin, strLicense, strComment, strUrl = _Flatten(entry.License)
            lines.append("{0};;{1};{2};{3};{4};{5}".format(entry.RelativePath, strOrigin, strLicense, contentType, strComment, strUrl))

    lines.append("\n")
    lines.append("Duplicated files ({0})".format(len(duplicatedEntryDict)))
    for key in sortedDuplicatedFiles:
        lines.append("{0};;;;{1};;".format(key, GetContentTypeByExtension(extensionDict, key)))
        for entry in duplicatedEntryDict[key]:
            contentType = GetContentTypeByExtension(extensionDict, entry.RelativePath)
            if entry.License is None:
                lines.append(";{0};;;{1};;".format(entry.RelativePath, contentType))
            else:
                strOrigin, strLicense, strComment, strUrl = _Flatten(entry.License)
                lines.append(";{0};{1};{2};{3};{4};{5}".format(entry.RelativePath, strOrigin, strLicense, contentType, strComment, strUrl))

    WriteFile(dstFilename, "\n".join(lines));




def PrintIssueDirectories(fileList: List[Resource], dict: Dict[str,List[Resource]]) -> None:
    uniqueDirs = set() # type: Set[str]
    for entry in fileList:
        if not entry.SourceDirectory in uniqueDirs:
            uniqueDirs.add(entry.SourceDirectory)

    for value in list(dict.values()):
        for entry in value:
            if not entry.SourceDirectory in uniqueDirs:
                uniqueDirs.add(entry.SourceDirectory)

    if len(uniqueDirs) > 0:
        print("Investigate license for the following directories:")
        uniqueDirSet = list(uniqueDirs)
        uniqueDirSet.sort()
        for dirEntry in uniqueDirSet:
            print("  {0}".format(dirEntry))


def Filter(config: Config, ignoreDirList: List[str], inputDirectory: str, files: List[str]) -> List[str]:
    inputDirectory = ToUnixStylePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = []
    for entry in files:
        relativeName = entry[skipChars:]
        if not GetDirectoryName(relativeName) in ignoreDirList:
            res.append(entry)
    return res


def ProcessDictLicenses(config: Config, licenseFilename: str, dict: Dict[str, List[Resource]]) -> None:
    licenseManager = LicenseManager()
    newLicenseDirs = set() # type: Set[str]
    for key, entryList in dict.items():
        firstLicenseEntry = None
        noLicenseEntries = []
        for entry in entryList:
            if entry.License is None:
                noLicenseEntries.append(entry)
            elif firstLicenseEntry is None:
                firstLicenseEntry = entry
            elif not firstLicenseEntry.License.Compare(entry.License):
                raise Exception("The license of the duplicated resource at {0} and {1} is different".format(firstLicenseEntry.SourceDirectory, entry.SourceDirectory))

        if len(noLicenseEntries) > 0 and firstLicenseEntry is not None:
            config.LogPrint("Info: Found duplicated resource missing a license, cloning source license from {0}".format(firstLicenseEntry.SourcePath))
            for entry in noLicenseEntries:
                entry.License = firstLicenseEntry.License
                if not entry.SourceDirectory in newLicenseDirs:
                    newLicenseDirs.add(entry.SourceDirectory)
                    newLicenseFile = Join(entry.SourceDirectory, licenseFilename)
                    if os.path.isfile(newLicenseFile):
                        raise Exception("Could not create a new license at {0} since one already exist".format(newLicenseFile))
                    if firstLicenseEntry.License is None:
                        raise Exception("internal error")

                    oldLicenseFile = Join(firstLicenseEntry.SourceDirectory, licenseFilename)

                    CopySmallFile(oldLicenseFile, newLicenseFile)
                    # licenseManager.SaveLicense(newLicenseFile, firstLicenseEntry.License)


def FilterDictBasedOnLicense(dict: Dict[str, List[Resource]]) -> Dict[str,List[Resource]]:
    newDict = {} # type: Dict[str,List[Resource]]
    for key, entryList in dict.items():
        newList = []
        firstLicenseEntry = None
        for entry in entryList:
            if entry.License is None:
                newList.append(entry)
        if len(newList) > 0:
            if len(entryList) != len(newList):
                raise Exception("Internal error, license for duplicated files have not been fixed prior to this filtering")
            newDict[key] = newList
    return newDict


def PrintListFixTags(entries: List[Resource]) -> None:
    for entry in entries:
        if entry.License is not None:
            for licenseEntry in entry.License.Licenses:
                if licenseEntry is not None and "fix" in licenseEntry.TagsIdList:
                    print("Fix: {0}".format(entry.SourcePath))


def PrintFixTags(uniqueEntries: List[Resource], duplicatedEntriesDict: Dict[str, List[Resource]]) -> None:
    PrintListFixTags(uniqueEntries)
    for entries in list(duplicatedEntriesDict.values()):
        PrintListFixTags(entries)


def AddLicenses(dict: Dict[str, List[Resource]], entries: List[Resource]) -> None:
    for entry in entries:
        if entry.License is not None:
            for licenseEntry in entry.License.Licenses:
                if licenseEntry is not None:
                    if not licenseEntry.License in dict:
                        dict[licenseEntry.License] = [entry]
                    else:
                        dict[licenseEntry.License].append(entry)


def ExpandLicense(key: str) -> bool:
    key = key.lower()
    return key != 'bsd-3-clause' and key != "mit" and key != "mixed" and key != "cc0-1.0" and key != "cc-by-3.0" and key != "cc-by-sa-4.0" and key != "modified 3-clause bsd-license"


def PrintLicenses(uniqueEntries: List[Resource], duplicatedEntriesDict: Dict[str, List[Resource]]) -> None:
    licenseDict = {}  # type: Dict[str, List[Resource]]
    AddLicenses(licenseDict, uniqueEntries)
    for entries in list(duplicatedEntriesDict.values()):
        AddLicenses(licenseDict, entries)

    sortedKeys = list(licenseDict.keys())
    sortedKeys.sort()

    print("License")
    for key in sortedKeys:
        value = licenseDict[key]
        print("- '{0}' entries: {1}".format(key, len(value)))
        if ExpandLicense(key):
            for entry in value:
                print("  * {0}".format(entry.SourcePath))


def Process(config: Config, ignoreDirList: List[str], inputDirectory: str, extensions: List[Tuple[str,str]],
            ignoreFiles: List[str], licenseFilename: str, listLicenses: List[str],
            saveCSVs: bool) -> None:
    if not os.path.isdir(inputDirectory):
        raise Exception("'{0}' is not a directory".format(inputDirectory));

    print("Please run this on a clean checkout as compiler obj files could be found otherwise.")
    extensionList = GetExtensionList(extensions)
    allFiles = ScanForFiles(inputDirectory, extensionList, ignoreFiles)
    config.LogPrint("Found {0} resource files".format(len(allFiles)))

    files = Filter(config, ignoreDirList, inputDirectory, allFiles)
    if len(files) != len(allFiles):
        config.LogPrint("Skipping {0} files since they are part of the ignored directory list".format(len(allFiles)-len(files)))

    uniqueFiles = [] # type: List[str]
    duplicatedFilesDict = BuildUniqueFileDict(config, files, uniqueFiles)

    config.LogPrint("Found {0} unique resource files".format(len(uniqueFiles)))
    config.LogPrint("Found {0} duplicated resource files".format(len(duplicatedFilesDict)))

    # Check license information
    resourceDirectories = BuildResourceDirectorySet(uniqueFiles, duplicatedFilesDict)
    directoryLicenseDict = BuildDirectoryLicenseDict(config, resourceDirectories, licenseFilename)

    config.LogPrint("Found {0} license files".format(len(directoryLicenseDict)))

    uniqueEntries = TagListWithLicenses(inputDirectory, uniqueFiles, directoryLicenseDict)
    duplicatedEntriesDict = TagDictWithLicenses(inputDirectory, duplicatedFilesDict, directoryLicenseDict)

    ProcessDictLicenses(config, licenseFilename, duplicatedEntriesDict)

    # All resources
    if saveCSVs:
        WriteCSV("resources.csv", extensions, uniqueEntries, duplicatedEntriesDict)

    # Remove all entries that have a license associated
    noLicenseUniqueEntries = [entry for entry in uniqueEntries if entry.License is None]
    noLicenseDuplicatedEntriesDict = FilterDictBasedOnLicense(duplicatedEntriesDict)

    if len(noLicenseUniqueEntries) > 0:
        print("WARNING: Found {0} unique resource files with no license attached".format(len(noLicenseUniqueEntries)))
    if len(noLicenseDuplicatedEntriesDict) > 0:
        print("WARNING: Found {0} duplicated resource files with no license attached".format(len(noLicenseDuplicatedEntriesDict)))

    if saveCSVs:
        WriteCSV("resourcesIssues.csv", extensions, noLicenseUniqueEntries, noLicenseDuplicatedEntriesDict)

    PrintIssueDirectories(noLicenseUniqueEntries, noLicenseDuplicatedEntriesDict)

    PrintFixTags(uniqueEntries, duplicatedEntriesDict)

    if listLicenses:
        PrintLicenses(uniqueEntries, duplicatedEntriesDict)


def AddDefaultOptions(parser: argparse.ArgumentParser) -> None:
    parser.add_argument('-v', '--verbosity', action='count', default=0, help='Set verbosity level')
    parser.add_argument('--debug', action='store_true',  help='Enable script debugging')


def EarlyArgumentParser() -> bool:
    global __g_verbosityLevel
    global __g_debugEnabled
    ### Parse the initial options this allows us to use the required debug and verbosity levels while
    ### creating the actual command line argumnets.
    try:
        parser = argparse.ArgumentParser(add_help=False)
        AddDefaultOptions(parser)
        args, unknown = parser.parse_known_args()
        __g_verbosityLevel = args.verbosity
        __g_debugEnabled = True if args.debug else False;
    except (Exception) as ex:
        print("ERROR: {0}".format(str(ex)))
        if __g_debugEnabled:
            raise
        else:
            return False
    return True


def Main() -> None:
    global __g_verbosityLevel
    global __g_debugEnabled
    global __g_extensions
    global __g_ignore
    global __g_licenseFilename
    global __g_ignoreDir

    if not EarlyArgumentParser():
        return

    print(GetTitle())

    ### Add the main command line arguments
    parser = argparse.ArgumentParser(description='Quick and dirty image resource comparision script.')
    parser.add_argument("directory", help="to scan for resource files {0}".format(GetExtensionList(__g_extensions)))
    parser.add_argument('--list', action='store_true',  help='List all licenses')
    parser.add_argument('--saveCSVs', action='store_true',  help='Save CSVs')
    AddDefaultOptions(parser)


    try:
        args = parser.parse_args()
        config = Config(args.verbosity)
        Process(config, __g_ignoreDir, args.directory, __g_extensions, __g_ignore, __g_licenseFilename, args.list, args.saveCSVs)
    except (Exception) as ex:
        ShowTitleIfNecessary()
        print("ERROR: {0}".format(str(ex)))
        if __g_debugEnabled:
            raise
    return

Main()