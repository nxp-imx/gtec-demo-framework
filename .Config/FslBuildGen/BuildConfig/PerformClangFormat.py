#!/usr/bin/env python3

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

from typing import List
from typing import Optional
import os
import os.path
import subprocess
import sys
from FslBuildGen import IOUtil
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildConfig.ClangFormatConfiguration import ClangFormatConfiguration
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory

class MagicValues(object):
    ClangFormatCommand = "clang-format"

def _RunClangFormat(log: Log, toolConfig: ToolConfig, clangFormatConfiguration: ClangFormatConfiguration,
                    clangExeInfo: ClangExeInfo,
                    package: Package, filteredFiles: Optional[List[str]],
                    repairEnabled: bool) -> None:
    if not package.ResolvedBuildAllIncludeFiles or not package.AllowCheck or package.IsVirtual:
        return
    log.LogPrint("- {0}".format(package.Name))

    if package.AbsolutePath is None or package.ResolvedBuildSourceFiles is None:
        raise Exception("Invalid package")

    allFiles = [] # type: List[str]

    if filteredFiles is None:
        for fileName in package.ResolvedBuildAllIncludeFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            # Only process files with the expected extension
            if PerformClangUtil.IsValidExtension(fileName, clangFormatConfiguration.FileExtensions):
                allFiles.append(fileName)

        for fileName in package.ResolvedBuildSourceFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            if PerformClangUtil.IsValidExtension(fileName, clangFormatConfiguration.FileExtensions):
                allFiles.append(fileName)
    else:
        allFiles += filteredFiles

    cmd = clangExeInfo.ClangCommand
    buildCommand = [cmd, '-style=file']
    if repairEnabled:
        buildCommand.append('-i')
    if len(clangFormatConfiguration.AdditionalUserArguments) > 0:
        log.LogPrint("Adding user supplied arguments before '--' {0}".format(clangFormatConfiguration.AdditionalUserArguments))
        buildCommand += clangFormatConfiguration.AdditionalUserArguments

    buildCommand += allFiles

    currentWorkingDirectory = package.AbsolutePath
    FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, clangFormatConfiguration.CustomFormatFile)

    try:
        # if verbose enabled we log the clang-format version
        if log.Verbosity >= 4:
            log.LogPrint("Running command '{0}' in cwd: {1}".format(buildCommand, currentWorkingDirectory))

        result = subprocess.call(buildCommand, cwd=currentWorkingDirectory)
        if result != 0:
            log.LogPrintWarning("The command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
            sys.exit(result)
    except FileNotFoundError:
            log.DoPrintWarning("The command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(buildCommand), currentWorkingDirectory))
            raise



class PerformClangFormat(object):
    @staticmethod
    def Run(log: Log, toolConfig: ToolConfig, formatPackageList: List[Package],
            customPackageFileFilter: Optional[CustomPackageFileFilter],
            packageRecipeResultManager: PackageRecipeResultManager,
            clangFormatConfiguration: ClangFormatConfiguration, repairEnabled: bool) -> None:
        """ RunClangFormat on a package at a time """

        clangExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, clangFormatConfiguration.RecipePackageName,
                                                            MagicValues.ClangFormatCommand)

        if log.Verbosity >= 1:
            PerformClangUtil.ShowVersion(log, clangExeInfo)

        log.LogPrint("Running clang-format")

        # Filter the package list so it only contains things we can process
        finalPackageList = [package for package in formatPackageList if PerformClangUtil.CanProcessPackage(package)]

        sortedPackages = list(finalPackageList)
        sortedPackages.sort(key=lambda s: s.Name.lower())
        for package in sortedPackages:
            filteredFiles = None if customPackageFileFilter is None else customPackageFileFilter.TryLocateFilePatternInPackage(log, package,
                                                                                                                               clangFormatConfiguration.FileExtensions)
            if customPackageFileFilter is None or filteredFiles is not None:
                _RunClangFormat(log, toolConfig, clangFormatConfiguration, clangExeInfo, package, filteredFiles, repairEnabled)
