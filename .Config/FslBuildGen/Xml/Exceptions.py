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
import xml.etree.ElementTree as ET
from FslBuildGen import PackageConfig


class XmlException(Exception):
    """ Error
    """

class XmlException2(Exception):
    def __init__(self, xmlElement: ET.Element, message: str) -> None:
        super().__init__("{0}".format(message))


class XmlRequiredAttributeMissingException(XmlException2):
    def __init__(self, xmlElement: ET.Element, attribName: str) -> None:
        msg = "Element '{0}' did not contain the required '{1}' attribute".format(xmlElement.tag, attribName)
        super().__init__(xmlElement, msg)

                #raise XmlDuplicatedCompilerConfigurationException(result[config.Id].BasedOn, result[config.Id].Name, config.XMLElement, config.Name)

class XmlDuplicatedCompilerConfigurationException(XmlException2):
    def __init__(self, xmlElement1: ET.Element, name1: str, xmlElement2: ET.Element, name2: str) -> None:
        msg = "Compiler comfigured multiple times as '{0}' and '{1}'".format(name1, name2)
        super().__init__(xmlElement1, msg)


class XmlUnsupportedCompilerVersionException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str, version: str, validVersions: str) -> None:
        msg = "Compiler '{0}' does not support version '{1}', expected '{2}'".format(name, version, validVersions)
        super().__init__(xmlElement, msg)


class XmlUnsupportedPlatformException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "Platform name: '{0}' is not a valid platform name, expected: {1}".format(name, ", ".join(PackageConfig.APPROVED_PLATFORM_NAMES))
        super().__init__(xmlElement, msg)


class XmlMissingWindowsVisualStudioProjectIdException(XmlException2):
    def __init__(self, xmlElement: ET.Element, packageName: str) -> None:
        msg = "The windows platform requires a ProjectId to be defined. Package name '{0}'".format(packageName)
        super().__init__(xmlElement, msg)


class XmlInvalidPackageNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, correctName: str, invalidName: str, strPath: str, strPathRoot: str) -> None:
        msg = "The name: '{0}' does not match the expected name of '{1}' for package at {2} with a package root of {3}".format(invalidName, correctName, strPath, strPathRoot)
        super().__init__(xmlElement, msg)


class XmlInvalidSubPackageNameException(XmlInvalidPackageNameException):
    def __init__(self, xmlElement: ET.Element, correctName: str, invalidName: str, strPath: str, strPathRoot: str) -> None:
        super().__init__(xmlElement, correctName, invalidName, strPath, strPathRoot)


class XmlInvalidSubPackageShortNameConflictException(XmlException2):
    def __init__(self, xmlElement: ET.Element, correctName: str, invalidName: str) -> None:
        msg = "The short sub package name: '{0}' is not allowed because a directory called '{1}' exist".format(invalidName, correctName)
        super().__init__(xmlElement, msg)


class XmlUnsupportedPackageNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The name: '{0}' contains unsupported characters".format(name)
        super().__init__(xmlElement, msg)


class XmlUnsupportedVariantNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The variant name: '{0}' contains unsupported characters".format(name)
        super().__init__(xmlElement, msg)


class XmlRequirementNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The requirement name: '{0}' contains unsupported characters".format(name)
        super().__init__(xmlElement, msg)


class XmlRequirementStringException(XmlException2):
    def __init__(self, xmlElement: ET.Element, desc: str, name: str) -> None:
        msg = "The requirement {0}: '{1}' contains unsupported characters".format(desc, name)
        super().__init__(xmlElement, msg)


class XmlRequirementTypeException(XmlException2):
    def __init__(self, xmlElement: ET.Element, entryName: str, entryType: str, entryExtends: str, validList: List[str]) -> None:
        msg = "The requirement type: '{0}' is invalid for '{1}' extends '{2}', expected [{3}]".format(entryType, entryName, entryExtends, ', '.join(validList))
        super().__init__(xmlElement, msg)


class XmlRequirementTypeExtensionRequiresAValidExtendFieldException(XmlException2):
    def __init__(self, xmlElement: ET.Element, entryName: str) -> None:
        msg = "The requirement '{0}' with type of 'extension' requires a non empty 'Extends' attribute".format(entryName)
        super().__init__(xmlElement, msg)


class XmlUnsupportedVirtualVariantNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The variant name: '{0}' contains unsupported characters".format(name)
        super().__init__(xmlElement, msg)


class XmlUnsupportedTag(XmlException2):
    def __init__(self, xmlElement: ET.Element, message: str) -> None:
        super().__init__(xmlElement, message)


class XmlUnsupportedVariantOptionNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The variant option name: '{0}' contains unsupported characters".format(name)
        super().__init__(xmlElement, msg)


class XmlInvalidVirtualVariantOptionException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "Variant '{0}'. A virtual variant must contain exactly one option.".format(name)
        super().__init__(xmlElement, msg)


class XmlUnsupportedVirtualVariantOptionNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, variantName: str, name: str) -> None:
        msg = "The virtual variant option name: '{0}' is not equal to the variant name '{1}'".format(variantName, name)
        super().__init__(xmlElement, msg)


class XmlUnsupportedSubPackageNameException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The name: '{0}' is a invalid sub package name".format(name)
        super().__init__(xmlElement, msg)


class XmlVariantOptionNameCollisionException(XmlException2):
    def __init__(self, xmlElement: ET.Element, firstName: str, secondName: str) -> None:
        msg = "The option name: '{0}' collides with the previously defined '{1}'".format(secondName, firstName)
        super().__init__(xmlElement, msg)


class UnknownBuildCustomizationException(XmlException2):
    def __init__(self, xmlElement: ET.Element) -> None:
        msg = "The build customization name: '{0}' is not valid.".format(xmlElement.tag)
        super().__init__(xmlElement, msg)


class DefaultValueAlreadyDefinedException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The default value name: '{0}' has already been defined.".format(name)
        super().__init__(xmlElement, msg)


class UnknownDefaultValueException(XmlException2):
    def __init__(self, xmlElement: ET.Element) -> None:
        msg = "The default value name: '{0}' is not valid.".format(xmlElement.tag)
        super().__init__(xmlElement, msg)


class XmlFormatException(Exception):
    """Indicate that a error exist in the config xml
    """


class ImportTemplateNotFoundException(XmlException2):
    def __init__(self, xmlElement: ET.Element, templateName: str) -> None:
        msg = "'{0}' tries to import unknown template '{1}'".format(xmlElement.tag, templateName)
        super().__init__(xmlElement, msg)


class XmlInvalidRootElement(Exception):
    """ Error
    """

class XmlUnsupportedPackageType(Exception):
    """ Error
    """


class PlatformAlreadyDefinedException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The platform name: '{0}' has already been defined.".format(name)
        super().__init__(xmlElement, msg)


class BuildCustomizationAlreadyDefinedException(XmlException2):
    def __init__(self, xmlElement: ET.Element, name: str) -> None:
        msg = "The build customization name: '{0}' has already been defined.".format(name)
        super().__init__(xmlElement, msg)
