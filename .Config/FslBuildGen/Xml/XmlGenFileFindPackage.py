#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
#    * Neither the name of the NXP. nor the names of
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

from typing import Optional
import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.DataTypes import DependencyCondition
from FslBuildGen.Version import Version
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import XmlFormatException
from FslBuildGen.Xml.XmlBase import XmlBase

class XmlGenFileFindPackage(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.Version = self._TryReadAttribAsVersion(xmlElement, 'Version')  # type: Optional[Version]
        self.TargetName = self._TryReadAttrib(xmlElement, 'TargetName')  # type: Optional[str]
        # A optional path that will be specified to cmake
        self.Path = self._TryReadAttrib(xmlElement, 'Path')  # type: Optional[str]
        self.IfCondition = self._TryReadAttrib(xmlElement, 'If', DependencyCondition.FindPackageAllowed)  # type: Optional[str]
        if self.IfCondition != DependencyCondition.FindPackageAllowed:
            raise XmlFormatException("Unsupported IfCondition '{0}' on FindPackage: '{1}'. Expected {2}".format(self.Version, self.Name, DependencyCondition.FindPackageAllowed))
        if self.Path is not None and IOUtil.IsAbsolutePath(self.Path):
            raise XmlFormatException("Path '{0}' can not be absolute".format(self.Path))


class FakeXmlGenFileFindPackage(XmlGenFileFindPackage):
    def __init__(self, log: Log, name: str, version: Optional[Version], targetName: Optional[str], path: Optional[str], ifCondition: Optional[str]) -> None:
        fakeXmlElementAttribs = {'Name': name}
        if version is not None:
            fakeXmlElementAttribs['Version'] = str(version)
        if targetName is not None:
            fakeXmlElementAttribs['TargetName'] = targetName
        if path is not None:
            fakeXmlElementAttribs['Path'] = path
        if ifCondition is not None:
            fakeXmlElementAttribs['If'] = ifCondition

        fakeXmlElement = FakeXmlElementFactory.Create("FakeXmlGenFileFindPackage", fakeXmlElementAttribs)
        super().__init__(log, fakeXmlElement)
