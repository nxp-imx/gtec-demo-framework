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
from typing import Tuple
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import PackageInstanceType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.BasicBuildConfig import BasicBuildConfig
from FslBuildGen.Engine.Cache.JsonProjectIdCache import JsonProjectIdCache
from FslBuildGen.Engine.Cache.ProjectIdCache import ProjectIdCache
from FslBuildGen.Engine.Resolver.DotUtil import DotUtil
from FslBuildGen.Engine.Resolver.PackageGraphBuilder import PackageGraphBuilder
from FslBuildGen.Engine.Resolver.PreResolver import PreResolver
from FslBuildGen.Engine.Resolver.PreResolvePackageResult import PreResolvePackageResult
from FslBuildGen.Engine.Resolver.ResolvedPackageInstance import ResolvedPackageInstance
from FslBuildGen.Engine.Resolver.ResolvedPackageInstance import ResolvedPackageInstanceDependency
from FslBuildGen.Engine.Resolver.ProcessedFactory import ProcessedFactory
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackage
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackageFlags
from FslBuildGen.Engine.Resolver.ProcessedPackage import ProcessedPackagePaths
from FslBuildGen.Engine.Resolver.ProcessedPackageDependency import ProcessedPackageDependency
from FslBuildGen.Engine.Unresolved.UnresolvedBasicPackage import UnresolvedBasicPackage
from FslBuildGen.Engine.Unresolved.UnresolvedPackageDependency import UnresolvedPackageDependency
from FslBuildGen.Engine.Unresolved.UnresolvedPackageFlavor import UnresolvedPackageFlavor
from FslBuildGen.Engine.Unresolved.UnresolvedPackageName import UnresolvedPackageName
from FslBuildGen.Generator import GeneratorVCUtil
from FslBuildGen.Log import Log
from FslBuildGen.Packages.PackageCustomInfo import PackageCustomInfo
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName
from FslBuildGen.Packages.PackageNameInfo import PackageNameInfo
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedFactory import FactoryCreateContext
from FslBuildGen.Packages.Unresolved.UnresolvedFactory import UnresolvedFactory
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackage
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackageFlags
from FslBuildGen.Packages.Unresolved.UnresolvedPackage import UnresolvedPackagePaths
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement

class LocalVerbosityLevel(object):
    Info = 3
    Debug = 4
    Trace = 5


class PackageManager2(object):
    @staticmethod
    def Resolve(log: Log, allPackages: List[UnresolvedBasicPackage], dump: bool = False) -> List[ResolvedPackageInstance]:
        log.LogPrintVerbose(LocalVerbosityLevel.Info, "Initial package resolve")
        log.PushIndent()
        try:
            res = [] # type: List[ResolvedPackageInstance]

            # Build a package dictionary for quick lookup
            allPackageDict = dict() # type: Dict[str, UnresolvedBasicPackage]

            if len(allPackages) > 0:
                allPackages = list(allPackages)
                allPackages.sort(key=lambda s: s.Name.Value.upper())

            log.LogPrintVerbose(LocalVerbosityLevel.Debug, "Available packages")
            #numTopLevel = 0
            #for package in allPackages:
            #    if package.Type == PackageType.TopLevel:
            #        numTopLevel = numTopLevel + 1
            #    else:
            #        log.LogPrintVerbose(LocalVerbosityLevel.Debug, "- {0}".format(package))
            #        allPackageDict[package.Name.Value] = package

            #if numTopLevel != 1:
            #    raise Exception("Missing a top level package");

            #graph = PackageGraphBuilder.Build(log, list(allPackageDict.values()))

            for package in allPackages:
                if package.Type == PackageType.TopLevel:
                    raise Exception("Unsupported package type")
                if log.Verbosity >= LocalVerbosityLevel.Debug:
                    log.LogPrint("- {0}".format(package))
                allPackageDict[package.Name.Value] = package

            graph = PackageGraphBuilder.Build(log, allPackages)

            for node in graph.DebugNodes():
                packageInstance = node.Source
                if isinstance(packageInstance, ResolvedPackageInstance):
                    res.append(packageInstance)

            if dump:
                DotUtil.ToFile(log, "AllDependencies", graph)

            return res
        finally:
            log.PopIndent()

    @staticmethod
    def Resolve2(basicBuildConfig: BasicBuildConfig, createContext: FactoryCreateContext, allPackages: List[UnresolvedPackage],
                 dump: bool = False) -> List[PreResolvePackageResult]:
        allUnresolvedPackages = PackageManager2.__ToUnresolvedBasicPackages(allPackages)

        # Prepare the project id cache
        vsProjectIdCacheFilename = IOUtil.Join(basicBuildConfig.BuildCacheDirAbsolutePath, 'VSProjectIds.json')
        jsonProjectIdCache = JsonProjectIdCache.TryLoad(createContext.Log, vsProjectIdCacheFilename)
        if jsonProjectIdCache is None:
            jsonProjectIdCache = JsonProjectIdCache({})
        projectIdCache = ProjectIdCache(jsonProjectIdCache)
        PackageManager2.__UpdateProjectIdCache(createContext.Log, projectIdCache, allPackages)

        resolvedInstanceBuildOrder = PackageManager2.Resolve(createContext.Log, allUnresolvedPackages, dump)

        resolvedBuildOrder = PackageManager2.__CreatePackageList(createContext, allPackages, resolvedInstanceBuildOrder, projectIdCache)

        if not basicBuildConfig.IsDryRun:
            IOUtil.SafeMakeDirs(basicBuildConfig.BuildCacheDirAbsolutePath)
            JsonProjectIdCache.Save(createContext.Log, vsProjectIdCacheFilename, jsonProjectIdCache)

        return PreResolver.PreResolve(createContext.Log, resolvedBuildOrder)

    @staticmethod
    def __ToUnresolvedBasicPackages(allPackages: List[UnresolvedPackage]) -> List[UnresolvedBasicPackage]:
        allUnresolvedPackages = []  # type: List[UnresolvedBasicPackage]
        for package in allPackages:
            directDependencies = package.DirectDependencies
            flavors = package.ResolvedPlatform.Flavors
            unresolvedBasicPackage = UnresolvedBasicPackage(UnresolvedPackageName(package.NameInfo.FullName.Value), package.Type, directDependencies, flavors)
            allUnresolvedPackages.append(unresolvedBasicPackage)
        return allUnresolvedPackages


    @staticmethod
    def __CreatePackageList(createContext: FactoryCreateContext, allPackages: List[UnresolvedPackage],
                            resolvedInstances: List[ResolvedPackageInstance], projectIdCache: ProjectIdCache) -> List[ProcessedPackage]:

        createContext.Log.LogPrintVerbose(LocalVerbosityLevel.Info, "Creating packages")
        createContext.Log.PushIndent()
        try:
            #allPackagesClone = list(allPackages)

            packageLookupDict = PackageManager2.__BuildPackageLookupDict(allPackages)

            newDict = {} # type: Dict[str, ProcessedPackage]
            finalResolvedOrder = [] # type: List[ProcessedPackage]
            for instance in resolvedInstances:
                instanceName = instance.Name.Value
                if instanceName in newDict:
                    raise Exception("Internal error '{0}' already exist".format(instanceName))
                originalPackageName = instance.Name.Unresolved.Value
                if originalPackageName not in packageLookupDict:
                    raise Exception("Internal error '{0}' not found".format(originalPackageName))
                originalPackage = packageLookupDict[originalPackageName]
                finalPackage = PackageManager2.__CreateFlavorPackage(createContext, instance, originalPackage, projectIdCache)

                newDict[instance.Name.Value] = finalPackage
                finalResolvedOrder.append(finalPackage)

            return finalResolvedOrder
        finally:
            createContext.Log.PopIndent()

    @staticmethod
    def __BuildPackageLookupDict(allPackages: List[UnresolvedPackage]) -> Dict[str, UnresolvedPackage]:
        packageLookupDict = dict() # type: Dict[str, UnresolvedPackage]
        for package in allPackages:
            packageLookupDict[package.NameInfo.FullName.Value] = package
        return packageLookupDict

    @staticmethod
    def __CreateFlavorPackage(createContext: FactoryCreateContext, instance: ResolvedPackageInstance, originalPackage: UnresolvedPackage,
                              projectIdCache: ProjectIdCache) -> ProcessedPackage:
        if createContext.Log.Verbosity >= LocalVerbosityLevel.Debug:
            createContext.Log.LogPrint("Package '{0}' of type: '{1}'".format(instance.Name.Value, PackageInstanceType.ToString(instance.InstanceType)))

        # patch the direct dependencies to match the flavor packages
        projectContext = originalPackage.ProjectContext

        nameInfo = PackageManager2.__PatchPackageNameInfo(originalPackage.NameInfo, instance)

        companyName = originalPackage.CompanyName
        creationYear = originalPackage.CreationYear
        packageFile = originalPackage.PackageFile
        sourceFileHash = originalPackage.SourceFileHash
        packageType = originalPackage.Type
        packageFlags = PackageManager2.__ProcessFlags(originalPackage.Flags)

        packageLanguage = originalPackage.PackageLanguage
        generateList = originalPackage.GenerateList
        generateGrpcProtoFileList = originalPackage.GenerateGrpcProtoFileList

        # patch the direct dependencies to match the flavor selection
        directDependencies = PackageManager2.__ToProcessedDependencyList(instance.DirectDependencies)
        directRequirements, directDefines, externalDependencies = PackageManager2.__ExtractInstanceRequirementsAndDefinesAndExtDeps(instance, originalPackage)
        path = PackageManager2.__ToProcessedPackagePaths(originalPackage.Path)
        templateType = originalPackage.TemplateType
        buildCustomization = originalPackage.BuildCustomization
        directExperimentalRecipe = originalPackage.DirectExperimentalRecipe

        newVisualStudioProjectGUID = originalPackage.CustomInfo.VisualStudioProjectGUID
        if newVisualStudioProjectGUID is None or instance.InstanceType == PackageInstanceType.Flavor:
            newVisualStudioProjectGUID = projectIdCache.TryGetByName(nameInfo.FullName.Value)
            if newVisualStudioProjectGUID is None:
                newVisualStudioProjectGUID = GeneratorVCUtil.CreateGUID2(projectIdCache.Contains)
                projectIdCache.AddNew(nameInfo.FullName.Value, newVisualStudioProjectGUID)
                if createContext.Log.Verbosity >= LocalVerbosityLevel.Trace:
                    createContext.Log.LogPrint("- New project id is {0}".format(newVisualStudioProjectGUID))

        resolvedPlatform = PackageManager2.__PatchResolvedPlatform(createContext, originalPackage.ResolvedPlatform, newVisualStudioProjectGUID)
        directPlatformSupported = originalPackage.DirectPlatformSupported

        customInfo = PackageManager2.__PatchPackageCustomInfo(originalPackage.CustomInfo, newVisualStudioProjectGUID)

        traceContext = originalPackage.TraceContext

        return ProcessedFactory.CreatePackage(createContext.Log, createContext.GeneratorInfo, projectContext, nameInfo, companyName, creationYear,
                                              packageFile, sourceFileHash, packageType, packageFlags, packageLanguage, generateList,
                                              generateGrpcProtoFileList, directDependencies,
                                              directRequirements, directDefines, externalDependencies, path, templateType, buildCustomization,
                                              directExperimentalRecipe, resolvedPlatform, directPlatformSupported, customInfo, traceContext)

    @staticmethod
    def __ExtractInstanceRequirementsAndDefinesAndExtDeps(instance: ResolvedPackageInstance,
                                                          originalPackage: UnresolvedPackage) -> Tuple[List[UnresolvedPackageRequirement], List[UnresolvedPackageDefine], List[UnresolvedExternalDependency]]:
        onlyOriginalRequirements = True
        onlyOriginalDefines = True
        onlyOriginalExternalDeps = True
        finalDirectRequirements = originalPackage.DirectRequirements
        finalDirectDefines = originalPackage.DirectDefines
        finalExternalDependencies = originalPackage.ExternalDependencies

        if len(instance.FlavorSelections.Selections) > 0:
            # Run through all flavors and extract their defines
            for selection in instance.FlavorSelections.Selections:
                flavor = instance.FlavorTemplate.TryGetFlavor(selection.Name)
                if flavor is not None:
                    flavorOption = flavor.TryGetOptionByName(selection.Option)
                    if flavorOption is None:
                        raise Exception("Package '{0}' flavor: '{1}' unknown flavor option name: '{2}' ({3})".format(instance.Name, selection.Name, selection.Option, flavor.Description))

                    originalFlavor = originalPackage.TryGetFlavorByName(selection.Name)
                    if originalFlavor is None:
                        raise Exception("Package '{0}' unknown flavor: '{1}'".format(instance.Name, selection.Name))

                    originalFlavorOption = originalFlavor.TryGetOptionByName(selection.Option)
                    if originalFlavorOption is None:
                        raise Exception("Package '{0}' flavor: '{1}' unknown flavor option name: '{2}' ({3})".format(instance.Name, selection.Name, selection.Option, originalFlavor.Description))

                    if len(originalFlavorOption.DirectRequirements) > 0:
                        if onlyOriginalRequirements:
                            onlyOriginalRequirements = False
                            finalDirectRequirements = list(finalDirectRequirements)
                        finalDirectRequirements += originalFlavorOption.DirectRequirements

                    if len(originalFlavorOption.DirectDefines) > 0:
                        if onlyOriginalDefines:
                            onlyOriginalDefines = False
                            finalDirectDefines = list(finalDirectDefines)
                        finalDirectDefines += originalFlavorOption.DirectDefines

                    if len(originalFlavorOption.ExternalDependencies) > 0:
                        if onlyOriginalExternalDeps:
                            onlyOriginalExternalDeps = False
                            finalExternalDependencies = list(finalExternalDependencies)
                        finalExternalDependencies += originalFlavorOption.ExternalDependencies

        return (finalDirectRequirements, finalDirectDefines, finalExternalDependencies)


    @staticmethod
    def __ProcessFlags(flags: UnresolvedPackageFlags) -> ProcessedPackageFlags:
        isVirtual = flags.IsVirtual
        allowCheck = flags.AllowCheck
        unitTest = flags.UnitTest
        showInMainReadme = flags.ShowInMainReadme
        packageNameBasedIncludePath = flags.PackageNameBasedIncludePath
        platformDefaultSupportedValue = flags.PlatformDefaultSupportedValue
        enableExtendedSourceExtensions = flags.EnableExtendedSourceExtensions
        return ProcessedPackageFlags(isVirtual, allowCheck, unitTest, showInMainReadme, packageNameBasedIncludePath, platformDefaultSupportedValue,
                                     enableExtendedSourceExtensions)

    @staticmethod
    def __ToProcessedPackagePaths(paths: UnresolvedPackagePaths) -> ProcessedPackagePaths:
        includePath = paths.IncludePath
        sourcePath = paths.SourcePath
        contentPath = paths.ContentPath
        contentSourcePath = paths.ContentSourcePath
        baseIncludePath = paths.BaseIncludePath
        baseSourcePath = paths.BaseSourcePath
        return ProcessedPackagePaths(includePath, sourcePath, contentPath, contentSourcePath, baseIncludePath, baseSourcePath)

    @staticmethod
    def ProcessPackage(createContext: FactoryCreateContext, packageProjectContext: PackageProjectContext,
                       unresolvedPackage: UnresolvedPackage) -> ProcessedPackage:
        nameInfo = unresolvedPackage.NameInfo
        companyName = unresolvedPackage.CompanyName
        creationYear = unresolvedPackage.CreationYear
        packageFile = unresolvedPackage.PackageFile
        sourceFileHash = unresolvedPackage.SourceFileHash
        packageType = unresolvedPackage.Type
        packageFlags = PackageManager2.__ProcessFlags(unresolvedPackage.Flags)
        packageLanguage = unresolvedPackage.PackageLanguage
        generateList = unresolvedPackage.GenerateList
        generateGrpcProtoFileList = unresolvedPackage.GenerateGrpcProtoFileList
        directDependencies = PackageManager2.__ToProcessedDependencyList2(unresolvedPackage.DirectDependencies)
        directRequirements = unresolvedPackage.DirectRequirements
        directDefines = unresolvedPackage.DirectDefines
        externalDependencies = unresolvedPackage.ExternalDependencies
        path = PackageManager2.__ToProcessedPackagePaths(unresolvedPackage.Path)
        templateType = unresolvedPackage.TemplateType
        buildCustomization = unresolvedPackage.BuildCustomization
        directExperimentalRecipe = unresolvedPackage.DirectExperimentalRecipe
        resolvedPlatform = unresolvedPackage.ResolvedPlatform
        directPlatformSupported = unresolvedPackage.DirectPlatformSupported
        customInfo = unresolvedPackage.CustomInfo
        traceContext = unresolvedPackage.TraceContext

        return ProcessedFactory.CreatePackage(createContext.Log, createContext.GeneratorInfo, packageProjectContext, nameInfo, companyName,
                                              creationYear, packageFile, sourceFileHash, packageType, packageFlags, packageLanguage, generateList,
                                              generateGrpcProtoFileList, directDependencies, directRequirements, directDefines, externalDependencies,
                                              path, templateType, buildCustomization, directExperimentalRecipe, resolvedPlatform,
                                              directPlatformSupported, customInfo, traceContext)


    @staticmethod
    def __ToProcessedDependencyList(instanceDependencies: List[ResolvedPackageInstanceDependency]) -> List[ProcessedPackageDependency]:
        return [PackageManager2.__ToProcessedDependency(entry) for entry in instanceDependencies]

    @staticmethod
    def __ToProcessedDependency(entry: ResolvedPackageInstanceDependency) -> ProcessedPackageDependency:
        name = PackageInstanceName(entry.Package.Name.SmartValue)
        accessType = entry.SourceDependency.Access
        flavorConstraints = entry.SourceDependency.FlavorConstraints
        ifCondition = entry.SourceDependency.IfCondition
        return ProcessedPackageDependency(name, accessType, flavorConstraints, ifCondition)

    @staticmethod
    def __ToProcessedDependencyList2(instanceDependencies: List[UnresolvedPackageDependency]) -> List[ProcessedPackageDependency]:
        return [PackageManager2.__UnresolvedPackageDependencyToProcessedDependency(entry) for entry in instanceDependencies]

    @staticmethod
    def __UnresolvedPackageDependencyToProcessedDependency(entry: UnresolvedPackageDependency) -> ProcessedPackageDependency:
        name = PackageInstanceName(entry.Name.Value)
        accessType = entry.Access
        flavorConstraints = entry.FlavorConstraints
        ifCondition = entry.IfCondition
        return ProcessedPackageDependency(name, accessType, flavorConstraints, ifCondition)

    @staticmethod
    def __PatchResolvedPlatform(createContext: FactoryCreateContext, sourcePlatform: PackagePlatform,
                                newVisualStudioProjectGUID: Optional[str]) -> PackagePlatform:
        name = sourcePlatform.Name
        directRequirements = sourcePlatform.DirectRequirements
        # direct dependencies has been resolved
        directDependencies = []  # type: List[UnresolvedPackageDependency]
        variants = sourcePlatform.Variants
        supported = sourcePlatform.Supported
        externalDependencies = sourcePlatform.ExternalDependencies
        directDefines = sourcePlatform.DirectDefines
        directExperimentalRecipe = sourcePlatform.DirectExperimentalRecipe
        # flavors has been resolved
        flavors = [] # type: List[UnresolvedPackageFlavor]
        return UnresolvedFactory.CreatePackagePlatform(createContext, name, directRequirements, directDependencies, variants, supported,
                                                       externalDependencies, directDefines, directExperimentalRecipe, flavors)

    @staticmethod
    def __PatchPackageCustomInfo(customInfo: PackageCustomInfo, newVisualStudioProjectGUID: Optional[str]) -> PackageCustomInfo:
        return PackageCustomInfo(newVisualStudioProjectGUID)

    @staticmethod
    def __UpdateProjectIdCache(log: Log, projectIdCache: ProjectIdCache, allPackages: List[UnresolvedPackage]) -> None:
        for package in allPackages:
            if package.CustomInfo.VisualStudioProjectGUID is not None:
                projectIdCache.Add(package.NameInfo.FullName.Value, package.CustomInfo.VisualStudioProjectGUID)


    @staticmethod
    def __PatchPackageNameInfo(originalNameInfo: PackageNameInfo, instance: ResolvedPackageInstance) -> PackageNameInfo:
        if instance.Name.SmartValue == originalNameInfo.FullName.Value:
            return originalNameInfo
        return PackageNameInfo(PackageInstanceName(instance.Name.SmartValue))
