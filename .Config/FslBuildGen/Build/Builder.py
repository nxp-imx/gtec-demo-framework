#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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

from typing import Any
from typing import Callable
from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from typing import Set
import os
import multiprocessing
import shlex
import sys
import subprocess
from FslBuildGen import IOUtil
from FslBuildGen import PackageUtil
from FslBuildGen import PackageListUtil
from FslBuildGen import PlatformUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Build.BuildConfigRecord import BuildConfigRecord
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.Build.Filter import RequirementFilter
from FslBuildGen.Build.RequirementTree import RequirementTree
from FslBuildGen.Build.RequirementTreeNode import RequirementTreeNode
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.BuildContent.SharedValues import CONFIG_FSLBUILDCONTENT_ENABLED
from FslBuildGen.BuildExternal import RecipeBuilder
from FslBuildGen.BuildExternal.BuilderSettings import BuilderSettings
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import BuildThreads
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import BuildVariantType
from FslBuildGen.DataTypes import PackageCreationYearString
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Generator import PluginConfig
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorPluginBase2
from FslBuildGen.Generator.GeneratorVC import GeneratorVCUtil
from FslBuildGen.Generator.Report.ReportVariableFormatter import ReportVariableFormatter
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackagePlatformVariant
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Log import Log
from FslBuildGen.SharedGeneration import GEN_BUILD_ENV_VARIANT_SETTING
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.Xml.XmlStuff import XmlGenFileVariantOption


class LocalPlatformBuildContext(object):
    def __init__(self, log: Log, generatorOriginalName: str, buildCommand: int, buildThreads: int) -> None:
        self.Log = log
        self.AdditionalBuildArguments = [] # type: List[str]
        PlatformBuildUtil.AddBuildThreads(log, self.AdditionalBuildArguments, generatorOriginalName, buildThreads)
        if buildCommand == CommandType.Clean:
            self.__HandleCleanBuilds(self.AdditionalBuildArguments, generatorOriginalName)


    def __HandleCleanBuilds(self, rArgumentList: List[str], generatorOriginalName: str) -> None:
        platformName = generatorOriginalName
        if platformName == PlatformNameString.QNX or platformName == PlatformNameString.YOCTO or platformName == PlatformNameString.UBUNTU:
            rArgumentList += ['clean']
        elif platformName == PlatformNameString.WINDOWS:
            rArgumentList += ['/t:Clean']
        else:
            self.Log.LogPrintWarning("The builder ignored --Command: clean")


class LocalBuildContext(object):
    def __init__(self, config: Config, platformBuildContext: LocalPlatformBuildContext,
                 generatorReportDict: Dict[Package, PackageGeneratorReport],
                 generatorName: str) -> None:
        self.Config = config
        self.Platform = platformBuildContext
        self.GeneratorReportDict = generatorReportDict
        self.GeneratorName = generatorName


BuildMethodType = Callable[[LocalBuildContext, Package, BuildConfigRecord, Dict[str, str], Optional[List[str]]], None]

class Builder(object):
    # requestedPackages is the packages specifically requested by the user or None for SDK builds.
    def __init__(self, generatorContext: GeneratorContext,
                 config: Config,
                 topLevelPackage: Package,
                 buildConfig: BuildConfigRecord,
                 enableContentBuilder: bool,
                 forceClaimInstallArea: bool) -> None:
        super(Builder, self).__init__()
        self.Log = config

        localPlatformBuildContext = LocalPlatformBuildContext(config, generatorContext.Generator.OriginalName,
                                                              buildConfig.BuildCommand, buildConfig.BuildThreads)


        # Do a final filter that removes all unsupported packages
        resolvedBuildOrder = topLevelPackage.ResolvedBuildOrder
        resolvedBuildOrder = PackageFilter.FilterNotSupported(self.Log, topLevelPackage, resolvedBuildOrder)
        if not PackageFilter.WasThisAExecutableBuildAndAreThereAnyLeft(topLevelPackage.ResolvedBuildOrder, resolvedBuildOrder):
            self.Log.DoPrint("No executables left, skipping all")
            return

        # Run the recipe builder on the packages we have left
        # We run the recipe builder on the resolvedBuildOrder since it all required packages, not just the ones we need to build as libs and executables
        builderSettings = BuilderSettings()
        builderSettings.ForceClaimInstallArea = forceClaimInstallArea
        builderSettings.BuildThreads = buildConfig.BuildThreads
        RecipeBuilder.BuildPackagesInOrder(config, generatorContext, resolvedBuildOrder, builderSettings)


        resolvedBuildOrderBuildable = PackageFilter.FilterBuildablePackages(resolvedBuildOrder)
        if len(resolvedBuildOrderBuildable) == 0:
            config.DoPrint("Nothing to build!")
            return

        generatorReportDict = generatorContext.Generator.GenerateReport(self.Log, resolvedBuildOrderBuildable)

        packageCount = len(resolvedBuildOrderBuildable)

        resolvedBuildOrderBuildable = self.__ApplyPlatformOrderChanges(resolvedBuildOrderBuildable, buildConfig.PlatformName)

        originalBuildArgs = buildConfig.BuildArgs

        # Default content building for all platform (for those generators that don't add it to the build file)
        if enableContentBuilder:
            for package in resolvedBuildOrderBuildable:
                if package.Type == PackageType.Executable:
                    featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
                    if package.AbsolutePath is None:
                        raise Exception("Invalid package")
                    ContentBuilder.Build(config, package.AbsolutePath, featureList)

        # Windows runs its validation checks slightly differently
        runValidationChecks = (buildConfig.PlatformName != PlatformNameString.WINDOWS)

        buildContext = LocalBuildContext(config, localPlatformBuildContext, generatorReportDict, generatorContext.GeneratorName)
        for package in resolvedBuildOrderBuildable:
            config.LogPrint("Building package: {0}".format(package.Name))
            config.LogPrint("Package location: {0}".format(package.AbsolutePath))
            if not config.IsDryRun:
                buildEnv = os.environ.copy()  # type: Dict[str, str]
                buildEnv[CONFIG_FSLBUILDCONTENT_ENABLED] = "false"
                self.__ExtendEnvironmentDictWithVariants(config, buildEnv, package, buildConfig.VariantSettingsDict)
                buildConfig.BuildArgs = list(originalBuildArgs)
                if config.Verbosity > 4:
                    config.DoPrint("Package build arguments1: {0}".format(buildConfig.BuildArgs))
                    config.DoPrint("General build arguments2: {0}".format(originalBuildArgs))
                strRunCommands = buildConfig.RunCommand
                runCommands = None  # type: Optional[List[str]]
                if strRunCommands is not None:
                    userRunCommands = shlex.split(strRunCommands)
                    runCommands = self.__TryGenerateRunCommandForExecutable(buildContext, package, buildConfig.VariantSettingsDict, userRunCommands)
                if runValidationChecks:
                    featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
                    Validate.ValidatePlatform(config, buildConfig.PlatformName, featureList, 4)
                self.__BuildPackage(buildContext, package, buildConfig, buildEnv, runCommands)

        if packageCount > 0:
            config.LogPrint("Build {0} packages".format(packageCount))
        else:
            config.DoPrint("Nothing build!")


    def __ApplyPlatformOrderChanges(self, resolvedBuildOrder: List[Package], platformName: str) -> List[Package]:
        if platformName != PlatformNameString.WINDOWS:
            return resolvedBuildOrder

        # Since visual studio does its own dependency management and building
        # we dont need to request packages to be build multiple times.
        markedForBuild = set()  # type: Set[str]
        result = []  # type: List[Package]
        for package in reversed(resolvedBuildOrder):
            if not package.Name in markedForBuild:
                markedForBuild.add(package.Name)
                result.append(package)
                for entry in package.ResolvedAllDependencies:
                    markedForBuild.add(entry.Name)

        return result


    def __BuildPackage(self, buildContext: LocalBuildContext,
                       package: Package,
                       buildConfig: BuildConfigRecord,
                       buildEnv: Dict[str, str],
                       runCommands: Optional[List[str]]) -> None:
        if package.AbsolutePath is None or package.ResolvedBuildPath is None:
            raise Exception("Invalid package")
        if buildContext.GeneratorReportDict is None:
            raise Exception("Build not supported by generator '{0}' for package: {1}".format(buildContext.GeneratorName, package.Name))
        if package not in buildContext.GeneratorReportDict:
            raise Exception("Build not supported by generator '{0}' for package: {1}".format(buildContext.GeneratorName, package.Name))
        generatorReport = buildContext.GeneratorReportDict[package]
        buildReport = generatorReport.BuildReport
        variableReport = generatorReport.VariableReport
        if buildReport is None:
            raise Exception("Build report not supported by generator '{0}' for package: {1}".format(buildContext.GeneratorName, package.Name))
        buildCommandReport = buildReport.BuildCommandReport
        if buildCommandReport is None:
            if self.Log.Verbosity >= 5:
                self.Log.LogPrint("Skipping package '{0}' as its build command was None".format(package.Name))
            return

        currentWorkingDirectory = package.AbsolutePath
        if buildCommandReport.CurrentWorkingDirectoryFormatString is not None:
            currentWorkingDirectory = ReportVariableFormatter.Format(buildCommandReport.CurrentWorkingDirectoryFormatString, variableReport, buildConfig.VariantSettingsDict)

        buildCommandStr = ReportVariableFormatter.Format(buildCommandReport.CommandFormatString, variableReport, buildConfig.VariantSettingsDict)
        if not buildCommandReport.UseAsRelative:
            buildCommandStr = IOUtil.Join(currentWorkingDirectory, buildCommandStr)

        buildArgumentList = []
        for buildArgument in buildCommandReport.Arguments:
            buildArgument = ReportVariableFormatter.Format(buildArgument, variableReport, buildConfig.VariantSettingsDict)
            buildArgumentList.append(buildArgument)


        buildCommand = [buildCommandStr] + buildArgumentList + buildConfig.BuildArgs
        if len(buildContext.Platform.AdditionalBuildArguments) > 0:
            buildCommand += buildContext.Platform.AdditionalBuildArguments

        try:
            result = subprocess.call(buildCommand, cwd=currentWorkingDirectory, env=buildEnv)
            if result != 0:
                self.Log.LogPrintWarning("The build command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
                sys.exit(result)
        except FileNotFoundError:
                self.Log.DoPrintWarning("The build command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(buildCommand), currentWorkingDirectory))
                raise

        if runCommands:
            try:
                # TODO: Allow the working directory for the run command to be changed too. For now use the original choice of absolute path for the package
                currentWorkingDirectory = package.AbsolutePath
                result = subprocess.call(runCommands, cwd=currentWorkingDirectory, env=buildEnv)
                if result != 0:
                    self.Log.LogPrintWarning("The run command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(runCommands), result, currentWorkingDirectory))
                    sys.exit(result)
            except FileNotFoundError:
                    self.Log.LogPrintWarning("The run command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(runCommands), currentWorkingDirectory))
                    raise



    def __TryLocateVariant(self, package: Package, key: str) -> Optional[PackagePlatformVariant]:
        if key in package.ResolvedAllVariantDict:
            return package.ResolvedAllVariantDict[key]
        # try a manual search for 'virtual keys'
        for entry in list(package.ResolvedAllVariantDict.values()):
            if key in entry.PurifiedName:
                return entry
        return None


    def __ExtendEnvironmentDictWithVariants(self, config: Config,
                                            buildEnv: Dict[str, str],
                                            package: Package,
                                            userVariantSettingDict: Dict[str, str]) -> None:
        for key, value in list(userVariantSettingDict.items()):
            variant = self.__TryLocateVariant(package, key)
            if variant is not None:
                if variant.Type == VariantType.Virtual or (value in variant.OptionDict):
                    envName = "{0}{1}".format(GEN_BUILD_ENV_VARIANT_SETTING, key.upper())
                    if envName in buildEnv:
                        raise Exception("The environment variable {0} has allready been defined".format(envName))
                    buildEnv[envName] = value
                else:
                    validValues = list(variant.OptionDict.keys())
                    validValues.sort()
                    config.LogPrint("WARNING: Variant '%s' expects one of the following values: '%s' not '%s'" % (key, ','.join(validValues), value))



    def __ExtractRelevantVariantSettingsDict(self, config: Config,
                                             package: Package,
                                             userVariantSettingDict: Dict[str, str]) -> Dict[str, str]:
        """ Filters the userVariantSettingsDict down into a dict containing only the entries that are relevant for this package.
            It also validates that the user-value is valid for the given variant.
        """
        dictVariantSettings = {}  # type: Dict[str, str]
        for key, value in userVariantSettingDict.items():
            if key in package.ResolvedAllVariantDict:
                variant = package.ResolvedAllVariantDict[key]
                if value in variant.OptionDict:
                    dictVariantSettings[key] = value
                else:
                    validValues = list(variant.OptionDict.keys())
                    validValues.sort()
                    config.DoPrintWarning("Variant '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))
        return dictVariantSettings





    def __TryGenerateRunCommandForExecutable(self, buildContext: LocalBuildContext,
                                             package: Package,
                                             userVariantSettingDict: Dict[str, str],
                                             runCommands: Optional[List[str]]) -> Optional[List[str]]:
        if package.Type != PackageType.Executable or runCommands is None or len(runCommands) <= 0:
            return None
        if package.ResolvedBuildPath is None or package.AbsolutePath is None:
            raise Exception("Invalid package")

        if package not in buildContext.GeneratorReportDict:
            raise Exception("ForAllExe not supported by generator for package: {0}".format(package.Name))

        generatorReport = buildContext.GeneratorReportDict[package]
        variableReport = generatorReport.VariableReport
        executableReport = generatorReport.ExecutableReport
        if executableReport is None:
            raise Exception("ForAllExe not supported by generator for package {0} as it didnt contain a executable record".format(package.Name))

        foundVariantExePath = ReportVariableFormatter.Format(executableReport.ExeFormatString,
                                                             variableReport, userVariantSettingDict,
                                                             executableReport.EnvironmentVariableResolveMethod)
        packagePath = package.AbsolutePath
        fullPathExe = IOUtil.Join(packagePath, foundVariantExePath)
        exeName = IOUtil.GetFileName(foundVariantExePath)
        exePath = IOUtil.GetDirectoryName(fullPathExe)
        contentPath = IOUtil.Join(packagePath, "Content")
        fullBuildDirPath = IOUtil.Join(packagePath, package.ResolvedBuildPath)
        fullBuildDirPath = buildContext.Config.ToCurrentOSPathDirectConversion(fullBuildDirPath)
        fullPathExe = buildContext.Config.ToCurrentOSPathDirectConversion(fullPathExe)
        exeName = buildContext.Config.ToCurrentOSPathDirectConversion(exeName)
        exePath = buildContext.Config.ToCurrentOSPathDirectConversion(exePath)
        packagePath = buildContext.Config.ToCurrentOSPathDirectConversion(packagePath)
        contentPath = buildContext.Config.ToCurrentOSPathDirectConversion(contentPath)

        commands = []
        if executableReport.RunScript is not None:
            runScript = executableReport.RunScript
            if not executableReport.UseAsRelative:
                runScript = IOUtil.Join(packagePath, runScript)

            commands.append(runScript)

        for commandToRun in runCommands:
            command = commandToRun
            command = command.replace("(EXE)", fullPathExe)
            command = command.replace("(EXE_NAME)", exeName)
            command = command.replace("(EXE_PATH)", exePath)
            command = command.replace("(PACKAGE_NAME)", package.Name)
            command = command.replace("(PACKAGE_PATH)", packagePath)
            command = command.replace("(CONTENT_PATH)", contentPath)
            command = command.replace("(BUILD_PATH)", fullBuildDirPath)
            commands.append(command)
        return commands



def __BuildCompleteVariantDict(topLevelPackage: Package) -> Dict[str, PackagePlatformVariant]:
    variantDict = dict(topLevelPackage.ResolvedAllVariantDict) # type: Dict[str, PackagePlatformVariant]
    return variantDict


def __ValidateUserVariantSettings(config: Config,
                                  topLevelPackage: Package,
                                  userVariantSettingDict: Dict[str, str]) -> None:
    variantDict = __BuildCompleteVariantDict(topLevelPackage)
    for key, value in list(userVariantSettingDict.items()):
        if key in variantDict:
            variant = variantDict[key]
            if not value in variant.OptionDict:
                validValues = list(variant.OptionDict.keys())
                validValues.sort()
                raise Exception("Variant '{0}' expects one of the following values: '{1}' not '{2}'".format(key, ','.join(validValues), value))
        elif key != ToolAddedVariant.CONFIG:
            config.LogPrint("WARNING: Unused variant setting '{0}'".format(key))


def __LogVariantSettings(config: Config, variantSettingsDict: Dict[str, str]) -> None:
    if len(variantSettingsDict) <= 0:
        return
    names = list(variantSettingsDict.keys())
    names.sort()
    result = []
    for name in names:
        result.append("{0}={1}".format(name, variantSettingsDict[name]))
    config.LogPrint("Variant settings: {0}".format(", ".join(result)))


# generator = the generator that was used to build the files
def BuildPackages(generatorContext: GeneratorContext,
                  config: Config,
                  packages: List[Package],
                  variantSettingsDict: Dict[str, str],
                  buildArgs: List[str],
                  buildForAllExe: Optional[str],
                  generator: GeneratorPluginBase2,
                  enableContentBuilder: bool,
                  forceClaimInstallArea: bool,
                  buildThreads: int,
                  buildCommand: int) -> None:
    PlatformUtil.CheckBuildPlatform(generatorContext.PlatformName)
    topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)

    __ValidateUserVariantSettings(config, topLevelPackage, variantSettingsDict)
    __LogVariantSettings(config, variantSettingsDict)

    buildConfig = BuildConfigRecord(generatorContext.PlatformName, variantSettingsDict, buildCommand, buildArgs, buildForAllExe, generator, buildThreads)
    Builder(generatorContext, config, topLevelPackage, buildConfig, enableContentBuilder, forceClaimInstallArea)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowVariantList(log: Log,
                    topLevelPackage: Package,
                    requestedFiles: Optional[List[str]],
                    generator: GeneratorPluginBase2) -> None:

    variantDict = __BuildCompleteVariantDict(topLevelPackage)


    # This is kind of a hack to list this here (its also not a real variant inside our model)
    generatorVariants = generator.GetVariants()
    if len(variantDict) <= 0 and len(generatorVariants) <= 0:
        log.DoPrint("Variants: None")
        return

    # Pretty print useful information
    log.DoPrint("Variants:")

    generatorVariants.sort(key=lambda s: s.Name.lower())
    for variantInfo in generatorVariants:
        if variantInfo.Type == BuildVariantType.Static:
            log.DoPrint("  {0}={1} (Introduced by native build system generator)".format(variantInfo.Name, variantInfo.Description))
        else:
            log.DoPrint("  {0}={1} (Introduced by native build system)".format(variantInfo.Name, variantInfo.Description))

    variantNames = list(variantDict.keys())
    variantNames.sort()
    for variantName in variantNames:
        variant = variantDict[variantName]
        optionNames = list(variant.OptionDict.keys())
        optionNames.sort()
        if variant.Type == VariantType.Virtual:
            log.DoPrint("  {0}={1} *Virtual* (Introduced by package: {2})".format(variant.PurifiedName, ', '.join(optionNames), variant.IntroducedByPackageName))
        else:
            log.DoPrint(("  {0}={1} (Introduced by package: {2})".format(variant.PurifiedName, ', '.join(optionNames), variant.IntroducedByPackageName)))


def ShowBuildVariantList(log: Log, generator: GeneratorPluginBase2) -> None:
    # This is kind of a hack to list this here (its also not a real variant inside our model)
    generatorVariants = [variant for variant in generator.GetVariants() if variant.Type == BuildVariantType.Static]

    if len(generatorVariants) <= 0:
        log.DoPrint("Build variants: None")
        return
    log.DoPrint("Build variants:")

    generatorVariants.sort(key=lambda s: s.Name.lower())

    for variantInfo in generatorVariants:
        log.DoPrint("  {0}={1} (Introduced by native build system generator)".format(variantInfo.Name, variantInfo.Description))


def __PrintRequirementsNode(log: Log,
                            node: RequirementTreeNode,
                            currentIndent: str,
                            strAddIndent: str) -> None:
    if node.Content is None:
        raise Exception("Invalid node")
    strFormat = "{0}-"
    if node.Content.Type == PackageRequirementTypeString.Feature:
        strFormat += " '{2}'"
    else:
        strFormat += " {1}: '{2}'"
    if len(node.Content.Version) > 0:
        strFormat += " V{3}"
    #if len(node.Content.Extends) > 0:
    #    strFormat += " extends '{4}'"
    strFormat += " (introduced by package: {5})"
    log.DoPrint(strFormat.format(currentIndent, node.Content.Type, node.Content.Name, node.Content.Version, node.Content.Extends, ", ".join(node.Content.IntroducedByPackages)))

    # Group by type
    dictGroup = {}  # type: Dict[str, List[RequirementTreeNode]]
    for childNode in node.Children:
        if childNode.Content is None:
            raise Exception("Invalid node")
        if childNode.Content.Type in dictGroup:
            dictGroup[childNode.Content.Type].append(childNode)
        else:
            dictGroup[childNode.Content.Type] = [childNode]

    # Pretty print useful information in name sorted order
    sortedGroupIds = list(dictGroup.keys())
    # sort by type name, but make sure that new 'features' go last
    sortedGroupIds.sort(key=lambda s: s if s != PackageRequirementTypeString.Feature else '{{feature}}')

    for groupId in sortedGroupIds:
        groupedRequirements = dictGroup[groupId]
        groupedRequirements.sort(key=lambda s: None if s.Content is None else s.Content.Id)
        for childNode in groupedRequirements:
            __PrintRequirementsNode(log, childNode, currentIndent + strAddIndent, strAddIndent)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowRequirementList(log: Log,
                        basicConfig: BasicConfig,
                        topLevelPackage: Package,
                        requestedFiles: Optional[List[str]],
                        showFeaturesOnly: bool = False) -> None:
    message = "Requirements" if not showFeaturesOnly else "Features"
    filterName = None if not showFeaturesOnly else PackageRequirementTypeString.Feature
    requestedPackages = PackageUtil.GetPackageListFromFilenames(topLevelPackage, requestedFiles)
    requirements = RequirementFilter.GetRequirementList(topLevelPackage, requestedPackages, filterName)

    if len(requirements) <= 0:
        log.DoPrint("{0}: None".format(message))
        return
    log.DoPrint("{0}:".format(message))

    rootNode = RequirementTree(requirements).RootNode

    strAddIndent = "  "
    # We only show the type group info when there is more than one
    #showTypeGroup = len(rootNode.Children) > 1
    baseIndent = "" #strAddIndent if len(rootNode.Children) > 1 else ""

    sortedFeatures = list(rootNode.Children)
    sortedFeatures.sort(key=lambda s: None if s.Content is None else s.Content.Id)
    for sortedFeature in sortedFeatures:
        __PrintRequirementsNode(log, sortedFeature, baseIndent, strAddIndent)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowFeatureList(log: Log,
                    basicConfig: Config,
                    topLevelPackage: Package,
                    requestedFiles: Optional[List[str]]) -> None:
    ShowRequirementList(log, basicConfig, topLevelPackage, requestedFiles, True)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowExtensionList(log: Log,
                      topLevelPackage: Package,
                      requestedFiles: Optional[List[str]]) -> None:
    requestedPackages = PackageUtil.GetPackageListFromFilenames(topLevelPackage, requestedFiles)
    requirements = RequirementFilter.GetRequirementList(topLevelPackage, requestedPackages, PackageRequirementTypeString.Extension)

    if len(requirements) <= 0:
        log.DoPrint("Extensions: None")
        return
    log.DoPrint("Extensions:")

    # Pretty print useful information in name sorted order
    requirements.sort(key=lambda s: s.Id)

    currentIndent = "  "

    for requirement in requirements:
        strFormat = "{0}- '{1}'"
        if len(requirement.Version) > 0:
            strFormat += " V{2}"
        if len(requirement.Extends) > 0:
            strFormat += " extends '{3}'"
        strFormat += " (introduced by package: {4})"
        log.DoPrint(strFormat.format(currentIndent, requirement.Name, requirement.Version, requirement.Extends, ", ".join(requirement.IntroducedByPackages)))
