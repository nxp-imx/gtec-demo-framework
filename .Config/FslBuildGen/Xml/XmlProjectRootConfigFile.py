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
from typing import Optional
from typing import List
from typing import Union
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.DataTypes import MagicStrings
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Exceptions import FileNotFoundException
from FslBuildGen.Xml.Exceptions import XmlException
from FslBuildGen.Vars.VariableProcessor import VariableEnvironment
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import XmlInvalidRootElement
from FslBuildGen.Xml.ToolConfig import LoadUtil
from FslBuildGen.Xml.ToolConfig.XmlConfigFileAddNewProjectTemplatesRootDirectory import XmlConfigFileAddNewProjectTemplatesRootDirectory
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageConfiguration import XmlConfigPackageConfiguration
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageLocation import XmlConfigPackageLocation
from FslBuildGen.Xml.XmlBase import XmlBase


class LocalInvalidValues(object):
    INVALID_FILE_NAME = "**NotDefined**"
    INVALID_COMPANY_NAME = "**INVALID_COMPANY_NAME**"

class XmlConfigFileAddRootDirectory(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlConfigFileAddRootDirectory, self).__init__(basicConfig, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.Id = self.Name.lower()
        self.Create = self._ReadBoolAttrib(xmlElement, 'Create', False)


class XmlConfigCompilerConfiguration(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlConfigCompilerConfiguration, self).__init__(basicConfig, xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name')
        self.Platform = XmlBase._ReadAttrib(self, xmlElement, 'Platform')
        self.DefaultVersion = XmlBase._ReadAttrib(self, xmlElement, 'DefaultVersion')
        self.Id = self.Name.lower()


class XmlExperimentalDefaultThirdPartyInstallDirectory(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlExperimentalDefaultThirdPartyInstallDirectory, self).__init__(basicConfig, xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name')


class XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory, self).__init__(basicConfig, xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name')


class XmlExperimental(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlExperimental, self).__init__(basicConfig, xmlElement)
        self.DefaultThirdPartyInstallDirectory = self.__TryLoadInstallDirectory(basicConfig, xmlElement)
        self.DefaultThirdPartyInstallReadonlyCacheDirectory = self.__TryLoadReadonlyCacheDirectory(basicConfig, xmlElement) # type: Optional[XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory]

        self.AllowDownloads = self._ReadBoolAttrib(xmlElement, "AllowDownloads")
        self.DisableDownloadEnv = self._ReadAttrib(xmlElement, "DisableDownloadEnv")


    def __TryLoadInstallDirectory(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlExperimentalDefaultThirdPartyInstallDirectory]:
        extendedElement = xmlElement.find("DefaultThirdPartyInstallDirectory")
        if extendedElement is None:
            return None
        return XmlExperimentalDefaultThirdPartyInstallDirectory(basicConfig, extendedElement)


    def __TryLoadReadonlyCacheDirectory(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory]:
        extendedElement = xmlElement.find("DefaultThirdPartyInstallReadonlyCacheDirectory")
        if extendedElement is None:
            return None
        return XmlExperimentalDefaultThirdPartyInstallReadonlyCacheDirectory(basicConfig, extendedElement)



    def Merge(self, src: Optional['XmlExperimental']) -> None:
        if src is None:
            return
        if not src.DefaultThirdPartyInstallDirectory is None:
            raise Exception("DefaultThirdPartyInstallDirectory can only be set by the root package")
        if not src.DefaultThirdPartyInstallReadonlyCacheDirectory is None:
            raise Exception("DefaultThirdPartyInstallReadonlyCacheDirectory can only be set by the root package")


def _LoadPackageConfigurations(basicConfig: BasicConfig, projectElem: ET.Element, filename: str) -> List[XmlConfigPackageConfiguration]:
    xmlPackageConfigurations = LoadUtil.XMLLoadPackageConfiguration(basicConfig, projectElem, filename)
    for entry in xmlPackageConfigurations:
        # if no locations has been supplied then we assume the root folder of the project file
        #if entry.Name == 'default' and len(entry.Locations) <= 0:
        if len(entry.Locations) <= 0:
            xmlConfigPackageLocation = XmlConfigPackageLocation(basicConfig, FakeXmlElementFactory.CreateWithName("PackageLocation", MagicStrings.ProjectRoot))
            entry.Locations = [xmlConfigPackageLocation]
    return xmlPackageConfigurations


def _LoadAddRootDirectory(basicConfig: BasicConfig, xmlElement: ET.Element, filename: str) -> List[XmlConfigFileAddRootDirectory]:
    res = []
    foundElements = xmlElement.findall("AddRootDirectory")
    for foundElement in foundElements:
        res.append(XmlConfigFileAddRootDirectory(basicConfig, foundElement))

    if len(res) < 1:
        raise XmlException("The file '{0}' did not contain at least one AddRootDirectory element".format(filename))

    return res


def _LoadCompilerConfiguration(basicConfig: BasicConfig, xmlElement: ET.Element, filename: str) -> List[XmlConfigCompilerConfiguration]:
    res = []
    foundElements = xmlElement.findall("CompilerConfiguration")
    for foundElement in foundElements:
        res.append(XmlConfigCompilerConfiguration(basicConfig, foundElement))
    return res


def _TryLoadExperimental(basicConfig: BasicConfig, xmlElement: ET.Element, filename: str) -> Optional[XmlExperimental]:
    extendedElement = xmlElement.find("Experimental")
    if extendedElement is None:
        return None
    return XmlExperimental(basicConfig, extendedElement)


class XmlExtendedProject(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element, filename: str) -> None:
        super(XmlExtendedProject, self).__init__(basicConfig, xmlElement)
        #raise Exception("ExtendedProject not implemented");
        self.Parent = XmlBase._ReadAttrib(self, xmlElement, 'Parent')  # type: str
        self.ParentRoot = XmlBase._ReadAttrib(self, xmlElement, 'ParentRoot')  # type: str
        configFilename = IOUtil.GetFileName(filename)   # type: str
        self.ParentConfigFilename = IOUtil.Join(self.ParentRoot, configFilename)  # type: str

        variableProcessor = VariableProcessor(basicConfig)
        self.AbsoluteParentConfigFilename = variableProcessor.ResolveAbsolutePathWithLeadingEnvironmentVariablePath(self.ParentConfigFilename)
        self.XmlPackageConfiguration = _LoadPackageConfigurations(basicConfig, xmlElement, filename)  # type: List[XmlConfigPackageConfiguration]
        self.XmlRootDirectories = _LoadAddRootDirectory(basicConfig, xmlElement, filename) # type: List[XmlConfigFileAddRootDirectory]
        self.XmlNewProjectTemplatesRootDirectories = LoadUtil.LoadAddNewProjectTemplatesRootDirectory(basicConfig, xmlElement, filename)
        self.XmlCompilerConfiguration = _LoadCompilerConfiguration(basicConfig, xmlElement, filename)  # type: List[XmlConfigCompilerConfiguration]
        self.XmlExperimental = _TryLoadExperimental(basicConfig, xmlElement, filename)  # type: Optional[XmlExperimental]


class XmlProjectRootConfigFile(XmlBase):
    def __init__(self, basicConfig: BasicConfig, filename: str) -> None:
        xmlElement = self.__LoadXml(basicConfig, filename)
        super(XmlProjectRootConfigFile, self).__init__(basicConfig, xmlElement)

        self.__LoadFromXml(basicConfig, xmlElement, filename)
        if not self.XmlExperimental is None and self.XmlExperimental.DefaultThirdPartyInstallDirectory is None:
            raise Exception("DefaultThirdPartyInstallDirectory was not defined")


    def __LoadXml(self, basicConfig: BasicConfig, filename: str) -> ET.Element:
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


    def __LoadFromXml(self, basicConfig: BasicConfig, xmlElement: ET.Element, filename: str, canExtend: bool = True) -> None:
        self.Version = '1'  # type: str
        self.RootDirectory = LocalInvalidValues.INVALID_FILE_NAME  # type: str
        self.DefaultPackageLanguage = PackageLanguage.CPP  # type: int
        self.DefaultCompany = LocalInvalidValues.INVALID_COMPANY_NAME  # type: str
        self.ToolConfigFile = LocalInvalidValues.INVALID_FILE_NAME  # type: str
        self.RequirePackageCreationYear = False
        self.XmlExperimental = None # type: Optional[XmlExperimental]
        self.XmlPackageConfiguration = []  # type: List[XmlConfigPackageConfiguration]
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
                variableEnvironment = VariableEnvironment(self.BasicConfig)
                variableEnvironment.Set("PROJECT_ROOT", rootDirectory)
                variableProcessor = VariableProcessor(self.BasicConfig, variableEnvironment)
                self.Version = XmlBase._ReadAttrib(self, xmlElement, 'Version')
                self.RootDirectory = rootDirectory
                projectElem = XmlBase._GetElement(self, xmlElement, "Project") # type: ET.Element
                toolConfigFilePath = XmlBase._ReadAttrib(self, projectElem, 'ToolConfigFile')  # type: str
                self.DefaultPackageLanguage = self.__GetDefaultPackageLanguage(projectElem)
                self.DefaultCompany = XmlBase._ReadAttrib(self, projectElem, 'DefaultCompany')
                # if this is set to true each package is required to contian a 'CreationYear=""' attribute
                self.RequirePackageCreationYear = XmlBase._ReadBoolAttrib(self, projectElem, 'RequirePackageCreationYear', False)
                self.ToolConfigFile = variableProcessor.ResolvePathToAbsolute(toolConfigFilePath, self.XMLElement)
                self.XmlPackageConfiguration = _LoadPackageConfigurations(basicConfig, projectElem, filename)
                self.XmlRootDirectories = _LoadAddRootDirectory(basicConfig, projectElem, filename)
                self.XmlNewProjectTemplatesRootDirectories = LoadUtil.LoadAddNewProjectTemplatesRootDirectory(basicConfig, projectElem, filename)
                self.XmlCompilerConfiguration = _LoadCompilerConfiguration(basicConfig, projectElem, filename)
                self.XmlExperimental = _TryLoadExperimental(basicConfig, projectElem, filename)
                self.SourceFileName = filename
                self.DefaultTemplate = XmlBase._ReadAttrib(self, projectElem, 'DefaultTemplate', MagicStrings.VSDefaultCPPTemplate)
            else:
                # Do something with the extended element
                extendedProject = XmlExtendedProject(basicConfig, extendedElement, filename)
                parentFileName = extendedProject.AbsoluteParentConfigFilename
                parentElem = self.__LoadXml(basicConfig, parentFileName)
                self.__LoadFromXml(basicConfig, parentElem, parentFileName, True) # True to allow multiple extensions
                self.ExtendedProject.append(extendedProject)
                self.__ApplyExtended(self.XmlPackageConfiguration, extendedProject.XmlPackageConfiguration, True)
                self.__ApplyExtended(self.XmlRootDirectories, extendedProject.XmlRootDirectories, False)
                self.__ApplyExtended(self.XmlNewProjectTemplatesRootDirectories, extendedProject.XmlNewProjectTemplatesRootDirectories, False)
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


    def __ApplyExtendedExperimental(self, dst: Optional[XmlExperimental], src: Optional[XmlExperimental]) -> None:
        if dst is not None:
            dst.Merge(src)


    def __GetDefaultPackageLanguage(self, xmlElement: ET.Element) -> int:
        defaultPackageLanguage = XmlBase._ReadAttrib(self, xmlElement, 'DefaultPackageLanguage', "C++")
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
