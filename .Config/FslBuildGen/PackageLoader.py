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

from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
#from typing import Tuple
#import os
from FslBuildGen import IOUtil
from FslBuildGen import PluginSharedValues
#from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Config import Config
#from FslBuildGen.DataTypes import ScanMethod
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Exceptions import BasePackageNotFoundException
#from FslBuildGen.Exceptions import DependencyNotFoundException
from FslBuildGen.Exceptions import ToolDependencyNotFoundException
from FslBuildGen.Generator.GeneratorPluginBase import GeneratorPluginBase
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Exceptions import PackageHasMultipleDefinitions2Exception
#from FslBuildGen.PackageCachedLocation import PackageCachedLocation
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.PackageFinder import PackageFinder
from FslBuildGen.PackageTemplateLoader import PackageTemplateLoader
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfig import ToolConfigPackageConfiguration
#from FslBuildGen.ToolConfig import ToolConfigPackageLocation
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
from FslBuildGen.Xml.Exceptions import XmlInvalidPackageNameException
from FslBuildGen.Xml.Exceptions import XmlInvalidSubPackageNameException
from FslBuildGen.Xml.XmlGenFile import XmlGenFile
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency

def _CreateXmlGenFile(log: Log, toolConfig: ToolConfig, defaultPackageLanguage: PackageLanguage) -> XmlGenFile:
    return XmlGenFile(log, toolConfig, defaultPackageLanguage)

def _ThrowToolDependencyNotFoundException(packageName: str) -> None:
    raise ToolDependencyNotFoundException(packageName)

def _ThrowBasePackageNotFoundException(packageName: str) -> None:
    raise BasePackageNotFoundException(packageName)


class PackageLoader(object):
    def __init__(self, config: Config, files: List[str], generator: GeneratorPluginBase,
                 forceImportPackageNames: Optional[List[str]] = None) -> None:
        super().__init__()
        self.BasicConfig = config

        genFilename = config.GenFileName  # type: str
        packageConfigDict = config.ToolConfig.PackageConfiguration  # type: Dict[str, ToolConfigPackageConfiguration]
        #packageLocations = [] if not config.Type in packageConfigDict else packageConfigDict[config.Type].Locations  # type: List[ToolConfigPackageLocation]
        factoryFunction = _CreateXmlGenFile

        templateLocationCache = self.__CacheTemplateLocations(config)
        self.PackageTemplateLoader = PackageTemplateLoader(config, templateLocationCache)
        self.PackageFinder = PackageFinder(config, generator, config.Type, packageConfigDict, genFilename, config.IsTestMode)

        inputFiles = self.PackageFinder.LocateInputFiles(files)  # type: List[PackageFile]

        config.LogPrint("- Parsing")
        try:
            config.PushIndent()

            # Preload all known package files from the cache if the package configuration requested it.
            if config.Type in packageConfigDict and packageConfigDict[config.Type].Preload:
                inputFiles = self.PackageFinder.GetKnownPackageFiles(inputFiles)

            if (generator.PlatformName == PlatformNameString.ANDROID):
                internalNinjaToolPackageName = config.ToolConfig.CMakeConfiguration.NinjaRecipePackageName
                config.LogPrintVerbose(4, "Adding package {0}".format(internalNinjaToolPackageName))
                self.__AddPackageToList(inputFiles, internalNinjaToolPackageName, _ThrowToolDependencyNotFoundException)
            elif generator.IsCMake and generator.CMakeConfig is not None and generator.CMakeConfig.CMakeFinalGeneratorName.upper() == "NINJA":
                internalNinjaToolPackageName = config.ToolConfig.CMakeConfiguration.NinjaRecipePackageName
                config.LogPrintVerbose(4, "Adding package {0}".format(internalNinjaToolPackageName))
                self.__AddPackageToList(inputFiles, internalNinjaToolPackageName, _ThrowToolDependencyNotFoundException)
            #    internalCMakeToolPackageName = "Recipe.BuildTool.CMake"
            #    config.LogPrintVerbose(4, "Adding package {0}".format(internalCMakeToolPackageName))
            #    self.__AddPackageToList(inputFiles, internalCMakeToolPackageName, _ThrowToolDependencyNotFoundException)

            if forceImportPackageNames is not None:
                for packageName in forceImportPackageNames:
                    self.__AddPackageToList(inputFiles, packageName, _ThrowToolDependencyNotFoundException)

            # Ensure we load the project base package dependencies specified in the project.gen file
            inputFiles = self.__AddBasePackages(inputFiles, config.ToolConfig.ProjectInfo.Contexts)

            # sort the input files to ensure a predictable 'initial' order
            inputFiles.sort(key=lambda s: s.AbsoluteDirPath.lower())

            packageDict = {} # type: Dict[str, List[XmlGenFile]]
            genFiles = []  # type: List[XmlGenFile]
            # Load the initial package files
            self.__LoadFiles(config, inputFiles, packageDict, genFiles, config.ToolConfig.DefaultPackageLanguage, factoryFunction)


            searchForPackages = True
            newGenFiles = genFiles
            while searchForPackages:
                missingPackages = self.__DiscoverMissingPackages(config, generator, packageDict, newGenFiles)
                if len(missingPackages) > 0:
                    newFiles = self.PackageFinder.LocateMissingPackages(missingPackages)
                    newGenFiles = []
                    self.__LoadFiles(config, newFiles, packageDict, newGenFiles, config.ToolConfig.DefaultPackageLanguage, factoryFunction)
                    genFiles += newGenFiles
                else:
                    searchForPackages = False

            self.FoundInputFiles = inputFiles
            self.SourceFiles = files
            self.GenFiles = genFiles
        finally:
            config.PopIndent()


    def __AddBasePackages(self, inputFiles: List[PackageFile], projectContexts: List[ToolConfigProjectContext]) -> List[PackageFile]:
        newInputFiles = list(inputFiles)
        for projectContext in projectContexts:
            for basePackage in projectContext.BasePackages:
                self.__AddPackageToList(newInputFiles, basePackage.Name, _ThrowBasePackageNotFoundException)
        return newInputFiles


    def __AddPackageToList(self, rInputFiles: List[PackageFile], packageName: str, throwFunc: Callable[[str], None]) -> None:
        packageFile = self.PackageFinder.TryLocateMissingPackagesByName(packageName)
        if packageFile is None:
            throwFunc(packageName)
            raise Exception("throwFunc did not throw")
        # prevent file duplication (FIX: this is a workaround due to 'initial' packages not being in the lookup cache)
        if not self.__ContainsName(rInputFiles, packageName):
            rInputFiles.append(packageFile)


    def __ContainsName(self, packageFiles: List[PackageFile], name: str) -> bool:
        for entry in packageFiles:
            if entry.PackageName == name:
                return True
        return False

    def __ContainsPackage(self, packageFiles: List[PackageFile], packageName: str) -> bool:
        for entry in packageFiles:
            if entry.PackageName == packageName:
                return True
        return False

    def __CacheTemplateLocations(self, config: Config) -> Dict[str, str]:
        """ Build a dict of all *.gen files found in the template import directories """
        resDict = {} # type: Dict[str, str]
        for location in config.TemplateImportDirectories:
            files = IOUtil.GetFilesAt(location.ResolvedPath, True)
            for entry in files:
                if entry.endswith(".gen"):
                    resDict[IOUtil.GetFileNameWithoutExtension(entry)] = IOUtil.Join(location.ResolvedPath, entry)
        return resDict


    def __DiscoverMissingPackages(self, log: Log, activePlatform: GeneratorPluginBase,
                                  packageDict: Dict[str, List[XmlGenFile]], genFiles: List[XmlGenFile]) -> Dict[str, XmlGenFile]:
        """ Create a dict where the key is the name of the missing package and the value is the xmlGenFile that first requested it """
        missingPackages = {}  # type: Dict[str, XmlGenFile]
        for entry in genFiles:
            self.__FindMissingPackages(log, packageDict, entry.DirectDependencies, missingPackages, entry)
            for platform in list(entry.Platforms.values()):
                if platform.Name == activePlatform.PlatformName:
                    for dep in platform.DirectDependencies:
                        if not dep.Name in packageDict:
                            if not dep.Name in missingPackages:
                                missingPackages[dep.Name] = entry
                                if log.Verbosity >= 2:
                                    log.LogPrint(".. Package '{0}' Platform '{1}' missing '{2}'".format(entry.Name, platform.Name, dep.Name))
                    for flavor in platform.Flavors:
                        for flavorOption in flavor.Options:
                            for dep in flavorOption.DirectDependencies:
                                if not dep.Name in packageDict:
                                    if not dep.Name in missingPackages:
                                        missingPackages[dep.Name] = entry
                                        if log.Verbosity >= 2:
                                            log.LogPrint(".. Package '{0}' platform '{1}' flavor '{2}' Option '{3}' missing '{4}'".format(entry.Name, platform.Name, flavor.Name, flavorOption.Name, dep.Name))


        return missingPackages

    def __FindMissingPackages(self, log: Log, packageDict: Dict[str, List[XmlGenFile]], entries: List[XmlGenFileDependency],
                              missingPackages: Dict[str, XmlGenFile], source: XmlGenFile) -> None:
        for dep in entries:
            if not dep.Name in packageDict:
                if not dep.Name in missingPackages:
                    missingPackages[dep.Name] = source
                if log.Verbosity > 1:
                    log.LogPrint(".. '{0}' missing '{1}'".format(source.Name, dep.Name))

    def __LoadFiles(self, config: Config, files: List[PackageFile],
                    rPackageDict: Dict[str, List[XmlGenFile]], rGenFiles: List[XmlGenFile],
                    defaultPackageLanguage: PackageLanguage,
                    factoryFunction: Callable[[Log, ToolConfig, PackageLanguage], XmlGenFile]) -> None:
        log = config  # type: Log
        toolConfig = config.ToolConfig
        for file in files:
            log.LogPrint("'{0}'".format(file.AbsoluteFilePath))
            xml = factoryFunction(log, toolConfig, defaultPackageLanguage)
            xml.Load(config, self.PackageTemplateLoader, file)
            self.__ValidatePackage(config, file, xml)
            rGenFiles.insert(0, xml)
            if not xml.Name in rPackageDict:
                rPackageDict[xml.Name] = [xml]
            else:
                rPackageDict[xml.Name].append(xml)

        for packageList in list(rPackageDict.values()):
            if len(packageList) > 1:
                raise PackageHasMultipleDefinitions2Exception(packageList)


    def __ValidatePackage(self, log: Log, sourceFile: PackageFile, xmlGenFile: XmlGenFile) -> None:
        """ Do some basic package consistency checks """
        self.__ValidatePackageName(log, sourceFile, xmlGenFile)


    def __ValidatePackageName(self, log: Log, sourceFile: PackageFile, xmlGenFile: XmlGenFile) -> None:
        """ Validate that the package name is fully equal to the containing directory names """
        pathBasedPackageName = sourceFile.PackageName
        if xmlGenFile.Name != pathBasedPackageName:
            if '.' in pathBasedPackageName:
                raise XmlInvalidSubPackageNameException(xmlGenFile.XMLElement, pathBasedPackageName, xmlGenFile.Name, sourceFile.AbsoluteFilePath, sourceFile.PackageRootLocation.ResolvedPath)
            raise XmlInvalidPackageNameException(xmlGenFile.XMLElement, pathBasedPackageName, xmlGenFile.Name, sourceFile.AbsoluteFilePath, sourceFile.PackageRootLocation.ResolvedPath)
