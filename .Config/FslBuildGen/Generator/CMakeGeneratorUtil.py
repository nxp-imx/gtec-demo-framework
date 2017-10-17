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

from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.Packages.Package import Package

class CMakePathType(object):
    LocalRelative = 0
    # relative to a specific root
    Relative = 1
    # absolute path (not supported by cmake)
    # Absolute = 2


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
    def __init__(self, config: Config, strTemplatePath: str, filePrefix: str, hasManifest: bool) -> None:
        super(CodeTemplateCMake, self).__init__()
        self.TemplatePath = strTemplatePath
        self.AbsoluteTemplatePath = IOUtil.Join(config.SDKConfigTemplatePath, strTemplatePath)
        self.Master = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "%sCMakeLists.txt" % (filePrefix)))
        self.PackageTargetIncludeDirectories = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "Package_TargetIncludeDirectories.txt"))
        self.PackageTargetIncludeDirEntry = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "Package_TargetIncludeDirEntry.txt"))
        self.PackageDependencyAddSubdirectories = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "PackageDependency_add_subdirectories.txt"))
        self.PackageDependencyTargetLinkLibraries = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "PackageDependency_target_link_libraries.txt"))
        self.PackageDependencyTargetCompileDefinitions = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "PackageDependency_target_compile_definitions.txt"))
        self.PackageDependencyFindPackage = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "PackageDependency_find_package.txt"))
        self.SnippetDefaultTargetCompileOptions = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "Snippet_DefaultTargetCompileOptions.txt"))
        self.SnippetDefaultTargetCompileFeatures = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "Snippet_DefaultTargetCompileFeatures.txt"))


        if filePrefix == 'Ext':
            self.PackageTargetIncludeDirectories = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "%sPackage_TargetIncludeDirectories.txt" % (filePrefix)))
        else:
            self.PackageTargetIncludeDirectories = IOUtil.ReadFile(IOUtil.Join(self.AbsoluteTemplatePath, "Package_TargetIncludeDirectories.txt"))


def GetSDKBasedPath(config: Config, path: str) -> str:
    return Util.ChangeToCMakeEnvVariables(config.ToPath(path))


def GetSDKBasedPathUsingCMakeVariable(config: Config, path: str) -> str:
    return Util.ChangeToCMakeVariables(config.ToPath(path))


def GetRelativePath(config: Config, rootPath: str, path: str) -> str:
    if not rootPath or not path:
        raise Exception("rootPath or root can not be None")
    if not path.startswith(rootPath):
        raise Exception("Path not a part of the root path")
    return path[len(rootPath)+1:]


def GetAccessTypeString(package: Package, accessType: int, allowPrivate: bool = True) -> str:
    if not package.IsVirtual:
        if accessType == AccessType.Public or not allowPrivate:
            return "PUBLIC"
        return "PRIVATE"
    else:
        if accessType == AccessType.Public:
            return "INTERFACE"
        raise Exception("Not supported")


def GetPackageName(package: Package) -> str:
    return package.Name if not package.IsVirtual else ("_Virtual_{0}".format(package.Name))


def GetPackageShortName(package: Package) -> str:
    if package.ShortName is None:
        raise Exception("Invalid package")
    return package.ShortName if not package.IsVirtual else ("_Virtual_{0}".format(package.ShortName))


def GetAliasPackageName(package: Package) -> str:
    return GetAliasName(package.Name)


def GetAliasName(name: str) -> str:
    return "FslDemoFramework::{0}".format(name)


def BuildFindDirectExternalDependencies(config: Config, package: Package, templatePackageDependencyFindPackage: str) -> str:
    externalDeps = []
    for externalDep in package.ResolvedDirectExternalDependencies:
        if externalDep.Type == ExternalDependencyType.Find:
            externalDeps.append(externalDep)

    if len(externalDeps) <= 0:
        return ""

    snippet = templatePackageDependencyFindPackage
    content = ""
    for externalDep in externalDeps:
        findParams = "%s REQUIRED" % (externalDep.Name)
        contentEntry = snippet
        contentEntry = contentEntry.replace("##FIND_PARAMS##", findParams)
        content += contentEntry
    return content


def BuildTargetLinkLibrariesForDirectDependencies(config: Config,
                                                  package: Package,
                                                  templatePackageDependencyTargetLinkLibraries: str,
                                                  ignoreLibs: Optional[List[str]] = None) -> str:
    if ignoreLibs is None:
        ignoreLibs = []

    if package.ResolvedDirectDependencies is None:
        raise Exception("Invalid package")

    deps = ""
    for entry1 in package.ResolvedDirectDependencies:
        deps += "\n  %s %s" % (GetAccessTypeString(package, entry1.Access, False), GetPackageName(entry1.Package))
#        deps += "\n  %s %s" % (GetAccessTypeString(package, entry1.Access), GetAliasPackageName(entry1.Package))

    # FIX: handle debug libraries
    for entry2 in package.ResolvedDirectExternalDependencies:
        if entry2.Name not in ignoreLibs:
            if entry2.Type == ExternalDependencyType.StaticLib:
                location = entry2.Location if entry2.Location else ""
                fullPathLinkDir = Util.ChangeToCMakeEnvVariables(IOUtil.Join(location, entry2.Name))
                deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry2.Access, False), fullPathLinkDir)
            if entry2.Type == ExternalDependencyType.Find:
                linkName = "${%s_LIBRARY}" % (entry2.Name)
                deps += "\n  {0} {1}".format(GetAccessTypeString(package, entry2.Access, False), linkName)
        else:
            config.LogPrint("INFO: Force ignored '{0}'".format(entry2.Name))

    if len(deps) <= 0:
        return ""

    content = templatePackageDependencyTargetLinkLibraries
    content = content.replace("##PACKAGE_DIRECT_DEPENDENCIES##", deps)
    return content


def BuildDirectDefinitions(config: Config, package: Package, templatePackageDependencyTargetCompileDefinitions: str) -> str:
    if package.ResolvedBuildDirectDefines is None:
        raise Exception("Invalid package")

    if len(package.ResolvedBuildDirectDefines) <= 0:
        return ""
    snippet = templatePackageDependencyTargetCompileDefinitions
    content = ""
    for entry in package.ResolvedBuildDirectDefines:
        content += "\n  {0} {1}".format(GetAccessTypeString(package, entry.Access), entry.Name)

    return snippet.replace("##PACKAGE_COMPILE_DEFINITIONS##", content)


def __GenerateDirEntryString(access: str, incPath: str, templatePackageTargetIncludeDirEntry: str) -> str:
    content = templatePackageTargetIncludeDirEntry
    content = content.replace("##DIR_ACCESS##", access)
    content = content.replace("##DIR_PATH##", incPath)
    return content


def __GetPackageIncludePath(config: Config, package: Package, absPathInsidePackage: str, pathType: int) -> str:
    if pathType == CMakePathType.LocalRelative:
        if package.AbsolutePath is None:
            raise Exception("Invalid package")
        lenAbsPath = len(package.AbsolutePath)
        return absPathInsidePackage[lenAbsPath+1:]
    elif pathType == CMakePathType.Relative:
        return GetSDKBasedPathUsingCMakeVariable(config, absPathInsidePackage)
    raise Exception("Unsupported path type")


def BuildTargetIncludeDirectories(config: Config, package: Package,
                                  templatePackageTargetIncludeDirectories: str,
                                  templatePackageTargetIncludeDirEntry: str,
                                  pathType: int = CMakePathType.Relative) -> str:
    publicIncludeDir = ""
    if package.AbsoluteIncludePath:
        pubIncPath = __GetPackageIncludePath(config, package, package.AbsoluteIncludePath, pathType)
        accessString = "PUBLIC" if not package.IsVirtual else "INTERFACE"
        publicIncludeDir = "\n" + __GenerateDirEntryString(accessString, pubIncPath, templatePackageTargetIncludeDirEntry)
    privateIncludeDir = ""
    if package.AbsoluteSourcePath:
        priIncPath = __GetPackageIncludePath(config, package, package.AbsoluteSourcePath, pathType)
        accessString = "PRIVATE" if not package.IsVirtual else "INTERFACE"
        privateIncludeDir = "\n" + __GenerateDirEntryString(accessString, priIncPath, templatePackageTargetIncludeDirEntry)

    for directExternalDeps in package.ResolvedDirectExternalDependencies:
        if directExternalDeps.Type != ExternalDependencyType.Find:
            currentIncDir = directExternalDeps.Include
            if currentIncDir:
                if package.AbsolutePath is None:
                    raise Exception("Invalid package")
                packageRootPath = config.ToPath(package.AbsolutePath)
                if currentIncDir.startswith(packageRootPath):
                    relativeCurrentIncDir = currentIncDir[len(packageRootPath)+1:] if pathType == CMakePathType.LocalRelative else Util.ChangeToCMakeVariables(currentIncDir)
                    add = "\n" + __GenerateDirEntryString(GetAccessTypeString(package, directExternalDeps.Access), relativeCurrentIncDir, templatePackageTargetIncludeDirEntry)
                else:
                    add = "\n  {0} {1}".format(GetAccessTypeString(package, directExternalDeps.Access), Util.ChangeToCMakeEnvVariables(currentIncDir))
                if directExternalDeps.Access == AccessType.Public:
                    publicIncludeDir += add
                else:
                    privateIncludeDir += add
        else:
            add = "\n  %s ${%s_INCLUDE_DIRS}" % (GetAccessTypeString(package, directExternalDeps.Access), directExternalDeps.Name)
            if directExternalDeps.Access == AccessType.Public:
                publicIncludeDir += add
            else:
                privateIncludeDir += add


    if len(publicIncludeDir) <= 0 and len(privateIncludeDir) <= 0:
        return ""

    content = templatePackageTargetIncludeDirectories
    content = content.replace("##PACKAGE_PUBLIC_INCLUDE_DIRECTORIES##", publicIncludeDir)
    content = content.replace("##PACKAGE_PRIVATE_INCLUDE_DIRECTORIES##", privateIncludeDir)
    return content
