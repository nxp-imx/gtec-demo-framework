#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
#    * Neither the name of the NXP. nor the names of
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
from FslBuildGen.Exceptions import CircularDependencyException
from FslBuildGen.Engine.Order.Exceptions import CircularDependencyInDependentPackageException
from FslBuildGen.Engine.Order.EvaluationPackage import EvaluationPackage

class DependencyGraphNode(object):
    def __init__(self, source: EvaluationPackage) -> None:
        super().__init__()
        self.Source = source
        self.From = []   # type: List[DependencyGraphNode]
        self.To = []   # type: List[DependencyGraphNode]

    def AddEdge(self, toNode: 'DependencyGraphNode') -> None:
        if toNode == self:
            raise Exception("Can't add edge to self")

        if toNode not in self.To:
            self.To.append(toNode)
            toNode.From.append(self)

    def RemoveEdges(self) -> None:
        for edge in self.From:
            edge.To.remove(self)

        for edge in self.To:
            edge.From.remove(self)

        self.From.clear()
        self.To.clear()

    def __str__(self) -> str:
        return str(self.Source)

class DependencyGraph(object):
    def __init__(self, allPackages: Optional[List[EvaluationPackage]] = None) -> None:
        super().__init__()
        self.__uniqueNodeDict = dict() # type: Dict[EvaluationPackage, DependencyGraphNode]
        self.__nodes = [] # type: List[DependencyGraphNode]
        if allPackages is not None:
            self.AddAllNodes(allPackages)
            self.AddAllDependencies(allPackages)

    def Empty(self) -> bool:
        return len(self.__nodes) <= 0

    def DebugNodes(self) -> List[DependencyGraphNode]:
        return self.__nodes

    def Contains(self, package: EvaluationPackage) -> bool:
        return package in self.__uniqueNodeDict

    def GetNode(self, package: EvaluationPackage) -> DependencyGraphNode:
        return self.__uniqueNodeDict[package]

    def Remove(self, node: DependencyGraphNode) -> None:
        node.RemoveEdges()
        self.__uniqueNodeDict.pop(node.Source, None)
        self.__nodes.remove(node)

    def AddNode(self, package: EvaluationPackage) -> DependencyGraphNode:
        node = DependencyGraphNode(package)
        self.__uniqueNodeDict[package] = node
        self.__nodes.append(node)
        return node

    def AddEdge(self, fromObj: Union[DependencyGraphNode, EvaluationPackage], toObj: Union[DependencyGraphNode, EvaluationPackage]) -> None:
        if isinstance(fromObj, EvaluationPackage):
            if fromObj in self.__uniqueNodeDict:
                fromNode = self.__uniqueNodeDict[fromObj]
        else:
            fromNode = fromObj
            #if fromNode not in self.__nodes:
            #    raise Exception("Unknown node: '{0}'".format(fromNode))

        if isinstance(toObj, EvaluationPackage):
            if toObj in self.__uniqueNodeDict:
                toNode = self.__uniqueNodeDict[toObj]
        else:
            toNode = toObj
            #if toNode not in self.__nodes:
            #    raise Exception("Unknown node: '{0}'".format(toNode))

        fromNode.AddEdge(toNode)


    def FindNodesWithNoIncomingDependencies(self) -> List[DependencyGraphNode]:
        return [entry for entry in self.__nodes if len(entry.From) <= 0]

    def FindNodesWithNoOutgoingDependencies(self) -> List[DependencyGraphNode]:
        return [entry for entry in self.__nodes if len(entry.To) <= 0]


    def AddAllNodes(self, allPackages: List[EvaluationPackage]) -> None:
        for package in allPackages:
            self.AddNode(package)


    def AddPackageDirectDependencies(self, node: DependencyGraphNode) -> None:
        for depPackage in node.Source.DirectDependencies:
            self.AddEdge(node, depPackage.Package)


    def AddAllDependencies(self, allPackages: List[EvaluationPackage]) -> None:
        for package in allPackages:
            self.AddPackageDirectDependencies(self.GetNode(package))

    def DetermineBuildOrder(self, rootPackage: EvaluationPackage) -> List[EvaluationPackage]:
        """
        This extract the correct build order, but it also clears the graph!
        """
        orderedDependencyList = [] # type: List[EvaluationPackage]

        rootNode = self.GetNode(rootPackage)
        while not self.Empty():
            removedNodes = self.RemoveNodesWithNoIncomingDependencies()  # type: List[DependencyGraphNode]
            if len(removedNodes) <= 0:
                self.__HandleCircularDependencies(rootNode)

            removedNodes.sort(key=lambda s: s.Source.Name.Value.upper())

            for node in removedNodes:
                if node != rootNode:
                    orderedDependencyList.append(node.Source)

        orderedDependencyList.reverse()
        return orderedDependencyList


    def __RemoveNodesWithNoOutgoingDependencies(self) -> List[DependencyGraphNode]:
        removeList = self.FindNodesWithNoOutgoingDependencies() # type: List[DependencyGraphNode]
        for node in removeList:
            self.Remove(node)
        return removeList

    def RemoveNodesWithNoIncomingDependencies(self) -> List[DependencyGraphNode]:
        """
        This is useful for finding the dependency order
        """
        removeList = self.FindNodesWithNoIncomingDependencies() # type: List[DependencyGraphNode]
        for node in removeList:
            self.Remove(node)
        return removeList

    def __HandleCircularDependencies(self, node: DependencyGraphNode) -> None:
        # Nodes without outgoing dependencies can not be part of the cycle -> so we remove them to simplify the graph
        while (len(self.__RemoveNodesWithNoOutgoingDependencies()) > 0):
            pass

        # check if this package is actually part of the circular dependency or not
        if not self.Contains(node.Source):
            raise CircularDependencyInDependentPackageException("'{0}' uses a package that has a circular dependency".format(node.Source.Name))

        # We are only interested in the dependencies that start at packageNode
        circularDependencies = [] # type: List[List[DependencyGraphNode]]
        dependencies = [node] # type: List[DependencyGraphNode]
        DependencyGraph.__BuildDependencyList(circularDependencies, dependencies, node)

        circularDepStringsSet = set() # type: Set[str]
        for circularDep in circularDependencies:
            circularDepStringsSet.add(DependencyGraph.__GetCircularDependencyString(circularDep))

        circularDepStrings = list(circularDepStringsSet) # type: List[str]
        circularDepStrings.sort(key=lambda s: (DependencyGraph.__CountArrows(s), s.upper()))

        strDependencies = "\n  ".join(circularDepStrings)
        raise CircularDependencyException("Circular dependency detected while validating {0}:\n  {1}".format(node.Source.Name.Value, strDependencies))

    @staticmethod
    def __CountArrows(strContent: str) -> int:
        count = 0
        index = strContent.find("->")
        lenStr = len(strContent)
        while index >= 0 and ((index + 2) < lenStr):
            count = count + 1
            index = strContent.find("->", index + 2)
        return count

    @staticmethod
    def __GetCircularDependencyString(srcList: List[DependencyGraphNode]) -> str:
        if srcList is None or len(srcList) < 2:
            raise Exception("No circular dependency exist in the supplied list")

        # Since the last element is where the circular dependency is detected
        # we need to find the first occurrence of the last element
        endIndex = len(srcList) - 1
        lastElement = srcList[endIndex]
        foundIndex = srcList.index(lastElement) if lastElement in srcList else -1
        if foundIndex < 0 or foundIndex == endIndex:
            raise Exception("No circular dependency exist in the supplied list")
        del srcList[0:foundIndex]
        return "->".join([entry.Source.Name.Value for entry in srcList])


    @staticmethod
    def __BuildDependencyList(circularDependencies: List[List[DependencyGraphNode]], dependencies: List[DependencyGraphNode], node: DependencyGraphNode) -> None:
        for depNode in node.To:
            if depNode not in dependencies:
                dependencies.append(depNode)
                DependencyGraph.__BuildDependencyList(circularDependencies, dependencies, depNode)
                dependencies.pop()
            else:
                circularDependencyList = list(dependencies) # type: List[DependencyGraphNode]
                circularDependencyList.append(depNode)
                circularDependencies.append(circularDependencyList)
