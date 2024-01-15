#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2017 NXP
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

#from typing import Any
#from typing import cast
#from typing import Dict
from typing import List
from typing import Optional
#import hashlib
#import json
from FslBuildGen import IOUtil
#from FslBuildGen import PackageUtil
from FslBuildGen import PackageListUtil
from FslBuildGen.BasicConfig import BasicConfig
#from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.Build.BuildConfigRecord import BuildConfigRecord
from FslBuildGen.Build.DataTypes import CommandType
#from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.BuildExternal.BuilderConfig import BuilderConfig
from FslBuildGen.BuildExternal.BuilderSettings import BuilderSettings
#from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.BuildExternal.Pipeline import RecipeRecord
from FslBuildGen.BuildExternal.PipelineCommandBuilder import PipelineCommandBuilder
from FslBuildGen.BuildExternal.ValidationEngine import ErrorRecord
from FslBuildGen.BuildExternal.ValidationEngine import InstallationStatus
from FslBuildGen.BuildExternal.ValidationEngine import ValidationEngine
from FslBuildGen.BuildExternal.State.BuildAreaInfoFileUtil import BuildAreaInfoFileUtil
from FslBuildGen.BuildExternal.State.BuildInfoFileUtil import BuildInfoFileUtil
#from FslBuildGen.BuildExternal.State.RecipePackageState import RecipePackageState
from FslBuildGen.BuildExternal.State.RecipePackageStateCache import RecipePackageStateCache
from FslBuildGen.BuildExternal.State.PackageRecipeUtil import PackageRecipeUtil
#from FslBuildGen.ErrorHelpManager import ErrorHelpManager
from FslBuildGen.ExternalVariantConstraints import ExternalVariantConstraints
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
#from FslBuildGen.BuildContent.Sync.BuildState import BasicContent
#from FslBuildGen.BuildContent.Sync.BuildState import BasicContentState
from FslBuildGen.ToolConfig import ToolConfig

__g_installAreaInformationFilename = "BuildDirInfo.json"
__g_BuildPackageInformationFilename = "BuildInfo.json"
__g_BuildPackageInformationOverrideFilename = "BuildOverride"


def __RunValidationEngineCheck(validationEngine: ValidationEngine, package: Package) -> bool:
    errorRecordList = []  # type: List[ErrorRecord]
    status = validationEngine.GetInstallationStatus(package, errorRecordList)
    if status == InstallationStatus.Installed or status == InstallationStatus.Undefined:
        return True
    elif status == InstallationStatus.EnvironmentError:
        validationEngine.RaiseExceptionFromErrorRecords(package, errorRecordList)
    return False


# packagesToBuild contains all packages that are scheduled for a rebuild
def __TryValidateInstallation(log: Log, validationEngine: ValidationEngine, package: Package, packagesToBuild: List[Package],
                              recipePackageStateCache: RecipePackageStateCache, cmakeConfig: GeneratorCMakeConfig) -> bool:
    if package.ResolvedDirectExperimentalRecipe is None:
        raise Exception("Invalid package")
    sourceRecipe = package.ResolvedDirectExperimentalRecipe
    installPath = sourceRecipe.ResolvedInstallLocation
    if installPath is not None:
        if not IOUtil.IsDirectory(installPath.ResolvedPath):
            log.LogPrintVerbose(2, "Installation directory not located: {0}".format(installPath.ResolvedPath))
            return False
        elif log.Verbosity >= 2:
            log.LogPrint("Installation directory located at '{0}'".format(installPath.ResolvedPath))

    # Check if the user decided to do a build override by creating the required file.
    # This allows the user to tell the system that it has been build and it should mind its own buisness
    packageHasUserBuildOverride = False
    if not installPath is None:
        overrideFilename = IOUtil.Join(installPath.ResolvedPath, __g_BuildPackageInformationOverrideFilename)
        packageHasUserBuildOverride = IOUtil.IsFile(overrideFilename)
        if packageHasUserBuildOverride:
            log.LogPrint("Package {0} contained a build override file '{1}'".format(package.Name, __g_BuildPackageInformationOverrideFilename))

    if not __RunValidationEngineCheck(validationEngine, package):
        if packageHasUserBuildOverride:
            raise Exception("Package {0} contained a build override file '{1}', but it failed validation. Fix the issues or delete the override file '{2}'".format(package.Name, __g_BuildPackageInformationOverrideFilename, overrideFilename))
        log.LogPrintVerbose(2, "Install validation failed")
        return False

    # If there is a user build override we dont check the build dependency json file
    if packageHasUserBuildOverride:
        return True

    # If there is no build pipeline we consider the validation to be completed, else we need to check the saved build info
    if not PackageRecipeUtil.HasBuildPipeline(package):
        return True

    if not BuildInfoFileUtil.TryValidateBuildInformation(log, package, packagesToBuild, recipePackageStateCache, cmakeConfig,
                                                         __g_BuildPackageInformationFilename):
        log.LogPrintVerbose(2, "Install validator failed to load build information")
        return False
    return True


def __FindMissingInstallations(log: Log,
                               validationEngine: ValidationEngine,
                               resolvedBuildOrder: List[Package],
                               recipePackageStateCache: RecipePackageStateCache,
                               cmakeConfig: GeneratorCMakeConfig) -> List[Package]:
    """ Check packages in the resolvedBuildOrder and return all that fails install validation, keeping the initial order """
    missingPackages = []  # type: List[Package]
    for package in resolvedBuildOrder:
        log.LogPrint("Checking if package {0} is installed".format(package.Name))
        try:
            log.PushIndent()
            if not __TryValidateInstallation(log, validationEngine, package, missingPackages, recipePackageStateCache, cmakeConfig):
                missingPackages.append(package)
        finally:
            log.PopIndent()
    return missingPackages


def __CreatePipelines(log: Log,
                      builder: PipelineCommandBuilder,
                      resolvedBuildOrder: List[Package]) -> List[RecipeRecord]:
    pipelines = []  # type: List[RecipeRecord]
    for package in resolvedBuildOrder:
        log.LogPrint("Creating package {0} build pipelines".format(package.Name))
        try:
            log.PushIndent()
            record = RecipeRecord(log, builder, package)
            pipelines.append(record)
        finally:
            log.PopIndent()
    return pipelines




def ValidateInstallationForPackages(log: Log, configSDKPath: str,
                                    generatorContext: GeneratorContext,
                                    resolvedBuildOrder: List[Package],
                                    builderSettings: BuilderSettings = BuilderSettings(),
                                    packageRecipeResultManager: Optional[PackageRecipeResultManager] = None) -> None:
    if packageRecipeResultManager is None:
        packageRecipeResultManager = PackageRecipeResultManager(log)

    if not generatorContext.RecipePathBuilder.IsEnabled:
        log.LogPrintVerbose(3, "External building has been disabled in the Project.gen file")
        return

    if generatorContext.RecipePathBuilder.TargetLocation is None:
        raise Exception("Invalid path builder")

    # Claim the 'package' install directory to prevent multiple builds from using the same
    # as it would give concurrency issues
    BuildAreaInfoFileUtil.ProcessInstallDirClaim(log, generatorContext.RecipePathBuilder.TargetLocation.ResolvedPath, configSDKPath,
                                                 builderSettings.ForceClaimInstallArea, __g_installAreaInformationFilename)

    if resolvedBuildOrder is None:
        return

    # Filter all packages that don't have a experimental recipe
    resolvedBuildOrder = [entry for entry in resolvedBuildOrder if not entry.ResolvedDirectExperimentalRecipe is None]

    if len(resolvedBuildOrder) == 0:
        return

    recipePackageStateCache = RecipePackageStateCache(log)

    # Here we basically run the installation validation engine and see if there is anything that triggers a exception
    validationEngine = ValidationEngine(log, generatorContext.VariableProcessor, packageRecipeResultManager, generatorContext.ErrorHelpManager)
    __FindMissingInstallations(log, validationEngine, resolvedBuildOrder, recipePackageStateCache, generatorContext.CMakeConfig)

# requestedPackages is the packages specifically requested by the user or None for SDK builds.
def BuildPackagesInOrder(log: Log, configSDKPath: str, configIsDryRun: bool,
                         generatorContext: GeneratorContext,
                         resolvedBuildOrder: List[Package],
                         builderSettings: BuilderSettings = BuilderSettings(),
                         packageRecipeResultManager: Optional[PackageRecipeResultManager] = None) -> None:
    if packageRecipeResultManager is None:
        packageRecipeResultManager = PackageRecipeResultManager(log)
    __DoBuildPackagesInOrder(log, configSDKPath, configIsDryRun, generatorContext, resolvedBuildOrder, builderSettings, packageRecipeResultManager)


def __DoBuildPackagesInOrder(log: Log, configSDKPath: str, configIsDryRun: bool, generatorContext: GeneratorContext,
                             resolvedBuildOrder: List[Package], builderSettings: BuilderSettings,
                             packageRecipeResultManager: PackageRecipeResultManager) -> None:
    if not generatorContext.RecipePathBuilder.IsEnabled:
        log.LogPrintVerbose(3, "External building has been disabled in the Project.gen file")
        return
    if generatorContext.RecipePathBuilder.TargetLocation is None:
        raise Exception("Invalid path builder")

    # Claim the 'package' install directory to prevent multiple builds from using the same
    # as it would give concurrency issues
    BuildAreaInfoFileUtil.ProcessInstallDirClaim(log, generatorContext.RecipePathBuilder.TargetLocation.ResolvedPath, configSDKPath,
                                                 builderSettings.ForceClaimInstallArea, __g_installAreaInformationFilename)

    if resolvedBuildOrder is None:
        log.LogPrintVerbose(2, "No recipes to build")
        return

    # Filter all packages that don't have a experimental recipe
    resolvedBuildOrder = [entry for entry in resolvedBuildOrder if not entry.ResolvedDirectExperimentalRecipe is None]

    if len(resolvedBuildOrder) == 0:
        log.LogPrintVerbose(2, "No recipes to build")
        return


    recipePackageStateCache = RecipePackageStateCache(log)
    validationEngine = ValidationEngine(log, generatorContext.VariableProcessor, packageRecipeResultManager, generatorContext.ErrorHelpManager)
    missingPackagesInBuildOrder = __FindMissingInstallations(log, validationEngine, resolvedBuildOrder, recipePackageStateCache,
                                                             generatorContext.CMakeConfig)
    builder = PipelineCommandBuilder(generatorContext, builderSettings.CheckBuildCommands, builderSettings.BuildThreads)
    recipeRecords = __CreatePipelines(log, builder, missingPackagesInBuildOrder)

    for recipeRecord in recipeRecords:
        log.LogPrint("Package location: {0}".format(recipeRecord.SourcePackage.AbsolutePath))
        try:
            log.PushIndent()
            if not recipeRecord.SourcePackage.ResolvedPlatformDirectSupported:
                raise Exception("The package '{0}' is not supported on this platform".format(recipeRecord.SourcePackage.Name))
            if not recipeRecord.Pipeline is None:
                log.DoPrint("Building package: {0}".format(recipeRecord.SourcePackage.Name))
                if builderSettings.PreDeleteBuild:
                    # We clear the build path to prepare for a new build
                    IOUtil.SafeRemoveDirectoryTree(recipeRecord.Pipeline.BuildPath)

                for command in recipeRecord.Pipeline.CommandList:
                    if not configIsDryRun:
                        command.Execute()

                # We finished building, so lets save some information about what we did
                BuildInfoFileUtil.SaveBuildInformation(log, recipeRecord, recipePackageStateCache, generatorContext.CMakeConfig,
                                                       __g_BuildPackageInformationFilename)

                if builderSettings.PostDeleteBuild:
                    # We clear the build path if a build is successfull
                    IOUtil.SafeRemoveDirectoryTree(recipeRecord.Pipeline.BuildPath, True)

            else:
                # Since we are trying to build this it means that the installation validation failed earlier and
                # we apparently have no pipelines that could remedy it, so force the install validation to occur so
                # we fail early as 'dependent' pipes might fail to build due to this
                # generatorContext.RecipeFilterManager
                if generatorContext.RecipeFilterManager.AllRecipesEnabled or recipeRecord.SourcePackage.Name in generatorContext.RecipeFilterManager.ContentDict:
                    log.DoPrintWarning("Missing installation of package '{0}' and no recipe for solving it is available".format(recipeRecord.SourcePackage.Name))
                else:
                    log.LogPrintVerbose(4, "Package '{0}' recipe not enabled".format(recipeRecord.SourcePackage.Name))
                validationEngine.Process(recipeRecord.SourcePackage)
        finally:
            log.PopIndent()

        validationEngine.Process(recipeRecord.SourcePackage)

    packageCount = len(recipeRecords)
    if packageCount > 0:
        log.LogPrint("Build {0} packages".format(packageCount))
    else:
        log.LogPrintVerbose(2, "No recipe was build!")


def __BuildNow(log: Log, configSDKPath: str, configIsDryRun: bool, generatorContext: GeneratorContext, builderConfig: BuilderConfig,
               topLevelPackage: Package, buildConfig: BuildConfigRecord, packageRecipeResultManager: PackageRecipeResultManager) -> None:
    __DoBuildPackagesInOrder(log, configSDKPath, configIsDryRun, generatorContext, topLevelPackage.ResolvedBuildOrder, builderConfig.Settings, packageRecipeResultManager)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
# generator = the generator that was used to build the files
def BuildPackages(log: Log, configSDKPath: str, configIsDryRun: bool, toolConfig: ToolConfig,
                  generatorContext: GeneratorContext,
                  builderConfig: BuilderConfig,
                  packages: List[Package],
                  packageRecipeResultManager: Optional[PackageRecipeResultManager] = None) -> None:
    if packageRecipeResultManager is None:
        packageRecipeResultManager = PackageRecipeResultManager(log)

    PlatformUtil.CheckBuildPlatform(generatorContext.Platform.PlatformName)
    topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)

    buildConfig = BuildConfigRecord(toolConfig.ToolVersion, generatorContext.Platform.PlatformName, ExternalVariantConstraints({}),
                                    generatorContext.GeneratorInfo.VariableContext.UserSetVariables, CommandType.Build, [], [], None, None, 0)

    try:
        log.LogPrint("- Building recipe packages")
        log.PushIndent()
        __BuildNow(log, configSDKPath, configIsDryRun, generatorContext, builderConfig, topLevelPackage, buildConfig, packageRecipeResultManager)
    finally:
        log.PopIndent()
