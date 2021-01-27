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
from FslBuildGen import Util
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.Log import Log
from FslBuildGen.Xml.Exceptions import XmlFormatException
from FslBuildGen.Xml.XmlBase import XmlBase


class XmlGenFileDependency(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')  # type: str
        flavor = self._TryReadAttrib(xmlElement, 'Flavor')  # type: Optional[str]
        self.Flavor = self.__TryParseFlavor(flavor)
        access = self._ReadAttrib(xmlElement, 'Access', 'Public')  # type: str
        self.IfCondition = self._TryReadAttrib(xmlElement, 'If')  # type: Optional[str]

        if access == "Public":
            self.Access = AccessType.Public  # type: AccessType
        elif access == "Private":
            self.Access = AccessType.Private
        elif access == "Link":
            self.Access = AccessType.Link
        else:
            raise XmlFormatException("Unknown access type '{0}' on Dependency: '{1}'".format(access, self.Name))

    def __TryParseFlavor(self, flavor: Optional[str]) -> Dict[str, str]:
        if flavor is None or len(flavor) <= 0:
            return {}
        uniqueIds = {} # type: Dict[str, str]
        resDict = {} # type: Dict[str, str]
        entries = flavor.split(',')
        for entry in entries:
            parts = entry.split('=')
            if len(parts) != 2:
                raise XmlFormatException("Dependency flavor constraint '{0}' not in the expected format 'flavor1=option, flavor2=option'".format(flavor))
            key = parts[0].strip()
            value = parts[1].strip()
            if key in resDict:
                raise XmlFormatException("Dependency flavor constraint key '{0}' already defined to '{1}'".format(key, resDict[key]))
            keyId = key.upper()
            if keyId in uniqueIds:
                raise XmlFormatException("Dependency flavor constraint key '{0}' already collides with '{1}'".format(key, uniqueIds[keyId]))

            if not Util.IsValidFlavorName(key):
                raise XmlFormatException("Dependency flavor name '{0}' is invalid".format(key))

            if not Util.IsValidFlavorOptionName(value):
                raise XmlFormatException("Dependency flavor option '{1}' is invalid in {0}={1}".format(key, value))


            uniqueIds[keyId] = key
            resDict[key] = value
        return resDict
