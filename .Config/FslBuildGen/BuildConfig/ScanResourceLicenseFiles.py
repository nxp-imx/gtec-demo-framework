#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2021 NXP
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
from typing import Tuple
from typing import Set
import errno
import hashlib
import io
import os
import os.path
import json
import sys
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.BuildConfig.LicenseConfig import LicenseConfig
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Packages.Package import Package
from FslBuildGen.ToolMinimalConfig import ToolMinimalConfig

#import argparse
#import os
#import subprocess

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

__g_ignore = [] # type: List[str]

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

    def Merge(self, license: 'JsonComplexLicense') -> None:
        for entry in license.Licenses:
            if not self.__IsMember(entry):
                self.Licenses.append(entry)

def _GetExtensionList(extensions: List[Tuple[str,str]]) -> List[str]:
    return [extension[0] for extension in extensions]


def _ReadJsonFile(filename: str) -> Any: # nasty any return
    content = IOUtil.ReadFile(filename)
    return json.loads(content)


def _WriteJsonFileIfChanged(filename: str, dict: Dict[str,str]) -> None:
    content = str(json.dumps(dict, ensure_ascii=False, indent=2))
    IOUtil.WriteFileUTF8IfChanged(filename, content)


class Resource(object):
    def __init__(self, sourcePath: str, relativeSkipChars: int) -> None:
        super().__init__()
        self.SourcePath = sourcePath
        self.SourceDirectory = IOUtil.GetDirectoryName(sourcePath)
        self.License = None  # type: Optional[JsonComplexLicense]
        self.RelativePath = sourcePath[relativeSkipChars:]


def _ScanForFiles(path: str, extensionList: List[str], ignoreFiles: List[str], ignoreDirs: List[str]) -> List[str]:
    foundFiles = []
    for root, dirs, files in os.walk(path):
        if ignoreDirs is not None:
            dirs[:] = [dir for dir in dirs if IOUtil.ToUnixStylePath(os.path.join(root, dir)) not in ignoreDirs]
        for file in files:
            fileId = file.lower()
            for extension in extensionList:
                if fileId.endswith(extension) and not fileId in ignoreFiles:
                    foundFiles.append(IOUtil.Join(root, file))
                    break
    return foundFiles


def _BuildFileLengthDict(files: List[str]) -> Dict[int,List[str]]:
    dict = {}  # type: Dict[int,List[str]]
    for file in files:
        fileLength = os.stat(file).st_size
        if fileLength in dict:
            dict[fileLength].append(file)
        else:
            dict[fileLength] = [file]
    return dict


def _BuildFileContentHashDict(files: List[str]) -> Dict[str, List[str]]:
    dict = {} # type: Dict[str, List[str]]
    for file in files:
        hash = IOUtil.HashFile(file)
        if hash in dict:
            dict[hash].append(file)
        else:
            dict[hash] = [file]
    return dict


def _BuildDuplicatedList(fileName: str, files: List[str]) -> List[str]:
    srcFilename = files[0]
    srcContentSet = set(IOUtil.ReadBinaryFile(srcFilename))
    matchingFiles = [fileName]
    for file in files:
        content = IOUtil.ReadBinaryFile(file)
        if len(srcContentSet.intersection(content)) == len(srcContentSet):
            matchingFiles.append(file)
    return matchingFiles


def _BuildDuplicatedDict(log: Log, files: List[str], uniqueFiles: List[str]) -> Dict[str, List[str]]:
    dict = {} # type: Dict[str, List[str]]
    while(len(files) > 1):
        srcFile = files[0]
        remainingFiles = files[1:]
        matchingFiles = _BuildDuplicatedList(srcFile, remainingFiles)
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


def _BuildUniqueFileDictByContent(log: Log, files: List[str], uniqueFiles: List[str]) -> Dict[str, List[str]]:
    # we start by sorting files by their hash
    # this should limit the amount of files that have to be byte compared quite a bit
    duplicationDict = {} # type: Dict[str, List[str]]
    dictHash = _BuildFileContentHashDict(files)
    for fileList in list(dictHash.values()):
        if len(fileList) > 1:
            newDuplicationDict = _BuildDuplicatedDict(log, fileList, uniqueFiles)
            duplicationDict.update(newDuplicationDict)
        else:
            uniqueFiles.append(fileList[0])
    return duplicationDict


def _BuildUniqueFileDict(log: Log, files: List[str], uniqueFiles: List[str]) -> Dict[str, List[str]]:
    # we start by sorting files by their size
    # this should limit the amount of files that have to be byte compared quite a bit
    dictFileLength = _BuildFileLengthDict(files)

    #log.LogPrint("Initial bins {0}".format(len(dictFileLength)))

    duplicationDict = {}
    for fileList in list(dictFileLength.values()):
        if len(fileList) > 1:
            newDuplicationDict = _BuildUniqueFileDictByContent(log, fileList, uniqueFiles)
            duplicationDict.update(newDuplicationDict)
        else:
            uniqueFiles.append(fileList[0])
    return duplicationDict



def _BuildExtensionDict(extensions: List[Tuple[str,str]]) -> Dict[str,str]:
    dict = {} # type: Dict[str,str]
    for extension in extensions:
        dict[extension[0]] = extension[1]
    return dict


def _GetContentTypeByExtension(extensionDict: Dict[str,str], filename: str) -> str:
    filenameExtension = IOUtil.GetFileNameExtension(filename).lower()
    return extensionDict[filenameExtension] if filenameExtension in extensionDict else ""


def _BuildResourceDirectorySet(uniqueFiles: List[str], duplicatedFilesDict: Dict[str, List[str]]) -> Set[str]:
    # build unique dir list
    resourceDirSet = set()  # type: Set[str]
    for entry in uniqueFiles:
        dirName = IOUtil.GetDirectoryName(entry)
        if not dirName in resourceDirSet:
            resourceDirSet.add(dirName)

    for fileList in list(duplicatedFilesDict.values()):
        for entry in fileList:
            dirName = IOUtil.GetDirectoryName(entry)
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

    def CreateDefaultLicense(self, origin: str, license: str) -> JsonBasicLicense:
        jsonDict = {} # type: Dict[str,str]
        jsonDict[self.KeyOrigin] = origin
        jsonDict[self.KeyLicense] = license

        jsonLicense = JsonBasicLicense(jsonDict)
        jsonLicense.Origin = jsonDict[self.KeyOrigin]
        jsonLicense.License = jsonDict[self.KeyLicense]
        return jsonLicense

    def WriteLicenseIfChanged(self, log: Log, dstFilename: str, srcComplexLicense: JsonComplexLicense) -> None:

        licenses = [] # type: List[Dict[str, str]]
        for license in srcComplexLicense.Licenses:
            licenseDict = {}
            if len(license.Origin) > 0:
                licenseDict[self.KeyOrigin] = license.Origin
            if len(license.License) > 0:
                licenseDict[self.KeyLicense] = license.License
            if len(license.Comment) > 0:
                licenseDict[self.KeyComment] = license.Comment
            if len(license.Tags) > 0:
                licenseDict[self.KeyTags] = license.Tags
            if len(license.Url) > 0:
                licenseDict[self.KeyURL] = license.Url
            if len(licenseDict) > 0:
                licenses.append(licenseDict)

        jsonLicenseDict = {} # type: Dict[str,Any]
        jsonLicenseDict[self.KeyComplexLicenses] = licenses
        if len(srcComplexLicense.Comment) > 0:
            jsonLicenseDict[self.KeyComplexComment] = srcComplexLicense.Comment

        jsonDict = {} # type: Dict[str,Any]
        jsonDict[self.KeyComplexLicense] = jsonLicenseDict

        _WriteJsonFileIfChanged(dstFilename, jsonDict)


    def TryReadLicense(self, log: Log, filename: str) -> Optional[JsonComplexLicense]:
        if not os.path.isfile(filename):
            return None

        content = None
        try:
            content = _ReadJsonFile(filename)
        except (Exception) as ex:
            print("ERROR: Exception while parsing {0}".format(filename))
            raise

        if self.KeyComplexLicense in content:
            return self.__TryParseComplexLicense(log, content, filename)

        if not self.KeyOrigin in content:
            log.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyOrigin, filename));
            return None
        if not self.KeyLicense in content:
            log.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyLicense, filename));
            return None
        basicLicense = self.__TryParseBasicLicense(log, content, filename)
        return  JsonComplexLicense([basicLicense], None) if basicLicense is not None else None

    def __TryParseBasicLicense(self, log: Log, jsonDict: Any, debugFilename: str) -> Optional[JsonBasicLicense]:
        license = JsonBasicLicense(jsonDict)
        license.Origin = jsonDict[self.KeyOrigin]
        license.License = jsonDict[self.KeyLicense]
        license.Comment = jsonDict[self.KeyComment] if self.KeyComment in jsonDict else ""
        license.Url = jsonDict[self.KeyURL] if self.KeyURL in jsonDict else ""
        license.SetTags(jsonDict[self.KeyTags] if self.KeyTags in jsonDict else "")
        return  license

    def __TryParseComplexLicense(self, log: Log, jsonDict: Any, debugFilename: str) -> Optional[JsonComplexLicense]:
        jsonDict = jsonDict[self.KeyComplexLicense]
        if not self.KeyComplexLicenses in jsonDict:
            log.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyComplexLicenses, debugFilename));
            return None

        licenses = [] # type: List[JsonBasicLicense]
        for entry in jsonDict[self.KeyComplexLicenses]:
            basicLicense = self.__TryParseBasicLicense(log, entry, debugFilename)
            if basicLicense is None:
                log.LogPrint("ERROR: Failed to parse complex license in file '{0}'".format(debugFilename));
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

    #        _WriteJsonFile(filename, license.Licenses[0].SourceDict)


    #def __AddKeyIfNeeded(self, dict, key, value):
    #    if len(value) <= 0:
    #        return
    #    dict[key] = value

def _HasScreenshot(package: Package, licenseConfig: LicenseConfig) -> bool:
    if package.AbsolutePath is None:
        return False
    screenshotName = IOUtil.Join(package.AbsolutePath, licenseConfig.ScreenshotName)
    return IOUtil.IsFile(screenshotName)


def _BuildPackageLicenseList(log: Log, package: Package, licenseConfig: LicenseConfig) -> List[JsonComplexLicense]:
    licenseManager = LicenseManager()
    licenseList = [] # type: List[JsonComplexLicense]

    if package.ResolvedContentBuilderAllInputFiles is not None:
        for fileEntry in package.ResolvedContentBuilderAllInputFiles:
            if fileEntry.ResolvedPath.endswith(licenseConfig.LicenseFilename):
                license = licenseManager.TryReadLicense(log, fileEntry.ResolvedPath)
                if license is None:
                    raise Exception("Failed to read license file: {0}".format(fileEntry.ResolvedPath))
                licenseList.append(license)

    if package.ResolvedContentFiles is not None:
        for fileEntry in package.ResolvedContentFiles:
            if fileEntry.ResolvedPath.endswith(licenseConfig.LicenseFilename):
                license = licenseManager.TryReadLicense(log, fileEntry.ResolvedPath)
                if license is None:
                    raise Exception("Failed to read license file: {0}".format(fileEntry.ResolvedPath))
                licenseList.append(license)

    # Add the default license for executables unless we dont have any other licenses and no screenshot was found
    if package.Type == PackageType.Executable and (_HasScreenshot(package, licenseConfig) or len(licenseList) > 0):
        licenseManager = LicenseManager()
        defaultLicense = licenseManager.CreateDefaultLicense(licenseConfig.DefaultOrigin, licenseConfig.DefaultLicense)
        defaultLicenses = [defaultLicense]
        complexDefaultLicense = JsonComplexLicense(defaultLicenses, None)
        licenseList.append(complexDefaultLicense)

    return licenseList


def _BuildDirectoryLicenseDict(log: Log, resourceDirectories: Set[str], licenseFilename: str) -> Dict[str, JsonComplexLicense]:
    licenseManager = LicenseManager()
    licenseDict = {} # type: Dict[str, JsonComplexLicense]
    for dir in resourceDirectories:
        license = licenseManager.TryReadLicense(log, IOUtil.Join(dir, licenseFilename))
        if license is not None:
            licenseDict[dir] = license
    return licenseDict


def _TagListWithLicenses(inputDirectory: str, files: List[str], directoryLicenseDict: Dict[str, JsonComplexLicense]) -> List[Resource]:
    inputDirectory = IOUtil.NormalizePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = []
    for entry in files:
        resource = Resource(entry, skipChars)
        if resource.SourceDirectory in directoryLicenseDict:
            resource.License = directoryLicenseDict[resource.SourceDirectory]
        res.append(resource)
    return res;


def _TagDictWithLicenses(inputDirectory: str, fileDict: Dict[str, List[str]], directoryLicenseDict: Dict[str, JsonComplexLicense]) -> Dict[str, List[Resource]]:
    inputDirectory = IOUtil.NormalizePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = {} # type: Dict[str, List[Resource]]
    for key, value in fileDict.items():
        keyFilename = key[skipChars:]
        res[keyFilename] = _TagListWithLicenses(inputDirectory, value, directoryLicenseDict)
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



def _WriteCSV(dstFilename: str, extensions: List[Tuple[str,str]], uniqueEntries: List[Resource], duplicatedEntryDict: Dict[str, List[Resource]]) -> None:
    #count = len(uniqueFiles)
    #for list in duplicatedFilesDict.values():
    #  count += len(list)
    #log.LogPrint("Found {0} resource files".format(count))

    uniqueEntries.sort(key=lambda s: s.SourcePath.lower())
    sortedDuplicatedFiles = list(duplicatedEntryDict.keys())
    sortedDuplicatedFiles.sort()
    for fileList in list(duplicatedEntryDict.values()):
        fileList.sort(key=lambda s: s.SourcePath.lower());

    extensionDict = _BuildExtensionDict(extensions)

    lines = []
    lines.append("Unique files ({0});;Origin;License;Type;Comment;URL".format(len(uniqueEntries)))
    for entry in uniqueEntries:
        contentType = _GetContentTypeByExtension(extensionDict, entry.RelativePath)
        if entry.License is None:
            lines.append("{0};;;;{1};;".format(entry.RelativePath, contentType))
        else:
            strOrigin, strLicense, strComment, strUrl = _Flatten(entry.License)
            lines.append("{0};;{1};{2};{3};{4};{5}".format(entry.RelativePath, strOrigin, strLicense, contentType, strComment, strUrl))

    lines.append("\n")
    lines.append("Duplicated files ({0})".format(len(duplicatedEntryDict)))
    for key in sortedDuplicatedFiles:
        lines.append("{0};;;;{1};;".format(key, _GetContentTypeByExtension(extensionDict, key)))
        for entry in duplicatedEntryDict[key]:
            contentType = _GetContentTypeByExtension(extensionDict, entry.RelativePath)
            if entry.License is None:
                lines.append(";{0};;;{1};;".format(entry.RelativePath, contentType))
            else:
                strOrigin, strLicense, strComment, strUrl = _Flatten(entry.License)
                lines.append(";{0};{1};{2};{3};{4};{5}".format(entry.RelativePath, strOrigin, strLicense, contentType, strComment, strUrl))

    IOUtil.WriteFile(dstFilename, "\n".join(lines));




def _PrintIssueDirectories(fileList: List[Resource], dict: Dict[str,List[Resource]]) -> None:
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


def _ProcessDictLicenses(log: Log, licenseFilename: str, dict: Dict[str, List[Resource]]) -> None:
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
            log.LogPrint("Info: Found duplicated resource missing a license, cloning source license from {0}".format(firstLicenseEntry.SourcePath))
            for entry in noLicenseEntries:
                entry.License = firstLicenseEntry.License
                if not entry.SourceDirectory in newLicenseDirs:
                    newLicenseDirs.add(entry.SourceDirectory)
                    newLicenseFile = IOUtil.Join(entry.SourceDirectory, licenseFilename)
                    if os.path.isfile(newLicenseFile):
                        raise Exception("Could not create a new license at {0} since one already exist".format(newLicenseFile))
                    if firstLicenseEntry.License is None:
                        raise Exception("internal error")

                    oldLicenseFile = IOUtil.Join(firstLicenseEntry.SourceDirectory, licenseFilename)

                    IOUtil.CopySmallFile(oldLicenseFile, newLicenseFile)
                    # licenseManager.SaveLicense(newLicenseFile, firstLicenseEntry.License)


def _FilterDictBasedOnLicense(dict: Dict[str, List[Resource]]) -> Dict[str,List[Resource]]:
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


def _PrintListFixTags(entries: List[Resource]) -> None:
    for entry in entries:
        if entry.License is not None:
            for licenseEntry in entry.License.Licenses:
                if licenseEntry is not None and "fix" in licenseEntry.TagsIdList:
                    print("Fix: {0}".format(entry.SourcePath))


def _PrintFixTags(uniqueEntries: List[Resource], duplicatedEntriesDict: Dict[str, List[Resource]]) -> None:
    _PrintListFixTags(uniqueEntries)
    for entries in list(duplicatedEntriesDict.values()):
        _PrintListFixTags(entries)


def _AddLicenses(dict: Dict[str, List[Resource]], entries: List[Resource]) -> None:
    for entry in entries:
        if entry.License is not None:
            for licenseEntry in entry.License.Licenses:
                if licenseEntry is not None:
                    if not licenseEntry.License in dict:
                        dict[licenseEntry.License] = [entry]
                    else:
                        dict[licenseEntry.License].append(entry)


def _ExpandLicense(key: str) -> bool:
    key = key.lower()
    return key != 'bsd-3-clause' and key != "mit" and key != "mixed" and key != "cc0-1.0" and key != "cc-by-3.0" and key != "cc-by-sa-4.0" and key != "modified 3-clause bsd-license"


def _PrintLicenses(uniqueEntries: List[Resource], duplicatedEntriesDict: Dict[str, List[Resource]]) -> None:
    licenseDict = {}  # type: Dict[str, List[Resource]]
    _AddLicenses(licenseDict, uniqueEntries)
    for entries in list(duplicatedEntriesDict.values()):
        _AddLicenses(licenseDict, entries)

    sortedKeys = list(licenseDict.keys())
    sortedKeys.sort()

    print("License")
    for key in sortedKeys:
        value = licenseDict[key]
        print("- '{0}' entries: {1}".format(key, len(value)))
        if _ExpandLicense(key):
            for entry in value:
                print("  * {0}".format(entry.SourcePath))


def _Process(log: Log, ignoreDirList: List[str], inputDirectory: str, extensions: List[Tuple[str,str]],
            ignoreFiles: List[str], licenseFilename: str, listLicenses: bool,
            saveCSVs: bool) -> None:
    if not os.path.isdir(inputDirectory):
        raise Exception("'{0}' is not a directory".format(inputDirectory));

    print("Please run this on a clean checkout as compiler obj files could be found otherwise.")
    extensionList = _GetExtensionList(extensions)
    files = _ScanForFiles(inputDirectory, extensionList, ignoreFiles, ignoreDirList)
    log.LogPrint("Found {0} resource files".format(len(files)))

    uniqueFiles = [] # type: List[str]
    duplicatedFilesDict = _BuildUniqueFileDict(log, files, uniqueFiles)

    log.LogPrint("Found {0} unique resource files".format(len(uniqueFiles)))
    log.LogPrint("Found {0} duplicated resource files".format(len(duplicatedFilesDict)))

    # Check license information
    resourceDirectories = _BuildResourceDirectorySet(uniqueFiles, duplicatedFilesDict)
    directoryLicenseDict = _BuildDirectoryLicenseDict(log, resourceDirectories, licenseFilename)

    log.LogPrint("Found {0} license files".format(len(directoryLicenseDict)))

    uniqueEntries = _TagListWithLicenses(inputDirectory, uniqueFiles, directoryLicenseDict)
    duplicatedEntriesDict = _TagDictWithLicenses(inputDirectory, duplicatedFilesDict, directoryLicenseDict)

    _ProcessDictLicenses(log, licenseFilename, duplicatedEntriesDict)

    # All resources
    if saveCSVs:
        _WriteCSV("resources.csv", extensions, uniqueEntries, duplicatedEntriesDict)

    # Remove all entries that have a license associated
    noLicenseUniqueEntries = [entry for entry in uniqueEntries if entry.License is None]
    noLicenseDuplicatedEntriesDict = _FilterDictBasedOnLicense(duplicatedEntriesDict)

    if len(noLicenseUniqueEntries) > 0:
        print("WARNING: Found {0} unique resource files with no license attached".format(len(noLicenseUniqueEntries)))
    if len(noLicenseDuplicatedEntriesDict) > 0:
        print("WARNING: Found {0} duplicated resource files with no license attached".format(len(noLicenseDuplicatedEntriesDict)))

    if saveCSVs:
        _WriteCSV("resourcesIssues.csv", extensions, noLicenseUniqueEntries, noLicenseDuplicatedEntriesDict)

    _PrintIssueDirectories(noLicenseUniqueEntries, noLicenseDuplicatedEntriesDict)

    _PrintFixTags(uniqueEntries, duplicatedEntriesDict)

    if listLicenses:
        _PrintLicenses(uniqueEntries, duplicatedEntriesDict)

def _ScanPackageContent(log: Log, licenseConfig: LicenseConfig, scanPackageList: List[Package], disableWrite: bool) -> None:
    for package in scanPackageList:
        packageLicenseList = _BuildPackageLicenseList(log, package, licenseConfig)

        if len(packageLicenseList) > 0:
            # Create initial empty license
            packageLicense = JsonComplexLicense([], "Merged complex license for everything that could be used in a screenshot")
            for license in packageLicenseList:
                packageLicense.Merge(license)

            licenseManager = LicenseManager()
            if not disableWrite and package.AbsolutePath is not None:
                dstFilename = IOUtil.Join(package.AbsolutePath, licenseConfig.LicenseFilename)
                licenseManager.WriteLicenseIfChanged(log, dstFilename, packageLicense)
        elif package.Type == PackageType.Executable and package.AbsolutePath is not None and not disableWrite:
            # no licenses, so remove the license if it exist
            dstFilename = IOUtil.Join(package.AbsolutePath, licenseConfig.LicenseFilename)
            if IOUtil.IsFile(dstFilename):
                IOUtil.RemoveFile(dstFilename);

def GetExtensionList() -> List[str]:
    return _GetExtensionList(__g_extensions)


def Scan(log: Log, miniToolConfig: ToolMinimalConfig, directory: str, scanPackageList: List[Package], repairEnabled: bool, disableWrite: bool,
         listLicenses: bool, saveCSVs: bool, licenseConfig: LicenseConfig) -> None:
    """
    Run through all resource files and check if the licenses are specified.
    """

    #log: Log, ignoreDirList: List[str], inputDirectory: str, extensions: List[Tuple[str,str]],
    #        ignoreFiles: List[str], licenseFilename: str, listLicenses: List[str],
    #        saveCSVs: bool

    log.LogPrint("Building package content licenses")
    _ScanPackageContent(log, licenseConfig, scanPackageList, disableWrite)

    log.LogPrint("Running resource license scan")

    ignoreDirs = list(miniToolConfig.IgnoreDirectories)

    for rootDirectory in miniToolConfig.RootDirectories:
        if rootDirectory.BashName == "$FSL_GRAPHICS_SDK":
            for ignoreDir in __g_ignoreDir:
                ignoreDirs.append(IOUtil.Join(rootDirectory.ResolvedPath, ignoreDir))

    _Process(log, ignoreDirs, directory, __g_extensions, __g_ignore, licenseConfig.LicenseFilename, listLicenses, saveCSVs)


