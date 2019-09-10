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
from typing import List
from typing import Optional
from typing import Tuple
from concurrent.futures import ThreadPoolExecutor
import os
import os.path
import queue
import subprocess
import sys
import threading
from FslBuildGen import IOUtil
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.BuildConfig.CaptureLog import CaptureLog
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildConfig.ClangFormatConfiguration import ClangFormatConfiguration
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.BuildConfig.SimpleCancellationToken import SimpleCancellationToken
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import AggregateException
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

        if package.ResolvedContentFiles is not None:
            for resolvedPath in package.ResolvedContentFiles:
                if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, clangFormatConfiguration.FileExtensions):
                    allFiles.append(resolvedPath.ResolvedPath)

        if package.ResolvedContentBuilderSyncInputFiles is not None:
            for resolvedPath in package.ResolvedContentBuilderSyncInputFiles:
                if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, clangFormatConfiguration.FileExtensions):
                    allFiles.append(resolvedPath.ResolvedPath)

        if package.ResolvedContentBuilderBuildInputFiles is not None:
            for resolvedPath in package.ResolvedContentBuilderBuildInputFiles:
                if PerformClangUtil.IsValidExtension(resolvedPath.ResolvedPath, clangFormatConfiguration.FileExtensions):
                    allFiles.append(resolvedPath.ResolvedPath)
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


class PerformClangFormatHelper(object):
    @staticmethod
    def Process(log: Log, toolConfig: ToolConfig, customPackageFileFilter: Optional[CustomPackageFileFilter],
                clangFormatConfiguration: ClangFormatConfiguration, clangExeInfo: ClangExeInfo, packageList: List[Package],
                repairEnabled: bool, buildThreads: int) -> int:
        totalProcessedCount = 0
        if buildThreads <= 1 or len(packageList) <= 1:
            for package in packageList:
                filteredFiles = None if customPackageFileFilter is None else customPackageFileFilter.TryLocateFilePatternInPackage(log, package,
                                                                                                                                   clangFormatConfiguration.FileExtensions)
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
            finalBuildThreads = buildThreads if len(packageList) >= buildThreads else len(packageList)
            exceptionList = []
            cancellationToken = SimpleCancellationToken()
            totalExaminedCount = 0
            try:
                with ThreadPoolExecutor(max_workers=finalBuildThreads) as executor:

                    futures = []
                    for index in range(0, buildThreads):
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
                        filteredFiles = None
                        if customPackageFileFilter is not None:
                            filteredFiles = customPackageFileFilter.TryLocateFilePatternInPackage(captureLog, package, clangFormatConfiguration.FileExtensions)
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


class PerformClangFormat(object):
    @staticmethod
    def Run(log: Log, toolConfig: ToolConfig, formatPackageList: List[Package],
            customPackageFileFilter: Optional[CustomPackageFileFilter],
            packageRecipeResultManager: PackageRecipeResultManager,
            clangFormatConfiguration: ClangFormatConfiguration, repairEnabled: bool, buildThreads: int) -> None:
        """ RunClangFormat on a package at a time """

        # Lookup the recommended build threads using the standard build algorithm
        buildThreads = PlatformBuildUtil.GetRecommendedBuildThreads(buildThreads)

        clangExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, clangFormatConfiguration.RecipePackageName,
                                                            MagicValues.ClangFormatCommand)

        if log.Verbosity >= 1:
            PerformClangUtil.ShowVersion(log, clangExeInfo)

        log.LogPrint("Running clang-format")

        # Filter the package list so it only contains things we can process
        finalPackageList = [package for package in formatPackageList if PerformClangUtil.CanProcessPackage(package)]

        sortedPackages = list(finalPackageList)
        sortedPackages.sort(key=lambda s: s.Name.lower())

        count = PerformClangFormatHelper.Process(log, toolConfig, customPackageFileFilter, clangFormatConfiguration, clangExeInfo, sortedPackages,
                                                 repairEnabled, buildThreads)
        if count == 0:
            if customPackageFileFilter is None:
                log.DoPrintWarning("No files processed")
            else:
                log.DoPrintWarning("No files processed, could not find a package that matches {0}".format(customPackageFileFilter))
