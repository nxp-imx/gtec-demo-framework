#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2023 NXP
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

from typing import Dict
from typing import List
import xml.etree.ElementTree as ET
from FslBuildGen import Util
from FslBuildGen.Log import Log
from FslBuildGen.Xml.Exceptions import XmlFlavorOptionNameCollisionException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedFlavorNameException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedFlavorOptionNameException
from FslBuildGen.Xml.Flavor.XmlGenFileFlavorOption import XmlGenFileFlavorOption
from FslBuildGen.Xml.XmlBase import XmlBase


class XmlGenFileFlavorExtension(XmlBase):
    __AttribName = 'Name'

    def __init__(self, log: Log, requirementTypes: List[str], xmlElement: ET.Element, ownerPackageName: str) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.IntroducedByPackageName = ownerPackageName
        #elementType = self._ReadAttrib(xmlElement, 'Type', 'Normal')
        self.Options = self.__GetXMLFlavorOptions(requirementTypes, xmlElement, ownerPackageName)
        self.OptionDict = self.__BuildOptionDict()  # type: Dict[str, XmlGenFileFlavorOption]
        self.__ValidateFlavorName()
        self.__ValidateOptionNames()


    def __ValidateFlavorName(self) -> None:
        if not Util.IsValidFlavorExtensionName(self.Name):
            raise XmlUnsupportedFlavorNameException(self.XMLElement, self.Name)

    def __ValidateOptionNames(self) -> None:
        for option in self.Options:
            if not Util.IsValidName(option.Name):
                raise XmlUnsupportedFlavorOptionNameException(option.XMLElement, option.Name)

    def __BuildOptionDict(self) -> Dict[str, XmlGenFileFlavorOption]:
        optionDict = {} # type: Dict[str, XmlGenFileFlavorOption]
        optionNameSet = {}  # type: Dict[str, str]
        for option in self.Options:
            optionDict[option.Name] = option
            key = option.Name.lower()
            if not key in optionNameSet:
                optionNameSet[key] = option.Name
            else:
                raise XmlFlavorOptionNameCollisionException(self.XMLElement, optionNameSet[key], option.Name)
        return optionDict


    def __GetXMLFlavorOptions(self, requirementTypes: List[str], elem: ET.Element, ownerPackageName: str) -> List[XmlGenFileFlavorOption]:
        options = []
        if elem != None:
            for child in elem:
                if child.tag == 'Option':
                    options.append(XmlGenFileFlavorOption(self.Log, requirementTypes, child, ownerPackageName))
        options.sort(key=lambda s: s.Name.lower())
        return options
