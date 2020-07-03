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
import io
import queue
import threading
from FslBuildGen import IOUtil
from FslBuildGen.BuildConfig.BuildUtil import BuildUtil
from FslBuildGen.BuildConfig.CaptureLog import CaptureLog
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildConfig.ClangTidyConfiguration import ClangTidyConfiguration
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.FileFinder import FileFinder
from FslBuildGen.BuildConfig.PackagePathUtil import PackagePathUtil
from FslBuildGen.BuildConfig.PerformClangTidyConfig import PerformClangTidyConfig
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.BuildConfig.RunHelper import RunHelper
from FslBuildGen.BuildConfig.SimpleCancellationToken import SimpleCancellationToken
from FslBuildGen.BuildConfig.ninja_syntax import Writer
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.Build.BuildVariantUtil import BuildVariantUtil
from FslBuildGen.Build.BuildUtil import PlatformBuildUtil
from FslBuildGen.Build.DataTypes import CommandType
from FslBuildGen.Build.VirtualVariantEnvironmentCache import VirtualVariantEnvironmentCache
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.DataTypes import ClangTidyProfile
from FslBuildGen.Exceptions import AggregateException
from FslBuildGen.Exceptions import ExitException
from FslBuildGen.Generator.GeneratorCMakeConfig import GeneratorCMakeConfig
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.Report.ParsedFormatString import ParsedFormatString
from FslBuildGen.Generator.Report.StringVariableDict import StringVariableDict
from FslBuildGen.ProjectId import ProjectId
from FslBuildGen.Location.ResolvedPath import ResolvedPath
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigProjectContext import ToolConfigProjectContext
from FslBuildGen.ToolConfigProjectInfo import ToolConfigProjectInfo

class MagicValues(object):
    ClangCompileCommand = "clang"
    ClangTidyCommand = "clang-tidy"
    ClangTidyApplyReplacements = "clang-apply-replacements"
    NinjaCommand = "ninja"


class LocalVariantInfo(object):
    def __init__(self, resolvedVariantSettingsDict: Dict[str,str],
                 generatorReportDict: Dict[Package, PackageGeneratorReport],
                 pythonScriptRoot: str) -> None:
        super().__init__()
        self.ResolvedVariantSettingsDict = resolvedVariantSettingsDict
        self.GeneratorReportDict = generatorReportDict
        self.PythonScriptRoot = pythonScriptRoot


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
        log.LogPrintVerbose(2, "Extracting dynamic variants from {0}".format(package.Name))
        runInEnvScript = report.BuildReport.BuildCommandReport.RunInEnvScript
        if runInEnvScript is not None:
            if package.AbsolutePath is None:
                raise Exception("internal error")
            runCommand.append(IOUtil.Join(package.AbsolutePath, runInEnvScript))

    # Cache all environment
    virtualVariantEnvironmentCache.CacheEnv(environmentVariableList, runCommand)

    #
    return allIncludeDirs


def _BuildClangTidyPackageIncludePaths(log: Log, localVariantInfo: LocalVariantInfo,
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
            includeDirCommands.append(IOUtil.NormalizePath(packageIncludeDir))
        else:
            packageIncludeDir = __ResolveVariables(includeDir, variableDict, virtualVariantEnvironmentCache)
            includeDirCommands.append(IOUtil.NormalizePath(packageIncludeDir))
    return includeDirCommands

def _BuildClangTidyPackageDefines(log: Log, localVariantInfo: LocalVariantInfo, package: Package) -> List[str]:

    defineCommands = [] # type: List[str]
    for define in package.ResolvedBuildAllDefines:
        defineCommands.append(define.Name)

    variantDefines = __ExtractVariantDefines(log, localVariantInfo, package)
    for define2 in variantDefines:
        defineCommands.append(define2)
    return defineCommands

def _LookupEnvironmentVariables(variables: List[str], virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache) -> List[str]:
    variableDict = StringVariableDict()
    result = [] # type: List[str]
    for entry in variables:
        updated = __ResolveVariables(entry, variableDict, virtualVariantEnvironmentCache)
        result.append(updated)
    return result

def __GetResolvedPathFromAbsPath(package: Package, absFilePath: str) -> ResolvedPath:
    if package.Path is None:
        raise Exception("invalid package")
    packagePath = package.Path.AbsoluteDirPath + '/'
    if not absFilePath.startswith(packagePath):
        raise Exception("file '{0}' is not part of the package at path '{1}'".format(absFilePath, packagePath))
    relativePath = absFilePath[len(packagePath):]
    return ResolvedPath(relativePath, absFilePath)

def _BuildAllFiles(package: Package, filteredFiles: Optional[List[str]], clangTidyConfiguration: ClangTidyConfiguration) -> List[ResolvedPath]:
    if package.AbsolutePath is None or package.ResolvedBuildAllIncludeFiles is None or package.ResolvedBuildSourceFiles is None:
        raise Exception("invalid package")

    allFiles = [] # type: List[ResolvedPath]
    if filteredFiles is None:
        for fileName in package.ResolvedBuildAllIncludeFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            # Only process files with the expected extension
            if PerformClangUtil.IsValidExtension(fileName, clangTidyConfiguration.FileExtensions):
                allFiles.append(ResolvedPath(fileName, fullPath))

        for fileName in package.ResolvedBuildSourceFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            if PerformClangUtil.IsValidExtension(fileName, clangTidyConfiguration.FileExtensions):
                allFiles.append(ResolvedPath(fileName, fullPath))
    else:
        for fileName in filteredFiles:
            if PerformClangUtil.IsValidExtension(fileName, clangTidyConfiguration.FileExtensions):
                allFiles.append(__GetResolvedPathFromAbsPath(package, fileName))
    return allFiles

def _AddCmdToEachEntry(cmd: str, entries: List[str]) -> List[str]:
    newList = []
    for entry in entries:
        newList.append(cmd)
        newList.append(entry)
    return newList

class TidyPlatformConfig(object):
    def __init__(self, log: Log, platformId: str, clangTidyConfiguration: ClangTidyConfiguration,
                 virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache) -> None:
        platformCompilerFlags = [] # type: List[str]
        platformDefineCommands = [] # type: List[str]
        platformStrictChecks = [] # type: List[str]
        if platformId in clangTidyConfiguration.PlatformDict:
            clangPlatformConfig = clangTidyConfiguration.PlatformDict[platformId]
            platformCompilerFlags = clangPlatformConfig.Compiler.Flags
            for platformDefine in clangPlatformConfig.Defines.All:
                platformDefineCommands.append(platformDefine)
            # We default to release for now
            for platformDefine in clangPlatformConfig.Defines.Release:
                platformDefineCommands.append(platformDefine)
            for strictCheck in clangPlatformConfig.StrictChecks:
                platformStrictChecks.append(strictCheck)

        # sort these to ensure we generate the things in the same order everytime
        platformDefineCommands.sort()
        platformStrictChecks.sort()

        self.PlatformCompilerFlags = _LookupEnvironmentVariables(platformCompilerFlags, virtualVariantEnvironmentCache)
        self.PlatformDefineCommands = platformDefineCommands
        self.PlatformStrictChecks = platformStrictChecks

class TidyPackageConfig(object):
    def __init__(self, log: Log, package: Package, filteredFiles: Optional[List[str]], clangTidyConfiguration: ClangTidyConfiguration,
                 localVariantInfo: LocalVariantInfo, virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache) -> None:

        self.AllFiles = _BuildAllFiles(package, filteredFiles, clangTidyConfiguration) # type: List[ResolvedPath]
        self.IncludePaths = _BuildClangTidyPackageIncludePaths(log, localVariantInfo, virtualVariantEnvironmentCache, package)
        self.PackageDefineCommands = _BuildClangTidyPackageDefines(log, localVariantInfo, package)

        self.AllFiles.sort(key=lambda s: s.ResolvedPath.upper())
        self.IncludePaths.sort()
        self.PackageDefineCommands.sort()


def _RunClangTidy(log: Log, toolConfig: ToolConfig, platformId: str,
                  performClangTidyConfig: PerformClangTidyConfig,
                  clangExeInfo: ClangExeInfo,
                  package: Package,
                  filteredFiles: Optional[List[str]],
                  clangFormatFilename: Optional[str],
                  localVariantInfo: LocalVariantInfo,
                  virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache,
                  logOutput: bool = False) -> None:
    if package.AbsolutePath is None or package.ResolvedBuildAllIncludeFiles is None or package.ResolvedBuildSourceFiles is None:
        raise Exception("invalid package")

    log.LogPrint("- {0}".format(package.Name))

    tidyPlatformConfig = TidyPlatformConfig(log, platformId, performClangTidyConfig.ClangTidyConfiguration, virtualVariantEnvironmentCache)
    tidyPackageConfig = TidyPackageConfig(log, package, filteredFiles, performClangTidyConfig.ClangTidyConfiguration, localVariantInfo, virtualVariantEnvironmentCache)

    cmd = clangExeInfo.Command
    buildCommand = [cmd]
    if performClangTidyConfig.Repair:
        buildCommand.append('-fix')

    #buildCommand.append('-fix-errors')

    #buildCommand.append('-header-filter=.*')

    usingCheckCommand = False
    if len(performClangTidyConfig.OverrideChecks) > 0:
        newOverrideChecks = ",".join(performClangTidyConfig.OverrideChecks)
        log.LogPrintVerbose(2, "Overriding checks checks '{0}'".format(newOverrideChecks))
        if performClangTidyConfig.Profile == ClangTidyProfile.Strict:
            log.DoPrintWarning("Ignoring strict checks because 'override' is enabled")
        buildCommand.append("--checks")
        buildCommand.append(newOverrideChecks)
        usingCheckCommand = True
    elif performClangTidyConfig.Profile == ClangTidyProfile.Strict and len(tidyPlatformConfig.PlatformStrictChecks) > 0:
        newStrictChecks = ",".join(tidyPlatformConfig.PlatformStrictChecks)
        log.LogPrintVerbose(2, "Adding strict checks '{0}'".format(newStrictChecks))
        buildCommand.append("--checks")
        buildCommand.append(newStrictChecks)
        usingCheckCommand = True

    if len(performClangTidyConfig.AdditionalUserArguments) > 0:
        log.LogPrintVerbose(2, "Adding user supplied arguments before '--' {0}".format(performClangTidyConfig.AdditionalUserArguments))
        if usingCheckCommand and '--checks' in performClangTidyConfig.AdditionalUserArguments:
            log.DoPrintWarning("another command is adding '--checks' so it could conflict with the user supplied argument")
        buildCommand += performClangTidyConfig.AdditionalUserArguments

    buildCommand += [resolvedPath.SourcePath for resolvedPath in tidyPackageConfig.AllFiles]

    buildCommand.append('--')

    if len(tidyPlatformConfig.PlatformCompilerFlags) > 0:
        buildCommand += tidyPlatformConfig.PlatformCompilerFlags
    if len(tidyPlatformConfig.PlatformDefineCommands) > 0:
        buildCommand += _AddCmdToEachEntry("-D", tidyPlatformConfig.PlatformDefineCommands)

    if len(tidyPackageConfig.IncludePaths) > 0:
        buildCommand += _AddCmdToEachEntry('-I', tidyPackageConfig.IncludePaths)
    if len(tidyPackageConfig.PackageDefineCommands) > 0:
        buildCommand += _AddCmdToEachEntry("-D", tidyPackageConfig.PackageDefineCommands)

    if len(performClangTidyConfig.PostfixArguments) > 0:
        log.LogPrintVerbose(2, "Adding user supplied arguments after '--' {0}".format(performClangTidyConfig.PostfixArguments))
        buildCommand += performClangTidyConfig.PostfixArguments

    currentWorkingDirectory = package.AbsolutePath

    if clangFormatFilename is not None:
        FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, clangFormatFilename)
    FileFinder.FindClosestFileInRoot(log, toolConfig, currentWorkingDirectory, performClangTidyConfig.ClangTidyConfiguration.CustomTidyFile)



    try:
        if log.Verbosity >= 4:
            log.LogPrint("Running command '{0}' in cwd: {1}".format(buildCommand, currentWorkingDirectory))
        result = RunHelper.RunNow(log, buildCommand, currentWorkingDirectory, logOutput)
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
            virtualVariantEnvironmentCache = VirtualVariantEnvironmentCache(log, pythonScriptRoot, False)
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
            virtualVariantEnvironmentCache = VirtualVariantEnvironmentCache(mainLog, pythonScriptRoot, False)
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

class PackageOutputFolder(object):
    def __init__(self, package: Package, outputFolder: str) -> None:
        self.Package = package
        self.OutputFolder = outputFolder

class PerformClangTidyHelper2(object):
    RULE_COMPILE = "compile"
    RULE_TIDY = "tidy"
    VAR_PACKAGE_DEFINES = "PACKAGE_DEFINES"
    VAR_PLATFORM_DEFINES = "PLATFORM_DEFINES"
    VAR_FLAGS = "FLAGS"
    VAR_INCLUDES = "INCLUDES"
    VAR_POSTFIX_ARGS = "POSTFIX_ARGS"
    VAR_YAML_FILE = "YAML_FILE"
    VAR_CUSTOM_CHECKS = "CUSTOM_CHECKS"

    @staticmethod
    def ProcessAllPackages(log: Log, toolConfig: ToolConfig, platformId: str, pythonScriptRoot: str,
                           performClangTidyConfig: PerformClangTidyConfig,
                           clangExeInfo: ClangExeInfo, clangTidyExeInfo: ClangExeInfo, clangTidyApplyReplacementsExeInfo: ClangExeInfo,
                           ninjaExeInfo: ClangExeInfo, packageList: List[Package], cmakeConfig: GeneratorCMakeConfig,
                           customPackageFileFilter: Optional[CustomPackageFileFilter],
                           localVariantInfo: LocalVariantInfo,
                           numBuildThreads: int) -> int:

        virtualVariantEnvironmentCache = VirtualVariantEnvironmentCache(log, pythonScriptRoot, performClangTidyConfig.AllowDynamicVariantCache)

        logOutput = False

        # we sort the package list for consistency in the generated ninja file (and ninja resolves dependencies so it will run the same way no matter what order we write)
        sortedPackageList = list(packageList)
        sortedPackageList.sort(key=lambda s: s.Name.upper())

        currentWorkingDirectory = BuildUtil.GetBuildDir(log, toolConfig, cmakeConfig.CheckDir)

        # default to different folders depending on the options set
        tidyFolder = "tidy" if performClangTidyConfig.Profile == ClangTidyProfile.Strict else "tidyF"
        if len(performClangTidyConfig.OverrideChecks) > 0 or len(performClangTidyConfig.PostfixArguments) > 0 or len(performClangTidyConfig.AdditionalUserArguments) > 0:
            tidyFolder = "tidyC"

        clangTidyFixOutputFolder = IOUtil.Join(currentWorkingDirectory, tidyFolder)
        currentWorkingDirectory = clangTidyFixOutputFolder
        ninjaOutputFile =  IOUtil.Join(clangTidyFixOutputFolder, "build.ninja")
        toolVersionOutputFile =  IOUtil.Join(clangTidyFixOutputFolder, "ToolVersions.txt")
        IOUtil.SafeMakeDirs(currentWorkingDirectory)

        log.LogPrint("Using path: '{0}'".format(currentWorkingDirectory))

        # since clang-apply-replacements doesnt 'remove' the applied fixes we need to delete the yaml files :(
        useWorkAround = True
        if useWorkAround:
            foundFixes = PerformClangTidyHelper2.FindFixesSimple(log, clangTidyFixOutputFolder)
            PerformClangTidyHelper2.DeleteFixes(log, clangTidyFixOutputFolder, foundFixes)

        log.LogPrint("Storing tool versions.")
        PerformClangTidyHelper2.WriteToolVersionFile(log, toolVersionOutputFile, clangExeInfo, clangTidyExeInfo, ninjaExeInfo)

        log.LogPrint("Generating ninja tidy file.")
        totalProcessedCount, outputFolders = PerformClangTidyHelper2.GenerateNinjaTidyFile(log, ninjaOutputFile, clangTidyFixOutputFolder,
                                                                                           toolVersionOutputFile, toolConfig, platformId,
                                                                                           performClangTidyConfig, clangExeInfo, clangTidyExeInfo,
                                                                                           sortedPackageList, customPackageFileFilter,
                                                                                           localVariantInfo, virtualVariantEnvironmentCache)
        log.LogPrint("Executing ninja tidy file.")

        RunHelper.RunNinja(log, ninjaExeInfo, ninjaOutputFile, currentWorkingDirectory, numBuildThreads, logOutput)

        log.LogPrint("Checking for tidy fixes.")

        # Since we delete all yaml files on startup we dont need the more advanced version
        unusedFixes = [] # type: List[str]
        if useWorkAround:
            foundFixes = PerformClangTidyHelper2.FindFixesSimple(log, clangTidyFixOutputFolder)
        else:
            foundFixes, unusedFixes = PerformClangTidyHelper2.FindFixes(log, clangTidyFixOutputFolder, outputFolders)

        if len(foundFixes) > 0:
            log.LogPrint("Applying tidy {0} fixes.".format(len(foundFixes)))
            if not useWorkAround:
                # we delete the unused fixes as they might be from a earlier run on a different package set
                PerformClangTidyHelper2.DeleteFixes(log, clangTidyFixOutputFolder, unusedFixes)
            PerformClangTidyHelper2.ApplyFixes(log, clangTidyApplyReplacementsExeInfo, clangTidyFixOutputFolder, foundFixes,
                                               currentWorkingDirectory, logOutput)
            # since clang-apply-replacements doesnt 'remove' the applied fixes we need to delete the yaml files :(
            # but we do that on launch
            #if useWorkAround:
            #    PerformClangTidyHelper2.DeleteFixes(log, clangTidyFixOutputFolder, foundFixes)
        else:
            log.LogPrint("No fixes found")

        return totalProcessedCount

    @staticmethod
    def WriteToolVersionFile(log: Log, outputFile: str, clangExeInfo: ClangExeInfo, clangTidyExeInfo: ClangExeInfo,
                             ninjaExeInfo: ClangExeInfo) -> None:
        content = "Tool versions\n"
        content += "Clang: {0}\n".format(clangExeInfo.Version)
        content += "ClangTidy: {0}\n".format(clangTidyExeInfo.Version)
        content += "Ninja: {0}\n".format(ninjaExeInfo.Version)
        IOUtil.WriteFileIfChanged(outputFile, content)

    @staticmethod
    def DeleteFixes(log: Log, clangTidyFixOutputFolder: str, fixFiles: List[str]) -> None:
        if not clangTidyFixOutputFolder.endswith('/'):
            clangTidyFixOutputFolder += "/"
        for fileEntry in fixFiles:
            if fileEntry.startswith(clangTidyFixOutputFolder):
                if IOUtil.IsFile(fileEntry):
                    log.LogPrint("Removing file '{0}'".format(fileEntry))
                    IOUtil.RemoveFile(fileEntry)
            else:
                log.DoPrintWarning("file '{0}' not at expected location '{1}'".format(fileEntry, clangTidyFixOutputFolder))

            # delete the obj file so we recompile the files with fixes the next time we run this
            objFilename = "{0}.obj".format(IOUtil.GetFileNameWithoutExtension(fileEntry))
            dirName = IOUtil.GetDirectoryName(fileEntry)
            objFilename = IOUtil.Join(dirName, objFilename)
            if objFilename.startswith(clangTidyFixOutputFolder):
                if IOUtil.IsFile(objFilename):
                    log.LogPrint("Removing file '{0}'".format(objFilename))
                    IOUtil.RemoveFile(objFilename)
            else:
                log.DoPrintWarning("file '{0}' not at expected location '{1}'".format(objFilename, clangTidyFixOutputFolder))

    @staticmethod
    def ApplyFixes(log: Log, clangTidyApplyReplacementsExeInfo: ClangExeInfo, clangTidyFixOutputFolder: str,
                   fixFiles: List[str], currentWorkingDirectory: str, logOutput: bool) -> None:
        # Dump the current warnings
        if log.Verbosity >= 4:
            for fileEntry in fixFiles:
                log.DoPrint(IOUtil.ReadFile(fileEntry))

        # Apply the fixes
        PerformClangTidyHelper2.RunApplyFixes(log, clangTidyApplyReplacementsExeInfo, clangTidyFixOutputFolder, currentWorkingDirectory, logOutput)



    @staticmethod
    def RunApplyFixes(log: Log, clangTidyApplyReplacementsExeInfo: ClangExeInfo, clangTidyFixOutputFolder: str, currentWorkingDirectory: str, logOutput: bool) -> None:
        buildCommand = [clangTidyApplyReplacementsExeInfo.Command, "--format", "-style=file"]
        buildCommand.append(clangTidyFixOutputFolder)
        try:
            if log.Verbosity >= 4:
                log.LogPrint("Running command '{0}' in cwd: {1}".format(buildCommand, currentWorkingDirectory))
            result = RunHelper.RunNow(log, buildCommand, currentWorkingDirectory, logOutput)
            if result != 0:
                log.LogPrintWarning("The command '{0}' failed with '{1}'. It was run with CWD: '{2}'".format(" ".join(buildCommand), result, currentWorkingDirectory))
                raise ExitException(result)
        except FileNotFoundError:
                log.DoPrintWarning("The command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(" ".join(buildCommand), currentWorkingDirectory))
                raise

    @staticmethod
    def IsValidPath(currentPath: str, pathSet: Set[str]) -> bool:
        for entry in pathSet:
            if currentPath.startswith(entry):
                return True
        return False

    @staticmethod
    def FindFixes(log: Log, clangTidyFixOutputFolder: str, outputFolders: List[PackageOutputFolder]) -> Tuple[List[str], List[str]]:
        outputFolderSet = set()  # type: Set[str]
        for entry in outputFolders:
            if entry.Package.BaseIncludePath is not None:
                outputFolderSet.add(IOUtil.Join(entry.OutputFolder, entry.Package.BaseIncludePath + '/'))
            if entry.Package.BaseSourcePath is not None:
                outputFolderSet.add(IOUtil.Join(entry.OutputFolder, entry.Package.BaseSourcePath + '/'))

        foundYamlFiles = IOUtil.GetFilePaths(clangTidyFixOutputFolder, ".yaml")
        yamlFiles = [] # type: List[str]
        unusedYamlFiles = [] # type: List[str]
        for filename in foundYamlFiles:
            if IOUtil.IsFile(filename) and IOUtil.FileLength(filename) > 0:
                dirName = IOUtil.GetDirectoryName(filename)
                if PerformClangTidyHelper2.IsValidPath(dirName, outputFolderSet):
                    yamlFiles.append(filename)
                else:
                    unusedYamlFiles.append(filename)
        return (yamlFiles, unusedYamlFiles)

    @staticmethod
    def FindFixesSimple(log: Log, clangTidyFixOutputFolder: str) -> List[str]:
        foundYamlFiles = IOUtil.GetFilePaths(clangTidyFixOutputFolder, ".yaml")
        yamlFiles = [] # type: List[str]
        unusedYamlFiles = [] # type: List[str]
        for filename in foundYamlFiles:
            if IOUtil.IsFile(filename) and IOUtil.FileLength(filename) > 0:
                yamlFiles.append(filename)
        return yamlFiles


    @staticmethod
    def GenerateNinjaTidyFile(log: Log, ninjaOutputFile: str, clangTidyFixOutputFolder: str, toolVersionOutputFile: str,
                              toolConfig: ToolConfig, platformId: str, performClangTidyConfig: PerformClangTidyConfig,
                              clangExeInfo: ClangExeInfo, clangTidyExeInfo: ClangExeInfo, packageList: List[Package],
                              customPackageFileFilter: Optional[CustomPackageFileFilter],
                              localVariantInfo: LocalVariantInfo,
                              virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache) -> Tuple[int, List[PackageOutputFolder]]:
        totalProcessedCount = 0

        tidyPlatformConfig = TidyPlatformConfig(log, platformId, performClangTidyConfig.ClangTidyConfiguration, virtualVariantEnvironmentCache)

        usingCheckCommand = False
        customChecks = None # type: Optional[str]
        if len(performClangTidyConfig.OverrideChecks) > 0:
            customChecks = ",".join(performClangTidyConfig.OverrideChecks)
            log.LogPrintVerbose(2, "Overriding checks checks '{0}'".format(customChecks))
            if performClangTidyConfig.Profile == ClangTidyProfile.Strict:
                log.DoPrintWarning("Ignoring strict checks because 'override' is enabled")
            usingCheckCommand = True
        elif performClangTidyConfig.Profile == ClangTidyProfile.Strict and len(tidyPlatformConfig.PlatformStrictChecks) > 0:
            customChecks = ",".join(tidyPlatformConfig.PlatformStrictChecks)
            log.LogPrintVerbose(2, "Adding strict checks '{0}'".format(customChecks))
            usingCheckCommand = True

        if len(performClangTidyConfig.AdditionalUserArguments) > 0:
            if usingCheckCommand and '--checks' in performClangTidyConfig.AdditionalUserArguments:
                log.DoPrintWarning("another command is adding '--checks' so it could conflict with the user supplied argument")

        if len(performClangTidyConfig.PostfixArguments) > 0:
            log.LogPrintVerbose(2, "Adding user supplied arguments after '--' {0}".format(performClangTidyConfig.PostfixArguments))

        availablePackageDict = {}  # Dict[str,Package]
        for package in packageList:
            availablePackageDict[package.Name] = package

        packageOutputFolders = []

        with io.StringIO() as ninjaFile:
            ninjaFile.writelines("ninja_required_version = 1.8\n")

            writer = Writer(ninjaFile, 149)

            emptyStringList = [] # type: List[str]
            writer.variable(key=PerformClangTidyHelper2.VAR_FLAGS, value=tidyPlatformConfig.PlatformCompilerFlags)
            writer.variable(key=PerformClangTidyHelper2.VAR_PLATFORM_DEFINES, value=_AddCmdToEachEntry("-D", tidyPlatformConfig.PlatformDefineCommands))
            writer.variable(key=PerformClangTidyHelper2.VAR_POSTFIX_ARGS, value=performClangTidyConfig.AdditionalUserArguments)
            writer.variable(key=PerformClangTidyHelper2.VAR_PACKAGE_DEFINES, value=emptyStringList)
            writer.variable(key=PerformClangTidyHelper2.VAR_INCLUDES, value=emptyStringList)
            writer.variable(key=PerformClangTidyHelper2.VAR_YAML_FILE, value=emptyStringList)
            writer.variable(key=PerformClangTidyHelper2.VAR_CUSTOM_CHECKS, value=customChecks)

            compilerArguments="${0} ${1} ${2} ${3} ${4}".format(PerformClangTidyHelper2.VAR_FLAGS,
                                                                PerformClangTidyHelper2.VAR_PLATFORM_DEFINES,
                                                                PerformClangTidyHelper2.VAR_INCLUDES,
                                                                PerformClangTidyHelper2.VAR_PACKAGE_DEFINES,
                                                                PerformClangTidyHelper2.VAR_POSTFIX_ARGS)

            writer.rule(name=PerformClangTidyHelper2.RULE_COMPILE,
                        depfile="$out.d",
                        command="{0} -c -x c++ $in -o $out -MD -MF $out.d {1}".format(clangExeInfo.Command, compilerArguments),
                        #command="{0} -c -x c++ $in -o $out -MD -MF $out.d @$out.rsp".format(clangExeInfo.Command),
                        #rspfile="$out.rsp",
                        #rspfile_content=compilerArguments
                        )

            tidyChecks = "" if customChecks is None else "--checks ${0} ".format(PerformClangTidyHelper2.VAR_CUSTOM_CHECKS)
            tidyCommand = "{0} $in --export-fixes=${1} {2}-- {3}".format(clangTidyExeInfo.Command, PerformClangTidyHelper2.VAR_YAML_FILE,
                                                                         tidyChecks, compilerArguments)
            writer.rule(name=PerformClangTidyHelper2.RULE_TIDY,
                        command=tidyCommand,
                        #rspfile="$out.rsp",
                        #rspfile_content="{0}-- {1}".format(tidyChecks, compilerArguments),
                        restat=True)

            toolProjectContextsDict = PackagePathUtil.CreateToolProjectContextsDict(toolConfig.ProjectInfo)
            for package in packageList:
                filteredFiles = None
                if customPackageFileFilter is not None:
                    filteredFiles = customPackageFileFilter.TryLocateFilePatternInPackage(log, package,
                                                                                            performClangTidyConfig.ClangTidyConfiguration.FileExtensions)
                if customPackageFileFilter is None or filteredFiles is not None:
                    totalProcessedCount += 1;
                    packageOutputFolder = PerformClangTidyHelper2.AddPackage(log, toolConfig, toolVersionOutputFile, writer, clangTidyFixOutputFolder,
                                                                             toolProjectContextsDict, platformId, performClangTidyConfig, package,
                                                                             filteredFiles, None, localVariantInfo, virtualVariantEnvironmentCache,
                                                                             availablePackageDict)
                    packageOutputFolders.append(packageOutputFolder)

            # finally we write the ninja file
            IOUtil.WriteFileIfChanged(ninjaOutputFile, ninjaFile.getvalue())

            writer.close()
        return (totalProcessedCount, packageOutputFolders)



    @staticmethod
    def AddPackage(log: Log, toolConfig: ToolConfig, toolVersionOutputFile: str,
                   writer: Writer, clangTidyFixOutputFolder: str, toolProjectContextsDict: Dict[ProjectId, ToolConfigProjectContext],
                   platformId: str, performClangTidyConfig: PerformClangTidyConfig, package: Package,
                   filteredFiles: Optional[List[str]],
                   clangFormatFilename: Optional[str],
                   localVariantInfo: LocalVariantInfo,
                   virtualVariantEnvironmentCache: VirtualVariantEnvironmentCache,
                   availablePackages: Dict[str,Package]) -> PackageOutputFolder:
        if package.AbsolutePath is None or package.ResolvedBuildAllIncludeFiles is None or package.ResolvedBuildSourceFiles is None or package.Path is None:
            raise Exception("invalid package")
        log.LogPrint("- {0}".format(package.Name))

        if clangFormatFilename is not None:
            # Verify that there is a format file for the package
            FileFinder.FindClosestFileInRoot(log, toolConfig, package.Path.AbsoluteDirPath, clangFormatFilename)

        tidyPackageConfig = TidyPackageConfig(log, package, filteredFiles, performClangTidyConfig.ClangTidyConfiguration, localVariantInfo, virtualVariantEnvironmentCache)

        variables = {}
        variables[PerformClangTidyHelper2.VAR_PACKAGE_DEFINES] = _AddCmdToEachEntry("-D", tidyPackageConfig.PackageDefineCommands)
        variables[PerformClangTidyHelper2.VAR_INCLUDES] = _AddCmdToEachEntry("-I", ['"{0}"'.format(includePath) for includePath in tidyPackageConfig.IncludePaths])

        # //build cmake_object_order_depends_target_FslGraphics: phony || cmake_object_order_depends_target_FslBase

        # add a phony target with dependencies
        phonyCompileTargetPackageName = PerformClangTidyHelper2._GeneratePhonyCompileTargetName(package.Name)
        addCompileDirectDependencies = PerformClangTidyHelper2._GenerateCompileDependencies(package, availablePackages)
        writer.build(outputs= phonyCompileTargetPackageName, rule="phony", order_only= addCompileDirectDependencies)

        phonyTargetPackageName = PerformClangTidyHelper2._GeneratePhonyTargetName(package.Name)
        addDirectDependencies = PerformClangTidyHelper2._GenerateDependencies(package, availablePackages)
        addDirectDependencies.append(phonyCompileTargetPackageName)
        writer.build(outputs= phonyTargetPackageName, rule="phony", order_only= addDirectDependencies)

        clangTidyFixOutputFolder = IOUtil.Join(clangTidyFixOutputFolder, PackagePathUtil.GetPackagePath(package, toolProjectContextsDict))
        for fileEntry in tidyPackageConfig.AllFiles:
            compiledOutputFile = "{0}.obj".format(IOUtil.Join(clangTidyFixOutputFolder, fileEntry.SourcePath))
            writer.build(outputs=compiledOutputFile, rule=PerformClangTidyHelper2.RULE_COMPILE, inputs=fileEntry.ResolvedPath,
                         implicit=toolVersionOutputFile, variables=variables, order_only=phonyCompileTargetPackageName)

            variables[PerformClangTidyHelper2.VAR_YAML_FILE] = ["{0}.yaml".format(IOUtil.Join(clangTidyFixOutputFolder, fileEntry.SourcePath))]

            outputFile = "{0}.dmy".format(IOUtil.Join(clangTidyFixOutputFolder, fileEntry.SourcePath))
            # Locate the closest clang tidy configuration file so we can add it as a implicit package dependency
            packageClosestTidyPath = FileFinder.FindClosestFileInRoot(log, toolConfig, fileEntry.ResolvedPath, performClangTidyConfig.ClangTidyConfiguration.CustomTidyFile)
            packageClosestTidyFile = IOUtil.Join(packageClosestTidyPath,performClangTidyConfig.ClangTidyConfiguration.CustomTidyFile)
            implicitDeps = [compiledOutputFile, packageClosestTidyFile, toolVersionOutputFile]

            writer.build(outputs=outputFile, rule=PerformClangTidyHelper2.RULE_TIDY, inputs=fileEntry.ResolvedPath, implicit=implicitDeps, variables=variables, order_only=phonyTargetPackageName)

            # Write a dummy file so ninja finds a file (this is because clang tidy wont generate a fix file unless its needed)
            if not IOUtil.Exists(outputFile):
                parentDir = IOUtil.GetDirectoryName(outputFile)
                if not IOUtil.Exists(parentDir):
                    IOUtil.SafeMakeDirs(parentDir)
                IOUtil.WriteFileIfChanged(outputFile, "")
        return PackageOutputFolder(package, clangTidyFixOutputFolder)


    @staticmethod
    def _GeneratePhonyCompileTargetName(name: str) -> str:
        return "magic_compile_order_dependent_target_{0}".format(name)

    @staticmethod
    def _GenerateCompileDependencies(package: Package, availablePackages: Dict[str,Package]) -> List[str]:
        result = [] # type: List[str]
        for dep in package.ResolvedDirectDependencies:
            if dep.Name in availablePackages:
                result.append(PerformClangTidyHelper2._GeneratePhonyCompileTargetName(dep.Name))
        result.sort()
        return result

    @staticmethod
    def _GeneratePhonyTargetName(name: str) -> str:
        return "magic_order_dependent_target_{0}".format(name)

    @staticmethod
    def _GenerateDependencies(package: Package, availablePackages: Dict[str,Package]) -> List[str]:
        result = [] # type: List[str]
        for dep in package.ResolvedDirectDependencies:
            if dep.Name in availablePackages:
                result.append(PerformClangTidyHelper2._GeneratePhonyTargetName(dep.Name))
        result.sort()
        return result


class PerformClangTidy(object):
    @staticmethod
    def Run(log: Log, toolConfig: ToolConfig, platformId: str,
            topLevelPackage: Package, tidyPackageList: List[Package], userBuildVariantsDict: Dict[str, str],
            pythonScriptRoot: str, generatorContext: GeneratorContext, sdkConfigTemplatePath: str,
            packageRecipeResultManager: PackageRecipeResultManager,
            performClangTidyConfig: PerformClangTidyConfig,
            customPackageFileFilter: Optional[CustomPackageFileFilter],
            clangFormatFilename: Optional[str], buildThreads: int, useLegacyTidyMethod: bool) -> None:
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

        clangExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, performClangTidyConfig.ClangTidyConfiguration.ClangRecipePackageName,
                                                                MagicValues.ClangCompileCommand)

        clangTidyExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, performClangTidyConfig.ClangTidyConfiguration.ClangTidyRecipePackageName,
                                                                MagicValues.ClangTidyCommand)
        clangTidyApplyReplacementsExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager,
                                                                                 performClangTidyConfig.ClangTidyConfiguration.ClangTidyRecipePackageName,
                                                                                 MagicValues.ClangTidyApplyReplacements)

        ninjaExeInfo = PerformClangUtil.LookupRecipeResults(packageRecipeResultManager, performClangTidyConfig.ClangTidyConfiguration.NinjaRecipePackageName,
                                                            MagicValues.NinjaCommand)

        BuildVariantUtil.ValidateUserVariantSettings(log, topLevelPackage, userBuildVariantsDict)
        BuildVariantUtil.LogVariantSettings(log, userBuildVariantsDict)
        resolvedVariantSettingsDict = BuildVariantUtil.CreateCompleteStaticVariantSettings(topLevelPackage.ResolvedAllVariantDict,
                                                                                           userBuildVariantsDict)

        log.LogPrint("Clang version: {0}".format(clangExeInfo.Version))
        log.LogPrint("ClangTidy version: {0}".format(clangTidyExeInfo.Version))
        log.LogPrint("Ninja version: {0}".format(ninjaExeInfo.Version))

        log.LogPrint("Running clang-tidy")

        # Filter the package list so it only contains things we can process
        finalPackageList = [package for package in tidyPackageList if PerformClangUtil.CanProcessPackage(package)]

        generatorConfig = GeneratorConfig(generatorContext.Generator.PlatformName, sdkConfigTemplatePath, toolConfig, buildThreads, CommandType.Build)
        generatorReport = generatorContext.Generator.GenerateReport(log, generatorConfig, finalPackageList)
        generatorReportDict = generatorReport.PackageReportDict

        # Validate report dict
        for package in finalPackageList:
            if package not in generatorReportDict:
                raise Exception("Generator report is missing information for package '{0}'".format(package.Name))

        localVariantInfo = LocalVariantInfo(resolvedVariantSettingsDict, generatorReportDict, pythonScriptRoot)

        if useLegacyTidyMethod:
            log.LogPrint("Using legacy tidy method")
            count = PerformClangTidyHelper.ProcessAllPackages(log, toolConfig, platformId, pythonScriptRoot, performClangTidyConfig,
                                                              clangTidyExeInfo, finalPackageList, customPackageFileFilter,
                                                              localVariantInfo, buildThreads)
        else:
            count = PerformClangTidyHelper2.ProcessAllPackages(log, toolConfig, platformId, pythonScriptRoot, performClangTidyConfig,
                                                               clangExeInfo, clangTidyExeInfo, clangTidyApplyReplacementsExeInfo,
                                                               ninjaExeInfo, finalPackageList, generatorContext.CMakeConfig,
                                                               customPackageFileFilter, localVariantInfo, buildThreads)

        if count == 0:
            if customPackageFileFilter is None:
                log.DoPrintWarning("No files processed")
            else:
                log.DoPrintWarning("No files processed, could not find a package that matches {0}".format(customPackageFileFilter))

