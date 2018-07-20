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
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen.Exceptions import FileNotFoundException
from FslBuildGen.Log import Log
from FslBuildGen.Xml.Exceptions import XmlException
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.Exceptions import XmlInvalidRootElement
from FslBuildGen.Xml.ToolConfig import LoadUtil
from FslBuildGen.Xml.ToolConfig.XmlConfigFileAddNewProjectTemplatesRootDirectory import XmlConfigFileAddNewProjectTemplatesRootDirectory
from FslBuildGen.Xml.ToolConfig.XmlConfigPackageConfiguration import XmlConfigPackageConfiguration
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.Project.XmlBuildDocConfiguration import XmlBuildDocConfiguration
from FslBuildGen.Xml.Project.XmlClangTidyConfiguration import XmlClangTidyConfiguration
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlClangFormatConfiguration
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlConfigCompilerConfiguration
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlConfigFileAddRootDirectory
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlExperimental
from FslBuildGen.Xml.Project.XmlProjectRootConfigFile import XmlProjectRootConfigFile


class XmlConfigFileGenFile(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')


class XmlConfigFileTemplateFolder(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')



class XmlConfigFileAddTemplateImportDirectory(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')


class XmlConfigContentBuilderAddExtension(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.Description = self._ReadAttrib(xmlElement, 'Description')
        self.PostfixedOutputExtension = self._ReadAttrib(xmlElement, 'PostfixedOutputExtension', '')


class XmlConfigContentBuilder(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')  # type: str
        self.Executable = self._ReadAttrib(xmlElement, 'Executable')  # type: str
        self.Parameters = self._ReadAttrib(xmlElement, 'Parameters')  # type: str
        self.FeatureRequirements = self._ReadAttrib(xmlElement, 'FeatureRequirements', '')  # type: str
        self.DefaultExtensions = self.__LoadDefaultExtensions(log, xmlElement)  # type: List[XmlConfigContentBuilderAddExtension]
        self.Description = self._ReadAttrib(xmlElement, 'Description')  # type: str


    def __LoadDefaultExtensions(self, log: Log, xmlElement: ET.Element) -> List[XmlConfigContentBuilderAddExtension]:
        res = []
        foundElements = xmlElement.findall("AddExtension")
        for foundElement in foundElements:
            res.append(XmlConfigContentBuilderAddExtension(log, foundElement))
        return res


class XmlConfigContentBuilderConfiguration(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.ContentBuilders = self.__LoadContentBuilders(log, xmlElement)  # type: List[XmlConfigContentBuilder]


    def __LoadContentBuilders(self, log: Log, xmlElement: ET.Element) -> List[XmlConfigContentBuilder]:
        res = []
        foundElements = xmlElement.findall("ContentBuilder")
        for foundElement in foundElements:
            res.append(XmlConfigContentBuilder(log, foundElement))
        return res


class FakeXmlConfigContentBuilderConfiguration(XmlConfigContentBuilderConfiguration):
    def __init__(self, log: Log) -> None:
        xmlElement = FakeXmlElementFactory.Create("Config")
        super().__init__(log, xmlElement)


class XmlToolConfigFile(XmlBase):
    def __init__(self, log: Log, filename: str, projectRootConfig: XmlProjectRootConfigFile) -> None:
        if projectRootConfig is None:
            raise Exception("projectRootConfig can not be None")
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate config file %s", filename)

        tree = ET.parse(filename)
        elem = tree.getroot()
        if elem.tag != 'FslBuildGenConfig':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGenConfig'")

        super().__init__(log, elem)
        currentVersion = '2'
        fileVersion = self._ReadAttrib(elem, 'Version')
        if fileVersion != currentVersion:
            raise XmlException("The file was not of the expected version {0}".format(currentVersion))

        # In V2 we do not support local AddRootDirectory elements, we use the ones in ProjectRootConfig
        rootDirs = projectRootConfig.XmlRootDirectories
        if len(rootDirs) < 1:
            raise XmlException("The file did not contain at least one AddRootDirectory element")

        templateImportDirectory = self.__LoadAddTemplateImportDirectory(elem)

        self.__CheckForLegacyElements(elem, filename)

        # In V2 we do not support local PackageConfiguration elements, we use the ones in ProjectRootConfig
        xmlPackageConfigurations = projectRootConfig.XmlPackageConfiguration  # type: List[XmlConfigPackageConfiguration]
        if len(xmlPackageConfigurations) < 1:
            if projectRootConfig.SourceFileName is None:
                raise XmlException("The file '{0}' did not contain at least one PackageConfiguration element".format(filename))
            else:
                raise XmlException("The file '{0}' and {1} did not contain at least one PackageConfiguration element".format(filename, projectRootConfig.SourceFileName))

        newProjectTemplatesRootDirectories = LoadUtil.LoadAddNewProjectTemplatesRootDirectory(log, elem, filename)
        newProjectTemplatesRootDirectories = self.__MergeNewProjectTemplatesRootDirectories(newProjectTemplatesRootDirectories, projectRootConfig.XmlNewProjectTemplatesRootDirectories)

        xmlContentBuilderConfiguration = self.__LoadContentBuilderConfiguration(elem)

        xmlConfigFileTemplateFolder = self.__LoadTemplateFolder(elem)

        self.Version = int(fileVersion)  # type: int
        self.RootDirectories = rootDirs  # type: List[XmlConfigFileAddRootDirectory]
        self.TemplateImportDirectories = templateImportDirectory  # type: List[XmlConfigFileAddTemplateImportDirectory]
        self.PackageConfiguration = self.__ResolvePackageConfiguration(xmlPackageConfigurations)  # type: Dict[str, XmlConfigPackageConfiguration]
        self.NewProjectTemplateRootDirectories = newProjectTemplatesRootDirectories  # type: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]
        self.TemplateFolder = xmlConfigFileTemplateFolder  # type: XmlConfigFileTemplateFolder
        self.GenFileName = self.__LoadGenFileName(elem)  # type: XmlConfigFileGenFile
        self.ContentBuilderConfiguration = xmlContentBuilderConfiguration  # type: XmlConfigContentBuilderConfiguration
        self.BuildDocConfiguration = projectRootConfig.XmlBuildDocConfiguration # type: List[XmlBuildDocConfiguration]
        self.ClangFormatConfiguration = projectRootConfig.XmlClangFormatConfiguration  # type: List[XmlClangFormatConfiguration]
        self.ClangTidyConfiguration = projectRootConfig.XmlClangTidyConfiguration  # type: List[XmlClangTidyConfiguration]
        self.CompilerConfiguration = projectRootConfig.XmlCompilerConfiguration  # type: List[XmlConfigCompilerConfiguration]
        self.Experimental = self.__ResolveExperimental(projectRootConfig.XmlExperimental)  # type: Optional[XmlExperimental]



    def __MergeNewProjectTemplatesRootDirectories(self,
                                                  newProjectTemplatesRootDirectories1: List[XmlConfigFileAddNewProjectTemplatesRootDirectory],
                                                  newProjectTemplatesRootDirectories2: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]) -> List[XmlConfigFileAddNewProjectTemplatesRootDirectory]:
        uniqueDict = {} # type: Dict[str, XmlConfigFileAddNewProjectTemplatesRootDirectory]
        result = [] # type: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]
        self.__AddNewProjectTemplatesRootDirectories(result, uniqueDict, newProjectTemplatesRootDirectories1) # type: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]
        self.__AddNewProjectTemplatesRootDirectories(result, uniqueDict, newProjectTemplatesRootDirectories2) # type: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]
        return result


    def __AddNewProjectTemplatesRootDirectories(self,
                                                rDst: List[XmlConfigFileAddNewProjectTemplatesRootDirectory],
                                                rDstUniqueDict: Dict[str, XmlConfigFileAddNewProjectTemplatesRootDirectory],
                                                src: List[XmlConfigFileAddNewProjectTemplatesRootDirectory]) -> None:
        for entry in src:
            if entry.Id in rDstUniqueDict:
                raise Exception("Duplicated template root with the id {0} at {1} and {2}".format(entry.Id, rDstUniqueDict[entry.Id].SourceFileName, entry.SourceFileName))
            rDstUniqueDict[entry.Id] = entry
            rDst.append(entry)


    def __CheckForLegacyElements(self, xmlElement: ET.Element, filename: str) -> None:
        self.__CheckForLegacyElement(xmlElement, filename, "PackageConfiguration")
        self.__CheckForLegacyElement(xmlElement, filename, "AddRootDirectory")


    def __CheckForLegacyElement(self, xmlElement: ET.Element, filename: str, name: str) -> None:
        found = xmlElement.find(name)
        if found is not None:
            raise XmlException("The file '{0}' contained a legacy {1} element which is not supported anymore. Use the 'Project.gen' file instead".format(filename, name))


    def __ResolvePackageConfiguration(self, xmlPackageConfigurations: List[XmlConfigPackageConfiguration]) -> Dict[str, XmlConfigPackageConfiguration]:
        # prepare the package configurations
        packageConfigurationDict = {} # type: Dict[str, XmlConfigPackageConfiguration]
        for entry in xmlPackageConfigurations:
            if entry.Name in packageConfigurationDict:
                firstEntry = packageConfigurationDict[entry.Name]
                raise XmlException2(entry.XMLElement, "Duplicated package configuration name '{0}' found in '{1}' and '{2}'".format(entry.Name, entry.SourceFile, firstEntry.SourceFile))
            packageConfigurationDict[entry.Name] = entry

        if 'default' not in packageConfigurationDict:
            raise XmlException("The file did not contain the 'default' PackageConfiguration element")
        return packageConfigurationDict


    def __ResolveExperimental(self, xmlExperimental: Optional[XmlExperimental]) -> Optional[XmlExperimental]:
        return xmlExperimental


    def __LoadTemplateFolder(self, xmlElement: ET.Element) -> XmlConfigFileTemplateFolder:
        foundElement = xmlElement.find("TemplateFolder")
        if foundElement is None:
            raise XmlException2(xmlElement, "Could not locate the TemplateFolder element")
        return XmlConfigFileTemplateFolder(self.Log, foundElement)


    def __LoadGenFileName(self, xmlElement: ET.Element) -> XmlConfigFileGenFile:
        foundElement = xmlElement.find("GenFile")
        if foundElement is None:
            raise XmlException2(xmlElement, "Could not locate the GenFile element")
        return XmlConfigFileGenFile(self.Log, foundElement)


    def __LoadAddTemplateImportDirectory(self, xmlElement: ET.Element) -> List[XmlConfigFileAddTemplateImportDirectory]:
        res = []
        foundElements = xmlElement.findall("AddTemplateImportDirectory")
        for foundElement in foundElements:
            res.append(XmlConfigFileAddTemplateImportDirectory(self.Log, foundElement))
        return res


    def __LoadContentBuilderConfiguration(self, xmlElement: ET.Element) -> XmlConfigContentBuilderConfiguration:
        foundElements = xmlElement.findall("ContentBuilderConfiguration")
        if len(foundElements) > 1:
            raise XmlException("The file contained more than one ContentBuilderConfiguration")
        elif len(foundElements) == 1:
            return XmlConfigContentBuilderConfiguration(self.Log, foundElements[0])
        return FakeXmlConfigContentBuilderConfiguration(self.Log)

