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

from typing import List
from typing import Optional
import xml.etree.ElementTree as ET
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.Xml.XmlGenFileDefine import XmlGenFileDefine
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement
from FslBuildGen.Xml.XmlStuff import XmlGenFilePlatform
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariant


class PackagePlatform(object):
    def __init__(self, name: str, xmlGenFilePlatform: XmlGenFilePlatform) -> None:
        super(PackagePlatform, self).__init__()
        if xmlGenFilePlatform != None and xmlGenFilePlatform.Name != name:
            raise Exception("the xmlGenFilePlatform.Name and name must be equal")
        self.Name = name
        self.DirectUsedFeatures = [requirement for requirement in xmlGenFilePlatform.DirectRequirements if requirement.Type == PackageRequirementTypeString.Feature]  # type: List[XmlGenFileRequirement]
        self.DirectRequirements = xmlGenFilePlatform.DirectRequirements  # type: List[XmlGenFileRequirement]
        self.DirectDependencies = xmlGenFilePlatform.DirectDependencies  # type: List[XmlGenFileDependency]
        self.Variants = xmlGenFilePlatform.Variants  # type: List[XmlGenFileVariant]
        self.ProjectId = xmlGenFilePlatform.ProjectId  # type: Optional[str]
        self.Supported = xmlGenFilePlatform.Supported  # type: bool
        self.ExternalDependencies = xmlGenFilePlatform.ExternalDependencies  # type: List[XmlGenFileExternalDependency]
        self.DirectDefines = xmlGenFilePlatform.DirectDefines  # type: List[XmlGenFileDefine]
        self.XMLElement = xmlGenFilePlatform.XMLElement  # type: ET.Element
