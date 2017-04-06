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

import copy
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen import IOUtil, Util, PackageConfig

class XmlBase(object):
    def __init__(self, xmlElement):
        super(XmlBase, self).__init__()
        self.XMLElement = xmlElement

    def _GetElement(self, xmlElement, elementName):
        foundElement = xmlElement.find(elementName)
        if foundElement == None:
            raise XmlException2(element, "Could not locate the expected {0} element".format(elementName))
        return foundElement

    def _ReadAttrib(self, xmlElement, attribName, isRequired, defaultValue):
        if attribName in xmlElement.attrib:
            return xmlElement.attrib[attribName]
        elif isRequired:
            raise XmlRequiredAttributeMissingException(xmlElement, attribName)
        else:
            return defaultValue

    def _ReadBoolAttrib(self, xmlElement, attribName, isRequired, defaultValue):
        value = self._ReadAttrib(xmlElement, attribName, isRequired, defaultValue)
        res = False
        if value == "true":
            return True
        elif value == "false":
            return False
        else:
            raise XmlFormatException("%s expects a value of either 'true' or 'false' not '%s'" % (attribName, value))


    def _ResolveEnvPath(self, pathName):
        environmentName = self.__ExtractEnvironmentVariablesFromName(pathName)
        if environmentName != None:
            path = IOUtil.GetEnvironmentVariableForDirectory(environmentName)
            pathName = pathName.replace("$("+environmentName+")", path)
        elif not os.path.isabs(pathName):
            raise EnvironmentError("%s path is not absolute" % (pathName))
        return pathName


    def __ExtractEnvironmentVariablesFromName(self, entry):
        if entry.startswith("$("):
            endIndex = entry.find(")")
            if endIndex < 0:
                raise XmlException2(self.XMLElement, "Incomplete environment variable found");
            return entry[2:endIndex]
        elif entry.find("$(") >= 0:
            raise XmlException2(self.XMLElement, "Environment variables can not be located in the middle of a string");
        return None



class SubPackageSupportConfig(object):
    def __init__(self, type, subPackageSupport):
        super(SubPackageSupportConfig, self).__init__()
        self.Type = type;
        self.Support = subPackageSupport
        self.AllowSubPackages = self.__AllowSubPackages(type, subPackageSupport)

    def __AllowSubPackages(self, type, subPackageSupport):
        return subPackageSupport == SubPackageSupport.Enabled or (subPackageSupport == SubPackageSupport.ExecutableOnly and type == PackageType.Executable)


class XmlBase2(XmlBase):
    def __init__(self, xmlElement, subPackageSupport):
        if not type(subPackageSupport) is SubPackageSupportConfig:
            raise UsageErrorException("The support object was not of the correct type");
        super(XmlBase2, self).__init__(xmlElement)
        self.__SubPackageSupport = subPackageSupport
        self.ExternalDependencies = self.__GetXMLExternalDependencies(xmlElement)
        self.DirectCPPDefines = self.__GetXMLCPPDefines(xmlElement)
        self.DirectDependencies = self._GetXMLDependencies(xmlElement)

    def BaseLoad(self, xmlElement, subPackageSupport):
        self.__SubPackageSupport = subPackageSupport
        self.ExternalDependencies = self.__GetXMLExternalDependencies(xmlElement)
        self.DirectCPPDefines = self.__GetXMLCPPDefines(xmlElement)
        self.DirectDependencies = self._GetXMLDependencies(xmlElement)

    def GetSubPackageSupport(self):
        return self.__SubPackageSupport

    def __GetXMLExternalDependencies(self, elem):
        dependencies = []
        if elem != None:
            for child in elem:
                if child.tag == 'ExternalDependency':
                    dependencies.append(XmlGenFileExternalDependency(child))
        return dependencies


    def __GetXMLCPPDefines(self, elem):
        dependencies = []
        if elem != None:
            for child in elem:
                if child.tag == 'CPPDefine':
                    dependencies.append(XmlGenFileCPPDefine(child))
        return dependencies


    def _ValidateName(self, xmlElement, name):
        allowSubPackages = self.__SubPackageSupport.AllowSubPackages
        if not Util.IsValidPackageName(name, allowSubPackages):
            if allowSubPackages and name.find('..') >= 0:
                raise XmlUnsupportedSubPackageNameException(xmlElement, name)
            else:
                raise XmlUnsupportedPackageNameException(xmlElement, name)


    def _GetXMLDependencies(self, elem):
        elements = []
        if elem != None:
            for child in elem:
                if child.tag == 'Dependency':
                    xmlDep = XmlGenFileDependency(child)
                    self._ValidateName(xmlDep.XMLElement, xmlDep.Name)
                    elements.append(xmlDep)
        return elements


class XmlCommonFslBuild(XmlBase2):
    def __init__(self, xmlElement, subPackageSupport):
        super(XmlCommonFslBuild, self).__init__(xmlElement, subPackageSupport)


    def _GetXMLUsesFeatures(self, elem):
        elements = []
        for child in elem:
            if child.tag == 'UsesFeature':
                elements.append(XmlGenFileUsesFeature(child))
        return elements



class XmlGenFslBuildTemplate(XmlCommonFslBuild):
    def __init__(self, filename, subPackageSupport):
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate gen file %s", filename)

        tree = ET.parse(filename)
        xmlElement = tree.getroot()
        if xmlElement.tag != 'FslBuildTemplate':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildTemplate'")

        super(XmlGenFslBuildTemplate, self).__init__(xmlElement, subPackageSupport)

        self.Name = IOUtil.GetFileNameWithoutExtension(filename)
        self.DirectUsedFeatures = self._GetXMLUsesFeatures(xmlElement)


class XmlGenFileDependency(XmlBase):
    def __init__(self, xmlElement):
        super(XmlGenFileDependency, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        access = XmlBase._ReadAttrib(self, xmlElement, 'Access', False, 'Public')
        if access == "Public":
            self.Access = AccessType.Public
        elif access == "Private":
            self.Access = AccessType.Private
        elif access == "Link":
            self.Access = AccessType.Link
        else:
            raise XmlFormatException("Unknown access type '%s' on Dependency: '%s'" % (access, self.Name))


class XmlGenFileUsesFeature(XmlBase):
    def __init__(self, xmlElement):
        super(XmlGenFileUsesFeature, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')


class XmlGenFileImportTemplate(XmlBase):
    def __init__(self, xmlElement):
        super(XmlGenFileImportTemplate, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')


class XmlGenFileExternalDependency(XmlBase):
    def __init__(self, xmlElement):
        super(XmlGenFileExternalDependency, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.DebugName = XmlBase._ReadAttrib(self, xmlElement, 'DebugName', False, self.Name)
        self.Include = XmlBase._ReadAttrib(self, xmlElement, 'Include', False, None)
        self.Location = XmlBase._ReadAttrib(self, xmlElement, 'Location', False, None)
        # The access type is only relevant for the include file location
        # the rest should always be included
        self.Access = AccessType.Public

        if self.Include != None:
            access = XmlBase._ReadAttrib(self, xmlElement, 'Access', True, '')
            if access == "Public":
                self.Access = AccessType.Public
            elif access == "Private":
                self.Access = AccessType.Private
            else:
                raise XmlFormatException("Unknown access type '%s' on external dependency: '%s'" % (access, self.Name))

        type = XmlBase._ReadAttrib(self, xmlElement, 'Type', True, None)
        if type == 'StaticLib':
            self.Type = ExternalDependencyType.StaticLib
        elif type == 'Headers':
            self.Type = ExternalDependencyType.Headers
        elif type == 'Find':
            self.Type = ExternalDependencyType.Find
        else:
            raise XmlException(xmlElement, "Unknown external dependency type: '%s' expected: StaticLib, Headers, Find" % type)


class XmlGenFileVariantOption(XmlBase2):
    def __init__(self, xmlElement, ownerPackageName, subPackageSupport):
        super(XmlGenFileVariantOption, self).__init__(xmlElement, subPackageSupport)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.IntroducedByPackageName = ownerPackageName


class XmlGenFileVariant(XmlBase):
    def __init__(self, xmlElement, ownerPackageName, subPackageSupport):
        super(XmlGenFileVariant, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.IntroducedByPackageName = ownerPackageName
        self.AllowExtend = XmlBase._ReadBoolAttrib(self, xmlElement, 'Extend', False, 'false')
        type = XmlBase._ReadAttrib(self, xmlElement, 'Type', False, 'Normal')
        self.Options = self.__GetXMLVariantOptions(xmlElement, ownerPackageName, subPackageSupport)
        self.Type = self.__ExtractType(type)
        self.OptionDict = self.__BuildOptionDict()
        self.__ValidateVariantName()
        self.__ValidateOptionNames()

    def __ValidateVariantName(self):
        if self.Type == VariantType.Normal:
                if not Util.IsValidName(self.Name):
                    raise XmlUnsupportedVariantNameException(self.XMLElement, self.Name)
        elif self.Type == VariantType.Virtual:
            if not Util.IsValidVirtualVariantName(self.Name):
                raise XmlUnsupportedVirtualVariantNameException(self.XMLElement, self.Name)
        else:
            raise XmlException(self.XMLElement, "Unknown variant type")


    def __ValidateOptionNames(self):
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


    def __ExtractType(self, type):
        if type == 'Normal':
            return VariantType.Normal
        elif type == 'Virtual':
            return VariantType.Virtual
        else:
            raise XmlException(self.XMLElement, "Unknown variant type: '%s' expected: Normal, Virtual" % type)


    def __BuildOptionDict(self):
        optionDict = {}
        optionNameSet = {}
        for option in self.Options:
            optionDict[option.Name] = option
            key = option.Name.lower()
            if not key in optionNameSet:
                optionNameSet[key] = option.Name
            else:
                raise XmlVariantOptionNameCollisionException(self.XMLElement, optionNameSet[key], option.Name)
        return optionDict


    def __GetXMLVariantOptions(self, elem, ownerPackageName, subPackageSupport):
        options = []
        if elem != None:
            for child in elem:
                if child.tag == 'Option':
                    options.append(XmlGenFileVariantOption(child, ownerPackageName, subPackageSupport))
        options.sort(key=lambda s: s.Name.lower())
        return options


class XmlGenFilePlatform(XmlBase2):
    def __init__(self, xmlElement, features, dependencies, variants, subPackageSupport):
        super(XmlGenFilePlatform, self).__init__(xmlElement, subPackageSupport)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.DirectUsedFeatures = features
        self.DirectDependencies = dependencies
        self.Variants = variants
        self.ProjectId = XmlBase._ReadAttrib(self, xmlElement, 'ProjectId', False, None)
        self.NotSupported = XmlBase._ReadAttrib(self, xmlElement, 'NotSupported', False, False)

        #self.DirectVariantDependencyUnion = self.__CreateVariantDependencyUnion(variants)

        if not self.Name in PackageConfig.APPROVED_PLATFORM_NAMES:
            raise XmlUnsupportedPlatformException(xmlElement, self.Name)
        # Wrong place to check this.
        # We need to know what type the project file is and we also need to know
        # if the target environment is windows
        #if self.Name == PackageConfig.PLATFORM_WINDOWS and self.ProjectId == None:
        #    raise XmlMissingWindowsVisualStudioProjectIdException(xmlElement)

    #def __CreateVariantDependencyUnion(self, variants):
    #    res = []
    #    if variants != None:
    #        for variant in variants:
    #            for option in variant.Options:
    #                for dep in option.DirectDependencies:
    #                    print("Variant dep: '%s'" % dep.Name)
    #    return res


class XmlGenFileCPPDefine(XmlBase):
    def __init__(self, xmlElement):
        super(XmlGenFileCPPDefine, self).__init__(xmlElement)
        self.Name = XmlBase._ReadAttrib(self, xmlElement, 'Name', True, '')
        self.ConsumedBy = None
        access = XmlBase._ReadAttrib(self, xmlElement, 'Access', True, '')
        if access == "Public":
            self.Access = AccessType.Public
        elif access == "Private":
            self.Access = AccessType.Private
        else:
            raise XmlFormatException("Unknown access type '%s' on CPPDefine: '%s'" % (access, self.Name))


class XmlGenFileBuildCustomization(XmlBase):
    def __init__(self, xmlElement):
        super(XmlGenFileBuildCustomization, self).__init__(xmlElement)
        self.Name = xmlElement.tag
        self.ValueString = XmlBase._ReadAttrib(self, xmlElement, 'Value', True, '')
        self.Value = self.ValueString


class XmlGenFileBuildCustomization_Optimization(XmlGenFileBuildCustomization):
    def __init__(self, xmlElement):
        super(XmlGenFileBuildCustomization_Optimization, self).__init__(xmlElement)
        if self.ValueString == "Disabled":
            self.Value = OptimizationType.Disabled
        elif self.ValueString == "Default":
            self.Value = OptimizationType.Default
        elif self.ValueString == "Full":
            self.Value = OptimizationType.Full
        else:
            raise XmlFormatException("Unknown optimization type '{0}' : '{1}'".format(self.ValueString, self.Name))



class XmlGenFile(XmlCommonFslBuild):
    def __init__(self, defaultPackageLanguage):
        super(XmlGenFile, self).__init__(None, SubPackageSupportConfig(PackageType.TopLevel, SubPackageSupport.Disabled))
        self.Name = ''
        self.ShortName = None
        self.Namespace = None
        self.PackageLocation = None
        self.Type = PackageType.Library
        self.IsVirtual = False
        self.DirectDependencies = []
        self.DirectUsedFeatures = []
        self.DirectCPPDefines = []
        self.Platforms = {}
        self.AbsolutePath = None
        self.AbsoluteIncludePath = None
        self.AbsoluteSourcePath = None
        self.AbsoluteContentPath = None
        self.AbsoluteContentSourcePath = None
        self.SourcePackageFile = None
        self.PackageLanguage = defaultPackageLanguage
        self.BaseIncludePath = "include"
        self.BaseSourcePath = "source"
        self.BuildCustomization = None

    def Load(self, config, packageTemplateLoader, packageFile):

        filename = packageFile.FileName
        if not os.path.isfile(filename):
            raise FileNotFoundException("Could not locate gen file %s", filename)

        self.PackageLocation = packageFile.PackageLocation

        tree = ET.parse(filename)
        elem = tree.getroot()
        if elem.tag != 'FslBuildGen':
            raise XmlInvalidRootElement("The file did not contain the expected root tag 'FslBuildGen'")

        theType = PackageType.Library

        currentElem = elem.find("Library")
        if currentElem == None:
            currentElem = elem.find("Executable")
            if currentElem == None:
                currentElem = elem.find("ExternalLibrary")
                if currentElem == None:
                    currentElem = elem.find("HeaderLibrary")
                    if currentElem == None:
                        raise XmlUnsupportedPackageType("Could not locate a Executable, Library, ExternalLibrary or HeaderLibrary element")
                    else:
                        theType = PackageType.HeaderLibrary
                else:
                    theType = PackageType.ExternalLibrary
            else:
                theType = PackageType.Executable
        elem = currentElem

        packageName = XmlBase._ReadAttrib(self, elem, 'Name', True, '')
        allowNoInclude = XmlBase._ReadBoolAttrib(self, elem, 'NoInclude', False, 'false')

        self.BaseIncludePath = XmlBase._ReadAttrib(self, elem, 'OverrideInclude', False, 'include')
        self.BaseSourcePath = XmlBase._ReadAttrib(self, elem, 'OverrideSource', False, 'source')
        self.AllowCombinedDirectory = XmlBase._ReadBoolAttrib(self, elem, 'AllowCombinedDirectory', False, 'false')

        self.BaseLoad(elem, SubPackageSupportConfig(theType, config.SubPackageSupport))

        features = self._GetXMLUsesFeatures(elem)
        platforms = self.__GetXMLPlatforms(elem, packageName)
        self.BuildCustomization = self.__GetBuildCustomizations(elem, packageName)

        templates = self.__GetXMLImportTemplates(elem)
        self.__ImportTemplates(packageTemplateLoader, templates, features, self.DirectDependencies, self.ExternalDependencies, self.DirectCPPDefines)

        if self.BaseIncludePath == self.BaseSourcePath and not self.AllowCombinedDirectory:
            raise XmlException2(elem, "Package '{0}' uses the same directory for include and source '{1}'".format(packageName, self.BaseIncludePath));

        self.SourcePackageFile = packageFile
        self.XMLElement = elem
        self.Name = packageName
        self.ShortName = None
        self.Namespace = None
        self.SetType(theType)
        self.Platforms = platforms
        self.DirectUsedFeatures = features
        self.AbsolutePath = None
        self.AbsoluteIncludePath = None
        self.AbsoluteSourcePath = None

        self._ValidateName(elem, self.Name)
        self.__ValidateFilename(config, filename)
        self.__ResolveNames(self.Name)
        self.__ValidateBasicDependencyCorrectness()
        self.__ValidateCPPDefines()
        self.__ResolvePaths(config, filename, allowNoInclude)
        # FIX: check for clashes with platform addition
        #      check for platform variant name clashes


    def __ResolveNames(self, name):
        index = name.rfind('.')
        if index < 0:
            self.ShortName = name
            self.Namespace = ''
        else:
            self.ShortName = name[index+1:]
            self.Namespace = name[:index]

    def SetType(self, theType):
        self.Type = theType
        self.IsVirtual = (theType == PackageType.TopLevel or theType == PackageType.ExternalLibrary or theType == PackageType.HeaderLibrary)


    def __GetXMLPlatforms(self, elem, ownerPackageName):
        platforms = {}
        for child in elem:
            if child.tag == 'Platform':
                dependencies = self._GetXMLDependencies(child)
                features = self._GetXMLUsesFeatures(child)
                variants = self.__GetXMLVariants(child, ownerPackageName)
                xmlPlatform = XmlGenFilePlatform(child, features, dependencies, variants, self.GetSubPackageSupport())
                if xmlPlatform.Name in platforms:
                    raise PlatformAlreadyDefinedException(xmlPlatform, platforms[xmlPlatform.Name]);
                platforms[xmlPlatform.Name] = xmlPlatform
        return platforms


    def __GetBuildCustomizations(self, elem, ownerPackageName):
        customizations = {}
        for child in elem:
            if child.tag == 'BuildCustomization.Debug.Optimization':
                xmlBuildCustomization = XmlGenFileBuildCustomization_Optimization(child)
                if xmlBuildCustomization.Name in customizations:
                    raise BuildCustomizationAlreadyDefinedException(xmlBuildCustomization, customizations[xmlBuildCustomization.Name]);
                customizations[xmlBuildCustomization.Name] = xmlBuildCustomization
            elif child.tag.startswith('BuildCustomization.'):
                raise UnknownBuildCustomizationException(child);

        return customizations



    def __GetXMLImportTemplates(self, elem):
        elements = []
        for child in elem:
            if child.tag == 'ImportTemplate':
                elements.append(XmlGenFileImportTemplate(child))
        return elements


    def __GetXMLVariants(self, elem, ownerPackageName):
        elements = []
        for child in elem:
            if child.tag == 'Variant':
                elements.append(XmlGenFileVariant(child, ownerPackageName, self.GetSubPackageSupport()))
        return elements


    def __ImportTemplates(self, packageTemplateLoader, templates, features, dependencies, externalDependencies, directCPPDefines):
        for template in templates:
            imported = packageTemplateLoader.Import(self, template, template.Name)
            for entry in imported.DirectUsedFeatures:
                features.append(entry)
            for entry in imported.DirectDependencies:
                dependencies.append(entry)
            for entry in imported.ExternalDependencies:
                externalDependencies.append(entry)
            for entry in imported.DirectCPPDefines:
                directCPPDefines.append(entry)

    def __ValidateFilename(self, config, filename):
        if not os.path.isabs(filename):
            raise UsageErrorException()

        if self.Name.find('.') < 0:
            containingDirName = os.path.basename(os.path.dirname(filename))
            if self.Name != containingDirName:
                raise XmlInvalidPackageNameException(self.XMLElement, containingDirName, self.Name)
        else:
            names = self.Name.split('.')
            currentDirName = filename
            # For C# we allow dir names to be the full package name as long as the 'initial' namespace isn't a valid dir name
            if self.PackageLanguage == PackageLanguage.CSharp and len(names) > 1:
                currentDirName = os.path.dirname(currentDirName)
                containingDirName = os.path.basename(currentDirName)
                if containingDirName == self.Name:
                  parentDirName = os.path.dirname(currentDirName)
                  namespaceDirName = os.path.join(parentDirName, names[0])
                  if IOUtil.IsDirectory(namespaceDirName) or IOUtil.IsDirectory(namespaceDirName.lower()):
                    raise XmlInvalidSubPackageShortNameConflictException(self.XMLElement, names[0], self.Name)
                  else:
                    # Short name was valid, so accept it
                    return
            while currentDirName != None and len(names) > 0:
              currentName = names.pop()
              currentDirName = os.path.dirname(currentDirName)
              containingDirName = os.path.basename(currentDirName)
              if currentName != containingDirName:
                raise XmlInvalidSubPackageNameException(self.XMLElement, os.path.dirname(filename), self.Name)
            if len(names) != 0:
                raise XmlInvalidSubPackageNameException(self.XMLElement, os.path.dirname(filename), self.Name)



    def __ValidateBasicDependencyCorrectness(self):
        errorMsg = "Dependency defined multiple times '%s'"
        nameDict = {}
        self.__ValidateNames(nameDict, self.DirectDependencies, errorMsg)
        for platform in self.Platforms.values():
            platformNameDict = copy.copy(nameDict)
            self.__ValidateNames(platformNameDict, platform.DirectDependencies, errorMsg)
            self.__ValidateNames(platformNameDict, platform.ExternalDependencies, errorMsg)


    def __ValidateCPPDefines(self):
        errorMsg = "CPPDefine defined multiple times '%s'"
        nameDict = {}
        self.__ValidateNames(nameDict, self.DirectCPPDefines, errorMsg)
        for platform in self.Platforms.values():
            platformNameDict = copy.copy(nameDict)
            self.__ValidateNames(platformNameDict, platform.DirectCPPDefines, errorMsg)


    def __ValidateNames(self, nameDict, dependencyList, errorStr):
        for entry in dependencyList:
            entryId = entry.Name.lower()
            if not entryId in nameDict:
                nameDict[entryId] = entry
            else:
                raise XmlException2(entry, errorStr % (entry.Name))


    def __ResolvePathIncludeDir(self, config, allowNoInclude):
        self.AbsoluteIncludePath = IOUtil.Join(self.AbsolutePath, self.BaseIncludePath)
        includeDirExist = os.path.isdir(self.AbsoluteIncludePath)
        if not includeDirExist and (os.path.exists(self.AbsoluteIncludePath) or not (allowNoInclude or config.DisableIncludeDirCheck)):
            raise PackageMissingRequiredIncludeDirectoryException(self.AbsoluteIncludePath)
        if not includeDirExist and allowNoInclude:
            self.AbsoluteIncludePath = None


    def __ResolvePaths(self, config, filename, allowNoInclude):
        if not os.path.isabs(filename):
            raise UsageErrorException()

        self.AbsolutePath = IOUtil.NormalizePath(os.path.dirname(filename))
        if not self.IsVirtual:
            sourcePath = self.BaseSourcePath
            if self.PackageLanguage == PackageLanguage.CPP:
                self.__ResolvePathIncludeDir(config, allowNoInclude)
            elif self.PackageLanguage == PackageLanguage.CSharp:
                sourcePath = self.Name
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
                


    def GetTypeString(self, type):
        if type == PackageType.Library:
            return "Library"
        elif type == PackageType.Executable:
            return "Executable"
        elif type == PackageType.ExternalLibrary:
            return "ExternalLibrary"
        elif type == PackageType.HeaderLibrary:
            return "HeaderLibrary"
        elif type == PackageType.TopLevel:
            return "TopLevel"
        else:
            raise UnknownTypeException()
