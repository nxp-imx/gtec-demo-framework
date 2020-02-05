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

from typing import List
from typing import Optional
from typing import Tuple
import os
import re
import subprocess
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.PackageRecipeResult import PackageRecipeResult
from FslBuildGen.BuildExternal.PackageRecipeResultFoundExecutable import PackageRecipeResultFoundExecutable
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.DataTypes import BuildRecipeValidateCommand
from FslBuildGen.DataTypes import BuildRecipeValidateMethod
from FslBuildGen.Exceptions import GroupedException
from FslBuildGen.Packages.Package import Package
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Vars.VariableProcessor import VariableProcessor
from FslBuildGen import Util
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeInstallation
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddHeaders
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddLib
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddDLL
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddTool
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandEnvironmentVariable
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandFindFileInPath
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandFindExecutableFileInPath
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandPath

class InstallationStatus(object):
    NotInstalled = 0
    Installed = 1
    # There was a user environment error that caused the validation to fail.
    EnvironmentError = 2
    # No installation validation was available
    Undefined = 3


class ErrorClassification(object):
    # Currently valued according to severity
    NoError = 0
    Help = 1
    Environment = 2
    Critical = 3


class ErrorRecord(object):
    def __init__(self, errorClassification: int, message: str) -> None:
        self.Classification = errorClassification
        self.Message = message

    def ToString(self) -> str:
        return self.Message


class ValidationEngine(object):
    def __init__(self, basicConfig: BasicConfig, variableProcessor: VariableProcessor,
                 packageRecipeResultManager: PackageRecipeResultManager) -> None:
        self.__BasicConfig = basicConfig
        self.__VariableProcessor = variableProcessor
        self.__PackageRecipeResultManager = packageRecipeResultManager


    def Process(self, sourcePackage: Package) -> None:
        packageRecipeResult = self.__PackageRecipeResultManager.AddIfMissing(sourcePackage.Name)

        sourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
        if sourceRecipe is None:
            raise Exception("Can't validate a package '{0}' that doesnt contain a source recipe".format(sourcePackage.Name))

        errorRecordList = []  # type: List[ErrorRecord]
        status = self.__GetInstallationStatus(errorRecordList, sourceRecipe, sourcePackage.ResolvedRecipeVariants, packageRecipeResult)
        if status == InstallationStatus.Installed or status == InstallationStatus.Undefined:
            return

        self.RaiseExceptionFromErrorRecords(sourcePackage, errorRecordList)


    def RaiseExceptionFromErrorRecords(self, sourcePackage: Package, errorRecordList: List[ErrorRecord]) -> None:
        self.__PackageRecipeResultManager.AddIfMissing(sourcePackage.Name)

        sourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
        if sourceRecipe is None:
            raise Exception("Invalid recipe")

        exceptionList = [Exception("Installation validation failed for package '{0}' recipe '{1}'".format(sourcePackage.Name, sourceRecipe.Name))]
        for errorRecord in errorRecordList:
            exceptionList.append(Exception(errorRecord.ToString()))
        raise GroupedException(exceptionList)


    def GetInstallationStatus(self, sourcePackage: Package, rErrorRecordList: Optional[List[ErrorRecord]] = None) -> int:
        packageRecipeResult = self.__PackageRecipeResultManager.AddIfMissing(sourcePackage.Name)

        sourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
        if sourceRecipe is None:
            raise Exception("Can't validate a package '{0}' that doesnt contain a source recipe".format(sourcePackage.Name))

        errorRecordList = [] if rErrorRecordList is None else rErrorRecordList

        status = self.__GetInstallationStatus(errorRecordList, sourceRecipe, sourcePackage.ResolvedRecipeVariants, packageRecipeResult)
        if status == InstallationStatus.Installed or status == InstallationStatus.Undefined:
            return status

        if rErrorRecordList is None:
            self.__BasicConfig.LogPrintWarning("Installation validation failed for package '{0}' recipe '{1}'".format(sourcePackage.Name, sourceRecipe.Name))
            for errorRecord in errorRecordList:
                self.__BasicConfig.LogPrintWarning(errorRecord.ToString())
        return status


    def __ChoseMostSevereClasification(self, errorClassification1: int, errorClassification2: int) -> int:
        return errorClassification1 if errorClassification1 > errorClassification2 else errorClassification2


    def __GetStatusFromErrorRecords(self, errorRecordList: List[ErrorRecord]) -> int:
        errorClassification = ErrorClassification.NoError
        for record in errorRecordList:
            errorClassification = self.__ChoseMostSevereClasification(errorClassification, record.Classification)

        if errorClassification == ErrorClassification.NoError:
            return InstallationStatus.Installed
        elif errorClassification == ErrorClassification.Help:
            return InstallationStatus.Installed
        elif errorClassification == ErrorClassification.Environment:
            return InstallationStatus.EnvironmentError
        elif errorClassification == ErrorClassification.Critical:
            return InstallationStatus.NotInstalled
        raise Exception("Unsupported error classification: {0}".format(errorClassification))


    def __GetInstallationStatus(self, rErrorRecordList: List[ErrorRecord], sourceRecipe: PackageExperimentalRecipe,
                                recipeVariants: List[str], packageRecipeResult: PackageRecipeResult) -> int:
        if sourceRecipe.ValidateInstallation is None:
            self.__BasicConfig.LogPrintVerbose(3, "WARNING: no intallation validation available for recipe '{0}'".format(sourceRecipe.Name))
            return InstallationStatus.Undefined

        if self.__BasicConfig.Verbosity >= 1:
            if len(sourceRecipe.ValidateInstallation.CommandList) > 0:
                self.__BasicConfig.LogPrint("Validating installation for '{0}'".format(sourceRecipe.Name))
            else:
                self.__BasicConfig.LogPrint("WARNING: No installation validation commands available for '{0}'".format(sourceRecipe.Name))

        self.__DoValidateInstallation(rErrorRecordList, sourceRecipe, recipeVariants, packageRecipeResult)

        return self.__GetStatusFromErrorRecords(rErrorRecordList)


    def __DoValidateInstallation(self, rErrorRecordList: List[ErrorRecord], sourceRecipe: PackageExperimentalRecipe,
                                 recipeVariants: List[str], packageRecipeResult: PackageRecipeResult) -> None:
        self.__BasicConfig.PushIndent()
        try:
            installationPath = sourceRecipe.ResolvedInstallLocation.ResolvedPath if sourceRecipe.ResolvedInstallLocation is not None else None
            validateInstallation = sourceRecipe.ValidateInstallation
            if validateInstallation is None:
                raise Exception("Invalid recipe")

            if len(recipeVariants) <= 0:
                self.__DoValidate(rErrorRecordList, installationPath, validateInstallation, packageRecipeResult)
            else:
                for entry in recipeVariants:
                    installationPathCopy = IOUtil.Join(installationPath, entry) if installationPath is not None else None
                    self.__BasicConfig.LogPrint("Recipe variant: {0}".format(entry))
                    self.__BasicConfig.PushIndent()
                    try:
                        self.__DoValidate(rErrorRecordList, installationPathCopy, validateInstallation, packageRecipeResult)
                    finally:
                        self.__BasicConfig.PopIndent()
        finally:
            self.__BasicConfig.PopIndent()

    def __DoValidate(self, rErrorRecordList: List[ErrorRecord], installationPath: Optional[str],
                     validateInstallation: XmlRecipeInstallation, packageRecipeResult: PackageRecipeResult) -> None:
        for command in validateInstallation.CommandList:
            result = False
            if command.CommandType == BuildRecipeValidateCommand.EnvironmentVariable:
                if not isinstance(command, XmlRecipeValidateCommandEnvironmentVariable):
                    raise Exception("Invalid command")
                result = self.__ValidateEnvironmentVariable(rErrorRecordList, command)
            elif command.CommandType == BuildRecipeValidateCommand.Path:
                if not isinstance(command, XmlRecipeValidateCommandPath):
                    raise Exception("Invalid command")
                result = self.__ValidatePath(rErrorRecordList, installationPath, command)
            elif command.CommandType == BuildRecipeValidateCommand.FindFileInPath:
                if not isinstance(command, XmlRecipeValidateCommandFindFileInPath):
                    raise Exception("Invalid command")
                result = self.__ValidateFindFileInPath(rErrorRecordList, installationPath, command)
            elif command.CommandType == BuildRecipeValidateCommand.FindExecutableFileInPath:
                if not isinstance(command, XmlRecipeValidateCommandFindExecutableFileInPath):
                    raise Exception("Invalid command")
                result = self.__ValidateFindExecutableFileInPath(rErrorRecordList, installationPath, command, packageRecipeResult)
            elif command.CommandType == BuildRecipeValidateCommand.AddHeaders:
                if not isinstance(command, XmlRecipeValidateCommandAddHeaders):
                    raise Exception("Invalid command")
                result = self.__ValidateAddHeaders(rErrorRecordList, installationPath, command)
            elif command.CommandType == BuildRecipeValidateCommand.AddLib:
                if not isinstance(command, XmlRecipeValidateCommandAddLib):
                    raise Exception("Invalid command")
                result = self.__ValidateAddLib(rErrorRecordList, installationPath, command)
            elif command.CommandType == BuildRecipeValidateCommand.AddDLL:
                if not isinstance(command, XmlRecipeValidateCommandAddDLL):
                    raise Exception("Invalid command")
                result = self.__ValidateAddDLL(rErrorRecordList, installationPath, command)
            elif command.CommandType == BuildRecipeValidateCommand.AddTool:
                if not isinstance(command, XmlRecipeValidateCommandAddTool):
                    raise Exception("Invalid command")
                result = self.__ValidateAddTool(rErrorRecordList, installationPath, command)
            else:
                raise Exception("Unknown validation command '{0}'".format(command.CommandName))
            if not result and not command.Help is None:
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Help, "  {0}".format(command.Help)))


    def __ValidateEnvironmentVariable(self, rErrorRecordList: List[ErrorRecord], command: XmlRecipeValidateCommandEnvironmentVariable) -> bool:
        result, value = self.__DoValidateEnvironmentVariable(rErrorRecordList, command)
        if self.__BasicConfig.Verbosity >= 1:
            self.__BasicConfig.LogPrint("Validating environment variable '{0}' '{1}': {2}".format(command.Name, BuildRecipeValidateMethod.TryToString(command.Method, True), result))
            if self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        return result


    def __ValidatePath(self, rErrorRecordList: List[ErrorRecord],
                       installationPath: Optional[str],
                       command: XmlRecipeValidateCommandPath) -> bool:
        result, value = self.__DoValidatePath(rErrorRecordList, installationPath, command)
        if self.__BasicConfig.Verbosity >= 1:
            self.__BasicConfig.LogPrint("Validating path '{0}' '{1}': {2}".format(command.Name, BuildRecipeValidateMethod.TryToString(command.Method, True), result))
            if self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        return result


    def __ValidateFindFileInPath(self, rErrorRecordList: List[ErrorRecord],
                                 installationPath: Optional[str],
                                 command: XmlRecipeValidateCommandFindFileInPath) -> bool:
        result, value = self.__TryFindFileInPath(rErrorRecordList, installationPath, command.Name, command.ExpectedPath)
        if self.__BasicConfig.Verbosity >= 1:
            if command.ExpectedPath is None:
                self.__BasicConfig.LogPrint("Validating file '{0}' is in path: {1}".format(command.Name, result))
            else:
                self.__BasicConfig.LogPrint("Validating file '{0}' is in path at '{1}': {2}".format(command.Name, command.ExpectedPath, result))
            if not value is None and self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        return result

    def __CheckVersion(self, minVersionList: List[int], foundVersionList: List[int]) -> bool:
        for idx, val in enumerate(minVersionList):
            if idx > len(foundVersionList):
                return idx > 0
            if val < foundVersionList[idx]:
                return True
            if val > foundVersionList[idx]:
                return False
        return True

    def __TryValidateCommandVersion(self, cmd: str, versionCommand: str, versionRegEx: str, minVersion: str) -> bool:
        output = ""
        try:
            runCmd = [cmd, versionCommand]
            with subprocess.Popen(runCmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, universal_newlines=True) as proc:
                output = proc.stdout.read().strip()
                proc.stdout.close()
                result = proc.wait()
                if result != 0:
                    self.__BasicConfig.LogPrintWarning("The command '{0}' failed with '{1}'".format(" ".join(runCmd), result))
                    return False
        except FileNotFoundError:
                self.__BasicConfig.DoPrintWarning("The command '{0}' failed with 'file not found'.".format(" ".join(runCmd)))
                return False

        match = re.search(versionRegEx, output)
        if match is None:
            self.__BasicConfig.DoPrintWarning("The regex '{0}' did not capture the version".format(versionRegEx))
            return False
        if len(match.groups()) > 1:
            self.__BasicConfig.DoPrintWarning("The regex '{0}' captured more than one group: {1}".format(versionRegEx, match.groups))
        if len(match.groups()) != 1:
            self.__BasicConfig.DoPrintWarning("The regex '{0}' did not capture a group with version information".format(versionRegEx))
        matchResult = match.group(1)
        try:
            # time to parse the version string
            foundVersionList = Util.ParseVersionString(matchResult, maxValues=4)
            minVersionList = Util.ParseVersionString(minVersion, maxValues=4)
            if len(minVersionList) > len(foundVersionList):
                self.__BasicConfig.DoPrintWarning("The regex '{0}' did not capture the enough version number elements to compare against the min version '{1}'".format(versionRegEx, minVersion))
                return False
            if not self.__CheckVersion(minVersionList, foundVersionList):
                return False
            self.__BasicConfig.LogPrint("  Found version {0} expected minVersion {1}".format(matchResult, minVersion))
        except Exception as ex:
            self.__BasicConfig.DoPrintWarning("Failed to parse version string: {0}".format(str(ex)))
            return False
        return True


    def __ValidateFindExecutableFileInPath(self, rErrorRecordList: List[ErrorRecord],
                                           installationPath: Optional[str],
                                           command: XmlRecipeValidateCommandFindExecutableFileInPath,
                                           packageRecipeResult: PackageRecipeResult) -> bool:
        # Patch filename with the platform dependent name
        alternatives = [command.Name]
        if len(command.Alternatives) > 0:
            alternatives += command.Alternatives
        retry = True
        currentCommandName = ""
        newErrors = [] # type: List[ErrorRecord]
        while retry and len(alternatives) > 0:
            currentCommandName = alternatives[0]
            retry = False
            platformFilename = PlatformUtil.GetPlatformDependentExecuteableName(currentCommandName, PlatformUtil.DetectBuildPlatformType())

            result, value = self.__TryFindFileInPath(newErrors, installationPath, platformFilename, command.ExpectedPath)

            if result and value is not None and command.MinVersion is not None:
                if command.VersionCommand is None or command.VersionRegEx is None or command.MinVersion is None:
                    raise Exception("Internal error")
                result = self.__TryValidateCommandVersion(value, command.VersionCommand, command.VersionRegEx, command.MinVersion)

            # Cache information about what we discovered
            if result and value is not None:
                exeRecord = PackageRecipeResultFoundExecutable(command.Name, currentCommandName, value)
                packageRecipeResult.AddFoundExecutable(exeRecord)
            elif len(alternatives) > 0:
                alternatives = alternatives[1:]
                retry = True

        if self.__BasicConfig.Verbosity >= 1:
            # On failure we show the 'most' common name
            if result or (len(command.Alternatives) < 0):
                if command.ExpectedPath is None:
                    self.__BasicConfig.LogPrint("Validating executable file '{0}' is in path: {1}".format(currentCommandName, result))
                else:
                    self.__BasicConfig.LogPrint("Validating executable file '{0}' is in path at '{1}': {2}".format(currentCommandName, command.ExpectedPath, result))
            else:
                allAlternatives = [command.Name]
                if command.Alternatives is not None:
                    allAlternatives += command.Alternatives
                if command.ExpectedPath is None:
                    self.__BasicConfig.LogPrint("Validating one of these executable files '{0}' is in path: {1}".format(", ".join(allAlternatives), result))
                else:
                    self.__BasicConfig.LogPrint("Validating one of these executable files '{0}' is in path at '{1}': {2}".format(", ".join(allAlternatives), command.ExpectedPath, result))
            if not value is None and self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))

        if not result:
            rErrorRecordList += newErrors

        return result


    def __ValidateAddHeaders(self, rErrorRecordList: List[ErrorRecord],
                             installationPath: Optional[str],
                             command: XmlRecipeValidateCommandAddHeaders) -> bool:
        result, value = self.__DoValidateAddHeaders(rErrorRecordList, installationPath, command)
        if self.__BasicConfig.Verbosity >= 1:
            self.__BasicConfig.LogPrint("Validating AddHeaders '{0}': {1}".format(command.Name, result))
            if self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        return result


    def __ValidateAddLib(self, rErrorRecordList: List[ErrorRecord],
                         installationPath: Optional[str],
                         command: XmlRecipeValidateCommandAddLib) -> bool:
        result, value = self.__DoValidateFile(rErrorRecordList, installationPath, command.Name, False)
        if self.__BasicConfig.Verbosity >= 1:
            self.__BasicConfig.LogPrint("Validating AddLib '{0}': {1}".format(command.Name, result))
            if self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        result2 = True
        if command.Name != command.DebugName:
            result2, value2 = self.__DoValidateFile(rErrorRecordList, installationPath, command.DebugName, False)
            if self.__BasicConfig.Verbosity >= 1:
                self.__BasicConfig.LogPrint("Validating AddLib (Debug) '{0}': {1}".format(command.DebugName, result2))
                if self.__BasicConfig.Verbosity >= 2:
                    self.__BasicConfig.LogPrint("  '{0}'".format(value2))
        return result and result2


    def __ValidateAddDLL(self, rErrorRecordList: List[ErrorRecord],
                         installationPath: Optional[str],
                         command: XmlRecipeValidateCommandAddDLL) -> bool:
        result, value = self.__DoValidateFile(rErrorRecordList, installationPath, command.Name, False)
        if self.__BasicConfig.Verbosity >= 1:
            self.__BasicConfig.LogPrint("Validating AddDLL '{0}': {1}".format(command.Name, result))
            if self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        result2 = True
        if command.Name != command.DebugName:
            result2, value2 = self.__DoValidateFile(rErrorRecordList, installationPath, command.DebugName, False)
            if self.__BasicConfig.Verbosity >= 1:
                self.__BasicConfig.LogPrint("Validating AddDLL (Debug) '{0}': {1}".format(command.DebugName, result2))
                if self.__BasicConfig.Verbosity >= 2:
                    self.__BasicConfig.LogPrint("  '{0}'".format(value2))
        return result and result2



    def __ValidateAddTool(self, rErrorRecordList: List[ErrorRecord],
                         installationPath: Optional[str],
                         command: XmlRecipeValidateCommandAddTool) -> bool:
        toolName = PlatformUtil.GetPlatformDependentExecuteableName(command.Name, PlatformUtil.DetectBuildPlatformType())
        result, value = self.__DoValidateFile(rErrorRecordList, installationPath, toolName, False)
        if self.__BasicConfig.Verbosity >= 1:
            self.__BasicConfig.LogPrint("Validating AddTool '{0}': {1}".format(command.Name, result))
            if self.__BasicConfig.Verbosity >= 2:
                self.__BasicConfig.LogPrint("  '{0}'".format(value))
        return result


    def __DoValidateEnvironmentVariable(self, rErrorRecordList: List[ErrorRecord],
                                        command: XmlRecipeValidateCommandEnvironmentVariable) -> Tuple[bool, Optional[str]]:
        value = os.environ.get(command.Name)
        if not value:
            rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Environment variable '{0}' is not defined, please define it as required.".format(command.Name)))
            return False, value

        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("ValidateEnvironmentVariable: '{0}'='{1}'".format(command.Name, value))

        if command.Method == BuildRecipeValidateMethod.IsDirectory:
            if not IOUtil.IsDirectory(value):
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Environment variable '{0}' contained '{1}' which is not a directory.".format(command.Name, value)))
                return False, value
        elif command.Method == BuildRecipeValidateMethod.IsFile:
            if not IOUtil.IsFile(value):
                fileHelp = self.__GetFailedFileCheckExtraHelpString(value)
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Environment variable '{0}' contained '{1}' which is not a file.{2}".format(command.Name, value, fileHelp)))
                return False, value
        elif command.Method == BuildRecipeValidateMethod.Exists:
            if not IOUtil.Exists(value):
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Environment variable '{0}' contained '{1}' which is a path that dont exist.".format(command.Name, value)))
                return False, value
        else:
            raise Exception("Unsupported BuildRecipeValidateMethod '{0}'".format(command.Method))


        if not command.AllowEndSlash  and (value.endswith('/') or value.endswith('\\')):
            rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Environment variable '{0}' content '{1}' can not end with a slash '/' or backslash '\\'.".format(command.Name, value)))
            return False, value

        return True, IOUtil.NormalizePath(value)


    def __DoValidatePath(self, rErrorRecordList: List[ErrorRecord],
                         installationPath: Optional[str],
                         command: XmlRecipeValidateCommandPath) -> Tuple[bool, Optional[str]]:
        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("ValidatePath '{0}'".format(command.Name))

        result, path = self.__TryResolvePath(rErrorRecordList, installationPath, command.Name)
        if not result or path is None:
            return False, path

        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("Resolving to '{0}'".format(path))

        if command.Method == BuildRecipeValidateMethod.IsDirectory:
            if not IOUtil.IsDirectory(path):
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Critical, "Path '{0}' resolved to '{1}' which is not a directory.".format(command.Name, path)))
                return False, path
        elif command.Method == BuildRecipeValidateMethod.IsFile:
            if not IOUtil.IsFile(path):
                fileHelp = self.__GetFailedFileCheckExtraHelpString(path)
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Critical, "Path '{0}' resolved to '{1}' which is not a file.{2}".format(command.Name, path, fileHelp)))
                return False, path
        elif command.Method == BuildRecipeValidateMethod.Exists:
            if not IOUtil.Exists(path):
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Critical, "Path '{0}' resolved to '{1}' which is a path that dont exist.".format(command.Name, path)))
                return False, path
        else:
            raise Exception("Unsupported BuildRecipeValidateMethod '{0}'".format(command.Method))
        return True, path


    def __TryFindFileInPath(self, rErrorRecordList: List[ErrorRecord],
                            installationPath: Optional[str],
                            commandFileName: str,
                            commandExpectedPath: Optional[str]) -> Tuple[bool, Optional[str]]:
        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("FindFileInPath: '{0}'".format(commandFileName))

        path = IOUtil.TryFindFileInPath(commandFileName)
        if path is None:
            rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "File '{0}' could not be found in path.".format(commandFileName)))
            return False, path

        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("Found at '{0}'".format(path))

        path = IOUtil.NormalizePath(path)

        if commandExpectedPath is not None:
            result, expectedPath = self.__TryResolvePath(rErrorRecordList, installationPath, commandExpectedPath)
            if not result or expectedPath is None:
                return False, path
            expectedPath = IOUtil.Join(expectedPath, commandFileName)
            if path != expectedPath:
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "File '{0}' was not found at the expected path '{1}' which resolves to '{2}' but at '{3}' instead".format(commandFileName, commandExpectedPath, expectedPath, path)))
                if expectedPath.lower() == path.lower():
                    rErrorRecordList.append(ErrorRecord(ErrorClassification.Help, "Please beware that '{0}'=='{1}' if you ignore character case".format(expectedPath, path)))
                return False, path

        return True, path


    def __DoValidateAddHeaders(self, rErrorRecordList: List[ErrorRecord],
                               installationPath: Optional[str],
                               command: XmlRecipeValidateCommandAddHeaders) -> Tuple[bool, Optional[str]]:
        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("ValidateAddHeaders '{0}'".format(command.Name))

        result, path = self.__TryResolvePath(rErrorRecordList, installationPath, command.Name, False)
        if not result or path is None:
            return False, path

        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("Resolving to '{0}'".format(path))

        if not IOUtil.IsDirectory(path):
            rErrorRecordList.append(ErrorRecord(ErrorClassification.Critical, "Path '{0}' resolved to '{1}' which is not a directory.".format(command.Name, path)))
            return False, path
        return True, path


    def __DoValidateFile(self, rErrorRecordList: List[ErrorRecord],
                         installationPath: Optional[str],
                         filename: str,
                         allowEnvironmentVariable: bool) -> Tuple[bool, Optional[str]]:
        result, path = self.__TryResolvePath(rErrorRecordList, installationPath, filename, allowEnvironmentVariable)
        if not result or path is None:
            return False, path

        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("Resolving to '{0}'".format(path))

        if not IOUtil.IsFile(path):
            fileHelp = self.__GetFailedFileCheckExtraHelpString(path)
            rErrorRecordList.append(ErrorRecord(ErrorClassification.Critical, "Path '{0}' resolved to '{1}' which is not a file.{2}".format(filename, path, fileHelp)))
            return False, path
        return True, path


    def __TryResolvePath(self, rErrorRecordList: List[ErrorRecord],
                         installationPath: Optional[str],
                         sourcePath: str,
                         allowEnvironmentVariable: bool = True) -> Tuple[bool, Optional[str]]:
        if not allowEnvironmentVariable:
            if installationPath is None:
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Path '{0}' tried to use a undefined install path. Is the recipe missing a Pipeline section, ExternalInstallDirectory or is the supplied path missing a environment variable to qualify it?".format(sourcePath)))
                return False, None
            return True, IOUtil.Join(installationPath, sourcePath)

        # NOTE: workaround Union of tuples not being iterable bug in mypy https://github.com/python/mypy/issues/1575
        tupleResult = self.__VariableProcessor.TrySplitLeadingEnvironmentVariablesNameAndPath(sourcePath)
        environmentVariableName = tupleResult[0]
        restPath = tupleResult[1]
        if environmentVariableName is None:
            if installationPath is None:
                rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Path '{0}' tried to use a undefined install path. Is the recipe missing a Pipeline section, ExternalInstallDirectory or is the supplied path missing a environment variable to qualify it?".format(sourcePath)))
                return False, None
            return True, IOUtil.Join(installationPath, sourcePath)
        restPath = restPath if restPath is not None else ""

        value = os.environ.get(environmentVariableName)
        if value is None:
            rErrorRecordList.append(ErrorRecord(ErrorClassification.Environment, "Path '{0}' contained environment variable '{1}' which is not defined.".format(sourcePath, environmentVariableName)))
            return False, None
        path = IOUtil.Join(value, restPath)
        if self.__BasicConfig.Verbosity >= 4:
            self.__BasicConfig.LogPrint("Path: '{0}' using environment variable '{1}'='{2}' resolving to '{3}'".format(sourcePath, environmentVariableName, value, path))
        return True, path


    def __GetFailedFileCheckExtraHelpString(self, sourcePath: str) -> str:
        directoryName = IOUtil.GetDirectoryName(sourcePath)
        if not directoryName or IOUtil.IsDirectory(directoryName):
            return ""
        #filename = IOUtil.GetDirectoryName(sourcePath)
        return " The parent directory '{0}' did not exist either.".format(directoryName)
