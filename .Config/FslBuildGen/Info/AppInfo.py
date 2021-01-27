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

from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen import IOUtil
#from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator.Report.GeneratorExecutableReport import GeneratorExecutableReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Info.PackageInfo import PackageInfo
from FslBuildGen.Info.PackageGeneratorReportInfo import PackageGeneratorExecutableReportInfo
from FslBuildGen.Info.PackageGeneratorReportInfo import PackageGeneratorVariableReportInfo
from FslBuildGen.Info.RequirementInfo import RequirementInfo
from FslBuildGen.Info.RequirementInfo import RequirementType
from FslBuildGen.Log import Log


class AppInfo(object):
    def __init__(self, platformName: str, resolvedPackageList: List[PackageInfo]) -> None:
        self.PlatformName = platformName
        self.ResolvedPackageList = resolvedPackageList


class AppInfoPackage(object):
    def __init__(self, log: Log, appInfo: AppInfo, sourceFilename: str) -> None:
        self.Log = log
        self.SourceAppInfo = appInfo
        self.AbsolutePathSourceFile = sourceFilename

        self.AbsolutePath = IOUtil.GetDirectoryName(sourceFilename)

        self.__ResolvedPackageDict = self.__BuildPackageDict(appInfo)
        self.Name = self.__GuessDiscoverFilename(self.__ResolvedPackageDict, sourceFilename)
        self.ResolvedPackage = self.__ResolvedPackageDict[self.Name]
        self.ResolvedAllRequirements = self.__ResolvedAllRequirements(self.ResolvedPackage)         # type: List[RequirementInfo]
        self.ResolvedAllUsedFeatures = self.__ResolvedAllUsedFeatures(self.ResolvedPackage)         # type: List[RequirementInfo]
        self.ResolvedPlatformSupported = self.__ResolvedPlatformSupported(self.ResolvedPackage)     # type: bool
        self.Type = self.ResolvedPackage.Type

        self.GeneratorReport = self.__ResolveGeneratorReport(self.ResolvedPackage)
        # To be compatible with the Package in some methods
        self.ResolvedDirectExperimentalRecipe = None


    def __ResolvedAllRequirements(self, packageInfo: PackageInfo) -> List[RequirementInfo]:
        return list(packageInfo.AllRequirements)


    def __ResolvedAllUsedFeatures(self, packageInfo: PackageInfo) -> List[RequirementInfo]:
        return [requirement for requirement in packageInfo.AllRequirements if requirement.Type == RequirementType.Feature]


    def __ResolvedPlatformSupported(self, packageInfo: PackageInfo) -> bool:
        return packageInfo.Supported


    def __BuildPackageDict(self, appInfo: AppInfo) -> Dict[str, PackageInfo]:
        return {package.Name: package for package in appInfo.ResolvedPackageList}


    def __ResolveGeneratorReport(self, package: PackageInfo) -> Optional[PackageGeneratorReport]:
        if package.GeneratorReport is None:
            return None

        executableReportInfo = package.GeneratorReport.ExecutableReport
        variableReportInfo = package.GeneratorReport.VariableReport

        executableReport = None if executableReportInfo is None else self.__ResolveGeneratorExecutableReport(executableReportInfo)
        variableReport = self.__ResolveGeneratorVariableReport(variableReportInfo)
        return PackageGeneratorReport(None, executableReport, variableReport, None)


    def __ResolveGeneratorExecutableReport(self, report: PackageGeneratorExecutableReportInfo) -> GeneratorExecutableReport:
        return GeneratorExecutableReport(report.UseAsRelative, report.ExeFormatString, report.RunScript, report.EnvironmentVariableResolveMethod)


    def __ResolveGeneratorVariableReport(self, report: PackageGeneratorVariableReportInfo) -> GeneratorVariableReport:
        variabelReport = GeneratorVariableReport(self.Log, True)
        for variableReport in report.VariableReport:
            variabelReport.Add(variableReport.Name, variableReport.Options, variableReport.LinkTargetName)

        for defaultOptionKey, defaultOptionValue in report.DefaultOptions.items():
            variabelReport.SetDefaultOptionIndex(defaultOptionKey, defaultOptionValue)
        return variabelReport


    def __GuessDiscoverFilename(self, packageDict: Dict[str, PackageInfo], sourceFilename: str) -> str:
        sourceDirectory = IOUtil.GetDirectoryName(sourceFilename)
        sourcePackageName = sourceDirectory.replace('/', '.')
        allSubPackageNames = sourcePackageName.split('.')

        while len(allSubPackageNames) > 0:
            packageNameCandidate = self.__GeneratePackageName(allSubPackageNames)
            if packageNameCandidate in packageDict:
                return packageNameCandidate
            allSubPackageNames.pop(0)

        raise Exception("Could not determine the package name for the file at '{0}'".format(sourceFilename))


    def __GeneratePackageName(self, subNameList: List[str]) -> str:
        if len(subNameList) <= 0:
            return ""
        name = subNameList[0]
        for index in range(1, len(subNameList)):
            name = "{0}.{1}".format(name, subNameList[index])
        return name
