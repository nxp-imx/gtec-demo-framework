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
from typing import Tuple
#from FslBuildGen.DataTypes import ExternalDependencyType

# TODO: add a proper type to entries. It is a type that contains the Name attribute of type str
def ExtractNames(entries: Any) -> List[str]:
    resList = []
    for entry in entries:
        resList.append(entry.Name)
    return resList

# TODO: add a proper type to entries. It is a type that contains the Name attribute of type str
def ExtractValues(entries: Any) -> List[str]:
    resList = []
    for entry in entries:
        resList.append(entry.Value)
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
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9'))


def IsValidNameCharacter(ch: str) -> bool:
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))

def IsValidFlavorOptionNameCharacter(ch: str) -> bool:
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_') or (ch == '-'))


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

def IsValidCompanyName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if ch != ' ' and not IsValidNameCharacter(ch):
            return False
    if name[-1] == ' ':
        return False
    return True



def IsValidConstraintFlavorName(name: str) -> bool:
    if len(name) < 3 or '.' not in name:
        return False
    return IsValidPackageName(name)

def IsValidFlavorName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidNameCharacter(ch):
            return False
    return True

def IsValidUnresolvedPackageFlavorName(name: str) -> bool:
    return IsValidPackageName(name)

def IsValidUnresolvedPackageFlavorUnqualifiedName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidNameCharacter(ch):
            return False
    return True

def IsValidFlavorExtensionName(name: str) -> bool:
    return IsValidConstraintFlavorName(name)


def IsValidFlavorOptionName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    previousChar = ' '
    for ch in name:
        if not IsValidFlavorOptionNameCharacter(ch):
            return False
        # Double underscores are reserved for internal use
        if ch == '_' and previousChar == '_':
            return False
        previousChar = ch
    return True


def IsValidCommandName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidNameCharacter(ch) and ch != '-' and ch != '+':
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

def __IsValidPackageName(name: str) -> bool:
    isFirstCharInName = True
    previousChar = ' '
    for ch in name:
        if ch != '.':
            if not isFirstCharInName:
                if not IsValidNameCharacter(ch):
                    return False
            else:
                if not IsValidNameStartCharacter(ch):
                    return False
                isFirstCharInName = False
        elif previousChar == '.':
            return False
        else:
            isFirstCharInName = True
        previousChar = ch
    return True

def __IsValidUnresolvedPackageName(name: str) -> bool:
    isFirstCharInName = True
    previousChar = ' '
    for ch in name:
        if ch != '.':
            if not isFirstCharInName:
                # Double underscores are reserved for internal use
                if ch == '_' and previousChar == '_':
                    return False
                if not IsValidNameCharacter(ch):
                    return False
            else:
                if not IsValidNameStartCharacter(ch):
                    return False
                isFirstCharInName = False
        elif previousChar == '.':
            return False
        else:
            isFirstCharInName = True
        previousChar = ch
    return True


def IsValidPackageName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False
    if not IsValidNameEndCharacter(name[-1]):
        return False
    return __IsValidPackageName(name)

def IsValidPackageInstanceName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False
    if not IsValidNameEndCharacter(name[-1]):
        return False
    return __IsValidPackageName(name)

def IsValidPackageShortName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidNameCharacter(ch):
            return False
    return True

def IsValidPackageNamespaceName(name: str) -> bool:
    if len(name) <= 0:
        return True
    if not IsValidNameStartCharacter(name[0]) or not IsValidNameEndCharacter(name[-1]):
        return False
    return __IsValidPackageName(name)


def IsValidUnresolvedPackageName(name: str) -> bool:
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False
    if not IsValidNameEndCharacter(name[-1]):
        return False

    return __IsValidUnresolvedPackageName(name)


def IsValidRecipeName(name: str) -> bool:
    return IsValidUnresolvedPackageName(name)


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


def IsValidDefineValue(name: Optional[str]) -> bool:
    if name is None:
        return True;
    if len(name) <= 0:
        return False

    for ch in name:
        if not IsValidNameCharacter(ch):
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
    path = "{0}$ENV{{{1}}}{2}".format(start, envName, end)
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

def ParseVersionString(version: str, splitChar: str = '.', maxValues: int = 4) -> List[int]:
    valueStrings = version.split(splitChar)
    if len(valueStrings) > maxValues:
        raise Exception("Version string contained more values than allowed '{0}'".format(version))
    return [int(value) for value in valueStrings]

def GetPackageNames(name: str) -> Tuple[str, str]:
    """
    extract the namespace and pure name
    """
    index = name.rfind('.')
    if index < 0:
        return name, ''
    return name[index+1:], name[:index]

def GetPackageSourceAndFlavorNames(name: str) -> Tuple[str, str]:
    """
    extract the namespace and pure name
    """
    index = name.rfind('___')
    if index < 0:
        return name, ''
    return name[:index], name[index+3:]
