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

from typing import cast
from typing import Dict
from typing import List
from typing import Optional
#from FslBuildGen import IOUtil
from FslBuildGen.Exceptions import DependencyNotFoundException
from FslBuildGen.Exceptions import PackageHasMultipleDefinitionsException
from FslBuildGen.Exceptions import PackageLoaderFailedToLocatePackageException
from FslBuildGen.Generator.GeneratorPluginBase import GeneratorPluginBase
from FslBuildGen.Log import Log
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.PackageLocationCache import PackageLocationCache
from FslBuildGen.PackageLocationCache import PackageLocationCachePath
from FslBuildGen.ToolConfig import ToolConfigPackageConfiguration
from FslBuildGen.ToolConfig import ToolConfigPackageLocation
from FslBuildGen.Xml.XmlGenFile import XmlGenFile


class PackageFinder(object):
    def __init__(self, log: Log,
                 platform: GeneratorPluginBase,
                 configTypeName: str,
                 packageConfigDict: Dict[str, ToolConfigPackageConfiguration],
                 genFilename: str,
                 testModeEnabled: bool) -> None:
        self.Log = log
        self.ConfigTypeName = configTypeName
        # The locations available in this configuration
        self.PackageLocations = [] if not configTypeName in packageConfigDict else packageConfigDict[configTypeName].Locations  # type: List[ToolConfigPackageLocation]
        self.GenFilename = genFilename
        self.PackageLocationCache = PackageLocationCache(log, self.PackageLocations, self.GenFilename)
        self.__TestModeEnabled = testModeEnabled
        self.__InitialSearchLocations = self.__BuildInitialSearchLocations(self.PackageLocations) # type: List[ToolConfigPackageLocation]


        if not configTypeName in packageConfigDict:
            log.DoPrintWarning("The configuration name '{0}' is unknown, expected one of {1}".format(configTypeName, list(packageConfigDict.keys())))


    def __BuildInitialSearchLocations(self, selectedConfigurationLocationList: List[ToolConfigPackageLocation]) -> List[ToolConfigPackageLocation]:
        """ Scan the package configurations and build a list of unique locations for the initial scan """
        uniqueLocationDict = {}  # type: Dict[str, ToolConfigPackageLocation]
        for location in selectedConfigurationLocationList:
            resolvedPathId = location.ResolvedPath
            if resolvedPathId in uniqueLocationDict:
                raise PackageHasMultipleDefinitionsException("The package location '{0}' was already added by {1} and {2} tried to add it again.".format(resolvedPathId, location.Name, uniqueLocationDict[resolvedPathId].Name))
            uniqueLocationDict[resolvedPathId] = location

        sortedList = list(uniqueLocationDict.values())
        sortedList.sort(key=lambda s: -len(s.ResolvedPath))
        return sortedList


    def LocateInputFiles(self, files: List[str]) -> List[PackageFile]:
        oldFiles = files
        resultFileList = []  # type: List[PackageFile]
        for file in oldFiles:
            location = self.__LocateInputFileLocation(file)
            resultFileList.append(PackageFile(file, None, location))
        return resultFileList

    def __LocateInputFileLocation(self, file: str) -> ToolConfigPackageLocation:
        # print(file)
        for location in self.__InitialSearchLocations:
            if file.startswith(location.ResolvedPathEx):
                return location
        scannedLocations = []
        for location in self.__InitialSearchLocations:
            scannedLocations.append(location.ResolvedPathEx)
        raise Exception("Could not find package location for file {0} scanned locations: {1}".format(file, scannedLocations))

    def TryLocateMissingPackagesByName(self, packageName: str) -> Optional[PackageFile]:
        # Check to see if the package can be found
        foundLocation = self.PackageLocationCache.TryLocatePackage(packageName)
        if foundLocation is not None and foundLocation.FoundPackageFilePath is not None:
            return PackageFile(foundLocation.FoundPackageFilePath, packageName, foundLocation.SourceLocation)
        return None

    def LocateMissingPackages(self, missingDict: Dict[str, XmlGenFile]) -> List[PackageFile]:
        """ Given a dict of missing package requests, try to locate them """
        files = []
        for packageName in sorted(missingDict.keys()):
            foundLocation = self.__LocateMissingPackage(packageName, missingDict)
            files.append(PackageFile(cast(str, foundLocation.FoundPackageFilePath), packageName, foundLocation.SourceLocation))
        return files

    def __LocateMissingPackage(self, packageName: str, missingDict: Dict[str, XmlGenFile], allowRetry: bool = True) -> PackageLocationCachePath:
        """ We only provide the missing dict here so that we can use it in case of a exception """
        # Check to see if the package can be found
        foundLocation = self.PackageLocationCache.TryLocatePackage(packageName)
        if foundLocation is not None and foundLocation.FoundPackageFilePath is not None:
            return foundLocation

        # So we did not locate a package file, it's time to fire and exception,
        # but lets try to provide some useful feedback.
        candidateList = self.PackageLocationCache.FindCandidates(packageName, True)
        if len(candidateList) != 1 or packageName not in candidateList:
            raise DependencyNotFoundException(missingDict[packageName].Name, packageName, candidateList)
        else:
            # So the candidate list was able to find the package,
            # that indicates that our finder/cache combo has a bug.
            if allowRetry and not self.__TestModeEnabled:
                self.Log.LogPrintWarning("Optimized package locator failed to locate package, but we have reason to assume that it might exist so trying again")
                return self.__LocateMissingPackage (packageName, missingDict, False)
            raise PackageLoaderFailedToLocatePackageException(missingDict[packageName].Name, packageName)

    def GetKnownPackageFiles(self, theFiles: List[PackageFile]) -> List[PackageFile]:
        """ Get all the files associated with the typeId then merge it with the supplied file list.
        """

        knownPackageLocationList = self.PackageLocationCache.GetKnownPackageLocations()  # type: List[PackageLocationCachePath]
        result = list(theFiles)
        for packageLocation in knownPackageLocationList:
            result.append(PackageFile(cast(str, packageLocation.FoundPackageFilePath), packageLocation.PackageName, packageLocation.SourceLocation))

        return result


    def TryLocatePackageFileByName(self, packageName: str) -> Optional[PackageFile]:
        foundLocation = self.PackageLocationCache.TryLocatePackage(packageName)
        if foundLocation is None:
            return None
        return PackageFile(cast(str, foundLocation.FoundPackageFilePath), packageName, foundLocation.SourceLocation)


    def LocatePackageFileByName(self, packageName: str) -> PackageFile:
        found = self.TryLocatePackageFileByName(packageName)
        if found is None:
            raise Exception("Could not locate package {0}".format(packageName))
        return found
