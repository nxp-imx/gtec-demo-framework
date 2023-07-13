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

from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple
import xml.etree.ElementTree as ET
from FslBuildGen import PackageConfig
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.Build.Filter import RequirementFilter
#from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.BasicBuildConfig import BasicBuildConfig
from FslBuildGen.Engine.EngineResolveConfig import EngineResolveConfig
from FslBuildGen.Engine.GraphImageSaveInfo import GraphImageSaveInfo
from FslBuildGen.Engine.Resolver.PackageManager2 import PackageManager2
from FslBuildGen.Engine.Resolver.PreResolver import PreResolver
from FslBuildGen.Engine.Resolver.PreResolvePackageResult import PreResolvePackageResult
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackage
from FslBuildGen.Engine.Resolver import PreResolvePackageResultUtil
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Exceptions import DependencyNotFoundException
from FslBuildGen.Exceptions import InternalErrorException
from FslBuildGen.Exceptions import InvalidDependencyException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.ExternalVariantConstraints import ExternalVariantConstraints
from FslBuildGen.Log import Log
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDependency
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.PackageProjectContextBasePackage import PackageProjectContextBasePackage
from FslBuildGen.Packages.Unresolved.XmlConvert import XmlConvert
from FslBuildGen.Packages.Unresolved.UnresolvedFactory import FactoryCreateContext
#from FslBuildGen.Packages.Unresolved.UnresolvedFactory import UnresolvedFactory
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackage
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigBasePackage import ToolConfigBasePackage
from FslBuildGen.ToolConfigPackageProjectContextUtil import ToolConfigPackageProjectContextUtil
from FslBuildGen.Version import Version
from FslBuildGen.Xml.XmlGenFile import XmlGenFile
from FslBuildGen.Xml.XmlStuff import FakeXmlGenFilePlatform
from FslBuildGen.Xml.XmlStuff import LocalPackageDefaultValues
from FslBuildGen.Xml.XmlStuff import XmlGenFilePlatform


class PackageManagerFilter(object):
    def __init__(self, requestedFiles: List[str], packageFilters: PackageFilters) -> None:
        super().__init__()
        self.RequestedFiles = requestedFiles
        self.PackageFilters = packageFilters

class ProjectContextCache(object):
    def __init__(self) -> None:
        super().__init__()
        self.__Cache = dict()  # type: Dict[str, PackageProjectContext]

    def Add(self, projectContext: PackageProjectContext) -> None:
        self.__Cache[projectContext.ProjectName] = projectContext

    def TryGet(self, projectName: str) -> Optional[PackageProjectContext]:
        return self.__Cache[projectName] if projectName in self.__Cache else None

class PackageManager(object):
    def __init__(self, log: Log, configBuildDir: str, configIgnoreNotSupported: bool, toolConfig: ToolConfig, platformName: str,
                 hostPlatformName: str, basicBuildConfig: BasicBuildConfig, generatorInfo: GeneratorInfo, genFiles: List[XmlGenFile],
                 packageManagerFilter: PackageManagerFilter, externalVariantConstraints: ExternalVariantConstraints,
                 engineResolveConfig: EngineResolveConfig, writeGraph: bool) -> None:
        super().__init__()

        self.__GeneratorInfo = generatorInfo
        self.__ProjectContextCache = ProjectContextCache()

        createContext = PackageManager.__CreateFactoryCreateContext(log, toolConfig, generatorInfo)

        # Handle flavor package resolving.
        unresolvedInput = self.__CreateInitialUnresolvedPackageList(configIgnoreNotSupported, toolConfig, platformName, hostPlatformName, genFiles,
                                                                    createContext)

        graphImageSaveInfo = GraphImageSaveInfo(toolConfig.ProjectInfo.Contexts) if writeGraph else None
        unresolvedPackages = PackageManager2.Resolve2(basicBuildConfig, createContext, unresolvedInput, externalVariantConstraints,
                                                      engineResolveConfig, graphImageSaveInfo)
        unresolvedPackages = PackageManager.__Filter(log, unresolvedPackages, packageManagerFilter)

        uniqueDict = {}  # type: Dict[PackageInstanceName, Package]
        for unresolvedPackage in unresolvedPackages:
            if unresolvedPackage.SourcePackage.NameInfo.FullName not in uniqueDict:
                uniqueDict[unresolvedPackage.SourcePackage.NameInfo.FullName] = Package(log, configBuildDir, unresolvedPackage)
            else:
                raise InternalErrorException("Package has been defined multiple times, this ought to have been caught earlier")

        self.__unresolvedPackages = unresolvedPackages
        self.OriginalPackageDict = uniqueDict
        self.Packages = list(uniqueDict.values())  # type: List[Package]

        # Resolve dependency package names -> actual package objects
        for package in self.Packages:
            self.__ResolvePackageDependencies(package, self.OriginalPackageDict)


    def __CreateInitialUnresolvedPackageList(self, configIgnoreNotSupported: bool, toolConfig: ToolConfig, platformName: str, hostPlatformName: str,
                                             genFiles: List[XmlGenFile], createContext: FactoryCreateContext) -> List[UnresolvedPackage]:
        #clonedList = list(genFiles)
        # Create a fake top level file and append it to the list
        #topLevelGenFile = XmlGenFile(config, config.ToolConfig.DefaultPackageLanguage)
        #topLevelGenFile.Name = PackageNameMagicString.TopLevelName
        #topLevelGenFile.SetType(PackageType.TopLevel)
        #clonedList.append(topLevelGenFile)

        unresolvedPackageDict = {}  # type: Dict[str, UnresolvedPackage]
        for genFile in genFiles:
            if genFile.Name not in unresolvedPackageDict:
                packageProjectContext = self.__FindProjectContext(toolConfig, genFile)
                unresolvedPackageDict[genFile.Name] = PackageManager.__AllocateUnresolvedPackage(createContext, packageProjectContext,
                                                                                                 configIgnoreNotSupported, platformName,
                                                                                                 hostPlatformName, genFile, False)
            else:
                raise InternalErrorException("Package has been defined multiple times, this ought to have been caught earlier")

        return list(unresolvedPackageDict.values())


    def __FindProjectContext(self, toolConfig: ToolConfig, genFile: XmlGenFile) -> PackageProjectContext:
        """
        Associate the package with the 'project' that it belongs to
        """
        if genFile.PackageLocation is None:
            if genFile.Type != PackageType.TopLevel:
                raise UsageErrorException("Package '{0}' did not contain a valid location".format(genFile.Name))
            # The top level package is not associated with a project context
            topLevelProjectContext = PackageProjectContext(ProjectId("__TopLevel__"), "__TopLevel__", Version(0,0,0,0), None, [])
            self.__ProjectContextCache.Add(topLevelProjectContext)
            return topLevelProjectContext
        projectContext = ToolConfigPackageProjectContextUtil.FindProjectContext(toolConfig.ProjectInfo.Contexts,
                                                                                genFile.PackageLocation.ResolvedPath)
        basePackages = self.__CreateBasePackageList(projectContext.BasePackages)
        packageProjectContext = self.__ProjectContextCache.TryGet(projectContext.ProjectName)
        if packageProjectContext is None:
            packageProjectContext = PackageProjectContext(projectContext.ProjectId, projectContext.ProjectName, projectContext.ProjectVersion,
                                                          projectContext.GitHash, basePackages)
            self.__ProjectContextCache.Add(packageProjectContext)
        return packageProjectContext

    def __CreateBasePackageList(self, basePackages: List[ToolConfigBasePackage]) -> List[PackageProjectContextBasePackage]:
        res = [] # type: List[PackageProjectContextBasePackage]
        for basePackage in basePackages:
            res.append(PackageProjectContextBasePackage(basePackage.Name))
        return res


    def CreatePackage(self, log: Log, configBuildDir: str, configIgnoreNotSupported: bool, toolConfig: ToolConfig,
                      platformName: str, hostPlatformName: str, genFile: XmlGenFile, insertAtFront: bool = False) -> Package:

        filePackageInstanceName = PackageInstanceName(genFile.Name)
        if filePackageInstanceName in self.OriginalPackageDict:
            raise UsageErrorException("Package '{0}' already exist".format(filePackageInstanceName))

        createContext = PackageManager.__CreateFactoryCreateContext(log, toolConfig, self.__GeneratorInfo)
        packageProjectContext = self.__FindProjectContext(toolConfig, genFile)
        processedPackage = PackageManager.__AllocatePreprocessedPackage(createContext, packageProjectContext, configIgnoreNotSupported, platformName,
                                                                        hostPlatformName, genFile, True)
        packageLookupDict = PreResolver.CreatePackageLookupDict(self.__unresolvedPackages)
        preResolvedPackageResult = PreResolver.PreResolvePackage(log, packageLookupDict, processedPackage, 0xffffffff)
        package = Package(log, configBuildDir, preResolvedPackageResult)

        self.__ResolvePackageDependencies(package, self.OriginalPackageDict)
        if not insertAtFront:
            self.Packages.append(package)
        else:
            self.Packages.insert(0, package)
        self.OriginalPackageDict[package.NameInfo.FullName] = package
        return package


    def __ResolvePackageDependencies(self, package: Package, originalPackageDict: Dict[PackageInstanceName, Package]) -> None:
        for dep in package.GetDirectDependencies():
            depPackage = self.__ResolvePackageDependency(package, dep.Name, originalPackageDict)
            package.ResolvedDirectDependencies.append(PackageDependency(depPackage, dep.Access))

    def __ResolvePackageDependency(self, package: Package, depPackageName: PackageInstanceName, originalPackageDict: Dict[PackageInstanceName, Package]) -> Package:
        depPackageNameInstanceName = PackageInstanceName(depPackageName.Value)
        if depPackageNameInstanceName not in originalPackageDict:
            raise DependencyNotFoundException(package.Name, depPackageNameInstanceName.Value)
        depPackage = originalPackageDict[depPackageNameInstanceName]
        if package.Type != PackageType.TopLevel and not depPackage.AllowDependencyOnThis:
            raise InvalidDependencyException(package.Name, depPackageNameInstanceName.Value)
        return depPackage

    @staticmethod
    def __AllocatePreprocessedPackage(createContext: FactoryCreateContext, packageProjectContext: PackageProjectContext,
                                      configIgnoreNotSupported: bool, platformName: str, hostPlatformName: str, genFile: XmlGenFile,
                                      allowInternalNames: bool) -> ProcessedPackage:
        unresolvedPackage = PackageManager.__AllocateUnresolvedPackage(createContext, packageProjectContext, configIgnoreNotSupported, platformName,
                                                                       hostPlatformName, genFile, allowInternalNames)
        return PackageManager2.ProcessPackage(createContext, packageProjectContext, unresolvedPackage)

    @staticmethod
    def __AllocateUnresolvedPackage(createContext: FactoryCreateContext, packageProjectContext: PackageProjectContext,
                                    configIgnoreNotSupported: bool, platformName: str, hostPlatformName: str, genFile: XmlGenFile,
                                    allowInternalNames: bool) -> UnresolvedPackage:
        resolvedPlatform, resolvedPlatformDirectSupported = PackageManager.__ResolvePlatform(createContext, platformName, hostPlatformName,
                                                                                             configIgnoreNotSupported, genFile.Platforms,
                                                                                             genFile.Name, genFile.Type,
                                                                                             genFile.XMLElement, genFile.SystemDefaultValues)

        return XmlConvert.ToUnresolvedPackage(createContext, packageProjectContext, genFile, resolvedPlatform, resolvedPlatformDirectSupported,
                                              allowInternalNames)

    # The returned tuple is the [resolvedPlatformName, packagePlatform, resolvedPlatformDirectSupported]
    @staticmethod
    def __ResolvePlatform(createContext: FactoryCreateContext, platformName: str, hostPlatformName: str, ignoreNotSupported: bool,
                          platforms: Dict[str, XmlGenFilePlatform], packageName: str, packageType: PackageType, packageXMLElement: ET.Element,
                          systemDefaultValues: LocalPackageDefaultValues) -> Tuple[PackagePlatform, bool]:
        # ToolRecipe's resolve using the hostPlatformName as we use this for 'cross compiling' for now
        currentPlatformName = platformName if packageType != PackageType.ToolRecipe else hostPlatformName
        resolvedPlatform = PackageManager.__CreatePlatform(createContext, platforms, currentPlatformName, packageType, systemDefaultValues)
        resolvedPlatformDirectSupported = resolvedPlatform.Supported if not ignoreNotSupported else True

        # Due to the new flavor project id generation code we no longer require a project id to be in the xml file
        #if currentPlatformName == PackageConfig.PlatformNameString.WINDOWS:
        #    if packageType == PackageType.Library or packageType == PackageType.Executable: # or packageType == PackageType.ExeLibCombo:
        #        if resolvedPlatform.ProjectId is None:  # split into separate line to make MyPy happy
        #            raise XmlMissingWindowsVisualStudioProjectIdException(packageXMLElement, packageName)
        if resolvedPlatform.Name != currentPlatformName:
            raise Exception("internal platform name error")
        return (resolvedPlatform, resolvedPlatformDirectSupported)

    @staticmethod
    def __CreatePlatform(createContext: FactoryCreateContext, platforms: Dict[str, XmlGenFilePlatform], platformName: str,
                         packageType: PackageType, systemDefaultValues: LocalPackageDefaultValues) -> PackagePlatform:
        # filter out any unwanted platform names
        if platformName in PackageConfig.APPROVED_PLATFORM_NAMES and platformName in platforms:
            return XmlConvert.CreateFromXmlGenFilePlatform(createContext, platformName, platforms[platformName])

        # create fake entries as needed
        fakeXmlGenFilePlatform = FakeXmlGenFilePlatform(createContext.Log, platformName, systemDefaultValues)
        if packageType == PackageType.TopLevel:
            fakeXmlGenFilePlatform.ProjectId = 'EDC12D73-5B32-4E45-8E2E-DFC82FAD5DF4'
        return XmlConvert.CreateFromXmlGenFilePlatform(createContext, platformName, fakeXmlGenFilePlatform)

    @staticmethod
    def __CreateFactoryCreateContext(log: Log, toolConfig: ToolConfig, generatorInfo: GeneratorInfo) -> FactoryCreateContext:
        ninjaRecipePackageName = UnresolvedPackageName(toolConfig.CMakeConfiguration.NinjaRecipePackageName)
        isWindows = PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows
        return FactoryCreateContext(log, generatorInfo, ninjaRecipePackageName, isWindows)

    @staticmethod
    def __Filter(log: Log, sourcePackageBuildOrder: List[PreResolvePackageResult], packageManagerFilter: PackageManagerFilter) -> List[PreResolvePackageResult]:
        log.LogPrint("- Filtering")
        log.PushIndent()
        try:
            #topLevelPackage = PackageListUtil.GetTopLevelPackage(sourcePackageBuildOrder)
            requestedPackages = PreResolvePackageResultUtil.TryGetPackageListFromFilenames(sourcePackageBuildOrder, packageManagerFilter.RequestedFiles, False)

            #if filterHack == FilterHack.TrimFlavors:
            #    # Remove all packages that were not imported because of a user requirement
            #    sourcePackageBuildOrder = PackageManager.__FilterNotUserRequested(log, sourcePackageBuildOrder, requestedPackages)

            if not packageManagerFilter.PackageFilters.ContainsFilters():
                return sourcePackageBuildOrder

            #requirements = RequirementFilter.GetRequirementList(topLevelPackage, None)
            requirements = RequirementFilter.GetRequirementListFromPackages(sourcePackageBuildOrder)
            return PackageFilter.Filter2(log, sourcePackageBuildOrder, requirements, requestedPackages, packageManagerFilter.PackageFilters)
        finally:
            log.PopIndent()

    #@staticmethod
    #def __FilterNotUserRequested(log: Log, sourcePackageBuildOrder: List[PreResolvePackageResult],
    #                             requestedPackages: Optional[List[PreResolvePackageResult]]) -> List[PreResolvePackageResult]:
    #    if requestedPackages is None or len(requestedPackages) <= 0:
    #        return sourcePackageBuildOrder

    #    userRequestedPackageNameSet = set()
    #    for preResolvePackageResult in requestedPackages:
    #        package = preResolvePackageResult.SourcePackage
    #        if not package.NameInfo.FullName in userRequestedPackageNameSet:
    #            userRequestedPackageNameSet.add(package.NameInfo.FullName)
    #            for depPreResolvePackageResult in preResolvePackageResult.ResolvedBuildOrder:
    #                if not depPreResolvePackageResult.SourcePackage.NameInfo.FullName in userRequestedPackageNameSet:
    #                    userRequestedPackageNameSet.add(depPreResolvePackageResult.SourcePackage.NameInfo.FullName)

    #    # This one liner
    #    #    return [entry for entry in sourcePackageBuildOrder if entry.SourcePackage.NameInfo.FullName in userRequestedPackageNameSet]
    #    # Was expanded because we need to log information

    #    logResult = log.Verbosity > 2
    #    result = []
    #    for entry in sourcePackageBuildOrder:
    #        if entry.SourcePackage.NameInfo.FullName in userRequestedPackageNameSet:
    #            result.append(entry)
    #        elif logResult:
    #            log.LogPrint("- {0} (Removed because it was not requested by the user)".format(entry.SourcePackage.NameInfo.FullName))
    #    return result
