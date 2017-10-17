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

from typing import Any
from typing import cast
from typing import Dict
from typing import List
from typing import Optional
import json
from FslBuildGen import IOUtil
from FslBuildGen import PackageUtil
from FslBuildGen import PackageListUtil
from FslBuildGen import PlatformUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Config import Config
from FslBuildGen.Build.BuildConfigRecord import BuildConfigRecord
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.BuildExternal.BuilderConfig import BuilderConfig
from FslBuildGen.BuildExternal.BuilderSettings import BuilderSettings
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.Pipeline import RecipeRecord
from FslBuildGen.BuildExternal.PipelineCommandBuilder import PipelineCommandBuilder
from FslBuildGen.BuildExternal.ValidationEngine import ErrorRecord
from FslBuildGen.BuildExternal.ValidationEngine import InstallationStatus
from FslBuildGen.BuildExternal.ValidationEngine import ValidationEngine
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package


__g_installAreaInformationFilename = "BuildDirInfo.json"
__g_BuildPackageInformationFilename = "BuildInfo.json"
__g_BuildPackageInformationOverrideFilename = "BuildOverride"

JSonDictType = Dict[str, Any]


class BuildInfoFileElements(object):
    PackageName = "PackageName"
    PackageDependencies = "PackageDependencies"
    FileFormatVersion = "FileFormatVersion"

    CURRENT_VERSION = "1"

class BuildInfoFile(object):
    def __init__(self, jsonDict: JSonDictType) -> None:
        super(BuildInfoFile, self).__init__()
        self.PackageName = jsonDict[BuildInfoFileElements.PackageName]
        self.PackageDependencies = jsonDict[BuildInfoFileElements.PackageDependencies]
        self.FileFormatVersion = jsonDict[BuildInfoFileElements.FileFormatVersion]

    @staticmethod
    def IsDictValid(srcDict: JSonDictType) -> bool:
        if not BuildInfoFileElements.PackageName in srcDict or not BuildInfoFileElements.PackageDependencies in srcDict or not BuildInfoFileElements.FileFormatVersion in srcDict:
            return False

        if not isinstance(srcDict[BuildInfoFileElements.PackageName], str):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.PackageDependencies], list):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.FileFormatVersion], str):
            return False

        return cast(bool, srcDict[BuildInfoFileElements.FileFormatVersion] == BuildInfoFileElements.CURRENT_VERSION)


    @staticmethod
    def TryCreateJsonBuildInfoRootDict(sourcePackage: Package, sourceRecipe: PackageExperimentalRecipe) -> Optional[JSonDictType]:
        if sourcePackage is None or sourceRecipe is None or sourceRecipe.ResolvedInstallPath is None:
            return None

        referencedPackageSet = PackageListUtil.BuildReferencedPackageSet([sourcePackage])
        referencedPackageNameList = [package.Name for package in referencedPackageSet]  # type: List[str]
        referencedPackageNameList.sort()

        jsonRootDict = {}  # type: JSonDictType
        jsonRootDict[BuildInfoFileElements.PackageName] = sourcePackage.Name
        jsonRootDict[BuildInfoFileElements.PackageDependencies] = referencedPackageNameList
        jsonRootDict[BuildInfoFileElements.FileFormatVersion] = BuildInfoFileElements.CURRENT_VERSION
        return jsonRootDict


def __HasBuildPipeline(sourcePackage: Package) -> bool:
    if sourcePackage is None or sourcePackage.ResolvedDirectExperimentalRecipe is None or sourcePackage.ResolvedDirectExperimentalRecipe.ResolvedInstallPath is None:
        return False

    return not sourcePackage.ResolvedDirectExperimentalRecipe.Pipeline is None


def __TryLoadBuildInformation(log: Log, sourcePackage: Package) -> Optional[JSonDictType]:
    if not __HasBuildPipeline(sourcePackage):
        return None

    sourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
    if sourceRecipe is None or sourceRecipe.ResolvedInstallPath is None:
        raise Exception("Invalid recipe")

    # Generally this should not be called if there is no pipeline

    srcFilePath = IOUtil.Join(sourceRecipe.ResolvedInstallPath, __g_BuildPackageInformationFilename)

    fileContent = IOUtil.TryReadFile(srcFilePath)
    if fileContent is None:
        log.LogPrint("Package build information for package {0} not found in the expected file '{1}'".format(sourcePackage.Name, srcFilePath))
        return None

    jsonBuildInfoDict = json.loads(fileContent)
    if not BuildInfoFile.IsDictValid(jsonBuildInfoDict):
        log.LogPrint("Package build information for package {0} found in file '{1}' is invalid".format(sourcePackage.Name, srcFilePath))
        return None
    return cast(JSonDictType, jsonBuildInfoDict)


def __TryValidateBuildInformation(log: Log, sourcePackage: Package) -> bool:
    if not __HasBuildPipeline(sourcePackage):
        return False

    loadedBuildInfoDict = __TryLoadBuildInformation(log, sourcePackage)
    if loadedBuildInfoDict is None:
        return False

    if sourcePackage.ResolvedDirectExperimentalRecipe is None:
        raise Exception("Invalid package")

    currentInfoDict = BuildInfoFile.TryCreateJsonBuildInfoRootDict(sourcePackage, sourcePackage.ResolvedDirectExperimentalRecipe)
    if currentInfoDict is None:
        log.LogPrint("Failed to create Package build information for package {0}".format(sourcePackage.Name))
        return False

    loadedInfo = BuildInfoFile(loadedBuildInfoDict)
    currentInfo = BuildInfoFile(currentInfoDict)

    if loadedInfo.PackageName != currentInfo.PackageName:
        log.LogPrint("The current package name {0} did not match the stored package {1}".format(currentInfo.PackageName, loadedInfo.PackageName))
        return False

    if len(currentInfo.PackageDependencies) != len(loadedInfo.PackageDependencies):
        log.LogPrint("The current package dependencies {0} did not match the stored package dependencies {1}".format(currentInfo.PackageDependencies, loadedInfo.PackageDependencies))
        return False

    for dependencyName in currentInfo.PackageDependencies:
        if not dependencyName in loadedInfo.PackageDependencies:
            log.LogPrint("The dependency {0} did not exist in the stored package {1}".format(dependencyName, loadedInfo.PackageName))
            return False
    return True



def __SaveBuildInformation(recipeRecord: Optional[RecipeRecord]) -> None:
    if recipeRecord is None or not __HasBuildPipeline(recipeRecord.SourcePackage):
        return

    jsonRootDict = BuildInfoFile.TryCreateJsonBuildInfoRootDict(recipeRecord.SourcePackage, recipeRecord.SourceRecipe)
    if jsonRootDict is None:
        return

    if recipeRecord.SourceRecipe.ResolvedInstallPath is None:
        raise Exception("Invalid package")

    jsonText = json.dumps(jsonRootDict, ensure_ascii=False, sort_keys=True, indent=2)

    dstFilePath = IOUtil.Join(recipeRecord.SourceRecipe.ResolvedInstallPath, __g_BuildPackageInformationFilename)
    IOUtil.WriteFileIfChanged(dstFilePath, jsonText)


def __RunValidationEngineCheck(validationEngine: ValidationEngine, package: Package) -> bool:
    errorRecordList = []  # type: List[ErrorRecord]
    status = validationEngine.GetInstallationStatus(package, errorRecordList)
    if status == InstallationStatus.Installed or status == InstallationStatus.Undefined:
        return True
    elif status == InstallationStatus.EnvironmentError:
        validationEngine.RaiseExceptionFromErrorRecords(package, errorRecordList)
    return False


def __TryValidateInstallation(basicConfig: BasicConfig, validationEngine: ValidationEngine, package: Package) -> bool:
    if package.ResolvedDirectExperimentalRecipe is None:
        raise Exception("Invalid package")
    sourceRecipe = package.ResolvedDirectExperimentalRecipe
    installPath = sourceRecipe.ResolvedInstallPath
    if installPath is not None and not IOUtil.IsDirectory(installPath):
        basicConfig.LogPrintVerbose(2, "Installation directory not located: {0}".format(installPath))
        return False
    elif basicConfig.Verbosity >= 2:
        basicConfig.LogPrint("Installation directory located at '{0}'".format(installPath))

    # Check if the user decided to do a build override by creating the required file.
    # This allows the user to tell the system that it has been build and it should mind its own buisness
    packageHasUserBuildOverride = False
    if not installPath is None:
        overrideFilename = IOUtil.Join(installPath, __g_BuildPackageInformationOverrideFilename)
        packageHasUserBuildOverride = IOUtil.IsFile(overrideFilename)
        if packageHasUserBuildOverride:
            basicConfig.LogPrint("Package {0} contained a build override file '{1}'".format(package.Name, __g_BuildPackageInformationOverrideFilename))

    if not __RunValidationEngineCheck(validationEngine, package):
        if packageHasUserBuildOverride:
            raise Exception("Package {0} contained a build override file '{1}', but it failed validation. Fix the issues or delete the override file '{2}'".format(package.Name, __g_BuildPackageInformationOverrideFilename, overrideFilename))
        basicConfig.LogPrintVerbose(2, "Install validation failed")
        return False

    # If there is a user build override we dont check the build dependency json file
    if packageHasUserBuildOverride:
        return True

    # If there is no build pipeline we consider the validation to be completed, else we need to check the saved build info
    if not __HasBuildPipeline(package):
        return True

    if not __TryValidateBuildInformation(basicConfig, package):
        basicConfig.LogPrintVerbose(2, "Install validator failed to load build information")
        return False
    return True


def __FindMissingInstallations(basicConfig: BasicConfig,
                               validationEngine: ValidationEngine,
                               resolvedBuildOrder: List[Package]) -> List[Package]:
    """ Check packages in the resolvedBuildOrder and return all that fails install validation, keeping the initial order """
    missingPackages = []  # type: List[Package]
    for package in resolvedBuildOrder:
        basicConfig.LogPrint("Checking if package {0} is installed".format(package.Name))
        try:
            basicConfig.PushIndent()
            if not __TryValidateInstallation(basicConfig, validationEngine, package):
                missingPackages.append(package)
        finally:
            basicConfig.PopIndent()
    return missingPackages


def __CreatePipelines(basicConfig: BasicConfig,
                      builder: PipelineCommandBuilder,
                      resolvedBuildOrder: List[Package]) -> List[RecipeRecord]:
    pipelines = []  # type: List[RecipeRecord]
    for package in resolvedBuildOrder:
        basicConfig.LogPrint("Creating package {0} build pipelines".format(package.Name))
        try:
            basicConfig.PushIndent()
            record = RecipeRecord(basicConfig, builder, package)
            pipelines.append(record)
        finally:
            basicConfig.PopIndent()
    return pipelines


class BuildAreaInfoFileElements(object):
    FileFormatVersion = "FileFormatVersion"
    SDKPath = "SDKPath"

    CURRENT_VERSION = "1"


class BuildAreaInfoFile(object):
    def __init__(self, jsonDict: JSonDictType) -> None:
        super(BuildAreaInfoFile, self).__init__()
        self.FileFormatVersion = jsonDict[BuildAreaInfoFileElements.FileFormatVersion]
        self.SDKPath = IOUtil.NormalizePath(jsonDict[BuildAreaInfoFileElements.SDKPath])

    @staticmethod
    def IsDictValid(srcDict: JSonDictType) -> bool:
        if not BuildAreaInfoFileElements.FileFormatVersion in srcDict or not BuildAreaInfoFileElements.SDKPath in srcDict:
            return False
        if not isinstance(srcDict[BuildAreaInfoFileElements.FileFormatVersion], str):
            return False
        if not isinstance(srcDict[BuildAreaInfoFileElements.SDKPath], str):
            return False

        return cast(bool, srcDict[BuildAreaInfoFileElements.FileFormatVersion] == BuildAreaInfoFileElements.CURRENT_VERSION)

    @staticmethod
    def CreateDict(sdkPath: str) -> JSonDictType:
        jsonRootDict = {}
        jsonRootDict[BuildAreaInfoFileElements.FileFormatVersion] = BuildAreaInfoFileElements.CURRENT_VERSION
        jsonRootDict[BuildAreaInfoFileElements.SDKPath] = sdkPath
        return jsonRootDict



def __SaveInstallAreaInfo(dstFilePath: str, sdkPath: str) -> JSonDictType:
    jsonRootDict = BuildAreaInfoFile.CreateDict(sdkPath)
    jsonText = json.dumps(jsonRootDict, ensure_ascii=False, sort_keys=True, indent=2)
    IOUtil.WriteFileIfChanged(dstFilePath, jsonText)
    return jsonRootDict


def __ClaimInstallDirNow(log: Log,
                         targetPath: str,
                         dstFilePath: str,
                         sdkPath: str,
                         forceClaimInstallArea: bool,
                         logWarning: bool = True) -> JSonDictType:
    # Since we are claiming the install area, we check to see if its empty as expected
    files = IOUtil.GetFilePaths(targetPath, None)
    # Then give the user error about the files at the install area can be lost unless it is changed
    if len(files) > 0:
        if not forceClaimInstallArea:
            raise Exception("The install area at '{0}' was unclaimed, but it was not empty. To allow the tool to use the directory and do with its content as it see fit.\nYou need to rerun the command with the --ForceClaimInstallArea parameter to allow it, but BEWARE that doing so means the content of '{0}' can be lost".format(targetPath))
        if logWarning:
            log.DoPrintWarning("The install area was not empty but the user enabled --ForceClaimInstallArea and allowed the tool to use it, the files there could be lost because of it.")

    # then save the claim file
    return __SaveInstallAreaInfo(dstFilePath, sdkPath)


def __ProcessInstallDirClaim(log: Log,
                             targetPath: str,
                             sdkPath: str,
                             forceClaimInstallArea: bool) -> None:
    filePath = IOUtil.Join(targetPath, __g_installAreaInformationFilename)

    # Beware that this method to claim a area is in no way secure
    # It will basically only be useful after the claim file has been created
    # So we start with a possible race condition, but any future attempts to use the directory
    # will catch that multiple repos are trying to reuse the same install area
    fileContent = IOUtil.TryReadFile(filePath)
    if fileContent is None:
        log.LogPrint("Install area '{0}' is unclaimed, claiming it".format(targetPath))
        __ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea)
        return

    jsonBuildInfoDict = json.loads(fileContent)
    if not BuildAreaInfoFile.IsDictValid(jsonBuildInfoDict):
        if not forceClaimInstallArea:
            raise Exception("Install area '{0}' contained an invalid file '{1}', did you try to run concurrent builds using the same install directory or did it get corrupted? Delete the file to allow the build to continue or use --ForceClaimInstallArea to do so automatically. You could also set up a readonly cache area to reuse between repos see the documentation for more info.".format(targetPath, filePath))
        log.DoPrintWarning("Install area '{0}' contained an invalid file '{1}', as --ForceClaimInstallArea was specified it was overwritten and '{2}' now controls it".format(targetPath, filePath, sdkPath))
        jsonBuildInfoDict = __ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea, logWarning=False)

    buildAreaInfoFile = BuildAreaInfoFile(jsonBuildInfoDict)
    if buildAreaInfoFile.SDKPath != sdkPath:
        if not forceClaimInstallArea:
            raise Exception("The Install area at '{0}' is already claimed by the sdk at '{1}' so the sdk at '{2}' can not reuse it as it could give concurrency issues if multiple builds execute at the same time. If you are sure that you are not doing concurrent builds and you just want to use the area for a new SDK you can force claim it with --ForceClaimInstallArea. You could also set up a readonly cache area to reuse between repos see the documentation for more info.".format(targetPath, buildAreaInfoFile.SDKPath, sdkPath))
        log.DoPrintWarning("The Install area at '{0}' was already claimed by the sdk at '{1}' but '{2}' took control of it as --ForceClaimInstallArea was specified.".format(targetPath, buildAreaInfoFile.SDKPath, sdkPath))
        jsonBuildInfoDict = __ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea, logWarning=False)
        #jsonBuildInfoDict = __ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea, logWarning=False)
        #buildAreaInfoFile = BuildAreaInfoFile(jsonBuildInfoDict)


def ValidateInstallationForPackages(config: Config,
                                    generatorContext: GeneratorContext,
                                    resolvedBuildOrder: List[Package],
                                    builderSettings: BuilderSettings = BuilderSettings()) -> None:
    basicConfig = generatorContext.BasicConfig
    if not generatorContext.RecipePathBuilder.IsEnabled:
        basicConfig.LogPrintVerbose(3, "External building has been disabled in the Project.gen file")
        return

    if generatorContext.RecipePathBuilder.TargetPath is None:
        raise Exception("Invalid path builder")

    # Claim the 'package' install directory to prevent multiple builds from using the same
    # as it would give concurrency issues
    __ProcessInstallDirClaim(basicConfig, generatorContext.RecipePathBuilder.TargetPath, config.SDKPath, builderSettings.ForceClaimInstallArea)

    if resolvedBuildOrder is None:
        return

    # Filter all packages that don't have a experimental recipe
    resolvedBuildOrder = [entry for entry in resolvedBuildOrder if not entry.ResolvedDirectExperimentalRecipe is None]

    if len(resolvedBuildOrder) == 0:
        return

    # Here we basically run the installation validation engine and see if there is anything that triggers a exception
    validationEngine = ValidationEngine(basicConfig, generatorContext.VariableProcessor)
    __FindMissingInstallations(basicConfig, validationEngine, resolvedBuildOrder)


# requestedPackages is the packages specifically requested by the user or None for SDK builds.
def BuildPackagesInOrder(config: Config,
                         generatorContext: GeneratorContext,
                         resolvedBuildOrder: List[Package],
                         builderSettings: BuilderSettings = BuilderSettings()) -> None:
    basicConfig = generatorContext.BasicConfig
    if not generatorContext.RecipePathBuilder.IsEnabled:
        basicConfig.LogPrintVerbose(3, "External building has been disabled in the Project.gen file")
        return
    if generatorContext.RecipePathBuilder.TargetPath is None:
        raise Exception("Invalid path builder")

    # Claim the 'package' install directory to prevent multiple builds from using the same
    # as it would give concurrency issues
    __ProcessInstallDirClaim(basicConfig, generatorContext.RecipePathBuilder.TargetPath, config.SDKPath, builderSettings.ForceClaimInstallArea)

    if resolvedBuildOrder is None:
        basicConfig.LogPrintVerbose(2, "No recipes to build")
        return

    # Filter all packages that don't have a experimental recipe
    resolvedBuildOrder = [entry for entry in resolvedBuildOrder if not entry.ResolvedDirectExperimentalRecipe is None]

    if len(resolvedBuildOrder) == 0:
        basicConfig.LogPrintVerbose(2, "No recipes to build")
        return


    validationEngine = ValidationEngine(basicConfig, generatorContext.VariableProcessor)
    missingPackagesInBuildOrder = __FindMissingInstallations(basicConfig, validationEngine, resolvedBuildOrder)
    builder = PipelineCommandBuilder(generatorContext, builderSettings.CheckBuildCommands)
    recipeRecords = __CreatePipelines(basicConfig, builder, missingPackagesInBuildOrder)

    for recipeRecord in recipeRecords:
        basicConfig.LogPrint("Package location: {0}".format(recipeRecord.SourcePackage.AbsolutePath))
        try:
            basicConfig.PushIndent()
            if not recipeRecord.Pipeline is None:
                basicConfig.LogPrint("Building package: {0}".format(recipeRecord.SourcePackage.Name))
                if builderSettings.PreDeleteBuild:
                    # We clear the build path to prepare for a new build
                    IOUtil.SafeRemoveDirectoryTree(recipeRecord.Pipeline.BuildPath)

                for command in recipeRecord.Pipeline.CommandList:
                    if not config.IsDryRun:
                        command.Execute()

                # We finished building, so lets save some information about what we did
                __SaveBuildInformation(recipeRecord)

                if builderSettings.PostDeleteBuild:
                    # We clear the build path if a build is successfull
                    IOUtil.SafeRemoveDirectoryTree(recipeRecord.Pipeline.BuildPath, True)

            else:
                # Since we are trying to build this it means that the installation validation failed earlier and
                # we apparently have no pipelines that could remedy it, so force the install validation to occur so
                # we fail early as 'dependent' pipes might fail to build due to this
                basicConfig.DoPrintError("Missing installation of package '{0}' and no recipe for solving it is available".format(recipeRecord.SourcePackage.Name))
                validationEngine.Process(recipeRecord.SourcePackage)
        finally:
            basicConfig.PopIndent()

        validationEngine.Process(recipeRecord.SourcePackage)

    packageCount = len(recipeRecords)
    if packageCount > 0:
        basicConfig.LogPrint("Build {0} packages".format(packageCount))
    else:
        basicConfig.LogPrintVerbose(2, "No recipe was build!")


def __BuildNow(config: Config,
               generatorContext: GeneratorContext,
               builderConfig: BuilderConfig,
               topLevelPackage: Package,
               buildConfig: BuildConfigRecord) -> None:
    BuildPackagesInOrder(config, generatorContext, topLevelPackage.ResolvedBuildOrder, builderConfig.Settings)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
# generator = the generator that was used to build the files
def BuildPackages(config: Config,
                  generatorContext: GeneratorContext,
                  builderConfig: BuilderConfig,
                  packages: List[Package]) -> None:
    PlatformUtil.CheckBuildPlatform(generatorContext.Platform.Name)
    topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)

    buildConfig = BuildConfigRecord(generatorContext.Platform.Name, {}, CommandType.Build, [], None, None, 0)

    basicConfig = generatorContext.BasicConfig
    try:
        basicConfig.LogPrint("- Building recipe packages")
        basicConfig.PushIndent()
        __BuildNow(config, generatorContext, builderConfig, topLevelPackage, buildConfig,)
    finally:
        basicConfig.PopIndent()
