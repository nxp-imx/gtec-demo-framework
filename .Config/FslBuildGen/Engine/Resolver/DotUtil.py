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
from enum import Enum
import os
import subprocess
from FslBuildGen import IOUtil
from FslBuildGen.Engine.PackageFlavorName import PackageFlavorName
from FslBuildGen.Engine.PackageFlavorOptionName import PackageFlavorOptionName
from FslBuildGen.Engine.PackageFlavorSelections import PackageFlavorSelections
from FslBuildGen.Engine.Resolver.ResolvedPackage import ResolvedPackage
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import EdgeRecord
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import EdgeType
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import ResolvedPackageGraph
from FslBuildGen.Engine.Resolver.ResolvedPackageGraph import ResolvedPackageGraphNode
from FslBuildGen.Engine.Resolver.ResolvedPackageInstance import ResolvedPackageInstance
from FslBuildGen.Engine.Resolver.ResolvedPackageTemplate import ResolvedPackageTemplate
from FslBuildGen.Log import Log

class LocalVerbosityLevel(object):
    Info = 3
    Debug = 4
    Trace = 5

class RenderMode(Enum):
    Templates = 0
    Instances = 1
    Full = 2
    Raw = 3


class DotUtil(object):

    @staticmethod
    def __ToDotFile(graph: ResolvedPackageGraph) -> List[str]:
        renderMode = RenderMode.Full

        instanceNodes = []  # type: List[ResolvedPackageGraphNode]
        templateNodes = []  # type: List[ResolvedPackageGraphNode]

        allNodes = graph.DebugNodes()
        allowedTemplateFilterSet = set() # type: Set[str]
        DotUtil.__Extract(instanceNodes, templateNodes, allNodes)
        DotUtil.__ExtractUsedTemplates(allowedTemplateFilterSet, instanceNodes)

        nodeNameDict = {}  # type: Dict[ResolvedPackage, str]
        for node in allNodes:
            nodeNameDict[node.Source] = DotUtil.__ToNodeName(node.Source, renderMode != RenderMode.Raw)


        res = [] # type: List[str]
        res.append("digraph xmlTest")
        res.append("{")
        res.append("  overlap=scale;")
        res.append("  splines=true;")
        res.append("  edge [len=1];")

        if renderMode == RenderMode.Templates:
            DotUtil.__AddTemplateNodes(nodeNameDict, res, templateNodes)
        elif renderMode == RenderMode.Instances:
            DotUtil.__AddInstanceNodes(nodeNameDict, res, instanceNodes)
        elif renderMode == RenderMode.Full:
            DotUtil.__AddTemplateNodes(nodeNameDict, res, templateNodes)
            DotUtil.__AddInstanceNodes(nodeNameDict, res, instanceNodes)
        elif renderMode == RenderMode.Raw:
            DotUtil.__AddTemplateNodes(nodeNameDict, res, templateNodes)
            DotUtil.__AddInstanceNodes(nodeNameDict, res, instanceNodes)
        else:
            raise Exception("Unsupported renderMode")

        if renderMode == RenderMode.Raw:
            for node in allNodes:
                for toRecord in node.To:
                    if isinstance(toRecord.Node.Source, ResolvedPackageInstance):
                        res.append("  {0} -> {1}".format(nodeNameDict[node.Source], nodeNameDict[toRecord.Node.Source]))
        elif renderMode == RenderMode.Full or renderMode == RenderMode.Instances:
            for node in instanceNodes:
                for toRecord in node.To:
                    if isinstance(toRecord.Node.Source, ResolvedPackageInstance) and DotUtil.__IsFirstDependencyReference(node.To, toRecord.Node):
                        res.append("  {0} -> {1}".format(nodeNameDict[node.Source], nodeNameDict[toRecord.Node.Source]))

        res.append("  edge [color=Blue, style=dashed]")

        edgeTypeFilter = EdgeType.Template
        if renderMode == RenderMode.Full or renderMode == RenderMode.Raw:
            DotUtil.__AddEdgesFull(nodeNameDict, allNodes, res, edgeTypeFilter, False)
        elif renderMode == RenderMode.Templates:
            DotUtil.__AddEdgesTemplates(nodeNameDict, allNodes, res, edgeTypeFilter, True)

        res.append("  edge [color=Orange, style=dotted]")
        edgeTypeFilter = EdgeType.TemplateFlavor
        if renderMode == RenderMode.Full or renderMode == RenderMode.Raw:
            DotUtil.__AddEdgesFull(nodeNameDict, allNodes, res, edgeTypeFilter, False)
        elif renderMode == RenderMode.Templates:
            DotUtil.__AddEdgesTemplates(nodeNameDict, allNodes, res, edgeTypeFilter, True)

        res.append("}")
        return res

    @staticmethod
    def __AddEdgesTemplates(nodeNameDict: Dict[ResolvedPackage, str], allNodes: List[ResolvedPackageGraphNode], res: List[str],
                            edgeTypeFilter: EdgeType, removeInherited: bool) -> None:
        for node in allNodes:
            for toRecord in node.To:
                if (toRecord.Type == edgeTypeFilter and isinstance(node.Source, ResolvedPackageTemplate) and
                        isinstance(toRecord.Node.Source, ResolvedPackageTemplate) and
                        (not removeInherited or DotUtil.__IsFirstDependencyReference(node.To, toRecord.Node))):
                    res.append("  {0} -> {1}".format(nodeNameDict[node.Source], nodeNameDict[toRecord.Node.Source]))

    @staticmethod
    def __AddEdgesFull(nodeNameDict: Dict[ResolvedPackage, str], allNodes: List[ResolvedPackageGraphNode], res: List[str], edgeTypeFilter: EdgeType,
                       removeInherited: bool) -> None:
        for node in allNodes:
            for toRecord in node.To:
                if toRecord.Type == edgeTypeFilter:
                    toPackage = toRecord.Node.Source
                    if isinstance(toPackage, ResolvedPackageTemplate) and (not removeInherited or DotUtil.__IsFirstDependencyReference(node.To, toRecord.Node)):
                        strEdge = "  {0} -> {1}".format(nodeNameDict[node.Source], nodeNameDict[toPackage])

                        label = None # type: Optional[str]
                        if toRecord.Type == EdgeType.TemplateFlavor:
                            label = toRecord.Desc
                        if toRecord.Constraint is not None:
                            strConstraint = DotUtil.__TryExtractLabel(toRecord.Constraint)
                            if strConstraint is not None:
                                label = "{0}<{1}>".format(label, strConstraint) if label is not None else  "<{0}>".format(strConstraint)
                        if label is not None:
                            strEdge += "[taillabel = \"{0}\"]".format(label)
                        res.append(strEdge)

    @staticmethod
    def __IsFirstDependencyReference(edges: List[EdgeRecord], srcNode: ResolvedPackageGraphNode) -> bool:
        visitedSet = set() # type: Set[ResolvedPackageGraphNode]
        for edge in edges:
            if edge.Node != srcNode:
                if DotUtil.__FindNodeInDependencies(edge.Node.To, srcNode, visitedSet):
                    return False
        return True

    @staticmethod
    def __FindNodeInDependencies(edges: List[EdgeRecord], srcNode: ResolvedPackageGraphNode, visitedSet: Set[ResolvedPackageGraphNode]) -> bool:
        for edge in edges:
            if edge.Node not in visitedSet:
                visitedSet.add(edge.Node)
                if edge.Node == srcNode:
                    return True
                if DotUtil.__FindNodeInDependencies(edge.Node.To, srcNode, visitedSet):
                    return True
        return False

#     private static ResolvedPackageTemplateDependency TryLocateDependency(ResolvedPackageTemplate fromPackage, ResolvedPackageTemplate toPackage)
#     {
#       if (fromPackage == null)
#         return null;

#       foreach (var dep in fromPackage.DirectDependencies)
#       {
#         if (dep.Template.Name == toPackage.Name)
#         {
#           return dep;
#         }
#       }
#       return null;
#     }

    @staticmethod
    def __AddInstanceNodes(nodeNameDict: Dict[ResolvedPackage, str], res: List[str], instanceNodes: List[ResolvedPackageGraphNode]) -> None:
        res.append("  node [color=Black];")
        for node in instanceNodes:
            res.append("  {0}".format(nodeNameDict[node.Source]))

    @staticmethod
    def __AddTemplateNodes(nodeNameDict: Dict[ResolvedPackage, str], res: List[str], templateNodes: List[ResolvedPackageGraphNode],
                           templateNameFilterSet: Optional[Set[str]] = None) -> None:

        indent = "  "
        for node in templateNodes:
            sourceEx = node.Source
            if isinstance(sourceEx, ResolvedPackageTemplate) and (templateNameFilterSet is None or sourceEx.Name.Value in templateNameFilterSet):
                res.append("{0}node [color=blue];".format(indent))
                res.append("{0}{1} [label=\"{2}\"]".format(indent, nodeNameDict[sourceEx], DotUtil.__ToNodeLabel(sourceEx)))

    @staticmethod
    def __ExtractUsedTemplates(allowedTemplateFilterSet: Set[str], instanceNodes: List[ResolvedPackageGraphNode]) -> None:
        foundSet = set() # type: Set[ResolvedPackageTemplate]
        for node in instanceNodes:
            if isinstance(node.Source, ResolvedPackageInstance):
                if node.Source.FlavorTemplate is not None:
                    foundSet.add(node.Source.FlavorTemplate)
                    allowedTemplateFilterSet.add(node.Source.FlavorTemplate.Name.Value)

        resolveSet = set() # type: Set[ResolvedPackageTemplate]

        while len(foundSet) > 0:
            tmp = foundSet
            foundSet = resolveSet
            resolveSet = tmp
            foundSet.clear()

            for entry in resolveSet:
                for dependency in entry.DirectDependencies:
                    if dependency.Template.Name.Value not in allowedTemplateFilterSet:
                        foundSet.add(dependency.Template)
                        allowedTemplateFilterSet.add(dependency.Template.Name.Value)

    @staticmethod
    def __Extract(instanceNodes: List[ResolvedPackageGraphNode], templateNodes: List[ResolvedPackageGraphNode], allNodes: List[ResolvedPackageGraphNode]) -> None:
        for node in allNodes:
            if isinstance(node.Source, ResolvedPackageInstance):
                instanceNodes.append(node)
            else:
                templateNodes.append(node)

#     /// <summary>
#     /// All possible combinations
#     /// </summary>
#     /// <param name="package"></param>
#     /// <returns></returns>
#     //private static string ToCombinationsNodeName(ResolvedPackageTemplate package)
#     //{
#     //  if(package.InstanceConfigs.Count == 0)
#     //    return $"\"{package.Name.Value}<>\"";
#     //  var res = new string[package.InstanceConfigs.Count];
#     //  for (int i = 0; i < res.Length; ++i)
#     //  {
#     //    var instanceConfig = package.InstanceConfigs[i];
#     //    res[i] = instanceConfig.Description;
#     //  }
#     //  return $"\"{package.Name.Value}<{string.Join(" * ", res)}>\"";
#     //}


#     private static string ToNodeLabel2(ResolvedPackageTemplate package)
#     {
#       if (package.PackageFlavors.Count == 0)
#         return $"\"{package.Name.Value}<>\"";
#       var res = new string[package.PackageFlavors.Count];
#       for (int i = 0; i < res.Length; ++i)
#       {
#         res[i] = ExtractFlavorConfigurationString(package.PackageFlavors[i]); ;
#       }
#       return $"{package.Name.Value}<{string.Join(" * ", res)}>";
#     }

    @staticmethod
    def __ToNodeLabel(package: ResolvedPackageTemplate) -> str:
        if len(package.InstanceConfigs) == 0:
            return "\"{0}<>\"".format(package.Name.Value)

        fullFlavorHash = DotUtil.__BuildPartialFlavorDict(package)
        flavorDict = DotUtil.__BuildUsageDict(package)
        res = DotUtil.__ExtractFlavorCombination(flavorDict, fullFlavorHash)
        return "{0}<{1}>".format(package.Name.Value, ", ".join(res))

    @staticmethod
    def __ExtractFlavorCombination(flavorDict: Dict[PackageFlavorName, Set[PackageFlavorOptionName]], fullFlavorHash: Set[PackageFlavorName]) -> List[str]:
        flavorNames = [] # type: List[PackageFlavorName]
        for flavorName in flavorDict.keys():
            flavorNames.append(flavorName)
        flavorNames.sort(key=lambda s: s.Value.upper())

        res = [] # type: List[str]
        for flavorName in flavorNames:
            entry = flavorDict[flavorName]
            options = [] # type: List[str]

            for optionEntry in entry:
                options.append(optionEntry.Value)
            options.sort(key=lambda s: s.upper())

            if flavorName in fullFlavorHash:
                res.append("{0}={1}".format(flavorName, "|".join(options)))
            else:
                res.append("[{0}={1}]".format(flavorName, "|".join(options)))
        return res

    @staticmethod
    def __BuildUsageDict(package: ResolvedPackageTemplate) -> Dict[PackageFlavorName, Set[PackageFlavorOptionName]]:
        flavorDict = {} # type: Dict[PackageFlavorName, Set[PackageFlavorOptionName]]
        for instanceConfig in package.InstanceConfigs:
            for flavor in instanceConfig.FlavorSelections.Selections:
                if flavor.Name not in flavorDict:
                    optionSet = set() # type: Set[PackageFlavorOptionName]
                    flavorDict[flavor.Name] = optionSet
                else:
                    optionSet = flavorDict[flavor.Name]
                optionSet.add(flavor.Option)
        return flavorDict

    @staticmethod
    def __BuildPartialFlavorDict(package: ResolvedPackageTemplate) -> Set[PackageFlavorName]:
        flavorDict = {} # type: Dict[PackageFlavorName, int]
        for instanceConfig in package.InstanceConfigs:
            for flavor in instanceConfig.FlavorSelections.Selections:
                if flavor.Name in flavorDict:
                    flavorDict[flavor.Name] += 1
                else:
                    flavorDict[flavor.Name] = 1

        targetHitCount = len(package.InstanceConfigs)
        fullFlavorHash = set() # type: Set[PackageFlavorName]
        for pairKey, pairValue in flavorDict.items():
            if pairValue == targetHitCount:
                fullFlavorHash.add(pairKey)
        return fullFlavorHash

#     private static string ExtractFlavorConfigurationString(ResolvedPackageFlavor flavor)
#     {
#       if (flavor.Options.Length == 1)
#         return $"{flavor.Name.Value}={flavor.Options[0].Name}";
#       var res = new string[flavor.Options.Length];
#       for (int i = 0; i < res.Length; ++i)
#       {
#         res[i] = flavor.Options[i].Name.Value;
#       }
#       return $"{flavor.Name.Value}={string.Join("|", res)}";
#     }

    @staticmethod
    def __ToResolvedPackageTemplateNodeName(package: ResolvedPackageTemplate, useSmartName: bool) -> str:
        return '"{0}<>"'.format(package.Name.SmartValue if useSmartName else package.Name.Value)

    @staticmethod
    def __ToResolvedPackageInstanceNodeName(package: ResolvedPackageInstance, useSmartName: bool) -> str:
        return '"{0}"'.format(package.Name.SmartValue if useSmartName else package.Name.Value)

    @staticmethod
    def __ToNodeName(package: ResolvedPackage, useSmartName: bool = True) -> str:
        if isinstance(package, ResolvedPackageTemplate):
            return DotUtil.__ToResolvedPackageTemplateNodeName(package, useSmartName)
        if isinstance(package, ResolvedPackageInstance):
            return DotUtil.__ToResolvedPackageInstanceNodeName(package, useSmartName)
        raise Exception("ToNodeName failed")

#     //private static string TryExtractLabel(ResolvedPackageTemplate from, ResolvedPackageTemplate to)
#     //{
#     //  foreach(var dep in from.DirectDependencies)
#     //  {
#     //    if(dep.Template == to)
#     //    {
#     //      return TryExtractLabel(dep.FlavorConstraint);
#     //    }
#     //  }
#     //  return null;
#     //}

    @staticmethod
    def __TryExtractLabel(depFlavor: PackageFlavorSelections) -> Optional[str]:
        if depFlavor is not None and len(depFlavor.Selections) > 0:
            flavors = [] # type: List[str]
            for entry in depFlavor.Selections:
                flavors.append("{0}={1}".format(entry.Name.Value, entry.Option.Value))
            return ", ".join(flavors)
        return None

    @staticmethod
    def ToFile(log: Log, filename: str, graph: ResolvedPackageGraph) -> None:
        lines = DotUtil.__ToDotFile(graph)
        content = "\n".join(lines)

        dotFilename = "{0}.dot".format(filename)

        log.LogPrintVerbose(LocalVerbosityLevel.Debug, "Writing dot file to '{0}'".format(dotFilename))
        IOUtil.WriteFileIfChanged(dotFilename, content)

        outputFile = "{0}.png".format(filename)

        try:
            log.LogPrintVerbose(LocalVerbosityLevel.Debug, "Writing png file to '{0}'".format(outputFile))
            subprocess.call(["dot", "-Tpng", "-o{0}".format(outputFile), dotFilename])
            os.remove(dotFilename)
        except Exception as ex:
            print("WARNING: Failed to execute dot, is it part of the path?")
            os.remove(dotFilename)
            raise
