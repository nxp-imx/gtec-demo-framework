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

import copy
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import *
from FslBuildGen import IOUtil, Util, PackageConfig

class XmlConfigFileGenFile(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigFileGenFile, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')

class XmlConfigFileTemplateFolder(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigFileTemplateFolder, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')


class XmlConfigFileAddRootDirectory(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigFileAddRootDirectory, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')


class XmlConfigFileAddTemplateImportDirectory(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigFileAddTemplateImportDirectory, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')


class XmlConfigPackageLocationBlacklist(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigPackageLocationBlacklist, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')


class XmlConfigPackageLocation(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigPackageLocation, self).__init__(xmlElement)
        if xmlElement != None:
            self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
            self.Blacklist = self.__LoadBlacklist(xmlElement)
        else:
            self.Name = None
            self.Blacklist = []
        self.ResolvedActualPath = None

    def __LoadBlacklist(self, element):
        res = []
        foundElements = element.findall("Blacklist")
        for foundElement in foundElements:
            res.append(XmlConfigPackageLocationBlacklist(foundElement))
        return res


class XmlConfigPackageConfiguration(XmlBase):
    def __init__(self, xmlElement, sourceFile):
        super(XmlConfigPackageConfiguration, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.Locations = self.__LoadLocations(xmlElement)
        self.SourceFile = sourceFile

    def __LoadLocations(self, element):
        res = []
        foundElements = element.findall("PackageLocation")
        for foundElement in foundElements:
            res.append(XmlConfigPackageLocation(foundElement))
        return res


class XmlConfigContentBuilderAddExtension(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigContentBuilderAddExtension, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.Description = XmlBase._ReadAttrib(self, xmlElement, 'Description', True, '')
        self.PostfixedOutputExtension = XmlBase._ReadAttrib(self, xmlElement, 'PostfixedOutputExtension', False, '')


class XmlConfigContentBuilder(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigContentBuilder, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.Executable = XmlBase._ReadAttrib(self, xmlElement, 'Executable', True, '')
        self.Parameters = XmlBase._ReadAttrib(self, xmlElement, 'Parameters', True, '')
        self.FeatureRequirements = XmlBase._ReadAttrib(self, xmlElement, 'FeatureRequirements', False, '')
        self.DefaultExtensions = self.__LoadDefaultExtensions(xmlElement)
        self.Description = XmlBase._ReadAttrib(self, xmlElement, 'Description', True, '')

    def __LoadDefaultExtensions(self, element):
        res = []
        foundElements = element.findall("AddExtension")
        for foundElement in foundElements:
            res.append(XmlConfigContentBuilderAddExtension(foundElement))
        return res


class XmlConfigContentBuilderConfiguration(XmlBase):
    def __init__(self, xmlElement):
        super(XmlConfigContentBuilderConfiguration, self).__init__(xmlElement)
        self.ContentBuilders = self.__LoadContentBuilders(xmlElement)

    def __LoadContentBuilders(self, element):
        res = []
        foundElements = element.findall("ContentBuilder")
        for foundElement in foundElements:
            res.append(XmlConfigContentBuilder(foundElement))
        return res


def XMLLoadPackageConfiguration(element, sourceFile):
    res = []
    foundElements = element.findall("PackageConfiguration")
    for foundElement in foundElements:
        res.append(XmlConfigPackageConfiguration(foundElement, sourceFile))
    return res


class XmlToolConfigFile(XmlBase):
    def __init__(self, filename, projectRootConfig):
        if projectRootConfig == None:
            raise Exception("projectRootConfig can not be None")
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate config file %s", filename)

        tree = ET.parse(filename)
        elem = tree.getroot()
        if elem.tag != 'FslBuildGenConfig':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGenConfig'")

        super(XmlToolConfigFile, self).__init__(elem)
        self.Version = XmlBase._ReadAttrib(self, elem, 'Version', True, '1')

        rootDirs = self.__LoadAddRootDirectory(elem)
        if len(rootDirs) < 1:
            raise XmlException("The file did not contain at least one AddRootDirectory element")

        templateImportDirectory = self.__LoadAddTemplateImportDirectory(elem)

        # Load the package configurations and merge them with the ones found in the ProjectRootConfig
        xmlPackageConfigurations = XMLLoadPackageConfiguration(elem, filename)
        xmlPackageConfigurations += projectRootConfig.XmlPackageConfiguration
        if len(xmlPackageConfigurations) < 1:
            if projectRootConfig.SourceFileName == None:
                raise XmlException("The file '{0}' did not contain at least one PackageConfiguration element".format(filename))
            else:
                raise XmlException("The file '{0}' and {1} did not contain at least one PackageConfiguration element".format(filename, projectRootConfig.SourceFileName))

        xmlContentBuilderConfiguration = self.__LoadContentBuilderConfiguration(elem)

        xmlConfigFileTemplateFolder = self.__LoadTemplateFolder(elem)

        self.Version = 1
        self.RootDirectories = rootDirs
        self.TemplateImportDirectories = templateImportDirectory
        self.PackageConfiguration = self.__ResolvePackageConfiguration(xmlPackageConfigurations)
        self.TemplateFolder = xmlConfigFileTemplateFolder
        self.GenFileName = self.__LoadGenFileName(elem)
        self.ContentBuilderConfiguration = xmlContentBuilderConfiguration


    def __ResolvePackageConfiguration(self, xmlPackageConfigurations):
        # prepare the package configurations
        packageConfigurationDict = {}
        for entry in xmlPackageConfigurations:
            if entry.Name in packageConfigurationDict:
                firstEntry = packageConfigurationDict[entry.Name]
                raise XmlException2(entry.XMLElement, "Duplicated package configuration name '{0}' found in '{1}' and '{2}'".format(entry.Name, entry.SourceFile, firstEntry.SourceFile))
            packageConfigurationDict[entry.Name] = entry

        if not 'default' in packageConfigurationDict:
            raise XmlException("The file did not contain the 'default' PackageConfiguration element")
        return packageConfigurationDict



    def __LoadTemplateFolder(self, element):
        foundElement = element.find("TemplateFolder")
        if foundElement == None:
            raise XmlException2(element, "Could not locate the TemplateFolder element")
        return XmlConfigFileTemplateFolder(foundElement)


    def __LoadGenFileName(self, element):
        foundElement = element.find("GenFile")
        if foundElement == None:
            raise XmlException2(element, "Could not locate the GenFile element")
        return XmlConfigFileGenFile(foundElement)

    def __LoadAddRootDirectory(self, element):
        res = []
        foundElements = element.findall("AddRootDirectory")
        for foundElement in foundElements:
            res.append(XmlConfigFileAddRootDirectory(foundElement))
        return res


    def __LoadAddTemplateImportDirectory(self, element):
        res = []
        foundElements = element.findall("AddTemplateImportDirectory")
        for foundElement in foundElements:
            res.append(XmlConfigFileAddTemplateImportDirectory(foundElement))
        return res



    def __LoadContentBuilderConfiguration(self, element):
        foundElements = element.findall("ContentBuilderConfiguration")
        if len(foundElements) > 1:
            raise XmlException("The file contained more than one ContentBuilderConfiguration")
        elif len(foundElements) == 1:
            return XmlConfigContentBuilderConfiguration(foundElements[0])
        return None



