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

from typing import Optional
from typing import List
import xml.etree.ElementTree as ET
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.DataTypes import ScanMethod
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.XmlBase import XmlBase



class XmlConfigPackageLocationBlacklist(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlConfigPackageLocationBlacklist, self).__init__(basicConfig, xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name')


class XmlConfigPackageLocation(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super(XmlConfigPackageLocation, self).__init__(basicConfig, xmlElement)
        defaultScanMethod = ScanMethod.Directory
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name') # type: str
        self.Blacklist = self.__LoadBlacklist(xmlElement)
        self.ScanMethod = ScanMethod.FromString(XmlBase._ReadAttrib(self, xmlElement, 'ScanMethod', ScanMethod.ToString(defaultScanMethod)))
        self.Id = self.Name.lower() if self.Name is not None else None
        self.ResolvedActualPath = None


    def __LoadBlacklist(self, xmlElement: ET.Element) -> List[XmlConfigPackageLocationBlacklist]:
        res = []
        foundElements = xmlElement.findall("Blacklist")
        for foundElement in foundElements:
            res.append(XmlConfigPackageLocationBlacklist(self.BasicConfig, foundElement))
        return res


class FakeXmlConfigPackageLocation(XmlConfigPackageLocation):
    def __init__(self, basicConfig: BasicConfig, name: str, scanMethod: Optional[int] = None, blacklist: Optional[List[str]] = None) -> None:
        xmlAttribs = {'Name': name}
        if scanMethod is not None:
            xmlAttribs['ScanMethod'] = ScanMethod.ToString(scanMethod)
        xmlElement = FakeXmlElementFactory.Create("PackageLocation", xmlAttribs)
        if blacklist is not None:
            for blacklistEntry in blacklist:
                xmlBlacklistElement = FakeXmlElementFactory.CreateWithName("Blacklist", blacklistEntry)
                xmlElement.append(xmlBlacklistElement)

        super(FakeXmlConfigPackageLocation, self).__init__(basicConfig, xmlElement)

