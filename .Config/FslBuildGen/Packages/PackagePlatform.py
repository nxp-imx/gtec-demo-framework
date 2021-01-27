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
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariant import UnresolvedPackageVariant
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe

class PackagePlatform(object):
    def __init__(self, name: str, directRequirements: List[UnresolvedPackageRequirement], directDependencies: List[UnresolvedPackageDependency],
                 variants: List[UnresolvedPackageVariant], supported: bool, externalDependencies: List[UnresolvedExternalDependency],
                 directDefines: List[UnresolvedPackageDefine], directExperimentalRecipe: Optional[XmlExperimentalRecipe],
                 flavors: List[UnresolvedPackageFlavor]) -> None:
        super().__init__()
        self.Name = name
        self.DirectUsedFeatures = [requirement for requirement in directRequirements if requirement.Type == PackageRequirementTypeString.Feature]  # type: List[UnresolvedPackageRequirement]
        self.DirectRequirements = directRequirements
        self.DirectDependencies = directDependencies
        self.Variants = variants
        self.Supported = supported
        self.ExternalDependencies = externalDependencies
        self.DirectDefines = directDefines  # type: List[UnresolvedPackageDefine]
        self.DirectExperimentalRecipe = directExperimentalRecipe
        self.Flavors = flavors
