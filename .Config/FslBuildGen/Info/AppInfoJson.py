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

#from typing import Any
from typing import cast
from typing import Dict
from typing import List
from typing import Optional
#from typing import Union
import json
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator.Report.Datatypes import FormatStringEnvironmentVariableResolveMethod
from FslBuildGen.Info.AppInfo import AppInfo
from FslBuildGen.Info.RequirementInfo import RequirementInfo
from FslBuildGen.Info.RequirementInfo import RequirementType
from FslBuildGen.Info.RequirementFeatureInfo import RequirementFeatureInfo
from FslBuildGen.Info.RequirementExtensionInfo import RequirementExtensionInfo
from FslBuildGen.Info.PackageInfo import PackageInfo
from FslBuildGen.Info.PackageGeneratorReportInfo import PackageGeneratorReportInfo
from FslBuildGen.Info.PackageGeneratorReportInfo import PackageGeneratorExecutableReportInfo
from FslBuildGen.Info.PackageGeneratorReportInfo import PackageGeneratorVariableReportInfo
from FslBuildGen.Info.PackageGeneratorReportInfo import GeneratorVariableReportInfo
from FslBuildGen.Log import Log

class JsonRequirementKey(object):
    Name = "Name"
    Type = "Type"
    Version = "Version"
    Extends = "Extends"

class JsonPackageGeneratorExecutableReport(object):
    UseAsRelative = "UseAsRelative"
    EnvironmentVariableResolveMethod = "EnvironmentVariableResolveMethod"
    ExeFormatString = "ExeFormatString"
    RunScript = "RunScript"


class JsonPackageVariableReport(object):
    Name = "Name"
    Options = "Options"
    LinkTargetName = "LinkTargetName"


class JsonPackageGeneratorVariableReport(object):
    VariableReportList = "VariableReportList"
    DefaultOptions = "DefaultOptions"


class JsonPackageGeneratorReport(object):
    ExecutableReport = "ExecutableReport"
    VariableReport = "VariableReport"


class JsonPackageKey(object):
    AllRequirements = "AllRequirements"
    Type = "Type"
    Supported = "Supported"
    GeneratorReport = "GeneratorReport"
    SourcePackageName = "SourcePackageName"
    CreationYear = "CreationYear"


class JsonRootKey(object):
    PlatformName = "PlatformName"
    ResolvedPackageList = "ResolvedPackageList"


def __TryReadDictStrAttrib(sourceDict: Dict[str, object], key: str, defaultValue: Optional[str] = None) -> Optional[str]:
    """ Read the attrib if its available, else return defaultValue """
    value = None if not key in sourceDict else sourceDict[key]  # type: Optional[object]
    if value is not None:
        if isinstance(value, str):
            return value
        raise Exception("{0} expects a string value not '{1}' of type '{2}'".format(key, value, type(value)))
    elif defaultValue is not None:
        return defaultValue
    return None


def __ReadDictStrAttrib(sourceDict: Dict[str, object], key: str, defaultValue: Optional[str] = None) -> str:
    """ If the attrib is there we return it
        if its not there and defaultValue is not None we return the default value.
        if its not there and defaultValue is None we throw a exception.
    """
    value = None if not key in sourceDict else sourceDict[key]  # type: Optional[object]
    if value is not None:
        if isinstance(value, str):
            return value
    elif defaultValue is not None:
        return defaultValue
    raise Exception("{0} expects a string value not '{1}' of type '{2}'".format(key, value, type(value)))


def __TryReadDictBoolAttrib(sourceDict: Dict[str, object], key: str, defaultValue: Optional[bool]) -> Optional[bool]:
    """ Read the attrib if its available, else return defaultValue """
    value = None if not key in sourceDict else sourceDict[key]  # type: Optional[object]
    if value is not None:
        if isinstance(value, bool):
            return value
        raise Exception("{0} expects a bool value of either 'true' or 'false' not '{1}' of type '{2}'".format(key, value, type(value)))
    elif defaultValue is not None:
        return defaultValue
    return None


def __ReadDictBoolAttrib(sourceDict: Dict[str, object], key: str, defaultValue: Optional[bool] = None) -> bool:
    """ If the attrib is there we return it
        if its not there and defaultValue is not None we return the default value.
        if its not there and defaultValue is None we throw a exception.
    """
    value = None if not key in sourceDict else sourceDict[key]  # type: Optional[object]
    if value is not None:
        if isinstance(value, bool):
            return value
    elif defaultValue is not None:
        return defaultValue
    raise Exception("{0} expects a bool value of either 'true' or 'false' not '{1}' of type '{2}'".format(key, value, type(value)))



def __ReadDictIntAttrib(sourceDict: Dict[str, object], key: str, defaultValue: Optional[int] = None) -> int:
    """ If the attrib is there we return it
        if its not there and defaultValue is not None we return the default value.
        if its not there and defaultValue is None we throw a exception.
    """
    value = None if not key in sourceDict else sourceDict[key]  # type: Optional[object]
    if value is not None:
        if isinstance(value, int):
            return value
    elif defaultValue is not None:
        return defaultValue
    raise Exception("{0} expects a int value not '{1}' of type '{2}'".format(key, value, type(value)))


def __ParseJsonStringList(log: Log, packageName: str, jsonList: List[object]) -> List[str]:
    if not isinstance(jsonList, list):
        raise Exception("Invalid file format")

    for jsonEntry in jsonList:
        if not isinstance(jsonEntry, str):
            raise Exception("The list entries are expected to be strings")

    return cast(List[str], jsonList)


def __ParseRequirementInfo(log: Log, jsonRequirementDict: Dict[str, object]) -> RequirementInfo:
    name = __ReadDictStrAttrib(jsonRequirementDict, JsonRequirementKey.Name)
    strRequirementType = __ReadDictStrAttrib(jsonRequirementDict, JsonRequirementKey.Type)
    version = __ReadDictStrAttrib(jsonRequirementDict, JsonRequirementKey.Version, "")
    extends = __ReadDictStrAttrib(jsonRequirementDict, JsonRequirementKey.Extends, "")
    requirementType = RequirementType.FromString(strRequirementType)

    if requirementType == RequirementType.Feature:
        return RequirementFeatureInfo(name, version, extends)
    elif requirementType == RequirementType.Extension:
        return RequirementExtensionInfo(name, version, extends)

    log.LogPrintWarning("No special handling for requirement type: {0}, using default".format(strRequirementType))
    return RequirementInfo(name, requirementType, version, extends)


def __ParseRequirementsInfo(log: Log, jsonList: List[Dict[str, object]]) -> List[RequirementInfo]:
    if not isinstance(jsonList, list):
        raise Exception("Invalid file format")

    result = [] # type: List[RequirementInfo]
    for dictEntry in jsonList:
        entry = __ParseRequirementInfo(log, dictEntry)
        result.append(entry)
    return result


def __ParsePackageGeneratorExecutableReport(log: Log, packageName: str, jsonDict: Dict[str, object]) -> Optional[PackageGeneratorExecutableReportInfo]:
    useAsRelative = __ReadDictBoolAttrib(jsonDict, JsonPackageGeneratorExecutableReport.UseAsRelative)
    environmentVariableResolveMethod = __ReadDictIntAttrib(jsonDict, JsonPackageGeneratorExecutableReport.EnvironmentVariableResolveMethod)  # type: int
    exeFormatString = __ReadDictStrAttrib(jsonDict, JsonPackageGeneratorExecutableReport.ExeFormatString)
    runScript = __TryReadDictStrAttrib(jsonDict, JsonPackageGeneratorExecutableReport.RunScript)

    convertedEnvironmentVariableResolveMethod = FormatStringEnvironmentVariableResolveMethod.Convert(environmentVariableResolveMethod)
    return PackageGeneratorExecutableReportInfo(useAsRelative, exeFormatString, runScript, convertedEnvironmentVariableResolveMethod)



def __ParsePackageVariableReport(log: Log, packageName: str, jsonDict: Dict[str, object]) -> GeneratorVariableReportInfo:
    name = __ReadDictStrAttrib(jsonDict, JsonPackageVariableReport.Name)
    linkTargetName = __TryReadDictStrAttrib(jsonDict, JsonPackageVariableReport.LinkTargetName)

    if JsonPackageVariableReport.Options not in jsonDict:
        raise Exception("Invalid file format")

    jsonOptionsList = jsonDict[JsonPackageVariableReport.Options]
    if not isinstance(jsonOptionsList, list):
        raise Exception("Invalid file format")
    options = __ParseJsonStringList(log, packageName, jsonOptionsList)

    return GeneratorVariableReportInfo(name, options, linkTargetName)


def __ParsePackageGeneratorVariableReport_VariableReportList(log: Log, packageName: str, jsonList: List[object]) -> List[GeneratorVariableReportInfo]:
    resultList = [] # type: List[GeneratorVariableReportInfo]
    for jsonEntry in jsonList:
        if not isinstance(jsonEntry, dict):
            raise Exception("Invalid file format")
        variableReport = __ParsePackageVariableReport(log, packageName, jsonEntry)
        resultList.append(variableReport)
    return resultList


def __ParsePackageGeneratorVariableReport_DefaultOptions(log: Log, packageName: str, jsonDict: Dict[object, object]) -> Dict[str, int]:
    # do basic validation
    for key, value in jsonDict.items():
        if not isinstance(key, str) or not isinstance(value, int):
            errFormatStr = "The default options must be of the format: key=str, value=int. Key was '{0}' of type '{1}', value was '{2}' of type '{3}'"
            raise Exception(errFormatStr.format(key, type(key), value, type(value)))
    return cast(Dict[str, int], jsonDict)


def __ParsePackageGeneratorVariableReport(log: Log, packageName: str, jsonDict: Dict[str, object]) -> PackageGeneratorVariableReportInfo:
    if JsonPackageGeneratorVariableReport.VariableReportList not in jsonDict:
        raise Exception("Invalid file format")
    if JsonPackageGeneratorVariableReport.DefaultOptions not in jsonDict:
        raise Exception("Invalid file format")

    jsonVariableReportList = jsonDict[JsonPackageGeneratorVariableReport.VariableReportList]
    jsonDefaultOptions = jsonDict[JsonPackageGeneratorVariableReport.DefaultOptions]

    if not isinstance(jsonVariableReportList, list):
        raise Exception("Invalid file format")
    variableReportList = __ParsePackageGeneratorVariableReport_VariableReportList(log, packageName, jsonVariableReportList)

    if not isinstance(jsonDefaultOptions, dict):
        raise Exception("Invalid file format")
    defaultOptions = __ParsePackageGeneratorVariableReport_DefaultOptions(log, packageName, jsonDefaultOptions)

    return PackageGeneratorVariableReportInfo(variableReportList, defaultOptions)


def __ParsePackageGeneratorReport(log: Log, packageName: str, jsonDict: Dict[str, object]) -> Optional[PackageGeneratorReportInfo]:
    if not isinstance(jsonDict, dict):
        raise Exception("Invalid file format")

    executableReport = None  # type: Optional[PackageGeneratorExecutableReportInfo]
    if JsonPackageGeneratorReport.ExecutableReport in jsonDict:
        jsonExecutableReportDict = jsonDict[JsonPackageGeneratorReport.ExecutableReport]
        if not isinstance(jsonExecutableReportDict, dict):
            raise Exception("Invalid file format")
        executableReport = __ParsePackageGeneratorExecutableReport(log, packageName, jsonExecutableReportDict)


    if JsonPackageGeneratorReport.VariableReport in jsonDict:
        jsonVariableReportDict = jsonDict[JsonPackageGeneratorReport.VariableReport]
        if not isinstance(jsonVariableReportDict, dict):
            raise Exception("Invalid file format")
        variableReport = __ParsePackageGeneratorVariableReport(log, packageName, jsonVariableReportDict)

    return PackageGeneratorReportInfo(executableReport, variableReport)



def __ParsePackage(log: Log, packageName: str, jsonPackageDict: Dict[str, object]) -> PackageInfo:
    jsonAllRequirements = jsonPackageDict[JsonPackageKey.AllRequirements]

    if not isinstance(jsonAllRequirements, list):
        raise Exception("Invalid file format")
    allRequirements = __ParseRequirementsInfo(log, jsonAllRequirements)

    strSourcePackageName = __ReadDictStrAttrib(jsonPackageDict, JsonPackageKey.SourcePackageName)
    strCreationYear = __ReadDictStrAttrib(jsonPackageDict, JsonPackageKey.CreationYear)
    strPackageType = __ReadDictStrAttrib(jsonPackageDict, JsonPackageKey.Type)
    packageType = PackageType.FromString(strPackageType)
    supported = __ReadDictBoolAttrib(jsonPackageDict, JsonPackageKey.Supported, True)

    jsonPackageGeneratorReport = None if JsonPackageKey.GeneratorReport not in jsonPackageDict else jsonPackageDict[JsonPackageKey.GeneratorReport]
    generatorReport = None  # type: Optional[PackageGeneratorReportInfo]
    if jsonPackageGeneratorReport is not None:
        if not isinstance(jsonPackageGeneratorReport, dict):
            raise Exception("Invalid file format")
        generatorReport = __ParsePackageGeneratorReport(log, packageName, jsonPackageGeneratorReport)

    return PackageInfo(packageName, strSourcePackageName, allRequirements, packageType, supported, generatorReport)


def __ParseResolvedPackageList(log: Log, jsonResolvedPackageListDict: Dict[str, object]) -> List[PackageInfo]:
    packageList = []
    for packageName, jsonPackageDict in jsonResolvedPackageListDict.items():
        if isinstance(jsonPackageDict, dict):
            packageInfo = __ParsePackage(log, packageName, jsonPackageDict)
            packageList.append(packageInfo)
        else:
            raise Exception("Invalid file format")
    return packageList


def TryLoad(log: Log, path: str) -> Optional[AppInfo]:
    content = IOUtil.TryReadFile(path)
    if content is None:
        return None

    try:
        jsonDict = json.loads(content)

        platformName = jsonDict[JsonRootKey.PlatformName]  # type: str
        resolvedPackageList = __ParseResolvedPackageList(log, jsonDict[JsonRootKey.ResolvedPackageList])

        return AppInfo.CreateAppInfo(platformName, resolvedPackageList)
    except Exception:
        log.LogPrintWarning("Failed to parse json content in file: '{0}'".format(path))
        raise
