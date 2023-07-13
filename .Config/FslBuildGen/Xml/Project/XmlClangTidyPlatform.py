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

#from typing import List
from typing import Optional
import xml.etree.ElementTree as ET
from FslBuildGen.Log import Log
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.Project.XmlClangTidyPlatformCompiler import XmlClangTidyPlatformCompiler
from FslBuildGen.Xml.Project.XmlClangTidyPlatformDefines import XmlClangTidyPlatformDefines
from FslBuildGen.Xml.Project.XmlClangTidyPlatformStrictChecks import XmlClangTidyPlatformStrictChecks

class XmlClangTidyPlatform(XmlBase):
    __AttribName = 'Name'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.Compiler = self.__TryReadCompiler(log, xmlElement)  # Optional[type: XmlClangTidyPlatformCompiler]
        self.Defines = self.__TryReadDefines(log, xmlElement)    # Optional[type: XmlClangTidyPlatformDefines]
        self.StrictChecks = self.__TryReadStrictChecks(log, xmlElement)    # Optional[type: XmlClangTidyPlatformStrictChecks]

    def __TryReadCompiler(self, log: Log, xmlElement: ET.Element) -> Optional[XmlClangTidyPlatformCompiler]:
        foundElements = xmlElement.findall("Compiler")
        if len(foundElements) > 1:
            raise XmlException2("There can only be one 'Compiler' element in a ClangTidyConfiguration.Platform")
        if len(foundElements) <= 0:
            return None
        return XmlClangTidyPlatformCompiler(log, foundElements[0])

    def __TryReadDefines(self, log: Log, xmlElement: ET.Element) -> Optional[XmlClangTidyPlatformDefines]:
        foundElements = xmlElement.findall("Defines")
        if len(foundElements) > 1:
            raise XmlException2("There can only be one 'Defines' element in a ClangTidyConfiguration.Platform")
        if len(foundElements) <= 0:
            return None
        return XmlClangTidyPlatformDefines(log, foundElements[0])

    def __TryReadStrictChecks(self, log: Log, xmlElement: ET.Element) -> Optional[XmlClangTidyPlatformStrictChecks]:
        foundElements = xmlElement.findall("StrictChecks")
        if len(foundElements) > 1:
            raise XmlException2("There can only be one 'StrictChecks' element in a ClangTidyConfiguration.Platform")
        if len(foundElements) <= 0:
            return None
        return XmlClangTidyPlatformStrictChecks(log, foundElements[0])
