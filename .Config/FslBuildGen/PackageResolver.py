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

from typing import Any
from typing import Dict
from typing import List
from typing import Set
from typing import Tuple
from typing import Optional
from typing import Union
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen import ToolSharedValues
from FslBuildGen import Util
from FslBuildGen.AndroidUtil import AndroidUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildContent.PathVariables import PathVariables
from FslBuildGen.BuildContent.Processor.ContentBuildCommandFile import ContentBuildCommandFile
from FslBuildGen.BuildContent.Processor.SourceContent import SourceContent
from FslBuildGen.BuildContent.Sync.Content import Content
#from FslBuildGen.BuildContent.Sync.Content import Content
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.State.PackageRecipeUtil import PackageRecipeUtil
#from FslBuildGen.BuildExternal.RecipePathBuilder import RecipePathBuilder
from FslBuildGen.Config import Config
from FslBuildGen.Context.PlatformContext import PlatformContext
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import BuildRecipeValidateCommand
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Exceptions import GroupedException
from FslBuildGen.Exceptions import PackageIncludeFilePathInvalidException
from FslBuildGen.Exceptions import PackageRequirementExtendsUnusedFeatureException
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Location.PathBuilder import PathBuilder
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.PackageBuilder import PackageBuilder
from FslBuildGen.Packages.ExceptionsXml import ExternalDependencyDuplicatedException
from FslBuildGen.Packages.ExceptionsXml import ExtendingVariantCanNotIntroduceNewOptionsException
from FslBuildGen.Packages.ExceptionsXml import RequirementUseDuplicatedException
from FslBuildGen.Packages.ExceptionsXml import RequirementNameCollisionException
from FslBuildGen.Packages.ExceptionsXml import VariantExtensionNotSupportedException
from FslBuildGen.Packages.ExceptionsXml import VariantNameCollisionException
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDefine
from FslBuildGen.Packages.Package import PackageExternalDependency
from FslBuildGen.Packages.Package import PackagePlatformVariant
from FslBuildGen.Packages.Package import PackagePlatformVariantOption
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.RecipeFilterManager import RecipeFilterManager
from FslBuildGen.Vars.Variable import Variable
from FslBuildGen.Xml.Exceptions import XmlMissingWindowsVisualStudioProjectIdException
from FslBuildGen.Xml.XmlGenFileExternalDependency import FakeXmlGenFileExternalDependencyHeaders
from FslBuildGen.Xml.XmlGenFileExternalDependency import FakeXmlGenFileExternalDependencyStaticLib
from FslBuildGen.Xml.XmlGenFileExternalDependency import FakeXmlGenFileExternalDependencyDLL
from FslBuildGen.Xml.XmlGenFile import XmlGenFile
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariant
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariantOption
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddHeaders
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddLib
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddDLL

class PackageResolvedInclude(object):
    def __init__(self, path: str, fromPackageAccess: int) -> None:
        super().__init__()
        self.Path = path
        # the access to the package this was received from
        self.FromPackageAccess = fromPackageAccess


class PackageResolver(object):
    def __init__(self,
                 platformContext: PlatformContext,
                 config: Config,
                 genFiles: List[XmlGenFile],
                 autoAddRecipeExternals: bool,
                 fullResolve: bool,
                 markExternalLibFirstUse: bool,
                 recipeFilterManager: RecipeFilterManager) -> None:
        """
        fullResolve
        - if this is false only the dependencies, platform, requirements and not supported will be resolved.
          This is useful for doing some filtering before doing a full resolve
        """
        super().__init__()
        logVerbosity = 1 if fullResolve else 4
        if fullResolve:
            config.LogPrintVerbose(logVerbosity, "- Processing platform: {0}".format(platformContext.PlatformName))
        else:
            config.LogPrintVerbose(logVerbosity, "- Partially processing platform: {0}".format(platformContext.PlatformName))
        try:
            config.PushIndent()
            # Do the initial dependency checks
            packageBuilder = PackageBuilder(config, platformContext.PlatformName, genFiles, logVerbosity)
            packages = packageBuilder.AllPackages

            if fullResolve:
                config.LogPrintVerbose(logVerbosity, "Resolving")
            else:
                config.LogPrintVerbose(logVerbosity, "Partial resolve")

            # FIX:
            # If we resolve the packages using the top level package build order we can do everything a lot smarter
            # than we currently do
            finalResolveOrder = packageBuilder.TopLevelPackage.ResolvedBuildOrder  # type: List[Package]
            #config.LogPrint("Order: " + ", ".join(Util.ExtractNames(finalResolveOrder)))

            self.__AutoHeaderCounter = 0  # type: int
            config.LogPrintVerbose(4, "- Platform")
            self.__ResolvePlatform(platformContext.PlatformName, platformContext.HostPlatformName, packages, config.IgnoreNotSupported)
            config.LogPrintVerbose(4, "- requirements")
            self.__ResolveBuildRequirements(config, platformContext.PlatformName, finalResolveOrder)
            config.LogPrintVerbose(4, "- not supported")
            self.__ResolveBuildPlatformNotSupported(config, platformContext.PlatformName, finalResolveOrder)

            if fullResolve:
                config.LogPrintVerbose(4, "- Recipe")
                # - Can trigger environment variables missing
                self.__ResolveExperimentalRecipe(platformContext, finalResolveOrder, recipeFilterManager, autoAddRecipeExternals)
                config.LogPrintVerbose(4, "- tool dependencies")
                self.__ResolveBuildToolDependencies(platformContext, finalResolveOrder)
                config.LogPrintVerbose(4, "- Direct externals")
                # - Can trigger environment variables missing
                self.__ResolveDirectExternalDependencies(config, platformContext, finalResolveOrder, autoAddRecipeExternals)
                config.LogPrintVerbose(4, "- Direct variants")
                self.__ResolveDirectVariants(platformContext.PlatformName, finalResolveOrder)
                config.LogPrintVerbose(4, "- Checking variants")
                self.__CheckVariants(config, finalResolveOrder)
                config.LogPrintVerbose(4, "- Resolving variants")
                self.__ResolveAllVariants(finalResolveOrder)
                if markExternalLibFirstUse:
                    self.__MarkExternalLibFirstUse(packageBuilder.TopLevelPackage)
                # Everything checks out, so we can now start resolving files for the packages
                config.LogPrintVerbose(4, "- include dirs")
                self.__ResolveBuildIncludeDirs(config, finalResolveOrder)
                config.LogPrintVerbose(4, "- source files")
                self.__ResolveBuildSourceFiles(packages)
                config.LogPrintVerbose(4, "- include files")
                self.__ResolveBuildIncludeFiles(packages)
                config.LogPrintVerbose(4, "- content files")
                self.__ResolveBuildContentFiles(config, packages)
                config.LogPrintVerbose(4, "- defines")
                self.__ResolveBuildDefines(config, platformContext.PlatformName, finalResolveOrder)
                config.LogPrintVerbose(4, "- MConfig")
                self.__ResolveMakeConfig(config, packages)
                config.LogPrintVerbose(4, "- Done")

            self.Packages = packages
        finally:
            config.PopIndent()


    def __ResolveExperimentalRecipe(self,
                                    platformContext: PlatformContext,
                                    finalResolveOrder: List[Package],
                                    recipeFilterManager: RecipeFilterManager,
                                    resolveInstallPath: bool = True) -> None:
        for package in finalResolveOrder:
            platformName = platformContext.PlatformName if package.Type != PackageType.ToolRecipe else platformContext.HostPlatformName
            forceDisable = not recipeFilterManager.IsEnabled(package.Name)
            package.ResolvedDirectExperimentalRecipe = package.TryGetExperimentaleRecipe(platformName, forceDisable)
            if package.ResolvedDirectExperimentalRecipe is not None:
                # Resolve the experimental recipe internally
                self.__ResolveExperimentalRecipeEntry(platformContext, package, package.ResolvedDirectExperimentalRecipe, resolveInstallPath)

                if package.Type != PackageType.ToolRecipe and package.ResolvedPlatform is not None and package.ResolvedPlatform.Name == PackageConfig.PlatformNameString.ANDROID:
                    if PackageRecipeUtil.ContainsBuildCMake(package.ResolvedDirectExperimentalRecipe):
                        package.ResolvedRecipeVariants = AndroidUtil.GetKnownABIList(False)


            package.ResolvedExperimentalRecipeBuildOrder = [entry for entry in package.ResolvedBuildOrder if not entry.ResolvedDirectExperimentalRecipe is None]


    def __ResolveExperimentalRecipeEntry(self, platformContext: PlatformContext, package: Package,
                                         rRecipe: PackageExperimentalRecipe, resolveInstallPath: bool) -> None:
        rRecipe.ResolvedInstallLocation = platformContext.RecipePathBuilder.TryGetInstallPath(rRecipe.XmlSource) if resolveInstallPath else ResolvedPath("NotResolved", "NotResolved")


    def __ResolveBuildToolDependencies(self,
                                       platformContext: PlatformContext,
                                       finalResolveOrder: List[Package]) -> None:
        for package in finalResolveOrder:
            package.ResolvedToolDependencyOrder = self.__ResolveBuildToolDependencyList(package.ResolvedExperimentalRecipeBuildOrder)


    def __ResolveBuildToolDependencyList(self, resolvedBuildOrder: List[Package]) -> List[Package]:
        dependencyDirList = []
        for package in resolvedBuildOrder:
            if package.Type == PackageType.ToolRecipe:
                if package.ResolvedDirectExperimentalRecipe is not None and package.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation is not None:
                    dependencyDirList.append(package)
        return dependencyDirList


    def __ResolveMakeConfig(self, config: Config, packages: List[Package]) -> None:
        for package in packages:
            normalVariants = []  # type: List[PackagePlatformVariant]
            virtualVariants = []  # type: List[PackagePlatformVariant]
            for entry in list(package.ResolvedAllVariantDict.values()):
                if entry.Type == VariantType.Normal:
                    normalVariants.append(entry)
                elif entry.Type == VariantType.Virtual:
                    virtualVariants.append(entry)
                else:
                    raise NotImplementedError("unknown variant type")

            # sort by name to ensure that the ordering is fixed
            normalVariants.sort(key=lambda s: s.Name.lower())
            virtualVariants.sort(key=lambda s: s.Name.lower())

            if len(normalVariants) > 0 or len(virtualVariants) > 0:
                # NOTE: using unqualifying variant names is kind of dangerous as they could conflict with the build system
                #       but changing it now could break a lot of things.
                #       normalVariants = ["VARIANT_{0}".format(name) for name in normalVariants]
                #       virtualVariants = ["VARIANT_{0}".format(name) for name in virtualVariants]
                normalVariableVariantNames = Util.ExtractNamesAsVariables(normalVariants)
                normalMakeVariantNames = Util.ExtractNamesAsMakeEnvironmentVariables(normalVariants)
                virtualMakeVariantNames = Util.ExtractNamesAsMakeEnvironmentVariables(virtualVariants)

                strVariableNormal = "" if len(normalVariants) <= 0 else "_{0}".format("_".join(normalVariableVariantNames))
                strMakeNormal = "" if len(normalVariants) <= 0 else "_{0}".format("_".join(normalMakeVariantNames))
                strMakeVirtual = "" if len(virtualVariants) <= 0 else "_{0}".format("_".join(virtualMakeVariantNames))

                package.ResolvedNormalVariantNameHint = strVariableNormal
                package.ResolvedVirtualVariantNameHint = strMakeVirtual
                # the variant name string in a format that is suitable for insertion into make files
                package.ResolvedVariantNameHint = strVariableNormal + strMakeVirtual
                package.ResolvedMakeVariantNameHint = strMakeNormal + strMakeVirtual
                package.ResolvedMakeConfigName = "{0}{1}".format(package.ResolvedPlatformName, package.ResolvedMakeVariantNameHint)
                package.ResolvedNormalVariantNameList = [variant.Name for variant in normalVariants]
                package.ResolvedVirtualVariantNameList = [variant.Name for variant in virtualVariants]
            else:
                package.ResolvedMakeConfigName = package.ResolvedPlatformName
                package.ResolvedMakeVariantNameHint = ""
                package.ResolvedNormalVariantNameHint = ""
                package.ResolvedVirtualVariantNameHint = ""
                package.ResolvedNormalVariantNameList = []
                package.ResolvedVirtualVariantNameList = []
            package.ResolvedBuildPath = "{0}/{1}".format(config.GetBuildDir(), package.ResolvedPlatformName)
            package.ResolvedMakeObjectPath = "{0}/obj{1}/$(config)".format(package.ResolvedBuildPath, package.ResolvedMakeVariantNameHint)


    def __GetExtensions(self, packageLanguage: int, enableExtendedExtensions: bool) -> Union[str, Tuple[str, ...]]:
        if packageLanguage == PackageLanguage.CPP:
            if not enableExtendedExtensions:
                return (".cpp", ".c")
            else:
                return (".cpp", ".c", ".cc")
        elif packageLanguage == PackageLanguage.CSharp:
            return ".cs"
        else:
            raise UnsupportedException("Unsupported package language {0}".format(PackageLanguage.ToString(packageLanguage)))


    def __ResolveBuildSourceFiles(self, packages: List[Package]) -> None:
        for package in packages:
            if not package.IsVirtual:
                if package.AbsolutePath is None or package.AbsoluteSourcePath is None:
                    raise Exception("package in invalid")
                startIdx = len(package.AbsolutePath) + 1
                languageSourceExtensions = self.__GetExtensions(package.PackageLanguage, package.EnableExtendedSourceExtensions)
                files = IOUtil.GetFilePaths(package.AbsoluteSourcePath, languageSourceExtensions)
                files = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in files]
                files.sort(key=lambda s: s.lower())
                package.ResolvedBuildSourceFiles = files
            else:
                package.ResolvedBuildSourceFiles = []

    def __RemoveDuplicatedFiles(self, filesSourceHeaders: List[str], files: List[str]) -> List[str]:
        result = list(filesSourceHeaders)
        for file in files:
            if file in result:
                result.remove(file)
        return result


    def __HasPathOverlap(self, path1: Optional[str], path2: Optional[str]) -> bool:
        if path1 == path2:
            return True
        elif path1 is None or path2 is None:
            return False
        elif len(path1) < len(path2) and path2.startswith(path1):
            return True
        elif len(path1) > len(path2) and path1.startswith(path2):
            return True
        return False

    def __ValidateIncludePaths(self, package: Package, files: List[str]) -> None:
        # skip the base include path name and its slash
        strip = len(package.BaseIncludePath) + 1
        packageNameAsDir = package.Name.replace('.','/') + '/'
        errors = None # type: Optional[List[Exception]]
        for filename in files:
            baseFilename = filename[strip:]
            if not baseFilename.startswith(packageNameAsDir):
                if errors is None:
                    errors = []
                errors.append(PackageIncludeFilePathInvalidException(package.Name, baseFilename, packageNameAsDir))
        if errors is not None:
            raise GroupedException(errors)

    def __ResolveBuildIncludeFiles(self, packages: List[Package]) -> None:
        for package in packages:
            if not package.IsVirtual or package.Type == PackageType.HeaderLibrary:
                if package.AbsolutePath is None:
                    raise Exception("invalid package")
                startIdx = len(package.AbsolutePath) + 1
                filesPub = [] if package.AbsoluteIncludePath is None else IOUtil.GetFilePaths(package.AbsoluteIncludePath, (".hpp", ".h", ".inl"))
                filesPri = [] # type: List[str]
                filesAll = list(filesPub)
                if package.AbsoluteSourcePath:
                    filesSourceHeaders = IOUtil.GetFilePaths(package.AbsoluteSourcePath, (".hpp", ".h", ".inl"))
                    if self.__HasPathOverlap(package.AbsoluteIncludePath, package.AbsoluteSourcePath):
                        filesSourceHeaders = self.__RemoveDuplicatedFiles(filesSourceHeaders, filesPub)
                    filesPri += filesSourceHeaders
                    filesAll += filesSourceHeaders


                filesPub = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in filesPub]
                filesPri = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in filesPri]
                filesAll = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in filesAll]
                filesPub.sort(key=lambda s: s.lower())
                filesPri.sort(key=lambda s: s.lower())
                filesAll.sort(key=lambda s: s.lower())
                package.ResolvedBuildPublicIncludeFiles = filesPub
                package.ResolvedBuildPrivateIncludeFiles = filesPri
                package.ResolvedBuildAllIncludeFiles = filesAll
                if package.PackageNameBasedIncludePath:
                    self.__ValidateIncludePaths(package, filesPub)
            else:
                package.ResolvedBuildPublicIncludeFiles = []
                package.ResolvedBuildAllIncludeFiles = []


    def __ResolveBuildContentFiles(self, basicConfig: Config, packages: List[Package]) -> None:
        for package in packages:
            if not package.IsVirtual:
                sourceContent = self.__ProcesssContentCommandFile(basicConfig, package)

                package.ResolvedContentBuilderBuildInputFiles = list(sourceContent.ContentBuildSource.Files)
                package.ResolvedContentBuilderSyncInputFiles = list(sourceContent.ContentSource.Files)
                package.ResolvedContentBuilderAllInputFiles = list(sourceContent.AllContentSource.Files)

                package.ResolvedContentBuilderBuildOuputFiles = []
                if len(package.ResolvedContentBuilderBuildInputFiles) > 0 and package.Path is not None:
                    # Run through all files that will be build and determine what the output filename will be
                    featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
                    contentProcessorManager = ContentBuilder.GetContentProcessorManager(basicConfig, featureList)
                    for contentEntry in package.ResolvedContentBuilderBuildInputFiles:
                        contentProcessor = contentProcessorManager.TryFindContentProcessor(contentEntry)
                        if contentProcessor is not None:
                            packageContentOutputPath = ContentBuilder.GetContentOutputPath(package.Path)
                            contentOutputFile = contentProcessor.GetOutputFileName(basicConfig, packageContentOutputPath, contentEntry)
                            package.ResolvedContentBuilderBuildOuputFiles.append(contentOutputFile)
                    package.ResolvedContentBuilderBuildOuputFiles.sort()

                # Run through all sync files and determine their output filename
                package.ResolvedContentBuilderSyncOutputFiles = []
                if len(package.ResolvedContentBuilderSyncInputFiles) > 0 and package.Path is not None:
                    packageContentOutputPath = ContentBuilder.GetContentOutputPath(package.Path)
                    syncDstRoot = ContentBuilder.GetContentOuputContentRootRecord(basicConfig, packageContentOutputPath)
                    for syncContentEntry in package.ResolvedContentBuilderSyncInputFiles:
                        syncContentOutputFile = ContentBuilder.GetContentSyncOutputFilename(basicConfig, syncDstRoot, syncContentEntry)
                        package.ResolvedContentBuilderSyncOutputFiles.append(syncContentOutputFile.ResolvedPath)
                    package.ResolvedContentBuilderSyncOutputFiles.sort()

                # Build the union of the sync files + build files
                package.ResolvedContentBuilderAllOuputFiles = package.ResolvedContentBuilderBuildOuputFiles + package.ResolvedContentBuilderSyncOutputFiles
                package.ResolvedContentBuilderAllOuputFiles.sort()

                # Finally resolve the actual content files from the "Content" directory (excluding any files generated by the content builder)
                package.ResolvedContentFiles = self.__ProcesssContentFiles(basicConfig, package, package.ResolvedContentBuilderAllOuputFiles)

            else:
                package.ResolvedContentBuilderBuildInputFiles = []
                package.ResolvedContentBuilderSyncInputFiles = []
                package.ResolvedContentBuilderAllInputFiles = []
                package.ResolvedContentBuilderBuildOuputFiles = []
                package.ResolvedContentBuilderSyncOutputFiles = []
                package.ResolvedContentBuilderAllOuputFiles = []
                package.ResolvedContentFiles = []


    def __ProcesssContentFiles(self, log: Log, package: Package, resolvedContentBuilderAllOuputFiles: List[str]) -> List[PathRecord]:
        if package.ContentPath is None or not IOUtil.IsDirectory(package.ContentPath.AbsoluteDirPath):
            return []

        absContentPath = package.ContentPath.AbsoluteDirPath
        content = Content(log, absContentPath, True)
        if len(content.Files) <= 0:
            return []

        generatedContentSet = set(resolvedContentBuilderAllOuputFiles)
        # Blacklist the content sync cache
        generatedContentSet.add(IOUtil.Join(absContentPath, ToolSharedValues.CONTENT_SYNC_CACHE_FILENAME))
        res = [entry for entry in content.Files if entry.ResolvedPath not in generatedContentSet]
        return res

    def __ProcesssContentCommandFile(self, config: Config, package: Package) -> SourceContent:
        if package.AbsoluteBuildPath is None or package.ContentSourcePath is None or package.ContentPath is None:
            raise Exception("Invalid package")
        pathVariables = PathVariables(config, package.AbsoluteBuildPath, package.ContentSourcePath.AbsoluteDirPath, package.ContentPath.AbsoluteDirPath)
        commandFilename = IOUtil.Join(package.ContentSourcePath.AbsoluteDirPath, ToolSharedValues.CONTENT_BUILD_FILE_NAME)
        commands = ContentBuildCommandFile(config, commandFilename, pathVariables)
        return SourceContent(config, package.ContentPath.AbsoluteDirPath, package.ContentSourcePath.AbsoluteDirPath, commands, False)


    def __AddBuildIncludeDir(self, srcDir: str,
                             currentAccess: int,
                             fromPackageAccess: int,
                             rIncludeDirs: Dict[str, PackageResolvedInclude],
                             rPrivateIncludeDirs: List[str],
                             rPublicIncludeDirs: List[str]) -> None:
        rIncludeDirs[srcDir] = PackageResolvedInclude(srcDir, fromPackageAccess)
        if currentAccess == AccessType.Private:
            rPrivateIncludeDirs.append(srcDir)
        else:
            rPublicIncludeDirs.append(srcDir)


    def __RemoveBuildIncludeDir(self, resolvedDir: PackageResolvedInclude,
                                rIncludeDirs: Dict[str, PackageResolvedInclude],
                                rPrivateIncludeDirs: List[str],
                                rPublicIncludeDirs: List[str]) -> None:
        rIncludeDirs.pop(resolvedDir.Path, None)
        if resolvedDir.Path in rPrivateIncludeDirs:
            rPrivateIncludeDirs.remove(resolvedDir.Path)
        if resolvedDir.Path in rPublicIncludeDirs:
            rPublicIncludeDirs.remove(resolvedDir.Path)


    def __ResolveBuildIncludeDirs(self, config: Config, finalResolveOrder: List[Package]) -> None:
        for package in finalResolveOrder:
            hasLocalIncludeDir = False
            includeDirs = {}  # type: Dict[str, PackageResolvedInclude]
            publicIncludeDirs = [] # type: List[str]
            privateIncludeDirs = [] # type: List[str]
            directPrivateIncludeDirs = [] # type: List[ResolvedPath]

            # First process the include paths present in this package
            if package.AbsoluteIncludePath != None:
                hasLocalIncludeDir = True
            for extDependency in package.ResolvedDirectExternalDependencies:
                if extDependency.Include != None:
                    if extDependency.Include in includeDirs:
                        raise Exception("External include dir defined multiple times: '{0}'".format(extDependency.Include))
                    if extDependency.Include is None:
                        raise Exception("Invalid external dependency")
                    self.__AddBuildIncludeDir(extDependency.Include, extDependency.Access, AccessType.Public, includeDirs, privateIncludeDirs, publicIncludeDirs)

            # Then pull in the dependencies from the packages we depend upon
            # here we take advantage of the fact that all packages we are dependent upon
            # have been resolved.
            for dep in package.ResolvedDirectDependencies:
                if dep.Access != AccessType.Link:
                    if dep.Package.ResolvedBuildPublicIncludeDirs is None:
                        raise Exception("Invalid package")
                    for dirEntry in dep.Package.ResolvedBuildPublicIncludeDirs:
                        if dirEntry == dep.Package.BaseIncludePath:
                            dirEntry = self.__ExtractIncludePath(config, dep.Package)
                        if dirEntry not in includeDirs:
                            self.__AddBuildIncludeDir(dirEntry, dep.Access, dep.Access, includeDirs, privateIncludeDirs, publicIncludeDirs)
                        elif dep.Access < includeDirs[dirEntry].FromPackageAccess:
                            self.__RemoveBuildIncludeDir(includeDirs[dirEntry], includeDirs, privateIncludeDirs, publicIncludeDirs)
                            self.__AddBuildIncludeDir(dirEntry, dep.Access, dep.Access, includeDirs, privateIncludeDirs, publicIncludeDirs)

            # If this is a unit test package 'allow access to the source files as a include path' this is done to allow access to private headers
            if package.IsUnitTest:
                for parentPackage in package.ResolvedBuildOrder:
                    if parentPackage.Name == package.Namespace and parentPackage.AbsoluteSourcePath is not None:
                        sourceDir = self.__ExtractSourcePath(config, parentPackage)
                        if config.Verbosity >= 4:
                            config.LogPrint("  Adding source {0} as include for unit test {1}".format(sourceDir, package.Name))
                        self.__AddBuildIncludeDir(sourceDir, AccessType.Private, AccessType.Private, includeDirs, privateIncludeDirs, publicIncludeDirs)
                        directPrivateIncludeDirs.append(ResolvedPath(sourceDir, parentPackage.AbsoluteSourcePath))

            allIncludeDirs = list(includeDirs.keys())
            allIncludeDirs.sort(key=lambda s: s.lower())
            publicIncludeDirs.sort(key=lambda s: s.lower())
            privateIncludeDirs.sort(key=lambda s: s.lower())
            directPrivateIncludeDirs.sort(key=lambda s: s.ResolvedPath.lower())
            if hasLocalIncludeDir:
                allIncludeDirs.insert(0, package.BaseIncludePath)
                publicIncludeDirs.insert(0, package.BaseIncludePath)
            package.ResolvedBuildPublicIncludeDirs = publicIncludeDirs
            package.ResolvedBuildPrivateIncludeDirs = privateIncludeDirs
            package.ResolvedBuildAllIncludeDirs = allIncludeDirs
            package.ResolvedBuildDirectPrivateIncludeDirs = directPrivateIncludeDirs


    def __ResolveBuildDefines(self, config: Config, platformName: str, finalResolveOrder: List[Package]) -> None:
        # We proces the packages in the correct resolve order
        # This ensure that all dependencies have been processed
        for package in finalResolveOrder:
            allDefines = {}  # type: Dict[str, PackageDefine]
            publicDefines = []  # type: List[PackageDefine]
            privateDefines = []  # type: List[PackageDefine]

            # Find the direct cpp defines in this package
            depDefine = package.GetDirectDefines(platformName)
            for define in depDefine:
                processedDefine = PackageDefine(define, package.Name, AccessType.Public)
                if not package.IsVirtual:
                    processedDefine.ConsumedBy = package
                    processedDefine.IsFirstActualUse = True
                allDefines[processedDefine.Name] = processedDefine
                if define.Access == AccessType.Public:
                    publicDefines.append(processedDefine)
                elif define.Access == AccessType.Private: # and depPackage.Name == package.Name:
                    privateDefines.append(processedDefine)

            package.ResolvedBuildDirectDefines = list(allDefines.values())

            directDefines = dict(allDefines)
            # Add the cpp defines from all direct dependencies
            for depPackage in package.ResolvedDirectDependencies:
                if depPackage.Access != AccessType.Link:
                    if depPackage.Package.ResolvedBuildAllPublicDefines is None:
                        raise Exception("Invalid package")
                    for entry in depPackage.Package.ResolvedBuildAllPublicDefines:
                        processedDefine = PackageDefine(entry, entry.IntroducedByPackageName, depPackage.Access)
                        if not processedDefine.Name in allDefines:
                            self.__ResolveAdd(package, processedDefine, allDefines, publicDefines, privateDefines)
                        elif processedDefine.Name in directDefines:
                            raise UsageErrorException("Define: %s was already defined by %s" % (processedDefine.Name, processedDefine.IntroducedByPackageName))
                        elif processedDefine.FromPackageAccess < allDefines[processedDefine.Name].FromPackageAccess:
                            # We have access to the define but at more open access level, so adopt that instead
                            self.__ResolveRemove(allDefines[processedDefine.Name], allDefines, publicDefines, privateDefines)
                            self.__ResolveAdd(package, processedDefine, allDefines, publicDefines, privateDefines)

            allDefinesList = list(allDefines.values())
            allDefinesList.sort(key=lambda s: s.Name.lower())
            publicDefines.sort(key=lambda s: s.Name.lower())
            privateDefines.sort(key=lambda s: s.Name.lower())
            package.ResolvedBuildAllDefines = allDefinesList
            package.ResolvedBuildAllPublicDefines = publicDefines
            package.ResolvedBuildAllPrivateDefines = privateDefines


    def __TryLocateRequiredFeature(self, allUsedRequirements: List[PackageRequirement], name: str) -> Optional[PackageRequirement]:
        for requirement in allUsedRequirements:
            if requirement.Type == PackageRequirementTypeString.Feature and requirement.Name == name:
                return requirement
        return None


    def __ValidateRequirementExtends(self, config: Config, platformName: str, finalResolveOrder: List[Package]) -> None:
        errors = []
        for package in finalResolveOrder:
            for requirement in package.ResolvedDirectRequirements:
                if requirement.Type != PackageRequirementTypeString.Feature or len(requirement.Extends) != 0:
                    parent = self.__TryLocateRequiredFeature(package.ResolvedAllRequirements, requirement.Extends)
                    if parent is None:
                        errors.append((package, requirement))
        if len(errors) > 0:
            exceptionList = [] # type: List[Exception]
            for error in errors:
                exceptionList.append(PackageRequirementExtendsUnusedFeatureException(error[1].Name, error[1].Extends, error[0].Name))
            raise GroupedException(exceptionList)


    def __ResolveBuildRequirements(self, config: Config, platformName: str, finalResolveOrder: List[Package]) -> None:
        for package in finalResolveOrder:
            uniqueDict = {}  # type: Dict[str, Set[str]]
            # Resolve the direct used requirements
            # package.ResolvedDirectRequirements
            for requirement1 in package.GetDirectRequirements(platformName):
                packageRequirement = PackageRequirement(requirement1, package.Name)
                uniqueSet = uniqueDict[packageRequirement.GroupId] if packageRequirement.GroupId in uniqueDict else set()
                if packageRequirement.Id in uniqueSet:
                    raise RequirementUseDuplicatedException(package, requirement1)
                package.ResolvedDirectRequirements.append(packageRequirement)

                uniqueSet.add(packageRequirement.Id)
                if len(uniqueSet) == 1:
                    uniqueDict[packageRequirement.GroupId] = uniqueSet
            # extract feature requirements
            package.ResolvedDirectUsedFeatures = [requirement for requirement in package.ResolvedDirectRequirements if requirement.Type == PackageRequirementTypeString.Feature]

            # Resolve all used requirements
            # package.ResolvedAllRequirements
            uniqueRequirementFullIds = {}  # type: Dict[str, PackageRequirement]
            fullIdDict = {}  # type: Dict[str, Tuple[Package, str]]
            for depPackage in package.ResolvedDirectDependencies:
                for requirement2 in depPackage.Package.ResolvedAllRequirements:
                    if not requirement2.FullId in uniqueRequirementFullIds:
                        # ensure that we dont have two requirement names with different casing
                        if requirement2.FullId in fullIdDict:
                            raise RequirementNameCollisionException(depPackage.Package, requirement2.Name, fullIdDict[requirement2.FullId][0], fullIdDict[requirement2.FullId][1])
                        fullIdDict[requirement2.FullId] = (depPackage.Package, requirement2.FullId)
                        package.ResolvedAllRequirements.append(requirement2)
                        uniqueRequirementFullIds[requirement2.FullId] = requirement2
                    else:
                        for introPackageName in requirement2.IntroducedByPackages:
                            uniqueIntroPackage = uniqueRequirementFullIds[requirement2.FullId]
                            if not introPackageName in uniqueIntroPackage.IntroducedByPackages:
                                uniqueIntroPackage.IntroducedByPackages.add(introPackageName)

            # Add this package's direct used requirements to
            # package.ResolvedAllRequirements
            for requirement3 in package.ResolvedDirectRequirements:
                if not requirement3.FullId in uniqueRequirementFullIds:
                    # ensure that we dont have two requirement names with different casing
                    if requirement3.FullId in fullIdDict:
                        raise RequirementNameCollisionException(package, requirement3.Name, fullIdDict[requirement3.FullId][0], fullIdDict[requirement3.FullId][1])
                    fullIdDict[requirement3.FullId] = (package, requirement3.FullId)
                    package.ResolvedAllRequirements.append(requirement3)
                    uniqueRequirementFullIds[requirement3.FullId] = requirement3
                else:
                    for introPackageName in requirement3.IntroducedByPackages:
                        uniqueIntroPackage = uniqueRequirementFullIds[requirement3.FullId]
                        if not introPackageName in uniqueIntroPackage.IntroducedByPackages:
                            uniqueIntroPackage.IntroducedByPackages.add(introPackageName)

            package.ResolvedAllRequirements.sort(key=lambda s: s.Id)
            package.ResolvedAllUsedFeatures = [requirement for requirement in package.ResolvedAllRequirements if requirement.Type == PackageRequirementTypeString.Feature]
        self.__ValidateRequirementExtends(config, platformName, finalResolveOrder)


    def __ResolveBuildPlatformNotSupported(self, config: Config, platformName: str, finalResolveOrder: List[Package]) -> None:
        for package in finalResolveOrder:
            notSupported = package.ResolvedPlatformDirectSupported != True
            if not notSupported:
                # Ensure that if one package dependency is marked as NotSupported
                # this package gets marked as not supported too
                for depPackage in package.ResolvedDirectDependencies:
                    if depPackage.Package.ResolvedPlatformNotSupported:
                        notSupported = True
            package.ResolvedPlatformNotSupported = notSupported


    def __ExtractIncludePath(self, config: Config, package: Package) -> str:
        if package.AbsoluteIncludePath is None:
            raise Exception("Invalid package")
        path = config.ToPath(package.AbsoluteIncludePath)
        return Util.UTF8ToAscii(path)

    def __ExtractSourcePath(self, config: Config, package: Package) -> str:
        if package.AbsoluteSourcePath is None:
            raise Exception("Invalid package")
        path = config.ToPath(package.AbsoluteSourcePath)
        return Util.UTF8ToAscii(path)


    def __ResolveAdd(self, package: Package, processedEntry: Any, rAllDict: Dict[str, Any],
                     rPublicList: List[Any], rPrivateList: List[Any]) -> None:
        if processedEntry.Name in rAllDict:
            raise Exception("Usage error {0} already exist in the list. Entries can not be duplicated, please remove it first.".format(processedEntry.Name))

        rAllDict[processedEntry.Name] = processedEntry
        if processedEntry.FromPackageAccess == AccessType.Private:
            rPrivateList.append(processedEntry)
        else:
            rPublicList.append(processedEntry)
        # If the entry has'nt been marked as consumed yet and this package isn't virtual then consume it
        if processedEntry.ConsumedBy is None and not package.IsVirtual:
            processedEntry.ConsumedBy = package
            processedEntry.IsFirstActualUse = True


    def __ResolveRemove(self, processedEntry: Any, rAllDict: Dict[str, Any],
                        rPublicList: List[Any], rPrivateList: List[Any]) -> None:
        if processedEntry in rPrivateList:
            rPrivateList.remove(processedEntry)
        if processedEntry in rPublicList:
            rPublicList.remove(processedEntry)
        rAllDict.pop(processedEntry.Name, None)


    def __CreateExternalHeadersDependency(self, basicConfig: BasicConfig,
                                          pathBuilder: PathBuilder,
                                          package: Package,
                                          sourceRecipe: PackageExperimentalRecipe,
                                          command: XmlRecipeValidateCommandAddHeaders) -> PackageExternalDependency:
        """ Automatically generate a external dependency to header files just as if it had been in the original source.
        """
        if IOUtil.IsAbsolutePath(command.Name):
            raise Exception("Path can not be absolute '{0}".format(command.Name))

        if sourceRecipe.ResolvedInstallLocation is None:
            raise Exception("sourceRecipe is missing the expected ResolvedInstallLocation")

        resolvedLocation = sourceRecipe.ResolvedInstallLocation
        sourcePath = resolvedLocation.SourcePath if resolvedLocation.SourcePath.startswith("$(") else resolvedLocation.ResolvedPath

        if package.Type == PackageType.ExternalLibrary and len(package.ResolvedRecipeVariants) > 0:
            sourcePath = IOUtil.Join(sourcePath, Variable.RecipeVariant)
        absoluteLocation = IOUtil.Join(sourcePath, command.Name)

        self.__AutoHeaderCounter = self.__AutoHeaderCounter + 1
        fakeEntry = FakeXmlGenFileExternalDependencyHeaders(basicConfig, "AutoHeaders{0}".format(self.__AutoHeaderCounter), absoluteLocation,
                                                            AccessType.Public, True)
        return PackageExternalDependency(pathBuilder, fakeEntry, package.Name, AccessType.Public, True)


    def __CreateExternalStaticLibDependency(self, basicConfig: BasicConfig,
                                            pathBuilder: PathBuilder,
                                            package: Package,
                                            sourceRecipe: PackageExperimentalRecipe,
                                            command: XmlRecipeValidateCommandAddLib) -> PackageExternalDependency:
        """ Automatically generate a external dependency to the StaticLib just as if it had been in the original source.
        """
        srcLocation = IOUtil.GetDirectoryName(command.Name)
        srcDebugLocation = IOUtil.GetDirectoryName(command.DebugName)

        if srcLocation != srcDebugLocation:
            raise Exception("We currently require that the source and debug lib reside at the same location '{0}' and '{1}', due to limits in external dependencies".format(srcLocation, srcDebugLocation))

        if IOUtil.IsAbsolutePath(srcLocation):
            raise Exception("Path can not be absolute '{0}".format(srcLocation))
        if IOUtil.IsAbsolutePath(srcDebugLocation):
            raise Exception("Path can not be absolute '{0}".format(srcDebugLocation))

        srcName = IOUtil.GetFileName(command.Name)
        srcDebugName = IOUtil.GetFileName(command.DebugName)

        if sourceRecipe.ResolvedInstallLocation is None:
            raise Exception("sourceRecipe is missing the expected ResolvedInstallLocation")

        resolvedLocation = sourceRecipe.ResolvedInstallLocation
        sourcePath = resolvedLocation.SourcePath if resolvedLocation.SourcePath.startswith("$(") else resolvedLocation.ResolvedPath
        if package.Type == PackageType.ExternalLibrary and len(package.ResolvedRecipeVariants) > 0:
            sourcePath = IOUtil.Join(sourcePath, Variable.RecipeVariant)
        absoluteLocation = IOUtil.Join(sourcePath, srcLocation)

        fakeEntry = FakeXmlGenFileExternalDependencyStaticLib(basicConfig, srcName, srcDebugName, absoluteLocation, AccessType.Public, True)
        return PackageExternalDependency(pathBuilder, fakeEntry, package.Name, AccessType.Public, True)


    def __CreateExternalDLLDependency(self, basicConfig: BasicConfig,
                                      pathBuilder: PathBuilder,
                                      package: Package,
                                      sourceRecipe: PackageExperimentalRecipe,
                                      command: XmlRecipeValidateCommandAddDLL) -> PackageExternalDependency:
        """ Automatically generate a external dependency to the DLL just as if it had been in the original source.
        """
        srcLocation = IOUtil.GetDirectoryName(command.Name)
        srcDebugLocation = IOUtil.GetDirectoryName(command.DebugName)

        if srcLocation != srcDebugLocation:
            raise Exception("We currently require that the source and debug DLL reside at the same location '{0}' and '{1}', due to limits in external dependencies".format(srcLocation, srcDebugLocation))

        if IOUtil.IsAbsolutePath(srcLocation):
            raise Exception("Path can not be absolute '{0}".format(srcLocation))
        if IOUtil.IsAbsolutePath(srcDebugLocation):
            raise Exception("Path can not be absolute '{0}".format(srcDebugLocation))

        srcName = IOUtil.GetFileName(command.Name)
        srcDebugName = IOUtil.GetFileName(command.DebugName)

        if sourceRecipe.ResolvedInstallLocation is None:
            raise Exception("sourceRecipe is missing the expected ResolvedInstallLocation")

        resolvedLocation = sourceRecipe.ResolvedInstallLocation
        sourcePath = resolvedLocation.SourcePath if resolvedLocation.SourcePath.startswith("$(") else resolvedLocation.ResolvedPath
        absoluteLocation = IOUtil.Join(sourcePath, srcLocation)

        fakeEntry = FakeXmlGenFileExternalDependencyDLL(basicConfig, srcName, srcDebugName, absoluteLocation, AccessType.Public, True)
        return PackageExternalDependency(pathBuilder, fakeEntry, package.Name, AccessType.Public, True)


    def __AddResolveExternalEntriesGeneratedByRecipeValidationCommandList(self, basicConfig: BasicConfig, pathBuilder: PathBuilder,
                                                                          package: Package,
                                                                          rAllDict: Dict[str, PackageExternalDependency],
                                                                          rPublicList: List[PackageExternalDependency],
                                                                          rPrivateList: List[PackageExternalDependency]) -> None:
        # if there is a recipe and it contains a ValidateInstallation section
        if package.ResolvedDirectExperimentalRecipe is None:
            return
        sourceRecipe = package.ResolvedDirectExperimentalRecipe
        if sourceRecipe.ValidateInstallation is None:
            return
        commandList = sourceRecipe.ValidateInstallation.CommandList
        if commandList is None or len(commandList) <= 0:
            return

        for command in commandList:
            if command.CommandType == BuildRecipeValidateCommand.AddHeaders:
                if not isinstance(command, XmlRecipeValidateCommandAddHeaders):
                    raise Exception("Invalid command type")
                processedEntry = self.__CreateExternalHeadersDependency(basicConfig, pathBuilder, package, sourceRecipe, command)
                self.__ResolveAdd(package, processedEntry, rAllDict, rPublicList, rPrivateList)
                if basicConfig.Verbosity >= 2:
                    if processedEntry.ResolvedLocation is None:
                        raise Exception("processedEntry.ResolvedLocation is None")
                    basicConfig.LogPrint("Package {0} recipe '{1}' AddHeaders automatically added external dependency to the headers at {2}".format(package.Name, sourceRecipe.Name, processedEntry.ResolvedLocation.ResolvedPath))
            elif command.CommandType == BuildRecipeValidateCommand.AddLib:
                if not isinstance(command, XmlRecipeValidateCommandAddLib):
                    raise Exception("Invalid command type")
                processedEntry = self.__CreateExternalStaticLibDependency(basicConfig, pathBuilder, package, sourceRecipe, command)
                self.__ResolveAdd(package, processedEntry, rAllDict, rPublicList, rPrivateList)
                if basicConfig.Verbosity >= 2:
                    if processedEntry.ResolvedLocation is None:
                        raise Exception("processedEntry.ResolvedLocation is None")
                    if processedEntry.Name == processedEntry.DebugName:
                        basicConfig.LogPrint("Package {0} recipe '{1}' AddLib automatically added external dependency to {2} at {3}".format(package.Name, sourceRecipe.Name, processedEntry.Name, processedEntry.ResolvedLocation.ResolvedPath))
                    else:
                        basicConfig.LogPrint("Package {0} recipe '{1}' AddLib automatically added external dependency to {2} and debug name {3} at {4}".format(package.Name, sourceRecipe.Name, processedEntry.Name, processedEntry.DebugName, processedEntry.ResolvedLocation.ResolvedPath))
            elif command.CommandType == BuildRecipeValidateCommand.AddDLL:
                if not isinstance(command, XmlRecipeValidateCommandAddDLL):
                    raise Exception("Invalid command type")
                processedEntry = self.__CreateExternalDLLDependency(basicConfig, pathBuilder, package, sourceRecipe, command)
                self.__ResolveAdd(package, processedEntry, rAllDict, rPublicList, rPrivateList)
                if basicConfig.Verbosity >= 2:
                    if processedEntry.ResolvedLocation is None:
                        raise Exception("processedEntry.ResolvedLocation is None")
                    if processedEntry.Name == processedEntry.DebugName:
                        basicConfig.LogPrint("Package {0} recipe '{1}' AddDLL automatically added external dependency to {2} at {3}".format(package.Name, sourceRecipe.Name, processedEntry.Name, processedEntry.ResolvedLocation.ResolvedPath))
                    else:
                        basicConfig.LogPrint("Package {0} recipe '{1}' AddDLL automatically added external dependency to {2} and debug name {3} at {4}".format(package.Name, sourceRecipe.Name, processedEntry.Name, processedEntry.DebugName, processedEntry.ResolvedLocation.ResolvedPath))


    def __ResolveDirectExternalDependencies(self, basicConfig: BasicConfig,
                                            platformContext: PlatformContext,
                                            finalResolveOrder: List[Package],
                                            autoAddRecipeExternals: bool) -> None:
        try:
            basicConfig.PushIndent()

            for package in finalResolveOrder:
                allDict = {}   # type: Dict[str, PackageExternalDependency]
                publicList = []  # type: List[PackageExternalDependency]
                privateList = []  # type: List[PackageExternalDependency]
                ## We proces the packages in the correct resolve order
                ## This ensure that all dependencies have been processed
                directDependencies = package.GetExternalDependencies(platformContext.PlatformName)
                for dep in directDependencies:
                    if dep in allDict:
                        raise ExternalDependencyDuplicatedException(package, dep)
                    processedEntry = PackageExternalDependency(platformContext.PathBuilder, dep, package.Name, AccessType.Public)
                    if not package.IsVirtual:
                        processedEntry.ConsumedBy = package
                        processedEntry.IsFirstActualUse = True
                    allDict[dep.Name] = processedEntry
                    if processedEntry.Access == AccessType.Public:
                        publicList.append(processedEntry)
                    elif processedEntry.Access == AccessType.Private: # and depPackage.Name == package.Name:
                        privateList.append(processedEntry)

                if autoAddRecipeExternals:
                    self.__AddResolveExternalEntriesGeneratedByRecipeValidationCommandList(basicConfig, platformContext.PathBuilder, package, allDict, publicList, privateList)


                package.ResolvedDirectExternalDependencies = list(allDict.values())
                #print("Deps: " + ", ".join(Util.ExtractNames(package.ResolvedDirectExternalDependencies)))

                directDict = dict(allDict)
                # Add the external dependencies from all direct dependencies
                for depPackage in package.ResolvedDirectDependencies:
                    if depPackage.Access != AccessType.Link:
                        if depPackage.Package.ResolvedBuildAllPublicExternalDependencies is None:
                            raise Exception("Invalid package")
                        for entry in depPackage.Package.ResolvedBuildAllPublicExternalDependencies:
                            processedEntry = PackageExternalDependency(None, entry, entry.IntroducedByPackageName, depPackage.Access)
                            if not processedEntry.Name in allDict:
                                self.__ResolveAdd(package, processedEntry, allDict, publicList, privateList)
                            elif processedEntry.Name in directDict:
                                raise UsageErrorException("ExternalDependency: {0} was already defined by {1}".format(processedEntry.Name, processedEntry.IntroducedByPackageName))
                            elif processedEntry.FromPackageAccess < allDict[processedEntry.Name].FromPackageAccess:
                                # We have access to the entry but at more open access level, so adopt that instead
                                self.__ResolveRemove(allDict[processedEntry.Name], allDict, publicList, privateList)
                                self.__ResolveAdd(package, processedEntry, allDict, publicList, privateList)
                    else:
                        if depPackage.Package.ResolvedBuildAllExternalDependencies is None:
                            raise Exception("Invalid package")
                        for entry in depPackage.Package.ResolvedBuildAllExternalDependencies:
                            if entry.Type == ExternalDependencyType.DLL:
                                processedEntry = PackageExternalDependency(None, entry, entry.IntroducedByPackageName, depPackage.Access)
                                if not processedEntry.Name in allDict:
                                    self.__ResolveAdd(package, processedEntry, allDict, publicList, privateList)
                                elif processedEntry.Name in directDict:
                                    raise UsageErrorException("ExternalDependency: {0} was already defined by {1}".format(processedEntry.Name, processedEntry.IntroducedByPackageName))
                                elif processedEntry.FromPackageAccess < allDict[processedEntry.Name].FromPackageAccess:
                                    # We have access to the entry but at more open access level, so adopt that instead
                                    self.__ResolveRemove(allDict[processedEntry.Name], allDict, publicList, privateList)
                                    self.__ResolveAdd(package, processedEntry, allDict, publicList, privateList)


                allList = list(allDict.values())
                allList.sort(key=lambda s: s.Name.lower())
                publicList.sort(key=lambda s: s.Name.lower())
                privateList.sort(key=lambda s: s.Name.lower())
                package.ResolvedBuildAllExternalDependencies = allList
                package.ResolvedBuildAllPublicExternalDependencies = publicList
                package.ResolvedBuildAllPrivateExternalDependencies = privateList
                #print("package {0}: {1} {2} {3}".format(package.Name, [entry.Name for entry in allList], [entry.Name for entry in publicList], [entry.Name for entry in privateList]))
        finally:
            basicConfig.PopIndent()


    def __ResolveDirectVariants(self, platformName: str, finalResolveOrder: List[Package]) -> None:
        # First we resolve all direct variants introduced by a package
        for package in finalResolveOrder:
            sourceList = package.GetVariants(platformName)
            package.ResolvedDirectVariants = [PackagePlatformVariant(package.Name, entry, True) for entry in sourceList]


    def __GetVariantUndefinedOptions(self, baseVariant: PackagePlatformVariant, extendingVariant: PackagePlatformVariant) -> List[PackagePlatformVariantOption]:
        res = []  # type: List[PackagePlatformVariantOption]
        for entry in extendingVariant.Options:
            if not entry.Name in baseVariant.OptionDict:
                res.append(entry)
        return res

    # This is a fast way to validate various variant rules
    def __CheckVariants(self, config: Config, finalResolveOrder: List[Package]) -> None:
        variantDict = {} # type: Dict[str, Tuple[Package, PackagePlatformVariant]]
        for package in finalResolveOrder:
            # collect the variants the we encounter and check if any
            # later encounter of it tries to extend it with new options
            for variant in package.ResolvedDirectVariants:
                if not variant.Name in variantDict:
                    variantDict[variant.Name] = (package, variant)
                else:
                    record = variantDict[variant.Name]
                    if not config.AllowVariantExtension:
                        raise VariantExtensionNotSupportedException(package, variant, record[0])
                    undefinedOptions = self.__GetVariantUndefinedOptions(record[1], variant)
                    if len(undefinedOptions) > 0:
                        raise ExtendingVariantCanNotIntroduceNewOptionsException(package, variant, record[0], record[1], undefinedOptions)

        # Ensure that the variant names are unique when casing is ignored
        uniqueNames = {}  # type: Dict[str, Tuple[Package, PackagePlatformVariant]]
        for key, value in list(variantDict.items()):
            variantName = key.lower()
            if not variantName in uniqueNames:
                uniqueNames[variantName] = value
            else:
                record = uniqueNames[variantName]
                raise VariantNameCollisionException(value[0], value[1], record[0], record[1])


    def __ResolveAllVariants(self, finalResolveOrder: List[Package]) -> None:
        # For each package resolve all variants that it depends upon
        # NOTE: this resolver can be optimized by utilizing the fact that we resolve things in the correct order
        #       so all dependencies will have been resolved before the current package
        for package in finalResolveOrder:
            variantDict1 = {}  # type: Dict[str, PackagePlatformVariant]
            for depPackage in package.ResolvedBuildOrder:
                for variant1 in depPackage.ResolvedDirectVariants:
                    clonedVariant = PackagePlatformVariant(package.Name, variant1, depPackage == package)
                    if not variant1.Name in variantDict1:
                        variantDict1[variant1.Name] = clonedVariant
                    else:
                        variantDict1[variant1.Name] = variantDict1[variant1.Name].Extend(clonedVariant, depPackage.Name)

            package.ResolvedAllVariantDict = variantDict1

        # Update the directly resolved variants so they match the ones we utilize in the all list
        # we do this after everything has been resolved to prevent problems with modifying the ResolvedDirectVariants
        # that the above algorithm depends on
        for package in finalResolveOrder:
            variantDict2 = {}  # type: Dict[str, PackagePlatformVariant]
            for variant2 in list(package.ResolvedAllVariantDict.values()):
                variantDict2[variant2.Name] = variant2
            for i in range(len(package.ResolvedDirectVariants)):
                variantName = package.ResolvedDirectVariants[i].Name
                if variantName in variantDict2:
                    package.ResolvedDirectVariants[i] = variantDict2[variantName]


    def __MarkExternalLibFirstUse(self, topLevelPackage: Package) -> None:
#        self.__MarkExternalLibFirstUse2(topLevelPackage)
        for variant in list(topLevelPackage.ResolvedAllVariantDict.values()):
            for option in variant.Options:
                for dep in topLevelPackage.ResolvedDirectDependencies:
                    self.__MarkExternalLibFirstUse2(dep.Package, variant.Name, option.Name)


    def __MarkExternalLibFirstUse2(self, package: Package, variantName: str, optionName: str) -> Set[str]:
        if package.IsVirtual or not variantName in list(package.ResolvedAllVariantDict.keys()):
            return set()

        usedSet = set()  # type: Set[str]
        for dep in package.ResolvedDirectDependencies:
            usedSet |= self.__MarkExternalLibFirstUse2(dep.Package, variantName, optionName)

        option = package.ResolvedAllVariantDict[variantName].OptionDict[optionName]
        for entry in option.ExternalDependencies:
            if not entry.Name in usedSet:
                usedSet.add(entry.Name)
                entry.IsFirstActualUse = True
        return usedSet


    def __ResolvePlatform(self, platformName: str, hostPlatformName: str, packages: List[Package], ignoreNotSupported: bool) -> None:
        for package in packages:
            # ToolRecipe's resolve using the hostPlatformName as we use this for 'cross compiling' for now
            currentPlatformName = platformName if package.Type != PackageType.ToolRecipe else hostPlatformName
            package.ResolvedPlatformName = currentPlatformName
            package.ResolvedPlatform = package.GetPlatform(currentPlatformName)
            if not ignoreNotSupported:
                if package.ResolvedPlatform is not None:
                    # use the flag set in the xml
                    package.ResolvedPlatformDirectSupported = package.ResolvedPlatform.Supported
                else:
                    # Use the default value for the platform
                    package.ResolvedPlatformDirectSupported = package.PlatformDefaultSupportedValue
            else:
                package.ResolvedPlatformDirectSupported = True

            if currentPlatformName == PackageConfig.PlatformNameString.WINDOWS:
                if package.Type == PackageType.Library or package.Type == PackageType.Executable: # or package.Type == PackageType.ExeLibCombo:
                    if package.ResolvedPlatform is not None:
                        if package.ResolvedPlatform.ProjectId is None:  # split into separate line to make MyPy happy
                            raise XmlMissingWindowsVisualStudioProjectIdException(package.XMLElement, package.Name)
