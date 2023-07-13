#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
from enum import Enum
from FslBuildGen import IOUtil
from FslBuildGen import ToolSharedValues
from FslBuildGen import Util
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import SpecialFiles
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.ExternalVariantConstraints import ExternalVariantConstraints
from FslBuildGen.LibUtil import LibUtil
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDefine
from FslBuildGen.Packages.Package import PackageExternalDependency
from FslBuildGen.Packages.PackagePlatformExternalDependency import PackagePlatformExternalDependency
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
from FslBuildGen.ToolConfig import ToolConfig

class CMakeVariableType(Enum):
    Normal = 0
    Environment = 1

class CMakePathType(Enum):
    LocalRelative = 0
    # relative to a specific root
    Relative = 1
    # absolute path (not supported by cmake)
    # Absolute = 2

# This file is used to make the executables dependent on the 'content' + 'content building' in a way that doesnt confuse cmake
# into thinking that content like the .obj mesh format is a object file
_CONTENT_DEP_FILENAME = "${CMAKE_CURRENT_BINARY_DIR}/content_deps.txt"

# Status
# - External libs with special debug libraries are not handled
# - Content is not located corectly in VS since the 'current work directory' is 'wrong'
# - Variants are not handled
# - Build content is not done
# - FslBuild things dont work
# - Using the 'root' CMakeLists.txt is kind of a 'work around' to allow us to re-use libraries
#   It would have been better to have a unique build file for each package with its own 'build' dir
#   However that would be more complex to implement and might make it impossible to have 'all'
#   package dependencies added as sub-projects in the IDE.
# - Install target does not work due to the way external libs are handled :(
# - Android content sync is not implemented
# - Platform dependent defines can not be specified (its all or nothing at the moment)
# - Version tags and handling?

class CodeTemplateCMake(object):
    def __init__(self, sdkConfigTemplatePath: str, strTemplatePath: str, overrideDirName: str, hasManifest: bool, overrideTemplateName: Optional[str]) -> None:
        super().__init__()
        self.TemplatePath = strTemplatePath
        self.AbsoluteTemplatePath = IOUtil.Join(sdkConfigTemplatePath, strTemplatePath)
        self.__OverrideDirName = overrideDirName
        self.__OverrideTemplateName = IOUtil.Join(sdkConfigTemplatePath, overrideTemplateName) if overrideTemplateName is not None else None

        self.Master = self.__ReadFile("CMakeLists.txt")
        self.PackageCompilerConditional = self.__ReadOptionalFile("Package_CompilerConditional.txt", "")
        self.PackageTargetIncludeDirectories = self.__ReadFile("Package_TargetIncludeDirectories.txt")
        self.PackageTargetIncludeDirEntry = self.__ReadFile("Package_TargetIncludeDirEntry.txt")
        self.PackageTargetIncludeDirVirtualEntry = self.__ReadFile("Package_TargetIncludeDirEntry_Virtual.txt")
        self.PackageTargetSourceFiles = self.__ReadOptionalFile("Package_TargetSourceFiles.txt", "")
        self.PackageTargetSpecialFileNatvis = self.__ReadOptionalFile("Package_TargetSpecialFile_Natvis.txt", "")
        self.PackageDependencyAddSubdirectories = self.__ReadFile("PackageDependency_add_subdirectories.txt")
        self.PackageDependencyTargetLinkLibraries = self.__ReadFile("PackageDependency_target_link_libraries.txt")
        self.PackageDependencyTargetCompileDefinitions = self.__ReadFile("PackageDependency_target_compile_definitions.txt")
        self.PackageDependencyFindPackage = self.__ReadFile("PackageDependency_find_package.txt")
        self.SnippetTargetCompileOptionsDefault = self.__ReadFile("Snippet_TargetCompileOptions_Default.txt")
        self.SnippetTargetCompileFeaturesDefault = self.__ReadFile("Snippet_TargetCompileFeatures_Default.txt")
        self.SnippetTargetCompileFeaturesInterface = self.__ReadFile("Snippet_TargetCompileFeatures_Interface.txt")
        self.AddImportedLibrary = self.__ReadFile("AddImportedLibrary.txt")
        self.PackageInstall = self.__ReadFile("PackageInstall.txt")
        self.PackageInstallAppInfo = self.__ReadOptionalFile("PackageInstall_AppInfo.txt", "")
        self.PackageInstallContent = self.__ReadOptionalFile("PackageInstall_Content.txt", "")
        self.PackageInstallDLL = self.__ReadOptionalFile("PackageInstall_DLL.txt", "")
        self.PackageInstallHeaders = self.__ReadFile("PackageInstall_Headers.txt")
        self.PackageInstallTargets = self.__ReadFile("PackageInstall_Targets.txt")
        self.PackageTargetIncludeDirectories = self.__ReadFile("Package_TargetIncludeDirectories.txt")

        self.PackageTargetCopyFile = self.__ReadOptionalFile("PackageDependency_target_copy_file.txt", "")
        self.PackageTargetCopyFilePath = self.__ReadOptionalFile("PackageDependency_target_copy_file_path.txt", "")

        self.PackageContentBuilder = self.__ReadOptionalFile("Package_ContentBuilder.txt", "")
        self.PackageContent = self.__ReadOptionalFile("Package_Content.txt", "")
        self.PackageContentFile = self.__ReadOptionalFile("Package_Content_File.txt", "")
        self.PackageContentDep = self.__ReadOptionalFile("Package_ContentDep.txt", "")

        self.PackageEmscripten = self.__ReadOptionalFile("Package_Emscripten.txt", "")
        self.PackageEmscriptenContent = self.__ReadOptionalFile("Package_Emscripten_content.txt", "")

        self.PathEnvToVariable = self.__ReadOptionalFile("PathEnvToVariable.txt", "")
        self.AddExtendedPackage = self.__ReadOptionalFile("AddExtendedPackage.txt", "")

        self.PackageVariantSettings = self.__ReadOptionalFile("PackageVariantSettings.txt", "")

        self.SnippetCommonHeader = self.__ReadOptionalFile("Snippet_CommonHeader.txt", "")
        self.SnippetCommonModules = self.__ReadOptionalFile("Snippet_CommonModules.txt", "")
        self.SnippetOptionsEXE = self.__ReadOptionalFile("Snippet_Options_EXE.txt", "")
        self.SnippetOptionsLIB = self.__ReadOptionalFile("Snippet_Options_LIB.txt", "")
        self.SnippetCacheVariant = self.__ReadOptionalFile("Snippet_Cache_Variant.txt", "")
        self.SnippetProjectPreventSourceBuilds = self.__ReadOptionalFile("Snippet_ProjectPreventSourceBuilds.txt", "")

        self.PackageCompilerFileDict = self.__BuildCompilerFileDict(IOUtil.Join(self.AbsoluteTemplatePath, overrideDirName))


    def __ReadFile(self, filename: str) -> str:
        if self.__OverrideTemplateName is not None:
            res = self.__TryDoReadFile(self.__OverrideTemplateName, self.__OverrideDirName, filename)
            if res is not None:
                return res
        return self.__DoReadFile(self.AbsoluteTemplatePath, self.__OverrideDirName, filename)


    def __ReadOptionalFile(self, filename: str, defaultValue:str) -> str:
        if self.__OverrideTemplateName is not None:
            res = self.__TryDoReadFile(self.__OverrideTemplateName, self.__OverrideDirName, filename)
            if res is not None:
                return res
        return self.__DoReadOptionalFile(self.AbsoluteTemplatePath, self.__OverrideDirName, filename, defaultValue)


    def __DoReadOptionalFile(self, absoluteTemplatePath: str, overrideDirName: str, filename: str, defaultValue:str) -> str:
        """
        Read a file, if not found return the defaultValue
        """
        res = self.__TryDoReadFile(absoluteTemplatePath, overrideDirName, filename)
        return res if res is not None else defaultValue


    def __DoReadFile(self, absoluteTemplatePath: str, overrideDirName: str, filename: str) -> str:
        templateFilename = IOUtil.Join(absoluteTemplatePath, "{0}/{1}".format(overrideDirName, filename))
        res = IOUtil.TryReadFile(templateFilename)
        if res is None:
            templateFilename = IOUtil.Join(absoluteTemplatePath, filename)
            res = IOUtil.ReadFile(templateFilename)
        return res


    def __TryDoReadFile(self, absoluteTemplatePath: str, overrideDirName: str, filename: str) -> Optional[str]:
        """
        Try to read a file from the override path, if its not found there try to read it from the base path,
        return None if not found
        """
        templateFilename = IOUtil.Join(absoluteTemplatePath, "{0}/{1}".format(overrideDirName, filename))
        res = IOUtil.TryReadFile(templateFilename)
        if res is None:
            templateFilename = IOUtil.Join(absoluteTemplatePath, filename)
            res = IOUtil.TryReadFile(templateFilename)
        return res

    def __BuildCompilerFileDict(self, basePath: str) -> Dict[str, List[str]]:
        compilerDependentFilePath = IOUtil.Join(basePath, "CompilerId")
        if not IOUtil.IsDirectory(compilerDependentFilePath):
            return dict()
        foundDirs = IOUtil.GetDirectoriesAt(compilerDependentFilePath, False)
        if len(foundDirs) <= 0:
            return dict()

        result = dict() # type: Dict[str, List[str]]
        for dirName in foundDirs:
            absDirName = IOUtil.Join(compilerDependentFilePath, dirName)
            foundFiles = IOUtil.GetFilesAt(absDirName, True)
            if len(foundFiles) > 0:
                result[dirName] = foundFiles
        return result


def GetSDKBasedPath(toolConfig: ToolConfig, path: str) -> str:
    return Util.ChangeToCMakeEnvVariables(toolConfig.ToPath(path))


def GetSDKBasedPathUsingCMakeVariable(toolConfig: ToolConfig, path: str) -> str:
    return Util.ChangeToCMakeVariables(toolConfig.ToPath(path))


def GetPackageSDKBasedPathUsingCMakeVariable(toolConfig: ToolConfig, package: Package, packageRelativeFilePath: str) -> str:
    if package.AbsolutePath is None:
        raise Exception("Invalid package")
    return GetSDKBasedPathUsingCMakeVariable(toolConfig, IOUtil.Join(package.AbsolutePath, packageRelativeFilePath))


#def GetRelativePath(rootPath: str, path: str) -> str:
#    if not rootPath or not path:
#        raise Exception("rootPath or root can not be None")
#    if not path.startswith(rootPath):
#        raise Exception("Path not a part of the root path")
#    return path[len(rootPath)+1:]


def GetAccessTypeString(package: Package, accessType: AccessType, allowPrivate: bool = True) -> str:
    if not package.IsVirtual:
        if accessType == AccessType.Public or not allowPrivate:
            return "PUBLIC"
        return "PRIVATE"
        #return "INTERFACE" // unfortunately there don't seem to be a mapping that corresponds to "Link" in cmake
    else:
        if accessType == AccessType.Public:
            return "INTERFACE"
        #raise Exception("Not supported")
        return "INTERFACE"



def GetPackageName(package: Package) -> str:
    name = package.Name
    if len(package.ResolvedVariantNameHint) > 0:
        name += package.ResolvedVariantNameHint
    return name if not package.IsVirtual or package.Type == PackageType.HeaderLibrary else ("_Virtual_{0}".format(name))


def GetPackageShortName(package: Package) -> str:
    return package.NameInfo.ShortName.Value if not package.IsVirtual else ("_Virtual_{0}".format(package.NameInfo.ShortName.Value))


def GetAliasPackageName(package: Package) -> str:
    return GetAliasName(GetPackageName(package), package.ProjectContext.ProjectName)


def GetAliasName(name: str, projectName: str) -> str:
    return "{0}::{1}".format(projectName, name)


def GetFullyQualifiedPackageName(package: Package) -> str:
    return GetAliasPackageName(package)


def BuildFindDirectExternalDependencies(log: Log, package: Package, templatePackageDependencyFindPackage: str) -> str:
    externalDeps = []  # type: List[PackageExternalDependency]
    for externalDep in package.ResolvedDirectExternalDependencies:
        if externalDep.Type == ExternalDependencyType.CMakeFindLegacy or externalDep.Type == ExternalDependencyType.CMakeFindModern:
            externalDeps.append(externalDep)

    if len(externalDeps) <= 0:
        return ""

    snippet = templatePackageDependencyFindPackage
    content = ""
    for externalDep in externalDeps:
        strVersion = " {0}".format(externalDep.Version) if externalDep.Version is not None else ""
        findParams = "{0}{1} REQUIRED".format(externalDep.Name, strVersion)
        contentEntry = snippet
        contentEntry = contentEntry.replace("##FIND_PARAMS##", findParams)
        content += contentEntry
    return content


def GetVersion(package: Package) -> str:
    return str(package.ProjectContext.ProjectVersion)


def _FindPackageDirectDependencyPackage(package: Package, templatePackageDependencyFindPackage: str) -> str:
    findParams = "{0} {1}".format(GetFullyQualifiedPackageName(package), GetVersion(package))
    return templatePackageDependencyFindPackage.replace("##FIND_PARAMS##", findParams)


def __BuildTargetLinkLibrariesForDirectExternalDependencies(log: Log, package: Package,
                                                            resolvedDirectExternalDependencies: Union[List[PackageExternalDependency], List[PackagePlatformExternalDependency]],
                                                            ignoreLibs: Optional[List[str]] = None) -> str:
    if ignoreLibs is None:
        ignoreLibs = []

    isExternalLibrary = package.Type == PackageType.ExternalLibrary

    deps = ""
    for entry in resolvedDirectExternalDependencies:
        libraryName = LibUtil.ToUnixLibName(entry.Name)
        if libraryName not in ignoreLibs:
            if entry.Type == ExternalDependencyType.StaticLib or (entry.Type == ExternalDependencyType.DLL and entry.Name.lower().endswith(".so")):
                location = entry.Location if entry.Location is not None and (entry.IsManaged or not isExternalLibrary) else ""
                libraryName = libraryName if len(location) <= 0 else entry.Name
                fullPathLinkDir = Util.ChangeToCMakeEnvVariables(IOUtil.Join(location, libraryName))
                if entry.DebugName != entry.Name:
                    deps += "\n  {0} optimized {1}".format(GetAccessTypeString(package, entry.Access, False), fullPathLinkDir)
                    libraryName = LibUtil.ToUnixLibName(entry.DebugName)
                    fullPathLinkDir = Util.ChangeToCMakeEnvVariables(IOUtil.Join(location, libraryName))
                    deps += "\n  {0} debug {1}".format(GetAccessTypeString(package, entry.Access, False), fullPathLinkDir)
                else:
                    deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry.Access, False), fullPathLinkDir)
            if entry.Type == ExternalDependencyType.CMakeFindLegacy:
                linkName = "${{{0}_LIBRARY}}".format(libraryName)
                deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry.Access, False), linkName)
            elif entry.Type == ExternalDependencyType.CMakeFindModern:
                deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry.Access, False), entry.TargetName)
        else:
            log.LogPrintVerbose(2, "INFO: Force ignored '{0}'".format(libraryName))
    return deps

def BuildTargetLinkLibrariesForDirectDependencies(log: Log,
                                                  package: Package,
                                                  templatePackageDependencyTargetLinkLibraries: str,
                                                  templatePackageDependencyFindPackage: str,
                                                  ignoreLibs: Optional[List[str]] = None) -> str:
    if package.ResolvedDirectDependencies is None:
        raise Exception("Invalid package")

    deps = ""
    findDeps = ""
    for entry1 in package.ResolvedDirectDependencies:
        if entry1.Package.Type != PackageType.ToolRecipe:
            deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry1.Access, True), GetFullyQualifiedPackageName(entry1.Package))
#           deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry1.Access), GetAliasPackageName(entry1.Package))
            findDeps += "\n{0}".format(_FindPackageDirectDependencyPackage(entry1.Package, templatePackageDependencyFindPackage))

    deps += __BuildTargetLinkLibrariesForDirectExternalDependencies(log, package, package.ResolvedDirectExternalDependencies, ignoreLibs)

    if len(deps) <= 0:
        return ""

    content = templatePackageDependencyTargetLinkLibraries
    content = content.replace("##PACKAGE_DIRECT_DEPENDENCIES##", deps)
    content = content.replace("##PACKAGE_FINDDIRECT_DEPENDENCIES##", findDeps)
    return content

def __BuildDefinitions(package: Package, directDefines: List[PackageDefine], templatePackageDependencyTargetCompileDefinitions: str) -> str:
    if len(directDefines) <= 0:
        return ""
    snippet = templatePackageDependencyTargetCompileDefinitions
    content = ""
    for entry in directDefines:
        if entry.Value is None:
            content += "\n  {0}\n    {1}".format(GetAccessTypeString(package, entry.Access), entry.Name)
        else:
            content += "\n  {0}\n    {1}={2}".format(GetAccessTypeString(package, entry.Access), entry.Name, entry.Value)

    return snippet.replace("##PACKAGE_COMPILE_DEFINITIONS##", content)


def BuildDirectDefinitions(log: Log, package: Package, templatePackageDependencyTargetCompileDefinitions: str, extraDefines: Optional[List[PackageDefine]]=None) -> str:
    if package.ResolvedBuildDirectDefines is None:
        raise Exception("Invalid package")

    directDefines = package.ResolvedBuildDirectDefines
    if extraDefines is not None:
        directDefines += extraDefines
    return __BuildDefinitions(package, package.ResolvedBuildDirectDefines, templatePackageDependencyTargetCompileDefinitions)


def __GenerateDirEntryString(access: str, incPath: str, templatePackageTargetIncludeDirEntry: str) -> str:
    content = templatePackageTargetIncludeDirEntry
    content = content.replace("##DIR_ACCESS##", access)
    content = content.replace("##DIR_PATH##", incPath)
    return content


def __GetPackageIncludePath(toolConfig: ToolConfig, package: Package, absPathInsidePackage: str, pathType: CMakePathType) -> str:
    if pathType == CMakePathType.LocalRelative:
        #if package.AbsolutePath is None:
        #    raise Exception("Invalid package")
        #if absPathInsidePackage.startswith(package.AbsolutePath + '/'):
        #    lenAbsPath = len(package.AbsolutePath)
        #    return absPathInsidePackage[lenAbsPath+1:]
        #return IOUtil.RelativePath(absPathInsidePackage, package.AbsolutePath)
        return GetSDKBasedPathUsingCMakeVariable(toolConfig, absPathInsidePackage)
    elif pathType == CMakePathType.Relative:
        return GetSDKBasedPathUsingCMakeVariable(toolConfig, absPathInsidePackage)
    raise Exception("Unsupported path type")


def __TryTargetIncludeDirectoriesGetExternalDependencyString(toolConfig: ToolConfig, package: Package,
                                                             directExternalDeps: Union[PackageExternalDependency, PackagePlatformExternalDependency],
                                                             templatePackageTargetIncludeDirEntry: str,
                                                             templatePackageTargetIncludeDirVirtualEntry: str, pathType: CMakePathType) -> Optional[str]:
    add = None # type: Optional[str]
    relativeCurrentIncDir = None # type: Optional[str]
    if directExternalDeps.Type != ExternalDependencyType.CMakeFindLegacy:
        currentIncDir = directExternalDeps.Include
        if currentIncDir is not None:
            if package.AbsolutePath is None:
                raise Exception("Invalid package")
            packageRootPath = toolConfig.ToPath(package.AbsolutePath)
            if currentIncDir.startswith(packageRootPath):
                relativeCurrentIncDir = currentIncDir[len(packageRootPath)+1:] if pathType == CMakePathType.LocalRelative else Util.ChangeToCMakeVariables(currentIncDir)
                add = "\n" + __GenerateDirEntryString(GetAccessTypeString(package, directExternalDeps.Access), relativeCurrentIncDir, templatePackageTargetIncludeDirEntry)
            else:
                currentTemplate = templatePackageTargetIncludeDirEntry
                relativeCurrentIncDir = toolConfig.TryToPath(currentIncDir)
                if relativeCurrentIncDir is None:
                    relativeCurrentIncDir = currentIncDir
                if pathType != CMakePathType.LocalRelative:
                    relativeCurrentIncDir = Util.ChangeToCMakeVariables(relativeCurrentIncDir)
                else:
                    relativeCurrentIncDir = Util.ChangeToCMakeEnvVariables(relativeCurrentIncDir)
                    currentTemplate = templatePackageTargetIncludeDirVirtualEntry

                add = "\n" + __GenerateDirEntryString(GetAccessTypeString(package, directExternalDeps.Access), relativeCurrentIncDir, currentTemplate)
    else:
        add = "\n  %s ${%s_INCLUDE_DIRS}" % (GetAccessTypeString(package, directExternalDeps.Access), directExternalDeps.Name)
    return add


def BuildTargetIncludeDirectories(toolConfig: ToolConfig, package: Package,
                                  templatePackageTargetIncludeDirectories: str,
                                  templatePackageTargetIncludeDirEntry: str,
                                  templatePackageTargetIncludeDirVirtualEntry: str,
                                  pathType: CMakePathType) -> str:
    #isExternalLibrary = package.Type == PackageType.ExternalLibrary
    publicIncludeDir = ""
    if package.AbsoluteIncludePath is not None:
        pubIncPath = __GetPackageIncludePath(toolConfig, package, package.AbsoluteIncludePath, pathType)
        accessString = "PUBLIC" if not package.IsVirtual else "INTERFACE"
        publicIncludeDir = "\n" + __GenerateDirEntryString(accessString, pubIncPath, templatePackageTargetIncludeDirEntry)
    privateIncludeDir = ""
    if package.AbsoluteSourcePath is not None:
        priIncPath = __GetPackageIncludePath(toolConfig, package, package.AbsoluteSourcePath, pathType)
        accessString = "PRIVATE" if not package.IsVirtual else "INTERFACE"
        privateIncludeDir = "\n" + __GenerateDirEntryString(accessString, priIncPath, templatePackageTargetIncludeDirEntry)
    for privEntry in package.ResolvedBuildDirectPrivateIncludeDirs:
        priIncPath = __GetPackageIncludePath(toolConfig, package, privEntry.ResolvedPath, pathType)
        accessString = "PRIVATE" if not package.IsVirtual else "INTERFACE"
        privateIncludeDir += "\n" + __GenerateDirEntryString(accessString, priIncPath, templatePackageTargetIncludeDirEntry)

    for directExternalDeps in package.ResolvedDirectExternalDependencies:
        add = __TryTargetIncludeDirectoriesGetExternalDependencyString(toolConfig, package, directExternalDeps,
                                                                       templatePackageTargetIncludeDirEntry, templatePackageTargetIncludeDirVirtualEntry,
                                                                       pathType)
        if add is not None:
            if directExternalDeps.Access == AccessType.Public:
                publicIncludeDir += add
            else:
                privateIncludeDir += add

    #for variant in package.ResolvedAllVariantDict
    for variant in package.ResolvedDirectVariants:
        if variant.Type == VariantType.Virtual:
            if len(variant.Options) != 1:
                raise Exception("VirtualVariant has unsupported amount of options")
            for variantDirectExternalDeps in variant.Options[0].ExternalDependencies:
                add = __TryTargetIncludeDirectoriesGetExternalDependencyString(toolConfig, package, variantDirectExternalDeps,
                                                                               templatePackageTargetIncludeDirEntry,
                                                                               templatePackageTargetIncludeDirVirtualEntry,
                                                                               pathType)
                if add is not None:
                    if variantDirectExternalDeps.Access == AccessType.Public:
                        publicIncludeDir += add
                    else:
                        privateIncludeDir += add


    if len(publicIncludeDir) <= 0 and len(privateIncludeDir) <= 0:
        return ""

    content = templatePackageTargetIncludeDirectories
    content = content.replace("##PACKAGE_PUBLIC_INCLUDE_DIRECTORIES##", publicIncludeDir)
    content = content.replace("##PACKAGE_PRIVATE_INCLUDE_DIRECTORIES##", privateIncludeDir)
    return content



def BuildInstallInstructions(log: Log, package: Package, templateInstallInstructions: str,
                             templateInstallInstructionsTargets: str,
                             templateInstallInstructionsHeaders: str,
                             templateInstallInstructionsContent: str,
                             templateInstallInstructionsDLL: str,
                             templateInstallInstructionsAppInfo: str) -> str:
    hasIncludeDirectory = package.ResolvedBuildPublicIncludeFiles is not None and len(package.ResolvedBuildPublicIncludeFiles) > 0

    installTargets = templateInstallInstructionsTargets

    includeHeadersContent = ""
    if (package.Type == PackageType.Library or package.Type == PackageType.HeaderLibrary) and hasIncludeDirectory:
        includeHeadersContent = templateInstallInstructionsHeaders
        includeHeadersContent = includeHeadersContent.replace("##PACKAGE_INCLUDE_DIRECTORY##", package.BaseIncludePath)
    installContent = ""
    installDLL = ""
    installAppInfo = ""
    targetInstallDir = ""
    if (package.Type == PackageType.Executable):
        # Content
        if package.ContentPath is not None and (len(package.ResolvedContentFiles) > 0 or len(package.ResolvedContentBuilderAllOutputFiles) > 0):
            packageContentFolderName = IOUtil.GetFileName(package.ContentPath.AbsoluteDirPath)
            installContent = templateInstallInstructionsContent
            installContent = installContent.replace("##PACKAGE_CONTENT_DIR##", packageContentFolderName)
            installContent = "\n" + installContent

        # Windows DLL's
        files = _GetDLLFileList(package)
        if len(files) > 0:
            installDLL = templateInstallInstructionsDLL
            installDLL = installDLL.replace("##FILES##", "\n    " + "\n    ".join(files))
            installDLL = "\n" + installDLL

        # App info
        installAppInfo = templateInstallInstructionsAppInfo
        installAppInfo = "\n" + installAppInfo

        # target install dir
        targetInstallDir = "/" + package.Name.replace('.', '/')



    content = templateInstallInstructions
    content = content.replace("##PACKAGE_INSTALL_TARGETS##", installTargets)
    content = content.replace("##PACKAGE_INSTALL_HEADERS##", includeHeadersContent)
    content = content.replace("##PACKAGE_INSTALL_CONTENT##", installContent)
    content = content.replace("##PACKAGE_INSTALL_DLL##", installDLL)
    content = content.replace("##PACKAGE_INSTALL_APPINFO##", installAppInfo)
    content = content.replace("##PACKAGE_TARGET_INSTALL_PATH##", targetInstallDir)
    return content


def BuildCompileFeatures(log: Log, package: Package, templateTargetCompileFeaturesDefault: str, templateTargetCompileFeaturesInterface: str) -> str:
    if package.IsVirtual:
        return templateTargetCompileFeaturesInterface if package.Type == PackageType.HeaderLibrary else ""
    return templateTargetCompileFeaturesDefault


def BuildCompileOptions(log: Log, package: Package, templateTargetCompileOptionsDefault: str) -> str:
    return templateTargetCompileOptionsDefault


def _BuildDLLDepsDict(package: Package) -> Dict[str, PackageExternalDependency]:
    deps = dict()       #type: Dict[str, PackageExternalDependency]
    for depPackage in package.ResolvedBuildOrder:
        for directExternalDeps in depPackage.ResolvedDirectExternalDependencies:
            if directExternalDeps.Type == ExternalDependencyType.DLL and directExternalDeps.Location is not None:
                if directExternalDeps.Name not in deps:
                    deps[directExternalDeps.Name] = directExternalDeps
    return deps

def _GetDLLFileList(package: Package) -> List[str]:
    dllFiles = []
    deps = _BuildDLLDepsDict(package)
    for dependency in deps.values():
        if dependency.Location is not None:
            fullPathToFile = Util.ChangeToCMakeVariables(IOUtil.Join(dependency.Location, dependency.Name))
            srcFileCommand = fullPathToFile
            if dependency.DebugName != dependency.Name:
                fullPathToDebugFile = Util.ChangeToCMakeVariables(IOUtil.Join(dependency.Location, dependency.DebugName))
                srcFileCommand = "$<$<CONFIG:debug>:{0}>$<$<CONFIG:release>:{1}>".format(fullPathToDebugFile, srcFileCommand)
            dllFiles.append(srcFileCommand)
    return dllFiles


def BuildFileCopy(log: Log, package: Package, templatePackageTargetCopyFile: str, templatePackageTargetCopyFilePath: str) -> str:
    if package.Type != PackageType.Executable:
        return ""
    contentFiles = ""

    dllFileList = _GetDLLFileList(package)

    for srcFileCommand in dllFileList:
        copyFiles = "{0} $<TARGET_FILE_DIR:{1}>".format(srcFileCommand, package.Name)
        contentFile = templatePackageTargetCopyFilePath
        contentFile = contentFile.replace("##COPY_PATHS##", copyFiles)
        contentFiles += "\n{0}".format(contentFile)

    if len(contentFiles) <= 0:
        return ""

    content = templatePackageTargetCopyFile
    content = content.replace("##FILES##", contentFiles)
    return content


def GetAllPackageNames(package: Package, projectContextFilter: Optional[PackageProjectContext]) -> List[str]:
    """ Get a list of all package names used by the root cmake file """
    names = [GetFullyQualifiedPackageName(package) for package in package.ResolvedBuildOrder
            if (projectContextFilter is None or package.ProjectContext == projectContextFilter) and
            package.Type != PackageType.ToolRecipe and package.Type != PackageType.TopLevel]
    names.sort()
    return names


def GetProjectContexts(package: Package) -> List[PackageProjectContext]:
    contexts = set() # type: Set[PackageProjectContext]
    for package in package.ResolvedBuildOrder:
        if package.Type != PackageType.TopLevel and package.ProjectContext not in contexts:
            contexts.add(package.ProjectContext)
    return list(contexts)


def GetCacheVariants(package: Package, snippetCacheVariant: str) -> str:
    res = [] # type: List[str]
    for variant in package.ResolvedAllVariantDict.values():
        if len(variant.Options) > 0:
            variantOptions = " ".join([option.Name for option in variant.Options])

            content = snippetCacheVariant
            content = content.replace("##VARIANT_NAME##", variant.Name)
            content = content.replace("##VARIANT_DEFAULT_OPTION##", variant.Options[0].Name)
            content = content.replace("##VARIANT_OPTIONS##", variantOptions)
            res.append(content)
    return "\n".join(res)


def GetContentSectionOutputFileList(toolConfig: ToolConfig, package: Package, contentInBinaryDirectory: bool) -> List[str]:
    if package.ResolvedPath is None or len(package.ResolvedContentFiles) <= 0:
        return []
    outputContentFiles = _ExtractRelativePaths(toolConfig, package.ResolvedPath.ResolvedPathEx, package.ResolvedContentFiles)
    outputContentFiles.sort()
    if contentInBinaryDirectory:
        outputContentFiles = _MakeRelativeToCurrentBinaryDirectory(outputContentFiles)
    return outputContentFiles


def GetContentBuilderOutputFileList(toolConfig: ToolConfig, package: Package, contentInBinaryDirectory: bool) -> List[str]:
    if package.ResolvedPath is None or len(package.ResolvedContentBuilderAllOutputFiles) <= 0:
        return []
    outputContentFiles = _ExtractRelativePaths(toolConfig, package.ResolvedPath.ResolvedPathEx, package.ResolvedContentBuilderAllOutputFiles)
    outputContentFiles.sort()
    if contentInBinaryDirectory:
        outputContentFiles = _MakeRelativeToCurrentBinaryDirectory(outputContentFiles)
    return outputContentFiles


def GetContentDepOutputFile(log: Log, package: Package, contentInBinaryDirectory: bool) -> str:
    if len(package.ResolvedContentFiles) <= 0 and len(package.ResolvedContentBuilderAllOutputFiles) <= 0:
        return ""
    return "\n  " + _CONTENT_DEP_FILENAME


def _ExtractRelativePaths(toolConfig: ToolConfig, absoluteSourcePathEx: str, records: Union[List[PathRecord], List[str]], force: bool = False) -> List[str]:
    res = [] # type: List[str]
    for content in records:
        strPath = content.ResolvedPath if isinstance(content, PathRecord) else content
        if not force and strPath.startswith(absoluteSourcePathEx):
            res.append(strPath[len(absoluteSourcePathEx):])
        else:
            res.append(GetSDKBasedPathUsingCMakeVariable(toolConfig, strPath))
    return res

def _MakeRelativeToCurrentBinaryDirectory(files: List[str]) -> List[str]:
    return ["${CMAKE_CURRENT_BINARY_DIR}/" + filename for filename in files]


def GetContentBuilder(toolConfig: ToolConfig, package: Package, platformName:str, snippetContentBuilder: str, contentInBinaryDirectory: bool,
                      externalVariantConstraints: ExternalVariantConstraints) -> str:
    if package.ResolvedContentBuilderAllInputFiles is None or len(package.ResolvedContentBuilderAllInputFiles) <= 0:
        return ""
    if package.ResolvedPath is None:
        raise Exception("Package '{0}' is invalid as it is missing a path".format(package.Name))

    targetName = package.Name
    packagePath = package.ResolvedPath.ResolvedPath
    featureList = ",".join([entry.Name for entry in package.ResolvedAllUsedFeatures])

    inputContentFiles = _ExtractRelativePaths(toolConfig, package.ResolvedPath.ResolvedPathEx, package.ResolvedContentBuilderAllInputFiles, True)
    outputContentFiles = _ExtractRelativePaths(toolConfig, package.ResolvedPath.ResolvedPathEx, package.ResolvedContentBuilderAllOutputFiles)

    inputContentFiles.sort()
    outputContentFiles.sort()

    extraArgs = "" if not toolConfig.LowLevelToolConfig.AllowDevelopmentPlugins else " --dev"

    customArgs = ""
    if contentInBinaryDirectory:
        outputContentFiles = _MakeRelativeToCurrentBinaryDirectory(outputContentFiles)
        customArgs = " --output ${CMAKE_CURRENT_BINARY_DIR}/" + ToolSharedValues.CONTENT_FOLDER_NAME

    inputFiles = "\n    ".join(inputContentFiles)
    outputFiles = "\n    ".join(outputContentFiles)

    strExternalVariantConstraints = externalVariantConstraints.AsString()

    content = snippetContentBuilder
    content = content.replace("##PLATFORM_NAME##", platformName)
    content = content.replace("##PACKAGE_TARGET_NAME##", targetName)
    content = content.replace("##FEATURE_LIST##", featureList)
    content = content.replace("##PACKAGE_PATH##", packagePath)
    content = content.replace("##PACKAGE_CONTENTBUILDER_INPUT_FILES##", inputFiles)
    content = content.replace("##PACKAGE_CONTENTBUILDER_OUTPUT_FILES##", outputFiles)
    content = content.replace("##EXTRA_ARGS##", extraArgs)
    content = content.replace("##CUSTOM_ARGS##", customArgs)
    content = content.replace("##VARIANT_LIST##", strExternalVariantConstraints)

    return "\n" + content



def GetContentSection(toolConfig: ToolConfig, package: Package, platformName:str, snippetContentSection: str, snippetContentFile: str,
                      contentInBinaryDirectory: bool, externalVariantConstraints: ExternalVariantConstraints) -> str:
    if not contentInBinaryDirectory or package.ResolvedContentFiles is None or len(package.ResolvedContentFiles) <= 0:
        return ""
    if package.ResolvedPath is None:
        raise Exception("Package '{0}' is invalid as it is missing a path".format(package.Name))

    targetName = package.Name
    contentFiles = _ExtractRelativePaths(toolConfig, package.ResolvedPath.ResolvedPathEx, package.ResolvedContentFiles)

    contentFiles.sort()

    inputContentFiles = contentFiles
    outputContentFiles = contentFiles

    if contentInBinaryDirectory:
        outputContentFiles = _MakeRelativeToCurrentBinaryDirectory(outputContentFiles)

    strExternalVariantConstraints = externalVariantConstraints.AsString()

    contentCommands = [] # type: List[str]
    for i in range(len(inputContentFiles)):
        content = snippetContentFile
        content = content.replace("##PLATFORM_NAME##", platformName)
        content = content.replace("##PACKAGE_TARGET_NAME##", targetName)
        content = content.replace("##RELATIVE_INPUT_FILE##", inputContentFiles[i])
        content = content.replace("##INPUT_FILE##", "##PACKAGE_PATH##/{0}".format(inputContentFiles[i]))
        content = content.replace("##OUTPUT_FILE##", outputContentFiles[i])
        content = content.replace("##VARIANT_LIST##", strExternalVariantConstraints)
        contentCommands.append(content)

    contentSection = snippetContentSection
    contentSection = contentSection.replace("##CONTENT_FILE_COMMANDS##", "\n".join(contentCommands))
    return "\n" + contentSection


def GetContentDepSection(toolConfig: ToolConfig, package: Package, platformName:str, snippetContentSection: str, contentInBinaryDirectory: bool) -> str:
    outputContentFileList = GetContentSectionOutputFileList(toolConfig, package, contentInBinaryDirectory)
    outputContentBuildFileList = GetContentBuilderOutputFileList(toolConfig, package, contentInBinaryDirectory)
    if len(outputContentFileList) <= 0 and len(outputContentBuildFileList) <= 0:
        return ""

    allContentFileList = outputContentFileList + outputContentBuildFileList
    allContentFiles = "\n    " + "\n    ".join(allContentFileList)

    content = snippetContentSection
    content = content.replace("##OUTPUT_FILE##", _CONTENT_DEP_FILENAME)
    content = content.replace("##INPUT_FILES##", allContentFiles)
    return "\n" + content

def GetEmscriptenSection(toolConfig: ToolConfig, package: Package, platformName:str, snippetEmscriptenSection: str, snippetEmscriptenContent: str) -> str:
    if package.Type != PackageType.Executable:
        return ""

    content = " {0}".format(snippetEmscriptenContent) if len(package.ResolvedContentFiles) > 0 else ""
    result = snippetEmscriptenSection
    result = snippetEmscriptenSection.replace("##EMSCRIPTEN_CONTENT##", content)
    return result


def CompilerSpecificFileDependencies(toolConfig: ToolConfig, package: Package, snippetPackageCompilerConditional: str,
                                     snippetPackageTargetSourceFiles: str,
                                     packageCompilerFileDict: Dict[str, List[str]]) -> str:
    if len(packageCompilerFileDict) <= 0:
        return ""
    targetName = package.Name

    finalContent = ""
    for key, conditionalFiles in packageCompilerFileDict.items():
        content = [] # type: List[str]
        files = [] # type: List[str]
        for filename in conditionalFiles:
            inputFilename = GetSDKBasedPathUsingCMakeVariable(toolConfig, filename)
            outputFilename = IOUtil.GetFileName(filename)
            outputFilename = outputFilename.replace("__PACKAGE_TARGET_NAME__", targetName)
            content.append("configure_file({0} ${{CMAKE_CURRENT_BINARY_DIR}}/{1} COPYONLY)".format(inputFilename, outputFilename))
            files.append(outputFilename)

        contentTargetSource = snippetPackageTargetSourceFiles
        contentTargetSource =  contentTargetSource.replace("##PACKAGE_SOURCE_FILES##", "\n  " + "\n  ".join(files))
        targetSource = contentTargetSource.split("\n")

        content += targetSource
        contentConfigureTargetSource = "  " + "\n  ".join(content)

        conditionalContent = snippetPackageCompilerConditional
        conditionalContent = conditionalContent.replace("##COMPILER_ID##", key)
        conditionalContent = conditionalContent.replace("##CONDITIONAL_SECTION##", contentConfigureTargetSource)
        finalContent += conditionalContent
    return finalContent


def CreateDefineRootDirectoryEnvironmentAsVariables(toolConfig: ToolConfig, projectContext: ToolConfigProjectContext, includeParents: bool,
                                                    snippet: str, uniqueEnvironmentVariables: Set[str]) -> str:
    allProjectContextRootDirs = [] # List[ToolConfigRootDirectory]
    context = projectContext # type: Optional[ToolConfigProjectContext]
    while context is not None:
        rootDir = toolConfig.TryFindRootDirectory(context.Location.ResolvedPath)
        if rootDir is None:
            raise Exception("could not locate root directory")
        allProjectContextRootDirs.append(rootDir)
        context = context.ParentContext if includeParents else None

    allUniqueEnv = set(uniqueEnvironmentVariables)
    for entry in allProjectContextRootDirs:
        envVarName = entry.GetEnvironmentVariableName()
        if envVarName not in allUniqueEnv:
            allUniqueEnv.add(envVarName)

        for rootDir in toolConfig.RootDirectories:
            if entry != rootDir and entry.ProjectId == rootDir.ProjectId:
                envVarName = rootDir.GetEnvironmentVariableName()
                if envVarName not in allUniqueEnv:
                    allUniqueEnv.add(envVarName)

    allRootDirs = list(allUniqueEnv) # List[set]
    allRootDirs.sort(key=lambda s: s.lower())
    result = [] # List[str]
    for envEntry in allRootDirs:
        content = snippet
        content = content.replace("##ENVIRONMENT_VARIABLE_NAME##", envEntry)
        result.append(content)
    return "\n\n".join(result)


def GetAddExtendedPackageParent(toolConfig: ToolConfig, projectContext: ToolConfigProjectContext, snippet: str) -> str:
    parentContext = projectContext.ParentContext
    if parentContext is None:
        return ""

    rootDir = toolConfig.TryFindRootDirectory(parentContext.Location.ResolvedPath)
    if rootDir is None:
        raise Exception("could not locate root directory")

    content = snippet
    content = content.replace("##PARENT_NAME##", parentContext.ProjectName)
    #content = content.replace("##PARENT_ROOT##", "${{{0}}}".format(rootDir.GetEnvironmentVariableName()))
    content = content.replace("##PARENT_ROOT##", "../{0}".format(parentContext.ProjectId.ShortProjectId))
    return content

def GetVariantSettings(log: Log, package: Package, snippetPackageVariantSettings: str,
                       snippetDefine: str,
                       templatePackageDependencyTargetLinkLibraries: str) -> str:
    if len(package.ResolvedDirectVariants) <= 0:
        return ""
    result = [] # type: List[str]
    for variant in package.ResolvedDirectVariants:
        for variantOption in variant.Options:
            if len(variantOption.DirectDefines) > 0 or len(variantOption.ExternalDependencies) > 0:
                variantDefines = __BuildDefinitions(package, variantOption.DirectDefines, snippetDefine)
                if len(variantDefines) > 0:
                    variantDefines = "\n" + variantDefines

                depContent = ""
                variantStaticDeps = __BuildTargetLinkLibrariesForDirectExternalDependencies(log, package, variantOption.ExternalDependencies)
                if len(variantStaticDeps) > 0:
                    findDeps = ""
                    depContent = templatePackageDependencyTargetLinkLibraries
                    depContent = depContent.replace("##PACKAGE_DIRECT_DEPENDENCIES##", variantStaticDeps)
                    depContent = depContent.replace("##PACKAGE_FINDDIRECT_DEPENDENCIES##", findDeps)

                content = snippetPackageVariantSettings
                content = content.replace("##VARIANT_NAME##", variant.Name)
                content = content.replace("##VARIANT_OPTION##", variantOption.Name)
                content = content.replace("##VARIANT_DEFINES##", variantDefines)
                content = content.replace("##VARIANT_STATIC_DEPENDENCIES##", depContent)
                result.append(content)
    return "\n".join(result) + "\n"

#add_custom_command(TARGET ##PACKAGE_NAME POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                       ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:MyTarget>)


#add_custom_command(TARGET unitTests POST_BUILD
#COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<$<CONFIG:debug>:${DEBUG_EXE_PATH}>$<$<CONFIG:release>:${RELEASE_EXE_PATH}>" "$<$<CONFIG:debug>:${DEBUG_NEW_EXE}>$<$<CONFIG:release>:${RELEASE_NEW_EXE}>")

def __ContainsNatvis(package: Package) -> bool:
    for entry in package.ResolvedSpecialFiles:
        if entry.SourcePath == SpecialFiles.Natvis:
            return True
    return False

def GetTargetSpecialFiles(log: Log, toolConfig: ToolConfig, package: Package, snippetPackageTargetSpecialFileNatvis: str) -> str:
    if len(snippetPackageTargetSpecialFileNatvis) <= 0 or not __ContainsNatvis(package):
        return ""
    content = snippetPackageTargetSpecialFileNatvis
    natvis = GetPackageSDKBasedPathUsingCMakeVariable(toolConfig, package, SpecialFiles.Natvis)
    content = content.replace("##FULL_FILE_PATH##", natvis)
    return content

def ExpandPathAndJoinList(toolConfig: ToolConfig, package: Package, srcList: Optional[List[str]]) -> List[str]:
    if srcList is None or len(srcList) <= 0:
        return []
    if package.AbsolutePath is None:
        raise Exception("Invalid package")
    return [GetPackageSDKBasedPathUsingCMakeVariable(toolConfig, package, entry) for entry in srcList]


def ExpandPathAndJoin(toolConfig: ToolConfig, package: Package, srcList: Optional[List[str]]) -> str:
    if srcList is None or len(srcList) <= 0:
        return ""
    expandedList = ExpandPathAndJoinList(toolConfig, package, srcList)
    return "\n  " + "\n  ".join(expandedList)




def __TryExtractEnvironmentVariable(input: str) -> Optional[Tuple[str,CMakeVariableType]]:
    index = input.find("$")
    if index >= 0 and len(input) > (index + 3):
        if input[index+1] == '{':
            endIndex = input.find("}", index + 1)
            if endIndex >= 0:
                variableName = input[index+2:endIndex]
                if len(variableName) > 0:
                    return (variableName, CMakeVariableType.Normal)
        elif input[index+1] == '(':
            endIndex = input.find(")", index + 1)
            if endIndex >= 0:
                variableName = input[index+2:endIndex]
                if len(variableName) > 0:
                    return (variableName, CMakeVariableType.Environment)
    return None

def ExtractUniqueVariables(packages: List[Package]) -> Tuple[Set[str], Set[str]]:
    uniquePaths = set()
    uniqueNormalVariables = set()
    uniqueEnvironmentVariables = set()
    for package in packages:
        for directExternalDeps in package.ResolvedDirectExternalDependencies:
            if directExternalDeps.Type == ExternalDependencyType.DLL and directExternalDeps.Location is not None:
                if directExternalDeps.Location not in uniquePaths:
                    uniquePaths.add(directExternalDeps.Location)
                    extracted = __TryExtractEnvironmentVariable(directExternalDeps.Location)
                    if extracted is not None:
                        variableName, variableType = extracted
                        if variableType == CMakeVariableType.Normal:
                            if variableName not in uniqueNormalVariables:
                                uniqueNormalVariables.add(variableName)
                        elif variableType == CMakeVariableType.Environment:
                            if variableName not in uniqueEnvironmentVariables:
                                uniqueEnvironmentVariables.add(variableName)
    return (uniqueNormalVariables, uniqueEnvironmentVariables)
