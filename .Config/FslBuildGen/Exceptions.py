#!/usr/bin/env python

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

from FslBuildGen import PackageConfig
from FslBuildGen import Util

class GroupedException(Exception):
    def __init__(self, exceptionList):
        self.ExceptionList = exceptionList



class PackageHasMultipleDefinitionsException(Exception):
    def __init__(self, packageList):
        locationList = []
        for package in packageList:
            locationList.append(package.AbsolutePath)
        msg = "Package '%s' defined by '%s'" % (packageList[0].Name, ", ".join(locationList))
        super(PackageHasMultipleDefinitionsException, self).__init__(msg)


class PackageMissingRequiredIncludeDirectoryException(Exception):
    def __init__(self, directory):
        msg = "Required include directory '%s' not found" % (directory)
        super(PackageMissingRequiredIncludeDirectoryException, self).__init__(msg)


class PackageMissingRequiredSourceDirectoryException(Exception):
    def __init__(self, directory):
        msg = "Required source directory '%s' not found" % (directory)
        super(PackageMissingRequiredSourceDirectoryException, self).__init__(msg)


class InternalErrorException(Exception):
  """ Error
  """


class UsageErrorException(Exception):
  """ Error
  """

class UnknownTypeException(Exception):
  """ Error
  """

class FileNotFoundException(Exception):
  """ Error
  """

class XmlInvalidRootElement(Exception):
  """ Error
  """

class XmlUnsupportedPackageType(Exception):
  """ Error
  """

class XmlException(Exception):
  """ Error
  """

class XmlException2(Exception):
    def __init__(self, xmlElement, message):
        super(XmlException2, self).__init__("%s" % (message))

class XmlRequiredAttributeMissingException(XmlException2):
    def __init__(self, xmlElement, attribName):
        msg = "Element '%s' did not contain the required '%s' attribute" % (xmlElement.tag, attribName)
        super(XmlRequiredAttributeMissingException, self).__init__(xmlElement, msg)


class XmlUnsupportedPlatformException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "Platform name: '%s' is not a valid platform name, expected: %s" % (name, ", ".join(PackageConfig.APPROVED_PLATFORM_NAMES))
        super(XmlUnsupportedPlatformException, self).__init__(xmlElement, msg)


class XmlMissingWindowsVisualStudioProjectIdException(XmlException2):
    def __init__(self, xmlElement):
        msg = "The windows platform requires a ProjectId to be defined"
        super(XmlMissingWindowsVisualStudioProjectIdException, self).__init__(xmlElement, msg)


class PackageDuplicatedWindowsVisualStudioProjectIdException(XmlException2):
    def __init__(self, package1, package2, duplicatedProjectId):
        msg = "The windows platform requires a unique ProjectId to be defined. %s is defined in %s and %s " % (duplicatedProjectId, package1.Name, package2.Name)
        super(PackageDuplicatedWindowsVisualStudioProjectIdException, self).__init__(package1.XMLElement, msg)


class XmlInvalidPackageNameException(XmlException2):
    def __init__(self, xmlElement, correctName, invalidName):
        msg = "The name: '%s' does not match the dir name '%s'" % (invalidName, correctName)
        super(XmlInvalidPackageNameException, self).__init__(xmlElement, msg)


class XmlInvalidSubPackageNameException(XmlException2):
    def __init__(self, xmlElement, correctName, invalidName):
        msg = "The sub package names: '%s' does not match the dir names '%s'" % (invalidName, correctName)
        super(XmlInvalidSubPackageNameException, self).__init__(xmlElement, msg)

class XmlInvalidSubPackageShortNameConflictException(XmlException2):
    def __init__(self, xmlElement, correctName, invalidName):
        msg = "The short sub package name: '%s' is not allowed because a directory called '%s' exist" % (invalidName, correctName)
        super(XmlInvalidSubPackageShortNameConflictException, self).__init__(xmlElement, msg)


class XmlUnsupportedPackageNameException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "The name: '%s' contains unsupported characters" % (name)
        super(XmlUnsupportedPackageNameException, self).__init__(xmlElement, msg)


class XmlUnsupportedVariantNameException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "The variant name: '%s' contains unsupported characters" % (name)
        super(XmlUnsupportedVariantNameException, self).__init__(xmlElement, msg)


class XmlUnsupportedVirtualVariantNameException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "The variant name: '%s' contains unsupported characters" % (name)
        super(XmlUnsupportedVirtualVariantNameException, self).__init__(xmlElement, msg)


class XmlUnsupportedVariantOptionNameException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "The variant option name: '%s' contains unsupported characters" % (name)
        super(XmlUnsupportedVariantOptionNameException, self).__init__(xmlElement, msg)


class XmlInvalidVirtualVariantOptionException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "Variant '%s'. A virtual variant must contain exactly one option." % (name)
        super(XmlInvalidVirtualVariantOptionException, self).__init__(xmlElement, msg)

class XmlUnsupportedVirtualVariantOptionNameException(XmlException2):
    def __init__(self, xmlElement, variantName, name):
        msg = "The virtual variant option name: '%s' is not equal to the variant name '%s'" % (variantName, name)
        super(XmlUnsupportedVirtualVariantOptionNameException, self).__init__(xmlElement, msg)


class XmlUnsupportedSubPackageNameException(XmlException2):
    def __init__(self, xmlElement, name):
        msg = "The name: '%s' is a invalid sub package name" % (name)
        super(XmlUnsupportedSubPackageNameException, self).__init__(xmlElement, msg)


class XmlVariantOptionNameCollisionException(XmlException2):
    def __init__(self, xmlElement, firstName, secondName):
        msg = "The option name: '%s' collides with the previously defined '%s'" % (secondName, firstName)
        super(XmlVariantOptionNameCollisionException, self).__init__(xmlElement, msg)


class PlatformAlreadyDefinedException(XmlException2):
    def __init__(self, newPlaformDef, oldPlatformDef):
        msg = "The platform name: '%s' has already been defined." % (newPlaformDef.Name)
        super(PlatformAlreadyDefinedException, self).__init__(newPlaformDef.XMLElement, msg)


class BuildCustomizationAlreadyDefinedException(XmlException2):
    def __init__(self, newDef, oldDef):
        msg = "The build customization name: '{0}' has already been defined.".format(newDef.Name)
        super(BuildCustomizationAlreadyDefinedException, self).__init__(newDef.XMLElement, msg)


class UnknownBuildCustomizationException(XmlException2):
    def __init__(self, xmlElement):
        msg = "The build customization name: '{0}' is not valid.".format(xmlElement.tag)
        super(UnknownBuildCustomizationException, self).__init__(xmlElement, msg)


class VariantExtensionNotSupportedException(XmlException2):
    def __init__(self, extendingPackage, extendingVariant, basePackage):
        msg = "Package '%s' variant: '%s' can not extend the variant defined in '%s'" % (extendingPackage.Name, extendingVariant.Name, basePackage.Name)
        super(VariantExtensionNotSupportedException, self).__init__(extendingPackage.XMLElement, msg)


class VariantNotMarkedAsExtendingException(XmlException2):
    def __init__(self, previousVariant, extendingVariant):
        msg = "The variant '%s' in package '%s' is not marked for extend, but it would be extending '%s'" % (extendingVariant.Name, extendingVariant.IntroducedByPackageName, previousVariant.ExtensionInfo)
        super(VariantNotMarkedAsExtendingException, self).__init__(previousVariant.XMLElement, msg)

class ExtendingVariantCanNotIntroduceNewOptionsException(XmlException2):
    def __init__(self, extendingPackage, extendingVariant, basePackage, baseVariant, newOptions):
        msg = "Package '%s' variant: '%s' can not extend the options defined by '%s' with '%s'" % (extendingPackage.Name, extendingVariant.Name, basePackage.Name, ", ".join(Util.ExtractNames(newOptions)))
        super(ExtendingVariantCanNotIntroduceNewOptionsException, self).__init__(extendingPackage.XMLElement, msg)

class VariantNameCollisionException(XmlException2):
    def __init__(self, extendingPackage, extendingVariant, basePackage, baseVariant):
        msg = "Package '%s' variant: '%s' names collides with '%s' variant '%s'" % (extendingPackage.Name, extendingVariant.Name, basePackage.Name, baseVariant.Name)
        super(VariantNameCollisionException, self).__init__(extendingPackage.XMLElement, msg)

class VariantExtensionCanNotOverwriteExistingExternalDependencyException(XmlException2):
    def __init__(self, previous, introducedByPackageName, extending, extensionInfo):
        msg = "The variant option '%s' in package '%s' can overwrite a existing external dependency from '%s'" % (extending.Name, introducedByPackageName, extensionInfo)
        super(VariantExtensionCanNotOverwriteExistingExternalDependencyException, self).__init__(previous.XMLElement, msg)


class VariantExtensionCanNotOverwriteExistingCPPDefineException(XmlException2):
    def __init__(self, previous, introducedByPackageName, extending, extensionInfo):
        msg = "The variant option '%s' in package '%s' can overwrite a existing CPPDefine from '%s'" % (extending.Name, introducedByPackageName, extensionInfo)
        super(VariantExtensionCanNotOverwriteExistingCPPDefineException, self).__init__(previous.XMLElement, msg)



class FeatureUseDuplicatedException(XmlException2):
    def __init__(self, package, name):
        msg = "Feature name '%s' in package '%s' already defined" % (name, package.Name)
        super(FeatureUseDuplicatedException, self).__init__(package.XMLElement, msg)


class FeatureNameCollisionException(XmlException2):
    def __init__(self, package1, name1, package2, name2):
        msg = "Feature name '%s' in package '%s' collides with feature name '%s' from package '%s'" % (name1, package1.Name, name2, package2.Name)
        super(FeatureNameCollisionException, self).__init__(package1.XMLElement, msg)

class XmlFormatException(Exception):
  """Indicate that a error exist in the config xml
  """

class CircularDependencyException(Exception):
  """ E
  """

class CircularDependencyInDependentModuleException(Exception):
  """ E
  """

class ImportTemplateNotFoundException(XmlException2):
    def __init__(self, xmlElement, templateName):
        msg = "'%s' tries to import unknown template '%s'" % (xmlElement.tag, templateName)
        super(ImportTemplateNotFoundException, self).__init__(xmlElement, msg)

class DependencyNotFoundException(Exception):
    def __init__(self, xmlGenFile, depName):
        Exception.__init__(self, "'%s' has a dependency to a unknown package '%s'" % (xmlGenFile.Name, depName))

class InvalidDependencyException(Exception):
    def __init__(self, xmlGenFile, depName):
        Exception.__init__(self, "'%s' has a invalid dependency to package '%s'" % (xmlGenFile.Name, depName))

class NotImplementedException(Exception):
  """ E
  """

class UnsupportedException(Exception):
  """ E
  """

class DuplicatedConfigRootPath(Exception):
    """ E
    """
    def __init__(self, name, configFileName):
        msg = "Root path '%s' listed multiple times in %s" % (name, configFileName)
        super(DuplicatedConfigRootPath, self).__init__(msg)

class DuplicatedConfigPackageLocation(Exception):
    """ E
    """
    def __init__(self, name, configFileName):
        msg = "Package location '%s' listed multiple times in %s" % (name, configFileName)
        super(DuplicatedConfigPackageLocation, self).__init__(msg)


class DuplicatedConfigContentBuilder(Exception):
    """ E
    """
    def __init__(self, name, configFileName):
        msg = "ContentBuilder '%s' listed multiple times in %s" % (name, configFileName)
        super(DuplicatedConfigContentBuilder, self).__init__(msg)
