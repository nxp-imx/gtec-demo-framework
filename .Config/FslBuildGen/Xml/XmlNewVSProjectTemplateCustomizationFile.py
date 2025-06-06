#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2018 NXP
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

from typing import List
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
#from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Exceptions import FileNotFoundException
from FslBuildGen.Xml.Exceptions import XmlException
from FslBuildGen.Xml.Exceptions import XmlInvalidRootElement
from FslBuildGen.Xml.XmlBase import XmlBase


class XmlNewVSProjectTemplateCustomizationBuildOutput(XmlBase):
    __AttribLocation = 'Location'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribLocation})
        location = self._ReadAttrib(xmlElement, self.__AttribLocation)
        if '\\' in location:
            raise Exception("'\\' is now allowed in location use '/' instead ('{0}')".format(location))
        self.Location = location

class XmlNewVSProjectTemplateCustomizationFile(XmlBase):
    __AttribVersion = 'Version'

    def __init__(self, log: Log, filename: str) -> None:
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate config file %s", filename)

        tree = ET.parse(filename)
        elem = tree.getroot()
        if elem.tag != 'FslBuildGeneratorVSProjectTemplateCustomization':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGeneratorVSProjectTemplateCustomization'")

        super().__init__(log, elem)
        #self._CheckAttributes({self.__AttribVersion})
        strVersion = self._ReadAttrib(elem, self.__AttribVersion)
        if strVersion != "1":
            raise Exception("Unsupported version")

        xmlConfiguration = self.__LoadTemplateConfiguration(log, elem)
        if len(xmlConfiguration) != 1:
            raise XmlException("The file did not contain exactly one BuildOutput element")

        self.Version = 1
        self.BuildOutput = xmlConfiguration[0]
        self.Path = IOUtil.GetDirectoryName(filename)

    def __LoadTemplateConfiguration(self, log: Log, element: ET.Element) -> List[XmlNewVSProjectTemplateCustomizationBuildOutput]:
        res = []
        foundElements = element.findall("BuildOutput")
        for foundElement in foundElements:
            res.append(XmlNewVSProjectTemplateCustomizationBuildOutput(log, foundElement))
        return res
