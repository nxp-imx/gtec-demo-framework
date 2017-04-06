#!/usr/bin/env python
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

import argparse
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
    ]
__g_ignore = "example.jpg"

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
    def __init__(self, verbosityLevel):
        super(Config, self).__init__()
        self.VerbosityLevel = verbosityLevel
        self.IsVerbose = verbosityLevel > 0

    def LogPrint(self, str):
        if self.IsVerbose:
            print(str)
            sys.stdout.flush()


class JsonLicense(object):
    def __init__(self, sourceDict = {}):
        super(JsonLicense, self).__init__()
        self.Origin = ""
        self.License = ""
        self.Url = ""
        self.Tags = ""
        self.TagsIdList = []
        self.SourceDict = sourceDict

    def SetTags(self, tags):
        self.Tags = tags
        self.TagsIdList = [entry.lower() for entry in tags.split(';') if len(entry) > 0]

    def Compare(self, license):
        return self.Origin == license.Origin and  self.License == license.License and self.Url == license.Url and self.Tags == license.Tags and self.SourceDict == license.SourceDict


def GetExtensionList(extensions):
    return [extension[0] for extension in extensions]


def GetTitle():
    return 'FslResourceScan V0.0.5 alpha'


def ShowTitleIfNecessary():
    global __g_verbosityLevel
    if __g_verbosityLevel > 0:
        print(GetTitle())


def ReadBinaryFile(filename):
    content = None
    with open(filename, "rb") as theFile:
        content = theFile.read()
    return content


def WriteFile(filename, content):
    with open(filename, "w") as theFile:
        theFile.write(content)


def ReadJsonFile(filename):
    content = ReadBinaryFile(filename)
    return json.loads(content)


def WriteJsonFile(filename, dict):
    with io.open(filename, 'w', encoding='utf-8') as currentFile:
        currentFile.write(unicode(json.dumps(dict, ensure_ascii=False, indent=2)))

def ToUnixStylePath(path):
    if path == None:
        return None
    return path.replace("\\", "/")


def GetDirectoryName(path):
    return ToUnixStylePath(os.path.dirname(path))


def Join(path1, path2):
    return ToUnixStylePath(os.path.join(path1, path2))

class Resource(object):
    def __init__(self, sourcePath, relativeSkipChars):
        super(Resource, self).__init__()
        self.SourcePath = sourcePath
        self.SourceDirectory = GetDirectoryName(sourcePath)    
        self.License = None
        self.RelativePath = sourcePath[relativeSkipChars:]


def ScanForFiles(path, extensionList, ignoreFiles):    
    foundFiles = []
    for root, dirs, files in os.walk(path):
        for file in files:
            fileId = file.lower()
            for extension in extensionList:
                if fileId.endswith(extension) and not fileId in ignoreFiles:
                    foundFiles.append(ToUnixStylePath(os.path.join(root, file)))
                    break
    return foundFiles


def HashFile(filename, blocksize=65536):
    hasher = hashlib.sha256()
    with open(filename, "rb") as theFile:
        buf = theFile.read(blocksize)
        while len(buf) > 0:
            hasher.update(buf)
            buf = theFile.read(blocksize)
        return hasher.hexdigest()


def BuildFileContentHashDict(config, files):
    dictHash = {}
    for file in files:
        hash = HashFile(file)
        if hash in dictHash:
            dictHash[hash].append(file)
        else:
            dictHash[hash] = [file]
    return dictHash


def BuildFileLengthDict(files):
    dict = {}
    for file in files:
        fileLength = os.stat(file).st_size
        if fileLength in dict:
            dict[fileLength].append(file)
        else:
            dict[fileLength] = [file]
    return dict


def BuildFileContentHashDict(files):
    dict = {}
    for file in files:
        hash = HashFile(file)
        if hash in dict:
            dict[hash].append(file)
        else:
            dict[hash] = [file]
    return dict


def BuildDuplicatedList(fileName, files):
    srcFilename = files[0]
    srcContentSet = set(ReadBinaryFile(srcFilename))
    matchingFiles = [fileName]
    for file in files:
        content = ReadBinaryFile(file)
        if len(srcContentSet.intersection(content)) == len(srcContentSet):
            matchingFiles.append(file)
    return matchingFiles


def BuildDuplicatedDict(config, files, uniqueFiles):
    dict = {}
    while(len(files) > 1):
        srcFile = files[0]
        remainingFiles = files[1:]
        matchingFiles = BuildDuplicatedList(srcFile, remainingFiles) 
        if len(matchingFiles) > 1:
            dict[srcFile] = matchingFiles
        else:
            uniqueFiles.append(key)
        
        # Remove all non duplicated files
        files.remove(srcFile)
        files = []
        for file in remainingFiles:
            if not file in matchingFiles:
                files.append(file)    
    return dict


def BuildUniqueFileDictByContent(config, files, uniqueFiles):
    # we start by sorting files by their hash
    # this should limit the amount of files that have to be byte compared quite a bit
    duplicationDict = {}
    dictHash = BuildFileContentHashDict(files)
    for fileList in dictHash.values():
        if len(fileList) > 1:
            newDuplicationDict = BuildDuplicatedDict(config, fileList, uniqueFiles)
            duplicationDict.update(newDuplicationDict)
        else:
            uniqueFiles.append(fileList[0])
    return duplicationDict


def BuildUniqueFileDict(config, files, uniqueFiles):
    # we start by sorting files by their size
    # this should limit the amount of files that have to be byte compared quite a bit
    dictFileLength = BuildFileLengthDict(files)

    #config.LogPrint("Initial bins {0}".format(len(dictFileLength)))
    
    duplicationDict = {}
    for fileList in dictFileLength.values():
        if len(fileList) > 1:
            newDuplicationDict = BuildUniqueFileDictByContent(config, fileList, uniqueFiles)
            duplicationDict.update(newDuplicationDict)
        else:
            uniqueFiles.append(fileList[0])
    return duplicationDict


def GetFileExtension(filename):
    filename, fileExtension = os.path.splitext(filename)
    return fileExtension


def BuildExtensionDict(extensions):
    dict = {}
    for extension in extensions:
        dict[extension[0]] = extension[1]
    return dict


def GetContentTypeByExtension(extensionDict, filename):
    filenameExtension = GetFileExtension(filename).lower()
    return extensionDict[filenameExtension] if filenameExtension in extensionDict else ""


def BuildResourceDirectorySet(uniqueFiles, duplicatedFilesDict):
    # build unique dir list
    resourceDirSet = set()
    for entry in uniqueFiles:
        dirName = GetDirectoryName(entry)
        if not dirName in resourceDirSet:
            resourceDirSet.add(dirName)

    for fileList in duplicatedFilesDict.values():
        for entry in fileList:
            dirName = GetDirectoryName(entry)
            if not dirName in resourceDirSet:
                resourceDirSet.add(dirName)
    return resourceDirSet


class LicenseManager(object):
    def __init__(self):
        super(LicenseManager, self).__init__()
        self.KeyOrigin = "Origin"
        self.KeyLicense = "License"
        self.KeyComment = "Comment"
        self.KeyTags = "Tags"
        self.KeyURL = "URL"

    def TryReadLicense(self, config, filename):
        if not os.path.isfile(filename):
            return None

        content = None
        try:
            content = ReadJsonFile(filename)
        except (Exception) as ex:
            print("ERROR: Exception while parsing {0}".format(filename))
            raise

        if not self.KeyOrigin in content:
            config.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyOrigin, filename));
            return None
        if not self.KeyLicense in content:
            config.LogPrint("ERROR: '{0}' not present in file '{1}'".format(self.KeyLicense, filename));
            return None
        
        license = JsonLicense(content)
        license.Origin = content[self.KeyOrigin]
        license.License = content[self.KeyLicense]
        license.Comment = content[self.KeyComment] if self.KeyComment in content else ""
        license.URL = content[self.KeyURL] if self.KeyURL in content else ""
        license.SetTags(content[self.KeyTags] if self.KeyTags in content else "")
        return license


    def SaveLicense(self, filename, license):
        #contentDict = {}
        #self.__AddKeyIfNeeded(contentDict, self.KeyOrigin, license.Origin)
        #self.__AddKeyIfNeeded(contentDict, self.KeyLicense, license.License)
        #self.__AddKeyIfNeeded(contentDict, self.KeyURL, license.URL)

        WriteJsonFile(filename, license.SourceDict)


    def __AddKeyIfNeeded(self, dict, key, value):
        if len(value) <= 0:
            return
        dict[key] = value


def BuildDirectoryLicenseDict(config, resourceDirectories, licenseFilename):
    licenseManager = LicenseManager()
    licenseDict = {}
    for dir in resourceDirectories:
        license = licenseManager.TryReadLicense(config, Join(dir, licenseFilename))
        if license != None:        
            licenseDict[dir] = license
    return licenseDict


def TagListWithLicenses(inputDirectory, files, directoryLicenseDict):
    inputDirectory = ToUnixStylePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = []    
    for entry in files:
        resource = Resource(entry, skipChars)
        if resource.SourceDirectory in directoryLicenseDict:
            resource.License = directoryLicenseDict[resource.SourceDirectory]
        res.append(resource)
    return res;


def TagDictWithLicenses(inputDirectory, fileDict, directoryLicenseDict):
    inputDirectory = ToUnixStylePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = {}    
    for key, value in fileDict.iteritems():
        keyFilename = key[skipChars:]
        res[keyFilename] = TagListWithLicenses(inputDirectory, value, directoryLicenseDict)
    return res;


def WriteCSV(dstFilename, extensions, uniqueEntries, duplicatedEntryDict):
    #count = len(uniqueFiles)
    #for list in duplicatedFilesDict.values():
    #  count += len(list)  
    #config.LogPrint("Found {0} resource files".format(count))

    uniqueEntries.sort(key=lambda s: s.SourcePath.lower())
    sortedDuplicatedFiles = list(duplicatedEntryDict.keys())
    sortedDuplicatedFiles.sort()
    for fileList in duplicatedEntryDict.values():
        fileList.sort(key=lambda s: s.SourcePath.lower());

    extensionDict = BuildExtensionDict(extensions)

    lines = []   
    lines.append("Unique files ({0});;Origin;License;Type;Comment;URL".format(len(uniqueEntries)))
    for entry in uniqueEntries:
        contentType = GetContentTypeByExtension(extensionDict, entry.RelativePath)
        if entry.License == None:
            lines.append("{0};;;;{1};;".format(entry.RelativePath, contentType))
        else:
            lines.append("{0};;{1};{2};{3};{4};{5}".format(entry.RelativePath, entry.License.Origin, entry.License.License, contentType, entry.License.Comment, entry.License.URL))

    lines.append("\n")
    lines.append("Duplicated files ({0})".format(len(duplicatedEntryDict)))
    for key in sortedDuplicatedFiles:
        lines.append("{0};;;;{1};;".format(key, GetContentTypeByExtension(extensionDict, key)))
        for entry in duplicatedEntryDict[key]:
            contentType = GetContentTypeByExtension(extensionDict, entry.RelativePath)
            if entry.License == None:
                lines.append(";{0};;;{1};;".format(entry.RelativePath, contentType))
            else:
                lines.append(";{0};{1};{2};{3};{4};{5}".format(entry.RelativePath, entry.License.Origin, entry.License.License, contentType, entry.License.Comment, entry.License.URL))

    WriteFile(dstFilename, "\n".join(lines));


def PrintIssueDirectories(fileList, dict):
    uniqueDirs = set()
    for entry in fileList:
        if not entry.SourceDirectory in uniqueDirs:
            uniqueDirs.add(entry.SourceDirectory)

    for value in dict.values():
        for entry in value:
            if not entry.SourceDirectory in uniqueDirs:
                uniqueDirs.add(entry.SourceDirectory)

    if len(uniqueDirs) > 0:
        print("Investigate license for the following directories:")
        uniqueDirs = list(uniqueDirs)
        uniqueDirs.sort()
        for entry in uniqueDirs:
            print("  {0}".format(entry))


def Filter(config, ignoreDirList, inputDirectory, files):
    inputDirectory = ToUnixStylePath(inputDirectory)
    skipChars = len(inputDirectory) if inputDirectory.endswith('/') else len(inputDirectory)+1

    res = []
    for entry in files:
        relativeName = entry[skipChars:]
        if not GetDirectoryName(relativeName) in ignoreDirList:
            res.append(entry)
    return res


def ProcessDictLicenses(config, licenseFilename, dict):
    licenseManager = LicenseManager()
    newLicenseDirs = set()
    for key, entryList in dict.iteritems():
        firstLicenseEntry = None
        noLicenseEntries = []
        for entry in entryList:
            if entry.License == None:
                noLicenseEntries.append(entry)
            elif firstLicenseEntry == None:
                firstLicenseEntry = entry
            elif not firstLicenseEntry.License.Compare(entry.License):
                raise Exception("The license of the duplicated resource at {0} and {1} is different".format(firstLicenseEntry.SourceDirectory, entry.SourceDirectory))

        if len(noLicenseEntries) > 0 and firstLicenseEntry != None:
            config.LogPrint("Info: Found duplicated resource missing a license, cloning source license from {0}".format(firstLicenseEntry.SourcePath))
            for entry in noLicenseEntries:
                entry.License = firstLicenseEntry.License
                if not entry.SourceDirectory in newLicenseDirs:
                    newLicenseDirs.add(entry.SourceDirectory)
                    newLicenseFile = Join(entry.SourceDirectory, licenseFilename)
                    if os.path.isfile(newLicenseFile):
                        raise Exception("Could not create a new license at {0} since one already exist".format(newLicenseFile))
                    licenseManager.SaveLicense(newLicenseFile, firstLicenseEntry.License)


def FilterDictBasedOnLicense(dict):
    newDict = {}
    for key, entryList in dict.iteritems():
        newList = []
        firstLicenseEntry = None
        for entry in entryList:
            if entry.License == None:
                newList.append(entry)
        if len(newList) > 0:
            if len(entryList) != len(newList):
                raise Exception("Internal error, license for duplicated files have not been fixed prior to this filtering")
            newDict[key] = newList
    return newDict


def PrintListFixTags(entries):
    for entry in entries:
        if entry.License != None and "fix" in entry.License.TagsIdList:
            print("Fix: {0}".format(entry.SourcePath))


def PrintFixTags(uniqueEntries, duplicatedEntriesDict):
    PrintListFixTags(uniqueEntries)
    for entries in duplicatedEntriesDict.values():
        PrintListFixTags(entries)


def AddLicenses(dict, entries):
    for entry in entries:
        if entry.License != None:
            if not entry.License.License in dict:
                dict[entry.License.License] = [entry]
            else:
                dict[entry.License.License].append(entry)


def ExpandLicense(key):
    key = key.lower()
    return key != 'bsd-3-clause' and key != "mit" and key != "mixed" and key != "cc0-1.0" and key != "cc-by-3.0" and key != "cc-by-sa-4.0" and key != "modified 3-clause bsd-license"


def PrintLicenses(uniqueEntries, duplicatedEntriesDict):
    licenseDict = {}
    AddLicenses(licenseDict, uniqueEntries)
    for entries in duplicatedEntriesDict.values():
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


def Process(config, ignoreDirList, inputDirectory, extensions, ignoreFiles, licenseFilename, listLicenses):
    if not os.path.isdir(inputDirectory):
        raise Exception("'{0}' is not a directory".format(inputDirectory));

    print("Please run this on a clean checkout as compiler obj files could be found otherwise.")
    extensionList = GetExtensionList(extensions)
    allFiles = ScanForFiles(inputDirectory, extensionList, ignoreFiles)
    config.LogPrint("Found {0} resource files".format(len(allFiles)))

    files = Filter(config, ignoreDirList, inputDirectory, allFiles)
    if len(files) != len(allFiles):
        config.LogPrint("Skipping {0} files since they are part of the ignored directory list".format(len(allFiles)-len(files)))

    uniqueFiles = []
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
    WriteCSV("resources.csv", extensions, uniqueEntries, duplicatedEntriesDict)
    
    # Remove all entries that have a license associated
    noLicenseUniqueEntries = [entry for entry in uniqueEntries if entry.License == None]
    noLicenseDuplicatedEntriesDict = FilterDictBasedOnLicense(duplicatedEntriesDict)

    if len(noLicenseUniqueEntries) > 0:
        print("WARNING: Found {0} unique resource files with no license attached".format(len(noLicenseUniqueEntries)))
    if len(noLicenseDuplicatedEntriesDict) > 0:
        print("WARNING: Found {0} duplicated resource files with no license attached".format(len(noLicenseDuplicatedEntriesDict)))

    WriteCSV("resourcesIssues.csv", extensions, noLicenseUniqueEntries, noLicenseDuplicatedEntriesDict)

    PrintIssueDirectories(noLicenseUniqueEntries, noLicenseDuplicatedEntriesDict)

    PrintFixTags(uniqueEntries, duplicatedEntriesDict)

    if listLicenses:
        PrintLicenses(uniqueEntries, duplicatedEntriesDict)


def AddDefaultOptions(parser):
    parser.add_argument('-v', '--verbosity', action='count', default=0, help='Set verbosity level')
    parser.add_argument('--debug', action='store_true',  help='Enable script debugging')


def EarlyArgumentParser():
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
        print("ERROR: %s" % ex.message)
        if __g_debugEnabled:
            raise
        else:
            return False
    return True


def AddArgSubParser(subparsers, cmdName, cmdHelp):
    return parser


def Main():
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
    AddDefaultOptions(parser)


    try:
        args = parser.parse_args()
        config = Config(args.verbosity)
        Process(config, __g_ignoreDir, args.directory, __g_extensions, __g_ignore, __g_licenseFilename, args.list)
    except (Exception) as ex:
        ShowTitleIfNecessary()
        print("ERROR: %s" % ex.message)
        if __g_debugEnabled:
            raise
    return

Main()