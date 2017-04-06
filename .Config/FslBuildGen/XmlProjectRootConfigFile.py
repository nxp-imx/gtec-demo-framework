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
from FslBuildGen.XmlToolConfigFile import XMLLoadPackageConfiguration, XmlConfigPackageLocation
from FslBuildGen import IOUtil, Util, PackageConfig


class XmlProjectRootConfigFile(XmlBase):
    def __init__(self, filename):
        elem = None
        if filename != None:
            if not os.path.isfile(filename):
                raise FileNotFoundException("Could not locate config file %s", filename)

            tree = ET.parse(filename)
            elem = tree.getroot()
            if elem.tag != 'FslBuildGenProjectRoot':
                raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGenProjectRoot'")

        super(XmlProjectRootConfigFile, self).__init__(elem)

        self.Version = '1'
        self.RootDirectory = None
        self.DefaultPackageLanguage = PackageLanguage.CPP
        self.ToolConfigFile = None
        self.XmlPackageConfiguration = []
        self.SourceFileName = None
        self.DefaultTemplate = MagicStrings.VSDefaultCPPTemplate

        if elem != None:
            self.Version = XmlBase._ReadAttrib(self, elem, 'Version', True, '1')
            self.RootDirectory = IOUtil.GetDirectoryName(filename)

            projectElem = XmlBase._GetElement(self, elem, "Project")
            toolConfigFilePath = XmlBase._ReadAttrib(self, projectElem, 'ToolConfigFile', False, None)
            self.DefaultPackageLanguage = self.__GetDefaultPackageLanguage(projectElem)
            self.ToolConfigFile = XmlBase._ResolveEnvPath(self, toolConfigFilePath)
            self.XmlPackageConfiguration = self.__LoadPackageConfigurations(projectElem, filename)
            self.SourceFileName = filename
            self.DefaultTemplate = XmlBase._ReadAttrib(self, projectElem, 'DefaultTemplate', False, MagicStrings.VSDefaultCPPTemplate)


    def __LoadPackageConfigurations(self, projectElem, filename):
        xmlPackageConfigurations = XMLLoadPackageConfiguration(projectElem, filename)
        for entry in xmlPackageConfigurations:
            # If the default package and no locatiosn been supplied then we assume the root folder of the project file
            if  entry.Name == 'default' and len(entry.Locations) <= 0:
                xmlConfigPackageLocation = XmlConfigPackageLocation(None)
                xmlConfigPackageLocation.Name = MagicStrings.ProjectRoot
                entry.Locations = [xmlConfigPackageLocation]
        return xmlPackageConfigurations


    def __GetDefaultPackageLanguage(self, elem):
        defaultPackageLanguage = XmlBase._ReadAttrib(self, elem, 'DefaultPackageLanguage', False, "C++")
        return PackageLanguage.FromString(defaultPackageLanguage)

