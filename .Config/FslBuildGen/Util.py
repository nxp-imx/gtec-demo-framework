#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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

from typing import Any
from typing import List
from typing import Optional
#from FslBuildGen.DataTypes import ExternalDependencyType

# TODO: add a proper type to entries. It is a type that contains the Name attribute of type str
def ExtractNames(entries: Any) -> List[str]:
    resList = []
    for entry in entries:
        resList.append(entry.Name)
    return resList


# TODO: add a proper type to entries. It is a type that contains the Package attribute and the package attribute is of type Package
def ExtractNonVirtualNames(entries: Any) -> List[str]:
    resList = []
    for entry in entries:
        package = entry.Package
        if not package.IsVirtual:
            resList.append(package.Name)
    return resList


# TODO: add a proper type to entries. It is a type that contains the Name attribute of type str
def ExtractNamesAsMakeEnvironmentVariables(entries: Any) -> List[str]:
    resList = []
    for entry in entries:
        if not (entry.Name.startswith("$(") and entry.Name.endswith(")")):
            resList.append("$({0})".format(entry.Name))
        else:
            resList.append(entry.Name)
    return resList




def RemoveEnvironmentVariablePadding(environmentVariableName: str) -> str:
    if not environmentVariableName.startswith("$(") or not environmentVariableName.endswith(")"):
        raise Exception("Environment variable name '{0}' did not follow the expected pattern of $(name)".format(environmentVariableName))
    return environmentVariableName[2:-1]


# TODO: add a proper type to entries. It is a type that contains the Type attribute of type typeValue
def FilterByType(entries: List[Any], typeValue: Any) -> List[Any]: #: ExternalDependencyType):
    res = []
    if isinstance(typeValue, (list, set)):
        for entry in entries:
            if entry.Type in typeValue:
                res.append(entry)
    else:
        for entry in entries:
            if entry.Type == typeValue:
                res.append(entry)
    return res


#def EnsureUTF8(value: Optional[str]) -> Optional[str]:
#    if value is None:
#        return None
#    try:
#        # in python 3 all strings are unicode
#        return str(value)
#    except ValueError:
#        return value



def TryUTF8ToAscii(value: Optional[str]) -> Optional[str]:
    return value
    #if value is None:
    #    return None
    #try:
    #    return str(value.encode('ascii'))
    #except UnicodeEncodeError:
    #    return None


def UTF8ToAscii(value: str) -> str:
    return value
#    return str(value.encode('ascii'))


def IsValidNameStartCharacter(ch: str) -> bool:
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z'))


def IsValidNameEndCharacter(ch: str) -> bool:
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))


def IsValidNameCharacter(ch: str) -> bool:
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))


def IsValidPackageNameCharacter(ch: str) -> bool:
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_') or (ch == '.'))


def IsValidUppercaseNameStartCharacter(ch: str) -> bool:
    return ((ch >= 'A' and ch <= 'Z'))


def IsValidUppercaseNameCharacter(ch: str) -> bool:
    return ((ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))


def IsValidName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidNameCharacter(ch):
            return False
    return True

def IsValidUppercaseName(name: str) -> bool:
    if len(name) <= 0 or not IsValidUppercaseNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidUppercaseNameCharacter(ch):
            return False
    return True


def IsValidVirtualVariantName(name: str) -> bool:
    if not name.startswith("$(") or not name.endswith(")"):
        return False
    return IsValidUppercaseName(name[2:-1])


def IsValidPackageName(name: str, allowSubPackages: bool) -> bool:
    if not allowSubPackages:
        return IsValidName(name)

    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False
    if not IsValidNameEndCharacter(name[-1]):
        return False

    isFirstCharInName = True
    for ch in name:
        if ch != '.':
            if not isFirstCharInName:
                if not IsValidNameCharacter(ch):
                    return False
            else:
                if not IsValidNameStartCharacter(ch):
                    return False
                isFirstCharInName = False
        else:
            isFirstCharInName = True

    return name.find('..') < 0


def IsValidCStyleName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False
    for index in range(1, len(name)):
        if not IsValidNameCharacter(name[index]):
            return False
    return True


def IsValidRequirementName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False
    for index in range(1, len(name)):
        if name[index] != '.' and not IsValidNameCharacter(name[index]):
            return False
    return True


def ExtractNamesAsVariables(entries: Any) -> List[str]:
    resList = []
    for entry in entries:
        if IsValidCStyleName(entry.Name):
            resList.append("${{{0}}}".format(entry.Name))
        else:
            raise Exception("The name '{0}' is not a valid variable name".format(entry.Name))
    return resList


def ChangeToBashEnvVariables(path: str) -> str:
    index = path.find("$(")
    if index < 0:
        return path
    endIndex = path.find(")")
    if endIndex < 0:
        return path
    start = path[:index]
    envName = path[index+2:endIndex]
    end = path[endIndex+1:]
    path = "%s$%s%s" % (start, envName, end)
    return ChangeToBashEnvVariables(path)


def ChangeToCMakeVariables(path: str) -> str:
    index = path.find("$(")
    if index < 0:
        return path
    endIndex = path.find(")")
    if endIndex < 0:
        return path
    start = path[:index]
    envName = path[index+2:endIndex]
    end = path[endIndex+1:]
    path = "%s${%s}%s" % (start, envName, end)
    return ChangeToCMakeVariables(path)


def ChangeToCMakeEnvVariables(path: str) -> str:
    index = path.find("$(")
    if index < 0:
        return path
    endIndex = path.find(")")
    if endIndex < 0:
        return path
    start = path[:index]
    envName = path[index+2:endIndex]
    end = path[endIndex+1:]
    path = "%s$ENV{%s}%s" % (start, envName, end)
    return ChangeToCMakeEnvVariables(path)


def ChangeToDosEnvironmentVariables(path: str) -> str:
    index = path.find("$(")
    if index < 0:
        return path
    endIndex = path.find(")")
    if endIndex < 0:
        return path
    start = path[:index]
    envName = path[index+2:endIndex]
    end = path[endIndex+1:]
    path = "%s%%%s%%%s" % (start, envName, end)
    return ChangeToDosEnvironmentVariables(path)
