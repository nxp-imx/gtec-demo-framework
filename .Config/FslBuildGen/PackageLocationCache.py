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

from typing import Dict
from typing import List
from typing import Optional
from typing import Set
from typing import Tuple
#from typing import Deque
#from collections import deque
import copy
import difflib
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.DataTypes import ScanMethod
from FslBuildGen.Exceptions import PackageHasMultipleDefinitionsException
from FslBuildGen.Log import Log
from FslBuildGen.ToolConfig import ToolConfigPackageLocation
from FslBuildGen.ToolConfig import ToolConfigPackageLocationBlacklistEntry

g_verbosityMaxLevel = 4


class PackageLocationCachePath(object):
    def __init__(self, packageName: str, absolutePath: str, foundPackageFilePath: Optional[str], sourceLocation: ToolConfigPackageLocation) -> None:
        """ if foundPackageFilePath is set, this means that the package file was located under this cached path """
        self.PackageName = packageName
        self.AbsolutePath = absolutePath
        self.FoundPackageFilePath = foundPackageFilePath
        self.SourceLocation = sourceLocation


class PackageLocationCacheRecord(object):
    def __init__(self, name: str) -> None:
        self.Name = name
        self.PathList = [] # type: List[PackageLocationCachePath]
        # The location of a package with this name
        # - if this is None -> no such package exist
        self.PackagePath = None   # type: Optional[PackageLocationCachePath]


    def Append(self, packageLocationCachePath: PackageLocationCachePath) -> None:
        if packageLocationCachePath.FoundPackageFilePath is not None:
            if self.PackagePath is not None:
                raise PackageHasMultipleDefinitionsException("Two files tried to define the package name '{0}', file1: '{1}' file2: '{2}'".format(self.Name, self.PackagePath.FoundPackageFilePath, packageLocationCachePath.FoundPackageFilePath))
            self.PackagePath = packageLocationCachePath
        self.PathList.append(packageLocationCachePath)


class PackageLocationCandidates(object):
    def __init__(self) -> None:
        # We dont use a deque here since MyPy doesnt like it atm
        #self.__ScannedPaths = set()     # type: Set[str]
        self.__List = []                # type: List[PackageLocationCachePath]
        self.NewLocations = []          # type: List[PackageLocationCachePath]


    def Clear(self) -> None:
        """ Clear the queue """
        #self.__ScannedPaths.clear()
        self.__List.clear()
        self.NewLocations.clear()


    def IsEmpty(self) -> bool:
        """ Check if the queue is empty """
        return len(self.__List) <= 0


    def Append(self, location: PackageLocationCachePath) -> None:
        """ Append the location to the end of the queue """
        #if location.AbsolutePath in self.__ScannedPaths:
        #    return
        self.__List.append(location)


    def Insert(self, location: PackageLocationCachePath) -> None:
        """ Find the best location to add the element based on the existing content """
        #if location.AbsolutePath in self.__ScannedPaths:
        #    return

        index = self.__LocateInsertIndex(location)
        self.__List.insert(index, location)


    def TryPopFront(self) -> Optional[PackageLocationCachePath]:
        if len(self.__List) <= 0:
            return None
        result = self.__List.pop(0)
        #self.__ScannedPaths.add(result.AbsolutePath)
        return result


    def __LocateInsertIndex(self, location: PackageLocationCachePath) -> int:
        for index, entry in enumerate(self.__List):
            if len(entry.PackageName) < len(location.PackageName):
                return index
        return len(self.__List)


class PackageLocationCache(object):
    def __init__(self, log: Log, packageLocations: List[ToolConfigPackageLocation], genFilename: str) -> None:
        self.Log = log
        self.__PackageLocations = packageLocations
        self.__GenFilename = genFilename
        # Package name to location matching, this contains all entries also the ones that dont have a package
        self.__LocationDict = {}  # type: Dict[str, PackageLocationCacheRecord]
        self.__ScannedPathsCacheSet = set()  # type: Set[str]
        # Contains the paths thare are 'root' locations
        self.__RootLocationPaths = set()  # type: Set[str]
        self.__TryLocatePackageScratchpad = PackageLocationCandidates()   # type: PackageLocationCandidates

        for entry in packageLocations:
            self.__RootLocationPaths.add(entry.ResolvedPath)

        # Warmup the cache by scanning the initial locations
        self.__CacheInitialLocations(packageLocations)

    def SYS_TestAccess_GetLocationDict(self) -> Dict[str, PackageLocationCacheRecord]:
        return self.__LocationDict


    def GetKnownPackageLocations(self) -> List[PackageLocationCachePath]:
        """ Get all the currently known package locations """
        return [record.PackagePath for record in self.__LocationDict.values() if record.PackagePath is not None]


    def CacheEverything(self) -> None:
        """ Cache all subdirectories.
            This is useful if you want to create a package candidate list to provide a helpful error message.
        """
        self.Log.LogPrintVerbose(g_verbosityMaxLevel, "- Caching everything")
        try:
            self.Log.PushIndent()

            sourcePackageLocationCacheRecords = copy.deepcopy(list(self.__LocationDict.values())) # type: List[PackageLocationCacheRecord]
            for sourceRecord in sourcePackageLocationCacheRecords:
                for packageLocation in sourceRecord.PathList:
                    newLocationPackageName = packageLocation.PackageName + "."
                    self.__CacheLocation(self.__ScannedPathsCacheSet, self.__LocationDict, newLocationPackageName, packageLocation.AbsolutePath, ScanMethod.AllSubDirectories, packageLocation.SourceLocation)
        finally:
            self.Log.PopIndent()


    def FindCandidates(self, packageName: str, automaticallyCacheEverything: bool = False) -> List[str]:
        """ Look at the cached packages and try to create candidate list of packages that have similar names """
        if automaticallyCacheEverything:
            self.CacheEverything()

        if self.Log.Verbosity >= g_verbosityMaxLevel:
            self.Log.LogPrint("- Finding candidates for: {0}".format(packageName))
        allValidPackageList = [record.Name for record in self.__LocationDict.values() if record.PackagePath is not None]

        # Check for a exact match first
        if packageName in allValidPackageList:
            return [packageName]
        # Then try to guess
        candidateList = self.__FindCandidatesThatMatchPackageEnding(allValidPackageList, packageName)
        candidateList += self.__FindCandidatesThatAreClose(allValidPackageList, packageName, candidateList)
        return candidateList


    def __FindCandidatesThatMatchPackageEnding(self, allValidPackageList: List[str], packageName: str) -> List[str]:
        """ Find all packages has a end prefix that match the name 100% """
        resultList = []  # type: List[str]
        endName = "." + packageName
        for name in allValidPackageList:
            if name.endswith(endName):
                resultList.append(name)
        return resultList


    def __FindCandidatesThatAreClose(self, allValidPackageList: List[str], packageName: str, ignoreList: List[str], matchRating: float = 0.70) -> List[str]:
        """ Find all candidates that have a close matchRating to the name we are looking for """
        ratingList = []  # type: List[Tuple[float, str]]
        for name in allValidPackageList:
            if name not in ignoreList:
                ratio = difflib.SequenceMatcher(None, packageName, name).ratio()
                if ratio > matchRating:
                    ratingList.append((ratio, name))

        ratingList.sort(key=lambda s: -s[0])

        resultList = []  # type: List[str]
        for entry in ratingList:
            resultList.append(entry[1])
        return resultList



    def TryGet(self, packageName: str) -> Optional[PackageLocationCachePath]:
        """ Get cached package path if available """
        if packageName not in self.__LocationDict:
            return None
        found = self.__LocationDict[packageName]
        return None if found.PackagePath is None else found.PackagePath


    def TryLocatePackage(self, packageName: str) -> Optional[PackageLocationCachePath]:
        """ Locate the package """
        found = self.TryGet(packageName)
        if found is not None:
            return found

        # The package was not available in the cache, so lets try to locate it by building a candidate list
        locationCandidates = self.__TryLocatePackageScratchpad
        locationCandidates.Clear()
        try:
            self.__AddInitialCandidates(locationCandidates, packageName)
            if locationCandidates.IsEmpty():
                return None

            # We now have a location record where we can start searching for the package we are looking for
            return self.__TryLocatePackage(locationCandidates, packageName)
        finally:
            locationCandidates.Clear()


    def __AddInitialCandidates(self, rLocationCandidates: PackageLocationCandidates, sourcePackageName: str) -> None:
        """ Fill the location candidate queue with candidate locations """
        subNames = sourcePackageName.split('.')
        subNames.pop()
        while len(subNames) > 0:
            packageName = ".".join(subNames)
            if packageName in self.__LocationDict:
                for location in self.__LocationDict[packageName].PathList:
                    rLocationCandidates.Append(location)
            subNames.pop()


    def __AddCandidateLocations(self, rLocationCandidates: PackageLocationCandidates, sourcePackageName: str) -> None:
        """ Look at the newly added entries in rLocationCandidates.NewLocations and insert the new possible locations in the queue """
        for newLocation in rLocationCandidates.NewLocations:
            if(sourcePackageName.startswith(newLocation.PackageName) and len(sourcePackageName) > len(newLocation.PackageName) and
               sourcePackageName[len(newLocation.PackageName)] == '.'):
                rLocationCandidates.Insert(newLocation)


    def __TryLocatePackage(self, rLocationCandidates: PackageLocationCandidates, packageName: str) -> Optional[PackageLocationCachePath]:
        packageLocationCachePath = rLocationCandidates.TryPopFront()
        while packageLocationCachePath is not None:
            newLocationPackageName = packageLocationCachePath.PackageName + "."

            rLocationCandidates.NewLocations.clear()
            self.__CacheLocation(self.__ScannedPathsCacheSet, self.__LocationDict, newLocationPackageName, packageLocationCachePath.AbsolutePath,
                                 ScanMethod.Directory, packageLocationCachePath.SourceLocation, rLocationCandidates.NewLocations)

            # Lets see if the scan found the package for us
            found = self.TryGet(packageName)
            if found is not None:
                return found

            self.__AddCandidateLocations(rLocationCandidates, packageName)

            packageLocationCachePath = rLocationCandidates.TryPopFront()
        return None


    def __CacheInitialLocations(self, packageLocations: List[ToolConfigPackageLocation]) -> None:
        if self.Log.Verbosity >= g_verbosityMaxLevel:
            self.Log.LogPrint("- Caching initial locations")
        try:
            self.Log.PushIndent()
            for packageLocation in packageLocations:
                self.__CacheLocation(self.__ScannedPathsCacheSet, self.__LocationDict, "", packageLocation.ResolvedPathEx, packageLocation.ScanMethod, packageLocation)
        finally:
            self.Log.PopIndent()


    def __IsBlacklisted(self, currentPath: str, blacklist: List[ToolConfigPackageLocationBlacklistEntry]) -> bool:
        """ Check if a absolute path, is present in the blacklist """
        for entry in blacklist:
            if currentPath == entry.AbsoluteDirPath:
                return True
        return False


    def __CacheLocation(self, rScannedPathsCacheSet: Set[str],
                        rLocationDict: Dict[str, PackageLocationCacheRecord],
                        locationPackageName: str,
                        sourcePath: str,
                        scanMethod: int,
                        sourceLocation: ToolConfigPackageLocation,
                        rNewLocations: Optional[List[PackageLocationCachePath]] = None) -> None:
        # if rNewLocations is not None all new locations we find will be added to this list
        # Prevent multiple scannings of the same path
        if sourcePath in rScannedPathsCacheSet:
            return
        rScannedPathsCacheSet.add(sourcePath)

        directories = IOUtil.GetDirectoriesAt(sourcePath, False)
        directories.sort()

        for dirEntry in directories:
            if not Util.IsValidPackageName(dirEntry, True):
                if self.Log.Verbosity >= 4:
                    self.Log.LogPrint("Ignored directory '{0}' at '{1}' as it was not a valid package name".format(dirEntry, IOUtil.Join(sourcePath, dirEntry)))
                continue
            absoluteDirPath = IOUtil.Join(sourcePath, dirEntry)
            if absoluteDirPath in self.__RootLocationPaths:
                if self.Log.Verbosity >= g_verbosityMaxLevel:
                    self.Log.LogPrint("Not scanning '{0}' as a child of '{1}' since it is a root location".format(absoluteDirPath, sourceLocation.ResolvedPath))
                continue
            if self.__IsBlacklisted(absoluteDirPath, sourceLocation.Blacklist):
                if self.Log.Verbosity >= g_verbosityMaxLevel:
                    self.Log.LogPrint("Not scanning '{0}' as it was blacklisted".format(absoluteDirPath))
                continue

            # This is not a original location path, so we can cache it as a 'child' of this location
            directoryLocationPackageName = locationPackageName + dirEntry
            absoluteDirPackageFilename = IOUtil.Join(absoluteDirPath, self.__GenFilename)
            foundPackageFilePath = absoluteDirPackageFilename if IOUtil.IsFile(absoluteDirPackageFilename) else None

            newLocationRecord = PackageLocationCachePath(directoryLocationPackageName, absoluteDirPath, foundPackageFilePath, sourceLocation)
            if directoryLocationPackageName not in rLocationDict:
                newRecord = PackageLocationCacheRecord(directoryLocationPackageName)
                rLocationDict[directoryLocationPackageName] = newRecord
                if self.Log.Verbosity >= g_verbosityMaxLevel:
                    if foundPackageFilePath is None:
                        self.Log.LogPrint("- Cached '{0}' at '{1}'".format(directoryLocationPackageName, absoluteDirPath))
                    else:
                        self.Log.LogPrint("- Cached '{0}' at '{1}', found package here.".format(directoryLocationPackageName, absoluteDirPath))
            elif self.Log.Verbosity >= g_verbosityMaxLevel:
                if foundPackageFilePath is None:
                    self.Log.LogPrint("- Cached alias to '{0}' at '{1}'".format(directoryLocationPackageName, absoluteDirPath))
                else:
                    self.Log.LogPrint("- Cached alias to '{0}' at '{1}', found package here.".format(directoryLocationPackageName, absoluteDirPath))

            cacheRecord = rLocationDict[directoryLocationPackageName]
            cacheRecord.Append(newLocationRecord)
            if rNewLocations is not None:
                rNewLocations.append(newLocationRecord)

            if scanMethod == ScanMethod.Directory:
                # we already scanned the directory, so do nothing
                pass
            elif scanMethod == ScanMethod.OneSubDirectory:
                newLocationPackageName = directoryLocationPackageName + "."
                self.__CacheLocation(rScannedPathsCacheSet, rLocationDict, newLocationPackageName, absoluteDirPath, ScanMethod.Directory, sourceLocation, rNewLocations)
            elif scanMethod == ScanMethod.AllSubDirectories:
                newLocationPackageName = directoryLocationPackageName + "."
                self.__CacheLocation(rScannedPathsCacheSet, rLocationDict, newLocationPackageName, absoluteDirPath, ScanMethod.AllSubDirectories, sourceLocation, rNewLocations)
            else:
                raise Exception("Unsupported ScanMethod {0}".format(ScanMethod.TryToString(scanMethod, True)))
