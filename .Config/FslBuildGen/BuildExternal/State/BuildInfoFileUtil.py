#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2018 NXP
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

from typing import cast
from typing import Optional
from typing import List
import json
from FslBuildGen import IOUtil
from FslBuildGen.BuildExternal.Pipeline import RecipeRecord
from FslBuildGen.BuildExternal.State.BuildInfoComplexJsonEncoder import BuildInfoComplexJsonEncoder
from FslBuildGen.BuildExternal.State.BuildInfoComplexJsonDecoder import BuildInfoComplexJsonDecoder
from FslBuildGen.BuildExternal.State.BuildInfoFile import BuildInfoFile
from FslBuildGen.BuildExternal.State.BuildInfoFile import BuildInfoFileElements
from FslBuildGen.BuildExternal.State.JsonDictType import JsonDictType
from FslBuildGen.BuildExternal.State.RecipePackageStateCache import RecipePackageStateCache
from FslBuildGen.BuildExternal.State.PackageRecipeUtil import PackageRecipeUtil
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package

class BuildInfoFileHelper(object):
    @staticmethod
    def TryLoadBuildInformation(log: Log, sourcePackage: Package, path: str) -> Optional[JsonDictType]:
        try:
            if not PackageRecipeUtil.HasBuildPipeline(sourcePackage):
                return None

            sourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
            if sourceRecipe is None or sourceRecipe.ResolvedInstallLocation is None:
                raise Exception("Invalid recipe")

            # Generally this should not be called if there is no pipeline

            srcFilePath = IOUtil.Join(sourceRecipe.ResolvedInstallLocation.ResolvedPath, path)

            fileContent = IOUtil.TryReadFile(srcFilePath)
            if fileContent is None:
                log.LogPrint("Package build information for package {0} not found in the expected file '{1}'".format(sourcePackage.Name, srcFilePath))
                return None

            jsonBuildInfoDict = json.loads(fileContent)
            if not BuildInfoFile.IsDictValid(jsonBuildInfoDict):
                log.LogPrint("Package build information for package {0} found in file '{1}' is invalid".format(sourcePackage.Name, srcFilePath))
                return None

            # Decode the complex element to a object of the right type
            jsonBuildInfoDict[BuildInfoFileElements.ContentState] = BuildInfoComplexJsonDecoder.DecodeJson(jsonBuildInfoDict[BuildInfoFileElements.ContentState])
            if BuildInfoFileElements.SourceState in jsonBuildInfoDict:
                jsonBuildInfoDict[BuildInfoFileElements.SourceState] = BuildInfoComplexJsonDecoder.DecodeJson(jsonBuildInfoDict[BuildInfoFileElements.SourceState])

            return cast(JsonDictType, jsonBuildInfoDict)
        except Exception as ex:
            log.LogPrintWarning("TryLoadBuildInformation failed for package '{0}' with {1}".format(sourcePackage.Name, ex))
            return None


class BuildInfoFileUtil(object):
    @staticmethod
    def TryValidateBuildInformation(log: Log, sourcePackage: Package, packagesToBuild: List[Package],
                                    recipePackageStateCache: RecipePackageStateCache,
                                    path: str) -> bool:
        if not PackageRecipeUtil.HasBuildPipeline(sourcePackage):
            return False

        loadedBuildInfoDict = BuildInfoFileHelper.TryLoadBuildInformation(log, sourcePackage, path)
        if loadedBuildInfoDict is None:
            return False

        if sourcePackage.ResolvedDirectExperimentalRecipe is None:
            raise Exception("Invalid package")

        # Loaded information
        loadedInfo = BuildInfoFile(loadedBuildInfoDict)
        cachedContentState = loadedInfo.ContentState

        # Load current information
        currentInfoDict = BuildInfoFile.TryCreateJsonBuildInfoRootDict(log, path, sourcePackage,
                                                                       sourcePackage.ResolvedDirectExperimentalRecipe,
                                                                       recipePackageStateCache,
                                                                       cachedContentState,
                                                                       loadedInfo.SourceState)
        if currentInfoDict is None:
            log.LogPrint("Failed to create Package build information for package {0}".format(sourcePackage.Name))
            return False

        currentInfo = BuildInfoFile(currentInfoDict)

        if loadedInfo.PackageName != currentInfo.PackageName:
            log.LogPrint("The current package name {0} did not match the stored package {1}".format(currentInfo.PackageName, loadedInfo.PackageName))
            return False

        if loadedInfo.RecipeHash != currentInfo.RecipeHash:
            log.LogPrint("The current package recipe hash {0} did not match the stored package hash {1}".format(currentInfo.RecipeHash, loadedInfo.RecipeHash))
            return False

        if len(currentInfo.PackageDependencies) != len(loadedInfo.PackageDependencies):
            log.LogPrint("The current package dependencies {0} did not match the stored package dependencies {1}".format(currentInfo.PackageDependencies, loadedInfo.PackageDependencies))
            return False

        if currentInfo.ContentStateHash != loadedInfo.ContentStateHash:
            log.LogPrint("The current package contentStateHash {0} did not match the stored package contentStateHash {1}".format(currentInfo.ContentStateHash, loadedInfo.ContentStateHash))
            return False

        # As long as we trust the hash is unique this check wont be necessary
        #if not BuildInfoFile.Compare(loadedInfo.ContentState, currentInfo.ContentState):
        #    log.LogPrint("The current package build content {0} did not match the stored package content {1}".format(currentInfo.ContentState, loadedInfo.ContentState))
        #    return False

        if currentInfo.SourceStateHash != loadedInfo.SourceStateHash:
            log.LogPrint("The current package sourceStateHash {0} did not match the stored package sourceStateHash {1}".format(currentInfo.SourceStateHash, loadedInfo.SourceStateHash))
            return False

        loadedDependencyDict = {dep.Name: dep for dep in loadedInfo.DecodedPackageDependencies}

        rebuildPackages = set(package.Name for package in packagesToBuild)
        for dependency in currentInfo.DecodedPackageDependencies:
            # Future improvement:
            # If the user just deleted a package the 'newly build package' might not actually have changed
            # that can be checked using the ContentStateHash,
            # however the problem is that we would not have that available before we have build the package
            # meaning the validation check would have to occur while building instead of as a prebuild step
            if dependency.Name in rebuildPackages:
                log.LogPrint("The dependency {0} is being rebuild so we also rebuild {1}".format(dependency.Name, loadedInfo.PackageName))
                return False
            if not dependency.Name in loadedDependencyDict:
                log.LogPrint("The dependency {0} did not exist in the stored package {1}".format(dependency.Name, loadedInfo.PackageName))
                return False
            loadedDep = loadedDependencyDict[dependency.Name]
            currentState = recipePackageStateCache.TryGet(dependency.Name)
            if currentState is not None and loadedDep.Revision != currentState.ContentStateHash:
                log.LogPrint("The dependency {0} content revision has changed from {1} to {2}, rebuilding".format(dependency.Name, currentState.ContentStateHash, loadedDep.Revision))
                return False
        return True

    @staticmethod
    def SaveBuildInformation(log: Log, recipeRecord: Optional[RecipeRecord], recipePackageStateCache: RecipePackageStateCache, path: str) -> None:
        if recipeRecord is None or not PackageRecipeUtil.HasBuildPipeline(recipeRecord.SourcePackage):
            return
        if recipeRecord.SourceRecipe is None or recipeRecord.SourceRecipe.ResolvedInstallLocation is None:
            return

        installPath = recipeRecord.SourceRecipe.ResolvedInstallLocation.ResolvedPath

        jsonRootDict = BuildInfoFile.TryCreateJsonBuildInfoRootDict(log, path, recipeRecord.SourcePackage, recipeRecord.SourceRecipe, recipePackageStateCache)
        if jsonRootDict is None:
            return

        jsonText = json.dumps(jsonRootDict, ensure_ascii=False, sort_keys=True, indent=2, cls=BuildInfoComplexJsonEncoder)

        dstFilePath = IOUtil.Join(installPath, path)
        IOUtil.WriteFileIfChanged(dstFilePath, jsonText)


