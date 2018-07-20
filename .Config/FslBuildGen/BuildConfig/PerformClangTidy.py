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
from typing import Set
from typing import Tuple
from concurrent.futures import ThreadPoolExecutor
import copy
import os
import os.path
import queue
import subprocess
import sys
import threading
from FslBuildGen import IOUtil
from FslBuildGen import PackageListUtil
from FslBuildGen.Build.Filter import PackageFilter
from FslBuildGen.BuildConfig.CaptureLog import CaptureLog
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildConfig.ClangTidyConfiguration import ClangTidyConfiguration
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.PerformClangTidyConfig import PerformClangTidyConfig
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.Build.BuildVariantUtil import BuildVariantUtil
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.Build.VirtualVariantEnvironmentCache import VirtualVariantEnvironmentCache
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import BuildThreads
from FslBuildGen.Exceptions import AggregateException
from FslBuildGen.Exceptions import ExitException
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Generator.Report.ParsedFormatString import ParsedFormatString
from FslBuildGen.Generator.Report.StringVariableDict import StringVariableDict
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory

class MagicValues(object):
    ClangTidyCommand = "clang-tidy"


class LocalVariantInfo(object):
    def __init__(self, resolvedVariantSettingsDict: Dict[str,str],
                 generatorReportDict: Dict[Package, PackageGeneratorReport],
                 pythonScriptRoot: str) -> None:
        super().__init__()
        self.ResolvedVariantSettingsDict = resolvedVariantSettingsDict
        self.GeneratorReportDict = generatorReportDict
        self.PythonScriptRoot = pythonScriptRoot


class SimpleCancellationToken(object):
    def __init__(self) -> None:
        self.__Lock = threading.Lock()
        self.__IsCancelled = False

    def IsCancelled(self) -> bool:
        self.__Lock.acquire()
        try:
            return self.__IsCancelled
        finally:
            self.__Lock.release()

    def Cancel(self) -> None:
        self.__Lock.acquire()
        try:
            self.__IsCancelled = True
        finally:
            self.__Lock.release()


def __TryGetEnvironmentVariable(virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache, envVariable: str) -> str:
    res = virtualVariantEnvironmentCache.TryGetCachedValue(envVariable)
    if res is None:
        res = IOUtil.GetEnvironmentVariable(envVariable)
    return res


def __ResolveVariables(strWithVariables: str, variableDict: StringVariableDict, virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache) -> str:
    parsedStr = ParsedFormatString(strWithVariables, variableDict, lambda name: __TryGetEnvironmentVariable(virtualVariantEnvironmentCache, name) )
    for var1 in parsedStr.VarCommandList:
        parsedStr.SplitList[var1.SplitIndex] = var1.Report.Options[0]
    for var2 in parsedStr.EnvCommandList:
        parsedStr.SplitList[var2.SplitIndex] = var2.Value
    return "".join(parsedStr.SplitList)


def __ExtractVariantDefines(log: Log, localVariantInfo: LocalVariantInfo, package: Package) -> List[str]:
    """
    Extract all static and virtual defines
    """
    if len(package.ResolvedAllVariantDict) <= 0:
        return []

    allDefines = []  # type: List[str]
    for variant in package.ResolvedAllVariantDict.values():
        if variant.Type == VariantType.Virtual :
            if len(variant.Options) != 1:
                raise Exception("Unsupported virtual variant type")
            for externalDep in variant.Options[0].ExternalDependencies:
                for define in variant.Options[0].DirectDefines:
                    allDefines.append(define.Name)
        else:
            optionName = localVariantInfo.ResolvedVariantSettingsDict[variant.Name]
            selectedOption = variant.OptionDict[optionName]
            for define in selectedOption.DirectDefines:
                allDefines.append(define.Name)

    return allDefines


def __ExtractVariantIncludeDirs(log: Log, localVariantInfo: LocalVariantInfo,
                                virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache, package: Package) -> List[str]:
    """
    Extract all static and virtual include dirs and ensure that 'dynamic' environment variables gets cached
    """
    if len(package.ResolvedAllVariantDict) <= 0:
        return []

    environmentVariableList = []  # type: List[str]
    allIncludeDirs = []  # type: List[str]
    for variant in package.ResolvedAllVariantDict.values():
        if variant.Type == VariantType.Virtual :
            if len(variant.Options) != 1:
                raise Exception("Unsupported virtual variant type")
            parsedDeps = []  # type: List[ParsedFormatString]
            for externalDep in variant.Options[0].ExternalDependencies:
                if externalDep.Include is not None:
                    if externalDep.Include.startswith("$("):
                        endIndex = externalDep.Include.find(')')
                        if endIndex < 0:
                            raise Exception("external include path invalid no ending ')' in '{0}'".format(externalDep.Include))
                        environmentVariableList.append(externalDep.Include[:endIndex+1])
                    allIncludeDirs.append(externalDep.Include)
        else:
            optionName = localVariantInfo.ResolvedVariantSettingsDict[variant.Name]
            selectedOption = variant.OptionDict[optionName]
            for externalDep in selectedOption.ExternalDependencies:
                if externalDep.Include is not None:
                    allIncludeDirs.append(externalDep.Include)

    if package not in localVariantInfo.GeneratorReportDict:
        raise Exception("Could not find a report for package '{0}".format(package.Name))
    report = localVariantInfo.GeneratorReportDict[package]

    runCommand = []
    if report.BuildReport is not None and report.BuildReport.BuildCommandReport is not None:
        runInEnvScript = report.BuildReport.BuildCommandReport.RunInEnvScript
        if runInEnvScript is not None:
            if package.AbsolutePath is None:
                raise Exception("internal error")
            runCommand.append(IOUtil.Join(package.AbsolutePath, runInEnvScript))

    # Cache all environment
    virtualVariantEnvironmentCache.CacheEnv(environmentVariableList, runCommand)

    #
    return allIncludeDirs


def __BuildClangTidyPackageIncludePaths(log: Log, localVariantInfo: LocalVariantInfo,
                                        virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache, package: Package) -> List[str]:
    allIncludeDirs = package.ResolvedBuildAllIncludeDirs
    if allIncludeDirs is None:
        raise Exception("Package ResolvedBuildAllIncludeDirs was not resolved")
    if package.AbsolutePath is None:
        raise Exception("Package AbsolutePath was was None")

    variantIncludeDirs = __ExtractVariantIncludeDirs(log, localVariantInfo, virtualVariantEnvironmentCache, package)
    combinedIncludeDirs = allIncludeDirs + variantIncludeDirs

    variableDict = StringVariableDict()

    includeDirCommands = []
    for includeDir in combinedIncludeDirs:
        if not includeDir.startswith("$("):
            packageIncludeDir = IOUtil.Join(package.AbsolutePath, includeDir)
            includeDirCommands.append('-I')
            includeDirCommands.append(IOUtil.NormalizePath(packageIncludeDir))
        else:
            packageIncludeDir = __ResolveVariables(includeDir, variableDict, virtualVariantEnvironmentCache)
            includeDirCommands.append('-I')
            includeDirCommands.append(IOUtil.NormalizePath(packageIncludeDir))
    return includeDirCommands

def __BuildClangTidyPackageDefines(log: Log, localVariantInfo: LocalVariantInfo, package: Package) -> List[str]:

    defineCommands = [] # type: List[str]
    for define in package.ResolvedBuildAllDefines:
        defineCommands.append('-D')
        defineCommands.append(define.Name)

    variantDefines = __ExtractVariantDefines(log, localVariantInfo, package)
    for define2 in variantDefines:
        defineCommands.append('-D')
        defineCommands.append(define2)
    return defineCommands

def __RunNow(log: Log, buildCommand: List[str], currentWorkingDirectory: str, logOutput: bool) -> int:
    if not logOutput:
        return subprocess.call(buildCommand, cwd=currentWorkingDirectory)

    try:
        with subprocess.Popen(buildCommand, cwd=currentWorkingDirectory, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, universal_newlines=True) as proc:
            output = proc.stdout.read().strip()
            proc.stdout.close()
            return proc.wait()
        #output = ""
        #return 0
    finally:
        if output is not None and len(output) > 0:
            log.DoPrint(output)


def __LookupEnvironmentVariables(variables: List[str], virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache) -> List[str]:
    variableDict = StringVariableDict()
    result = [] # type: List[str]
    for entry in variables:
        updated = __ResolveVariables(entry, variableDict, virtualVariantEnvironmentCache)
        result.append(updated)
    return result

def _RunClangTidy(log: Log, toolConfig: ToolConfig, platformId: str,
                  performClangTidyConfig: PerformClangTidyConfig,
                  clangExeInfo: ClangExeInfo,
                  package: Package,
                  filteredFiles: Optional[List[str]],
                  clangFormatFilename: Optional[str],
                  localVariantInfo: LocalVariantInfo,
                  virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache,
                  logOutput: bool = False) -> None:
    if package.ResolvedBuildAllIncludeFiles is None:
        raise Exception("invalid package")
    log.LogPrint("- {0}".format(package.Name))

    clangTidyConfiguration = performClangTidyConfig.ClangTidyConfiguration

    if package.AbsolutePath is None or package.ResolvedBuildSourceFiles is None:
        raise Exception("Invalid package")

    allFiles = [] # type: List[str]

    if filteredFiles is None:
        for fileName in package.ResolvedBuildAllIncludeFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            # Only process files with the expected extension
            if PerformClangUtil.IsValidExtension(fileName, clangTidyConfiguration.FileExtensions):
                allFiles.append(fileName)

        for fileName in package.ResolvedBuildSourceFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            if PerformClangUtil.IsValidExtension(fileName, clangTidyConfiguration.FileExtensions):
                allFiles.append(fileName)
    else:
        for fileName in filteredFiles:
            if PerformClangUtil.IsValidExtension(fileName, clangTidyConfiguration.FileExtensions):
                allFiles.append(fileName)

    # ensure we process the files in the same order every time
    allFiles.sort()

    platformCompilerFlags = [] # type: List[str]
    platformDefineCommands = [] # type: List[str]
    platformStrictChecks = [] # type: List[str]
    if platformId in clangTidyConfiguration.PlatformDict:
        clangPlatformConfig = clangTidyConfiguration.PlatformDict[platformId]
        platformCompilerFlags = clangPlatformConfig.Compiler.Flags
        for platformDefine in clangPlatformConfig.Defines.All:
            platformDefineCommands.append('-D')
            platformDefineCommands.append(platformDefine)
        # We default to release for now
        for platformDefine in clangPlatformConfig.Defines.Release:
            platformDefineCommands.append('-D')
            platformDefineCommands.append(platformDefine)
        for strictCheck in clangPlatformConfig.StrictChecks:
            platformStrictChecks.append(strictCheck)

    includePaths = __BuildClangTidyPackageIncludePaths(log, localVariantInfo, virtualVariantEnvironmentCache, package)
    packageDefineCommands = __BuildClangTidyPackageDefines(log, localVariantInfo, package)

    cmd = clangExeInfo.ClangCommand
    buildCommand = [cmd]
    if performClangTidyConfig.Repair:
        buildCommand.append('-fix')

    #buildCommand.append('-fix-errors')

    #buildCommand.append('-header-filter=.*')

    usingCheckCommand = False
    if len(performClangTidyConfig.OverrideChecks) > 0:
        newOverrideChecks = ",".join(performClangTidyConfig.OverrideChecks)
        log.LogPrintVerbose(2, "Overriding checks checks '{0}'".format(newOverrideChecks))
        if performClangTidyConfig.StrictChecks:
            log.DoPrintWarning("Ignoreing strict checks because 'override' is enabled")
        buildCommand.append("--checks")
        buildCommand.append(newOverrideChecks)
        usingCheckCommand = True
    elif performClangTidyConfig.StrictChecks and len(platformStrictChecks) > 0:
        newStrictChecks = ",".join(platformStrictChecks)
        log.LogPrintVerbose(2, "Adding strict checks '{0}'".format(newStrictChecks))
        buildCommand.append("--checks")
        buildCommand.append(newStrictChecks)
        usingCheckCommand = True

    if len(performClangTidyConfig.AdditionalUserArguments) > 0:
        log.LogPrintVerbose(2, "Adding user supplied arguments before '--' {0}".format(performClangTidyConfig.AdditionalUserArguments))
        if usingCheckCommand and '--checks' in performClangTidyConfig.AdditionalUserArguments:
            log.DoPrintWarning("another command is adding '--checks' so it could conflict with the user supplied argument")
        buildCommand += performClangTidyConfig.AdditionalUserArguments

    buildCommand += allFiles

    buildCommand.append('--')

    if len(platformCompilerFlags) > 0:
        buildCommand += __LookupEnvironmentVariables(platformCompilerFlags, virtualVariantEnvironmentCache)
    if len(platformDefineCommands) > 0:
        buildCommand += platformDefineCommands

    if len(includePaths) > 0:
        buildCommand += includePaths
    if len(packageDefineCommands) > 0:
        buildCommand += packageDefineCommands

    if len(performClangTidyConfig.PostfixArguments) > 0:
        log.LogPrintVerbose(2, "Adding user supplied arguments after '--' {0}".format(performClangTidyConfig.PostfixArguments))
        buildCommand += performClangTidyConfig.PostfixArguments

    currentWorkingDirectory = package.AbsolutePath

    if clangFormatFilename is not None:
        FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, clangFormatFilename)
    FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, clangTidyConfiguration.CustomTidyFile)

    try:
        if log.Verbosity >= 4:
            log.LogPrint("Running command '{0}' in cwd: {1}".format(buildCommand, currentWorkingDirectory))
        result = __RunNow(log, buildCommand, currentWorkingDirectory, logOutput)
        if result != 0:
            log.LogPrintWarning("The command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
            raise ExitException(result)
    except FileNotFoundError:
            log.DoPrintWarning("The command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(buildCommand), currentWorkingDirectory))
            raise



class PerformClangTidyHelper(object):
    @staticmethod
    def ProcessAllPackages(log: Log, toolConfig: ToolConfig, platformId: str, pythonScriptRoot: str,
                           performClangTidyConfig: PerformClangTidyConfig,
                           clangExeInfo: ClangExeInfo, packageList: List[Package],
                           customPackageFileFilter: Optional[CustomPackageFileFilter],
                           localVariantInfo: LocalVariantInfo,
                           buildThreads: int) -> int:

        totalProcessedCount = 0
        if buildThreads <= 1 or len(packageList) <= 1:
            virtualVariantEnvironmentCache = VirtualVariantEnvironmentCache(log, pythonScriptRoot)
            # do standard serial processing
            for package in packageList:
                filteredFiles = None
                if customPackageFileFilter is not None:
                    filteredFiles = customPackageFileFilter.TryLocateFilePatternInPackage(log, package,
                                                                                          performClangTidyConfig.ClangTidyConfiguration.FileExtensions)
                if customPackageFileFilter is None or filteredFiles is not None:
                    totalProcessedCount += 1;
                    _RunClangTidy(log, toolConfig, platformId, performClangTidyConfig, clangExeInfo,
                                  package, filteredFiles, None, localVariantInfo, virtualVariantEnvironmentCache)
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
                        taskFuture = executor.submit(PerformClangTidyHelper.RunInAnotherThread,
                                                     packageQueue, cancellationToken,
                                                     log, toolConfig, platformId, pythonScriptRoot,
                                                     performClangTidyConfig, clangExeInfo, customPackageFileFilter,
                                                     localVariantInfo)
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
                           mainLog: Log, toolConfig: ToolConfig, platformId: str, pythonScriptRoot: str,
                           performClangTidyConfig: PerformClangTidyConfig, clangExeInfo: ClangExeInfo,
                           customPackageFileFilter: Optional[CustomPackageFileFilter], localVariantInfo: LocalVariantInfo) -> Tuple[int,int]:
        threadId = threading.get_ident()
        mainLog.LogPrintVerbose(4, "Starting thread {0}".format(threadId))
        examinedCount = 0
        processedCount = 0
        keepWorking = True
        package = None # type: Optional[Package]

        try:
            virtualVariantEnvironmentCache = VirtualVariantEnvironmentCache(mainLog, pythonScriptRoot)
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
                        mainLog.LogPrint("- clang-tidy on package '{0}' on thread {1}".format(package.Name, threadId))
                    else:
                        mainLog.LogPrint("- clang-tidy on package '{0}'".format(package.Name))

                    captureLog = CaptureLog(mainLog.Title, mainLog.Verbosity)
                    try:
                        filteredFiles = None
                        if customPackageFileFilter is not None:
                            filteredFiles = customPackageFileFilter.TryLocateFilePatternInPackage(captureLog, package,
                                                                                                  performClangTidyConfig.ClangTidyConfiguration.FileExtensions)
                        if customPackageFileFilter is None or filteredFiles is not None:
                            processedCount += 1
                            _RunClangTidy(captureLog, toolConfig, platformId, performClangTidyConfig, clangExeInfo,
                                          package, filteredFiles, None, localVariantInfo, virtualVariantEnvironmentCache, True)
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



class PerformClangTidy(object):
    @staticmethod
    def Run(log: Log, toolConfig: ToolConfig, platformId: str,
            topLevelPackage: Package, tidyPackageList: List[Package], userBuildVariantsDict: Dict[str, str],
            pythonScriptRoot: str, generatorContext: GeneratorContext, sdkConfigTemplatePath: str,
            packageRecipeResultManager: PackageRecipeResultManager,
            performClangTidyConfig: PerformClangTidyConfig,
            customPackageFileFilter: Optional[CustomPackageFileFilter],
            clangFormatFilename: Optional[str], buildThreads: int) -> None:
        """
        RunClangTidy on a package at a time
        :param topLevelPackage: the top level system package
        :param tidyPackageList: the packages to 'tidy'
        :param userBuildVariantsDict: the variant configuration supplied by the user
        :param performClangTidyConfig:
        :param customPackageFileFilter:
        :param clangFormatFilename:
        :param repairEnabled:
        """

        # Lookup the recommended build threads using the standard build algorithm
        buildThreads = PlatformBuildUtil.GetRecommendedBuildThreads(buildThreads)

        clangExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, performClangTidyConfig.ClangTidyConfiguration.RecipePackageName,
                                                            MagicValues.ClangTidyCommand)

        BuildVariantUtil.ValidateUserVariantSettings(log, topLevelPackage, userBuildVariantsDict)
        BuildVariantUtil.LogVariantSettings(log, userBuildVariantsDict)
        resolvedVariantSettingsDict = BuildVariantUtil.CreateCompleteStaticVariantSettings(topLevelPackage.ResolvedAllVariantDict,
                                                                                           userBuildVariantsDict)

        if log.Verbosity >= 1:
            PerformClangUtil.ShowVersion(log, clangExeInfo)
        log.LogPrint("Running clang-tidy")

        # Filter the package list so it only contains things we can process
        finalPackageList = [package for package in tidyPackageList if PerformClangUtil.CanProcessPackage(package)]

        generatorConfig = GeneratorConfig(sdkConfigTemplatePath, toolConfig)
        generatorReportDict = generatorContext.Generator.GenerateReport(log, generatorConfig, finalPackageList)

        # Validate report dict
        for package in finalPackageList:
            if package not in generatorReportDict:
                raise Exception("Generator report is missing information for package '{0}'".format(package.Name))

        localVariantInfo = LocalVariantInfo(resolvedVariantSettingsDict, generatorReportDict, pythonScriptRoot)

        count = PerformClangTidyHelper.ProcessAllPackages(log, toolConfig, platformId, pythonScriptRoot, performClangTidyConfig,
                                                          clangExeInfo, finalPackageList, customPackageFileFilter,
                                                          localVariantInfo, buildThreads)
        if count == 0:
            if customPackageFileFilter is None:
                log.DoPrintWarning("No files processed")
            else:
                log.DoPrintWarning("No files processed, could not find a package that matches {0}".format(customPackageFileFilter))

