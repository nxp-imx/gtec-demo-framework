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

from typing import Any
from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from typing import Set
import hashlib
from FslBuildGen import IOUtil
from FslBuildGen import PackageListUtil
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.State.BuildInfoFilePackageDependency import BuildInfoFilePackageDependency
from FslBuildGen.BuildExternal.State.JsonDictType import JsonDictType
from FslBuildGen.BuildExternal.State.JsonRecipePackageContentState import JsonRecipePackageContentState
from FslBuildGen.BuildExternal.State.RecipePackageState import RecipePackageState
from FslBuildGen.BuildExternal.State.RecipePackageStateCache import RecipePackageStateCache
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package

class BuildInfoFileElements(object):
    PackageName = "PackageName"
    PackageDependencies = "PackageDependencies"
    FileFormatVersion = "FileFormatVersion"
    RecipeHash = "RecipeHash"
    ContentState = "ContentState"
    ContentStateHash = "ContentStateHash"
    SourceState = "SourceState"
    SourceStateHash = "SourceStateHash"

    CURRENT_VERSION = "2"


class BuildInfoFile(object):
    def __init__(self, jsonDict: JsonDictType) -> None:
        super(BuildInfoFile, self).__init__()
        self.PackageName = jsonDict[BuildInfoFileElements.PackageName]                      # type: str
        self.PackageDependencies = jsonDict[BuildInfoFileElements.PackageDependencies]      # type: List[str]
        self.FileFormatVersion = jsonDict[BuildInfoFileElements.FileFormatVersion]          # type: str
        self.RecipeHash = jsonDict[BuildInfoFileElements.RecipeHash]                        # type: str
        self.ContentState = jsonDict[BuildInfoFileElements.ContentState]                    # type: JsonRecipePackageContentState
        self.ContentStateHash = jsonDict[BuildInfoFileElements.ContentStateHash]            # type: str
        self.DecodedPackageDependencies = [BuildInfoFilePackageDependency(entry) for entry in self.PackageDependencies]
        # Optional entries
        self.SourceState = jsonDict[BuildInfoFileElements.SourceState] if BuildInfoFileElements.SourceState in jsonDict else JsonRecipePackageContentState()    # type: JsonRecipePackageContentState
        self.SourceStateHash = jsonDict[BuildInfoFileElements.SourceStateHash] if BuildInfoFileElements.SourceStateHash in jsonDict else ""                     # type: str

    @staticmethod
    def IsDictValid(srcDict: JsonDictType) -> bool:
        if (not BuildInfoFileElements.PackageName in srcDict or
            not BuildInfoFileElements.PackageDependencies in srcDict or
            not BuildInfoFileElements.FileFormatVersion in srcDict or
            not BuildInfoFileElements.RecipeHash in srcDict or
            not BuildInfoFileElements.ContentState in srcDict or
            not BuildInfoFileElements.ContentStateHash in srcDict):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.PackageName], str):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.PackageDependencies], list):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.FileFormatVersion], str):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.RecipeHash], str):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.ContentState], Dict):
            return False
        if not isinstance(srcDict[BuildInfoFileElements.ContentStateHash], str):
            return False
        if BuildInfoFileElements.SourceState in srcDict and not isinstance(srcDict[BuildInfoFileElements.SourceState], Dict):
            return False
        if BuildInfoFileElements.SourceStateHash in srcDict and not isinstance(srcDict[BuildInfoFileElements.SourceStateHash], str):
            return False
        return cast(bool, srcDict[BuildInfoFileElements.FileFormatVersion] == BuildInfoFileElements.CURRENT_VERSION)


    @staticmethod
    def TryCreateJsonBuildInfoRootDict(log: Log, cacheFilename: str, sourcePackage: Package, sourceRecipe: PackageExperimentalRecipe,
                                       recipePackageStateCache: RecipePackageStateCache,
                                       cachedContentState: Optional[JsonRecipePackageContentState] = None,
                                       cachedSourceState: Optional[JsonRecipePackageContentState] = None) -> Optional[JsonDictType]:
        try:
            if sourcePackage is None or sourceRecipe is None or sourceRecipe.ResolvedInstallPath is None:
                return None

            localSourceState = None
            if sourceRecipe.IsLocalSourceBuild and sourcePackage.AbsolutePath is not None:
                localSourceState = RecipePackageState(log, sourcePackage.Name, sourcePackage.AbsolutePath,
                                                      "fsl-cached-state", sourcePackage.SourceFileHash, cachedSourceState)


            # Generate the package state
            recipePackageState = RecipePackageState(log, sourcePackage.Name, sourceRecipe.ResolvedInstallPath,
                                                    cacheFilename, sourcePackage.SourceFileHash, cachedContentState)
            recipePackageStateCache.Set(recipePackageState)

            referencedPackageSet = PackageListUtil.BuildReferencedPackageSet([sourcePackage])
            referencedPackageSet.remove(sourcePackage)
            referencedPackageNameList = BuildInfoFile.CreateReferencedPackageNameList(referencedPackageSet, recipePackageStateCache)
            referencedPackageNameList.sort()

            recipeHash = sourcePackage.SourceFileHash

            jsonRootDict = {}  # type: JsonDictType
            jsonRootDict[BuildInfoFileElements.PackageName] = sourcePackage.Name
            jsonRootDict[BuildInfoFileElements.PackageDependencies] = referencedPackageNameList
            jsonRootDict[BuildInfoFileElements.FileFormatVersion] = BuildInfoFileElements.CURRENT_VERSION
            jsonRootDict[BuildInfoFileElements.RecipeHash] = recipeHash
            jsonRootDict[BuildInfoFileElements.ContentState] = recipePackageState.ContentState
            jsonRootDict[BuildInfoFileElements.ContentStateHash] = recipePackageState.ContentStateHash
            if localSourceState is not None:
                jsonRootDict[BuildInfoFileElements.SourceState] = localSourceState.ContentState
                jsonRootDict[BuildInfoFileElements.SourceStateHash] = localSourceState.ContentStateHash
            return jsonRootDict
        except Exception as ex:
            log.LogPrintWarning("TryCreateJsonBuildInfoRootDict failed {0}".format(ex))
            return None


    @staticmethod
    def CreateReferencedPackageNameList(referencedPackageSet: Set[Package],
                                        recipePackageStateCache: RecipePackageStateCache) -> List[str]:
        referencedPackageNameList = [] # type: List[str]
        for package in referencedPackageSet:
            cachedState = recipePackageStateCache.TryGet(package.Name)
            dependencyHash = "0" if cachedState is None else cachedState.ContentStateHash
            referencedPackageNameList.append(BuildInfoFilePackageDependency.EncodeDependency(package.Name, dependencyHash))
        referencedPackageNameList.sort()
        return referencedPackageNameList
