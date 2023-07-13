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
from FslBuildGen.DataTypes import PackageInstanceType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.ComplexExternalFlavorConstraints import ComplexExternalFlavorConstraints
from FslBuildGen.Engine.EngineResolveConfig import EngineResolveConfig
from FslBuildGen.Engine.EngineResolveConfig import ExternalFlavorConstraintHelp
from FslBuildGen.Engine.EngineResolveConfig import FlavorResolveConstraints
from FslBuildGen.Engine.ExternalFlavorConstraints import ExternalFlavorConstraints
from FslBuildGen.Engine.Order.Exceptions import PackageExternalFlavorConstraintMustBeSetException
from FslBuildGen.Engine.Order.PackageBuildOrder import PackageBuildOrder
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
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
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.Unresolved.UnresolvedBasicPackage import UnresolvedBasicPackage
from FslBuildGen.Log import Log


class LocalVerbosityLevel(object):
    Info = 3
    Debug = 4
    Trace = 5


class BuildResult(object):
    def __init__(self, graph: Optional[ResolvedPackageGraph],
                       generatedFlavorConstraints: Optional[ComplexExternalFlavorConstraints]) -> None:
        super().__init__()
        self.Graph = graph
        self.GeneratedFlavorConstraints = generatedFlavorConstraints


class PackageGraphBuilder(object):

    @staticmethod
    def Build(log: Log, allPackages: List[UnresolvedBasicPackage],
              flavorConstraints: ExternalFlavorConstraints, engineResolveConfig: EngineResolveConfig) -> ResolvedPackageGraph:

        result = PackageGraphBuilder.__BuildNow(log, allPackages, ComplexExternalFlavorConstraints(flavorConstraints, dict()), engineResolveConfig)
        if result.Graph is not None:
            return result.Graph
        if result.GeneratedFlavorConstraints is not None:
            log.LogPrintVerbose(LocalVerbosityLevel.Info, "** Instance graph requires flavor constraints, trying default constraints **")
            newEngineResolveConfig = EngineResolveConfig.ModifyExternalFlavorConstraintHelp(engineResolveConfig, ExternalFlavorConstraintHelp.Disabled)
            result = PackageGraphBuilder.__BuildNow(log, allPackages, result.GeneratedFlavorConstraints, newEngineResolveConfig)
            if result.Graph is not None:
                return result.Graph
        raise Exception("Internal error")

    @staticmethod
    def __BuildNow(log: Log, allPackages: List[UnresolvedBasicPackage],
                   flavorConstraints: ComplexExternalFlavorConstraints, engineResolveConfig: EngineResolveConfig) -> BuildResult:

        buildOrder = PackageBuildOrder.ResolveBuildOrder(log, allPackages, flavorConstraints)  # type: List[UnresolvedBasicPackage]
        log.LogPrintVerbose(LocalVerbosityLevel.Info, "Building instance graph")
        log.PushIndent()
        try:
            queue = PackageResolveQueue(buildOrder)
            graph = PackageGraphBuilder.__BuildInstanceGraph(log, queue)
            graph.HasExternalContraints = flavorConstraints.HasConstraints()
            if engineResolveConfig.FlavorResolveConstraints == FlavorResolveConstraints.OnlyAllowOneFlavorPerRoot:
                generatedFlavorConstraints = PackageGraphBuilder.__EnsureOnlyOneFlavorPerRootNode(graph, flavorConstraints.HasConstraints(),
                                                                                                  engineResolveConfig.ExternalFlavorConstraintHelp)
                if generatedFlavorConstraints is not None:
                    mergedConstraints = ComplexExternalFlavorConstraints.Merge(flavorConstraints, generatedFlavorConstraints)
                    return BuildResult(None, mergedConstraints)
            return BuildResult(graph, None)
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
            instanceType = PackageInstanceType.FlavorSingleton if isFlavorSingleton else PackageInstanceType.Flavor

        # Since the instance and the template its created from is basically the same we can just say that this instances-flavor template is the same
        # as the instance-template's flavor-template.  This will remove what is basically duplicated information from the graph
        instance = ResolvedPackageInstance(packageInstanceName, resolvedPackageTemplate.Type, directDependencies, instanceConfig.FlavorSelections,
                                           resolvedPackageTemplate, instanceType)
        instanceDict[instance.Name] = instance
        return instance

    @staticmethod
    def __EnsureOnlyOneFlavorPerRootNode(graph: ResolvedPackageGraph, hasFlavorConstraints: bool,
                                         externalFlavorConstraintHelp: ExternalFlavorConstraintHelp) -> Optional[Dict[str, ExternalFlavorConstraints]]:

        flavorTemplateDict = PackageGraphBuilder.__BuildFlavorTemplateDict(graph, hasFlavorConstraints)

        if externalFlavorConstraintHelp == ExternalFlavorConstraintHelp.Disabled:
            PackageGraphBuilder.__VerifyOnlyOneFlavorPerRootNode(flavorTemplateDict)
        elif externalFlavorConstraintHelp == ExternalFlavorConstraintHelp.SelectDefaultFlavor:
            rootNodeConstraintDict = PackageGraphBuilder.__TryBuildRootNodeDefaultConstraints(flavorTemplateDict)
            return None if len(rootNodeConstraintDict) <= 0 else rootNodeConstraintDict
        else:
            raise Exception("Unsupported ExternalFlavorConstraintHelp: {0}".format(externalFlavorConstraintHelp))
        return None;

    @staticmethod
    def __BuildFlavorTemplateDict(graph: ResolvedPackageGraph, hasFlavorConstraints: bool) -> Dict[ResolvedPackageTemplate, List[ResolvedPackageInstance]]:
        # Run through all root nodes and determine if there are multiple flavors of each
        # if there are we determine which flavors need to be constrained and notify the user.

        flavorTemplateDict = dict() # type: Dict[ResolvedPackageTemplate, List[ResolvedPackageInstance]]
        rootNodes = graph.FindNodesWithNoIncomingDependencies()
        if hasFlavorConstraints:
            for rootNode in rootNodes:
                if rootNode.Source.Type == PackageType.ExternalFlavorConstraint and isinstance(rootNode.Source, ResolvedPackageInstance) and rootNode.Source.InstanceType == PackageInstanceType.Flavor:
                    if rootNode.Source.FlavorTemplate not in flavorTemplateDict:
                        flavorTemplateDict[rootNode.Source.FlavorTemplate] = [rootNode.Source]
                    else:
                        flavorTemplateDict[rootNode.Source.FlavorTemplate].append(rootNode.Source)
        else:
            for rootNode in rootNodes:
                if isinstance(rootNode.Source, ResolvedPackageInstance) and rootNode.Source.InstanceType == PackageInstanceType.Flavor:
                    if rootNode.Source.FlavorTemplate not in flavorTemplateDict:
                        flavorTemplateDict[rootNode.Source.FlavorTemplate] = [rootNode.Source]
                    else:
                        flavorTemplateDict[rootNode.Source.FlavorTemplate].append(rootNode.Source)
        return flavorTemplateDict

    @staticmethod
    def __VerifyOnlyOneFlavorPerRootNode(flavorTemplateDict: Dict[ResolvedPackageTemplate, List[ResolvedPackageInstance]]) -> None:
        flavorOptionDict = dict() # type: Dict[str, Set[str]]
        for instanceList in flavorTemplateDict.values():
            if len(instanceList) > 1:
                PackageGraphBuilder.__DetermineFlavorsThatDiffer(flavorOptionDict, instanceList)
        if len(flavorOptionDict) > 0:
            res = PackageGraphBuilder.__CreateFlavorMustBeConstrainedErrorMessage(flavorOptionDict)
            raise PackageExternalFlavorConstraintMustBeSetException(", ".join(res))


    @staticmethod
    def __TryBuildRootNodeDefaultConstraints(flavorTemplateDict: Dict[ResolvedPackageTemplate, List[ResolvedPackageInstance]]) -> Dict[str, ExternalFlavorConstraints]:
        rootNodeDefaultConstraints = dict() # type: Dict[str, ExternalFlavorConstraints]
        for packageTemplate, instanceList in flavorTemplateDict.items():
            if len(instanceList) > 1:
                flavorOptionDict = dict() # type: Dict[str, Set[str]]
                PackageGraphBuilder.__DetermineFlavorsThatDiffer(flavorOptionDict, instanceList)
                if len(flavorOptionDict) > 0:
                    rootNodeDefaultConstraints[packageTemplate.Name.Value] = PackageGraphBuilder.__BuildTemplateDefaultFlavorConstraints(flavorOptionDict)

        return rootNodeDefaultConstraints

    @staticmethod
    def __BuildTemplateDefaultFlavorConstraints(flavorOptionDict: Dict[str, Set[str]]) -> ExternalFlavorConstraints:
        typedDict = dict() # type: Dict[PackageFlavorName, PackageFlavorOptionName]
        for strFlavorName, flavorOptionNameSet in flavorOptionDict.items():
            if len(flavorOptionNameSet) > 1:
                flavorOptionNames = list(flavorOptionNameSet)
                flavorOptionNames.sort()
                typedDict[PackageFlavorName.FromString(strFlavorName)] = PackageFlavorOptionName(flavorOptionNames[0])
        return ExternalFlavorConstraints(typedDict)

    @staticmethod
    def __DetermineFlavorsThatDiffer(flavorOptionDict: Dict[str, Set[str]], instances: List[ResolvedPackageInstance]) -> None:
        for instance in instances:
            for flavorSelection in instance.FlavorSelections.Selections:
                if flavorSelection.Name.Value not in flavorOptionDict:
                    flavorOptionDict[flavorSelection.Name.Value] = set()
                flavorOptionDict[flavorSelection.Name.Value].add(flavorSelection.Option.Value)

    @staticmethod
    def __CreateFlavorMustBeConstrainedErrorMessage(flavorOptionDict: Dict[str, Set[str]]) -> List[str]:
        sortedFlavorNameList = list(flavorOptionDict.keys())
        sortedFlavorNameList.sort()

        res = [] # type: List[str]
        for flavorName in sortedFlavorNameList:
            flavorOptionList = flavorOptionDict[flavorName]
            if len(flavorOptionList) > 1:
                sortedFlavorOptionList = list(flavorOptionList)
                sortedFlavorOptionList.sort()
                res.append("{0}=[{1}]".format(flavorName, ", ".join(sortedFlavorOptionList)))
        return res
