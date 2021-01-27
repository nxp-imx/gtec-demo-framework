#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2019 NXP
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
from typing import Optional
from FslBuildGen import Util
from FslBuildGen.Engine.Cache.JsonProjectIdCache import JsonProjectIdCache
from FslBuildGen.Exceptions import InvalidPackageNameException

class ProjectIdCache(object):

    def __init__(self, projectIdCache: JsonProjectIdCache) -> None:
        super().__init__()
        self.__projectIdCache = projectIdCache

        projectIdToNameDict = {} # type: Dict[str, str]
        for packageName, packageProjectId in self.__projectIdCache.ProjectIdDict.items():
            if not Util.IsValidPackageName(packageName):
                raise InvalidPackageNameException(packageName)
            if packageProjectId in projectIdToNameDict:
                raise Exception("The package project id '{0}' is registered for multiple package names. First '{1}' Second '{2}'".format(packageProjectId, projectIdToNameDict[packageName], packageName))
            projectIdToNameDict[packageProjectId] = packageName

        self.__projectIdToNameDict = projectIdToNameDict

    def Contains(self, packageProjectId: str) -> bool:
        return packageProjectId in self.__projectIdToNameDict

    def TryGetByName(self, packageName: str) -> Optional[str]:
        if packageName in self.__projectIdCache.ProjectIdDict:
            return self.__projectIdCache.ProjectIdDict[packageName]
        return None

    #def Add(self, packageName: str, packageProjectId: str) -> bool:
    #    if not Util.IsValidPackageName(packageName):
    #        raise InvalidPackageNameException(packageName)

    #    noCollision = True
    #    if packageProjectId in self.__projectIdToNameDict:
    #        oldName = self.__projectIdToNameDict[packageProjectId]
    #        if oldName != packageName:
    #            self.__projectIdToNameDict.pop(packageProjectId)
    #            self.__projectIdCache.Remove(oldName)
    #            noCollision = False
    #    if packageName in self.__projectIdCache.ProjectIdDict:
    #        oldId = self.__projectIdCache.ProjectIdDict[packageName]
    #        if oldId != packageProjectId:
    #            self.__projectIdCache.Remove(packageName)
    #            self.__projectIdToNameDict.pop(oldId)
    #            noCollision = False

    #    self.__projectIdCache.Add(packageName, packageProjectId)
    #    self.__projectIdToNameDict[packageProjectId] = packageName
    #    return noCollision

    def Add(self, packageName: str, packageProjectId: str) -> None:
        if not Util.IsValidPackageName(packageName):
            raise InvalidPackageNameException(packageName)

        if packageProjectId in self.__projectIdToNameDict:
            oldName = self.__projectIdToNameDict[packageProjectId]
            if oldName != packageName:
                raise Exception("PackageName '{0}' uses project id '{1}' already used by package: '{2}'".format(packageName, packageProjectId, self.__projectIdToNameDict[packageProjectId]))
        if packageName in self.__projectIdCache.ProjectIdDict:
            oldId = self.__projectIdCache.ProjectIdDict[packageName]
            if oldId != packageProjectId:
                raise Exception("PackageName '{0}' already added with project Id '{1}'".format(packageName, self.__projectIdCache.ProjectIdDict[packageName]))

        self.__projectIdCache.Add(packageName, packageProjectId)
        self.__projectIdToNameDict[packageProjectId] = packageName

    def AddNew(self, packageName: str, packageProjectId: str) -> None:
        if not Util.IsValidPackageName(packageName):
            raise InvalidPackageNameException(packageName)

        if packageProjectId in self.__projectIdToNameDict:
            raise Exception("Package '{0}' uses project id '{1}' already used by package: '{2}'".format(packageName, packageProjectId, self.__projectIdToNameDict[packageProjectId]))
        if packageName in self.__projectIdCache.ProjectIdDict:
            raise Exception("Package '{0}' already added".format(packageName))

        self.__projectIdCache.Add(packageName, packageProjectId)
        self.__projectIdToNameDict[packageProjectId] = packageName
