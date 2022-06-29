#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2022 NXP
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
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple
from concurrent.futures import ThreadPoolExecutor
import io
#import os
import os.path
import queue
import subprocess
#import sys
import threading
from FslBuildGen import IOUtil
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.BuildConfig.BuildUtil import BuildUtil
from FslBuildGen.BuildConfig.CaptureLog import CaptureLog
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildConfig.DotnetFormatConfiguration import DotnetFormatConfiguration
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.ninja_syntax import Writer
from FslBuildGen.BuildConfig.PackagePathUtil import PackagePathUtil
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.BuildConfig.RunHelper import RunHelper
from FslBuildGen.BuildConfig.SimpleCancellationToken import SimpleCancellationToken
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
#from FslBuildGen.DataTypes import CheckType
#from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import AggregateException
from FslBuildGen.Exceptions import ExitException
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
#from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
#from FslBuildGen.ToolConfigProjectInfo import ToolConfigProjectInfo
#from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory


class MagicValues(object):
    DotnetCommand = "dotnet"
    NinjaCommand = "ninja"

class FormatPackageConfig(object):
    def __init__(self, log: Log, package: Package, dotnetFormatConfiguration: DotnetFormatConfiguration, filteredFiles: Optional[List[str]]) -> None:
        allFiles = [] # type: List[ResolvedPath]

        if package.ResolvedBuildAllIncludeFiles is not None and package.AllowCheck and not package.IsVirtual:
            if package.AbsolutePath is None or package.ResolvedBuildSourceFiles is None:
                raise Exception("Invalid package")

            if filteredFiles is None:
                for fileName in package.ResolvedBuildAllIncludeFiles:
                    fullPath = IOUtil.Join(package.AbsolutePath, fileName)
                    # Only process files with the expected extension
                    if PerformClangUtil.IsValidExtension(fileName, dotnetFormatConfiguration.FileExtensions):
                        allFiles.append(ResolvedPath(fileName, fullPath))

                for fileName in package.ResolvedBuildSourceFiles:
                    fullPath = IOUtil.Join(package.AbsolutePath, fileName)
                    if PerformClangUtil.IsValidExtension(fileName, dotnetFormatConfiguration.FileExtensions):
                        allFiles.append(ResolvedPath(fileName, fullPath))

                if package.ResolvedContentFiles is not None:
                    for resolvedPath in package.ResolvedContentFiles:
                        if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, dotnetFormatConfiguration.FileExtensions):
                            allFiles.append(self.__GetResolvedPath(package, resolvedPath))

                if package.ResolvedContentBuilderSyncInputFiles is not None:
                    for resolvedPath in package.ResolvedContentBuilderSyncInputFiles:
                        if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, dotnetFormatConfiguration.FileExtensions):
                            allFiles.append(self.__GetResolvedPath(package, resolvedPath))

                if package.ResolvedContentBuilderBuildInputFiles is not None:
                    for resolvedPath in package.ResolvedContentBuilderBuildInputFiles:
                        if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, dotnetFormatConfiguration.FileExtensions):
                            allFiles.append(self.__GetResolvedPath(package, resolvedPath))
            else:
                for fileEntry in filteredFiles:
                    allFiles.append(self.__GetResolvedPathFromAbsPath(package, fileEntry))
        self.AllFiles = allFiles

    def __GetResolvedPath(self, package: Package, path: PathRecord) -> ResolvedPath:
        if package.Path is None:
            raise Exception("invalid package")
        absFilePath = path.ResolvedPath
        packagePath = package.Path.AbsoluteDirPath + '/'
        if not absFilePath.startswith(packagePath):
            raise Exception("file '{0}' is not part of the package at path '{1}'".format(absFilePath, packagePath))
        relativePath = absFilePath[len(packagePath):]
        return ResolvedPath(relativePath, absFilePath)

    def __GetResolvedPathFromAbsPath(self, package: Package, absFilePath: str) -> ResolvedPath:
        if package.Path is None:
            raise Exception("invalid package")
        packagePath = package.Path.AbsoluteDirPath + '/'
        if not absFilePath.startswith(packagePath):
            raise Exception("file '{0}' is not part of the package at path '{1}'".format(absFilePath, packagePath))
        relativePath = absFilePath[len(packagePath):]
        return ResolvedPath(relativePath, absFilePath)

def _RunClangFormat(log: Log, toolConfig: ToolConfig, dotnetFormatConfiguration: DotnetFormatConfiguration,
                    clangExeInfo: ClangExeInfo,
                    package: Package, filteredFiles: Optional[List[str]],
                    repairEnabled: bool) -> None:
    if package.ResolvedBuildAllIncludeFiles is None or len(package.ResolvedBuildAllIncludeFiles) <= 0 or not package.AllowCheck or package.IsVirtual:
        return

    if package.AbsolutePath is None or package.ResolvedBuildSourceFiles is None:
        raise Exception("Invalid package")

    formatPackageConfig = FormatPackageConfig(log, package, dotnetFormatConfiguration, filteredFiles)

    cmd = clangExeInfo.Command
    buildCommand = [cmd]
    if not repairEnabled:
        buildCommand.append('--verify-no-changes')
    if len(dotnetFormatConfiguration.AdditionalUserArguments) > 0:
        log.LogPrint("Adding user supplied arguments before '--' {0}".format(dotnetFormatConfiguration.AdditionalUserArguments))
        buildCommand += dotnetFormatConfiguration.AdditionalUserArguments

    buildCommand += [entry.ResolvedPath for entry in formatPackageConfig.AllFiles]

    currentWorkingDirectory = package.AbsolutePath
    FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, dotnetFormatConfiguration.CustomFormatFile)

    try:
        # if verbose enabled we log the dotnet format version
        if log.Verbosity >= 4:
            log.LogPrint("Running command '{0}' in cwd: {1}".format(buildCommand, currentWorkingDirectory))

        result = subprocess.call(buildCommand, cwd=currentWorkingDirectory)
        if result != 0:
            log.LogPrintWarning("The command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
            raise ExitException(result)
    except FileNotFoundError:
        log.DoPrintWarning("The command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(buildCommand), currentWorkingDirectory))
        raise

def GetFilteredFiles(log: Log, customPackageFileFilter: Optional[CustomPackageFileFilter],
                     dotnetFormatConfiguration: DotnetFormatConfiguration, package: Package) -> Optional[List[str]]:
    if customPackageFileFilter is None:
        return None
    return customPackageFileFilter.TryLocateFilePatternInPackage(log, package, dotnetFormatConfiguration.FileExtensions)



class PerformFormatHelper(object):
    RULE_FORMAT = "format"

    @staticmethod
    def Process(log: Log, toolConfig: ToolConfig, customPackageFileFilter: Optional[CustomPackageFileFilter],
                dotnetFormatConfiguration: DotnetFormatConfiguration, cmakeConfig: GeneratorCMakeConfig,
                clangFormatExeInfo: ClangExeInfo, ninjaExeInfo: ClangExeInfo, sortedPackageList: List[Package],
                repairEnabled: bool, numBuildThreads: int) -> int:
        totalProcessedCount = 0

        logOutput = False
        currentWorkingDirectory = BuildUtil.GetBuildDir(toolConfig.ProjectInfo, cmakeConfig.CheckDir)
        currentWorkingDirectory = IOUtil.Join(currentWorkingDirectory, "netFormat")
        ninjaOutputFile = IOUtil.Join(currentWorkingDirectory, "build.ninja")
        toolVersionOutputFile = IOUtil.Join(currentWorkingDirectory, "ToolVersions.txt")
        IOUtil.SafeMakeDirs(currentWorkingDirectory)

        log.LogPrint("Using path: '{0}'".format(currentWorkingDirectory))

        log.LogPrint("Storing tool versions.")
        PerformFormatHelper.WriteToolVersionFile(log, toolVersionOutputFile, clangFormatExeInfo, ninjaExeInfo)

        log.LogPrint("Generating ninja format file.")

        totalProcessedCount = PerformFormatHelper.GenerateNinjaFormatFile(log, toolConfig, ninjaOutputFile, currentWorkingDirectory,
                                                                          toolVersionOutputFile, clangFormatExeInfo, customPackageFileFilter,
                                                                          dotnetFormatConfiguration, sortedPackageList, repairEnabled)
        log.LogPrint("Executing ninja format file.")

        RunHelper.RunNinja(log, ninjaExeInfo, ninjaOutputFile, currentWorkingDirectory, numBuildThreads, logOutput)

        return totalProcessedCount

    @staticmethod
    def WriteToolVersionFile(log: Log, outputFile: str, clangFormatExeInfo: ClangExeInfo, ninjaExeInfo: ClangExeInfo) -> None:
        content = "Tool versions\n"
        content += "ClangFormat: {0}\n".format(clangFormatExeInfo.Version)
        content += "Ninja: {0}\n".format(ninjaExeInfo.Version)
        IOUtil.WriteFileIfChanged(outputFile, content)

    @staticmethod
    def GenerateNinjaFormatFile(log: Log, toolConfig: ToolConfig, ninjaOutputFile: str, outputFolder: str, toolVersionOutputFile: str,
                                clangFormatExeInfo: ClangExeInfo, customPackageFileFilter: Optional[CustomPackageFileFilter],
                                dotnetFormatConfiguration: DotnetFormatConfiguration, sortedPackageList: List[Package], repairEnabled: bool) -> int:
        totalProcessedCount = 0
        toolProjectContextsDict = PackagePathUtil.CreateToolProjectContextsDict(toolConfig.ProjectInfo)
        with io.StringIO() as ninjaFile:
            ninjaFile.write("ninja_required_version = 1.8\n")

            writer = Writer(ninjaFile, 149)

            strVerbosity = ""
            if log.Verbosity >= 2:
                strVerbosity = " -v detailed"
            elif log.Verbosity >= 1:
                strVerbosity = " -v diagnostic"
            #elif log.Verbosity >= 1:
            #    strVerbosity = " -v minimal"

            formatCommand = "{0} format{1} whitespace $in".format(clangFormatExeInfo.Command, strVerbosity)
            if not repairEnabled:
                formatCommand += " --verify-no-changes"
            if len(dotnetFormatConfiguration.AdditionalUserArguments) > 0:
                log.LogPrint("Adding user supplied arguments before '--' {0}".format(dotnetFormatConfiguration.AdditionalUserArguments))
                formatCommand += " {0}".format(" ".join(dotnetFormatConfiguration.AdditionalUserArguments))

            writer.rule(name=PerformFormatHelper.RULE_FORMAT,
                        command=formatCommand,
                        restat=True)

            for package in sortedPackageList:
                PerformFormatHelper.AddPackage(log, toolConfig, toolVersionOutputFile, writer, package, outputFolder, toolProjectContextsDict,
                                                     customPackageFileFilter, dotnetFormatConfiguration)
                totalProcessedCount += 1

            # finally we write the ninja file
            IOUtil.WriteFileIfChanged(ninjaOutputFile, ninjaFile.getvalue())
            writer.close()
        return totalProcessedCount

    @staticmethod
    def AddPackage(log: Log, toolConfig: ToolConfig, toolVersionOutputFile: str, writer: Writer, package: Package, outputFolder: str,
                   toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext], customPackageFileFilter: Optional[CustomPackageFileFilter],
                   dotnetFormatConfiguration: DotnetFormatConfiguration) -> None:

        log.LogPrintVerbose(1, "Formatting package '{0}'".format(package.Name))

        outputFolder = IOUtil.Join(outputFolder, PackagePathUtil.GetPackagePath(package, toolProjectContextsDict))

        filteredFiles = GetFilteredFiles(log, customPackageFileFilter, dotnetFormatConfiguration, package)
        formatPackageConfig = FormatPackageConfig(log, package, dotnetFormatConfiguration, filteredFiles)

        packageFormatFileSet = set()
        outputFiles = []
        implicitDeps = []
        for fileEntry in formatPackageConfig.AllFiles:
            outputFile = "{0}.dmy".format(IOUtil.Join(outputFolder, fileEntry.SourcePath))

            # Locate the closest clang format configuration file so we can add it as a implicit package dependency
            packageClosestFormatPath = FileFinder.FindClosestFileInRoot(log, toolConfig, fileEntry.ResolvedPath, dotnetFormatConfiguration.CustomFormatFile)
            packageClosestFormatFile = IOUtil.Join(packageClosestFormatPath, dotnetFormatConfiguration.CustomFormatFile)

            packageFormatFileSet.add(packageClosestFormatFile)

            outputFiles.append(outputFile)
            implicitDeps.append(fileEntry.ResolvedPath)

            # Write a dummy file so ninja finds a file (this is because dotnet format overwrites the existing file, so we need a dummy file to track progress)
            if not IOUtil.Exists(outputFile):
                parentDir = IOUtil.GetDirectoryName(outputFile)
                if not IOUtil.Exists(parentDir):
                    IOUtil.SafeMakeDirs(parentDir)
                IOUtil.WriteFileIfChanged(outputFile, "")

        if len(packageFormatFileSet) > 1:
            raise Exception("only support one editorconfig per package, multiple found in: {0}. Found: {1}".format(package.Name, packageFormatFileSet))
        elif len(packageFormatFileSet) < 1:
            if len(formatPackageConfig.AllFiles) > 0:
                log.DoPrintWarning("No editorconfig found in package '{0}' so was skipped.".format(package.Name))
            else:
                log.LogPrintVerbose(4, "No source files found in package '{0}' so was skipped.".format(package.Name))

        if len(implicitDeps) > 0 and len(formatPackageConfig.AllFiles) > 1:
            if package.AbsolutePath is None:
                raise Exception("Invalid package")
            implicitDeps.append(toolVersionOutputFile)
            for entry in packageFormatFileSet:
                implicitDeps.append(entry)

            dstName = package.Name
            # FIX: hardcoded extension
            dstFileVC = IOUtil.Join(package.AbsolutePath, dstName + ".{0}".format("csproj"))
            dstFileVC = os.path.normpath(dstFileVC)

            writer.build(outputs=outputFiles, rule=PerformFormatHelper.RULE_FORMAT, implicit=implicitDeps, inputs=dstFileVC)


class PerformDotnetFormat(object):
    @staticmethod
    def Run(log: Log, toolConfig: ToolConfig, formatPackageList: List[Package],
            customPackageFileFilter: Optional[CustomPackageFileFilter],
            packageRecipeResultManager: PackageRecipeResultManager,
            dotnetFormatConfiguration: DotnetFormatConfiguration, cmakeConfig: GeneratorCMakeConfig,
            repairEnabled: bool, buildThreads: int) -> None:

        # Lookup the recommended build threads using the standard build algorithm
        numBuildThreads = PlatformBuildUtil.GetRecommendedBuildThreads(buildThreads)

        formatExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, dotnetFormatConfiguration.RecipePackageName,
                                                            MagicValues.DotnetCommand)
        ninjaExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, dotnetFormatConfiguration.NinjaRecipePackageName,
                                                            MagicValues.NinjaCommand)

        log.LogPrint("Dotnet version: {0}".format(formatExeInfo.Version))
        log.LogPrint("Ninja version: {0}".format(ninjaExeInfo.Version))

        log.LogPrint("Running dotnet format")

        # Filter the package list so it only contains things we can process
        finalPackageList = [package for package in formatPackageList if PerformClangUtil.CanProcessPackage(package)]

        sortedPackages = list(finalPackageList)
        sortedPackages.sort(key=lambda s: s.Name.lower())

        count = PerformFormatHelper.Process(log, toolConfig, customPackageFileFilter, dotnetFormatConfiguration, cmakeConfig, formatExeInfo,
                                            ninjaExeInfo, sortedPackages, repairEnabled, numBuildThreads)

        if count == 0:
            if customPackageFileFilter is None:
                log.DoPrintWarning("No files processed")
            else:
                log.DoPrintWarning("No files processed, could not find a package that matches {0}".format(customPackageFileFilter))
