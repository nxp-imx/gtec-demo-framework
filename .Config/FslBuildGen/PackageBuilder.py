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

from FslBuildGen.DataTypes import *
from FslBuildGen.DependencyGraph import DependencyGraph
from FslBuildGen.Exceptions import *
from FslBuildGen.Package import Package
from FslBuildGen.PackageDependency import PackageDependency
from FslBuildGen.PackageManager import PackageManager
from FslBuildGen import Util
from FslBuildGen.XmlStuff import XmlGenFile

class FakeXmlGenFileDependency(object):
    def __init__(self, name, access):
        super(FakeXmlGenFileDependency, self).__init__()
        self.Name = name
        self.Access = access


class PackageBuilder(object):
    def __init__(self, config, platformName, genFiles):
        super(PackageBuilder, self).__init__()

        # create top level package and resolve build order
        config.LogPrint("  Validating dependencies")

        packageManager = PackageManager(platformName, genFiles)
        packages = packageManager.Packages

        # Build a graph containing all packages
        graph = DependencyGraph(None) #, True)
        for package in packages:
            graph.AddNode(package)
        graph.Finalize()

        # Extract the top level nodes
        nodes = graph.GetNodesWithNoIncomingDependencies()
        useFallback = True
        if len(nodes) > 0:
            topLevelGenFile = XmlGenFile(config.ToolConfig.DefaultPackageLanguage)
            topLevelGenFile.Name = 'SYS__TopLevel'
            topLevelGenFile.SetType(PackageType.TopLevel)
            for entry in nodes:
                topLevelGenFile.DirectDependencies.append(FakeXmlGenFileDependency(entry.Name, AccessType.Public))

            topLevelGenFile.DirectDependencies.sort(key=lambda s: s.Name.lower())
        else:
            # We have circular dependencies and couldnt find any starting nodes
            # so generate a empty top level node and expect a circular dependency
            # error to be caught
            topLevelGenFile = XmlGenFile(config.ToolConfig.DefaultPackageLanguage)
            topLevelGenFile.Name = 'SYS__TopLevel'
            topLevelGenFile.SetType(PackageType.TopLevel)

        topLevelPackage = packageManager.CreatePackage(platformName, topLevelGenFile, True)
        graph.AddNodeAndEdges(topLevelPackage)

        # Since we need to resolve the build order we might as well verify dependencies at the same time
        self.__ValidateDependencies(config, packages)
        self.AllPackages = packages
        self.TopLevelPackage = topLevelPackage
        self.__ResolveAllPackageDependencies(config, topLevelPackage)


    def __ValidateDependencies(self, config, packages):
        exceptionList = []
        exceptionList2 = []
        for package in packages:
            try:
                allDependenciesDict = {}
                stack = [package]
                graph = DependencyGraph(package) #, True)
                orderedDependencyList = []
                self.__ValidateDependenciesFor(orderedDependencyList, package, graph)
                orderedDependencyList.reverse()
                package.ResolvedBuildOrder = orderedDependencyList
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


    def __ValidateDependenciesFor(self, orderedDependencyList, package, graph):
        packageNode = graph.Get(package)
        while len(graph.Nodes) > 0:
            nodes = graph.RemoveNodesWithNoIncomingDependencies()
            if len(nodes) <= 0:
                self.HandleCircularDependencies(graph, packageNode)
            nodes.sort(key = lambda node: node.Name.lower())
            for node in nodes:
                if node.Package != packageNode:
                    orderedDependencyList.append(node.Package)


    def HandleCircularDependencies(self, graph, packageNode):
        keepRemoving = True
        while keepRemoving:
            keepRemoving = len(graph.RemoveNodesWithNoDependencies()) > 0

        # check if this package is actually part of the circular dependency or not
        if not packageNode in graph.Nodes:
            raise CircularDependencyInDependentModuleException("'%s' uses a package that has a circular dependency" % (packageNode.Name))

        # We are only interested in the dependencies that start at packageNode
        circularDependencies = []
        dependencies = [packageNode]
        self.BuildDependencyList(circularDependencies, dependencies, packageNode)

        circularDepStrings = set()
        for circularDep in circularDependencies:
            circularDepStrings.add(self.GetCircularDependencyString(circularDep))

        circularDepStrings = list(circularDepStrings)
        circularDepStrings.sort(key = lambda str: (self.__CountArrows(str), str.lower()))

        raise CircularDependencyException("Circular dependency detected while validating " + packageNode.Name + ":\n  " + "\n  ".join(circularDepStrings))


    def BuildDependencyList(self, circularDependencies, dependencies, node):
        for depNode in node.To:
            if not depNode in dependencies:
                dependencies.append(depNode)
                self.BuildDependencyList(circularDependencies, dependencies, depNode)
                dependencies.pop()
            else:
                circularDependencyList = list(dependencies)
                circularDependencyList.append(depNode)
                circularDependencies.append(circularDependencyList)


    def GetCircularDependencyString(self, list):
        if list == None or len(list) < 1:
            raise UsageErrorException("No circular dependency exist in the supplied list")
        try:
            index = list.index(list[-1])
            return self.GetNameListString(list[index:], "->")
        except ValueError:
            raise UsageErrorException("No circular dependency exist in the supplied list")


    def GetNameListString(self, list, seperator):
        str = ""
        isFirst = True
        for entry in list:
            if isFirst:
                str = entry.Name
                isFirst = False
            else:
                str += seperator + entry.Name
        return str


    def __CountArrows(self, str):
        count = 0
        index = str.find("->")
        lenStr = len(str)
        while index >= 0 and (index + 2) < lenStr:
            ++count
            index = str.find("->", index + 2)
        return count


    def __ResolveAllPackageDependencies(self, config, topLevel):
        for package in topLevel.ResolvedBuildOrder:
            self.__DoResolveAllPackageDependencies(config, package)


    def __DoResolveAllPackageDependencies(self, config, package):
        # FIX: we are doing some of the same checks twice here
        addedDict = {}
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
                        foundDep = self.__FindDep(package.ResolvedDirectDependencies, dep)
                        if foundDep != None:
                            config.DoPrint("WARNING: Requested dependency access to '%s', overwritten by dependency from '%s" % (dep.Name, directDep.Name));
                            package.ResolvedDirectDependencies.remove(foundDep)
                            package.ResolvedDirectDependencies.append(dep)

        package.ResolvedDirectDependencies.sort(key=lambda s: s.Name.lower())
        package.ResolvedAllDependencies.sort(key=lambda s: s.Name.lower())

        #tmp = []
        #for dep in package.ResolvedAllDependencies:
        #    tmp.append(dep.Name)
        #print ("%s -> %s" % (package.Name, ", ".join(tmp)))


    def __FindDep(self, deps, findDep):
        for dep in deps:
            if dep.Name == findDep.Name:
                return dep
        return None
