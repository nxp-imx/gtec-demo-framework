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

from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.Exceptions import UsageErrorException
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.RecipeBuilderSetup import RecipeBuilderSetup
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe


class RecipePathBuilder(object):
    def __init__(self, log: Log, variableProcessor: VariableProcessor, recipeBuilderSetup: Optional[RecipeBuilderSetup], platformName: str,
                 cmakeConfig: GeneratorCMakeConfig) -> None:
        super().__init__()

        self.__Log = log  # type: Log
        self.__VariableProcessor = variableProcessor  # type: VariableProcessor

        self.IsEnabled = recipeBuilderSetup is not None  # type: bool

        self.TargetLocation = None  # Optional[ResolvedPath]
        self.DownloadCacheRootPath = None  # Optional[str]
        self.__TempRootPath = None  # Optional[str]
        self.__TempPipelineRootPath = None  # Optional[str]
        self.InstallRootLocation = None  # Optional[ResolvedPath]
        self.ReadonlyCache_DownloadCacheRootPath = None  # Optional[str]

        if self.IsEnabled and recipeBuilderSetup is not None:
            targetLocation = recipeBuilderSetup.TargetLocation
            readonlyCachePath = recipeBuilderSetup.ReadonlyCachePath

            if not IOUtil.IsAbsolutePath(targetLocation.ResolvedPath):
                raise Exception("Install area path is not absolute: '{0}'".format(targetLocation.ResolvedPath))
            if not readonlyCachePath is None and not IOUtil.IsAbsolutePath(readonlyCachePath):
                raise Exception("Install area readonly cache path is not absolute: '{0}'".format(readonlyCachePath))

            self.TargetLocation = targetLocation
            self.DownloadCacheRootPath = IOUtil.Join(targetLocation.ResolvedPath, ".DownloadCache")

            self.__TempRootPath = IOUtil.Join(targetLocation.ResolvedPath, ".Temp")

            baseTempDirectory = IOUtil.Join(self.__TempRootPath, "pipeline")
            baseTempDirectory = IOUtil.Join(baseTempDirectory, platformName)
            self.__TempPipelineRootPath = IOUtil.Join(baseTempDirectory, cmakeConfig.GeneratorRecipeShortName)

            sourceBaseInstallDirectory = IOUtil.Join(targetLocation.SourcePath, platformName)
            sourceInstallRootPath = IOUtil.Join(sourceBaseInstallDirectory, cmakeConfig.GeneratorRecipeShortName)

            baseInstallDirectory = IOUtil.Join(targetLocation.ResolvedPath, platformName)
            installRootPath = IOUtil.Join(baseInstallDirectory, cmakeConfig.GeneratorRecipeShortName)

            self.InstallRootLocation = ResolvedPath(sourceInstallRootPath, installRootPath)

            self.ReadonlyCache_DownloadCacheRootPath = None if readonlyCachePath is None else IOUtil.Join(readonlyCachePath, ".DownloadCache")


    def GetBuildPath(self, sourceRecipe: PackageExperimentalRecipe) -> str:
        if not self.IsEnabled or self.__TempPipelineRootPath is None:
            raise Exception("Can not GetBuildPath since the builder functionality has been disabled")
        return IOUtil.Join(self.__TempPipelineRootPath, sourceRecipe.FullName)


    def TryGetInstallPath(self, xmlSourceRecipe: XmlExperimentalRecipe) -> Optional[ResolvedPath]:
        if xmlSourceRecipe is None:
            return None
        elif not xmlSourceRecipe.ExternalInstallDirectory is None:
            if not xmlSourceRecipe.Pipeline is None:
                self.__Log.DoPrintWarning("SourceRecipe ExternalInstallDirectory overrides Pipeline '{0}'".format(xmlSourceRecipe.FullName))
            sourcePath = xmlSourceRecipe.ExternalInstallDirectory
            resolvedPath = self.__VariableProcessor.ResolveAbsolutePathWithLeadingEnvironmentVariablePathAsDir(sourcePath)
            return ResolvedPath(sourcePath, resolvedPath)

        if not self.IsEnabled or self.InstallRootLocation is None:
            raise Exception("Can not TryGetInstallPath since the builder functionality has been disabled, please enable the builder functionality for this project")

        if xmlSourceRecipe.Pipeline is None:
            return None
        sourcePath = IOUtil.Join(self.InstallRootLocation.SourcePath, xmlSourceRecipe.FullName)
        resolvedPath = IOUtil.Join(self.InstallRootLocation.ResolvedPath, xmlSourceRecipe.FullName)
        return ResolvedPath(sourcePath, resolvedPath)
