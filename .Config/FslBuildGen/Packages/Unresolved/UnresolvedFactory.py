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
from typing import TypeVar
from FslBuildGen import PackageConfig
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorOption import UnresolvedPackageFlavorOption
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Log import Log
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.Packages.CompanyName import CompanyName
from FslBuildGen.Packages.PackageCustomInfo import PackageCustomInfo
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName
from FslBuildGen.Packages.PackageNameInfo import PackageNameInfo
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.PackageTraceContext import PackageTraceContext
from FslBuildGen.Packages.Unresolved.UnresolvedFilter import UnresolvedFilter
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackage
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackageFlags
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackagePaths
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariant import UnresolvedPackageVariant
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlStuff import XmlGenFileBuildCustomization

# ninjaRecipePackageName = UnresolvedPackageName(self._BasicConfig.ToolConfig.CMakeConfiguration.NinjaRecipePackageName)
# isWindows = PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows
class FactoryCreateContext(object):
    def __init__(self, log: Log, generatorInfo: GeneratorInfo, ninjaRecipePackageName: UnresolvedPackageName, isWindows: bool) -> None:
        self.Log = log
        self.GeneratorInfo = generatorInfo
        self.NinjaRecipePackageName = ninjaRecipePackageName
        self.IsWindows = isWindows


FilterElementType = TypeVar('FilterElementType', UnresolvedExternalDependency, UnresolvedPackageDependency)

class UnresolvedFactory(object):

    @staticmethod
    def CreateUnresolvedPackageFlavorOption(createContext: FactoryCreateContext, name: PackageFlavorOptionName,
                                            introducedByPackageName: UnresolvedPackageName, directRequirements: List[UnresolvedPackageRequirement],
                                            directDependencies: List[UnresolvedPackageDependency], externalDependencies: List[UnresolvedExternalDependency],
                                            directDefines: List[UnresolvedPackageDefine]) -> UnresolvedPackageFlavorOption:

        directDependencies = UnresolvedFilter.FilterOnConditions(createContext.Log, createContext.GeneratorInfo, directDependencies, "Dependency")
        externalDependencies = UnresolvedFilter.FilterOnConditions(createContext.Log, createContext.GeneratorInfo, externalDependencies, "ExternalDependency")

        return UnresolvedPackageFlavorOption(name, introducedByPackageName, directRequirements, directDependencies, externalDependencies, directDefines)

    @staticmethod
    def CreatePackagePlatform(createContext: FactoryCreateContext,
                              name: str, directRequirements: List[UnresolvedPackageRequirement], directDependencies: List[UnresolvedPackageDependency],
                              variants: List[UnresolvedPackageVariant], supported: bool,
                              externalDependencies: List[UnresolvedExternalDependency], directDefines: List[UnresolvedPackageDefine],
                              directExperimentalRecipe: Optional[XmlExperimentalRecipe], flavors: List[UnresolvedPackageFlavor]) -> PackagePlatform:

        directDependencies = UnresolvedFilter.FilterOnConditions(createContext.Log, createContext.GeneratorInfo, directDependencies, "Dependency")
        externalDependencies = UnresolvedFilter.FilterOnConditions(createContext.Log, createContext.GeneratorInfo, externalDependencies, "ExternalDependency")
        return PackagePlatform(name, directRequirements, directDependencies, variants, supported, externalDependencies, directDefines,
                               directExperimentalRecipe, flavors)

    @staticmethod
    def CreateUnresolvedPackage(createContext: FactoryCreateContext, packageProjectContext: PackageProjectContext,
                                nameInfo: PackageNameInfo, companyName: CompanyName, creationYear: Optional[str], packageFile: Optional[PackageFile],
                                sourceFileHash: str, packageType: PackageType, packageFlags: UnresolvedPackageFlags, packageLanguage: PackageLanguage,
                                directDependencies: List[UnresolvedPackageDependency], directRequirements: List[UnresolvedPackageRequirement],
                                directDefines: List[UnresolvedPackageDefine], externalDependencies: List[UnresolvedExternalDependency],
                                path: UnresolvedPackagePaths, templateType: str, buildCustomization: Dict[str, XmlGenFileBuildCustomization],
                                directExperimentalRecipe: Optional[XmlExperimentalRecipe], resolvedPlatform: PackagePlatform,
                                directPlatformSupported: bool, customInfo: PackageCustomInfo,
                                traceContext: PackageTraceContext) -> UnresolvedPackage:

        # Add implicit dependencies
        directDependencies = UnresolvedFactory.__BuildDirectDependencies(createContext, packageProjectContext, nameInfo.FullName, packageType,
                                                                         directDependencies, directExperimentalRecipe, resolvedPlatform)

        # filter based on conditions
        externalDependencies = UnresolvedFilter.FilterOnConditions(createContext.Log, createContext.GeneratorInfo, externalDependencies, "ExternalDependency")
        directDependencies = UnresolvedFilter.FilterOnConditions(createContext.Log, createContext.GeneratorInfo, directDependencies, "Dependency")

        return UnresolvedPackage(packageProjectContext, nameInfo, companyName, creationYear, packageFile, sourceFileHash, packageType,
                                 packageFlags, packageLanguage, directDependencies, directRequirements, directDefines, externalDependencies, path,
                                 templateType, buildCustomization, directExperimentalRecipe, resolvedPlatform, directPlatformSupported,
                                 customInfo, traceContext)


    @staticmethod
    def __BuildDirectDependencies(createContext: FactoryCreateContext, packageProjectContext: PackageProjectContext, packageName: PackageInstanceName,
                                  packageType: PackageType, directDependencies: List[UnresolvedPackageDependency],
                                  directExperimentalRecipe: Optional[XmlExperimentalRecipe],
                                  resolvedPlatform: PackagePlatform) -> List[UnresolvedPackageDependency]:
        if (packageType == PackageType.ExternalLibrary and resolvedPlatform.Name == PackageConfig.PlatformNameString.ANDROID and
                createContext.IsWindows and directExperimentalRecipe is not None and
                not UnresolvedFactory.__ContainsDependency(directDependencies, createContext.NinjaRecipePackageName.Value)):
            directDependencies += [UnresolvedPackageDependency(createContext.NinjaRecipePackageName, AccessType.Public)]

        for basePackage in packageProjectContext.BasePackages:
            if packageType != PackageType.ToolRecipe and packageName.Value != basePackage.Name and not UnresolvedFactory.__ContainsDependency(directDependencies, basePackage.Name):
                directDependencies += [UnresolvedPackageDependency(UnresolvedPackageName(basePackage.Name), AccessType.Public)]

        return directDependencies + resolvedPlatform.DirectDependencies

    @staticmethod
    def __ContainsDependency(dependencies: List[UnresolvedPackageDependency], dependencyName: str) -> bool:
        for entry in dependencies:
            if entry.Name.Value == dependencyName:
                return True
        return False
