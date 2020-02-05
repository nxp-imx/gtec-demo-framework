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
from typing import Set
from typing import Union
import os
import os.path
from FslBuildGen import IOUtil
from FslBuildGen import PathUtil
from FslBuildGen import Util
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildConfig.BuildDocConfiguration import BuildDocConfiguration
from FslBuildGen.BuildConfig.BuildDocConfigurationRequirement import BuildDocConfigurationRequirement
from FslBuildGen.BuildConfig.ClangFormatConfiguration import ClangFormatConfiguration
from FslBuildGen.BuildConfig.ClangTidyConfiguration import ClangTidyConfiguration
from FslBuildGen.BuildConfig.ClangTidyPlatform import ClangTidyPlatform
from FslBuildGen.BuildConfig.ClangTidyPlatformCompiler import ClangTidyPlatformCompiler
from FslBuildGen.BuildConfig.ClangTidyPlatformDefines import ClangTidyPlatformDefines
from FslBuildGen.BuildConfig.CMakeConfiguration import CMakeConfiguration
from FslBuildGen.BuildConfig.CMakeConfigurationPlatform import CMakeConfigurationPlatform
from FslBuildGen.CMakeUtil import CMakeVersion
from FslBuildGen.CMakeUtil import CMakeUtil
from FslBuildGen.DataTypes import CompilerNames
from FslBuildGen.DataTypes import MagicStrings
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.DataTypes import VisualStudioVersion
from FslBuildGen.Exceptions import DuplicatedConfigBasePackage
from FslBuildGen.Exceptions import DuplicatedConfigContentBuilder
from FslBuildGen.Exceptions import DuplicatedConfigPackageLocation
from FslBuildGen.Exceptions import DuplicatedConfigRootPath
from FslBuildGen.Exceptions import DuplicatedNewProjectTemplatesRootPath
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Log import Log
from FslBuildGen.ToolConfigBasePackage import ToolConfigBasePackage
from FslBuildGen.ToolConfigExperimental import ToolConfigExperimental
from FslBuildGen.ToolConfigPackageRootUtil import ToolConfigPackageRootUtil
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
from FslBuildGen.ToolConfigProjectInfo import ToolConfigProjectInfo
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory
from FslBuildGen.ToolMinimalConfig import ToolMinimalConfig
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.Exceptions import XmlDuplicatedCompilerConfigurationException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedCompilerVersionException
from FslBuildGen.Xml.Project.XmlBuildDocConfiguration import XmlBuildDocConfiguration
from FslBuildGen.Xml.Project.XmlClangTidyConfiguration import XmlClangTidyConfiguration
from FslBuildGen.Xml.Project.XmlClangTidyPlatform import XmlClangTidyPlatform
from FslBuildGen.Xml.Project.XmlClangTidyPlatformCompiler import XmlClangTidyPlatformCompiler
from FslBuildGen.Xml.Project.XmlClangTidyPlatformDefines import XmlClangTidyPlatformDefines
from FslBuildGen.Xml.Project.XmlClangTidyPlatformStrictChecks import XmlClangTidyPlatformStrictChecks
from FslBuildGen.Xml.Project.XmlCMakeConfiguration import XmlCMakeConfiguration
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlClangFormatConfiguration
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlConfigCompilerConfiguration
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlConfigFileAddBasePackage
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlConfigFileAddRootDirectory
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlExperimental
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlProjectRootConfigFile
from FslBuildGen.Xml.ToolConfig.XmlConfigFileAddNewProjectTemplatesRootDirectory import XmlConfigFileAddNewProjectTemplatesRootDirectory
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageConfiguration import XmlConfigPackageConfiguration
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageLocation import FakeXmlConfigPackageLocation
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageLocation import XmlConfigPackageLocation
from FslBuildGen.Xml.XmlToolConfigFile import XmlConfigContentBuilder
from FslBuildGen.Xml.XmlToolConfigFile import XmlConfigContentBuilderConfiguration
from FslBuildGen.Xml.XmlToolConfigFile import XmlConfigFileAddTemplateImportDirectory
from FslBuildGen.Xml.XmlToolConfigFile import XmlConfigFileTemplateFolder
from FslBuildGen.Xml.XmlToolConfigFile import XmlToolConfigFile


class ToolConfigCompilerConfiguration(object):
    def __init__(self, basicConfig: BasicConfig, basedUponXML: XmlConfigCompilerConfiguration) -> None:
        super().__init__()
        self.BasedOn = basedUponXML
        self.Name = self.BasedOn.Name
        self.Id = self.BasedOn.Id
        self.Platform = self.BasedOn.Platform
        defaultVersion = VisualStudioVersion.TryParse(self.BasedOn.DefaultVersion)
        if defaultVersion is None:
            raise XmlUnsupportedCompilerVersionException(self.BasedOn.XMLElement, self.BasedOn.Name, self.BasedOn.DefaultVersion, ', '.join(str(x) for x in VisualStudioVersion.AllEntries))
        self.DefaultVersion = defaultVersion


class ToolConfigTemplateFolder(object):
    def __init__(self, basicConfig: BasicConfig, basedUponXML: XmlConfigFileTemplateFolder) -> None:
        super().__init__()
        self.BasedOn = basedUponXML
        self.Name = self.BasedOn.Name

        variableProcessor = VariableProcessor(basicConfig)

        self.ResolvedPath = variableProcessor.ResolveAbsolutePathWithLeadingEnvironmentVariablePathAsDir(self.Name)


class NewProjectTemplateRootDirectory(object):
    def __init__(self, basicConfig: BasicConfig, basedUponXML: XmlConfigFileAddNewProjectTemplatesRootDirectory) -> None:
        super().__init__()
        self.BasedOn = basedUponXML
        self.Id = basedUponXML.Id
        self.Name = basedUponXML.Name
        self.DynamicName = basedUponXML.Name

        variableProcessor = VariableProcessor(basicConfig)

        # NOTE: workaround Union of tuples not being iterable bug in mypy https://github.com/python/mypy/issues/1575
        tupleResult = variableProcessor.TryExtractLeadingEnvironmentVariableNameAndPath(self.DynamicName, True)
        env = tupleResult[0]
        remainingPath = tupleResult[1]
        if env is None:
            raise Exception("Root dirs are expected to contain environment variables '{0}'".format(self.DynamicName))
        remainingPath = remainingPath if remainingPath is not None else ""

        resolvedPath = IOUtil.GetEnvironmentVariableForDirectory(env) + remainingPath
        self.BashName = '${0}{1}'.format(env, remainingPath)
        self.DosName = '%{0}%{1}'.format(env, remainingPath)
        self.ResolvedPath = IOUtil.ToUnixStylePath(resolvedPath)
        self.ResolvedPathEx = "{0}/".format(self.ResolvedPath) if len(self.ResolvedPath) > 0 else ""
        self.__EnvironmentVariableName = env


class ToolConfigDirectory(object):
    def __init__(self, basicConfig: BasicConfig, basedUponXML: XmlConfigFileAddTemplateImportDirectory) -> None:
        super().__init__()

        self.BasedOn = basedUponXML
        self.Name = self.BasedOn.Name

        variableProcessor = VariableProcessor(basicConfig)

        # NOTE: workaround Union of tuples not being iterable bug in mypy https://github.com/python/mypy/issues/1575
        tupleResult = variableProcessor.TrySplitLeadingEnvironmentVariablesNameAndPath(self.Name)
        envName = tupleResult[0]
        rest = tupleResult[1]
        if envName is None:
            raise Exception("Template import dirs are expected to contain environment variables")
        rest = rest if rest is not None else ""

        self.DecodedName = envName
        self.BashName = IOUtil.Join('$' + self.DecodedName, rest)
        self.DosName = IOUtil.Join('%' + self.DecodedName + '%', rest)
        if self.Name is None:
            raise XmlException2(basedUponXML.XmlElement, "Dirs are expected to contain environment variables")
        self.ResolvedPath = IOUtil.Join(IOUtil.GetEnvironmentVariableForDirectory(self.DecodedName), rest)
        self.ResolvedPathEx = "{0}/".format(self.ResolvedPath) if len(self.ResolvedPath) > 0 else ""


# TODO: improve interface, dont allow so many None (remove None from rootDirs and projectRootDirectory)
class ToolConfigLocation(object):
    def __init__(self, basicConfig: BasicConfig,
                 rootDirs: Optional[List[ToolConfigRootDirectory]],
                 basedUponXML: XmlConfigPackageLocation,
                 projectRootDirectory: Optional[str],
                 resolvedPath: Optional[str] = None) -> None:
        super().__init__()
        if rootDirs is None or projectRootDirectory is None:
            if rootDirs is not None or projectRootDirectory is not None:
                raise Exception("When rootDirs is none, then the projectRootDirectory must be none")

        # Do some basic validation of the path
        PathUtil.ValidateIsNormalizedPath(basedUponXML.Name, "Location")

        self.BasedOn = basedUponXML
        self.Id = basedUponXML.Id
        self.Name = basedUponXML.Name


        if resolvedPath is not None:
            self.ResolvedPath = IOUtil.NormalizePath(resolvedPath)
        else:
            if rootDirs is None or projectRootDirectory is None:
                raise Exception("When resolvedPath is None then rootDirs and projectRootDirectory can not be None")
            self.ResolvedPath = self.__ResolvePath(basicConfig, rootDirs, self.Name, projectRootDirectory)
        self.ResolvedPathEx = "{0}/".format(self.ResolvedPath) if len(self.ResolvedPath) > 0 else ""
        self.ScanMethod = basedUponXML.ScanMethod


    def __ResolvePath(self, basicConfig: BasicConfig, rootDirs: List[ToolConfigRootDirectory], entryName: str, projectRootDirectory: str) -> str:
        rootDir = self.__LocateRootDir(basicConfig, rootDirs, entryName, projectRootDirectory)
        return entryName.replace(rootDir.Name, rootDir.ResolvedPath)


    def __LocateRootDir(self, basicConfig: BasicConfig, rootDirs: List[ToolConfigRootDirectory], entryName: str, projectRootDirectory: str) -> ToolConfigRootDirectory:
        if projectRootDirectory is None or not entryName.startswith(MagicStrings.ProjectRoot):
            for rootDir in rootDirs:
                if entryName.startswith(rootDir.Name):
                    return rootDir
        else:
            # Lets try to locate a root directory which the project is based in,
            # then use it to dynamically add a new allowed root directory based on the project file location
            for rootDir in rootDirs:
                if projectRootDirectory == rootDir.ResolvedPath:
                    return ToolConfigRootDirectory(basicConfig, None, rootDir.ProjectId, rootDir, MagicStrings.ProjectRoot, rootDir.DynamicName)
                elif projectRootDirectory.startswith(rootDir.ResolvedPathEx):
                    dynamicRootDir = projectRootDirectory[len(rootDir.ResolvedPathEx):]
                    dynamicRootDir = "{0}/{1}".format(rootDir.Name, dynamicRootDir)
                    return ToolConfigRootDirectory(basicConfig, None, rootDir.ProjectId, rootDir, MagicStrings.ProjectRoot, dynamicRootDir)
        raise Exception("Path '{0}' is not based on one of the valid root directories {1}".format(entryName, ", ".join(Util.ExtractNames(rootDirs))))


class ToolConfigPackageLocationBlacklistEntry(object):
    def __init__(self, sourceRootPath: str, relativePath: str) -> None:
        self.RootDirPath = IOUtil.NormalizePath(sourceRootPath)
        self.RelativeDirPath = IOUtil.NormalizePath(relativePath)
        self.AbsoluteDirPath = IOUtil.Join(sourceRootPath, relativePath)


# TODO: improve interface, dont allow so many None (remove None from rootDirs and projectRootDirectory)
class ToolConfigPackageLocation(ToolConfigLocation):
    def __init__(self, basicConfig: BasicConfig,
                 rootDirs: Optional[List[ToolConfigRootDirectory]],
                 basedUponXML: XmlConfigPackageLocation,
                 projectRootDirectory: Optional[str],
                 resolvedPath: Optional[str] = None) -> None:
        super().__init__(basicConfig, rootDirs, basedUponXML, projectRootDirectory, resolvedPath)
        self.Blacklist = [ToolConfigPackageLocationBlacklistEntry(self.ResolvedPath, entry.Name) for entry in basedUponXML.Blacklist]


class ToolConfigPackageConfigurationLocationSetup(object):
    def __init__(self, name: str, scanMethod: Optional[int] = None, blacklist: Optional[List[str]] = None) -> None:
        self.Name = name
        self.ScanMethod = scanMethod
        self.Blacklist = blacklist


ToolConfigPackageConfigurationAddLocationType = Union[str, ToolConfigPackageConfigurationLocationSetup, List[str], List[ToolConfigPackageConfigurationLocationSetup]]


class ToolConfigPackageConfiguration(object):
    def __init__(self, basicConfig: BasicConfig, rootDirs: List[ToolConfigRootDirectory],
                 basedUponXML: XmlConfigPackageConfiguration, configFileName: str, projectRootDirectory: str) -> None:
        super().__init__()
        self.__basicConfig = basicConfig
        self.BasedOn = basedUponXML
        self.Name = basedUponXML.Name
        self.Preload = basedUponXML.Preload
        self.Locations = self.__ResolveLocations(basicConfig, rootDirs, basedUponXML.Locations, configFileName, projectRootDirectory)

    def ClearLocations(self) -> None:
        self.Locations = []

    def AddLocations(self, newRootLocations: ToolConfigPackageConfigurationAddLocationType) -> None:
        # done in two steps to make mypy happy
        if isinstance(newRootLocations, str):
            newRootLocations = [newRootLocations]
        if isinstance(newRootLocations, ToolConfigPackageConfigurationLocationSetup):
            newRootLocations = [newRootLocations]

        for rootLocation in newRootLocations:
            if isinstance(rootLocation, str):
                resolvedPath = rootLocation
                fakeXml = FakeXmlConfigPackageLocation(self.__basicConfig, rootLocation)
            elif isinstance(rootLocation, ToolConfigPackageConfigurationLocationSetup):
                resolvedPath = rootLocation.Name
                fakeXml = FakeXmlConfigPackageLocation(self.__basicConfig, rootLocation.Name, rootLocation.ScanMethod, rootLocation.Blacklist)
            else:
                raise Exception("Unsupported type")
            self.Locations.append(ToolConfigPackageLocation(self.__basicConfig, None, fakeXml, None, resolvedPath))



    def __ResolveLocations(self, basicConfig: BasicConfig,
                           rootDirs: List[ToolConfigRootDirectory], locations: List[XmlConfigPackageLocation],
                           configFileName: str, projectRootDirectory: str) -> List[ToolConfigPackageLocation]:
        # Check for unique names and
        # convert to a ToolConfigPackageLocation list
        res = [] # List[ToolConfigPackageLocation]
        uniqueLocationIds = set() # type: Set[str]
        for location in locations:
            if not location.Id in uniqueLocationIds:
                uniqueLocationIds.add(location.Id)
                packageLocation = ToolConfigPackageLocation(basicConfig, rootDirs, location, projectRootDirectory)
                res.append(packageLocation)
            else:
                raise DuplicatedConfigPackageLocation(location.Name, configFileName)

        # We sort it so that the longest paths come first meaning we will always find the most exact match first
        # if searching from the front to the end of the list and comparing to 'startswith'
        res.sort(key=lambda s: -len(s.ResolvedPath))
        return res



class ToolContentBuilder(object):
    def __init__(self, basedUponXML: XmlConfigContentBuilder) -> None:
        super().__init__()
        self.BasedOn = basedUponXML
        self.Name = basedUponXML.Name
        self.Executable = basedUponXML.Executable
        self.Parameters = basedUponXML.Parameters
        self.FeatureRequirements = basedUponXML.FeatureRequirements
        self.DefaultExtensions = basedUponXML.DefaultExtensions
        self.Description = basedUponXML.Description


class ToolConfigContentBuilderConfiguration(object):
    def __init__(self, basedUponXML: XmlConfigContentBuilderConfiguration, configFileName: str) -> None:
        super().__init__()
        self.BasedOn = basedUponXML
        self.ContentBuilders = self.__ResolveContentBuilders(basedUponXML.ContentBuilders, configFileName) if basedUponXML else []


    def __ResolveContentBuilders(self, contentBuilders: List[XmlConfigContentBuilder], configFileName: str) -> List[ToolContentBuilder]:
        uniqueNames = set()  # type: Set[str]
        res = []  # type: List[ToolContentBuilder]
        for contentBuilder in contentBuilders:
            newContentBuilder = ToolContentBuilder(contentBuilder)
            if not newContentBuilder.Name in uniqueNames:
                uniqueNames.add(newContentBuilder.Name)
                res.append(newContentBuilder)
            else:
                raise DuplicatedConfigContentBuilder(newContentBuilder.Name, configFileName)
        return res


class ToolConfig(object):
    def __init__(self, basicConfig: BasicConfig, filename: str, projectRootConfig: XmlProjectRootConfigFile) -> None:
        super().__init__()
        basedUponXML = XmlToolConfigFile(basicConfig, filename, projectRootConfig)
        self.BasedOn = basedUponXML
        self.GenFileName = basedUponXML.GenFileName.Name
        self.RootDirectories = self.__ResolveRootDirectories(basicConfig, basedUponXML.RootDirectories, filename)
        self.TemplateImportDirectories = self.__ResolveDirectories(basicConfig, basedUponXML.TemplateImportDirectories)
        self.PackageConfiguration = self.__ResolvePackageConfiguration(basicConfig, self.RootDirectories, basedUponXML.PackageConfiguration, filename, projectRootConfig.RootDirectory)
        self.TemplateFolder = ToolConfigTemplateFolder(basicConfig, basedUponXML.TemplateFolder)
        self.NewProjectTemplateRootDirectories = self.__ResolveNewProjectTemplateRootDirectories(basicConfig, basedUponXML.NewProjectTemplateRootDirectories)
        self.ContentBuilderConfiguration = ToolConfigContentBuilderConfiguration(basedUponXML.ContentBuilderConfiguration, filename) if basedUponXML.ContentBuilderConfiguration is not None else None
        self.UnitTestPath = self.__TryResolveUnitTestPath()
        self.DefaultPackageLanguage = projectRootConfig.DefaultPackageLanguage
        self.DefaultCompany = projectRootConfig.DefaultCompany
        self.RequirePackageCreationYear = projectRootConfig.RequirePackageCreationYear
        self.ProjectRootConfig = projectRootConfig
        self.ProjectInfo = self.__GenerateProjectInfo(basicConfig, projectRootConfig)
        self.BuildDocConfiguration = self.__TryGetBuildDocConfiguration(basedUponXML.BuildDocConfiguration)
        self.ClangFormatConfiguration = self.__TryGetClangFormatConfiguration(basedUponXML.ClangFormatConfiguration)
        self.ClangTidyConfiguration = self.__TryGetClangTidyConfiguration(basedUponXML.ClangTidyConfiguration)
        self.CMakeConfiguration = self.__GetCMakeConfiguration(basedUponXML.CMakeConfiguration)
        self.CompilerConfigurationDict = self.__ProcessCompilerConfiguration(basicConfig, basedUponXML.CompilerConfiguration)
        self.RequirementTypes = [PackageRequirementTypeString.Extension, PackageRequirementTypeString.Feature]
        self.Experimental = self.__ResolveExperimental(basicConfig, self.RootDirectories, basedUponXML.Experimental, filename, projectRootConfig.RootDirectory) # type: Optional[ToolConfigExperimental]

    def GetMinimalConfig(self) -> ToolMinimalConfig:
        ignoreDirectories = []  # type: List[str]
        # ignore the template import directory
        for templateImport in self.TemplateImportDirectories:
            ignoreDirectories.append(templateImport.ResolvedPathEx)
        # ignore the NewProjectTemplateRootDirectories
        for newProjectTemplate in self.NewProjectTemplateRootDirectories:
            ignoreDirectories.append(newProjectTemplate.ResolvedPathEx)
        return ToolMinimalConfig(self.RootDirectories, ignoreDirectories)

    def __TryGetBuildDocConfiguration(self, configList: List[XmlBuildDocConfiguration]) -> BuildDocConfiguration:
        requirementList = [] # type: List[BuildDocConfigurationRequirement]
        if len(configList) < 1:
            return BuildDocConfiguration(requirementList)
        config = configList[0];
        for requirement in config.Requirements:
            requirementList.append(BuildDocConfigurationRequirement(requirement.Name, requirement.Skip))
        return BuildDocConfiguration(requirementList)

    def __TryGetClangFormatConfiguration(self, configList: List[XmlClangFormatConfiguration]) -> Optional[ClangFormatConfiguration]:
        if len(configList) < 1:
            return None
        config = configList[0];
        return ClangFormatConfiguration(config.FileExtensions, config.Recipe)

    def __TryGetClangTidyConfiguration(self, configList: List[XmlClangTidyConfiguration]) -> Optional[ClangTidyConfiguration]:
        if len(configList) < 1:
            return None
        config = configList[0];
        platforms = self.__GetClangTidyPlatforms(config.Platforms)
        clangConfig =  ClangTidyConfiguration(config.FileExtensions, config.Recipe, platforms)
        allPlatformName = 'all'
        if allPlatformName in clangConfig.PlatformDict:
            # append the all configuration to all other configurations
            allPlatform = clangConfig.PlatformDict[allPlatformName]
            for platform in clangConfig.PlatformDict.values():
                if platform != allPlatform:
                    platform.Merge(allPlatform)
        return clangConfig

    def __GetClangTidyPlatforms(self, clangTidyPlatforms: List[XmlClangTidyPlatform]) -> List[ClangTidyPlatform]:
        res = [] # type: List[ClangTidyPlatform]
        for platform in clangTidyPlatforms:
            res.append(self.__GetClangTidyPlatform(platform))
        return res

    def __GetClangTidyPlatform(self, clangTidyPlatform: XmlClangTidyPlatform) -> ClangTidyPlatform:
        compiler = self.__GetClangTidyPlatformCompiler(clangTidyPlatform.Compiler)
        defines = self.__GetClangTidyPlatformDefines(clangTidyPlatform.Defines)
        strictChecks = self.__GetClangTidyPlatformStrictChecks(clangTidyPlatform.StrictChecks)
        return ClangTidyPlatform(clangTidyPlatform.Name, compiler, defines, strictChecks)

    def __GetClangTidyPlatformCompiler(self, compiler: Optional[XmlClangTidyPlatformCompiler]) -> ClangTidyPlatformCompiler:
        if compiler is None:
            return ClangTidyPlatformCompiler([])
        return ClangTidyPlatformCompiler(compiler.Flags)

    def __GetClangTidyPlatformDefines(self, defines: Optional[XmlClangTidyPlatformDefines]) -> ClangTidyPlatformDefines:
        definesAll = []       # type: List[str]
        definesDebug = []     # type: List[str]
        definesRelease = []   # type: List[str]
        if defines is not None:
            definesAll += defines.All
            definesDebug += defines.Debug
            definesRelease += defines.Release
        return ClangTidyPlatformDefines(definesAll, definesDebug, definesRelease)

    def __GetClangTidyPlatformStrictChecks(self, strictChecks: Optional[XmlClangTidyPlatformStrictChecks]) -> Set[str]:
        if strictChecks is None:
            return set()
        return strictChecks.Checks

    def __GetCMakeConfiguration(self, configList: List[XmlCMakeConfiguration]) -> CMakeConfiguration:
        if len(configList) != 1:
            if len(configList) <= 0:
                return CMakeConfiguration("${TopProjectRoot}/build", None, CMakeUtil.GetMinimumVersion(), [])
            raise Exception("There can only be one CMakeConfiguration")
        configEntry = configList[0]

        defaultBuildDir = configEntry.DefaultBuildDir
        defaultInstallPrefix = configEntry.DefaultInstallPrefix
        minVersion = self.__ParseCMakeVersionString(configEntry.MinVersion)

        platformList = [] # type: List[CMakeConfigurationPlatform]
        for platformEntry in configEntry.Platforms:
            # Default to the platform one if its defined, else default to the general one (which can be None)
            platformEntryDefaultInstallPrefix = platformEntry.DefaultInstallPrefix if platformEntry.DefaultInstallPrefix is not None else defaultInstallPrefix
            # Generate the platform config object
            platformList.append(CMakeConfigurationPlatform(platformEntry.Name, platformEntry.DefaultGeneratorName, platformEntryDefaultInstallPrefix))

        if defaultBuildDir is None:
            raise Exception("CMakleConfiguration.DefaultBuildDir must be defined")
        return CMakeConfiguration(defaultBuildDir, defaultInstallPrefix, minVersion, platformList)


    def __ParseCMakeVersionString(self, versionStr: Optional[str]) -> CMakeVersion:
        toolMin = CMakeUtil.GetMinimumVersion()
        if versionStr is None:
            return toolMin
        parsedMinVersion = Util.ParseVersionString(versionStr, maxValues=3)
        while len(parsedMinVersion) < 3:
            parsedMinVersion.append(0)
        projectMin = CMakeVersion(parsedMinVersion[0], parsedMinVersion[1], parsedMinVersion[2])
        return projectMin if projectMin >= toolMin else toolMin


    def __GenerateProjectInfo(self, log: Log, projectRootConfig: XmlProjectRootConfigFile) -> ToolConfigProjectInfo:
        rootProjectBasePackages = self.__ResolveBasePackages(log, projectRootConfig.XmlBasePackages, projectRootConfig.SourceFileName)

        result = [] #  type: List[ToolConfigProjectContext]
        rootProjectContext = ToolConfigProjectContext(projectRootConfig.ProjectId, projectRootConfig.ProjectName,
                                                      projectRootConfig.ProjectVersion, projectRootConfig.RootDirectory,
                                                      rootProjectBasePackages, None)
        result.append(rootProjectContext)
        topProjectContext = rootProjectContext
        # FIX: context base packages does not resolve correctly if we have multiple extension projects inheriting
        #      We basically need to do a proper graph to resolve the hierarchy (but for the current simple use cases of max one extend it works)
        if len(projectRootConfig.ExtendedProject) > 1:
            raise Exception("Only one extended project supported")
        for entry in projectRootConfig.ExtendedProject:
            contextBasePackages = self.__ResolveBasePackages(log, entry.XmlBasePackages, entry.SourceFileName)
            if len(rootProjectBasePackages) > 0:
                contextBasePackages = rootProjectBasePackages + contextBasePackages
            extendedProjectContext = ToolConfigProjectContext(entry.ProjectId, entry.ProjectName, entry.ProjectVersion, entry.RootDirectory, contextBasePackages, rootProjectContext)
            result.append(extendedProjectContext)
            topProjectContext = extendedProjectContext
        return ToolConfigProjectInfo(result, topProjectContext)


    def __ResolveBasePackages(self, log: Log, basePackages: List[XmlConfigFileAddBasePackage], configFileName: str) -> List[ToolConfigBasePackage]:
        uniqueNameIds = set()  # type: Set[str]
        basePackageList = []  # type: List[ToolConfigBasePackage]
        for basePackageEntry in basePackages:
            basePackage = ToolConfigBasePackage(log, basePackageEntry.Name)
            if not basePackage.Id in uniqueNameIds:
                uniqueNameIds.add(basePackage.Id)
                basePackageList.append(basePackage)
            else:
                raise DuplicatedConfigBasePackage(basePackage.Name, configFileName)
        basePackageList.sort(key=lambda s: s.Name)
        return basePackageList


    def GetVisualStudioDefaultVersion(self) -> int:
        visualStudioId = CompilerNames.VisualStudio.lower()
        if visualStudioId in self.CompilerConfigurationDict:
            return self.CompilerConfigurationDict[visualStudioId].DefaultVersion
        return VisualStudioVersion.DEFAULT


    def TryToPath(self, path: Optional[str]) -> Optional[str]:
        if path is None:
            return None
        return ToolConfigPackageRootUtil.TryToPath(self.RootDirectories, path)


    def ToPath(self, path: str) -> str:
        return ToolConfigPackageRootUtil.ToPath(self.RootDirectories, path)

    def TryFindRootDirectory(self, path: Optional[str]) -> Optional[ToolConfigRootDirectory]:
        return ToolConfigPackageRootUtil.TryFindRootDirectory(self.RootDirectories, path)

    def ToBashPath(self, path: str) -> str:
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '{0}'".format(path))
        for rootDir in self.RootDirectories:
            if path.startswith(rootDir.ResolvedPathEx):
                lenRootPath = len(rootDir.ResolvedPathEx)
                path = path[lenRootPath:]
                return rootDir.BashName + "/" + Util.UTF8ToAscii(path)
            elif path == rootDir.ResolvedPath:
                return rootDir.Name + "/"
        raise UsageErrorException("the folder '{0}' does not reside inside one of the root dirs".format(path))



    def TryLegacyToBashPath(self, path: Optional[str]) -> Optional[str]:
        if path is None:
            return None
        return self.ToBashPath(path)


    def ToBashPathDirectConversion(self, path: str) -> str:
        """ This does not make the path relative to a root path """
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '{0}'".format(path))
        path = Util.ChangeToBashEnvVariables(path)
        return Util.UTF8ToAscii(path).replace('\\', '/')


    def TryLegacyToBashPathDirectConversion(self, path: Optional[str]) -> Optional[str]:
        """ This does not make the path relative to a root path """
        if path is None:
            return None
        return self.ToBashPathDirectConversion(path)


    def ToDosPath(self, path: str) -> str:
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '{0}'".format(path))
        for rootDir in self.RootDirectories:
            if path.startswith(rootDir.ResolvedPathEx):
                lenRootPath = len(rootDir.ResolvedPathEx)
                path = path[lenRootPath:]
                tmp = rootDir.DosName + "/" + Util.UTF8ToAscii(path)
                return tmp.replace('/', '\\')
            elif path == rootDir.ResolvedPath:
                tmp = rootDir.Name + "/"
                return tmp.replace('/', '\\')
        raise UsageErrorException("the folder '{0}' does not reside inside one of the root dirs".format(path))


    def TryLegacyToDosPath(self, path: Optional[str]) -> Optional[str]:
        if path is None:
            return None
        return self.ToDosPath(path)


    def ToDosPathDirectConversion(self, path: str) -> str:
        """ This does not make the path relative to a root path """
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '{0}'".format(path))
        path = Util.ChangeToDosEnvironmentVariables(path)
        return Util.UTF8ToAscii(path).replace('/', '\\')


    def TryLegacyToDosPathDirectConversion(self, path: Optional[str]) -> Optional[str]:
        """ This does not make the path relative to a root path """
        if path is None:
            return None
        return self.ToDosPathDirectConversion(path)


    def __ResolveNewProjectTemplateRootDirectories(self, basicConfig: BasicConfig,
                                                   newProjectTemplateRootDirectories: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]) -> List[NewProjectTemplateRootDirectory]:
        uniqueIdDict = {}  # type: Dict[str, NewProjectTemplateRootDirectory]
        rootDirs = []  # type: List[NewProjectTemplateRootDirectory]
        for rootDir in newProjectTemplateRootDirectories:
            toolRootDir = NewProjectTemplateRootDirectory(basicConfig, rootDir)
            if not toolRootDir.Id in uniqueIdDict:
                uniqueIdDict[toolRootDir.Id] = toolRootDir
                rootDirs.append(toolRootDir)
            else:
                raise DuplicatedNewProjectTemplatesRootPath(toolRootDir.Name, uniqueIdDict[toolRootDir.Id].Name, toolRootDir.Name)
        # We sort it so that the longest paths come first meaning we will always find the most exact match first
        # if searching from the front to the end of the list and comparing to 'startswith'
        rootDirs.sort(key=lambda s: -len(s.ResolvedPathEx))
        return rootDirs


    def __ResolveRootDirectories(self, basicConfig: BasicConfig,
                                 rootDirectories: List[XmlConfigFileAddRootDirectory],
                                 configFileName: str) -> List[ToolConfigRootDirectory]:
        uniqueNames = set()  # type: Set[str]
        rootDirs = []  # type: List[ToolConfigRootDirectory]
        for rootDir in rootDirectories:
            toolRootDir = ToolConfigRootDirectory(basicConfig, rootDir, rootDir.ProjectId)
            if not toolRootDir.Name in uniqueNames:
                uniqueNames.add(toolRootDir.Name)
                rootDirs.append(toolRootDir)
            else:
                raise DuplicatedConfigRootPath(toolRootDir.Name, configFileName)
        # We sort it so that the longest paths come first meaning we will always find the most exact match first
        # if searching from the front to the end of the list and comparing to 'startswith'
        rootDirs.sort(key=lambda s: -len(s.ResolvedPathEx))
        return rootDirs


    def __ResolveDirectories(self, basicConfig: BasicConfig, directories: List[XmlConfigFileAddTemplateImportDirectory]) -> List[ToolConfigDirectory]:
        dirs = []  # type: List[ToolConfigDirectory]
        for dirEntry in directories:
            dirs.append(ToolConfigDirectory(basicConfig, dirEntry))
        return dirs


    def __ResolvePackageConfiguration(self, basicConfig: BasicConfig, rootDirs: List[ToolConfigRootDirectory],
                                      packageConfiguration: Dict[str, XmlConfigPackageConfiguration],
                                      configFileName: str, projectRootDirectory: str) -> Dict[str, ToolConfigPackageConfiguration]:
        configs = {} # type Dict[str, ToolConfigPackageConfiguration]
        for packageConfig in list(packageConfiguration.values()):
            resolvedConfig = ToolConfigPackageConfiguration(basicConfig, rootDirs, packageConfig, configFileName, projectRootDirectory)
            configs[resolvedConfig.Name] = resolvedConfig
        return configs


    def __ResolveExperimental(self, basicConfig: BasicConfig, rootDirs: List[ToolConfigRootDirectory],
                              experimental: Optional[XmlExperimental],
                              configFileName: str, projectRootDirectory: str) -> Optional[ToolConfigExperimental]:
        if experimental is None:
            return None
        return ToolConfigExperimental(basicConfig, rootDirs, experimental, configFileName, projectRootDirectory)


    def __TryResolveUnitTestPath(self) -> Optional[str]:
        path = os.environ.get("FSL_GRAPHICS_INTERNAL")
        if path is None:
            return None
        return IOUtil.Join(path, "Tools/FslBuildGen/FslBuildGen/UnitTest/TestFiles")


    def __ProcessCompilerConfiguration(self, basicConfig: BasicConfig,
                                       xmlCompilerConfiguration: List[XmlConfigCompilerConfiguration]) -> Dict[str, ToolConfigCompilerConfiguration]:
        result = {}  # type: Dict[str, ToolConfigCompilerConfiguration]
        for config in xmlCompilerConfiguration:
            if config.Id in result:
                raise XmlDuplicatedCompilerConfigurationException(result[config.Id].BasedOn.XMLElement, result[config.Id].Name, config.XMLElement, config.Name)
            elif config.Name == CompilerNames.VisualStudio:
                result[config.Id] = ToolConfigCompilerConfiguration(basicConfig, config)
            else:
                msg = "CompilerConfiguration name: '{0}' is not currently supported, so entry is ignored".format(config.Name)
                basicConfig.LogPrint(msg)
        return result
