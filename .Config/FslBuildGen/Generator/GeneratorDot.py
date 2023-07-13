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
from typing import Union
import os
import os.path
import subprocess
from FslBuildGen import IOUtil
#from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDependency
from FslBuildGen.Packages.Package import PackageExternalDependency
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
#from FslBuildGen.Exceptions import *

class GeneratorDot(GeneratorBase):
    def __init__(self, log: Log, toolConfig: ToolConfig, packages: List[Package], platformName: str) -> None:
        super().__init__()

        self.UseVariantNames = True
        self.RequireExecutable = self.__HasExecutable(packages)
        self.ShowExternals = False
        self.ColorNodesByType = True

        # Find all base packages
        basePackages = self.__FindBasePackages(toolConfig.ProjectInfo.Contexts, packages)

        packageList = packages
        if self.RequireExecutable:
            packageList = self.FilterBasedOnExecutableDemand(packages)
            if len(packageList) <= 0:
                packageList = packages


        #dotFile = self.CreateDirectDependencies(log, packageList, platformName)
        #dotFile = self.CreateAllDependencies(log, packageList, platformName)
        #dotFile = self.CreateSimpleDependencies(log, packageList, platformName)
        dotFile = self.CreateSimpleDependencies2(log, packageList, basePackages, platformName)

        content = "\n".join(dotFile)

        tmpFile = "TmpDependencies_{0}.dot".format(platformName)
        outputFile = "Dependencies_{0}.png".format(platformName)

        IOUtil.WriteFile(tmpFile, content)

        # "dot -Tpng -o test.png Dependencies_Yocto.dot"
        try:
            subprocess.call(["dot", "-Tpng", "-o{0}".format(outputFile), tmpFile])
            os.remove(tmpFile)
        except Exception as ex:
            print("WARNING: Failed to execute dot, is it part of the path?")
            os.remove(tmpFile)
            raise

    def __HasExecutable(self, packages: List[Package]) -> bool:
        for package in packages:
            if package.Type == PackageType.Executable:
                return True
        return False

    def __FindBasePackages(self, projectContexts: List[ToolConfigProjectContext], packages: List[Package]) -> List[Package]:
        packageDict = {} # type: Dict[str, Package]
        for package in packages:
            packageDict[package.Name] = package

        basePackages = [] # type: List[Package]
        for projectContext in projectContexts:
            for basePackage in projectContext.BasePackages:
                basePackages.append(packageDict[basePackage.Name])
        return basePackages


    def CreateDirectDependencies(self, log: Log, packages: List[Package], platformName: str) -> List[str]:
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        laterPrivate = []
        laterLink = []

        #groups = {}

        for package in packages:
            if not package.Name.startswith('SYS_'):
                if package.ResolvedDirectDependencies is None:
                    raise Exception("Invalid package")

                for dep1 in package.ResolvedDirectDependencies:
                    if dep1.Access == AccessType.Link:
                        laterLink.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                    elif dep1.Access == AccessType.Private:
                        laterPrivate.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                    else:
                        dotFile.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                if self.ShowExternals:
                    for dep2 in package.ResolvedDirectExternalDependencies:
                        dotFile.append('  "{0}" -> "{1}"'.format(self.GetName(package), dep2.Name))

        if len(laterPrivate):
            dotFile.append('  edge [color="#2F4F4F", style=dashed]')
            for entry in laterPrivate:
                dotFile.append(entry)

        if len(laterLink):
            dotFile.append('  edge [color=Blue, style=dotted]')
            for entry in laterLink:
                dotFile.append(entry)

        dotFile.append('}')
        return dotFile


    def CreateAllDependencies(self, log: Log, packages: List[Package], platformName: str) -> List[str]:
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        laterPrivate = []
        laterLink = []

        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep1 in package.ResolvedAllDependencies:
                    if dep1.Access == AccessType.Link:
                        laterLink.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                    elif dep1.Access == AccessType.Private:
                        laterPrivate.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                    else:
                        dotFile.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                if self.ShowExternals:
                    for dep2 in package.ResolvedDirectExternalDependencies:
                        dotFile.append('  "{0}" -> "{1}"'.format(self.GetName(package), dep2.Name))

        if len(laterPrivate):
            dotFile.append('  edge [color="#2F4F4F", style=dashed]')
            for entry in laterPrivate:
                dotFile.append(entry)

        if len(laterLink):
            dotFile.append('  edge [color=Blue, style=dotted]')
            for entry in laterLink:
                dotFile.append(entry)

        dotFile.append('}')
        return dotFile


    def CreateSimpleDependencies(self, log: Log, packages: List[Package], platformName: str) -> List[str]:
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep1 in package.ResolvedAllDependencies:
                    if not self.IsAvailableFromDependentPackage(dep1, package):
                        dotFile.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                if self.ShowExternals:
                    for dep2 in package.ResolvedDirectExternalDependencies:
                        if not self.IsExternalAvailableFromDependentPackage(dep2, package.ResolvedDirectDependencies):
                            dotFile.append('  "{0}" -> "ext: {1}"'.format(self.GetName(package), dep2.Name))

        dotFile.append('}')
        return dotFile


    def IsAvailableFromDependentPackage(self, dep: PackageDependency, package: Package) -> bool:
        for entry in package.ResolvedAllDependencies:
            if entry.Name != dep.Name:
                if self.ExistIn(entry.Package.ResolvedAllDependencies, dep.Name):
                    return True
        return False


    def IsExternalAvailableFromDependentPackage(self, dep: PackageExternalDependency, otherDeps: List[PackageDependency]) -> bool:
        for entry in otherDeps:
            if self.ExistIn(entry.Package.ResolvedDirectExternalDependencies, dep.Name):
                return True
        return False


    # Was multiple defined, enabled the last one as that is the one the code has been calling
    #def ExistIn(self, dependencies, name):
    #    for dep in dependencies:
    #        if dep.Name == name:
    #            return True;
    #    return False;


    def GetRootNamespace(self, log: Log, namespace: Optional[str]) -> str:
        if namespace is None:
            return ''
        if not '.' in namespace:
            return namespace
        return namespace[0:namespace.find('.')]


    def GetRootName(self, log: Log, package: Package) -> str:
        if package.AbsoluteBuildPath is None:
            raise Exception("Invalid package")
        #if not '.' in package.NameInfo.Namespace:
        #    if 'ThirdParty' in package.AbsoluteBuildPath:
        #        return 'ThirdParty'
        #    return ""
        if 'ThirdParty' in package.AbsoluteBuildPath:
            return 'ThirdParty'
        return ""


    def GroupPackages(self, log: Log, packages: List[Package]) -> Dict[str, List[Package]]:
        groupDict = {}  # type: Dict[str, List[Package]]
        for package in packages:
            #rootNamespace = self.GetRootNamespace(config, package.Namespace)
            rootNamespace = self.GetRootName(log, package)
            if rootNamespace not in groupDict:
                groupDict[rootNamespace] = []
            groupDict[rootNamespace].append(package)

        for entry in list(groupDict.values()):
            entry.sort(key=lambda s: s.Name.lower())
        return groupDict


    def CreateSimpleDependencies2(self, log: Log, packages: List[Package], basePackages: List[Package], platformName: str) -> List[str]:
        groups = None # type: Optional[Dict[str, List[Package]]]
        #groups = self.GroupPackages(config, packages)

        showDependenciesToBasePackages = False
        showExternals = self.ShowExternals

        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        laterPrivate = []
        laterLink = []

        if groups is not None:
            laterGroups = []
            for key, group in groups.items():
                if len(key) > 0 and len(group) > 1:
                    laterGroups.append('  subgraph cluster_{0} {{'.format(key))
                    #laterGroups.append('      style=filled;')
                    #laterGroups.append('      color=lightgrey;')
                    laterGroups.append('      node [style=filled];')
                    for package in group:
                        laterGroups.append('      "{0}";'.format(self.GetName(package)))
                    laterGroups.append('      label = "{0}";'.format(key))
                    laterGroups.append('  }')

            if len(laterGroups):
                dotFile = dotFile + laterGroups

        if self.ColorNodesByType:
            for package in packages:
                packageName = self.GetName(package)
                col = "black" if package not in basePackages else "gold"
                if package.Type == PackageType.Library:
                    dotFile.append('  "{0}" [style=filled fillcolor=white color={1}]'.format(packageName, col))
                elif package.Type == PackageType.Executable:
                    dotFile.append('  "{0}" [style=filled fillcolor=green color={1}]'.format(packageName, col))
                elif package.Type == PackageType.ExternalLibrary:
                    dotFile.append('  "{0}" [style=filled fillcolor=lightgrey color={1}]'.format(packageName, col))
                elif package.Type == PackageType.HeaderLibrary:
                    dotFile.append('  "{0}" [fillcolor=gray42 color={1}]'.format(packageName, col))
                elif package.Type == PackageType.ToolRecipe:
                    dotFile.append('  "{0}" [style=filled fillcolor=deepskyblue color={1}]'.format(packageName, col))
                #elif package.Type == PackageType.Library:


        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep1 in package.ResolvedDirectDependencies:
                    if showDependenciesToBasePackages or dep1.Package not in basePackages:
                        if dep1.Access == AccessType.Private:
                            laterPrivate.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                        elif dep1.Access == AccessType.Link:
                            laterLink.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                        else:
                            if not self.IsAvailableFromDependentPackage2(dep1, package):
                                dotFile.append('  "{0}" -> "{1}"'.format(self.GetName(package), self.GetName(dep1.Package)))
                if showExternals:
                    for dep2 in package.ResolvedDirectExternalDependencies:
                        if not self.IsExternalAvailableFromDependentPackage(dep2, package.ResolvedDirectDependencies):
                            dotFile.append('  "{0}" -> "ext: {1}"'.format(self.GetName(package), dep2.Name))

        if len(laterPrivate):
            dotFile.append('  edge [color="#2F4F4F", style=dashed]')
            for entry in laterPrivate:
                dotFile.append(entry)

        if len(laterLink):
            dotFile.append('  edge [color=Blue, style=dotted]')
            for entry in laterLink:
                dotFile.append(entry)

        dotFile.append('}')
        return dotFile


    def IsAvailableFromDependentPackage2(self, dep: PackageDependency, package: Package) -> bool:
        for entry in package.ResolvedAllDependencies:
            if entry.Name != dep.Name and entry.Access == AccessType.Public:
                if self.ExistIn(entry.Package.ResolvedAllDependencies, dep.Name):
                    return True
        return False


    def ExistIn(self, dependencies: Union[List[PackageDependency], List[PackageExternalDependency]], name: str) -> bool:
        for dep in dependencies:
            if dep.Name == name and dep.Access == AccessType.Public:
                return True
        return False

    def GetName(self, package: Package) -> str:
        if self.UseVariantNames:
            if package.ResolvedMakeVariantNameHint is None:
                raise Exception("Invalid package")
            return package.Name + package.ResolvedMakeVariantNameHint
        return package.Name


    def FilterBasedOnExecutableDemand(self, packages: List[Package]) -> List[Package]:
        executablePackages = []  # type: List[Package]
        for package in packages:
            if package.Type == PackageType.Executable:
                executablePackages.append(package)

        packageList = list(executablePackages)
        interestingPackages = set()  # type: Set[str]
        for package in executablePackages:
            for dep in package.ResolvedAllDependencies:
                if not dep.Name in interestingPackages:
                    interestingPackages.add(dep.Name)
                    packageList.append(dep.Package)
        return packageList
