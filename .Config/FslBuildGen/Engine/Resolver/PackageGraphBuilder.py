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
from FslBuildGen.DataTypes import PackageInstanceType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.Order.PackageBuildOrder import PackageBuildOrder
from FslBuildGen.Engine.Resolver.InstanceConfig import InstanceConfig
from FslBuildGen.Engine.Resolver.PackageName import PackageName
from FslBuildGen.Engine.Resolver.PackageResolver import PackageResolver
from FslBuildGen.Engine.Resolver.PackageResolveQueue import PackageResolveQueue
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import EdgeType
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import ResolvedPackageGraph
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import ResolvedPackageGraphNode
from FslBuildGen.Engine.Resolver.ResolvedPackageInstance import ResolvedPackageInstance
from FslBuildGen.Engine.Resolver.ResolvedPackageInstance import ResolvedPackageInstanceDependency
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageTemplate
from FslBuildGen.Engine.Unresolved.UnresolvedBasicPackage import UnresolvedBasicPackage
from FslBuildGen.Log import Log


class LocalVerbosityLevel(object):
    Info = 3
    Debug = 4
    Trace = 5

class PackageGraphBuilder(object):

    @staticmethod
    def Build(log: Log, allPackages: List[UnresolvedBasicPackage]) -> ResolvedPackageGraph:
        buildOrder = PackageBuildOrder.ResolveBuildOrder(log, allPackages)  # type: List[UnresolvedBasicPackage]

        log.LogPrintVerbose(LocalVerbosityLevel.Info, "Building instance graph")
        log.PushIndent()
        try:
            queue = PackageResolveQueue(buildOrder)
            return PackageGraphBuilder.__BuildInstanceGraph(log, queue)
        finally:
            log.PopIndent()


    @staticmethod
    def __BuildInstanceGraph(log: Log, queue: PackageResolveQueue) -> ResolvedPackageGraph:
        graph = ResolvedPackageGraph()
        instanceDict = dict() # type: Dict[PackageName, ResolvedPackageInstance]

        resolver = PackageResolver(log)

        while len(queue) > 0:
            currentPackage = queue.Dequeue()
            if currentPackage.Type != PackageType.TopLevel:
                if log.Verbosity >= LocalVerbosityLevel.Info:
                    log.LogPrint("Resolving package: {0}".format(currentPackage))

                resolvedPackageTemplate = resolver.Resolve(currentPackage)
                PackageGraphBuilder.__AddTemplateNodeAndEdges(graph, resolvedPackageTemplate)

                if len(resolvedPackageTemplate.InstanceConfigs) <= 0:
                    raise Exception("ERROR: no instance configs provided by template")

                isFlavorSingleton = len(resolvedPackageTemplate.InstanceConfigs) <= 1
                for instanceConfig in resolvedPackageTemplate.InstanceConfigs:
                    packageInstance = PackageGraphBuilder.__CreateResolvedPackageInstance(log, instanceDict, resolvedPackageTemplate,
                                                                                          instanceConfig, isFlavorSingleton) # type: ResolvedPackageInstance
                    PackageGraphBuilder.__AddInstanceNodeAndEdges(graph, packageInstance)
            elif len(queue) != 0:
                raise Exception("internal error")
        return graph

    @staticmethod
    def __AddInstanceNodeAndEdges(graph: ResolvedPackageGraph, packageInstance: ResolvedPackageInstance) -> ResolvedPackageGraphNode:
        node = graph.AddNode(packageInstance)
        # Add edges
        for dep in packageInstance.DirectDependencies:
            graph.AddEdge(node, dep.Package, EdgeType.Normal, None, None)

        if packageInstance.FlavorTemplate is not None:
            graph.AddEdge(node, packageInstance.FlavorTemplate, EdgeType.Template, None, None)
        return node

    @staticmethod
    def __AddTemplateNodeAndEdges(graph: ResolvedPackageGraph, resolvedPackageTemplate: ResolvedPackageTemplate) -> ResolvedPackageGraphNode:
        node = graph.AddNode(resolvedPackageTemplate)
        # Add template edges
        for dep in resolvedPackageTemplate.DirectDependencies:
            graph.AddEdge(node, dep.Template, EdgeType.Template, dep.FlavorConstraints, None)

        for flavor in resolvedPackageTemplate.PackageFlavors:
            for option in flavor.Options:
                for dep in option.DirectDependencies:
                    desc = "{0}={1}".format(flavor.Name.Value, option.Name)
                    graph.AddEdge(node, dep.Template, EdgeType.TemplateFlavor, dep.FlavorConstraints, desc)

        return node

    @staticmethod
    def __CreateResolvedPackageInstance(log: Log, instanceDict: Dict[PackageName, ResolvedPackageInstance],
                                        resolvedPackageTemplate: ResolvedPackageTemplate,
                                        instanceConfig: InstanceConfig,
                                        isFlavorSingleton: bool) -> ResolvedPackageInstance:
        packageInstanceName = PackageName.CreateNameAndSelection(resolvedPackageTemplate.Name, instanceConfig.FlavorSelections, isFlavorSingleton)
        if log.Verbosity >= LocalVerbosityLevel.Info:
            log.LogPrint("- Creating instance: {0}".format(packageInstanceName))

        # lookup all instances for the direct dependencies
        directDependencies = [] # type: List[ResolvedPackageInstanceDependency]
        for source in instanceConfig.DirectDependencies:
            if source.Name not in instanceDict:
                raise Exception("Package '{0}' Dependency instance '{1}' not found".format(resolvedPackageTemplate.Name, source.Name))
            depInstance = instanceDict[source.Name]
            directDependencies.append(ResolvedPackageInstanceDependency(depInstance, source.Source))

        instanceType = PackageInstanceType.Normal
        if instanceConfig.IsDynamicFlavor:
            instanceType =  PackageInstanceType.FlavorSingleton if isFlavorSingleton else PackageInstanceType.Flavor

        # Since the instance and the template its created from is basically the same we can just say that this instances-flavor template is the same
        # as the instance-template's flavor-template.  This will remove what is basically duplicated information from the graph
        instance = ResolvedPackageInstance(packageInstanceName, resolvedPackageTemplate.Type, directDependencies, instanceConfig.FlavorSelections,
                                           resolvedPackageTemplate, instanceType)
        instanceDict[instance.Name] = instance
        return instance
