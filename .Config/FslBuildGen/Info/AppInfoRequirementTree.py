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
#from typing import Set
#from typing import Union
from FslBuildGen.Build import RequirementTreeUtil
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Info.AppInfo import AppInfo
from FslBuildGen.Info.AppInfoGlobalRequirementTreeNode import AppInfoGlobalRequirementTreeNode
from FslBuildGen.Info.PackageInfo import PackageInfo
from FslBuildGen.Info.RequirementInfo import RequirementInfo
from FslBuildGen.Info.RequirementInfo import RequirementType
from FslBuildGen.Log import Log

g_rootRequirement = RequirementInfo("", RequirementType.Undefined, None, "")

class AppInfoGlobalRequirementTree(object):
    def __init__(self, log: Log) -> None:
        self.Log = log
        self.TreeDict = {}              # type: Dict[str, AppInfoGlobalRequirementTreeNode]
        self.FeatureToNodeDict = {}     # type: Dict[str, AppInfoGlobalRequirementTreeNode]
        self.__RootNode = AppInfoGlobalRequirementTreeNode(g_rootRequirement)


    def Merge(self, treeDict: Dict[str, RequirementInfo]) -> None:
        # First we add all new nodes, then we link up the parent with its children
        newNodes = []
        for key, value in treeDict.items():
            if key in self.TreeDict:
                self.__CheckForConflicts(self.TreeDict[key].Content, value)
            else:
                newNode = AppInfoGlobalRequirementTreeNode(value)
                newNodes.append(newNode)
                self.TreeDict[key] = newNode
                self.FeatureToNodeDict[newNode.Content.Name] = newNode

        for newNode in newNodes:
            if newNode.Content.Extends:
                parentNode = self.FeatureToNodeDict[newNode.Content.Extends]
                parentNode.AddChild(newNode)
            else:
                self.__RootNode.AddChild(newNode)


    def __CheckForConflicts(self, src1: RequirementInfo, src2: RequirementInfo) -> None:
        if not src1.IsContentEqual(src2):
            strHelp = src1.DebugGetDifferenceString(src2)
            raise Exception("The requirement {0} already exist but there are differences {1}".format(src1.Name, strHelp))


    def SetExtensionSupport(self, log: Log, qualifiedExtensionNameList: ExtensionListManager) -> None:
        RequirementTreeUtil.SetExtensionSupport(log, self.__RootNode, self.FeatureToNodeDict, qualifiedExtensionNameList)


    def TryLocateExtensionNode(self, featureName: str, extensionName: str) -> Optional[AppInfoGlobalRequirementTreeNode]:
        return RequirementTreeUtil.TryLocateExtensionNode(self.FeatureToNodeDict, featureName, extensionName)


class AppInfoRequirementTree(object):
    def __init__(self, log: Log, appInfoDict: Dict[str, AppInfo], activePlatformNameId: str) -> None:
        """ Look at the available AppInfo and generate a complete merged requirement tree """
        self.Log = log
        appInfoGlobalRequirementTree = AppInfoGlobalRequirementTree(log)
        self.__ProcessAppInfoDict(appInfoGlobalRequirementTree, appInfoDict, activePlatformNameId)

        self.GlobalTree = appInfoGlobalRequirementTree


    def __ProcessAppInfoDict(self, rAppInfoGlobalRequirementTree: AppInfoGlobalRequirementTree,
                             appInfoDict: Dict[str, AppInfo], activePlatformId: str) -> None:
        for filename, appInfo in appInfoDict.items():
            self.__TryProcessAppInfo(rAppInfoGlobalRequirementTree, appInfo, filename, activePlatformId)


    def __TryProcessAppInfo(self, rAppInfoGlobalRequirementTree: AppInfoGlobalRequirementTree,
                            appInfo: AppInfo, sourceFilename: str, activePlatformId: str) -> None:
        # Verify the platform
        if appInfo.PlatformName.lower() != activePlatformId:
            self.Log.LogPrint("Skipping '{0}' as it was for '{1}' instead of the expected '{2}'".format(sourceFilename, appInfo.PlatformName, activePlatformId))
            return None

        self.__ProcessPackageList(rAppInfoGlobalRequirementTree, appInfo.ResolvedPackageList, sourceFilename)
        return None


    def __ProcessPackageList(self, rAppInfoGlobalRequirementTree: AppInfoGlobalRequirementTree,
                             resolvedPackageInfoList: List[PackageInfo], sourceFilename: str) -> None:
        for packageInfo in resolvedPackageInfoList:
            self.__ProcessPackageInfo(rAppInfoGlobalRequirementTree, packageInfo, sourceFilename)


    def __ProcessPackageInfo(self, rAppInfoGlobalRequirementTree: AppInfoGlobalRequirementTree,
                             packageInfo: PackageInfo, sourceFilename: str) -> None:
        requirementsDict = self.__ProcessAllRequirements(packageInfo.AllRequirements, sourceFilename)
        rAppInfoGlobalRequirementTree.Merge(requirementsDict)


    def __ProcessAllRequirements(self, allRequirementList: List[RequirementInfo], sourceFilename: str) -> Dict[str, RequirementInfo]:
        treeDict = {"": g_rootRequirement} # type: Dict[str, RequirementInfo]

        laterQueue = list(allRequirementList)
        previousCount = len(laterQueue) + 1

        # Here we basically validate the requirements 'Extends' by ensuring that we can build the tree
        while len(laterQueue) > 0 and len(laterQueue) != previousCount:
            previousCount = len(laterQueue)
            queue = laterQueue
            laterQueue = []
            while(len(queue) > 0):
                current = queue.pop()
                if current.Extends is None or current.Extends in treeDict:
                    if current.Name in treeDict:
                        raise Exception("The requirement '{0}' already exist".format(current.Name))
                    treeDict[current.Name] = current
                else:
                    # add it to the queue forlater
                    laterQueue.append(current)

        if len(laterQueue) > 0:
            invalidNameList = ["{0} extends {1}".format(entry.Name, entry.Extends) for entry in laterQueue]
            raise Exception("The requirements contained requirements with invalid extends {0} in file '{1}'".format(invalidNameList, sourceFilename))
        return treeDict
