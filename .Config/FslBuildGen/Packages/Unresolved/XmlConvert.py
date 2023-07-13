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
from FslBuildGen import PackageConfig
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.PackageFlavorQuickName import PackageFlavorQuickName
from FslBuildGen.Engine.PackageFlavorSelection import PackageFlavorSelection
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelectionsEmpty
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorExtension import UnresolvedPackageFlavorExtension
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorUnqualifiedName import UnresolvedPackageFlavorUnqualifiedName
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavorOption import UnresolvedPackageFlavorOption
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Packages.CompanyName import CompanyName
from FslBuildGen.Packages.PackageCustomInfo import PackageCustomInfo
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName
from FslBuildGen.Packages.PackageNameInfo import PackageNameInfo
from FslBuildGen.Packages.PackageShortName import PackageShortName
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.PackageTraceContext import PackageTraceContext
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependencyPackageManager import UnresolvedExternalDependencyPackageManager
from FslBuildGen.Packages.Unresolved.UnresolvedFactory import FactoryCreateContext
from FslBuildGen.Packages.Unresolved.UnresolvedFactory import UnresolvedFactory
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackage
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackageFlags
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackagePaths
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageGenerate import UnresolvedPackageGenerate
from FslBuildGen.Packages.Unresolved.UnresolvedPackageGenerateGrpcProtoFile import UnresolvedPackageGenerateGrpcProtoFile
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariant import UnresolvedPackageVariant
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariantOption import UnresolvedPackageVariantOption
from FslBuildGen.Xml.Flavor.XmlGenFileFlavor import XmlGenFileFlavor
from FslBuildGen.Xml.Flavor.XmlGenFileFlavorExtension import XmlGenFileFlavorExtension
from FslBuildGen.Xml.Flavor.XmlGenFileFlavorOption import XmlGenFileFlavorOption
from FslBuildGen.Xml.XmlGenFileDefine import XmlGenFileDefine
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency
from FslBuildGen.Xml.XmlGenFileExternalDependencyPackageManager import XmlGenFileExternalDependencyPackageManager
from FslBuildGen.Xml.XmlGenFileGenerate import XmlGenFileGenerate
from FslBuildGen.Xml.XmlGenFileGenerateGrpcProtoFile import XmlGenFileGenerateGrpcProtoFile
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement
from FslBuildGen.Xml.XmlStuff import XmlGenFilePlatform
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariant
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariantOption
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlGenFile import XmlGenFile
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency

class XmlConvert(object):
    @staticmethod
    def CreateFromXmlGenFilePlatform(createContext: FactoryCreateContext, name: str, xmlGenFilePlatform: XmlGenFilePlatform) -> 'PackagePlatform':
        if xmlGenFilePlatform is not None and xmlGenFilePlatform.Name != name:
            raise Exception("the xmlGenFilePlatform.Name and name must be equal")

        variants = XmlConvert.ToUnresolvedPackageVariantList(xmlGenFilePlatform.Variants)
        directRequirements = XmlConvert.ToUnresolvedPackageRequirementList(xmlGenFilePlatform.DirectRequirements)
        directDependencies = XmlConvert.ToUnresolvedPackageDependencyList(xmlGenFilePlatform.DirectDependencies, False)
        externalDependencies = XmlConvert.ToUnresolvedExternalDependencyList(xmlGenFilePlatform.ExternalDependencies)
        directDefines = XmlConvert.ToUnresolvedPackageDefineList(xmlGenFilePlatform.DirectDefines)
        flavors = XmlConvert.__GenerateFlavors(createContext, xmlGenFilePlatform.Flavors)
        flavorExtensions = XmlConvert.__GenerateFlavorExtensions(createContext, xmlGenFilePlatform.FlavorExtensions)
        return UnresolvedFactory.CreatePackagePlatform(createContext, name, directRequirements, directDependencies,
                                                       variants, xmlGenFilePlatform.Supported, externalDependencies, directDefines,
                                                       xmlGenFilePlatform.DirectExperimentalRecipe, flavors, flavorExtensions)

    @staticmethod
    def ToUnresolvedPackageDefine(xmlDefine: XmlGenFileDefine) -> UnresolvedPackageDefine:
        return UnresolvedPackageDefine(xmlDefine.Name, xmlDefine.Value, xmlDefine.Access)

    @staticmethod
    def ToUnresolvedPackageVariant(xmlVariant: XmlGenFileVariant) -> UnresolvedPackageVariant:
        options = [XmlConvert.ToUnresolvedPackageVariantOption(xmlOption) for xmlOption in xmlVariant.Options]
        return UnresolvedPackageVariant(xmlVariant.Name, xmlVariant.IntroducedByPackageName, xmlVariant.AllowExtend, xmlVariant.Type, options)

    @staticmethod
    def ToUnresolvedPackageVariantOption(xmlOption: XmlGenFileVariantOption) -> UnresolvedPackageVariantOption:
        directDefines = XmlConvert.ToUnresolvedPackageDefineList(xmlOption.DirectDefines)
        externalDependencies = [XmlConvert.ToUnresolvedExternalDependency(xmlExternalDependency) for xmlExternalDependency in xmlOption.ExternalDependencies]
        return UnresolvedPackageVariantOption(xmlOption.Name, xmlOption.IntroducedByPackageName, directDefines, externalDependencies)

    @staticmethod
    def ToUnresolvedExternalDependency(xmlExternalDependency: XmlGenFileExternalDependency) -> UnresolvedExternalDependency:
        packageManager = XmlConvert.ToUnresolvedExternalDependencyPackageManager(xmlExternalDependency.PackageManager)
        return UnresolvedExternalDependency(xmlExternalDependency.Name,
                                            xmlExternalDependency.DebugName,
                                            xmlExternalDependency.TargetName,
                                            xmlExternalDependency.Include,
                                            xmlExternalDependency.Location,
                                            xmlExternalDependency.HintPath,
                                            xmlExternalDependency.Version,
                                            xmlExternalDependency.PublicKeyToken,
                                            xmlExternalDependency.ProcessorArchitecture,
                                            xmlExternalDependency.Culture,
                                            packageManager,
                                            xmlExternalDependency.IfCondition,
                                            xmlExternalDependency.Type,
                                            xmlExternalDependency.Access,
                                            xmlExternalDependency.IsManaged)

    @staticmethod
    def ToUnresolvedExternalDependencyPackageManager(xmlValue: Optional[XmlGenFileExternalDependencyPackageManager]) -> Optional[UnresolvedExternalDependencyPackageManager]:
        return UnresolvedExternalDependencyPackageManager(xmlValue.Name, xmlValue.Version, xmlValue.PackageTargetFramework, xmlValue.PrivateAssets, xmlValue.IncludeAssets) if xmlValue is not None else None

    @staticmethod
    def ToUnresolvedPackageRequirement(xmlValue: XmlGenFileRequirement) -> UnresolvedPackageRequirement:
        return UnresolvedPackageRequirement(xmlValue.Name, xmlValue.Type, xmlValue.Extends, xmlValue.Version)

    @staticmethod
    def ToUnresolvedPackageDependency(xmlValue: XmlGenFileDependency, allowInternalNames: bool) -> UnresolvedPackageDependency:
        unresolvedPackageName = UnresolvedPackageName(xmlValue.Name, allowInternalNames)
        flavorConstraints = XmlConvert.__ToPackageFlavorSelections(xmlValue.Flavor)
        return UnresolvedPackageDependency(unresolvedPackageName, xmlValue.Access, flavorConstraints, xmlValue.IfCondition)

    @staticmethod
    def __ToPackageFlavorSelections(constraintsDict: Dict[str, str]) -> PackageFlavorSelections:
        if len(constraintsDict) <= 0:
            return PackageFlavorSelectionsEmpty.Empty

        res = [] # type: List[PackageFlavorSelection]
        for key, value in constraintsDict.items():
            res.append(PackageFlavorSelection(PackageFlavorName.FromString(key), PackageFlavorOptionName(value)))
        return PackageFlavorSelections(res)

    @staticmethod
    def ToUnresolvedPackage(createContext: FactoryCreateContext, packageProjectContext: PackageProjectContext,
                            xmlValue: XmlGenFile, resolvedPlatform: PackagePlatform, resolvedPlatformDirectSupported: bool,
                            allowInternalNames: bool = False) -> UnresolvedPackage:
        instanceName = PackageInstanceName(xmlValue.Name)
        nameInfo = PackageNameInfo(instanceName)

        companyName = CompanyName(xmlValue.CompanyName)
        creationYear = xmlValue.CreationYear
        packageFile = xmlValue.PackageFile
        sourceFileHash = xmlValue.SourceFileHash
        packageType = xmlValue.Type
        packageFlags = UnresolvedPackageFlags(xmlValue.IsVirtual, xmlValue.AllowCheck, xmlValue.UnitTest, xmlValue.ShowInMainReadme,
                                              xmlValue.PackageNameBasedIncludePath, xmlValue.PlatformDefaultSupportedValue,
                                              xmlValue.EnableExtendedSourceExtensions)
        packageLanguage = xmlValue.PackageLanguage

        generateList = XmlConvert.ToUnresolvedPackageGenerateList(xmlValue.GenerateList)
        generateGrpcProtoFileList = XmlConvert.ToUnresolvedPackageGenerateGrpcProtoFileList(xmlValue.GenerateGrpcProtoFileList)
        directDependencies = XmlConvert.ToUnresolvedPackageDependencyList(xmlValue.DirectDependencies, allowInternalNames)
        directRequirements = XmlConvert.ToUnresolvedPackageRequirementList(xmlValue.DirectRequirements)
        directDefines = XmlConvert.ToUnresolvedPackageDefineList(xmlValue.DirectDefines)
        externalDependencies = XmlConvert.ToUnresolvedExternalDependencyList(xmlValue.ExternalDependencies)
        path = UnresolvedPackagePaths(xmlValue.IncludePath, xmlValue.SourcePath, xmlValue.ContentPath, xmlValue.ContentSourcePath,
                                      xmlValue.BaseIncludePath, xmlValue.BaseSourcePath)
        templateType = xmlValue.TemplateType
        buildCustomization = xmlValue.BuildCustomization
        directExperimentalRecipe = XmlConvert.__TryGetExperimentalRecipe(xmlValue, resolvedPlatform)

        visualStudioProjectId = None
        if PackageConfig.PlatformNameString.WINDOWS in xmlValue.Platforms:
            visualStudioProjectId = xmlValue.Platforms[PackageConfig.PlatformNameString.WINDOWS].ProjectId

        packageCustomInfo = PackageCustomInfo(visualStudioProjectId)
        packageTraceContext = PackageTraceContext(xmlValue.PackageFile, xmlValue)


        return UnresolvedFactory.CreateUnresolvedPackage(createContext, packageProjectContext, nameInfo, companyName, creationYear, packageFile,
                                                         sourceFileHash, packageType, packageFlags, packageLanguage, generateList, generateGrpcProtoFileList, directDependencies,
                                                         directRequirements, directDefines, externalDependencies, path, templateType,
                                                         buildCustomization, directExperimentalRecipe, resolvedPlatform,
                                                         resolvedPlatformDirectSupported, packageCustomInfo, packageTraceContext)

    @staticmethod
    def ToUnresolvedExternalDependencyList(xmlList: List[XmlGenFileExternalDependency]) -> List[UnresolvedExternalDependency]:
        return [XmlConvert.ToUnresolvedExternalDependency(entry) for entry in xmlList]

    @staticmethod
    def ToUnresolvedPackageVariantList(xmlList: List[XmlGenFileVariant]) -> List[UnresolvedPackageVariant]:
        return [XmlConvert.ToUnresolvedPackageVariant(entry) for entry in xmlList]

    @staticmethod
    def ToUnresolvedPackageDefineList(xmlList: List[XmlGenFileDefine]) -> List[UnresolvedPackageDefine]:
        return [XmlConvert.ToUnresolvedPackageDefine(entry) for entry in xmlList]

    @staticmethod
    def ToUnresolvedPackageRequirementList(xmlList: List[XmlGenFileRequirement]) -> List[UnresolvedPackageRequirement]:
        return [XmlConvert.ToUnresolvedPackageRequirement(xmlEntry) for xmlEntry in xmlList]

    @staticmethod
    def ToUnresolvedPackageDependencyList(xmlList: List[XmlGenFileDependency], allowInternalNames: bool) -> List[UnresolvedPackageDependency]:
        return [XmlConvert.ToUnresolvedPackageDependency(xmlEntry, allowInternalNames) for xmlEntry in xmlList]

    @staticmethod
    def ToUnresolvedPackageGenerateList(xmlList: List[XmlGenFileGenerate]) -> List[UnresolvedPackageGenerate]:
        return [XmlConvert.ToUnresolvedPackageGenerate(xmlEntry) for xmlEntry in xmlList]

    @staticmethod
    def ToUnresolvedPackageGenerateGrpcProtoFileList(xmlList: List[XmlGenFileGenerateGrpcProtoFile]) -> List[UnresolvedPackageGenerateGrpcProtoFile]:
        return [XmlConvert.ToUnresolvedPackageGenerateGrpcProtoFile(xmlEntry) for xmlEntry in xmlList]

    @staticmethod
    def ToUnresolvedPackageGenerate(entry: XmlGenFileGenerate) -> UnresolvedPackageGenerate:
        return UnresolvedPackageGenerate(entry.TemplateFile, entry.TargetFile)

    @staticmethod
    def ToUnresolvedPackageGenerateGrpcProtoFile(entry: XmlGenFileGenerateGrpcProtoFile) -> UnresolvedPackageGenerateGrpcProtoFile:
        return UnresolvedPackageGenerateGrpcProtoFile(entry.Include, entry.GrpcServices)

    @staticmethod
    def __TryGetExperimentalRecipe(genFile: XmlGenFile, platformObject: PackagePlatform) -> Optional[XmlExperimentalRecipe]:
        xmlExperimentalRecipe = None  # type: Optional[XmlExperimentalRecipe]
        if platformObject.DirectExperimentalRecipe is None:
            xmlExperimentalRecipe = genFile.DirectExperimentalRecipe
        else:
            # Fallback to generic
            xmlExperimentalRecipe = platformObject.DirectExperimentalRecipe
        return xmlExperimentalRecipe

    @staticmethod
    def __GenerateFlavors(createContext: FactoryCreateContext, xmlFlavors: List[XmlGenFileFlavor]) -> List[UnresolvedPackageFlavor]:
        res = [] # type: List[UnresolvedPackageFlavor]
        for xmlFlavor in xmlFlavors:
            options = XmlConvert.__GenerateFlavorOptions(createContext, xmlFlavor.Options)
            introducedByPackageName = UnresolvedPackageName(xmlFlavor.IntroducedByPackageName)
            flavorName = PackageFlavorName(introducedByPackageName, UnresolvedPackageFlavorUnqualifiedName(xmlFlavor.Name))
            flavorQuickName = PackageFlavorQuickName(UnresolvedPackageFlavorUnqualifiedName(xmlFlavor.QuickName)) if xmlFlavor.QuickName is not None else None
            flavor = UnresolvedPackageFlavor(flavorName, flavorQuickName, options)
            res.append(flavor)
        return res

    @staticmethod
    def __GenerateFlavorExtensions(createContext: FactoryCreateContext, xmlFlavorExtensions: List[XmlGenFileFlavorExtension]) -> List[UnresolvedPackageFlavorExtension]:
        res = [] # type: List[UnresolvedPackageFlavorExtension]
        for xmlFlavorExtension in xmlFlavorExtensions:
            options = XmlConvert.__GenerateFlavorOptions(createContext, xmlFlavorExtension.Options)
            introducedByPackageName = UnresolvedPackageName(xmlFlavorExtension.IntroducedByPackageName)
            flavorName = PackageFlavorName.FromString(xmlFlavorExtension.Name)
            flavor = UnresolvedPackageFlavorExtension(flavorName, introducedByPackageName, options)
            res.append(flavor)
        return res

    @staticmethod
    def __GenerateFlavorOptions(createContext: FactoryCreateContext, xmlFlavorOptions: List[XmlGenFileFlavorOption]) -> List[UnresolvedPackageFlavorOption]:
        res = [] # type: List[UnresolvedPackageFlavorOption]
        for xmlFlavorOption in xmlFlavorOptions:
            name = PackageFlavorOptionName(xmlFlavorOption.Name)
            supported = xmlFlavorOption.Supported
            introducedByPackageName = UnresolvedPackageName(xmlFlavorOption.IntroducedByPackageName)
            directRequirements = XmlConvert.ToUnresolvedPackageRequirementList(xmlFlavorOption.DirectRequirements)
            directDependencies = XmlConvert.ToUnresolvedPackageDependencyList(xmlFlavorOption.DirectDependencies, False)
            externalDependencies = XmlConvert.ToUnresolvedExternalDependencyList(xmlFlavorOption.ExternalDependencies)
            directDefines = XmlConvert.ToUnresolvedPackageDefineList(xmlFlavorOption.DirectDefines)
            flavorOption = UnresolvedFactory.CreateUnresolvedPackageFlavorOption(createContext, name, supported, introducedByPackageName, directRequirements,
                                                                                 directDependencies, externalDependencies, directDefines)
            res.append(flavorOption)
        return res
