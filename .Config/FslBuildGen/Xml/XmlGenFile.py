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
from typing import Set
from typing import Tuple
from typing import Union
import copy
import hashlib
import os
import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import PackageCreationYearString
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import PackageString
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import SubPackageSupport
from FslBuildGen.Exceptions import FileNotFoundException
from FslBuildGen.Exceptions import PackageMissingRequiredIncludeDirectoryException
from FslBuildGen.Exceptions import PackageMissingRequiredSourceDirectoryException
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.PackageConfig import APPROVED_PLATFORM_NAMES
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.PackageTemplateLoader import PackageTemplateLoader
from FslBuildGen.ToolConfig import ToolConfigPackageLocation
from FslBuildGen.Xml import FakeXmlElementFactory
from FslBuildGen.Xml.Exceptions import BuildCustomizationAlreadyDefinedException
from FslBuildGen.Xml.Exceptions import DefaultValueAlreadyDefinedException
from FslBuildGen.Xml.Exceptions import PlatformAlreadyDefinedException
from FslBuildGen.Xml.Exceptions import UnknownBuildCustomizationException
from FslBuildGen.Xml.Exceptions import UnknownDefaultValueException
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.Exceptions import XmlInvalidRootElement
from FslBuildGen.Xml.Exceptions import XmlUnsupportedPackageType
from FslBuildGen.Xml.SubPackageSupportConfig import SubPackageSupportConfig
from FslBuildGen.Xml.XmlCommonFslBuild import XmlCommonFslBuild
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipeline
from FslBuildGen.Xml.XmlGenFileDefine import XmlGenFileDefine
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement
from FslBuildGen.Xml.XmlStuff import DefaultValueName
from FslBuildGen.Xml.XmlStuff import LocalPackageDefaultValues
from FslBuildGen.Xml.XmlStuff import XmlGenFileBuildCustomization
from FslBuildGen.Xml.XmlStuff import XmlGenFileBuildCustomization_Optimization
from FslBuildGen.Xml.XmlStuff import XmlGenFileImportTemplate
from FslBuildGen.Xml.XmlStuff import XmlGenFilePlatform
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariant


class XmlGenFile(XmlCommonFslBuild):
    def __init__(self, config: Config, defaultPackageLanguage: int) -> None:
        super(XmlGenFile, self).__init__(config, FakeXmlElementFactory.CreateWithName("FakeGenFile", "FSLBUILD_INVALID_INITIAL_VALUE"), SubPackageSupportConfig(PackageType.TopLevel, SubPackageSupport.Disabled))
        self.SourceFilename = None # type: Optional[str]
        self.SourceFileHash = ""   # type: str
        self.Name = ''
        self.ShortName = None  # type: Optional[str]
        self.Namespace = None  # type: Optional[str]
        self.PackageLocation = None  # type: Optional [ToolConfigPackageLocation]
        self.Type = PackageType.Library
        self.IsVirtual = False
        self.DirectDependencies = []  # type: List[XmlGenFileDependency]
        self.DirectRequirements = []  # type: List[XmlGenFileRequirement]
        self.DirectDefines = []
        self.DirectExperimentalRecipe = None    # type: Optional[XmlExperimentalRecipe]
        self.Platforms = {}  # type: Dict[str, XmlGenFilePlatform]
        self.AbsolutePath = None  # type: Optional[str]
        self.AbsoluteIncludePath = None  # type: Optional[str]
        self.AbsoluteSourcePath = None  # type: Optional[str]
        self.AbsoluteContentPath = None  # type: Optional[str]
        self.AbsoluteContentSourcePath = None  # type: Optional[str]
        self.SourcePackageFile = None  # type: Optional[PackageFile]
        self.PackageLanguage = defaultPackageLanguage
        self.BaseIncludePath = "include"
        self.BaseSourcePath = "source"
        self.BuildCustomization = {}  # type: Dict[str, XmlGenFileBuildCustomization]
        self.CompanyName = "NotDefined"
        self.CreationYear = None  # type: Optional[str]
        self.TemplateType = ""
        self.AllowCheck = True
        self.EnableExtendedSourceExtensions = False
        self.AllowCombinedDirectory = False
        self.PlatformDefaultSupportedValue = True
        self.SystemDefaultValues = LocalPackageDefaultValues()


    def Load(self, config: Config, packageTemplateLoader: PackageTemplateLoader, packageFile: PackageFile) -> None:
        filename = packageFile.AbsoluteFilePath
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate gen file %s", filename)

        self.SourceFilename = filename
        self.PackageLocation = packageFile.PackageRootLocation

        fileContent = IOUtil.ReadFile(filename)
        self.SourceFileHash = self.__CalcContentHash(fileContent)
        elem = ET.fromstring(fileContent)
        if elem.tag != 'FslBuildGen':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGen'")

        elem, theType = self.__FindPackageElementAndType(elem)

        packageName = self._ReadAttrib(elem, 'Name')
        defaultValues = self.__GetDefaultValues(elem, packageName)
        allowNoInclude = self._ReadBoolAttrib(elem, 'NoInclude', False)
        companyName = self._ReadAttrib(elem, 'Company', config.ToolConfig.DefaultCompany)

        if config.ToolConfig.RequirePackageCreationYear:
            creationYear = self._ReadAttrib(elem, 'CreationYear')
        else:
            creationYear = self._ReadAttrib(elem, 'CreationYear', PackageCreationYearString.NotDefined)

        templateType = self._ReadAttrib(elem, 'TemplateType', "")
        self.AllowCheck = self._ReadBoolAttrib(elem, 'AllowCheck', True)
        # if this is set we allow '.cc' files for C++ code.
        self.EnableExtendedSourceExtensions = self._ReadBoolAttrib(elem, 'EnableExtendedSourceExtensions', False)

        self.BaseIncludePath = self._ReadAttrib(elem, 'OverrideInclude', 'include')
        self.BaseSourcePath = self._ReadAttrib(elem, 'OverrideSource', 'source')
        self.AllowCombinedDirectory = self._ReadBoolAttrib(elem, 'AllowCombinedDirectory', False)

        self.BaseLoad(elem, SubPackageSupportConfig(theType, config.SubPackageSupport))

        requirements = self._GetXMLRequirements(elem)
        allowRecipes = self.__DoesTypeAllowRecipes(theType)

        # Add recipe and dependencies
        self.DirectExperimentalRecipe = self._TryGetExperimentalRecipe(elem, packageName, allowRecipes)
        if self.DirectExperimentalRecipe is not None:
            self.DirectDependencies += self.__AddExperimentalRecipeDependencies(self.DirectDependencies, [], self.DirectExperimentalRecipe)

        platforms = self.__GetXMLPlatforms(elem, packageName, self.DirectDependencies, allowRecipes, defaultValues)
        self.BuildCustomization = self.__GetBuildCustomizations(elem, packageName)

        templates = self.__GetXMLImportTemplates(elem)
        self.__ImportTemplates(packageTemplateLoader, templates, requirements, self.DirectDependencies, self.ExternalDependencies, self.DirectDefines)

        if self.BaseIncludePath == self.BaseSourcePath and not self.AllowCombinedDirectory:
            raise XmlException2(elem, "Package '{0}' uses the same directory for include and source '{1}'".format(packageName, self.BaseIncludePath))

        self.SourcePackageFile = packageFile
        self.XMLElement = elem
        self.Name = packageName
        self.ShortName = None
        self.Namespace = None
        self.SetType(theType)
        self.Platforms = platforms
        self.DirectRequirements = requirements
        self.AbsolutePath = None
        self.AbsoluteIncludePath = None
        self.AbsoluteSourcePath = None
        self.CompanyName = companyName
        self.CreationYear = creationYear
        self.TemplateType = templateType
        self.PlatformDefaultSupportedValue = defaultValues.Platform_Supported
        self.SystemDefaultValues = defaultValues

        self._ValidateName(elem, self.Name)
        # This check was moved to the package loader where it belongs
        #self.__ValidateFilename(config, filename)
        self.__ResolveNames(self.Name)
        self.__ValidateBasicDependencyCorrectness()
        self.__ValidateDefines()
        self.__ResolvePaths(config, filename, allowNoInclude)
        # FIX: check for clashes with platform addition
        #      check for platform variant name clashes

    def __CalcContentHash(self, content: str) -> str:
        encodedContent = content.encode()
        hashObject = hashlib.sha1(encodedContent)
        return hashObject.hexdigest()

    def __DoesTypeAllowRecipes(self, packageType: int) -> bool:
        return packageType == PackageType.ExternalLibrary or packageType == PackageType.ToolRecipe


    def __FindPackageElementAndType(self, elem: ET.Element) -> Tuple[ET.Element, int]:
        currentElem = elem.find("Library")
        if not currentElem is None:
            return (currentElem, PackageType.Library)

        currentElem = elem.find("Executable")
        if not currentElem is None:
            return (currentElem, PackageType.Executable)

        currentElem = elem.find("ExternalLibrary")
        if not currentElem is None:
            return (currentElem, PackageType.ExternalLibrary)

        currentElem = elem.find("HeaderLibrary")
        if not currentElem is None:
            return (currentElem, PackageType.HeaderLibrary)

        currentElem = elem.find("ToolRecipe")
        if not currentElem is None:
            return (currentElem, PackageType.ToolRecipe)

        raise XmlUnsupportedPackageType("Could not locate a Executable, Library, ExternalLibrary or HeaderLibrary element")


    def __ResolveNames(self, name: str) -> None:
        self.ShortName, self.Namespace = XmlGenFile.GetPackageNamesUtil(name)


    @staticmethod
    def GetPackageNamesUtil(name: str) -> Tuple[str, str]:
        index = name.rfind('.')
        if index < 0:
            return name, ''
        return name[index+1:], name[:index]


    def SetType(self, theType: int) -> None:
        self.Type = theType
        self.IsVirtual = (theType == PackageType.TopLevel or theType == PackageType.ExternalLibrary or theType == PackageType.HeaderLibrary or theType == PackageType.ToolRecipe)


    def __GetXMLPlatforms(self, elem: ET.Element, ownerPackageName: str,
                          directDependencies: List[XmlGenFileDependency], allowRecipes: bool,
                          defaultValues: LocalPackageDefaultValues) -> Dict[str, XmlGenFilePlatform]:
        platforms = {}  # type: Dict[str, XmlGenFilePlatform]
        for child in elem:
            if child.tag == 'Platform':
                dependencies = self._GetXMLDependencies(child)
                requirements = self._GetXMLRequirements(child)
                variants = self.__GetXMLVariants(child, ownerPackageName)
                experimentalRecipe = self._TryGetExperimentalRecipe(child, ownerPackageName, allowRecipes)
                dependencies = self.__AddExperimentalRecipeDependencies(directDependencies, dependencies, experimentalRecipe)
                xmlPlatform = XmlGenFilePlatform(self.BasicConfig, child, defaultValues, requirements, dependencies, variants, experimentalRecipe, self.GetSubPackageSupport())
                if xmlPlatform.Name in platforms:
                    raise PlatformAlreadyDefinedException(xmlPlatform.XMLElement, xmlPlatform.Name) #, platforms[xmlPlatform.Name])
                platforms[xmlPlatform.Name] = xmlPlatform

        # Handle wildcard platforms
        for platformName in APPROVED_PLATFORM_NAMES:
            if not platformName in platforms and PackageString.PLATFORM_WILDCARD in platforms:
                clone = copy.deepcopy(platforms[PackageString.PLATFORM_WILDCARD])
                clone.SYS_SetName(platformName)
                platforms[platformName] = clone

        return platforms


    def _TryGetExperimentalRecipe(self, xmlElement: ET.Element, defaultName: str, allowRecipe: bool) -> Optional[XmlExperimentalRecipe]:
        recipeElementName = 'ExperimentalRecipe'
        child = self._TryGetElement(xmlElement, recipeElementName)
        if child is None:
            return None
        if not allowRecipe:
            raise Exception("This package type does not allow '{0}' elments".format(recipeElementName))
        return XmlExperimentalRecipe(self.BasicConfig, child, defaultName)


    def __GetPipelineToolDependencyNames(self, pipeline: Optional[XmlRecipePipeline]) -> Set[str]:
        dependencies = set()  # type: Set[str]
        if not pipeline is None and not pipeline.CommandList is None:
            for command in pipeline.CommandList:
                if command.BuildToolPackageNames is not None:
                    for buildToolPackageName in command.BuildToolPackageNames:
                        dependencies.add(buildToolPackageName)
        return dependencies


    def __AddExperimentalRecipeDependencies(self, directDependencies: List[XmlGenFileDependency],
                                            dependencies: List[XmlGenFileDependency],
                                            experimentalRecipe: Optional[XmlExperimentalRecipe]) -> List[XmlGenFileDependency]:
        if experimentalRecipe is None:
            return dependencies
        pipelineDependencies = self.__GetPipelineToolDependencyNames(experimentalRecipe.Pipeline)
        if len(pipelineDependencies) <= 0:
            return dependencies

        predefinedUniqueDeps = set()  # type: Set[str]
        for entry in directDependencies:
            predefinedUniqueDeps.add(entry.Name)
        for entry in dependencies:
            predefinedUniqueDeps.add(entry.Name)

        combinedDependencies = list(dependencies)
        for dependencyPackageName in pipelineDependencies:
            if not dependencyPackageName in predefinedUniqueDeps:
                combinedDependencies.append(self._CreateFakeXMLDependencies(dependencyPackageName))
        return combinedDependencies


    def __GetDefaultValues(self, elem: ET.Element, ownerPackageName: str) -> LocalPackageDefaultValues:
        defaultValues = LocalPackageDefaultValues()
        customizations = set()  # type: Set[str]
        for child in elem:
            key = DefaultValueName.DEFAULT_PLATFORM_Supported
            if child.tag == key:
                defaultValues.Platform_Supported = self._ReadBoolAttrib(child, "Value")
                if key in customizations:
                    raise DefaultValueAlreadyDefinedException(child, key)
                customizations.add(key)
            elif child.tag.startswith('Default.'):
                raise UnknownDefaultValueException(child)

        return defaultValues


    def __GetBuildCustomizations(self, elem: ET.Element, ownerPackageName: str) -> Dict[str, XmlGenFileBuildCustomization]:
        customizations = {}  # type: Dict[str, XmlGenFileBuildCustomization]
        for child in elem:
            if child.tag == 'BuildCustomization.Debug.Optimization':
                xmlBuildCustomization = XmlGenFileBuildCustomization_Optimization(self.BasicConfig, child)
                if xmlBuildCustomization.Name in customizations:
                    raise BuildCustomizationAlreadyDefinedException(xmlBuildCustomization.XMLElement, xmlBuildCustomization.Name) #, customizations[xmlBuildCustomization.Name])
                customizations[xmlBuildCustomization.Name] = xmlBuildCustomization
            elif child.tag.startswith('BuildCustomization.'):
                raise UnknownBuildCustomizationException(child)

        return customizations



    def __GetXMLImportTemplates(self, elem: ET.Element) -> List[XmlGenFileImportTemplate]:
        elements = []  # type: List[XmlGenFileImportTemplate]
        for child in elem:
            if child.tag == 'ImportTemplate':
                elements.append(XmlGenFileImportTemplate(self.BasicConfig, child))
        return elements


    def __GetXMLVariants(self, elem: ET.Element, ownerPackageName: str) -> List[XmlGenFileVariant]:
        elements = []
        for child in elem:
            if child.tag == 'Variant':
                elements.append(XmlGenFileVariant(self.BasicConfig, child, ownerPackageName, self.GetSubPackageSupport()))
        return elements


    def __ImportTemplates(self, packageTemplateLoader: PackageTemplateLoader,
                          templates: List[XmlGenFileImportTemplate],
                          requirements: List[XmlGenFileRequirement],
                          dependencies: List[XmlGenFileDependency],
                          externalDependencies: List[XmlGenFileExternalDependency],
                          directDefines: List[XmlGenFileDefine]) -> None:
        for template in templates:
            imported = packageTemplateLoader.Import(self.GetSubPackageSupport(), template, template.Name)
            for reqEntry in imported.DirectRequirements:
                requirements.append(reqEntry)
            for directEntry in imported.DirectDependencies:
                dependencies.append(directEntry)
            for extDepEntry in imported.ExternalDependencies:
                externalDependencies.append(extDepEntry)
            for directDefEntry in imported.DirectDefines:
                directDefines.append(directDefEntry)


    def __ValidateBasicDependencyCorrectness(self) -> None:
        errorMsg = "Dependency defined multiple times '%s'"
        nameDict = {}  # type: Dict[str, Union[XmlGenFileDependency, XmlGenFileExternalDependency, XmlGenFileDefine]]
        self.__ValidateNames(nameDict, self.DirectDependencies, errorMsg)
        for platform in list(self.Platforms.values()):
            platformNameDict = copy.copy(nameDict)
            self.__ValidateNames(platformNameDict, platform.DirectDependencies, errorMsg)

        errorMsg = "ExternalDependency defined multiple times '%s'"
        nameDict.clear()
        self.__ValidateNames(nameDict, self.ExternalDependencies, errorMsg)
        for platform in list(self.Platforms.values()):
            platformNameDict = copy.copy(nameDict)
            self.__ValidateNames(platformNameDict, platform.ExternalDependencies, errorMsg)


    def __ValidateDefines(self) -> None:
        errorMsg = "Define defined multiple times '%s'"
        nameDict = {}  # type: Dict[str, Union[XmlGenFileDependency, XmlGenFileExternalDependency, XmlGenFileDefine]]
        self.__ValidateNames(nameDict, self.DirectDefines, errorMsg)
        for platform in list(self.Platforms.values()):
            platformNameDict = copy.copy(nameDict)
            self.__ValidateNames(platformNameDict, platform.DirectDefines, errorMsg)


    def __ValidateNames(self, rNameDict: Dict[str, Union[XmlGenFileDependency, XmlGenFileExternalDependency, XmlGenFileDefine]],
                        dependencyList: Union[List[XmlGenFileDependency], List[XmlGenFileExternalDependency], List[XmlGenFileDefine]],
                        errorStr: str) -> None:
        for entry in dependencyList:
            entryId = entry.Name.lower()
            if not entryId in rNameDict:
                rNameDict[entryId] = entry
            else:
                raise XmlException2(entry.XMLElement, errorStr % (entry.Name))


    def __ResolvePathIncludeDir(self, config: Config, allowNoInclude: bool) -> None:
        if self.AbsolutePath is None:
            raise Exception("AbsolutePath can not be None")

        self.AbsoluteIncludePath = IOUtil.NormalizePath(IOUtil.Join(self.AbsolutePath, self.BaseIncludePath))
        includeDirExist = os.path.isdir(self.AbsoluteIncludePath)
        if not includeDirExist and (os.path.exists(self.AbsoluteIncludePath) or not (allowNoInclude or config.DisableIncludeDirCheck)):
            raise PackageMissingRequiredIncludeDirectoryException(self.AbsoluteIncludePath)
        if not includeDirExist and allowNoInclude:
            self.AbsoluteIncludePath = None


    def __ResolvePaths(self, config: Config, filename: str, allowNoInclude: bool) -> None:
        if not os.path.isabs(filename):
            raise UsageErrorException()

        self.AbsolutePath = IOUtil.GetDirectoryName(filename)
        if not self.IsVirtual:
            sourcePath = self.BaseSourcePath
            if self.PackageLanguage == PackageLanguage.CPP:
                self.__ResolvePathIncludeDir(config, allowNoInclude)
            elif self.PackageLanguage == PackageLanguage.CSharp:
                #sourcePath = self.Name
                pass
            else:
                raise UnsupportedException("Unsupported package language: {0}".format(self.PackageLanguage))
            self.AbsoluteSourcePath = IOUtil.Join(self.AbsolutePath, sourcePath)
            self.AbsoluteContentPath = IOUtil.Join(self.AbsolutePath, "Content")
            self.AbsoluteContentSourcePath = IOUtil.Join(self.AbsolutePath, "Content.bld")
            if not os.path.isdir(self.AbsoluteSourcePath) and not config.DisableSourceDirCheck:
                raise PackageMissingRequiredSourceDirectoryException(self.AbsoluteSourcePath)
        elif self.Type == PackageType.HeaderLibrary:
            if self.PackageLanguage == PackageLanguage.CPP:
                self.__ResolvePathIncludeDir(config, allowNoInclude)
            else:
                raise UsageErrorException("HeaderLibrary is only supported for C++")
