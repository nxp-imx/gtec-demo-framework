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
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.Version import Version
from FslBuildGen.Log import Log
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import XmlException
from FslBuildGen.Xml.Exceptions import XmlFormatException
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.XmlGenFileExternalDependencyPackageManager import XmlGenFileExternalDependencyPackageManager


class XmlGenFileExternalDependency(XmlBase):
    __AttribName = 'Name'
    __AttribDebugName = 'DebugName'
    __AttribTargetName = 'TargetName'
    __AttribInclude = 'Include'
    __AttribLocation = 'Location'
    __AttribHintPath = 'HintPath'
    __AttribVersion = 'Version'
    __AttribPublicKeyToken = 'PublicKeyToken'
    __AttribProcessorArchitecture = 'ProcessorArchitecture'
    __AttribCulture = 'Culture'
    __AttribIf = 'If'
    __AttribAccess = 'Access'
    __AttribType = 'Type'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName, self.__AttribDebugName, self.__AttribTargetName, self.__AttribInclude, self.__AttribLocation, self.__AttribHintPath, self.__AttribVersion, self.__AttribPublicKeyToken, self.__AttribProcessorArchitecture, self.__AttribCulture, self.__AttribIf, self.__AttribAccess, self.__AttribType})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.DebugName = self._ReadAttrib(xmlElement, self.__AttribDebugName, self.Name) # type: str
        defaultTargetName = "{0}::{0}".format(self.Name)
        self.TargetName = self._ReadAttrib(xmlElement, self.__AttribTargetName, defaultTargetName) # type: str
        self.Include = self._TryReadAttrib(xmlElement, self.__AttribInclude)  # type: Optional['str']
        self.Location = self._TryReadAttrib(xmlElement, self.__AttribLocation)  # type: Optional['str']
        # New assembly keywords primarily used for C# assemblies
        self.HintPath = self._TryReadAttrib(xmlElement, self.__AttribHintPath)  # type: Optional['str']
        self.Version = self._TryReadAttribAsVersion(xmlElement, self.__AttribVersion)  # type: Optional[Version]
        self.PublicKeyToken = self._TryReadAttrib(xmlElement, self.__AttribPublicKeyToken)  # type: Optional['str']
        self.ProcessorArchitecture = self._TryReadAttrib(xmlElement, self.__AttribProcessorArchitecture)  # type: Optional['str']
        self.Culture = self._TryReadAttrib(xmlElement, self.__AttribCulture)  # type: Optional['str']
        self.PackageManager = self.__TryGetPackageManager(log, xmlElement)
        self.IfCondition = self._TryReadAttrib(xmlElement, self.__AttribIf)  # type: Optional[str]
        # Can only be set from code, and it indicates that this dependency is managed by a recipe or similar
        self.IsManaged = False # type: bool
        strAccess = self._TryReadAttrib(xmlElement, self.__AttribAccess)  # type: Optional['str']

        access = None
        if self.Include is not None or strAccess is not None:
            strAccess = self._ReadAttrib(xmlElement, self.__AttribAccess) if access is None else access
            if strAccess == "Public":
                access = AccessType.Public
            elif strAccess == "Private":
                access = AccessType.Private
            else:
                raise XmlFormatException("Unknown access type '{0}' on external dependency: '{1}'".format(access, self.Name))

        strElementType = self._ReadAttrib(xmlElement, self.__AttribType)
        elementType = ExternalDependencyType.TryFromString(strElementType)
        if elementType is None:
            raise XmlException(xmlElement, "Unknown external dependency type: '{0}' expected: {1}".format(strElementType,
                                                                                                          ExternalDependencyType.AllStrings()))
        self.Type = elementType  # type: ExternalDependencyType

        # The access type is only relevant for the include file location
        # the rest should always be included
        self.Access = AccessType.Public if access is None else access   # type: AccessType
        self.ConsumedBy = None

        if self.Type == ExternalDependencyType.DLL:
            if not self.Include is None:
                raise XmlException(xmlElement, "DLL dependency: '{0}' can not contain include paths".format(self.Name))
            if self.Access != AccessType.Public:
                raise XmlException(xmlElement, "DLL dependency: '{0}' can only have a access type of Public".format(self.Name))

        if not isinstance(self.Access, AccessType):
            raise Exception("Internal error")



    def __TryGetPackageManager(self, log: Log, xmlElement: ET.Element) -> Optional[XmlGenFileExternalDependencyPackageManager]:
        packageManager = None
        for child in xmlElement:
            if child.tag == 'PackageManager':
                if packageManager is not None:
                    raise Exception("PackageManager has already been defined")
                packageManager = XmlGenFileExternalDependencyPackageManager(self.Log, child)
        return packageManager



class FakeXmlGenFileExternalDependency(XmlGenFileExternalDependency):
    def __init__(self, log: Log, name: str, location: str, access: AccessType, extDepType: ExternalDependencyType,
                 debugName: Optional[str] = None, includeLocation: Optional[str] = None, isManaged: bool = False) -> None:
        strType = ExternalDependencyType.ToString(extDepType)
        fakeXmlElementAttribs = {'Name': name, 'Location': location, 'Access': AccessType.ToString(access), "Type": strType} # type: Dict[str, str]

        if debugName is not None:
            fakeXmlElementAttribs['DebugName'] = debugName
        if includeLocation is not None:
            fakeXmlElementAttribs['Include'] = location

        fakeXmlElement = FakeXmlElementFactory.Create("FakeExternalDep", fakeXmlElementAttribs)
        super().__init__(log, fakeXmlElement)
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
        # Override the value set in the base class
        self.IsManaged = isManaged


class FakeXmlGenFileExternalDependencyHeaders(FakeXmlGenFileExternalDependency):
    def __init__(self, log: Log, name: str, location: str, access: AccessType, isManaged: bool) -> None:
        super().__init__(log, name, location, access, ExternalDependencyType.Headers, None, location, isManaged=isManaged)


class FakeXmlGenFileExternalDependencyStaticLib(FakeXmlGenFileExternalDependency):
    def __init__(self, log: Log, name: str, debugName: str, location: str, access: AccessType, isManaged: bool) -> None:
        super().__init__(log, name, location, access, ExternalDependencyType.StaticLib, debugName, isManaged=isManaged)


class FakeXmlGenFileExternalDependencyDLL(FakeXmlGenFileExternalDependency):
    def __init__(self, log: Log, name: str, debugName: str, location: str, access: AccessType, isManaged: bool) -> None:
        super().__init__(log, name, location, access, ExternalDependencyType.DLL, debugName, isManaged=isManaged)

class FakeXmlGenFileExternalDependencyCMakeFindModern(XmlGenFileExternalDependency):
    def __init__(self, log: Log, name: str, version: Optional[Version], targetName: Optional[str], path: Optional[str], ifCondition: Optional[str]) -> None:
        fakeXmlElementAttribs = {'Name': name, 'Type': ExternalDependencyType.ToString(ExternalDependencyType.CMakeFindModern)}
        if version is not None:
            fakeXmlElementAttribs['Version'] = str(version)
        if targetName is not None:
            fakeXmlElementAttribs['TargetName'] = targetName
        if path is not None:
            fakeXmlElementAttribs['Location'] = path
        if ifCondition is not None:
            fakeXmlElementAttribs['If'] = ifCondition

        fakeXmlElement = FakeXmlElementFactory.Create("FakeExternalDep", fakeXmlElementAttribs)
        super().__init__(log, fakeXmlElement)
