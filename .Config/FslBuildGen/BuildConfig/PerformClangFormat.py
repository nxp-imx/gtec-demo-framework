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

from typing import Any
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple
from concurrent.futures import ThreadPoolExecutor
import io
import os
import os.path
import queue
import subprocess
import sys
import threading
from FslBuildGen import IOUtil
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.BuildConfig.BuildUtil import BuildUtil
from FslBuildGen.BuildConfig.CaptureLog import CaptureLog
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildConfig.ClangFormatConfiguration import ClangFormatConfiguration
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.ninja_syntax import Writer
from FslBuildGen.BuildConfig.PackagePathUtil import PackagePathUtil
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.BuildConfig.RunHelper import RunHelper
from FslBuildGen.BuildConfig.SimpleCancellationToken import SimpleCancellationToken
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import AggregateException
from FslBuildGen.Exceptions import ExitException
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
from FslBuildGen.ToolConfigProjectInfo import ToolConfigProjectInfo
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory

class MagicValues(object):
    ClangFormatCommand = "clang-format"
    NinjaCommand = "ninja"

class FormatPackageConfig(object):
    def __init__(self, log: Log, package: Package, clangFormatConfiguration: ClangFormatConfiguration, filteredFiles: Optional[List[str]]) -> None:
        allFiles = [] # type: List[ResolvedPath]

        if package.ResolvedBuildAllIncludeFiles is not None and package.AllowCheck and not package.IsVirtual:
            if package.AbsolutePath is None or package.ResolvedBuildSourceFiles is None:
                raise Exception("Invalid package")

            if filteredFiles is None:
                for fileName in package.ResolvedBuildAllIncludeFiles:
                    fullPath = IOUtil.Join(package.AbsolutePath, fileName)
                    # Only process files with the expected extension
                    if PerformClangUtil.IsValidExtension(fileName, clangFormatConfiguration.FileExtensions):
                        allFiles.append(ResolvedPath(fileName, fullPath))

                for fileName in package.ResolvedBuildSourceFiles:
                    fullPath = IOUtil.Join(package.AbsolutePath, fileName)
                    if PerformClangUtil.IsValidExtension(fileName, clangFormatConfiguration.FileExtensions):
                        allFiles.append(ResolvedPath(fileName, fullPath))

                if package.ResolvedContentFiles is not None:
                    for resolvedPath in package.ResolvedContentFiles:
                        if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, clangFormatConfiguration.FileExtensions):
                            allFiles.append(self.__GetResolvedPath(package, resolvedPath))

                if package.ResolvedContentBuilderSyncInputFiles is not None:
                    for resolvedPath in package.ResolvedContentBuilderSyncInputFiles:
                        if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, clangFormatConfiguration.FileExtensions):
                            allFiles.append(self.__GetResolvedPath(package, resolvedPath))

                if package.ResolvedContentBuilderBuildInputFiles is not None:
                    for resolvedPath in package.ResolvedContentBuilderBuildInputFiles:
                        if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, clangFormatConfiguration.FileExtensions):
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

def _RunClangFormat(log: Log, toolConfig: ToolConfig, clangFormatConfiguration: ClangFormatConfiguration,
                    clangExeInfo: ClangExeInfo,
                    package: Package, filteredFiles: Optional[List[str]],
                    repairEnabled: bool) -> None:
    if package.ResolvedBuildAllIncludeFiles is None or len(package.ResolvedBuildAllIncludeFiles) <= 0 or not package.AllowCheck or package.IsVirtual:
        return

    if package.AbsolutePath is None or package.ResolvedBuildSourceFiles is None:
        raise Exception("Invalid package")

    formatPackageConfig = FormatPackageConfig(log, package, clangFormatConfiguration, filteredFiles)

    cmd = clangExeInfo.Command
    buildCommand = [cmd, '-style=file']
    if repairEnabled:
        buildCommand.append('-i')
    if len(clangFormatConfiguration.AdditionalUserArguments) > 0:
        log.LogPrint("Adding user supplied arguments before '--' {0}".format(clangFormatConfiguration.AdditionalUserArguments))
        buildCommand += clangFormatConfiguration.AdditionalUserArguments

    buildCommand += [entry.ResolvedPath for entry in formatPackageConfig.AllFiles]

    currentWorkingDirectory = package.AbsolutePath
    FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, clangFormatConfiguration.CustomFormatFile)

    try:
        # if verbose enabled we log the clang-format version
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
                     clangFormatConfiguration: ClangFormatConfiguration, package: Package) -> Optional[List[str]]:
    if customPackageFileFilter is None:
        return None
    return customPackageFileFilter.TryLocateFilePatternInPackage(log, package, clangFormatConfiguration.FileExtensions)

class PerformClangFormatHelper(object):
    @staticmethod
    def Process(log: Log, toolConfig: ToolConfig, customPackageFileFilter: Optional[CustomPackageFileFilter],
                clangFormatConfiguration: ClangFormatConfiguration, clangExeInfo: ClangExeInfo, packageList: List[Package],
                repairEnabled: bool, numBuildThreads: int) -> int:
        totalProcessedCount = 0
        if numBuildThreads <= 1 or len(packageList) <= 1:
            for package in packageList:
                filteredFiles = GetFilteredFiles(log, customPackageFileFilter, clangFormatConfiguration, package)
                if customPackageFileFilter is None or filteredFiles is not None:
                    totalProcessedCount += 1
                    log.LogPrint("- {0}".format(package.Name))
                    _RunClangFormat(log, toolConfig, clangFormatConfiguration, clangExeInfo, package, filteredFiles, repairEnabled)
        else:
            # Do some threading magic. We can do this because each package touches separate files
            packageQueue = queue.Queue(len(packageList))  # type: Any
            for package in packageList:
                packageQueue.put(package)

            # Ensure we dont start threads we dont need
            finalBuildThreads = numBuildThreads if len(packageList) >= numBuildThreads else len(packageList)
            exceptionList = []
            cancellationToken = SimpleCancellationToken()
            totalExaminedCount = 0
            try:
                with ThreadPoolExecutor(max_workers=finalBuildThreads) as executor:

                    futures = []
                    for index in range(0, finalBuildThreads):
                        taskFuture = executor.submit(PerformClangFormatHelper.RunInAnotherThread,
                                                     packageQueue, cancellationToken,
                                                     log, toolConfig, customPackageFileFilter,
                                                     clangFormatConfiguration, clangExeInfo, repairEnabled)
                        futures.append(taskFuture)
                    # Wait for all futures to finish
                    for future in futures:
                        try:
                            examinedCount, processedCount = future.result()
                            totalExaminedCount += examinedCount
                            totalProcessedCount += processedCount
                        except Exception as ex:
                            cancellationToken.Cancel()
                            exceptionList.append(ex)
            finally:
                cancellationToken.Cancel()


            if len(exceptionList) > 0:
                raise AggregateException(exceptionList)

            if totalExaminedCount != len(packageList):
                raise Exception("internal error: we did not process the expected amount of packages Expected: {0} processed {1}".format(len(packageList), totalExaminedCount))
        return totalProcessedCount

    @staticmethod
    def RunInAnotherThread(packageQueue: Any,
                           cancellationToken: SimpleCancellationToken,
                           mainLog: Log, toolConfig: ToolConfig, customPackageFileFilter: Optional[CustomPackageFileFilter],
                           clangFormatConfiguration: ClangFormatConfiguration, clangExeInfo: ClangExeInfo, repairEnabled: bool) -> Tuple[int,int]:
        threadId = threading.get_ident()
        mainLog.LogPrintVerbose(4, "Starting thread {0}".format(threadId))
        examinedCount = 0
        processedCount = 0
        keepWorking = True
        package = None # type: Optional[Package]

        try:
            while keepWorking and not cancellationToken.IsCancelled():
                try:
                    # Since the queue is preloaded this is ok
                    package = packageQueue.get_nowait()
                except:
                    package = None
                if package is None:
                    keepWorking = False
                else:
                    if mainLog.Verbosity >= 4:
                        mainLog.LogPrint("- clang-format on package '{0}' on thread {1}".format(package.Name, threadId))
                    else:
                        mainLog.LogPrint("- clang-format on package '{0}'".format(package.Name))

                    captureLog = CaptureLog(mainLog.Title, mainLog.Verbosity)
                    try:
                        filteredFiles = GetFilteredFiles(captureLog, customPackageFileFilter, clangFormatConfiguration, package)
                        if customPackageFileFilter is None or filteredFiles is not None:
                            processedCount += 1
                            _RunClangFormat(captureLog, toolConfig, clangFormatConfiguration, clangExeInfo, package, filteredFiles, repairEnabled)
                        examinedCount += 1
                    finally:
                        try:
                            if len(captureLog.Captured) > 0:
                                capturedLog = "Package: '{0}' result:\n{1}".format(package.Name, "\n".join(captureLog.Captured))
                                mainLog.DoPrint(capturedLog)
                        except:
                            pass
        except Exception as ex:
            cancellationToken.Cancel()
            mainLog.DoPrintError("Cancelling tasks due to exception: {0}")
            raise
        finally:
            mainLog.LogPrintVerbose(4, "Ending thread {0}".format(threadId))

        return (examinedCount, processedCount)


class PerformClangFormatHelper2(object):
    RULE_FORMAT = "format"

    @staticmethod
    def Process(log: Log, toolConfig: ToolConfig, customPackageFileFilter: Optional[CustomPackageFileFilter],
                clangFormatConfiguration: ClangFormatConfiguration, cmakeConfig: GeneratorCMakeConfig,
                clangFormatExeInfo: ClangExeInfo, ninjaExeInfo: ClangExeInfo, sortedPackageList: List[Package],
                repairEnabled: bool, numBuildThreads: int) -> int:
        totalProcessedCount = 0

        logOutput = False
        currentWorkingDirectory = BuildUtil.GetBuildDir(log, toolConfig, cmakeConfig.CheckDir)
        currentWorkingDirectory = IOUtil.Join(currentWorkingDirectory, "format")
        ninjaOutputFile =  IOUtil.Join(currentWorkingDirectory, "build.ninja")
        toolVersionOutputFile =  IOUtil.Join(currentWorkingDirectory, "ToolVersions.txt")
        IOUtil.SafeMakeDirs(currentWorkingDirectory)

        log.LogPrint("Using path: '{0}'".format(currentWorkingDirectory))

        log.LogPrint("Storing tool versions.")
        PerformClangFormatHelper2.WriteToolVersionFile(log, toolVersionOutputFile, clangFormatExeInfo, ninjaExeInfo)

        log.LogPrint("Generating ninja format file.")
        #foundFixes = PerformClangTidyHelper2.FindFixes(log, clangTidyFixOutputFolder)
        #if len(foundFixes) > 0:
        #    PerformClangTidyHelper2.DeleteFixes(log, clangTidyFixOutputFolder, foundFixes)

        totalProcessedCount = PerformClangFormatHelper2.GenerateNinjaTidyFile(log, toolConfig, ninjaOutputFile, currentWorkingDirectory,
                                                                              toolVersionOutputFile, clangFormatExeInfo, customPackageFileFilter,
                                                                              clangFormatConfiguration, sortedPackageList, repairEnabled)
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
    def GenerateNinjaTidyFile(log: Log, toolConfig: ToolConfig, ninjaOutputFile: str, outputFolder: str, toolVersionOutputFile: str,
                              clangFormatExeInfo: ClangExeInfo, customPackageFileFilter: Optional[CustomPackageFileFilter],
                              clangFormatConfiguration: ClangFormatConfiguration, sortedPackageList: List[Package], repairEnabled: bool) -> int:
        totalProcessedCount = 0
        toolProjectContextsDict = PackagePathUtil.CreateToolProjectContextsDict(toolConfig.ProjectInfo)
        with io.StringIO() as ninjaFile:
            ninjaFile.writelines("ninja_required_version = 1.8\n")

            writer = Writer(ninjaFile, 149)

            formatCommand = "{0} $in --style=file".format(clangFormatExeInfo.Command)
            if repairEnabled:
                formatCommand += " -i"
            if len(clangFormatConfiguration.AdditionalUserArguments) > 0:
                log.LogPrint("Adding user supplied arguments before '--' {0}".format(clangFormatConfiguration.AdditionalUserArguments))
                formatCommand += " {0}".format(" ".join(clangFormatConfiguration.AdditionalUserArguments))

            writer.rule(name=PerformClangFormatHelper2.RULE_FORMAT,
                        command=formatCommand,
                        restat=True)

            for package in sortedPackageList:
                PerformClangFormatHelper2.AddPackage(log, toolConfig, toolVersionOutputFile, writer, package, outputFolder, toolProjectContextsDict,
                                                     customPackageFileFilter, clangFormatConfiguration)
                totalProcessedCount += 1

            # finally we write the ninja file
            IOUtil.WriteFileIfChanged(ninjaOutputFile, ninjaFile.getvalue())
            writer.close()
        return totalProcessedCount

    @staticmethod
    def AddPackage(log: Log, toolConfig: ToolConfig, toolVersionOutputFile: str, writer: Writer, package: Package, outputFolder: str,
                   toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext], customPackageFileFilter: Optional[CustomPackageFileFilter],
                   clangFormatConfiguration: ClangFormatConfiguration) -> None:

        outputFolder = IOUtil.Join(outputFolder, PackagePathUtil.GetPackagePath(package, toolProjectContextsDict))

        filteredFiles = GetFilteredFiles(log, customPackageFileFilter, clangFormatConfiguration, package)
        formatPackageConfig = FormatPackageConfig(log, package, clangFormatConfiguration, filteredFiles)

        for fileEntry in formatPackageConfig.AllFiles:
            outputFile = "{0}.dmy".format(IOUtil.Join(outputFolder, fileEntry.SourcePath))

            # Locate the closest clang format configuration file so we can add it as a implicit package dependency
            packageClosestFormatPath = FileFinder.FindClosestFileInRoot(log, toolConfig, fileEntry.ResolvedPath, clangFormatConfiguration.CustomFormatFile)
            packageClosestFormatFile = IOUtil.Join(packageClosestFormatPath, clangFormatConfiguration.CustomFormatFile)

            implicitDeps = [packageClosestFormatFile, toolVersionOutputFile]

            writer.build(outputs=outputFile, rule=PerformClangFormatHelper2.RULE_FORMAT, implicit=implicitDeps,  inputs=fileEntry.ResolvedPath)

            # Write a dummy file so ninja finds a file (this is because clang format overwrites the existing file, so we need a dummy file to track progress)
            if not IOUtil.Exists(outputFile):
                parentDir = IOUtil.GetDirectoryName(outputFile)
                if not IOUtil.Exists(parentDir):
                    IOUtil.SafeMakeDirs(parentDir)
                IOUtil.WriteFileIfChanged(outputFile, "")



class PerformClangFormat(object):
    @staticmethod
    def Run(log: Log, toolConfig: ToolConfig, formatPackageList: List[Package],
            customPackageFileFilter: Optional[CustomPackageFileFilter],
            packageRecipeResultManager: PackageRecipeResultManager,
            clangFormatConfiguration: ClangFormatConfiguration, cmakeConfig: GeneratorCMakeConfig,
            repairEnabled: bool, buildThreads: int,
            useLegacyTidyMethod: bool) -> None:
        """ RunClangFormat on a package at a time """

        # Lookup the recommended build threads using the standard build algorithm
        numBuildThreads = PlatformBuildUtil.GetRecommendedBuildThreads(buildThreads)

        clangExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, clangFormatConfiguration.RecipePackageName,
                                                            MagicValues.ClangFormatCommand)
        ninjaExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, clangFormatConfiguration.NinjaRecipePackageName,
                                                            MagicValues.NinjaCommand)

        log.LogPrint("ClangFormat version: {0}".format(clangExeInfo.Version))
        log.LogPrint("Ninja version: {0}".format(ninjaExeInfo.Version))

        log.LogPrint("Running clang-format")

        # Filter the package list so it only contains things we can process
        finalPackageList = [package for package in formatPackageList if PerformClangUtil.CanProcessPackage(package)]

        sortedPackages = list(finalPackageList)
        sortedPackages.sort(key=lambda s: s.Name.lower())

        #test = set()
        #for entry in sortedPackages:
        #    if entry.Name in test:
        #        raise Exception("duplicated package")
        #    else:
        #       test.add(entry.Name)

        if useLegacyTidyMethod:
            count = PerformClangFormatHelper.Process(log, toolConfig, customPackageFileFilter, clangFormatConfiguration, clangExeInfo, sortedPackages,
                                                     repairEnabled, numBuildThreads)
        else:
            count = PerformClangFormatHelper2.Process(log, toolConfig, customPackageFileFilter, clangFormatConfiguration, cmakeConfig, clangExeInfo,
                                                      ninjaExeInfo, sortedPackages, repairEnabled, numBuildThreads)


        if count == 0:
            if customPackageFileFilter is None:
                log.DoPrintWarning("No files processed")
            else:
                log.DoPrintWarning("No files processed, could not find a package that matches {0}".format(customPackageFileFilter))
