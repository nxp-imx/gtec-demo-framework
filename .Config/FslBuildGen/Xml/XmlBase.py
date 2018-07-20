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
import xml.etree.ElementTree as ET
from FslBuildGen.DataTypes import BoolStringHelper
from FslBuildGen.Log import Log
from FslBuildGen.Xml.XmlBaseInfo import XmlBaseInfo
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.Exceptions import XmlFormatException
from FslBuildGen.Xml.Exceptions import XmlRequiredAttributeMissingException

class XmlBase(XmlBaseInfo):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super(XmlBase, self).__init__(log, xmlElement)


    def _GetElement(self, xmlElement: ET.Element, elementName: str) -> ET.Element:
        foundElement = xmlElement.find(elementName)
        if foundElement is None:
            raise XmlException2(xmlElement, "Could not locate the expected {0} element".format(elementName))
        return foundElement


    def _TryGetElement(self, xmlElement: ET.Element, elementName: str) -> Optional[ET.Element]:
        return xmlElement.find(elementName)

    #def _LogAttributes(self, validAttributeNames):
    #    """ Raise a exception if there is any attributes isn't on the list """
    #    for entry in xmlElement.attrib.keys():
    #        if not entry in validAttributeNames:
    #            self.Log.LogPrint("Unexpected attribute '{0}' found, valid attributes={0}".format(validAttributeNames))

    def _HasAttrib(self, xmlElement: ET.Element, attribName: str) -> bool:
        return attribName in xmlElement.attrib


    def _TryReadAttrib(self, xmlElement: ET.Element, attribName: str, defaultValue: Optional[str] = None) -> Optional[str]:
        """ Read the attrib if its available, else return defaultValue """
        if attribName in xmlElement.attrib:
            return xmlElement.attrib[attribName]
        return defaultValue


    def _ReadAttrib(self, xmlElement: ET.Element, attribName: str, defaultValue: Optional[str] = None) -> str:
        """ If the attrib is there we return it
            if its not there and defaultValue is not None we return the default value.
            if its not there and defaultValue is None we throw a exception.
        """
        value = self._TryReadAttrib(xmlElement, attribName, defaultValue)
        if value is not None:
            return value
        elif defaultValue is not None:
            return defaultValue
        raise XmlRequiredAttributeMissingException(xmlElement, attribName)


    def _TryReadBoolAttrib(self, xmlElement: ET.Element, attribName: str, defaultValue: Optional[bool]) -> Optional[bool]:
        value = self._TryReadAttrib(xmlElement, attribName, None)
        if value is None:
            return defaultValue
        return BoolStringHelper.TryFromString(value)


    def _ReadBoolAttrib(self, xmlElement: ET.Element, attribName: str, defaultValue: Optional[bool] = None) -> bool:
        """ If the attrib is there we return it
            if its not there and defaultValue is not None we return the default value.
            if its not there and defaultValue is None we throw a exception.
        """
        strValue = self._TryReadAttrib(xmlElement, attribName, None)
        if strValue is not None:
            return BoolStringHelper.FromString(strValue)
        elif defaultValue is not None:
            return defaultValue
        raise XmlFormatException("{0} expects a value of either 'true' or 'false' not '{1}'".format(attribName, strValue))
