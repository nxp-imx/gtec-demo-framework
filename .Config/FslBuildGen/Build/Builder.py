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

#from typing import Any
from typing import Callable
#from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from typing import Set
from typing import Tuple
#import json
import os
#import multiprocessing
import shlex
#import sys
import subprocess
from FslBuildGen import IOUtil
from FslBuildGen import PackageUtil
from FslBuildGen import PackageListUtil
from FslBuildGen import ToolSharedValues
from FslBuildGen.Build.BuildConfigRecord import BuildConfigRecord
from FslBuildGen.Build.BuildConfigureCache import BuildConfigureCache
from FslBuildGen.Build.BuildUtil import PlatformBuildTypeInfo
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.Build.BuildVariantUtil import BuildVariantUtil
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.Build.Filter import RequirementFilter
from FslBuildGen.Build.RequirementTree import RequirementTree
from FslBuildGen.Build.RequirementTreeNode import RequirementTreeNode
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.BuildConfig.BuildUtil import BuildUtil
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildContent.SharedValues import CONFIG_FSLBUILDCONTENT_ENABLED
from FslBuildGen.BuildExternal import RecipeBuilder
from FslBuildGen.BuildExternal.BuilderSettings import BuilderSettings
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
#from FslBuildGen.DataTypes import BuildThreads
#from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import BuildVariantType
#from FslBuildGen.DataTypes import PackageCreationYearString
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Exceptions import ExitException
from FslBuildGen.OpenProject.OpenProjectCreateInfo import OpenProjectCreateInfo
from FslBuildGen.OpenProject.OpenProjectCMakeInfo import OpenProjectCMakeInfo
from FslBuildGen.OpenProject.OpenProjectCreateInfo import OpenProjectExecutableInfo
from FslBuildGen.OpenProject.OpenProjectUtil import OpenProjectUtil
#from FslBuildGen.ExtensionListManager import ExtensionListManager
#from FslBuildGen.Generator import PluginConfig
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.GeneratorPluginBase2 import GeneratorPluginBase2
#from FslBuildGen.Generator.GeneratorVC import GeneratorVCUtil
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
#from FslBuildGen.Generator.Report.PackageGeneratorBuildExecutableInfo import PackageGeneratorBuildExecutableInfo
from FslBuildGen.Generator.Report.PackageGeneratorConfigReport import PackageGeneratorConfigReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.Report.ReportVariableFormatter import ReportVariableFormatter
#from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.Packages.Package import Package
#from FslBuildGen.Packages.Package import PackagePlatformVariant
#from FslBuildGen.Packages.PackageRequirement import PackageRequirement
#from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Log import Log
#from FslBuildGen.SharedGeneration import GEN_BUILD_ENV_VARIANT_SETTING
#from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.ToolConfig import ToolConfig
#from FslBuildGen.ToolConfigProjectInfo import ToolConfigProjectInfo
#from FslBuildGen.Xml.XmlStuff import XmlGenFileVariantOption


class RunCmdInfo(object):
    def __init__(self, runCommands: List[str], runPath: str) -> None:
        super().__init__()
        self.RunCommands = runCommands
        self.RunPath = runPath


class LocalPlatformBuildContext(object):
    def __init__(self, log: Log, generatorOriginalName: str, isCMakeGenerator: bool, buildThreads: int) -> None:
        self.Log = log
        self.AdditionalBuildArguments = [] # type: List[str]
        # The CMake generator adds the necessary parameters automatically
        # A future pass on this code should move the AddBuildThreads arguments to the generators instead as its more appropriate.
        numBuildThreads = 1
        if not isCMakeGenerator:
            numBuildThreads = PlatformBuildUtil.AddBuildThreads(log, self.AdditionalBuildArguments, generatorOriginalName, buildThreads,
                                                                PlatformBuildTypeInfo.Legacy, None, False)
        else:
            numBuildThreads = PlatformBuildUtil.GetRecommendedBuildThreads(buildThreads)
        self.NumBuildThreads = numBuildThreads



class LocalBuildContext(object):
    def __init__(self, toolConfig: ToolConfig, platformBuildContext: LocalPlatformBuildContext,
                 generatorReportDict: Dict[Package, PackageGeneratorReport],
                 generatorName: str) -> None:
        self.ToolConfig = toolConfig
        self.Platform = platformBuildContext
        self.GeneratorReportDict = generatorReportDict
        self.GeneratorName = generatorName


BuildMethodType = Callable[[LocalBuildContext, Package, BuildConfigRecord, Dict[str, str], Optional[List[str]]], None]

class Builder(object):
    # requestedPackages is the packages specifically requested by the user or None for SDK builds.
    def __init__(self, log: Log, configBuildDir: str, configSDKPath: str, configSDKConfigTemplatePath: str, configDisableWrite: bool,
                 configIsDryRun: bool, toolConfig: ToolConfig, generatorContext: GeneratorContext, topLevelPackage: Package,
                 buildConfig: BuildConfigRecord, enableContentBuilder: bool, forceClaimInstallArea: bool, requestedPackages: List[Package],
                 forceConfigure: bool) -> None:
        super().__init__()

        self.Log = log
        self.UsedGeneratorConfig = None     # type: Optional[GeneratorConfig]
        self.UsedBuildContext = None        # type: Optional[LocalBuildContext]

        localPlatformBuildContext = LocalPlatformBuildContext(self.Log, generatorContext.Generator.OriginalPlatformName, generatorContext.Generator.IsCMake,
                                                              buildConfig.BuildThreads)

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
        RecipeBuilder.BuildPackagesInOrder(self.Log, configSDKPath, configIsDryRun, generatorContext, resolvedBuildOrder, builderSettings)


        resolvedBuildOrderBuildable = PackageFilter.FilterBuildablePackages(resolvedBuildOrder)
        if len(resolvedBuildOrderBuildable) == 0:
            self.Log.DoPrint("Nothing to build!")
            return

        generatorConfig = GeneratorConfig(generatorContext.PlatformName, configSDKConfigTemplatePath, toolConfig,
                                          localPlatformBuildContext.NumBuildThreads, buildConfig.BuildCommand)
        generatorConfigReport = generatorContext.Generator.TryGenerateConfigReport(self.Log, generatorConfig, topLevelPackage)

        packageCount = len(resolvedBuildOrderBuildable)

        resolvedBuildOrderBuildable = self.__ApplyPlatformOrderChanges(resolvedBuildOrderBuildable, buildConfig.PlatformName)
        originalBuildArgs = buildConfig.BuildArgs

        # Handle the configure step
        masterBuildReport = None # type: Optional[GeneratorBuildReport]
        masterBuildVariableReport = None # type: Optional[GeneratorVariableReport]
        if generatorConfigReport is not None:
            # Setup some extra variables for configure
            BuildUtil.AddCustomVariables(generatorConfigReport.VariableReport, toolConfig.ProjectInfo)

            if buildConfig.BuildCommand == CommandType.Config:
                forceConfigure = True
            self.__ConfigureBuild(generatorConfigReport, buildConfig, generatorContext.CMakeConfig.AllowFindPackage, forceConfigure)
            masterBuildReport = generatorConfigReport.MasterBuildReport
            masterBuildVariableReport = generatorConfigReport.MasterBuildVariableReport
            if masterBuildVariableReport is not None:
                BuildUtil.AddCustomVariables(masterBuildVariableReport, toolConfig.ProjectInfo)

        # Acquire information about the build step
        generatorReport = generatorContext.Generator.GenerateReport(self.Log, generatorConfig, resolvedBuildOrderBuildable)
        generatorReportDict = generatorReport.PackageReportDict
        for generatorEntry in generatorReportDict.values():
            if generatorEntry.VariableReport is not None:
                BuildUtil.AddCustomVariables(generatorEntry.VariableReport, toolConfig.ProjectInfo)

        runBuild = (buildConfig.BuildCommand != CommandType.Config and buildConfig.BuildCommand != CommandType.ConfigIfChanged and
                    buildConfig.BuildCommand != CommandType.Open2)

        # Default content building for all platform (for those generators that don't add it to the build file)
        builderCanBuildContent = (generatorConfigReport is not None and generatorConfigReport.CanBuildContent)
        if enableContentBuilder and not builderCanBuildContent:
            for package in resolvedBuildOrderBuildable:
                if package.Type == PackageType.Executable:
                    featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
                    if package.Path is None:
                        raise Exception("Invalid package")
                    if runBuild:
                        ContentBuilder.Build(log, configBuildDir, configDisableWrite, toolConfig, package.Path, featureList)

        # Windows runs its validation checks slightly differently
        runValidationChecks = (buildConfig.PlatformName != PlatformNameString.WINDOWS)

        buildContext = LocalBuildContext(toolConfig, localPlatformBuildContext, generatorReportDict, generatorContext.GeneratorName)

        if runBuild:
            if masterBuildReport is not None:
                if masterBuildVariableReport is None:
                    raise Exception("master-build must have a variable report")
                self.__BuildMaster(buildConfig, buildContext, masterBuildReport, masterBuildVariableReport, topLevelPackage,
                                    originalBuildArgs, builderCanBuildContent, runValidationChecks, configIsDryRun)

            # Build and run all the packages in the resolvedBuildOrderBuildable
            self.__BuildAndRunPackages(log, buildConfig, buildContext, resolvedBuildOrderBuildable, originalBuildArgs,
                                       builderCanBuildContent, runValidationChecks, masterBuildReport is None, configIsDryRun, generatorConfig)

            if packageCount > 0:
                log.LogPrint("Build {0} packages".format(packageCount))
            else:
                log.DoPrint("Nothing build!")

        if buildConfig.BuildCommand == CommandType.Clean:
            if generatorContext.Generator.IsCMake:
                self.Log.DoPrint("*** To do a full cmake clean build delete the out of source build folder ***")
            if not generatorContext.Generator.SupportCommandClean:
                raise Exception("*** clean not supported by this builder ***")
        if buildConfig.BuildCommand == CommandType.Install and not generatorContext.Generator.SupportCommandInstall:
            raise Exception("*** install not supported by this builder ***")
        if buildConfig.BuildCommand == CommandType.Open and not generatorContext.Generator.SupportCommandOpen:
            errorMessage = "open is not supported by this builder."
            if len(generatorContext.Generator.SupportCommandOpenHintMessage) > 0:
                errorMessage = "{0} {1}".format(errorMessage, generatorContext.Generator.SupportCommandOpenHintMessage)
            raise Exception(errorMessage)
        if buildConfig.BuildCommand == CommandType.Open2:
            self.__PerformOpen2(buildConfig, buildContext, generatorContext, generatorConfig, resolvedBuildOrderBuildable, requestedPackages, generatorConfigReport)

        self.UsedGeneratorConfig = generatorConfig
        self.UsedBuildContext = buildContext


    def __PerformOpen2(self, buildConfig: BuildConfigRecord, buildContext: LocalBuildContext, generatorContext: GeneratorContext,
                       generatorConfig: GeneratorConfig, resolvedBuildOrder: List[Package], requestedPackages: List[Package],
                       generatorConfigReport: Optional[PackageGeneratorConfigReport]) -> None:
        if not generatorContext.Generator.IsCMake:
            raise Exception("*** Open2 only support the cmake generator ***")
        if generatorConfigReport is None:
            raise Exception("*** Open2 requires that the cmake generator supplied a config report ***")
        if generatorConfigReport.ConfigReport.ConfigCommandCMakeReport is None:
            raise Exception("*** Open2 requires that the cmake generator supplied a cmake config report ***")

        primaryPackage = Builder.__DeterminePrimaryPackage(resolvedBuildOrder, requestedPackages)
        if primaryPackage not in buildContext.GeneratorReportDict:
            raise Exception("Open2 failed to locate open information for package '{0}'".format(primaryPackage.NameInfo.FullName))

        packageEntry = buildContext.GeneratorReportDict[primaryPackage]
        openProjectReport = packageEntry.OpenProjectReport
        if openProjectReport is None:
            raise Exception("*** Open2 not supported by generator ***")

        buildReport = packageEntry.BuildReport
        variableReport = packageEntry.VariableReport

        runCmdInfo = self.TryGenerateRunCommandForExecutable(buildContext, primaryPackage, buildConfig, ["(EXE)"], generatorConfig)
        exeInfo = None # type: Optional[OpenProjectExecutableInfo]
        if runCmdInfo is not None:
            exeInfo = OpenProjectExecutableInfo(runCmdInfo.RunCommands[0], runCmdInfo.RunPath)

        sourcePath = ReportVariableFormatter.Format(openProjectReport.SourcePath, variableReport, buildConfig.VariantSettingsDict)
        buildSourceDirectory = ReportVariableFormatter.Format(openProjectReport.BuildSourceDirectory, variableReport, buildConfig.VariantSettingsDict)

        cmakeGeneratorName = generatorContext.CMakeConfig.CMakeFinalGeneratorName

        # process the config report
        cmakeConfigReport = generatorConfigReport.ConfigReport.ConfigCommandCMakeReport
        cmakeConfigVariableReport = generatorConfigReport.VariableReport

        cmakeConfigureArgs = cmakeConfigReport.ConfigureArgs
        cmakeBuildDirectory = ReportVariableFormatter.Format(cmakeConfigReport.BuildDirectory, cmakeConfigVariableReport, buildConfig.VariantSettingsDict)
        cmakeConfigureSettingsDict = {} # type: Dict[str,object]
        cmakeInstallPrefix = ""
        for key, value in cmakeConfigReport.ConfigureSettingsDict.items():
            key = ReportVariableFormatter.Format(key, cmakeConfigVariableReport, buildConfig.VariantSettingsDict)
            value = ReportVariableFormatter.Format(value, cmakeConfigVariableReport, buildConfig.VariantSettingsDict)
            if key != "CMAKE_INSTALL_PREFIX":
                cmakeConfigureSettingsDict[key] = value
            else:
                cmakeInstallPrefix = value

        cmakeConfigureSettingsDict["CMAKE_PREFIX_PATH"] = cmakeConfigReport.PrefixPathList

        cmakeInfo = OpenProjectCMakeInfo(cmakeBuildDirectory, cmakeGeneratorName, buildSourceDirectory, cmakeConfigureArgs,
                                         cmakeConfigureSettingsDict, cmakeInstallPrefix, None)
        createInfo = OpenProjectCreateInfo(sourcePath, exeInfo, cmakeInfo)
        OpenProjectUtil.Run(self.Log, createInfo)

    @staticmethod
    def __DeterminePrimaryPackage(resolvedBuildOrder: List[Package], requestedPackages: List[Package]) -> Package:
        # Determine the 'primary' package to open
        if len(requestedPackages) == 1:
            # The user specified one package so the choice is simple
            return requestedPackages[0]
        if len(requestedPackages) > 1:
            # The user specified multiple package, so we chose the 'last' executable package to build in the resolved build order
            requestedExePackages = [package for package in requestedPackages if package.Type == PackageType.Executable]
            if len(requestedExePackages) > 0:
                requestedPackages = requestedExePackages
            # Find the last entry in the resolved build order
            return requestedPackages[0]
        # No requested files, so we just select the last entry in the build order
        return resolvedBuildOrder[-1]

    def __BuildMaster(self, buildConfig: BuildConfigRecord, buildContext: LocalBuildContext,
                      masterBuildReport: GeneratorBuildReport, masterBuildVariableReport: GeneratorVariableReport,
                      topLevelPackage: Package, originalBuildArgs: List[str], builderCanBuildContent: bool,
                      runValidationChecks: bool, isDryRun: bool) -> None:
        self.Log.LogPrintVerbose(2, "Using master-build for better performance")
        buildEnv = self.__CreateBuildEnv(self.Log, buildConfig, buildContext, topLevelPackage, builderCanBuildContent)
        buildConfig.BuildArgs = list(originalBuildArgs)

        if runValidationChecks:
            self.__RunValidationChecks(buildConfig, topLevelPackage)

        self.__Build(buildContext, buildConfig, buildEnv, masterBuildReport, masterBuildVariableReport, None, "build-master")


    def __BuildAndRunPackages(self, log: Log, buildConfig: BuildConfigRecord, buildContext: LocalBuildContext,
                              resolvedBuildOrderBuildable: List[Package], originalBuildArgs: List[str],
                              builderCanBuildContent: bool, runValidationChecks: bool, allowBuild: bool, isDryRun: bool,
                              generatorConfig: GeneratorConfig) -> None:
        for package in resolvedBuildOrderBuildable:
            if allowBuild:
                log.LogPrint("Building package: {0}".format(package.Name))
                log.LogPrint("Package location: {0}".format(package.AbsolutePath))
            if not isDryRun:
                buildEnv = self.__CreateBuildEnv(log, buildConfig, buildContext, package, builderCanBuildContent)
                buildConfig.BuildArgs = list(originalBuildArgs)
                if log.Verbosity > 4:
                    log.DoPrint("Package build arguments1: {0}".format(buildConfig.BuildArgs))
                    log.DoPrint("General build arguments2: {0}".format(originalBuildArgs))

                if runValidationChecks:
                    self.__RunValidationChecks(buildConfig, package)

                # Do the actual package build and then run the package if so desired
                if allowBuild:
                    self.__BuildPackage(buildContext, buildConfig, buildEnv, package)

                # Run commands
                strRunCommands = buildConfig.RunCommand
                runCommands = None  # type: Optional[List[str]]
                if strRunCommands is not None:
                    userRunCommands = shlex.split(strRunCommands)
                    runCmdInfo = self.TryGenerateRunCommandForExecutable(buildContext, package, buildConfig, userRunCommands, generatorConfig)
                    self.__RunPackage(buildContext, package, buildEnv, runCmdInfo)


    def __RunValidationChecks(self, buildConfig: BuildConfigRecord, package: Package) -> None:
        featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
        Validate.ValidatePlatform(self.Log, buildConfig.PlatformName, featureList, 4)


    def __CreateBuildEnv(self, log: Log, buildConfig: BuildConfigRecord, buildContext: LocalBuildContext, package: Package, builderCanBuildContent: bool) -> Dict[str, str]:
        buildEnv = os.environ.copy()  # type: Dict[str, str]
        if not builderCanBuildContent:
            buildEnv[CONFIG_FSLBUILDCONTENT_ENABLED] = "false"

        BuildVariantUtil.ExtendEnvironmentDictWithVariants(log, buildEnv, package, buildConfig.VariantSettingsDict)
        return buildEnv

    def __CheckBuildConfigureModifications(self, cacheFilename: str, generatedFileSet: Set[str],
                                           command: List[str], platformName: str, toolVersionStr: str,
                                           allowFindPackage: bool, forceDirty: bool) -> Optional[BuildConfigureCache]:
        """
        Generate hashes for all files in the set and compare them to the previously saved hashes
        Returns the new cache if its dirty else None if nothing was changed.
        """
        # Generate a hash for all generated files and compare them to the previous "hash"
        self.Log.LogPrintVerbose(4, "Checking current configuration")
        generatedFileDictCache = {} # type: Dict[str,str]
        for filename in generatedFileSet:
            generatedFileDictCache[filename] = IOUtil.HashFile(filename)
        allowFindPackageStr = "true" if allowFindPackage else "false"
        configureCache = BuildConfigureCache(generatedFileDictCache, command, platformName, toolVersionStr, allowFindPackageStr)

        isDirty = True
        self.Log.LogPrintVerbose(5, "- Loading previous configuration cache if present")
        previousConfigureCache = BuildConfigureCache.TryLoad(self.Log, cacheFilename)
        self.Log.LogPrintVerbose(5, "- Comparing cache entries")
        isDirty = previousConfigureCache is None or not BuildConfigureCache.IsEqual(configureCache, previousConfigureCache)

        return configureCache if isDirty or forceDirty else None


    def __ConfigureBuild(self, report: PackageGeneratorConfigReport, buildConfig: BuildConfigRecord, allowFindPackage: bool, forceConfigure: bool) -> None:
        configReport = report.ConfigReport.ConfigCommandReport
        variableReport = report.VariableReport

        buildArgumentList = []
        for buildArgument in configReport.Arguments:
            buildArgument = ReportVariableFormatter.Format(buildArgument, variableReport, buildConfig.VariantSettingsDict)
            buildArgumentList.append(buildArgument)

        configCommandStr = ReportVariableFormatter.Format(configReport.CommandFormatString, variableReport, buildConfig.VariantSettingsDict)

        currentWorkingDirectory = ReportVariableFormatter.Format(configReport.CurrentWorkingDirectoryFormatString,
                                                                 variableReport, buildConfig.VariantSettingsDict)

        configCommand = [configCommandStr] + buildArgumentList # + buildConfig.BuildConfigArgs
        #if len(buildContext.Platform.AdditionalBuildConfigArguments) > 0:
        #    buildCommand += buildContext.Platform.AdditionalBuildConfigArguments

        try:
            IOUtil.SafeMakeDirs(currentWorkingDirectory)

            cacheFilename = IOUtil.Join(currentWorkingDirectory, '.FslConfigureCache.json')

            dirtyBuildConfigureCache = self.__CheckBuildConfigureModifications(cacheFilename, report.GeneratedFileSet, configCommand,
                                                                               buildConfig.PlatformName,
                                                                               buildConfig.ToolVersion.ToMajorMinorPatchString(), allowFindPackage,
                                                                               forceConfigure)
            if dirtyBuildConfigureCache is None:
                self.Log.LogPrint("Build configuration not modified, skipping configure")
                return
            if not forceConfigure:
                self.Log.LogPrint("Build configuration modifed, running configure")
            else:
                self.Log.LogPrint("Forced configure")

            if self.Log.Verbosity >= 1:
                self.Log.LogPrint("Running build config command '{0}' in '{1}'".format(self.__SafeJoinCommandArguments(configCommand), currentWorkingDirectory))

            result = subprocess.call(configCommand, cwd=currentWorkingDirectory)
            if result != 0:
                self.Log.LogPrintWarning("The build config command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(self.__SafeJoinCommandArguments(configCommand), result, currentWorkingDirectory))
                raise ExitException(result)
            else:
                BuildConfigureCache.Save(self.Log, cacheFilename, dirtyBuildConfigureCache)
        except FileNotFoundError:
            self.Log.DoPrintWarning("The build config command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(self.__SafeJoinCommandArguments(configCommand), currentWorkingDirectory))
            raise

    def __SafeJoinCommandArguments(self, strings: List[str]) -> str:
        res = []
        for entry in strings:
            if ' ' in entry:
                entry = '"{0}"'.format(entry)
            res.append(entry)
        return " ".join(res)


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
                       buildConfig: BuildConfigRecord,
                       buildEnv: Dict[str, str],
                       package: Package) -> None:
        if package.AbsolutePath is None or package.ResolvedBuildPath is None:
            raise Exception("Invalid package")
        if buildContext.GeneratorReportDict is None:
            raise Exception("Build not supported by generator '{0}' for package: {1}".format(buildContext.GeneratorName, package.Name))
        if package not in buildContext.GeneratorReportDict:
            raise Exception("Build not supported by generator '{0}' for package: {1}".format(buildContext.GeneratorName, package.Name))

        generatorReport = buildContext.GeneratorReportDict[package]
        buildReport = generatorReport.BuildReport
        if buildReport is None:
            raise Exception("Build report not supported by generator '{0}' for package: {1}".format(buildContext.GeneratorName, package.Name))
        variableReport = generatorReport.VariableReport

        currentWorkingDirectory = package.AbsolutePath
        strHelpContext = "package: '{0}'".format(package.Name)
        self.__Build(buildContext, buildConfig, buildEnv, buildReport, variableReport, currentWorkingDirectory, strHelpContext)


    def __Build(self, buildContext: LocalBuildContext, buildConfig: BuildConfigRecord, buildEnv: Dict[str, str],
                buildReport: GeneratorBuildReport, variableReport: GeneratorVariableReport,
                currentWorkingDirectory: Optional[str], strHelpContext: str) -> None:
        buildCommandReport = buildReport.BuildCommandReport
        if buildCommandReport is None:
            if self.Log.Verbosity >= 5:
                self.Log.LogPrint("Skipping {0} as its build command was None".format(strHelpContext))
            return

        if buildCommandReport.CurrentWorkingDirectoryFormatString is not None:
            currentWorkingDirectory = ReportVariableFormatter.Format(buildCommandReport.CurrentWorkingDirectoryFormatString,
                                                                     variableReport, buildConfig.VariantSettingsDict)
        if currentWorkingDirectory is None:
            raise Exception("No current working directory supplied")

        buildCommandStr = ReportVariableFormatter.Format(buildCommandReport.CommandFormatString, variableReport, buildConfig.VariantSettingsDict)
        if not buildCommandReport.UseAsRelative:
            buildCommandStr = IOUtil.Join(currentWorkingDirectory, buildCommandStr)

        buildArgumentList = []
        for buildArgument in buildCommandReport.Arguments:
            buildArgument = ReportVariableFormatter.Format(buildArgument, variableReport, buildConfig.VariantSettingsDict)
            buildArgumentList.append(buildArgument)

        nativeBuildArgumentList = []
        for buildArgument in buildCommandReport.NativeArguments:
            buildArgument = ReportVariableFormatter.Format(buildArgument, variableReport, buildConfig.VariantSettingsDict)
            nativeBuildArgumentList.append(buildArgument)

        self.__AppendToRightArgumentList(buildArgumentList, nativeBuildArgumentList, buildCommandReport.NativeArgumentSeparator, buildConfig.BuildArgs)
        self.__AppendToRightArgumentList(buildArgumentList, nativeBuildArgumentList, buildCommandReport.NativeArgumentSeparator, buildContext.Platform.AdditionalBuildArguments)

        buildCommand = [buildCommandStr] + buildArgumentList

        if buildCommandReport.NativeArgumentSeparator is not None and len(nativeBuildArgumentList) > 0:
            buildCommand.append(buildCommandReport.NativeArgumentSeparator)
            buildCommand += nativeBuildArgumentList

        try:
            if self.Log.Verbosity >= 1:
                self.Log.LogPrint("Running build command '{0}' in '{1}'".format(self.__SafeJoinCommandArguments(buildCommand), currentWorkingDirectory))
            result = subprocess.call(buildCommand, cwd=currentWorkingDirectory, env=buildEnv)
            if result != 0:
                self.Log.LogPrintWarning("The build command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(self.__SafeJoinCommandArguments(buildCommand), result, currentWorkingDirectory))
                raise ExitException(result)
        except FileNotFoundError:
            self.Log.DoPrintWarning("The build command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(self.__SafeJoinCommandArguments(buildCommand), currentWorkingDirectory))
            raise

    def __AppendToRightArgumentList(self, buildArgumentList: List[str], nativeBuildArgumentList: List[str], nativeArgumentSeparator: Optional[str],
                                    newArguments: List[str]) -> None:
        if len(newArguments) <= 0:
            return
        if nativeArgumentSeparator is None:
            for entry in newArguments:
                buildArgumentList.append(entry)
            return
        # Complex merging
        isNativeArg = False
        for customArg in newArguments:
            if customArg == nativeArgumentSeparator:
                isNativeArg = True
            elif not isNativeArg:
                buildArgumentList.append(customArg)
            else:
                nativeBuildArgumentList.append(customArg)


    def __RunPackage(self, buildContext: LocalBuildContext, package: Package, buildEnv: Dict[str, str], runCmdInfo: Optional[RunCmdInfo]) -> None:
        if runCmdInfo is None:
            return
        if package.AbsolutePath is None:
            raise Exception("Invalid package")
        try:
            currentWorkingDirectory = runCmdInfo.RunPath
            if self.Log.Verbosity >= 1:
                self.Log.LogPrint("Running run command '{0}' in '{1}'".format(self.__SafeJoinCommandArguments(runCmdInfo.RunCommands), currentWorkingDirectory))
            result = subprocess.call(runCmdInfo.RunCommands, cwd=currentWorkingDirectory, env=buildEnv)
            if result != 0:
                self.Log.LogPrintWarning("The run command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(self.__SafeJoinCommandArguments(runCmdInfo.RunCommands), result, currentWorkingDirectory))
                raise ExitException(result)
        except FileNotFoundError:
            self.Log.LogPrintWarning("The run command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(self.__SafeJoinCommandArguments(runCmdInfo.RunCommands), currentWorkingDirectory))
            raise


    def TryGenerateRunCommandForExecutable(self, buildContext: LocalBuildContext,
                                           package: Package,
                                           buildConfig: BuildConfigRecord,
                                           runCommands: Optional[List[str]],
                                           generatorConfig: GeneratorConfig) -> Optional[RunCmdInfo]:
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
                                                             variableReport, buildConfig.VariantSettingsDict,
                                                             executableReport.EnvironmentVariableResolveMethod)
        runPath = package.AbsolutePath

        if buildConfig.Generator is None:
            raise Exception("Generator is missing")
        buildExecutableInfo = buildConfig.Generator.TryGetBuildExecutableInfo(self.Log, generatorConfig, package, generatorReport, buildConfig.VariantSettingsDict)
        if buildExecutableInfo is not None:
            # Override the "install-type" path with the "development" exe path
            foundVariantExePath = buildExecutableInfo.BuildExePath
            runPath = buildExecutableInfo.BuildExeCwdPath

        packagePath = package.AbsolutePath
        fullPathExe = IOUtil.Join(packagePath, foundVariantExePath)
        exeName = IOUtil.GetFileName(foundVariantExePath)
        exePath = IOUtil.GetDirectoryName(fullPathExe)
        contentPath = IOUtil.Join(packagePath, ToolSharedValues.CONTENT_FOLDER_NAME)
        fullBuildDirPath = IOUtil.Join(packagePath, package.ResolvedBuildPath)
        fullBuildDirPath = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(fullBuildDirPath)
        fullPathExe = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(fullPathExe)
        exeName = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(exeName)
        exePath = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(exePath)
        packagePath = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(packagePath)
        contentPath = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(contentPath)
        runPath = buildContext.ToolConfig.ToCurrentOSPathDirectConversion(runPath)

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
            command = command.replace("(RUN_PATH)", runPath)
            commands.append(command)
        return RunCmdInfo(commands, runPath)

# generator = the generator that was used to build the files
def BuildPackages(log: Log, configBuildDir: str, configSDKPath: str, configSDKConfigTemplatePath: str, configDisableWrite: bool, configIsDryRun: bool,
                  toolConfig: ToolConfig, generatorContext: GeneratorContext, packages: List[Package], requestedPackages: Optional[List[Package]],
                  variantSettingsDict: Dict[str, str], buildArgs: List[str], buildForAllExe: Optional[str], generator: GeneratorPluginBase2,
                  enableContentBuilder: bool, forceClaimInstallArea: bool, buildThreads: int, buildCommand: CommandType,
                  printPathIfCMake: bool = False, forceConfigure: bool = False) -> None:

    PlatformUtil.CheckBuildPlatform(generatorContext.PlatformName)
    topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)

    BuildVariantUtil.ValidateUserVariantSettings(log, topLevelPackage, variantSettingsDict)
    BuildVariantUtil.LogVariantSettings(log, variantSettingsDict)

    requestedPackages = [] if requestedPackages is None else requestedPackages
    buildConfig = BuildConfigRecord(toolConfig.ToolVersion, generatorContext.PlatformName, variantSettingsDict, buildCommand, buildArgs, buildForAllExe, generator, buildThreads)

    builder = Builder(log, configBuildDir, configSDKPath, configSDKConfigTemplatePath, configDisableWrite, configIsDryRun, toolConfig,
                      generatorContext, topLevelPackage, buildConfig, enableContentBuilder, forceClaimInstallArea, requestedPackages, forceConfigure)

    # Print executable paths if enabled and its a cmake type build
    if printPathIfCMake and generatorContext.Generator.IsCMake and buildCommand == CommandType.Build and topLevelPackage is not None:
        for depPackage in topLevelPackage.ResolvedAllDependencies:
            package = depPackage.Package
            if package.Type == PackageType.Executable and builder.UsedBuildContext is not None and builder.UsedGeneratorConfig is not None:
                if package.ResolvedPlatformSupported:
                    runCmdInfo = builder.TryGenerateRunCommandForExecutable(builder.UsedBuildContext, package, buildConfig, ["(EXE)"], builder.UsedGeneratorConfig)
                    if runCmdInfo is not None:
                        if IOUtil.GetCurrentWorkingDirectory() != depPackage.Package.AbsolutePath:
                            log.DoPrint("Run command: cd to '{0}' then run 'FslBuildRun.py'".format(depPackage.Package.AbsolutePath))
                        else:
                            log.DoPrint("Run command: 'FslBuildRun.py'")
                        log.DoPrint("- Executable at: '{0}'".format(runCmdInfo.RunCommands[0]))
                        log.DoPrint("- Exe CWD at:    '{0}'".format(runCmdInfo.RunPath))
                else:
                    log.LogPrint("Package '{0}' was not supported on this platform".format(package.Name))


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowVariantList(log: Log,
                    topLevelPackage: Package,
                    requestedFiles: Optional[List[str]],
                    generator: GeneratorPluginBase2) -> None:

    variantDict = BuildVariantUtil.BuildCompleteVariantDict(topLevelPackage)


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
        groupedRequirements.sort(key=lambda s: "" if s.Content is None else s.Content.Id)
        for childNode in groupedRequirements:
            __PrintRequirementsNode(log, childNode, currentIndent + strAddIndent, strAddIndent)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowRequirementList(log: Log,
                        topLevelPackage: Package,
                        requestedFiles: Optional[List[str]],
                        showFeaturesOnly: bool = False) -> None:
    message = "Requirements" if not showFeaturesOnly else "Features"
    filterName = None if not showFeaturesOnly else PackageRequirementTypeString.Feature
    # As the packages in requestedFiles might have been filtered at this point (and any issues already caught), we just ignore not found
    requestedPackages = PackageUtil.GetPackageListFromFilenames(topLevelPackage, requestedFiles, True)
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
    sortedFeatures.sort(key=lambda s: "" if s.Content is None else s.Content.Id)
    for sortedFeature in sortedFeatures:
        __PrintRequirementsNode(log, sortedFeature, baseIndent, strAddIndent)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowFeatureList(log: Log,
                    topLevelPackage: Package,
                    requestedFiles: Optional[List[str]]) -> None:
    ShowRequirementList(log, topLevelPackage, requestedFiles, True)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowExtensionList(log: Log,
                      topLevelPackage: Package,
                      requestedFiles: Optional[List[str]]) -> None:
    # As the packages in requestedFiles might have been filtered at this point (and any issues already caught), we just ignore not found
    requestedPackages = PackageUtil.GetPackageListFromFilenames(topLevelPackage, requestedFiles, True)
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
