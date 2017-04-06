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

from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import XmlGenFile
from FslBuildGen.PackageCachedLocation import PackageCachedLocation
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen import PluginConfig;
from FslBuildGen import IOUtil
from FslBuildGen.PackageTemplateLoader import PackageTemplateLoader
from FslBuildGen.ToolConfig import ToolConfigPackageLocation
import os


class PackageLoader(object):
    def __init__(self, config, files, platform):
        super(PackageLoader, self).__init__()


        templateLocationCache = self.__CacheTemplateLocations(config)
        self.PackageTemplateLoader = PackageTemplateLoader(templateLocationCache)

        files = self.__LocateInputFiles(config, files);

        config.LogPrint("- Parsing")

        if config.IsSDKBuild:
            files = self.__PreloadPackages(config, files)

        packageDict = {}
        genFiles = []
        # Load the initial package files
        self.__LoadFiles(config, files, packageDict, genFiles, config.ToolConfig.DefaultPackageLanguage)

        locationCache = self.__CachePackageLocations(config)

        searchForPackages = True
        newGenFiles = genFiles
        while searchForPackages:
            missingPackages = self.__DiscoverMissingPackages(config, platform, packageDict, newGenFiles)
            if len(missingPackages) > 0:
                newFiles = self.__LocateMissingPackages(locationCache, missingPackages, config.GenFileName)
                newGenFiles = []
                self.__LoadFiles(config, newFiles, packageDict, newGenFiles, config.ToolConfig.DefaultPackageLanguage)
                genFiles += newGenFiles
            else:
                searchForPackages = False

        self.GenFiles = genFiles


    def __LocateInputFiles(self, config, files):
        oldFiles = files
        files = []
        for file in oldFiles:
            location = self.__LocateInputFileLocation(config, file)
            files.append(PackageFile(file, location))
        return files

    def __LocateInputFileLocation(self, config, file):
        print(file)
        for packageConfiguration in config.ToolConfig.PackageConfiguration.values():
            for location in packageConfiguration.Locations:
                if file.startswith(IOUtil.NormalizePath(location.ResolvedPath) + '/'):
                   return location
        raise Exception("Could not find package location");



    def __CacheTemplateLocations(self, config):
        """ Build a dict of all *.gen files found in the template import directories """
        dict = {}
        for location in config.TemplateImportDirectories:
            files = IOUtil.GetFilesAt(location.ResolvedPath, True)
            for entry in files:
                if entry.endswith(".gen"):
                    dict[IOUtil.GetFileNameWithoutExtension(entry)] = IOUtil.Join(location.ResolvedPath, entry)
        return dict


    def __CachePackageLocations(self, config):
        dict = {}
        packageConfiguration = config.ToolConfig.PackageConfiguration[config.Type]
        for location in packageConfiguration.Locations:
            dirs = IOUtil.GetDirectoriesAt(location.ResolvedPath, False)
            for entry in dirs:
                fullPath = IOUtil.Join(location.ResolvedPath, entry)
                if not self.__IsBlackListed(location, fullPath):
                    dict[entry] = PackageCachedLocation(fullPath, location)
        return dict


    def __LocateMissingPackages(self, locationCache, missing, genFileName):
        files = []
        for entry in sorted(missing.keys()):
            found = False
            packageCachedLocation = None
            if entry in locationCache:
                packageCachedLocation = locationCache[entry]
            elif '.' in entry:
                subNames = entry.split('.')
                if subNames[0] in locationCache:
                    packageCachedLocation = self.__LocateMissingSubPackage(locationCache[subNames[0]], subNames)

            if packageCachedLocation != None:
                fullPathPackageFile = IOUtil.Join(packageCachedLocation.Path, genFileName)
                if os.path.isfile(fullPathPackageFile):
                    files.append(PackageFile(fullPathPackageFile, packageCachedLocation.Location))
                    found = True

            if not found:
                raise DependencyNotFoundException(missing[entry], entry)
        return files


    def __LocateMissingSubPackage(self, packageLocation, subNames):
        fullPath = packageLocation.Path
        subIndex = 1
        subNameCount = len(subNames)
        while(subIndex < subNameCount):        
            dirs = IOUtil.GetDirectoriesAt(fullPath, False)
            if not subNames[subIndex] in dirs:
                return None
            fullPath = IOUtil.Join(fullPath, subNames[subIndex])
            subIndex = subIndex + 1
        if subIndex != subNameCount:
            return None
        location = ToolConfigPackageLocation(None, packageLocation.Location.BasedOn, None, fullPath)
        return PackageCachedLocation(fullPath, location)


    def __PreloadPackages(self, config, theFiles):
        # Get all the files associated with the typeId then merge it with the supplied file list
        files = self.__GetFiles(config)
        dict = {}
        for entry in files:
            dict[entry.FileName] = entry;

        for packageFile in theFiles:
            if not packageFile.FileName in dict:
                files.append(packageFile)
        files.sort(key=lambda s: s.FileName.lower())
        return files


    def __IsBlackListed(self, location, path):
        for entry in location.Blacklist:
            entryName = entry.Name + '/'
            if path.startswith(entryName):
                return True
        return False

    def __LocatePackages(self, location, genFileName):
        files = []
        dirs = IOUtil.GetDirectoriesAt(location.ResolvedPath, False)
        for dir in dirs:
            packageFile = IOUtil.Join(dir, genFileName)
            fullPathPackageFile = IOUtil.Join(location.ResolvedPath, packageFile)
            if os.path.isfile(fullPathPackageFile) and not self.__IsBlackListed(location, packageFile):
                files.append(PackageFile(fullPathPackageFile, location))
        return files


    def __GetFiles(self, config):
        packageConfiguration = config.ToolConfig.PackageConfiguration[config.Type]
        files = []
        for location in packageConfiguration.Locations:
            files += self.__LocatePackages(location, config.GenFileName)
        return files


    def __DiscoverMissingPackages(self, config, activePlatform, packageDict, genFiles):
        missingPackages = {}
        for entry in genFiles:
            for dep in entry.DirectDependencies:
                if not dep.Name in packageDict:
                    if not dep.Name in missingPackages:
                        missingPackages[dep.Name] = entry
                    if config.Verbosity > 1:
                        config.LogPrint("  .. %s missing %s" % (entry.Name, dep.Name));
            for platform in entry.Platforms.values():
                if activePlatform.Id == PluginConfig.PLATFORM_ID_ALL or platform.Name == activePlatform.Name:
                    for dep in platform.DirectDependencies:
                        if not dep.Name in packageDict:
                            if not dep.Name in missingPackages:
                                missingPackages[dep.Name] = entry
                            if config.Verbosity > 1:
                                config.LogPrint("  .. Platform %s package %s missing %s" % (platform.Name, entry.Name, dep.Name));
        return missingPackages


    def __LoadFiles(self, config, files, packageDict, genFiles, defaultPackageLanguage):
        for file in files:
            config.LogPrint("  '%s'" % (file.FileName))
            xml = XmlGenFile(defaultPackageLanguage)
            xml.Load(config, self.PackageTemplateLoader, file)
            genFiles.insert(0, xml)
            if not xml.Name in packageDict:
                packageDict[xml.Name] = [xml]
            else:
                packageDict[xml.Name].append(xml)

        for packageList in packageDict.values():
            if len(packageList) > 1:
                raise PackageHasMultipleDefinitionsException(packageList)
