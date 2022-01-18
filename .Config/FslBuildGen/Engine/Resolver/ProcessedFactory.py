#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
from typing import Optional
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackage
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackageFlags
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackagePaths
from FslBuildGen.Engine.Resolver.ProcessedPackageDependency import ProcessedPackageDependency
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Log import Log
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.Packages.CompanyName import CompanyName
from FslBuildGen.Packages.PackageNameInfo import PackageNameInfo
from FslBuildGen.Packages.PackageCustomInfo import PackageCustomInfo
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackageTraceContext import PackageTraceContext
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedFilter import UnresolvedFilter
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageGenerate import UnresolvedPackageGenerate
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlStuff import XmlGenFileBuildCustomization

class ProcessedFactory(object):
    @staticmethod
    def CreatePackage(log: Log, generatorInfo: GeneratorInfo, packageProjectContext: PackageProjectContext, nameInfo: PackageNameInfo,
                      companyName: CompanyName, creationYear: Optional[str], packageFile: Optional[PackageFile], sourceFileHash: str,
                      packageType: PackageType, packageFlags: ProcessedPackageFlags, packageLanguage: PackageLanguage,
                      generateList: List[UnresolvedPackageGenerate], 
                      directDependencies: List[ProcessedPackageDependency], directRequirements: List[UnresolvedPackageRequirement],
                      directDefines: List[UnresolvedPackageDefine], externalDependencies: List[UnresolvedExternalDependency],
                      path: ProcessedPackagePaths, templateType: str, buildCustomization: Dict[str, XmlGenFileBuildCustomization],
                      directExperimentalRecipe: Optional[XmlExperimentalRecipe], resolvedPlatform: PackagePlatform, directPlatformSupported: bool,
                      customInfo: PackageCustomInfo, traceContext: PackageTraceContext) -> ProcessedPackage:

        # filter based on conditions
        externalDependencies = UnresolvedFilter.FilterOnConditions(log, generatorInfo, externalDependencies, "ExternalDependency")
        directDependencies = UnresolvedFilter.FilterOnConditions(log, generatorInfo, directDependencies, "Dependency")

        return ProcessedPackage(packageProjectContext, nameInfo, companyName, creationYear, packageFile, sourceFileHash, packageType,
                                packageFlags, packageLanguage, generateList, directDependencies, directRequirements, directDefines,
                                externalDependencies, path, templateType, buildCustomization, directExperimentalRecipe, resolvedPlatform,
                                directPlatformSupported, customInfo, traceContext)
