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
from typing import Optional
import xml.etree.ElementTree as ET
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import XmlException
from FslBuildGen.Xml.Exceptions import XmlFormatException
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.XmlGenFileExternalDependencyPackageManager import XmlGenFileExternalDependencyPackageManager


class XmlGenFileExternalDependency(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlGenFileExternalDependency, self).__init__(basicConfig, xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name')
        self.DebugName = XmlBase._ReadAttrib(self, xmlElement, 'DebugName', self.Name) # type: str
        self.Include = XmlBase._TryReadAttrib(self, xmlElement, 'Include')  # type: Optional['str']
        self.Location = XmlBase._TryReadAttrib(self, xmlElement, 'Location')  # type: Optional['str']
        # New assembly keywords primarily used for C# assemblies
        self.HintPath = XmlBase._TryReadAttrib(self, xmlElement, 'HintPath')  # type: Optional['str']
        self.Version = XmlBase._TryReadAttrib(self, xmlElement, 'Version')  # type: Optional['str']
        self.PublicKeyToken = XmlBase._TryReadAttrib(self, xmlElement, 'PublicKeyToken')  # type: Optional['str']
        self.ProcessorArchitecture = XmlBase._TryReadAttrib(self, xmlElement, 'ProcessorArchitecture')  # type: Optional['str']
        self.Culture = XmlBase._TryReadAttrib(self, xmlElement, 'Culture')  # type: Optional['str']
        self.PackageManager = self.__TryGetPackageManager(basicConfig, xmlElement)
        strAccess = XmlBase._TryReadAttrib(self, xmlElement, 'Access')  # type: Optional['str']

        access = None
        if self.Include != None or strAccess != None:
            strAccess = XmlBase._ReadAttrib(self, xmlElement, 'Access') if access is None else access
            if strAccess == "Public":
                access = AccessType.Public
            elif strAccess == "Private":
                access = AccessType.Private
            else:
                raise XmlFormatException("Unknown access type '{0}' on external dependency: '{1}'".format(access, self.Name))

        strElementType = XmlBase._ReadAttrib(self, xmlElement, 'Type')
        elementType = ExternalDependencyType.TryFromString(strElementType)
        if elementType is None:
            raise XmlException(xmlElement, "Unknown external dependency type: '{0}' expected: StaticLib, DLL, Headers, Assembly, Find".format(type))
        self.Type = elementType  # type: int

        # The access type is only relevant for the include file location
        # the rest should always be included
        self.Access = AccessType.Public if access is None else access   # type: int
        self.ConsumedBy = None

        if self.Type == ExternalDependencyType.DLL:
            if not self.Include is None:
                raise XmlException(xmlElement, "DLL dependency: '{0}' can not contain include paths".format(self.Name))
            if self.Access != AccessType.Public:
                raise XmlException(xmlElement, "DLL dependency: '{0}' can only have a access type of Public".format(self.Name))

        if not isinstance(self.Access, int):
            raise Exception("Internal error")



    def __TryGetPackageManager(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlGenFileExternalDependencyPackageManager]:
        packageManager = None
        for child in xmlElement:
            if child.tag == 'PackageManager':
                if packageManager != None:
                    raise Exception("PackageManager has already been defined")
                packageManager = XmlGenFileExternalDependencyPackageManager(self.BasicConfig, child)
        return packageManager



class FakeXmlGenFileExternalDependency(XmlGenFileExternalDependency):
    def __init__(self, basicConfig: BasicConfig, name: str, location: str, access: int, extDepType: int,
                 debugName: Optional[str] = None, includeLocation: Optional[str] = None) -> None:
        strType = ExternalDependencyType.ToString(extDepType)
        fakeXmlElementAttribs = {'Name': name, 'Location': location, 'Access': AccessType.ToString(access), "Type": strType} # type: Dict[str, str]

        if debugName is not None:
            fakeXmlElementAttribs['DebugName'] = debugName
        if includeLocation is not None:
            fakeXmlElementAttribs['Include'] = location

        fakeXmlElement = FakeXmlElementFactory.Create("FakeExternalDep", fakeXmlElementAttribs)
        super(FakeXmlGenFileExternalDependency, self).__init__(basicConfig, fakeXmlElement)
        if self.Name != name:
            raise Exception("Failed to setting fake element attribute Name")
        if self.Location != location:
            raise Exception("Failed to setting fake element attribute Location")
        if self.Access != access:
            raise Exception("Failed to setting fake element attribute Access")
        if debugName is not None and self.DebugName != debugName:
            raise Exception("Failed to setting fake element attribute DebugName")
        if includeLocation is not None and self.Include != includeLocation:
            raise Exception("Failed to setting fake element attribute IncludeLocation")


class FakeXmlGenFileExternalDependencyHeaders(FakeXmlGenFileExternalDependency):
    def __init__(self, basicConfig: BasicConfig, name: str, location: str, access: int) -> None:
        extDepType = ExternalDependencyType.Headers
        super(FakeXmlGenFileExternalDependencyHeaders, self).__init__(basicConfig, name, location, access, extDepType, None, location)


class FakeXmlGenFileExternalDependencyStaticLib(FakeXmlGenFileExternalDependency):
    def __init__(self, basicConfig: BasicConfig, name: str, debugName: str, location: str, access: int) -> None:
        extDepType = ExternalDependencyType.StaticLib
        super(FakeXmlGenFileExternalDependencyStaticLib, self).__init__(basicConfig, name, location, access, extDepType, debugName)


class FakeXmlGenFileExternalDependencyDLL(FakeXmlGenFileExternalDependency):
    def __init__(self, basicConfig: BasicConfig, name: str, debugName: str, location: str, access: int) -> None:
        extDepType = ExternalDependencyType.DLL
        super(FakeXmlGenFileExternalDependencyDLL, self).__init__(basicConfig, name, location, access, extDepType, debugName)
