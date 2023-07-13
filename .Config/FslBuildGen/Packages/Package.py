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
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageTemplate
from FslBuildGen.Engine.Resolver.ProcessedPackageDependency import ProcessedPackageDependency
from FslBuildGen.Exceptions import InvalidDefineValueException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Location.PathBuilder import PathBuilder
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Packages.PackageBuildCustomization import PackageBuildCustomization
from FslBuildGen.Packages.PackageElement import PackageElement
from FslBuildGen.Packages.PackageGenerate import PackageGenerate
from FslBuildGen.Packages.PackageGenerateGrpcProtoFile import PackageGenerateGrpcProtoFile
#from FslBuildGen.Packages.PackageNameInfo import PackageNameInfo
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackagePlatformExternalDependency import PackagePlatformExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependencyPackageManager import UnresolvedExternalDependencyPackageManager
from FslBuildGen.Engine.Resolver.PreResolvePackageResult import PreResolvePackageResult
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageGenerate import UnresolvedPackageGenerate
from FslBuildGen.Packages.Unresolved.UnresolvedPackageGenerateGrpcProtoFile import UnresolvedPackageGenerateGrpcProtoFile
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariant import UnresolvedPackageVariant
from FslBuildGen.Packages.Unresolved.UnresolvedPackageVariantOption import UnresolvedPackageVariantOption
#from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.PackagePath import PackagePath
from FslBuildGen.Version import Version
from FslBuildGen.Xml.Exceptions import XmlException2

# We define the PackageDefine here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackageExternalDependency(PackageElement):
    def __init__(self, pathBuilder: Optional[PathBuilder],
                 base: Union[UnresolvedExternalDependency, 'PackageExternalDependency'],
                 introducedByPackageName: str, fromPackageAccess: AccessType, isAutoGenerated: bool = False) -> None:
        super().__init__(base.Name)
        self.IntroducedByPackageName = introducedByPackageName
        self.Access = base.Access  # type: AccessType
        self.IsFirstActualUse = False # type: bool
        self.ConsumedBy = base.ConsumedBy if isinstance(base, PackageExternalDependency) else None # type: Optional['Package']
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
        self.PackageManager = base.PackageManager  # type: Optional[UnresolvedExternalDependencyPackageManager]
        self.Type = base.Type  # type: ExternalDependencyType
        self.IsManaged = base.IsManaged # type: bool
        self.TargetName = base.TargetName # type: str

        # resolved paths
        self.IsAutoGenerated = isAutoGenerated  # type: bool
        self.ResolvedLocation = None  # type: Optional[ResolvedPath]

        # Do the resolve
        if pathBuilder is None:
            if isinstance(base, UnresolvedExternalDependency):
                raise Exception("UnresolvedExternalDependency does not contain a ResolvedLocation")
            self.ResolvedLocation = base.ResolvedLocation
        elif self.Location is not None:
            # The autogenerated entries might contain directories that dont exist yet, so disable the check
            self.ResolvedLocation = pathBuilder.ResolveDirectoryPath(self.Location, isAutoGenerated is False)


class Package(object):
    def __init__(self, log: Log, configBuildDir: str, preResolvePackageResult: PreResolvePackageResult) -> None:
        super().__init__()

        unresolvedPackage = preResolvePackageResult.SourcePackage

        self._Log = log
        self.SourceFileHash = unresolvedPackage.SourceFileHash
        self.TemplateType = unresolvedPackage.TemplateType
        self.AllowCheck = unresolvedPackage.Flags.AllowCheck
        self.PackageNameBasedIncludePath = unresolvedPackage.Flags.PackageNameBasedIncludePath
        self.EnableExtendedSourceExtensions = unresolvedPackage.Flags.EnableExtendedSourceExtensions    # type: bool
        self.IsUnitTest = unresolvedPackage.Flags.UnitTest                                              # type: bool
        self.ShowInMainReadme = unresolvedPackage.Flags.ShowInMainReadme
        self.TraceContext = unresolvedPackage.TraceContext
        self.CustomInfo = unresolvedPackage.CustomInfo

        # Apply condition filters to the 'read list'
        self.__FilteredExternalDependencies = unresolvedPackage.ExternalDependencies
        self.__DirectDependencies = unresolvedPackage.DirectDependencies

        # Clone all attributes that exist on a unresolvedPackage
        self.BaseIncludePath = unresolvedPackage.Path.BaseIncludePath
        self.BaseSourcePath = unresolvedPackage.Path.BaseSourcePath
        self.PackageLocation = unresolvedPackage.PackageLocation
        self.Name = unresolvedPackage.NameInfo.FullName.Value
        self.NameInfo = unresolvedPackage.NameInfo
        self.Type = unresolvedPackage.Type
        self.IsVirtual = unresolvedPackage.Flags.IsVirtual
        self.PlatformDefaultSupportedValue = unresolvedPackage.Flags.PlatformDefaultSupportedValue
        self.DirectDefines = unresolvedPackage.DirectDefines
        self.Path = unresolvedPackage.PackageFile # type: Optional[PackagePath]
        self.AbsolutePath = None if unresolvedPackage.PackageFile is None else unresolvedPackage.PackageFile.AbsoluteDirPath
        self.AbsoluteIncludePath = None if unresolvedPackage.Path.IncludePath is None else unresolvedPackage.Path.IncludePath.AbsoluteDirPath
        self.AbsoluteSourcePath = None if unresolvedPackage.Path.SourcePath is None else unresolvedPackage.Path.SourcePath.AbsoluteDirPath

        self.ContentPath = unresolvedPackage.Path.ContentPath
        self.ContentSourcePath = unresolvedPackage.Path.ContentSourcePath

        self.AbsoluteBuildPath = IOUtil.Join(self.AbsolutePath, configBuildDir) if self.AbsolutePath is not None else None
        self.AllowDependencyOnThis = self.__ResolveAllowDependencyOnThis(self.Type)
        self.PackageLanguage = unresolvedPackage.PackageLanguage
        self.BuildCustomization = PackageBuildCustomization(unresolvedPackage.BuildCustomization)
        # The year the package was created
        self.CreationYear = unresolvedPackage.CreationYear
        self.CompanyName = unresolvedPackage.CompanyName

        self.__allUnresolvedDirectDefines = unresolvedPackage.DirectDefines + unresolvedPackage.ResolvedPlatform.DirectDefines
        self.__allUnresolvedDirectRequirements = unresolvedPackage.DirectRequirements + unresolvedPackage.ResolvedPlatform.DirectRequirements
        self.__experimentalRecipe = unresolvedPackage.DirectExperimentalRecipe

        # All generate commands for the package
        self.ResolvedGenerateList = self.__ToResolvedGenerateList(self.Path, unresolvedPackage.GenerateList)
        self.ResolvedGenerateGrpcProtoFileList = self.__ToResolvedGenerateGrpcProtoFileList(self.Path, unresolvedPackage.GenerateGrpcProtoFileList)

        self.ResolvedFlavorSelections = preResolvePackageResult.SourcePackage.ResolvedFlavorSelections
        self.ResolvedFlavorTemplate = preResolvePackageResult.SourcePackage.ResolvedFlavorTemplate

        # Fill all the package attributes that will be resolved with a initial value
        self.ResolvedPlatform = unresolvedPackage.ResolvedPlatform # type: PackagePlatform
        self.ResolvedPlatformSupported = preResolvePackageResult.ResolvedPlatformSupported
        self.ResolvedPlatformDirectSupported = unresolvedPackage.DirectPlatformSupported
        self.ResolvedPlatformName = unresolvedPackage.ResolvedPlatform.Name # type: str
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
        self.ResolvedContentBuilderBuildOutputFiles = [] # type: List[str]
        # All files generated by the content builder "sync" process (content output files)
        self.ResolvedContentBuilderSyncOutputFiles = [] # type: List[str]
        # All files generated by content builder system (this includes both build files and synced files)
        # but it does not include the files in "Content" as they are not send through the ContentBuilder
        self.ResolvedContentBuilderAllOutputFiles = [] # type: List[str]

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
        # All direct feature uses of this package
        self.ResolvedDirectUsedFeatures = preResolvePackageResult.ResolvedDirectUsedFeatures
        # All feature uses both direct and indirect
        self.ResolvedAllUsedFeatures = preResolvePackageResult.ResolvedAllUsedFeatures
        # All direct requirements of this package
        self.ResolvedDirectRequirements = preResolvePackageResult.ResolvedDirectRequirements
        # All requirements both direct and indirect
        self.ResolvedAllRequirements = preResolvePackageResult.ResolvedAllRequirements
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
        # This means that while it could easily be possible for one variant to require
        # A->B->C->D and another to require D->C->B->A its simply not supported
        # since we require the global build order to stay fixed so we can do a static
        # build order setup
        #self.ResolvedDirectVariantDependencies = []
        # This is up the the build generator to set, it will be a PackageGeneratorReport object or None if the builder doesn't support it
        self.ResolvedBuildPath = None # type: Optional[str]

        # Contains about the project this package is associated with
        self.ProjectContext = unresolvedPackage.ProjectContext

        # The resolved path for the package
        packageFile = self.Path
        self.ResolvedPath = ResolvedPath(packageFile.PackageRootLocation.Name + '/' + packageFile.RootRelativeDirPath, packageFile.AbsoluteDirPath) if packageFile is not None else None

    def __repr__(self) -> str:
        return "Package({0})".format(self.Name)

    def ContainsRecipe(self) -> bool:
        return self.ResolvedDirectExperimentalRecipe is not None or self.__experimentalRecipe is not None

    def __ToResolvedGenerateList(self, packagePath: Optional[PackagePath], sourceList: List[UnresolvedPackageGenerate]) -> List[PackageGenerate]:
        if packagePath is None:
            if len(sourceList) > 0:
                raise Exception("Could not locate location of package '{0}'".format(self.Name))
            return []
        pathRelative = packagePath.RootRelativeDirPath
        pathAbsolute = packagePath.AbsoluteDirPath
        res = [] # type: List[PackageGenerate]
        for entry in sourceList:
            resolvedTemplate = ResolvedPath(IOUtil.Join(pathRelative, entry.TemplateFile), IOUtil.Join(pathAbsolute, entry.TemplateFile))
            resolvedTarget = ResolvedPath(IOUtil.Join(pathRelative, entry.TargetFile), IOUtil.Join(pathAbsolute, entry.TargetFile))

            res.append(PackageGenerate(resolvedTemplate, resolvedTarget))
        return res

    def __ToResolvedGenerateGrpcProtoFileList(self, packagePath: Optional[PackagePath], sourceList: List[UnresolvedPackageGenerateGrpcProtoFile]) -> List[PackageGenerateGrpcProtoFile]:
        if packagePath is None:
            if len(sourceList) > 0:
                raise Exception("Could not locate location of package '{0}'".format(self.Name))
            return []
        pathRelative = packagePath.RootRelativeDirPath
        pathAbsolute = packagePath.AbsoluteDirPath
        res = [] # type: List[PackageGenerateGrpcProtoFile]
        for entry in sourceList:
            resolvedInclude = ResolvedPath(entry.Include, IOUtil.Join(pathAbsolute, entry.Include))

            res.append(PackageGenerateGrpcProtoFile(resolvedInclude, entry.GrpcServices))
        return res



    def __ResolveAllowDependencyOnThis(self, packageType: PackageType) -> bool:
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
            raise XmlException2("Unknown package type: {0}".format(packageType))

    def GetDirectDependencies(self) -> List[ProcessedPackageDependency]:
        return self.__DirectDependencies

    @staticmethod
    def __ContainsDependency(dependencies: List[ProcessedPackageDependency], dependencyName: str) -> bool:
        for entry in dependencies:
            if entry.Name.Value == dependencyName:
                return True
        return False


    def GetExternalDependencies(self) -> List[UnresolvedExternalDependency]:
        return self.__FilteredExternalDependencies + self.ResolvedPlatform.ExternalDependencies


    def GetVariants(self) -> List[UnresolvedPackageVariant]:
        return self.ResolvedPlatform.Variants


    def GetUnresolvedDirectDefines(self) -> List[UnresolvedPackageDefine]:
        return self.__allUnresolvedDirectDefines


    def GetUnresolvedDirectRequirements(self) -> List[UnresolvedPackageRequirement]:
        return self.__allUnresolvedDirectRequirements


    def TryGetExperimentalRecipe(self, pathBuilder: PathBuilder, forceDisable: bool) -> Optional[PackageExperimentalRecipe]:
        return None if self.__experimentalRecipe is None else PackageExperimentalRecipe(self._Log, self.Name, pathBuilder, self.__experimentalRecipe, forceDisable)

# We define the PackageDependency here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackageDependency(object):
    def __init__(self, package: Package, access: AccessType) -> None:
        super().__init__()
        self.Package = package
        self.Access = access
        self.Name = package.Name # type: str


# We define the PackageDefine here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackageDefine(PackageElement):
    def __init__(self, base: Union[UnresolvedPackageDefine, 'PackageDefine'], introducedByPackageName: str, fromPackageAccess: AccessType) -> None:
        super().__init__(base.Name)
        self.IntroducedByPackageName = introducedByPackageName  # type: str
        self.Value = base.Value  # type: Optional[str]
        self.Access = base.Access # type: AccessType
        self.IsFirstActualUse = False  # type: bool
        self.ConsumedBy = base.ConsumedBy if isinstance(base, PackageDefine) else None # type: Optional[Package]
        # the access to the package this was received from
        self.FromPackageAccess = fromPackageAccess # type: AccessType

        if self.Value is not None and not Util.IsValidDefineValue(self.Value):
            raise InvalidDefineValueException(self.Name, self.Value)


class VariantExtensionCanNotOverwriteExistingExternalDependencyException(XmlException2):
    def __init__(self, previous: PackagePlatformExternalDependency, introducedByPackageName: str, extending: PackagePlatformExternalDependency, extensionInfo: str) -> None:
        msg = "The variant option '{0}' in package '{1}' can overwrite a existing external dependency from '{2}'".format(extending.Name, introducedByPackageName, extensionInfo)
        super().__init__(msg)


class VariantExtensionCanNotOverwriteExistingDefineException(XmlException2):
    def __init__(self, previous: PackageDefine, introducedByPackageName: str, extending: PackageDefine, extensionInfo: str) -> None:
        msg = "The variant option '{0}' in package '{1}' can overwrite a existing Define from '{2}'".format(extending.Name, introducedByPackageName, extensionInfo)
        super().__init__(msg)


class PackagePlatformVariantOption(PackageElement):
    def __init__(self, log: Log, generatorInfo: GeneratorInfo, ownerPackageName: str, base: Union[UnresolvedPackageVariantOption, 'PackagePlatformVariantOption'], allowPrivate: bool) -> None:
        super().__init__(base.Name)
        self.__Log = log
        self.__GeneratorInfo = generatorInfo
        self.IntroducedByPackageName = base.IntroducedByPackageName #  type: str
        self.ExtensionInfo = base.IntroducedByPackageName  #  type: str
        self.ExternalDependencies = self.__CloneExtDeps(base.ExternalDependencies, allowPrivate) # type: List[PackagePlatformExternalDependency]

        if isinstance(base, UnresolvedPackageVariantOption):
            if not allowPrivate:
                self.DirectDefines = [PackageDefine(entry, ownerPackageName, AccessType.Public) for entry in base.DirectDefines if entry.Access != AccessType.Private]  # type: List[PackageDefine]
            else:
                self.DirectDefines = [PackageDefine(entry, ownerPackageName, AccessType.Public) for entry in base.DirectDefines]
        else:
            if not allowPrivate:
                self.DirectDefines = [entry for entry in base.DirectDefines if entry.Access != AccessType.Private]
            else:
                self.DirectDefines = base.DirectDefines

    def __CloneExtDeps(self, externalDependencies: Union[List[UnresolvedExternalDependency], List[PackagePlatformExternalDependency]], allowPrivate: bool) -> List[PackagePlatformExternalDependency]:
        res = []  # type: List[PackagePlatformExternalDependency]
        for entry in externalDependencies:
            res.append(PackagePlatformExternalDependency(entry, allowPrivate))
        return res


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
        for i, entry in enumerate(entries):
            if entry.Name == entryName:
                return i
        return -1


class VariantNotMarkedAsExtendingException(XmlException2):
    def __init__(self, previousVariant: 'PackagePlatformVariant', extendingVariant: 'PackagePlatformVariant') -> None:
        msg = "The variant '{0}' in package '{1}' is not marked for extend, but it would be extending '{2}'".format(extendingVariant.Name, extendingVariant.IntroducedByPackageName, previousVariant.ExtensionInfo)
        super().__init__(msg)


# We define the PackageDefine here because it has a dependency to Package and having it externally
# would produce a circular dependency which does all kinds of bads things.
class PackagePlatformVariant(PackageElement):
    def __init__(self, log: Log, generatorInfo: GeneratorInfo, ownerPackageName: str, base: Union[UnresolvedPackageVariant, 'PackagePlatformVariant'], allowPrivate: bool) -> None:
        super().__init__(base.Name)
        self.__Log = log
        self.__GeneratorInfo = generatorInfo
        self.IntroducedByPackageName = base.IntroducedByPackageName  # type: str
        self.ExtensionInfo = base.IntroducedByPackageName  # type: str
        self.Options = self.__ProcessOptions(ownerPackageName, base.Options, allowPrivate)  # type: List[PackagePlatformVariantOption]
        self.Type = base.Type    # type: VariantType
        self._BuildOptionDict(self.Options)
        self.AllowExtend = base.AllowExtend  # type: bool
        self.PurifiedName = self.__PurifyName(base.Type, self.Name)

    def __PurifyName(self, variantType: VariantType, variantName: str) -> str:
        if variantType == VariantType.Virtual:
            variantName = Util.RemoveEnvironmentVariablePadding(variantName)
        return variantName


    def __ProcessOptions(self, ownerPackageName: str,
                         options: Union[List[UnresolvedPackageVariantOption], List[PackagePlatformVariantOption]],
                         allowPrivate: bool) -> List[PackagePlatformVariantOption]:
        res = []
        for entry in options:
            res.append(PackagePlatformVariantOption(self.__Log, self.__GeneratorInfo, ownerPackageName, entry, allowPrivate))
        return res

    def _BuildOptionDict(self, options: List[PackagePlatformVariantOption]) -> None:
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
        extendedVariant._BuildOptionDict(dstOptions)
        return extendedVariant


    def __IndexOf(self, entries: List[PackagePlatformVariantOption], entryName: str) -> int:
        for i, entry in enumerate(entries):
            if entry.Name == entryName:
                return i
        return -1
