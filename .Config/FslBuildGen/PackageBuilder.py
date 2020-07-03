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
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DependencyGraph import DependencyGraph
from FslBuildGen.DependencyGraph import DependencyGraphNode
from FslBuildGen.Exceptions import CircularDependencyException
from FslBuildGen.Exceptions import CircularDependencyInDependentModuleException
from FslBuildGen.Exceptions import GroupedException
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDependency
from FslBuildGen.PackageConfig import PackageNameMagicString
from FslBuildGen.PackageManager import PackageManager
from FslBuildGen.Xml.XmlGenFile import XmlGenFile
from FslBuildGen.Xml.XmlBase2 import FakeXmlGenFileDependency


class PackageBuilder(object):
    def __init__(self, config: Config, platformName: str, generatorInfo: GeneratorInfo, genFiles: List[XmlGenFile], logVerbosity: int = 1) -> None:
        super().__init__()

        # create top level package and resolve build order
        config.LogPrintVerbose(logVerbosity, "Validating dependencies")

        packageManager = PackageManager(config, platformName, generatorInfo, genFiles)
        packages = packageManager.Packages

        # Build a graph containing all packages
        graph = DependencyGraph(None) #, True)
        for package in packages:
            graph.AddNode(package)
        graph.Finalize()

        # Extract the top level nodes
        nodes = graph.GetNodesWithNoIncomingDependencies()
        #useFallback = True
        if len(nodes) > 0:
            topLevelGenFile = XmlGenFile(config, config.ToolConfig.DefaultPackageLanguage)
            topLevelGenFile.Name = PackageNameMagicString.TopLevelName
            topLevelGenFile.SetType(PackageType.TopLevel)
            for entry in nodes:
                topLevelGenFile.DirectDependencies.append(FakeXmlGenFileDependency(config, entry.Name, AccessType.Public))

            topLevelGenFile.DirectDependencies.sort(key=lambda s: s.Name.lower())
        else:
            # We have circular dependencies and couldnt find any starting nodes
            # so generate a empty top level node and expect a circular dependency
            # error to be caught
            topLevelGenFile = XmlGenFile(config, config.ToolConfig.DefaultPackageLanguage)
            topLevelGenFile.Name = PackageNameMagicString.TopLevelName
            topLevelGenFile.SetType(PackageType.TopLevel)

        topLevelPackage = packageManager.CreatePackage(config, platformName, topLevelGenFile, True)
        graph.AddNodeAndEdges(topLevelPackage)

        # Since we need to resolve the build order we might as well verify dependencies at the same time
        self.__ValidateDependencies(config, packages)
        self.AllPackages = packages  # type: List[Package]
        self.TopLevelPackage = topLevelPackage
        self.__ResolveAllPackageDependencies(config, topLevelPackage)


    def __ValidateDependencies(self, config: Config, packages: List[Package]) -> None:
        exceptionList = []  # type: List[Exception]
        exceptionList2 = []  # type: List[Exception]
        for package in packages:
            try:
                #allDependenciesDict = {}
                #stack = [package]
                graph = DependencyGraph(package) #, True)
                orderedDependencyList = []  # type: List[Package]
                self.__ValidateDependenciesFor(orderedDependencyList, package, graph)
                orderedDependencyList.reverse()
                package.ResolvedBuildOrder = orderedDependencyList
                # NOTE: we can not filter the dependency list based on 'ExperimentalRecipes' here since we need to resolve it first
                # package.ResolvedExperimentalRecipeBuildOrder = [entry for entry in orderedDependencyList if not entry.DirectExperimentalRecipe is None]
            except CircularDependencyException as ex:
                if config.GroupException:
                    exceptionList.append(ex)
                else:
                    raise
            except CircularDependencyInDependentModuleException as ex:
                if config.GroupException:
                    exceptionList2.append(ex)
                else:
                    raise

        if len(exceptionList) > 0:
            raise GroupedException(exceptionList)
        # If the below exception is fired we have a internal error
        # as any entry in exceptionList2 should result in entries in
        # exceptionList
        if len(exceptionList2) > 0:
            raise GroupedException(exceptionList2)


    def __ValidateDependenciesFor(self, rOrderedDependencyList: List[Package], package: Package, graph: DependencyGraph) -> None:
        packageNode = graph.Get(package)
        # TODO: make the get method never return none
        if packageNode is None:
            raise Exception("the package should always have a node in the graph")
        while len(graph.Nodes) > 0:
            nodes = graph.RemoveNodesWithNoIncomingDependencies()
            if len(nodes) <= 0:
                self.HandleCircularDependencies(graph, packageNode)
            nodes.sort(key=lambda node: node.Name.lower())
            for node in nodes:
                if node.Package != packageNode:
                    rOrderedDependencyList.append(node.Package)


    def HandleCircularDependencies(self, graph: DependencyGraph, packageNode: DependencyGraphNode) -> None:
        keepRemoving = True
        while keepRemoving:
            keepRemoving = len(graph.RemoveNodesWithNoDependencies()) > 0

        # check if this package is actually part of the circular dependency or not
        if not packageNode in graph.Nodes:
            raise CircularDependencyInDependentModuleException("'%s' uses a package that has a circular dependency" % (packageNode.Name))

        # We are only interested in the dependencies that start at packageNode
        circularDependencies = [] # type: List[List[DependencyGraphNode]]
        dependencies = [packageNode]  # type: List[DependencyGraphNode]
        self.BuildDependencyList(circularDependencies, dependencies, packageNode)

        circularDepStringsSet = set() # Set[str]
        for circularDep in circularDependencies:
            circularDepStringsSet.add(self.GetCircularDependencyString(circularDep))

        circularDepStrings = list(circularDepStringsSet)
        circularDepStrings.sort(key=lambda str: (self.__CountArrows(str), str.lower()))

        raise CircularDependencyException("Circular dependency detected while validating " + packageNode.Name + ":\n  " + "\n  ".join(circularDepStrings))


    def BuildDependencyList(self,
                            rCircularDependencies: List[List[DependencyGraphNode]],
                            dependencies: List[DependencyGraphNode],
                            node: DependencyGraphNode) -> None:
        for depNode in node.To:
            if not depNode in dependencies:
                dependencies.append(depNode)
                self.BuildDependencyList(rCircularDependencies, dependencies, depNode)
                dependencies.pop()
            else:
                circularDependencyList = list(dependencies)
                circularDependencyList.append(depNode)
                rCircularDependencies.append(circularDependencyList)


    def GetCircularDependencyString(self, srcList: List[DependencyGraphNode]) -> str:
        if srcList is None or len(srcList) < 1:
            raise UsageErrorException("No circular dependency exist in the supplied list")
        try:
            index = srcList.index(srcList[-1])
            return self.GetNameListString(srcList[index:], "->")
        except ValueError:
            raise UsageErrorException("No circular dependency exist in the supplied list")


    def GetNameListString(self, srcList: List[DependencyGraphNode], seperator: str) -> str:
        strContent = ""
        isFirst = True
        for entry in srcList:
            if isFirst:
                strContent = entry.Name
                isFirst = False
            else:
                strContent += seperator + entry.Name
        return strContent


    def __CountArrows(self, strContent: str) -> int:
        count = 0
        index = strContent.find("->")
        lenStr = len(strContent)
        while index >= 0 and (index + 2) < lenStr:
            count = count + 1
            index = strContent.find("->", index + 2)
        return count


    def __ResolveAllPackageDependencies(self, config: Config, topLevel: Package) -> None:
        for package in topLevel.ResolvedBuildOrder:
            self.__DoResolveAllPackageDependencies(config, package)


    def __DoResolveAllPackageDependencies(self, config: Config, package: Package) -> None:
        # FIX: we are doing some of the same checks twice here
        addedDict = {}  # type: Dict[str, PackageDependency]
        # First we resolve all direct dependencies
        for dep in package.ResolvedDirectDependencies:
            if not dep.Name in addedDict:
                package.ResolvedAllDependencies.append(dep)
                addedDict[dep.Name] = dep
            else:
                # The package was already added so we need to check if this dependency is less restrictive than the old one
                oldDep = addedDict[dep.Name]
                if dep.Access < oldDep.Access:
                    package.ResolvedAllDependencies.remove(oldDep)
                    addedDict[dep.Name] = dep

        # Then we pull in the childrens dependencies
        resolvedDirectDependencies = list(package.ResolvedDirectDependencies)
        for directDep in resolvedDirectDependencies:
            for dep in directDep.Package.ResolvedAllDependencies:
                # ensure that anything we get via a non public access type keeps gets a access type that is >= directDep.Access
                if dep.Access < directDep.Access or dep.Access == AccessType.Private:
                    #dep = PackageDependency(dep.Package, directDep.Access)
                    dep = PackageDependency(dep.Package, AccessType.Link)
                if not dep.Name in addedDict:
                    package.ResolvedAllDependencies.append(dep)
                    addedDict[dep.Name] = dep
                else:
                    # The package was already added so we need to check if this dependency is less restrictive than the old one
                    oldDep = addedDict[dep.Name]
                    if dep.Access < oldDep.Access:
                        package.ResolvedAllDependencies.remove(oldDep)
                        package.ResolvedAllDependencies.append(dep)
                        addedDict[dep.Name] = dep
                        foundDep = self.__TryFindDep(package.ResolvedDirectDependencies, dep)
                        if foundDep is not None:
                            config.DoPrintWarning("Requested dependency access to '{0}', overwritten by dependency from '{1}'".format(dep.Name, directDep.Name))
                            package.ResolvedDirectDependencies.remove(foundDep)
                            package.ResolvedDirectDependencies.append(dep)

        package.ResolvedDirectDependencies.sort(key=lambda s: s.Name.lower())
        package.ResolvedAllDependencies.sort(key=lambda s: s.Name.lower())

        #tmp = []
        #for dep in package.ResolvedAllDependencies:
        #    tmp.append(dep.Name)
        #print ("%s -> %s" % (package.Name, ", ".join(tmp)))


    def __TryFindDep(self, deps: List[PackageDependency], findDep: PackageDependency) -> Optional[PackageDependency]:
        for dep in deps:
            if dep.Name == findDep.Name:
                return dep
        return None
