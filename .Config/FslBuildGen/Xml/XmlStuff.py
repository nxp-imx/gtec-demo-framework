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
from typing import List
from typing import Optional
#import copy
#import os
#import os.path
import xml.etree.ElementTree as ET
#from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen import Util
from FslBuildGen.Log import Log
from FslBuildGen.DataTypes import OptimizationType
#from FslBuildGen.DataTypes import PackageCreationYearString
#from FslBuildGen.DataTypes import PackageLanguage
#from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.DataTypes import PackageString
#from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
#from FslBuildGen.Exceptions import FileNotFoundException
#from FslBuildGen.Exceptions import PackageMissingRequiredIncludeDirectoryException
#from FslBuildGen.Exceptions import PackageMissingRequiredSourceDirectoryException
#from FslBuildGen.Exceptions import UnsupportedException
#from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Exceptions import VariantOptionNameCollisionException
from FslBuildGen.Xml.Exceptions import XmlException
#from FslBuildGen.Xml.Exceptions import XmlException2
#from FslBuildGen.Xml.Exceptions import BuildCustomizationAlreadyDefinedException
#from FslBuildGen.Xml.Exceptions import DefaultValueAlreadyDefinedException
#from FslBuildGen.Xml.Exceptions import PlatformAlreadyDefinedException
#from FslBuildGen.Xml.Exceptions import UnknownBuildCustomizationException
#from FslBuildGen.Xml.Exceptions import UnknownDefaultValueException
from FslBuildGen.Xml.Exceptions import XmlFormatException
#from FslBuildGen.Xml.Exceptions import XmlInvalidRootElement
from FslBuildGen.Xml.Exceptions import XmlInvalidVirtualVariantOptionException
#from FslBuildGen.Xml.Exceptions import XmlRequirementNameException
#from FslBuildGen.Xml.Exceptions import XmlRequirementStringException
#from FslBuildGen.Xml.Exceptions import XmlRequirementTypeException
#from FslBuildGen.Xml.Exceptions import XmlRequirementTypeExtensionRequiresAValidExtendFieldException
#from FslBuildGen.Xml.Exceptions import XmlUnsupportedPackageType
from FslBuildGen.Xml.Exceptions import XmlUnsupportedPlatformException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedVariantNameException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedVariantOptionNameException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedVirtualVariantNameException
from FslBuildGen.Xml.Exceptions import XmlUnsupportedVirtualVariantOptionNameException
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Flavor.XmlGenFileFlavor import XmlGenFileFlavor
from FslBuildGen.Xml.Flavor.XmlGenFileFlavorExtension import XmlGenFileFlavorExtension
from FslBuildGen.Xml.XmlBase import XmlBase
from FslBuildGen.Xml.XmlBase2 import XmlBase2
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement

class DefaultValueName(object):
    DEFAULT_PLATFORM_Supported = "Default.Platform.Supported"


class LocalPackageDefaultValues(object):
    Platform_Supported = True


class XmlGenFileImportTemplate(XmlBase):
    __AttribName = 'Name'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)


class XmlGenFileVariantOption(XmlBase2):
    __AttribName = 'Name'

    def __init__(self, log: Log, xmlElement: ET.Element, ownerPackageName: str) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.IntroducedByPackageName = ownerPackageName


class XmlGenFileVariant(XmlBase):
    __AttribName = 'Name'
    __AttribExtend = 'Extend'
    __AttribType = 'Type'

    def __init__(self, log: Log, xmlElement: ET.Element, ownerPackageName: str) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName, self.__AttribExtend, self.__AttribType})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.IntroducedByPackageName = ownerPackageName
        self.AllowExtend = self._ReadBoolAttrib(xmlElement, self.__AttribExtend, False)
        elementType = self._ReadAttrib(xmlElement, self.__AttribType, 'Normal')
        self.Options = self.__GetXMLVariantOptions(xmlElement, ownerPackageName)
        self.Type = self.__ExtractType(elementType)
        self.OptionDict = self.__BuildOptionDict()  # type: Dict[str, XmlGenFileVariantOption]
        self.__ValidateVariantName()
        self.__ValidateOptionNames()


    def __ValidateVariantName(self) -> None:
        if self.Type == VariantType.Normal:
            if not Util.IsValidName(self.Name):
                raise XmlUnsupportedVariantNameException(self.XMLElement, self.Name)
        elif self.Type == VariantType.Virtual:
            if not Util.IsValidVirtualVariantName(self.Name):
                raise XmlUnsupportedVirtualVariantNameException(self.XMLElement, self.Name)
        else:
            raise XmlException(self.XMLElement, "Unknown variant type")


    def __ValidateOptionNames(self) -> None:
        if self.Type == VariantType.Normal:
            for option in self.Options:
                if not Util.IsValidName(option.Name):
                    raise XmlUnsupportedVariantOptionNameException(option.XMLElement, option.Name)
        elif self.Type == VariantType.Virtual:
            if len(self.Options) != 1:
                raise XmlInvalidVirtualVariantOptionException(self.XMLElement, self.Name)
            if not self.Options[0].Name == self.Name:
                raise XmlUnsupportedVirtualVariantOptionNameException(self.Options[0].XMLElement, self.Options[0].Name, self.Name)
        else:
            raise XmlException(self.XMLElement, "Unknown variant type")


    def __ExtractType(self, variantType: str) -> VariantType:
        if variantType == 'Normal':
            return VariantType.Normal
        elif variantType == 'Virtual':
            return VariantType.Virtual
        else:
            raise XmlException(self.XMLElement, "Unknown variant type: '{0}' expected: Normal, Virtual".format(variantType))


    def __BuildOptionDict(self) -> Dict[str, XmlGenFileVariantOption]:
        optionDict = {} # type: Dict[str, XmlGenFileVariantOption]
        optionNameSet = {}  # type: Dict[str, str]
        for option in self.Options:
            optionDict[option.Name] = option
            key = option.Name.lower()
            if not key in optionNameSet:
                optionNameSet[key] = option.Name
            else:
                raise VariantOptionNameCollisionException(optionNameSet[key], option.Name)
        return optionDict


    def __GetXMLVariantOptions(self, elem: ET.Element, ownerPackageName: str) -> List[XmlGenFileVariantOption]:
        options = []
        if elem is not None:
            for child in elem:
                if child.tag == 'Option':
                    options.append(XmlGenFileVariantOption(self.Log, child, ownerPackageName))
        options.sort(key=lambda s: s.Name.lower())
        return options


class XmlGenFilePlatform(XmlBase2):
    __AttribName = 'Name'
    __AttribProjectId = 'ProjectId'
    __AttribSupported = 'Supported'

    def __init__(self, log: Log, xmlElement: ET.Element, defaultValues: LocalPackageDefaultValues,
                 requirements: List[XmlGenFileRequirement], dependencies: List[XmlGenFileDependency], flavors: List[XmlGenFileFlavor],
                 flavorExtensions: List[XmlGenFileFlavorExtension], variants: List[XmlGenFileVariant],
                 experimentalRecipe: Optional[XmlExperimentalRecipe]) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName, self.__AttribProjectId, self.__AttribSupported})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.DirectRequirements = requirements
        self.DirectDependencies = dependencies
        self.DirectExperimentalRecipe = experimentalRecipe
        self.Flavors = flavors
        self.FlavorExtensions = flavorExtensions
        self.Variants = variants
        self.ProjectId = self._TryReadAttrib(xmlElement, self.__AttribProjectId)
        self.Supported = self._ReadBoolAttrib(xmlElement, self.__AttribSupported, defaultValues.Platform_Supported)

        #self.DirectVariantDependencyUnion = self.__CreateVariantDependencyUnion(variants)

        if((not PackageString.PLATFORM_SEPARATOR in self.Name) and not self.Name in PackageConfig.APPROVED_PLATFORM_NAMES and self.Name != PackageString.PLATFORM_WILDCARD):
            raise XmlUnsupportedPlatformException(xmlElement, self.Name)
        # Wrong place to check this.
        # We need to know what type the project file is and we also need to know
        # if the target environment is windows
        #if self.Name == PackageConfig.PlatformNameString.WINDOWS and self.ProjectId == None:
        #    raise XmlMissingWindowsVisualStudioProjectIdException(xmlElement)

    #def __CreateVariantDependencyUnion(self, variants):
    #    res = []
    #    if variants != None:
    #        for variant in variants:
    #            for option in variant.Options:
    #                for dep in option.DirectDependencies:
    #                    print("Variant dep: '%s'" % dep.Name)
    #    return res

    def SYS_SetName(self, name: str) -> None:
        self.Name = name


class FakeXmlGenFilePlatform(XmlGenFilePlatform):
    def __init__(self, log: Log, platformName: str,
                 defaultValues: LocalPackageDefaultValues) -> None:
        fakeXmlElement = FakeXmlElementFactory.CreateWithName("Platform", platformName)
        super().__init__(log, fakeXmlElement, defaultValues, [], [], [], [], [], None)


class XmlGenFileBuildCustomization(XmlBase):
    __AttribName = 'Name'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName})
        self.Name = xmlElement.tag
        self.ValueString = self._ReadAttrib(xmlElement, 'Value')

    def GetValueAsInt(self) -> int:
        raise Exception("XmlGenFileBuildCustomization is not a int")


class XmlGenFileBuildCustomization_Optimization(XmlGenFileBuildCustomization):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        if self.ValueString == "Disabled":
            self.Value = OptimizationType.Disabled
        elif self.ValueString == "Default":
            self.Value = OptimizationType.Default
        elif self.ValueString == "Full":
            self.Value = OptimizationType.Full
        else:
            raise XmlFormatException("Unknown optimization type '{0}' : '{1}'".format(self.ValueString, self.Name))

    def GetValueAsInt(self) -> int:
        return self.Value
