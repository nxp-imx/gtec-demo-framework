#!/usr/bin/env python

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

import itertools
from FslBuildGen import IOUtil

def __SafeAddEntry(gitIgnoreDict, package, entry):
    if not package.Name in gitIgnoreDict:
        gitIgnoreDict[package.Name] = set()
    gitIgnoreDict[package.Name].add(entry)

def AddPathIfInPackageRoot(gitIgnoreDict, package, pathToFile):
    pathDir = IOUtil.GetDirectoryName(pathToFile)
    if pathDir == package.AbsolutePath:
        fileName = IOUtil.GetFileName(pathToFile)
        __SafeAddEntry(gitIgnoreDict, package, fileName)


def __ExtractVariantNamesAndOrder(package, variantFormattingString):
    variants = variantFormattingString.split("$")
    nameList = []
    for entry in variants:
        if entry.startswith("("):
            index = entry.find(')')
            if index < 0:
                raise Exception("Failed to parse the variant name formatting string: %s" % (variantFormattingString) )
            name = entry[1:index]
            nameList.append(name)
    return nameList

def __ToKey(entry):
    return "$(%s)" % (entry)


def __BuildAllVariantStrings(package, variantFormattingString, variantNames):
    lists = []
    for name in variantNames:
        if not name in package.ResolvedAllVariantDict:
            raise Exception("Failed to locate the variant")
        variant = package.ResolvedAllVariantDict[name]
        lists.append(list(variant.OptionDict.keys()))
    cartesianProduct = list(itertools.product(*lists))

    finalNames = []
    for variantCombination in cartesianProduct:
        result = variantFormattingString
        for index, entry in enumerate(variantCombination):
            asKey =__ToKey(variantNames[index])
            result = result.replace(asKey, entry)
        finalNames.append(result)

    return finalNames


def AddVariantExecuteableNames(gitIgnoreSet, package):
    fileName = package.Name
    if len(package.ResolvedVariantName) <= 0:
        __SafeAddEntry(gitIgnoreSet, package, fileName)
        return

    variantFormattingString = package.ResolvedVariantName
    variantNames = __ExtractVariantNamesAndOrder(package, variantFormattingString)
    variantStrings = __BuildAllVariantStrings(package, variantFormattingString, variantNames)
    for entry in variantStrings:
        exeFileName = fileName+entry
        __SafeAddEntry(gitIgnoreSet, package, exeFileName)


