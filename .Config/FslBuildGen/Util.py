#!/usr/bin/env python

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

import sys


def ExtractNames(entries):
    list = []
    for entry in entries:
        list.append(entry.Name)
    return list


def ExtractNonVirtualNames(entries):
    list = []
    for entry in entries:
        package = entry.Package
        if not package.IsVirtual:
            list.append(package.Name)
    return list


def ExtractNamesAsMakeEnvironmentVariables(entries):
    list = []
    for entry in entries:
        if not (entry.Name.startswith('$(') and entry.Name.endswith(')')):
            list.append("$(%s)" % (entry.Name))
        else:
            list.append(entry.Name)
    return list



def RemoveEnvironmentVariablePadding(environmentVariableName):
    if not environmentVariableName.startswith("$(") or not environmentVariableName.endswith(")"):
        raise Exception("Environment variable name '%s' did not follow the expected pattern of $(name)" % (environmentVariableName))
    return environmentVariableName[2:-1]


def FilterByType(entries, type):
    res = []
    for entry in entries:
        if entry.Type == type:
            res.append(entry)
    return res

def EnsureUTF8(value):
    if value == None:
        return None
    try:
        if sys.version_info < (3,):
            return unicode(value.encode('utf-8'))
        else: # in python 3 all strings are unicode
            return str(value)
    except ValueError:
        return value

def UTF8ToAscii(value):
    if value == None:
        return None
    try:
        if sys.version_info < (3,):
            udata=value.decode("utf-8")
            return udata.encode("ascii","ignore")
        else:
            return value;
    except ValueError:
        return value


def IsValidNameStartCharacter(ch):
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z'))

def IsValidNameEndCharacter(ch):
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))

def IsValidNameCharacter(ch):
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))

def IsValidPackageNameCharacter(ch):
    return ((ch >= 'a' and ch <= 'z') or (ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_') or (ch == '.'))


def IsValidUppercaseNameStartCharacter(ch):
    return ((ch >= 'A' and ch <= 'Z'))

def IsValidUppercaseNameCharacter(ch):
    return ((ch >= 'A' and ch <= 'Z') or (ch >= '0' and ch <= '9') or (ch == '_'))


def IsValidName(name):
    if len(name) <= 0 or not IsValidNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidNameCharacter(ch):
            return False
    return True

def IsValidUppercaseName(name):
    if len(name) <= 0 or not IsValidUppercaseNameStartCharacter(name[0]):
        return False

    for ch in name:
        if not IsValidUppercaseNameCharacter(ch):
            return False
    return True


def IsValidVirtualVariantName(name):
    if not name.startswith("$(") or not name.endswith(")"):
        return False
    return IsValidUppercaseName(name[2:-1])


def IsValidPackageName(name, allowSubPackages):
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
            pass

    return name.find('..') < 0


def ChangeToBashEnvVariables(path):
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


def ChangeToCMakeVariables(path):
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


def ChangeToCMakeEnvVariables(path):
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