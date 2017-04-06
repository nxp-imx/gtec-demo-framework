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
from FslBuildGen.Generator import CMakeGeneratorUtil
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.SharedGeneration import *
from FslBuildGen.PackageBuildReport import *

def GetVCBuildConfigurationName(entry):
    return "-".join(entry)

# Status
# - External libs with special debug libraries are not handled
# - Content is not located corectly in VS since the 'current work directory' is 'wrong'
# - The manifest for DPI on windows is not added.
# - Icon resourcres on windows are not added
# - Variants are not handled
# - Build content is not done
# - FslBuild things dont work
# - Using the 'root' CMakeLists.txt is kind of a 'work around' to allow us to re-use libraries
#   It would have been better to have a unique build file for each package with its own 'build' dir
#   However that would be more complex to implement and might make it impossible to have 'all' 
#   package dependencies added as sub-projects in the IDE.
# - Install target does not work due to the way external libs are handled :(
# - Platform dependent defines can not be specified (its all or nothing at the moment)
# - Version tags and handling?
# Note:
# - The package headers might not completely follow the FslBuildGen standards
#   Meaning some packages might have access to more than their Fsl.gen file 
#   allows them access to


# This generator does not work if there are multiple source roots :(
class GeneratorCMake(object):
    def __init__(self, config, packages, platformName):
        super(GeneratorCMake, self).__init__()
        strTemplatePath = "CMake"
        extTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Ext", False)
        libTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Lib", False)
        exeTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Exe", False)
        rootTemplate = CMakeGeneratorUtil.CodeTemplateCMake(config, strTemplatePath, "Root", False)

        for package in packages:
            if package.Type == PackageType.ExternalLibrary or package.Type == PackageType.HeaderLibrary:
                self.__GenerateCMakeFile(config, package, platformName, extTemplate)
            elif package.Type == PackageType.Library:
                self.__GenerateCMakeFile(config, package, platformName, libTemplate)
            elif package.Type == PackageType.Executable:
                self.__GenerateCMakeFile(config, package, platformName, exeTemplate)
            elif package.Type == PackageType.TopLevel:
                self.__GenerateCMakeFile(config, package, platformName, rootTemplate)


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}


    def __GenerateCMakeFile(self, config, package, platformName, template):
        packageName = CMakeGeneratorUtil.GetPackageName(package)

        addSubDirectoriesDirectDependencies = ""
        #findExternalDependencies = ""
        if package.Type == PackageType.TopLevel:
            package.AbsolutePath = config.SDKPath
            packageName = "DemoFrameworkTopLevel"
            addSubDirectoriesDirectDependencies = self.__BuildAddSubDirectoriesForDirectDependencies(config, package, template)
            #findExternalDependencies = self.__BuildFindExternalDependencies(config, package, template)

        aliasPackageName = CMakeGeneratorUtil.GetAliasName(packageName)

        targetIncludeDirectories = CMakeGeneratorUtil.BuildTargetIncludeDirectories(config, package, template.PackageTargetIncludeDirectories, template.PackageTargetIncludeDirEntry)

        includeFiles = self.__Join(package.ResolvedBuildAllIncludeFiles)
        sourceFiles = self.__Join(package.ResolvedBuildSourceFiles)

        linkLibrariesDirectDependencies = CMakeGeneratorUtil.BuildTargetLinkLibrariesForDirectDependencies(config, package, template.PackageDependencyTargetLinkLibraries)
        directDefinitions = CMakeGeneratorUtil.BuildDirectDefinitions(config, package, template.PackageDependencyTargetCompileDefinitions)
        findDirectExternalDependencies = CMakeGeneratorUtil.BuildFindDirectExternalDependencies(config, package, template.PackageDependencyFindPackage)

        buildCMakeFile = template.Master
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_INCLUDE_FILES##", includeFiles)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_SOURCE_FILES##", sourceFiles)
        buildCMakeFile = buildCMakeFile.replace("##TARGET_INCLUDE_DIRECTORIES##", targetIncludeDirectories)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##", addSubDirectoriesDirectDependencies)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##", linkLibrariesDirectDependencies)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##", directDefinitions)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##", findDirectExternalDependencies)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##", template.SnippetDefaultTargetCompileOptions)
        buildCMakeFile = buildCMakeFile.replace("##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##", template.SnippetDefaultTargetCompileFeatures)
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_NAME!##", packageName.upper())
        buildCMakeFile = buildCMakeFile.replace("##PACKAGE_NAME##", packageName)
        buildCMakeFile = buildCMakeFile.replace("##ALIAS_PACKAGE_NAME##", aliasPackageName)

        buildBasePath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        if not config.DisableWrite:
            IOUtil.SafeMakeDirs(buildBasePath)
            dstFileCMakeFile = IOUtil.Join(package.AbsolutePath, "CMakeLists.txt")
            IOUtil.WriteFileIfChanged(dstFileCMakeFile, buildCMakeFile)


    def __Join(self, list):
        if not list or len(list) <= 0:
            return ''
        return "\n  " + "\n  ".join(list)


    def __BuildAddSubDirectoriesForDirectDependencies(self, config, package, template):       
        if len(package.ResolvedBuildOrder) <= 0:
            return ""
        content = "" 
        for entry in package.ResolvedBuildOrder:
            if entry != package:
                path = CMakeGeneratorUtil.GetRelativePath(config, package.AbsolutePath, entry.AbsolutePath)
                content += template.PackageDependencyAddSubdirectories.replace("##PACKAGE_PATH##", path)
        return content


    #def __BuildFindExternalDependencies(self, config, package, template):       

    #    dictExternal = {}
    #    for subPackage in package.ResolvedBuildOrder:
    #        for externalDep in subPackage.ResolvedDirectExternalDependencies:
    #            dictExternal[externalDep.Name] = externalDep
        
    #    externalDeps = []
    #    for externalDep in dictExternal.values():
    #        if externalDep.Type == ExternalDependencyType.Find:
    #            externalDeps.append(externalDep)

    #    if len(externalDeps) <= 0:
    #        return ""
    
        
    #    snippet = template.PackageDependencyFindPackage
    #    content = ""
    #    for externalDep in externalDeps:
    #        findParams = "%s REQUIRED" % (externalDep.Name)
    #        contentEntry = snippet
    #        contentEntry = contentEntry.replace("##FIND_PARAMS##", findParams)
    #        content += contentEntry
    #    return content
