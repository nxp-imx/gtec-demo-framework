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
from typing import Union
import copy
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Location.PathBuilder import PathBuilder
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Packages.FilterXmlElementUtil import FilterXmlElementUtil
from FslBuildGen.Packages.PackageBuildCustomization import PackageBuildCustomization
from FslBuildGen.Packages.PackageElement import PackageElement
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackagePlatformExternalDependency import PackagePlatformExternalDependency
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.PackageProjectContextBasePackage import PackageProjectContextBasePackage
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.Version import Version
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageConfig import APPROVED_PLATFORM_NAMES
from FslBuildGen.PackagePath import PackagePath
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Xml.Exceptions import XmlException2
from FslBuildGen.Xml.XmlBase2 import FakeXmlGenFileDependency
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlGenFile import XmlGenFile
from FslBuildGen.Xml.XmlGenFileDefine import XmlGenFileDefine
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency
from FslBuildGen.Xml.XmlGenFileExternalDependencyPackageManager import XmlGenFileExternalDependencyPackageManager
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement
from FslBuildGen.Xml.XmlStuff import FakeXmlGenFilePlatform
from FslBuildGen.Xml.XmlStuff import XmlGenFilePlatform
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariant
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariantOption
#from FslBuildGen.Packages.Package.PackageDefine import PackageDefine
#from FslBuildGen.Packages.package import PackageDefine

# We define the PackageDefine here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackageExternalDependency(PackageElement):
    def __init__(self, pathBuilder: Optional[PathBuilder],
                 base: Union[XmlGenFileExternalDependency, 'PackageExternalDependency'],
                 introducedByPackageName: str, fromPackageAccess: int, isAutoGenerated: bool = False) -> None:
        super().__init__(base.Name, base.XMLElement)
        self.IntroducedByPackageName = introducedByPackageName
        self.Access = base.Access  # type: int
        self.IsFirstActualUse = False # type: bool
        self.ConsumedBy = base.ConsumedBy # type: Optional['Package']
        # the access to the package this was received from
        self.FromPackageAccess = fromPackageAccess

        # Clone all the members of the base object
        self.DebugName = base.DebugName  # type: str
        self.Include = base.Include  # type: Optional[str]
        self.Location = base.Location  # type: Optional[str]
        self.HintPath = base.HintPath  # type: Optional[str]
        self.Version = base.Version  # type: Optional[Version]
        self.PublicKeyToken = base.PublicKeyToken  # type: Optional[str]
        self.ProcessorArchitecture = base.ProcessorArchitecture  # type: Optional[str]
        self.Culture = base.Culture  # type: Optional[str]
        self.PackageManager = base.PackageManager  # type: Optional[XmlGenFileExternalDependencyPackageManager]
        self.Type = base.Type  # type: ExternalDependencyType
        self.IsManaged = base.IsManaged # type: bool
        self.TargetName = base.TargetName # type: str

        # resolved paths
        self.IsAutoGenerated = isAutoGenerated  # type: bool
        self.ResolvedLocation = None  # type: Optional[ResolvedPath]

        # Do the resolve
        if pathBuilder is None:
            if isinstance(base, XmlGenFileExternalDependency):
                raise Exception("XmlGenFileExternalDependency does not contain a ResolvedLocation")
            self.ResolvedLocation = base.ResolvedLocation
        elif self.Location is not None:
            # The autogenerated entries might contain directories that dont exist yet, so disable the check
            self.ResolvedLocation = pathBuilder.ResolveDirectoryPath(self.Location, isAutoGenerated is False)


class Package(object):
    def __init__(self, config: Config, generatorInfo: GeneratorInfo, packageProjectContext: PackageProjectContext, genFile: XmlGenFile) -> None:
        super().__init__()

        log = config # type: Log

        self._BasicConfig = config
        self.GenFile = genFile
        self.SourceFileHash = genFile.SourceFileHash
        self.XMLElement = genFile.XMLElement
        self.TemplateType = genFile.TemplateType
        self.AllowCheck = genFile.AllowCheck
        self.PackageNameBasedIncludePath = genFile.PackageNameBasedIncludePath
        self.EnableExtendedSourceExtensions = genFile.EnableExtendedSourceExtensions    # type: bool
        self.IsUnitTest = genFile.UnitTest                                              # type: bool
        self.ShowInMainReadme = genFile.ShowInMainReadme

        # Apply condition filters to the 'read list'
        self.__FilteredExternalDependencies = FilterXmlElementUtil.FilterOnConditionsExternalDependency(log, generatorInfo, self.GenFile.ExternalDependencies)
        self.__DirectDependencies = FilterXmlElementUtil.FilterOnConditionsDependency(log, generatorInfo, self.GenFile.DirectDependencies)

        # Clone all attributes that exist on a genFile
        self.BaseIncludePath = genFile.BaseIncludePath
        self.BaseSourcePath = genFile.BaseSourcePath
        self.PackageLocation = genFile.PackageLocation
        self.Name = genFile.Name
        self.ShortName = genFile.ShortName
        self.Namespace = genFile.Namespace
        self.Type = genFile.Type
        self.IsVirtual = genFile.IsVirtual
        self.PlatformDefaultSupportedValue = genFile.PlatformDefaultSupportedValue
        #self.__DirectDependencies = genFile.DirectDependencies
        self.DirectDefines = genFile.DirectDefines
        self.Platforms = self.__BuildPlatformsDict(log, generatorInfo, genFile.Platforms)
        self.Path = genFile.PackageFile # type: Optional[PackagePath]
        self.AbsolutePath = None if genFile.PackageFile is None else genFile.PackageFile.AbsoluteDirPath
        self.AbsoluteIncludePath = None if genFile.IncludePath is None else genFile.IncludePath.AbsoluteDirPath
        self.AbsoluteSourcePath = None if genFile.SourcePath is None else genFile.SourcePath.AbsoluteDirPath

        self.ContentPath = genFile.ContentPath
        self.ContentSourcePath = genFile.ContentSourcePath

        self.AbsoluteBuildPath = IOUtil.Join(self.AbsolutePath, config.GetBuildDir()) if self.AbsolutePath is not None else None
        self.AllowDependencyOnThis = self.__ResolveAllowDependencyOnThis(self.Type)
        self.PackageLanguage = genFile.PackageLanguage
        self.BuildCustomization = PackageBuildCustomization(genFile.BuildCustomization)
        # The year the package was created
        self.CreationYear = genFile.CreationYear
        self.CompanyName = genFile.CompanyName

        # Fill all the package attributes that will be resolved with a initial value
        self.ResolvedPlatform = None  # type: Optional[PackagePlatform]
        self.ResolvedPlatformNotSupported = False
        self.ResolvedPlatformDirectSupported = True
        self.ResolvedPlatformName = None  # type: Optional[str]
        # All direct dependencies of this package
        self.ResolvedDirectDependencies = []  # type: List['PackageDependency']
        # All dependencies both direct and indirect
        self.ResolvedAllDependencies = []  # type: List['PackageDependency']
        # The build order of dependent packages
        self.ResolvedBuildOrder = []  # type: List['Package']
        # This is basically the 'resolved build order of the package' filtered down to packages that contain a experimental recipe.
        self.ResolvedExperimentalRecipeBuildOrder = []  # type: List['Package']
        self.ResolvedToolDependencyOrder = []  # type: List['Package']
        # The known recipe variants, currently used for android ABI's
        self.ResolvedRecipeVariants = []       # type: List[str]
        self.ResolvedBuildSourceFiles = None  # type: Optional[List[str]]

        # ContentBuilder input files

        # The content source files in this package (content input files that are send through the builder)
        self.ResolvedContentBuilderBuildInputFiles = None  # type: Optional[List[PathRecord]]
        # The content files in this package (content input files that are not send through the builder)
        self.ResolvedContentBuilderSyncInputFiles = None  # type: Optional[List[PathRecord]]
        # All input files to the content builder system (this includes both build files and synced files)
        # but it does not include the files in "Content" as they are not send through the ContentBuilder
        self.ResolvedContentBuilderAllInputFiles = None  # type: Optional[List[PathRecord]]

        # ContentBuilder output files

        # All files generated by the content builder "build" process (content output files)
        self.ResolvedContentBuilderBuildOuputFiles = [] # type: List[str]
        # All files generated by the content builder "sync" process (content output files)
        self.ResolvedContentBuilderSyncOutputFiles = [] # type: List[str]
        # All files generated by content builder system (this includes both build files and synced files)
        # but it does not include the files in "Content" as they are not send through the ContentBuilder
        self.ResolvedContentBuilderAllOuputFiles = [] # type: List[str]

        # Content files

        # Content files from "Content" except those generated by the content builder
        self.ResolvedContentFiles = [] # type: List[PathRecord]


        # The public include files in this package
        self.ResolvedBuildPublicIncludeFiles = None  # type: Optional[List[str]]
        # The private include files in this package
        self.ResolvedBuildPrivateIncludeFiles = None  # type: Optional[List[str]]
        # All include files in this package (public+private)
        self.ResolvedBuildAllIncludeFiles = None  # type: Optional[List[str]]
        self.ResolvedBuildAllIncludeDirs = None  # type: Optional[List[str]]
        self.ResolvedBuildPublicIncludeDirs = None  # type: Optional[List[str]]
        self.ResolvedBuildPrivateIncludeDirs = None  # type: Optional[List[str]]
        self.ResolvedBuildDirectPrivateIncludeDirs = []  # type: List[ResolvedPath]
        # Known special files that might mean something to some generators
        self.ResolvedSpecialFiles = [] # type: List[ResolvedPath]
        # All direct featue uses of this package
        self.ResolvedDirectUsedFeatures = []  # type: List[PackageRequirement]
        # All feature uses both direct and indirect
        self.ResolvedAllUsedFeatures = []  # type: List[PackageRequirement]
        # All direct requirements of this package
        self.ResolvedDirectRequirements = []  # type: List[PackageRequirement]
        # All requirements both direct and indirect
        self.ResolvedAllRequirements = []  # type: List[PackageRequirement]
        # All the direct cpp defines of this package
        self.ResolvedBuildDirectDefines = None  # type: Optional[List['PackageDefine']]
        # All the cpp defines that touch this package (direct and inherited)
        self.ResolvedBuildAllDefines = [] # type: List['PackageDefine']
        # All the public cpp defines that touch this package (direct and inherited)
        self.ResolvedBuildAllPublicDefines = None  # type: Optional[List['PackageDefine']]
        # All the private cpp defines that touch this package (direct and inherited)
        self.ResolvedBuildAllPrivateDefines = None  # type: Optional[List['PackageDefine']]
        # All direct external dependencies
        self.ResolvedDirectExternalDependencies = [] # type: List[PackageExternalDependency]
        # All the ExternalDeps that touch this package (direct and inherited)
        self.ResolvedBuildAllExternalDependencies = None  # type: Optional[List[PackageExternalDependency]]
        # All the public ExternalDeps that touch this package (direct and inherited)
        self.ResolvedBuildAllPublicExternalDependencies = None # type: Optional[List[PackageExternalDependency]]
        # All the private ExternalDeps that touch this package (direct and inherited)
        self.ResolvedBuildAllPrivateExternalDependencies = None # type: Optional[List[PackageExternalDependency]]
        self.ResolvedDirectVariants = [] # type: List['PackagePlatformVariant']
        self.ResolvedAllVariantDict = {}  # type: Dict[str, 'PackagePlatformVariant']
        self.ResolvedMakeConfigName = None  # type: Optional[str]
        self.ResolvedMakeObjectPath = None  # type: Optional[str]

        # The complete variant name containing both the normal and virtual variant names
        # in a format that is suitable for insertion into make files.
        # Beware this is just a hint and any generator is free to ignore it!
        # - each normal variant is inserted as a ${VARIANT_NAME} in the string
        # - each virtual variant is inserted as a $(VARIANT_NAME) in the string
        self.ResolvedVariantNameHint = "" # type: str

        # The complete variant name containing both the normal and virtual variant names
        # in a format that is suitable for insertion into make files.
        # Beware this is just a hint and any generator is free to ignore it!
        # - each normal variant is inserted as a $(VARIANT_NAME) in the string
        # - each virtual variant is inserted as a $(VARIANT_NAME) in the string
        self.ResolvedMakeVariantNameHint = None  # type: Optional[str]

        # The normal variant format string each variant is inserted as a ${VARIANT_NAME} in the string
        self.ResolvedNormalVariantNameHint = None  # type: Optional[str]
        # The virtual variant format string each variant is inserted as a $(VARIANT_NAME) in the string
        self.ResolvedVirtualVariantNameHint = None  # type: Optional[str]

        # A list of all normal variant names in a order that matches the one used for the name hints
        self.ResolvedNormalVariantNameList = []  # type: List[str]
        # A list of all virtual variant names in a order that matches the one used for the name hints
        self.ResolvedVirtualVariantNameList = []  # type: List[str]

        # The resolved direct experimental recipe if one exist
        self.ResolvedDirectExperimentalRecipe = None  # type: Optional[PackageExperimentalRecipe]

        # All variant dependencies (this is the dependencies pulled in by all variants)
        # Since we do a simple static evaluation of all variant dependencies that results in a fixed
        # build order for all variants we do a very strict variant dependency evaluation.
        # This means that while it could easly be possible for one variant to require
        # A->B->C->D and another to require D->C->B->A its simply not supported
        # since we require the global build order to stay fixed so we can do a static
        # build order setup
        #self.ResolvedDirectVariantDependencies = []
        # This is up the the build generator to set, it will be a PackageGeneratorReport object or None if the builder doesn't support it
        self.ResolvedBuildPath = None # type: Optional[str]

        # Contains about the project this package is associated with
        self.ProjectContext = packageProjectContext

        # The resolved path for the package
        packageFile = genFile.PackageFile
        self.ResolvedPath = ResolvedPath(packageFile.PackageRootLocation.Name + '/' + packageFile.RootRelativeDirPath, packageFile.AbsoluteDirPath) if packageFile is not None else None

    def __repr__(self) -> str:
        return "Package({0})".format(self.Name)

    def __BuildPlatformsDict(self, log: Log, generatorInfo: GeneratorInfo, platforms: Dict[str, XmlGenFilePlatform]) -> Dict[str, PackagePlatform]:
        platformDict = {} # type: Dict[str, PackagePlatform]

        # filter out any unwanted platform names
        for key, value in platforms.items():
            if key in APPROVED_PLATFORM_NAMES:
                platformDict[key] = PackagePlatform(log, generatorInfo, key, value)

        # create fake entries as needed
        for platformName in APPROVED_PLATFORM_NAMES:
            if not platformName in platformDict:
                fakeXmlGenFilePlatform = FakeXmlGenFilePlatform(self._BasicConfig, platformName, self.GenFile.SystemDefaultValues, self.GenFile.SystemSubPackageSupport)
                if self.Type == PackageType.TopLevel:
                    fakeXmlGenFilePlatform.ProjectId = 'EDC12D73-5B32-4E45-8E2E-DFC82FAD5DF4'
                platformDict[platformName] = PackagePlatform(log, generatorInfo, platformName, fakeXmlGenFilePlatform)
        return platformDict


    def ContainsRecipe(self) -> bool:
        if self.ResolvedPlatformName is None:
            raise Exception("Package has not been resolved")
        return self.ResolvedDirectExperimentalRecipe is not None or self.__TryGetExperimentaleRecipe(self.ResolvedPlatformName) is not None


    def __ResolveAllowDependencyOnThis(self, packageType: int) -> bool:
        if packageType == PackageType.Library:
            return True
        elif packageType == PackageType.Executable:
            return False
        elif packageType == PackageType.ExternalLibrary:
            return True
        elif packageType == PackageType.HeaderLibrary:
            return True
        elif packageType == PackageType.ToolRecipe:
            return True
        elif packageType == PackageType.TopLevel:
            return False
        else:
            raise XmlException2(self.XMLElement, "Unknown package type: {0}".format(packageType))


    def GetUnsupportedPlatformList(self) -> List[PackagePlatform]:
        unsupportedList = []  # type: List[PackagePlatform]
        for entry in list(self.Platforms.values()):
            if not entry.Supported:
                unsupportedList.append(entry)
        return unsupportedList


    def GetPlatform(self, platformName: str) -> PackagePlatform:
        if platformName in self.Platforms:
            return self.Platforms[platformName]
        raise Exception("Unknown platform name: '{0}'".format(platformName))


    def GetDirectDependencies(self, platformName: str) -> List[XmlGenFileDependency]:
        directDependencies = self.__DirectDependencies
        if (self.Type == PackageType.ExternalLibrary and
            platformName == PlatformNameString.ANDROID and
            PlatformUtil.DetectBuildPlatformType() == BuildPlatformType.Windows and
            self.__TryGetExperimentaleRecipe(platformName) is not None):
            ninjaRecipePackageName = self._BasicConfig.ToolConfig.CMakeConfiguration.NinjaRecipePackageName
            directDependencies += [ FakeXmlGenFileDependency(self._BasicConfig, ninjaRecipePackageName, AccessType.Public) ]
        if not platformName in self.Platforms:
            return directDependencies

        for basePackage in self.ProjectContext.BasePackages:
            if self.Name != basePackage.Name and not self.__ContainsDependency(directDependencies, basePackage.Name) and self.Type != PackageType.ToolRecipe:
                directDependencies += [ FakeXmlGenFileDependency(self._BasicConfig, basePackage.Name, AccessType.Public)]

        return directDependencies + self.Platforms[platformName].DirectDependencies

    def __ContainsDependency(self, deps: List[XmlGenFileDependency], dependencyName: str) -> bool:
        for entry in deps:
            if entry.Name == dependencyName:
                return True
        return False

    def GetExternalDependencies(self, platformName: str) -> List[XmlGenFileExternalDependency]:
        if not platformName in self.Platforms:
            return self.__FilteredExternalDependencies
        return self.__FilteredExternalDependencies + self.Platforms[platformName].ExternalDependencies


    def GetVariants(self, platformName: str) -> List[XmlGenFileVariant]:
        if not platformName in self.Platforms:
            return []
        return self.Platforms[platformName].Variants


    def GetDirectDefines(self, platformName: str) -> List[XmlGenFileDefine]:
        if not platformName in self.Platforms:
            return self.GenFile.DirectDefines
        return self.GenFile.DirectDefines + self.Platforms[platformName].DirectDefines


    def GetDirectRequirements(self, platformName: str) -> List[XmlGenFileRequirement]:
        if not platformName in self.Platforms:
            return self.GenFile.DirectRequirements
        return self.GenFile.DirectRequirements + self.Platforms[platformName].DirectRequirements


    def __TryGetExperimentaleRecipe(self, platformName: str) -> Optional[XmlExperimentalRecipe]:
        xmlExperimentalRecipe = None  # type: Optional[XmlExperimentalRecipe]
        if not platformName in self.Platforms:
            xmlExperimentalRecipe = self.GenFile.DirectExperimentalRecipe
        else:
            # Platform recipe overrides generic
            platformObject = self.Platforms[platformName]
            if platformObject.DirectExperimentalRecipe is None:
                xmlExperimentalRecipe = self.GenFile.DirectExperimentalRecipe
            else:
                # Fallback to generic
                xmlExperimentalRecipe = platformObject.DirectExperimentalRecipe
        return xmlExperimentalRecipe


    def TryGetExperimentaleRecipe(self, platformName: str, forceDisable: bool) -> Optional[PackageExperimentalRecipe]:
        xmlExperimentalRecipe = self.__TryGetExperimentaleRecipe(platformName)
        return None if xmlExperimentalRecipe is None else PackageExperimentalRecipe(self._BasicConfig, self.Name, xmlExperimentalRecipe, forceDisable)


# We define the PackageDependency here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackageDependency(object):
    def __init__(self, package: Package, access: int) -> None:
        super().__init__()
        self.Package = package
        self.Access = access
        self.Name = package.Name # type: str


# We define the PackageDefine here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackageDefine(PackageElement):
    def __init__(self, base: Union[XmlGenFileDefine, 'PackageDefine'], introducedByPackageName: str, fromPackageAccess: int) -> None:
        super().__init__(base.Name, base.XMLElement)
        self.IntroducedByPackageName = introducedByPackageName  # type: str
        self.Access = base.Access # type: int
        self.IsFirstActualUse = False  # type: bool
        self.ConsumedBy = base.ConsumedBy # type: Optional[Package]
        # the access to the package this was received from
        self.FromPackageAccess = fromPackageAccess # type: int


class VariantExtensionCanNotOverwriteExistingExternalDependencyException(XmlException2):
    def __init__(self, previous: PackagePlatformExternalDependency, introducedByPackageName: str, extending: PackagePlatformExternalDependency, extensionInfo: str) -> None:
        msg = "The variant option '{0}' in package '{1}' can overwrite a existing external dependency from '{2}'".format(extending.Name, introducedByPackageName, extensionInfo)
        super().__init__(previous.XMLElement, msg)


class VariantExtensionCanNotOverwriteExistingDefineException(XmlException2):
    def __init__(self, previous: PackageDefine, introducedByPackageName: str, extending: PackageDefine, extensionInfo: str) -> None:
        msg = "The variant option '{0}' in package '{1}' can overwrite a existing Define from '{2}'".format(extending.Name, introducedByPackageName, extensionInfo)
        super().__init__(previous.XMLElement, msg)


class PackagePlatformVariantOption(PackageElement):
    def __init__(self, log: Log, generatorInfo: GeneratorInfo, ownerPackageName: str, base: Union[XmlGenFileVariantOption, 'PackagePlatformVariantOption'], allowPrivate: bool) -> None:
        super().__init__(base.Name, base.XMLElement)
        self.__Log = log
        self.__GeneratorInfo = generatorInfo
        self.IntroducedByPackageName = base.IntroducedByPackageName #  type: str
        self.ExtensionInfo = base.IntroducedByPackageName  #  type: str
        self.ExternalDependencies = self.__CloneExtDeps(base.ExternalDependencies, allowPrivate) # type: List[PackagePlatformExternalDependency]

        if isinstance(base, XmlGenFileVariantOption):
            if not allowPrivate:
                self.DirectDefines = [PackageDefine(entry, ownerPackageName, AccessType.Public) for entry in base.DirectDefines if entry.Access != AccessType.Private]  # type: List[PackageDefine]
            else:
                self.DirectDefines = [PackageDefine(entry, ownerPackageName, AccessType.Public) for entry in base.DirectDefines]
        else:
            if not allowPrivate:
                self.DirectDefines = [entry for entry in base.DirectDefines if entry.Access != AccessType.Private]
            else:
                self.DirectDefines = base.DirectDefines

    def __CloneExtDeps(self, externalDependencies: Union[List[XmlGenFileExternalDependency], List[PackagePlatformExternalDependency]], allowPrivate: bool) -> List[PackagePlatformExternalDependency]:
        res = []  # type: List[PackagePlatformExternalDependency]
        for entry in externalDependencies:
            res.append(PackagePlatformExternalDependency(entry, allowPrivate))
        return res


#    def Extend(self, srcOption: Union[XmlGenFileVariantOption, PackagePlatformVariantOption],
    def Extend(self, srcOption: 'PackagePlatformVariantOption', extendingPackageName: str) -> 'PackagePlatformVariantOption':
        extendedOption = PackagePlatformVariantOption(self.__Log, self.__GeneratorInfo, extendingPackageName, self, False)
        extendedOption.ExtensionInfo = "{0}<-{1}".format(self.IntroducedByPackageName, extendingPackageName)

        dstExternals = extendedOption.ExternalDependencies
        srcExternals = srcOption.ExternalDependencies

        for srcExternalEntry in srcExternals:
            index = self.__IndexOf(dstExternals, srcExternalEntry.Name)
            if index >= 0:
                raise VariantExtensionCanNotOverwriteExistingExternalDependencyException(dstExternals[index], self.IntroducedByPackageName, srcExternalEntry, self.ExtensionInfo)
            else:
                dstExternals.append(srcExternalEntry)

        dstDefines = extendedOption.DirectDefines
        srcDefines = srcOption.DirectDefines  # type: List[PackageDefine]
        for srcDefineEntry in srcDefines:
            if srcDefineEntry.Access == AccessType.Public or (srcDefineEntry.Access == AccessType.Private and srcDefineEntry.IntroducedByPackageName == extendingPackageName):
                index = self.__IndexOf(dstDefines, srcDefineEntry.Name)
                if index >= 0:
                    raise VariantExtensionCanNotOverwriteExistingDefineException(dstDefines[index], self.IntroducedByPackageName, srcDefineEntry, self.ExtensionInfo)
                else:
                    dstDefines.append(srcDefineEntry)

        return extendedOption


    def __IndexOf(self, entries: Union[List[PackagePlatformExternalDependency], List[PackageDefine]], entryName: str) -> int:
        for i in range(len(entries)):
            if entries[i].Name == entryName:
                return i
        return -1


class VariantNotMarkedAsExtendingException(XmlException2):
    def __init__(self, previousVariant: 'PackagePlatformVariant', extendingVariant: 'PackagePlatformVariant') -> None:
        msg = "The variant '{0}' in package '{1}' is not marked for extend, but it would be extending '{2}'".format(extendingVariant.Name, extendingVariant.IntroducedByPackageName, previousVariant.ExtensionInfo)
        super().__init__(previousVariant.XMLElement, msg)


# We define the PackageDefine here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackagePlatformVariant(PackageElement):
    def __init__(self, log: Log, generatorInfo: GeneratorInfo, ownerPackageName: str, base: Union[XmlGenFileVariant, 'PackagePlatformVariant'], allowPrivate: bool) -> None:
        super().__init__(base.Name, base.XMLElement)
        self.__Log = log
        self.__GeneratorInfo = generatorInfo
        self.IntroducedByPackageName = base.IntroducedByPackageName  # type: str
        self.ExtensionInfo = base.IntroducedByPackageName  # type: str
        self.Options = self.__ProcessOptions(ownerPackageName, base.Options, allowPrivate)  # type: List[PackagePlatformVariantOption]
        self.Type = base.Type    # type: int
        self.__BuildOptionDict(self.Options)
        self.AllowExtend = base.AllowExtend  # type: bool
        self.PurifiedName = self.__PurifyName(base.Type, self.Name)

    def __PurifyName(self, variantType: int, variantName: str) -> str:
        if variantType == VariantType.Virtual:
            variantName = Util.RemoveEnvironmentVariablePadding(variantName)
        return variantName


    def __ProcessOptions(self, ownerPackageName: str,
                         options: Union[List[XmlGenFileVariantOption], List[PackagePlatformVariantOption]],
                         allowPrivate: bool) -> List[PackagePlatformVariantOption]:
        res = []
        for entry in options:
            res.append(PackagePlatformVariantOption(self.__Log, self.__GeneratorInfo, ownerPackageName, entry, allowPrivate))
        return res

    def __BuildOptionDict(self, options: List[PackagePlatformVariantOption]) -> None:
        optionDict = {}  # type: Dict[str, PackagePlatformVariantOption]
        for option in options:
            optionDict[option.Name] = option
        self.OptionDict = optionDict

    def Extend(self, variant: 'PackagePlatformVariant', extendingPackageName: str) -> 'PackagePlatformVariant':
        if not variant.AllowExtend:
            raise VariantNotMarkedAsExtendingException(self, variant)

        extendedVariant = PackagePlatformVariant(self.__Log, self.__GeneratorInfo, extendingPackageName, self, False)
        extendedVariant.ExtensionInfo = "{0}<-{1}".format(self.IntroducedByPackageName, extendingPackageName)

        dstOptions = extendedVariant.Options
        srcOptions = variant.Options
        for entry in srcOptions:
            index = self.__IndexOf(dstOptions, entry.Name)
            if index >= 0:
                dstOptions[index] = dstOptions[index].Extend(entry, extendingPackageName)
            else:
                raise UsageErrorException("A extending variant can not introduce new options")
        extendedVariant.__BuildOptionDict(dstOptions)
        return extendedVariant


    def __IndexOf(self, entries: List[PackagePlatformVariantOption], entryName: str) -> int:
        for i in range(len(entries)):
            if entries[i].Name == entryName:
                return i
        return -1
