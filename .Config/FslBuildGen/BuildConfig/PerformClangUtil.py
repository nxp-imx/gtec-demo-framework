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

from typing import List
#from typing import Optional
import subprocess
from FslBuildGen.BuildConfig.ClangExeInfo import ClangExeInfo
from FslBuildGen.BuildExternal.PackageRecipeResult import PackageRecipeResult
from FslBuildGen.BuildExternal.PackageRecipeResultManager import PackageRecipeResultManager
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Log import Log

class PerformClangUtil(object):
    @staticmethod
    def _TryLookupAlternativeResults(packageRecipeResultManager: PackageRecipeResultManager, commandName: str) -> List[PackageRecipeResult]:
        possibleResults = []   # type: List[PackageRecipeResult]
        for packageRecipeResult in packageRecipeResultManager.PackageDict.values():
            if commandName in packageRecipeResult.ExecutableDict:
                possibleResults.append(packageRecipeResult)
        return possibleResults


    @staticmethod
    def LookupRecipeResults(packageRecipeResultManager: PackageRecipeResultManager,
                            recipePackageName: str, magicCommandName: str) -> ClangExeInfo:
        if not recipePackageName in packageRecipeResultManager.PackageDict:
            # ok no direct lookup, so lets try to locate it by the magic command name
            candidateList = PerformClangUtil._TryLookupAlternativeResults(packageRecipeResultManager, magicCommandName)
            if len(candidateList) < 1:
                raise Exception("Could not locate '{0}' recipe '{1}' in external results".format(magicCommandName, recipePackageName))
            if len(candidateList) > 1:
                raise Exception("Could not locate '{0}' recipe '{1}' in external results, multiple possible values {2}".format(magicCommandName, recipePackageName, [entry.Name for entry in candidateList]))
            recipeResults = candidateList[0]
        else:
            recipeResults = packageRecipeResultManager.PackageDict[recipePackageName]
            if not magicCommandName in recipeResults.ExecutableDict:
                raise Exception("Could not locate information about '{0}' in recipe '{1}'".format(magicCommandName, recipePackageName))

        exeResult = recipeResults.ExecutableDict[magicCommandName]
        return ClangExeInfo(exeResult.Path, exeResult.Version)

    @staticmethod
    def IsValidExtension(fileName: str, validExtensions: List[str]) -> bool:
        fileNameId = fileName.lower()
        for entry in validExtensions:
            if fileNameId.endswith(entry):
                return True
        return False

    @staticmethod
    def ShowVersion(log: Log, clangExeInfo: ClangExeInfo) -> None:
        try:
            log.LogPrint("Listing version")
            cmd = clangExeInfo.Command
            versionCommand = [cmd, '-version']
            result = subprocess.call(versionCommand)
            if result != 0:
                log.LogPrintWarning("The command '{0}' failed with '{1}'.".format(" ".join(versionCommand), result))
        except FileNotFoundError:
            log.DoPrintWarning("The command '{0}' failed with 'file not found'.".format(" ".join(versionCommand)))
            raise

    @staticmethod
    def CanProcessPackage(package: Package) -> bool:
        return package.ResolvedBuildAllIncludeFiles is not None and package.AllowCheck and (not package.IsVirtual or package.Type == PackageType.HeaderLibrary)
