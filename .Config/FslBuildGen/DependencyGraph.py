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

from FslBuildGen.Exceptions import *

class DependencyGraphNode:
    def __init__(self, package):
        self.Name = package.Name
        self.From = []
        self.To = []
        self.Package = package

    def AddEdge(self, toNode):
        if not toNode in self.To:
            self.To.append(toNode)
            toNode.From.append(self)

    def RemoveToEdges(self):
        for node in self.To:
            node.From.remove(self)

    def RemoveFromEdges(self):
        for node in self.From:
            node.To.remove(self)


class DependencyGraph:
    def __init__(self, package): #, exploreVariants):
        #self.ExploreVariants = exploreVariants

        self.UniqueNodeDict = {}
        if package != None:
            self.AddNode(package)
            self.Finalize()
        else:
            self.Nodes = []

    def Get(self, package):
        for node in self.Nodes:
            if node.Package == package:
                return node
        return None

    def RemoveNodesWithNoDependencies(self):
        """ This is useful for finding the build order
        """
        removeList = []
        for node in self.Nodes:
            if len(node.To) <= 0:
                removeList.append(node)

        for node in removeList:
            node.RemoveFromEdges()
            self.Nodes.remove(node)
        return removeList

    def GetNodesWithNoIncomingDependencies(self):
        resultList = []
        for node in self.Nodes:
            if len(node.From) <= 0:
                resultList.append(node)
        return resultList

    def RemoveNodesWithNoIncomingDependencies(self):
        """ This is useful for finding the dependency order
        """
        removeList = self.GetNodesWithNoIncomingDependencies()
        for node in removeList:
            node.RemoveToEdges()
            self.Nodes.remove(node)
        return removeList

    def AddNode(self, package):
        if not package in self.UniqueNodeDict:
            self.UniqueNodeDict[package] = DependencyGraphNode(package)
            for dep in package.ResolvedDirectDependencies:
                self.AddNode(dep.Package)
            #if self.ExploreVariants:
            #    for dep in package.ResolvedDirectVariantDependencies:
            #        self.AddNode(dep.Package)


    def AddNodeAndEdges(self, package):
        if not package in self.UniqueNodeDict:
            newNode = DependencyGraphNode(package)
            for dep in package.ResolvedDirectDependencies:
                if not dep.Package in self.UniqueNodeDict:
                    raise UsageErrorException("Unknown dependency to: '%s'" % (dep.Name));
                newNode.AddEdge(self.UniqueNodeDict[dep.Package])
            self.UniqueNodeDict[package] = newNode
            self.Nodes.append(newNode)

    def Finalize(self):
        self.Nodes = list(self.UniqueNodeDict.values())
        for key, value in self.UniqueNodeDict.items():
            for dep in key.ResolvedDirectDependencies:
                value.AddEdge(self.UniqueNodeDict[dep.Package])
