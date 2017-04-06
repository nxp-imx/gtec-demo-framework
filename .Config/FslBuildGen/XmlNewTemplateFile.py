#!/usr/bin/env python

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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

import copy
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import *
from FslBuildGen import IOUtil, Util, PackageConfig

class XmlNewTemplate(XmlBase):
    def __init__(self, xmlElement):
        super(XmlNewTemplate, self).__init__(xmlElement)
        self.NoInclude = XmlBase._ReadBoolAttrib(self, xmlElement, 'NoInclude', False, 'false')
        self.Force = XmlBase._ReadBoolAttrib(self, xmlElement, 'Force', False, 'false')
        self.Warning = XmlBase._ReadAttrib(self, xmlElement, 'Warning', False, '')


class XmlNewTemplateFile(XmlBase):
    def __init__(self, filename):
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate config file %s", filename)

        tree = ET.parse(filename)
        elem = tree.getroot()
        if elem.tag != 'FslBuildNewTemplate':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGenConfig'")

        super(XmlNewTemplateFile, self).__init__(elem)
        self.Version = XmlBase._ReadAttrib(self, elem, 'Version', True, '1')

        xmlTemplate = self.__LoadTemplateConfiguration(elem)
        if len(xmlTemplate) != 1:
            raise XmlException("The file did not contain exactly one Template element")
        
        self.Name = IOUtil.GetFileName(IOUtil.GetDirectoryName(filename))
        self.Id = self.Name.lower()
        self.Version = 1
        self.Template = xmlTemplate[0]
        self.Path = IOUtil.GetDirectoryName(filename)
        self.Prefix = ("%s_" % (self.Name)).upper()

    def __LoadTemplateConfiguration(self, element):
        res = []
        foundElements = element.findall("Template")
        for foundElement in foundElements:
            res.append(XmlNewTemplate(foundElement))
        return res



