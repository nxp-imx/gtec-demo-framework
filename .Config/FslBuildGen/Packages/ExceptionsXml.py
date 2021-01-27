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
from FslBuildGen import Util
from FslBuildGen.Packages.Package import PackagePlatformVariant
from FslBuildGen.Packages.Package import PackagePlatformVariantOption
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement
#from FslBuildGen.Xml.XmlStuff import XmlGenFileVariant
#from FslBuildGen.Xml.XmlStuff import XmlGenFileVariantOption

#class FeatureUseDuplicatedException(XmlException2):
#    def __init__(self, package, name):
#        msg = "Feature name '{0}' in package '{1}' already defined".format(name, package.Name)
#        super().__init__(package.XMLElement, msg)


#class FeatureNameCollisionException(XmlException2):
#    def __init__(self, package1, name1, package2, name2) -> None:
#        msg = "Feature name '{0}' in package '{1}' collides with feature name '{2}' from package '{3}'".format(name1, package1.Name, name2, package2.Name)
#        super().__init__(package1.XMLElement, msg)

class RequirementNameCollisionException(XmlException2):
    def __init__(self, packageName1: PackageInstanceName, name1: str, packageName2: PackageInstanceName, name2: str) -> None:
        msg = "Requirement name '{0}' in package '{1}' collides with requirement name '{2}' from package '{3}'".format(name1, packageName1, name2, packageName2)
        super().__init__(msg)


class VariantExtensionNotSupportedException(XmlException2):
    def __init__(self, extendingPackageName: PackageInstanceName, extendingVariant: PackagePlatformVariant, basePackageName: PackageInstanceName) -> None:
        msg = "Package '{0}' variant: '{1}' can not extend the variant defined in '{2}'".format(extendingPackageName, extendingVariant.Name, basePackageName)
        super().__init__(msg)


class ExtendingVariantCanNotIntroduceNewOptionsException(XmlException2):
    def __init__(self, extendingPackageName: PackageInstanceName, extendingVariant: PackagePlatformVariant,
                 basePackageName: PackageInstanceName, baseVariant: PackagePlatformVariant, newOptions: List[PackagePlatformVariantOption]) -> None:
        msg = "Package '{0}' variant: '{1}' can not extend the options defined by '{2}' with '{3}'".format(extendingPackageName, extendingVariant.Name, basePackageName, ", ".join(Util.ExtractNames(newOptions)))
        super().__init__(msg)


class VariantNameCollisionException(XmlException2):
    def __init__(self, extendingPackageName: PackageInstanceName, extendingVariant: PackagePlatformVariant, basePackageName: PackageInstanceName, baseVariant: PackagePlatformVariant) -> None:
        msg = "Package '{0}' variant: '{1}' names collides with '{2}' variant '{3}'".format(extendingPackageName, extendingVariant.Name, basePackageName, baseVariant.Name)
        super().__init__(msg)
