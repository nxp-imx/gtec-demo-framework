#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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

from typing import Any
from typing import Dict
from typing import Optional
from typing import List
from typing import Union
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.DataTypes import MagicStrings
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Exceptions import FileNotFoundException
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.Xml.Exceptions import XmlException
from FslBuildGen.Vars.VariableEnvironment import VariableEnvironment
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.Exceptions import XmlInvalidRootElement
from FslBuildGen.Xml.Project.XmlBuildDocConfiguration import XmlBuildDocConfiguration
from FslBuildGen.Xml.Project.XmlClangTidyConfiguration import XmlClangTidyConfiguration
from FslBuildGen.Xml.Project.XmlCMakeConfiguration import XmlCMakeConfiguration
from FslBuildGen.Xml.Project.XmlExperimentalPlatform import XmlExperimentalPlatform
from FslBuildGen.Xml.ToolConfig import LoadUtil
from FslBuildGen.Xml.ToolConfig.XmlConfigFileAddNewProjectTemplatesRootDirectory import XmlConfigFileAddNewProjectTemplatesRootDirectory
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageConfiguration import XmlConfigPackageConfiguration
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageLocation import XmlConfigPackageLocation
from FslBuildGen.Xml.XmlBase import XmlBase

class LocalInvalidValues(object):
    INVALID_FILE_NAME = "**NotDefined**"
    INVALID_COMPANY_NAME = "**INVALID_COMPANY_NAME**"


class XmlConfigFileAddBasePackage(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')


class XmlConfigFileAddRootDirectory(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element, projectId: ProjectId) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.Id = self.Name.lower()
        self.Create = self._ReadBoolAttrib(xmlElement, 'Create', False)
        self.ProjectId = projectId


class XmlClangFormatConfiguration(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        fileExtensions = self._ReadAttrib(xmlElement, "FileExtensions")
        self.FileExtensions = fileExtensions.split(';')
        self.Recipe = self._ReadAttrib(xmlElement, "Recipe")


class XmlConfigCompilerConfiguration(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.Platform = self._ReadAttrib(xmlElement, 'Platform')
        self.DefaultVersion = self._ReadAttrib(xmlElement, 'DefaultVersion')
        self.Id = self.Name.lower()


class XmlExperimentalDefaultThirdPartyInstallDirectory(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')


class XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')


class XmlExperimental(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.DefaultThirdPartyInstallDirectory = self.__TryLoadInstallDirectory(log, xmlElement)
        self.DefaultThirdPartyInstallReadonlyCacheDirectory = self.__TryLoadReadonlyCacheDirectory(log, xmlElement) # type: Optional[XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory]

        self.AllowDownloads = self._ReadBoolAttrib(xmlElement, "AllowDownloads")
        self.DisableDownloadEnv = self._ReadAttrib(xmlElement, "DisableDownloadEnv")
        self.Platforms = self.__TryLoadPlatforms(log, xmlElement)                     # type: Dict[str, XmlExperimentalPlatform]


    def TryGetRecipesDefaultValue(self, platformName: str) -> Optional[str]:
        if platformName in self.Platforms:
            platform = self.Platforms[platformName]
            if platform.Recipes is not None:
                return platform.Recipes.Value
        return None

    def __TryLoadInstallDirectory(self, log: Log, xmlElement: ET.Element) -> Optional[XmlExperimentalDefaultThirdPartyInstallDirectory]:
        extendedElement = xmlElement.find("DefaultThirdPartyInstallDirectory")
        if extendedElement is None:
            return None
        return XmlExperimentalDefaultThirdPartyInstallDirectory(log, extendedElement)


    def __TryLoadReadonlyCacheDirectory(self, log: Log, xmlElement: ET.Element) -> Optional[XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory]:
        extendedElement = xmlElement.find("DefaultThirdPartyInstallReadonlyCacheDirectory")
        if extendedElement is None:
            return None
        return XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory(log, extendedElement)

    def __TryLoadPlatforms(self, log: Log, xmlElement: ET.Element) -> Dict[str, XmlExperimentalPlatform]:
        platformDict = {}                                      # type: Dict[str, XmlExperimentalPlatform]
        platformElements = xmlElement.findall("Platform")
        if platformElements is not None and len(platformElements) > 0:
            for element in platformElements:
                platform = XmlExperimentalPlatform(log, element)
                if platform.Id in platformDict:
                    errorMsg = "Multiple platforms called '{0}' found in Project.gen".format(platform.Id)
                    raise XmlException2(element, errorMsg)
                platformDict[platform.Id] = platform
        return platformDict


    def Merge(self, src: Optional['XmlExperimental']) -> None:
        if src is None:
            return
        if not src.DefaultThirdPartyInstallDirectory is None:
            raise Exception("DefaultThirdPartyInstallDirectory can only be set by the root package")
        if not src.DefaultThirdPartyInstallReadonlyCacheDirectory is None:
            raise Exception("DefaultThirdPartyInstallReadonlyCacheDirectory can only be set by the root package")


def _LoadPackageConfigurations(log: Log, projectElem: ET.Element, filename: str) -> List[XmlConfigPackageConfiguration]:
    xmlPackageConfigurations = LoadUtil.XMLLoadPackageConfiguration(log, projectElem, filename)
    for entry in xmlPackageConfigurations:
        # if no locations has been supplied then we assume the root folder of the project file
        #if entry.Name == 'default' and len(entry.Locations) <= 0:
        if len(entry.Locations) <= 0:
            xmlConfigPackageLocation = XmlConfigPackageLocation(log, FakeXmlElementFactory.CreateWithName("PackageLocation", MagicStrings.ProjectRoot))
            entry.Locations = [xmlConfigPackageLocation]
    return xmlPackageConfigurations


def _LoadAddBasePackage(log: Log, xmlElement: ET.Element, filename: str) -> List[XmlConfigFileAddBasePackage]:
    res = []
    foundElements = xmlElement.findall("AddBasePackage")
    for foundElement in foundElements:
        res.append(XmlConfigFileAddBasePackage(log, foundElement))
    return res


def _LoadAddRootDirectory(log: Log, xmlElement: ET.Element, filename: str, projectId: ProjectId) -> List[XmlConfigFileAddRootDirectory]:
    res = []
    foundElements = xmlElement.findall("AddRootDirectory")
    for foundElement in foundElements:
        res.append(XmlConfigFileAddRootDirectory(log, foundElement, projectId))

    if len(res) < 1:
        raise XmlException("The file '{0}' did not contain at least one AddRootDirectory element".format(filename))

    return res

def _LoadBuildDocConfiguration(log: Log, xmlElement: ET.Element, filename: str) -> List[XmlBuildDocConfiguration]:
    res = []
    foundElements = xmlElement.findall("BuildDocConfiguration")
    for foundElement in foundElements:
        res.append(XmlBuildDocConfiguration(log, foundElement))

    if len(res) > 1:
        raise XmlException("The file '{0}' contained more than one BuildDocConfiguration".format(filename))

    return res


def _LoadCMakeConfiguration(log: Log, xmlElement: ET.Element, filename: str) -> List[XmlCMakeConfiguration]:
    res = []
    foundElements = xmlElement.findall("CMakeConfiguration")
    for foundElement in foundElements:
        res.append(XmlCMakeConfiguration(log, foundElement))
    return res

def _LoadClangFormatConfiguration(log: Log, xmlElement: ET.Element, filename: str) -> List[XmlClangFormatConfiguration]:
    res = []
    foundElements = xmlElement.findall("ClangFormatConfiguration")
    for foundElement in foundElements:
        res.append(XmlClangFormatConfiguration(log, foundElement))
    return res

def _LoadClangTidyConfiguration(log: Log, xmlElement: ET.Element, filename: str) -> List[XmlClangTidyConfiguration]:
    res = []
    foundElements = xmlElement.findall("ClangTidyConfiguration")
    for foundElement in foundElements:
        res.append(XmlClangTidyConfiguration(log, foundElement))
    return res

def _LoadCompilerConfiguration(log: Log, xmlElement: ET.Element, filename: str) -> List[XmlConfigCompilerConfiguration]:
    res = []
    foundElements = xmlElement.findall("CompilerConfiguration")
    for foundElement in foundElements:
        res.append(XmlConfigCompilerConfiguration(log, foundElement))
    return res


def _TryLoadExperimental(log: Log, xmlElement: ET.Element, filename: str) -> Optional[XmlExperimental]:
    extendedElement = xmlElement.find("Experimental")
    if extendedElement is None:
        return None
    return XmlExperimental(log, extendedElement)

class XmlExtendedProject(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element, filename: str) -> None:
        super().__init__(log, xmlElement)
        #raise Exception("ExtendedProject not implemented");
        self.ProjectName = self._ReadAttrib(xmlElement, 'Name') # type: str
        self.ShortProjectName = self._TryReadAttrib(xmlElement, 'ShortName') # type: Optional[str]
        self.ProjectVersion = self._ReadAttrib(xmlElement, 'Version', "1.0.0.0") # type: str
        self.RootDirectory = IOUtil.GetDirectoryName(filename)
        self.Parent = self._ReadAttrib(xmlElement, 'Parent')  # type: str
        self.ParentRoot = self._ReadAttrib(xmlElement, 'ParentRoot')  # type: str
        configFilename = IOUtil.GetFileName(filename)   # type: str
        self.ParentConfigFilename = IOUtil.Join(self.ParentRoot, configFilename)  # type: str
        self.SourceFileName = filename  # type: str

        self.ProjectId = ProjectId(self.ProjectName, self.ShortProjectName)

        variableProcessor = VariableProcessor(log)
        self.AbsoluteParentConfigFilename = variableProcessor.ResolveAbsolutePathWithLeadingEnvironmentVariablePath(self.ParentConfigFilename)
        self.XmlPackageConfiguration = _LoadPackageConfigurations(log, xmlElement, filename)  # type: List[XmlConfigPackageConfiguration]
        self.XmlBasePackages = _LoadAddBasePackage(log, xmlElement, filename) # type: List[XmlConfigFileAddBasePackage]
        self.XmlRootDirectories = _LoadAddRootDirectory(log, xmlElement, filename, self.ProjectId) # type: List[XmlConfigFileAddRootDirectory]
        self.XmlNewProjectTemplatesRootDirectories = LoadUtil.LoadAddNewProjectTemplatesRootDirectory(log, xmlElement, filename)
        self.XmlBuildDocConfiguration = _LoadBuildDocConfiguration(log, xmlElement, filename)  # type: List[XmlBuildDocConfiguration]
        self.XmlClangFormatConfiguration = _LoadClangFormatConfiguration(log, xmlElement, filename)  # type: List[XmlClangFormatConfiguration]
        self.XmlClangTidyConfiguration = _LoadClangTidyConfiguration(log, xmlElement, filename)  # type: List[XmlClangTidyConfiguration]
        self.XmlCMakeConfiguration = _LoadCMakeConfiguration(log, xmlElement, filename)  # type: List[XmlCMakeConfiguration]
        self.XmlCompilerConfiguration = _LoadCompilerConfiguration(log, xmlElement, filename)  # type: List[XmlConfigCompilerConfiguration]
        self.XmlExperimental = _TryLoadExperimental(log, xmlElement, filename)  # type: Optional[XmlExperimental]


class XmlProjectRootConfigFile(XmlBase):
    def __init__(self, log: Log, filename: str) -> None:
        xmlElement = self.__LoadXml(log, filename)
        super().__init__(log, xmlElement)

        self.__LoadFromXml(log, xmlElement, filename)
        if not self.XmlExperimental is None and self.XmlExperimental.DefaultThirdPartyInstallDirectory is None:
            raise Exception("DefaultThirdPartyInstallDirectory was not defined")


    def __LoadXml(self, log: Log, filename: str) -> ET.Element:
        """ Careful this code must be self contained as it can be used before the parent class is initialized """
        if filename is None:
            raise Exception("filename can not be None")

        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate config file %s", filename)
        tree = ET.parse(filename)
        xmlElement = tree.getroot()
        if xmlElement.tag != 'FslBuildGenProjectRoot':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGenProjectRoot'")
        return xmlElement


    def __LoadFromXml(self, log: Log, xmlElement: ET.Element, filename: str, canExtend: bool = True) -> None:
        self.Version = '1'  # type: str
        self.ProjectName = "not set"
        self.ProjectVersion = "0.0.0.0"
        self.RootDirectory = LocalInvalidValues.INVALID_FILE_NAME  # type: str
        self.DefaultPackageLanguage = PackageLanguage.CPP  # type: int
        self.DefaultCompany = LocalInvalidValues.INVALID_COMPANY_NAME  # type: str
        self.ToolConfigFile = LocalInvalidValues.INVALID_FILE_NAME  # type: str
        self.RequirePackageCreationYear = False
        self.XmlExperimental = None # type: Optional[XmlExperimental]
        self.XmlPackageConfiguration = []  # type: List[XmlConfigPackageConfiguration]
        self.XmlBasePackages = [] # type: List[XmlConfigFileAddBasePackage]
        self.XmlRootDirectories = []  #  type: List[XmlConfigFileAddRootDirectory]
        self.XmlNewProjectTemplatesRootDirectories = []  # type: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]
        self.XmlCompilerConfiguration = []  # type: List[XmlConfigCompilerConfiguration]
        self.SourceFileName = LocalInvalidValues.INVALID_FILE_NAME  # type: str
        self.DefaultTemplate = MagicStrings.VSDefaultCPPTemplate # type: str
        self.ExtendedProject = []  # type: List[XmlExtendedProject]
        if xmlElement is not None:
            extendedElement = xmlElement.find("ExtendedProject") if canExtend else None
            if extendedElement is None:
                rootDirectory = IOUtil.GetDirectoryName(filename)
                variableEnvironment = VariableEnvironment(self.Log)
                variableEnvironment.Set("PROJECT_ROOT", rootDirectory)
                variableProcessor = VariableProcessor(self.Log, variableEnvironment)
                self.Version = self._ReadAttrib(xmlElement, 'Version')
                self.RootDirectory = rootDirectory
                projectElem = XmlBase._GetElement(self, xmlElement, "Project") # type: ET.Element
                self.ProjectName = self._ReadAttrib(projectElem, 'Name')
                self.ShortProjectName = self._TryReadAttrib(projectElem, 'ShortName')
                self.ProjectId = ProjectId(self.ProjectName, self.ShortProjectName)
                self.ProjectVersion = self._ReadAttrib(projectElem, 'Version', "1.0.0.0")
                toolConfigFilePath = self._ReadAttrib(projectElem, 'ToolConfigFile')  # type: str
                self.DefaultPackageLanguage = self.__GetDefaultPackageLanguage(projectElem)
                self.DefaultCompany = self._ReadAttrib(projectElem, 'DefaultCompany')
                # if this is set to true each package is required to contian a 'CreationYear=""' attribute
                self.RequirePackageCreationYear = self._ReadBoolAttrib(projectElem, 'RequirePackageCreationYear', False)
                self.ToolConfigFile = variableProcessor.ResolvePathToAbsolute(toolConfigFilePath, self.XMLElement)
                self.XmlPackageConfiguration = _LoadPackageConfigurations(log, projectElem, filename)
                self.XmlBasePackages = _LoadAddBasePackage(log, projectElem, filename)
                self.XmlRootDirectories = _LoadAddRootDirectory(log, projectElem, filename, self.ProjectId)
                self.XmlNewProjectTemplatesRootDirectories = LoadUtil.LoadAddNewProjectTemplatesRootDirectory(log, projectElem, filename)
                self.XmlBuildDocConfiguration = _LoadBuildDocConfiguration(log, projectElem, filename)
                self.XmlClangFormatConfiguration = _LoadClangFormatConfiguration(log, projectElem, filename)
                self.XmlClangTidyConfiguration = _LoadClangTidyConfiguration(log, projectElem, filename)
                self.XmlCMakeConfiguration = _LoadCMakeConfiguration(log, projectElem, filename)
                self.XmlCompilerConfiguration = _LoadCompilerConfiguration(log, projectElem, filename)
                self.XmlExperimental = _TryLoadExperimental(log, projectElem, filename)
                self.SourceFileName = filename
                self.DefaultTemplate = self._ReadAttrib(projectElem, 'DefaultTemplate', MagicStrings.VSDefaultCPPTemplate)
            else:
                # Do something with the extended element
                extendedProject = XmlExtendedProject(log, extendedElement, filename)
                parentFileName = extendedProject.AbsoluteParentConfigFilename
                parentElem = self.__LoadXml(log, parentFileName)
                self.__LoadFromXml(log, parentElem, parentFileName, True) # True to allow multiple extensions
                self.ExtendedProject.append(extendedProject)
                self.__ApplyExtended(self.XmlPackageConfiguration, extendedProject.XmlPackageConfiguration, True)
                self.__ApplyExtended(self.XmlRootDirectories, extendedProject.XmlRootDirectories, False)
                self.__ApplyExtended(self.XmlNewProjectTemplatesRootDirectories, extendedProject.XmlNewProjectTemplatesRootDirectories, False)
                self.__ApplyExtended(self.XmlBuildDocConfiguration, extendedProject.XmlBuildDocConfiguration, False)
                self.__ApplyExtended(self.XmlClangFormatConfiguration, extendedProject.XmlClangFormatConfiguration, False)
                self.__ApplyExtended(self.XmlCompilerConfiguration, extendedProject.XmlCompilerConfiguration, False)
                self.__ApplyExtendedExperimental(self.XmlExperimental, extendedProject.XmlExperimental)

        if self.RootDirectory == LocalInvalidValues.INVALID_FILE_NAME:
            raise Exception("RootDirectory not configured")
        if self.DefaultCompany == LocalInvalidValues.INVALID_COMPANY_NAME:
            raise Exception("Default company not configured")
        if self.ToolConfigFile == LocalInvalidValues.INVALID_FILE_NAME:
            raise Exception("ToolConfigFile not configured")
        if self.SourceFileName == LocalInvalidValues.INVALID_FILE_NAME:
            raise Exception("SourceFileName not configured")
        if len(self.XmlBuildDocConfiguration) > 1:
            raise Exception("There can only be one BuildDocConfiguration entry")
        if len(self.XmlClangFormatConfiguration) > 1:
            raise Exception("There can only be one ClangFormatConfiguration entry")
        if len(self.XmlClangTidyConfiguration) > 1:
            raise Exception("There can only be one ClangTidyConfiguration entry")
        if len(self.XmlCMakeConfiguration) > 1:
            raise Exception("There can only be one CMakeConfiguration entry")

    def __ApplyExtendedExperimental(self, dst: Optional[XmlExperimental], src: Optional[XmlExperimental]) -> None:
        if dst is not None:
            dst.Merge(src)

    def __GetDefaultPackageLanguage(self, xmlElement: ET.Element) -> int:
        defaultPackageLanguage = self._ReadAttrib(xmlElement, 'DefaultPackageLanguage', "C++")
        return PackageLanguage.FromString(defaultPackageLanguage)


    # TODO: deal with the dynamic types
    def __ApplyExtended(self, dst: Any, src: Any, canMerge: bool) -> None:
        for entry in src:
            parentItem = self.__TryFindById(dst, entry.Id)
            if parentItem is None:
                dst.append(entry)
            elif canMerge:
                parentItem.Merge(entry)
            else:
                raise Exception("The extended value can not be merged '{0}'.".format(entry.Name))


    # TODO: deal with the dynamic types
    def __TryFindById(self, sourceList: Any, findId: str) -> Optional[Any]:
        for entry in sourceList:
            if entry.Id == findId:
                return entry
        return None
