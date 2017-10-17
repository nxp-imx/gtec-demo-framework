#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
from typing import Set
import itertools
from FslBuildGen import IOUtil
from FslBuildGen.Generator.Report.GeneratorExecutableReport import GeneratorExecutableReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.ReportVariableFormatter import ReportVariableFormatter
from FslBuildGen.Generator.VariantNameHelper import VariantNameHelper
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package


def SafeAddEntry(rGitIgnoreDict: Dict[str, Set[str]], package: Package, entry: str) -> None:
    if not package.Name in rGitIgnoreDict:
        rGitIgnoreDict[package.Name] = set()
    rGitIgnoreDict[package.Name].add(entry)


def AddPathIfInPackageRoot(rGitIgnoreDict: Dict[str, Set[str]], package: Package, pathToFile: str) -> None:
    pathDir = IOUtil.GetDirectoryName(pathToFile)
    if pathDir == package.AbsolutePath:
        fileName = IOUtil.GetFileName(pathToFile)
        SafeAddEntry(rGitIgnoreDict, package, fileName)


def AddFromBuildReport(rGitIgnoreDict: Dict[str, Set[str]], package: Package,
                       executableReport: GeneratorExecutableReport,
                       variableReport: GeneratorVariableReport) -> None:
    if executableReport is None or variableReport is None:
        return

    allKnownExeFiles = ReportVariableFormatter.GetAllKnownCombinations(executableReport.ExeFormatString, variableReport)
    for filename in allKnownExeFiles:
        SafeAddEntry(rGitIgnoreDict, package, filename)
