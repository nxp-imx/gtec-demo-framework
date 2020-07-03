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

from typing import List
import xml.etree.ElementTree as ET
from FslBuildGen import Util
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Log import Log
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import XmlUnsupportedPackageNameException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedSubPackageNameException
from FslBuildGen.Xml.SubPackageSupportConfig import SubPackageSupportConfig
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.XmlGenFileDefine import XmlGenFileDefine
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency
from FslBuildGen.Xml.XmlGenFileExternalDependency import FakeXmlGenFileExternalDependencyCMakeFindModern
from FslBuildGen.Xml.XmlGenFileFindPackage import XmlGenFileFindPackage


class FakeXmlGenFileDependency(XmlGenFileDependency):
    def __init__(self, log: Log, name: str, access: int) -> None:
        fakeXmlElementAttribs = {'Name': name, 'Access': AccessType.ToString(access)}
        fakeXmlElement = FakeXmlElementFactory.Create("FakeXmlGenFileDependency", fakeXmlElementAttribs)
        super().__init__(log, fakeXmlElement)
        if self.Name != name:
            raise Exception("Failed to setting fake element name")
        if self.Access != access:
            raise Exception("Failed to setting fake element access")


class XmlBase2(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element, subPackageSupport: SubPackageSupportConfig) -> None:
        if not isinstance(subPackageSupport, SubPackageSupportConfig):
            raise UsageErrorException("The support object was not of the correct type")
        super().__init__(log, xmlElement)
        self.SystemSubPackageSupport = subPackageSupport  # type: SubPackageSupportConfig
        self.ExternalDependencies = self.__GetXMLExternalDependencies(xmlElement)
        self.DirectDefines = self.__GetXMLDefines(xmlElement)
        self.DirectDependencies = self._GetXMLDependencies(xmlElement)


    def BaseLoad(self, xmlElement: ET.Element, subPackageSupport: SubPackageSupportConfig) -> None:
        self.SystemSubPackageSupport = subPackageSupport
        self.ExternalDependencies = self.__GetXMLExternalDependencies(xmlElement)
        self.DirectDefines = self.__GetXMLDefines(xmlElement)
        self.DirectDependencies = self._GetXMLDependencies(xmlElement)


    def GetSubPackageSupport(self) -> SubPackageSupportConfig:
        return self.SystemSubPackageSupport


    def __GetXMLExternalDependencies(self, xmlElement: ET.Element) -> List[XmlGenFileExternalDependency]:
        dependencies = []
        if xmlElement != None:
            for child in xmlElement:
                if child.tag == 'ExternalDependency':
                    dependencies.append(XmlGenFileExternalDependency(self.Log, child))

        # find package is just a alias for a specific ExternalDependency type
        findPackageDependencies = self.__GetXMLFindPackageDependencies(xmlElement)
        for findPackage in findPackageDependencies:
            dependencies.append(self.ConvertToXmlGenFileExternalDependency(findPackage))
        return dependencies

    def __GetXMLFindPackageDependencies(self, xmlElement: ET.Element) -> List[XmlGenFileFindPackage]:
        dependencies = []
        if xmlElement != None:
            for child in xmlElement:
                if child.tag == 'FindPackage':
                    dependencies.append(XmlGenFileFindPackage(self.Log, child))
        return dependencies

    def ConvertToXmlGenFileExternalDependency(self, value: XmlGenFileFindPackage) -> XmlGenFileExternalDependency:
        return FakeXmlGenFileExternalDependencyCMakeFindModern(self.Log, value.Name, value.Version, value.TargetName, value.Path, value.IfCondition)


    def __GetXMLDefines(self, xmlElement: ET.Element) -> List[XmlGenFileDefine]:
        dependencies = []
        if xmlElement != None:
            for child in xmlElement:
                if child.tag == 'Define':
                    dependencies.append(XmlGenFileDefine(self.Log, child))
                elif child.tag == 'CPPDefine':
                    # todo log warning here
                    dependencies.append(XmlGenFileDefine(self.Log, child))
        return dependencies


    def _ValidateName(self, xmlElement: ET.Element, name: str) -> None:
        allowSubPackages = self.SystemSubPackageSupport.AllowSubPackages    # type: bool
        if not Util.IsValidPackageName(name, allowSubPackages):
            if allowSubPackages and name.find('..') >= 0:
                raise XmlUnsupportedSubPackageNameException(xmlElement, name)
            else:
                raise XmlUnsupportedPackageNameException(xmlElement, name)


    def _GetXMLDependencies(self, xmlElement: ET.Element) -> List[XmlGenFileDependency]:
        elements = []
        if xmlElement is not None:
            for child in xmlElement:
                if child.tag == 'Dependency':
                    xmlDep = XmlGenFileDependency(self.Log, child)
                    self._ValidateName(xmlDep.XMLElement, xmlDep.Name)
                    elements.append(xmlDep)
        return elements

    def _CreateFakeXMLDependencies(self, dependencyName: str, access: int = AccessType.Public) -> FakeXmlGenFileDependency:
        return FakeXmlGenFileDependency(self.Log, dependencyName, access)
