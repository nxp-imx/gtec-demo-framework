#!/usr/bin/env python

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

import itertools
import os
import os.path
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.SharedGeneration import *
from FslBuildGen.PackageBuildReport import *

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
    def __init__(self, config, strTemplatePath, filePrefix, hasManifest):
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


def GetSDKBasedPath(config, path):
    return Util.ChangeToCMakeEnvVariables(config.ToPath(path))

def GetSDKBasedPathUsingCMakeVariable(config, path):
    return Util.ChangeToCMakeVariables(config.ToPath(path))


def GetRelativePath(config, rootPath, path):
    if not rootPath or not path:
        raise Exception("rootPath or root can not be None");
    if not path.startswith(rootPath):
        raise Exception("Path not a part of the root path");
    return path[len(rootPath)+1:]


def GetAccessTypeString(package, accessType, allowPrivate=True):
    if not package.IsVirtual:
        if accessType == AccessType.Public or not allowPrivate:
            return "PUBLIC"
        else:
            return "PRIVATE"
    else:
        if accessType == AccessType.Public:
            return "INTERFACE"
        else:
            raise Exception("Not supported");


def GetPackageName(package):
    return package.Name if not package.IsVirtual else ("_Virtual_%s" % package.Name)


def GetPackageShortName(package):
    return package.ShortName if not package.IsVirtual else ("_Virtual_%s" % package.ShortName)


def GetAliasPackageName(package):
    return GetAliasName(package.Name)


def GetAliasName(name):
    return "FslDemoFramework::%s" % (name)


def BuildFindDirectExternalDependencies(config, package, templatePackageDependencyFindPackage):       
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


def BuildTargetLinkLibrariesForDirectDependencies(config, package, templatePackageDependencyTargetLinkLibraries, ignoreLibs = []):       
    deps = ""
    for entry in package.ResolvedDirectDependencies:
        deps += "\n  %s %s" % (GetAccessTypeString(package, entry.Access, False), GetPackageName(entry.Package))
#        deps += "\n  %s %s" % (GetAccessTypeString(package, entry.Access), GetAliasPackageName(entry.Package))

    # FIX: handle debug libraries
    for entry in package.ResolvedDirectExternalDependencies:
        if entry.Name not in ignoreLibs:
            if entry.Type == ExternalDependencyType.StaticLib:
                location = entry.Location if entry.Location else ""
                fullPathLinkDir = Util.ChangeToCMakeEnvVariables(IOUtil.Join(location, entry.Name))
                deps += "\n  %s %s" % (GetAccessTypeString(package, entry.Access, False), fullPathLinkDir)
            if entry.Type == ExternalDependencyType.Find:
                linkName = "${%s_LIBRARY}" % (entry.Name)
                deps += "\n  %s %s" % (GetAccessTypeString(package, entry.Access, False), linkName)
        else:
            config.LogPrint("INFO: Force ignored '%s'" % entry.Name);

    if len(deps) <= 0:
        return ""

    content = templatePackageDependencyTargetLinkLibraries
    content = content.replace("##PACKAGE_DIRECT_DEPENDENCIES##", deps)
    return content


def BuildDirectDefinitions(config, package, templatePackageDependencyTargetCompileDefinitions):
    if len(package.ResolvedBuildDirectCPPDefines) <= 0:
        return ""
    snippet = templatePackageDependencyTargetCompileDefinitions
    content = ""
    for entry in package.ResolvedBuildDirectCPPDefines:
        content += "\n  %s %s" % (GetAccessTypeString(package, entry.Access), entry.Name)
    
    return snippet.replace("##PACKAGE_COMPILE_DEFINITIONS##", content)


def __GenerateDirEntryString(access, incPath, templatePackageTargetIncludeDirEntry):        
    content = templatePackageTargetIncludeDirEntry
    content = content.replace("##DIR_ACCESS##", access)
    content = content.replace("##DIR_PATH##", incPath)
    return content


def __GetPackageIncludePath(config, package, absPathInsidePackage, useSDKRelativePath):
    if not useSDKRelativePath:
        lenAbsPath = len(package.AbsolutePath)
        return package.AbsoluteIncludePath[lenAbsPath+1:]
    else:
        return GetSDKBasedPathUsingCMakeVariable(config, absPathInsidePackage)


def BuildTargetIncludeDirectories(config, package, templatePackageTargetIncludeDirectories, templatePackageTargetIncludeDirEntry, useSDKRelativePath=False):
    publicIncludeDir = ""
    if package.AbsoluteIncludePath:
        pubIncPath = __GetPackageIncludePath(config, package, package.AbsoluteIncludePath, useSDKRelativePath)
        accessString = "PUBLIC" if not package.IsVirtual else "INTERFACE"
        publicIncludeDir = "\n" + __GenerateDirEntryString(accessString, pubIncPath, templatePackageTargetIncludeDirEntry)
    privateIncludeDir = ""
    if package.AbsoluteSourcePath:
        priIncPath = __GetPackageIncludePath(config, package, package.AbsoluteSourcePath, useSDKRelativePath)
        accessString = "PRIVATE" if not package.IsVirtual else "INTERFACE"
        privateIncludeDir = "\n" + __GenerateDirEntryString(accessString, priIncPath, templatePackageTargetIncludeDirEntry)

    for directExternalDeps in package.ResolvedDirectExternalDependencies:
        if directExternalDeps.Type != ExternalDependencyType.Find:
            currentIncDir = directExternalDeps.Include
            if currentIncDir:
                packageRootPath = config.ToPath(package.AbsolutePath)
                if currentIncDir.startswith(packageRootPath):
                    relativeCurrentIncDir = currentIncDir[len(packageRootPath)+1:] if not useSDKRelativePath else Util.ChangeToCMakeVariables(currentIncDir)
                    add = "\n" + __GenerateDirEntryString(GetAccessTypeString(package, directExternalDeps.Access), relativeCurrentIncDir, templatePackageTargetIncludeDirEntry)
                else:
                    add = "\n  %s %s" % (GetAccessTypeString(package, directExternalDeps.Access), Util.ChangeToCMakeEnvVariables(currentIncDir))
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

