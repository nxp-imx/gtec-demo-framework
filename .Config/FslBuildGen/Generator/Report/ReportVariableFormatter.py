#!/usr/bin/env python3

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

from typing import Callable
from typing import Dict
from typing import List
from typing import Optional
from typing import Tuple
from typing import Union
import itertools
import os
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.Log import Log
from FslBuildGen.Generator.Report.Datatypes import FormatStringEnvironmentVariableResolveMethod
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import InvalidVariableOptionNameException
from FslBuildGen.Generator.Report.ParsedFormatString import FormatStringEnvironmentVariableResolver
from FslBuildGen.Generator.Report.ParsedFormatString import FormatStringUndefinedVariableNameException
from FslBuildGen.Generator.Report.ParsedFormatString import LookupEnvironmentVariableCommand
from FslBuildGen.Generator.Report.ParsedFormatString import LookupVariableCommand
from FslBuildGen.Generator.Report.ParsedFormatString import ParsedFormatString

def GetLinkedCommandList(linkedVariables: List[LookupVariableCommand],
                         sourceDict: Dict[str, LookupVariableCommand]) -> Dict[LookupVariableCommand, List[LookupVariableCommand]]:
    linkedCommandDict = {}  # type: Dict[LookupVariableCommand, List[LookupVariableCommand]]
    for entry in linkedVariables:
        if entry.Report.LinkTargetName is None:
            raise Exception("entry.Report.LinkTargetName can not be None")
        # we assume the master is present in the sourceDict!
        master = sourceDict[entry.Report.LinkTargetName]
        if master in linkedCommandDict:
            linkedCommandDict[master].append(entry)
        else:
            linkedCommandDict[master] = [entry]
    return linkedCommandDict


def CreateLookupDict(varCommandList: List[LookupVariableCommand],
                     generatorVariableReport: GeneratorVariableReport) -> Tuple[Dict[str, LookupVariableCommand], List[LookupVariableCommand]]:
    """ Returns
        - Dict -> which is a lookup dict that maps variable names to their lookup command
        - List -> a list of all LookupVariableCommand that are linked to something
    """
    resultDict = {} # type: Dict[str, LookupVariableCommand]
    linkedCommands = [] # type: List[LookupVariableCommand]
    for varCommand in varCommandList:
        resultDict[varCommand.Name] = varCommand
        if varCommand.Report.LinkTargetName is not None:
            linkedCommands.append(varCommand)

    # Check if all linked commands have a 'command' to chain to, if not create a virtual one
    for varCommand in linkedCommands:
        linkedName = varCommand.Report.LinkTargetName
        if linkedName is None:
            raise Exception("entry.Report.LinkTargetName can not be None")
        if linkedName not in resultDict:
            variableValue = generatorVariableReport.TryGetVariableReport(linkedName)
            if variableValue is None:
                raise FormatStringUndefinedVariableNameException(linkedName)
            resultDict[linkedName] = LookupVariableCommand(linkedName, variableValue, -1)
    return (resultDict, linkedCommands)


def RecursiveReplace(rFormatList: List[str], command: LookupVariableCommand, commandOptionIndex: int,
                     linkedCommandDict: Dict[LookupVariableCommand, List[LookupVariableCommand]]) -> None:
    if command.SplitIndex >= 0:
        rFormatList[command.SplitIndex] = command.Report.Options[commandOptionIndex]
    if command in linkedCommandDict:
        for linkedCommand in linkedCommandDict[command]:
            RecursiveReplace(rFormatList, linkedCommand, commandOptionIndex, linkedCommandDict)


def GetFormattedString(rFormatList: List[str],
                       variableList: List[LookupVariableCommand],
                       variableOptionIndices: Union[List[int], Tuple[int, ...]],
                       envCommandList: List[LookupEnvironmentVariableCommand],
                       linkedCommandDict: Dict[LookupVariableCommand, List[LookupVariableCommand]]) -> str:
    for envCommand in envCommandList:
        rFormatList[envCommand.SplitIndex] = envCommand.Value

    for index, command in enumerate(variableList):
        RecursiveReplace(rFormatList, command, variableOptionIndices[index], linkedCommandDict)

    return "".join(rFormatList)


def TryGetEnvironmentVariableResolveMethod(environmentVariableResolveMethod: int) -> Optional[FormatStringEnvironmentVariableResolver]:
    if environmentVariableResolveMethod == FormatStringEnvironmentVariableResolveMethod.Lookup:
        return None
    elif environmentVariableResolveMethod == FormatStringEnvironmentVariableResolveMethod.OSShellEnvironmentVariable:
        if os.name == 'posix':
            return lambda s: "$({0})".format(s)
        elif os.name == 'nt':
            return lambda s: "%{0}%".format(s)
        else:
            raise Exception("Unsupported os: {0}".format(os.name))
    else:
        raise Exception("Unknown environment variable resolve method: {0}".format(environmentVariableResolveMethod))


class ReportVariableFormatter(object):
    @staticmethod
    def Format(strFormat: str, generatorVariableReport: GeneratorVariableReport,
               userVariantSettingDict: Dict[str, str],
               environmentVariableResolveMethod: int = FormatStringEnvironmentVariableResolveMethod.Lookup) -> str:
        environmentVariableResolver = TryGetEnvironmentVariableResolveMethod(environmentVariableResolveMethod)
        parsedFormatString = ParsedFormatString(strFormat, generatorVariableReport, environmentVariableResolver)
        sourceVariableDict, linkedVariables = CreateLookupDict(parsedFormatString.VarCommandList, generatorVariableReport)
        linkedCommandDict = GetLinkedCommandList(linkedVariables, sourceVariableDict)

        variableList = []           # type: List[LookupVariableCommand]
        variableOptionIndices = []  # type: List[int]
        # the ordering of the sourceVariableDict.values is pretty random, but it doesnt matter
        for command in sourceVariableDict.values():
            if command.Report.LinkTargetName is None:
                variableList.append(command)
                if command.Name in userVariantSettingDict:
                    userDefinedOptionName = userVariantSettingDict[command.Name]
                    if userDefinedOptionName not in command.Report.Options:
                        raise InvalidVariableOptionNameException(command.Name, userDefinedOptionName, str(command.Report.Options))
                    variableOptionIndex = command.Report.Options.index(userDefinedOptionName)
                else:
                    defaultOptionIndex = generatorVariableReport.TryGetDefaultOptionIndex(command.Name)
                    variableOptionIndex = 0 if defaultOptionIndex is None else defaultOptionIndex
                variableOptionIndices.append(variableOptionIndex)
            elif command.Name in userVariantSettingDict:
                raise Exception("A linked variable '{0}' was found in the user feature list, this indicates a internal error".format(command.Name))

        # Substitute the (env) variables with their values
        scratchpadFormatList = parsedFormatString.SplitList # type: List[str]
        return GetFormattedString(scratchpadFormatList, variableList, variableOptionIndices, parsedFormatString.EnvCommandList, linkedCommandDict)


    @staticmethod
    def GetAllKnownCombinations(strFormat: str, generatorVariableReport: GeneratorVariableReport) -> List[str]:
        parsedFormatString = ParsedFormatString(strFormat, generatorVariableReport)

        sourceVariableDict, linkedVariables = CreateLookupDict(parsedFormatString.VarCommandList, generatorVariableReport)
        linkedCommandDict = GetLinkedCommandList(linkedVariables, sourceVariableDict)

        variableList = []  # type: List[LookupVariableCommand]
        optionList = []  # type: List[List[int]]
        # the ordering of the sourceVariableDict.values is pretty random, but it doesnt matter
        for command in sourceVariableDict.values():
            if command.Report.LinkTargetName is None:
                variableList.append(command)
                optionList.append([optionIndex for optionIndex in range(0, len(command.Report.Options))])

        # since we dont need the parsed format string object after this we can just reuse its list
        resultList = [] # type: List[str]
        scratchpadFormatList = parsedFormatString.SplitList # type: List[str]
        cartesianProduct = list(itertools.product(*optionList))  # type: List[Tuple[int, ...]]
        newStr = strFormat
        for entry in cartesianProduct:
            result = GetFormattedString(scratchpadFormatList, variableList, entry, parsedFormatString.EnvCommandList, linkedCommandDict)
            resultList.append(result)
        return resultList
