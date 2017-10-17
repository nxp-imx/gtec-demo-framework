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
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.Packages.Package import Package

class DependencyGraphNode:
    def __init__(self, package: Package) -> None:
        self.Name = package.Name  # type: str
        self.From = []  # type: List['DependencyGraphNode']
        self.To = []  # type: List['DependencyGraphNode']
        self.Package = package  # type: Package

    def AddEdge(self, toNode: 'DependencyGraphNode') -> None:
        if not toNode in self.To:
            self.To.append(toNode)
            toNode.From.append(self)

    def RemoveToEdges(self) -> None:
        for node in self.To:
            node.From.remove(self)

    def RemoveFromEdges(self) -> None:
        for node in self.From:
            node.To.remove(self)


class DependencyGraph:
    def __init__(self, package: Optional[Package]) -> None: #, exploreVariants):
        #self.ExploreVariants = exploreVariants

        self.UniqueNodeDict = {}  # type: Dict[Package, DependencyGraphNode]
        self.Nodes = []  # type: List[DependencyGraphNode]

        if package is not None:
            self.AddNode(package)
            self.Finalize()


    def Get(self, package: Package) -> Optional[DependencyGraphNode]:
        for node in self.Nodes:
            if node.Package == package:
                return node
        return None

    def RemoveNodesWithNoDependencies(self) -> List[DependencyGraphNode]:
        """ This is useful for finding the build order
        """
        removeList = []  # type: List[DependencyGraphNode]
        for node in self.Nodes:
            if len(node.To) <= 0:
                removeList.append(node)

        for node in removeList:
            node.RemoveFromEdges()
            self.Nodes.remove(node)
        return removeList


    def GetNodesWithNoIncomingDependencies(self) -> List[DependencyGraphNode]:
        resultList = []  # type: List[DependencyGraphNode]
        for node in self.Nodes:
            if len(node.From) <= 0:
                resultList.append(node)
        return resultList


    def RemoveNodesWithNoIncomingDependencies(self) -> List[DependencyGraphNode]:
        """ This is useful for finding the dependency order
        """
        removeList = self.GetNodesWithNoIncomingDependencies()
        for node in removeList:
            node.RemoveToEdges()
            self.Nodes.remove(node)
        return removeList


    def AddNode(self, package: Package) -> None:
        if not package in self.UniqueNodeDict:
            self.UniqueNodeDict[package] = DependencyGraphNode(package)
            for dep in package.ResolvedDirectDependencies:
                self.AddNode(dep.Package)
            #if self.ExploreVariants:
            #    for dep in package.ResolvedDirectVariantDependencies:
            #        self.AddNode(dep.Package)


    def AddNodeAndEdges(self, package: Package) -> None:
        if not package in self.UniqueNodeDict:
            newNode = DependencyGraphNode(package)
            for dep in package.ResolvedDirectDependencies:
                if not dep.Package in self.UniqueNodeDict:
                    raise UsageErrorException("Unknown dependency to: '{0}'".format(dep.Name))
                newNode.AddEdge(self.UniqueNodeDict[dep.Package])
            self.UniqueNodeDict[package] = newNode
            self.Nodes.append(newNode)


    def Finalize(self) -> None:
        self.Nodes = list(self.UniqueNodeDict.values())
        for key, value in list(self.UniqueNodeDict.items()):
            for dep in key.ResolvedDirectDependencies:
                value.AddEdge(self.UniqueNodeDict[dep.Package])
