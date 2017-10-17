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

from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from typing import overload
from typing import TypeVar
from typing import Union
from FslBuildGen.Build.RequirementTreeNode import RequirementTreeNode
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Info.AppInfoGlobalRequirementTreeNode import AppInfoGlobalRequirementTreeNode
from FslBuildGen.Log import Log
from FslBuildGen.QualifiedRequirementExtensionName import QualifiedRequirementExtensionName


def __RecursiveSetExtensionSupport(currentNode: Union[RequirementTreeNode, AppInfoGlobalRequirementTreeNode], supported: bool) -> None:
    if currentNode.Content is not None and currentNode.Content.Type == PackageRequirementTypeString.Extension:
        currentNode.Supported = supported
    for child in currentNode.Children:
        __RecursiveSetExtensionSupport(child, supported)


def __RecursiveEnableExtension(currentNode: Union[RequirementTreeNode, AppInfoGlobalRequirementTreeNode], extensionName: str, log: Optional[Log], strIndent: str) -> bool:
    found = False
    if currentNode.Content is not None and currentNode.Content.Type == PackageRequirementTypeString.Extension and currentNode.Content.Name == extensionName:
        currentNode.Supported = True
        found = True
        if log is not None:
            log.LogPrint("{0}{1}".format(strIndent, QualifiedRequirementExtensionName.ToString(currentNode.Content.Extends, currentNode.Content.Name)))
    for child in currentNode.Children:
        if __RecursiveEnableExtension(child, extensionName, log, strIndent):
            found = True
    return found


@overload
def SetExtensionSupport(log: Log, rootNode: RequirementTreeNode, featureToNodeDict: Dict[str, RequirementTreeNode],
                        qualifiedExtensionNameList: ExtensionListManager) -> None:
    pass


@overload
def SetExtensionSupport(log: Log, rootNode: AppInfoGlobalRequirementTreeNode, featureToNodeDict: Dict[str, AppInfoGlobalRequirementTreeNode],
                        qualifiedExtensionNameList: ExtensionListManager) -> None:
    pass


def SetExtensionSupport(log: Log, rootNode: Union[RequirementTreeNode, AppInfoGlobalRequirementTreeNode],
                        featureToNodeDict: Union[Dict[str, RequirementTreeNode], Dict[str, AppInfoGlobalRequirementTreeNode]],
                        qualifiedExtensionNameList: ExtensionListManager) -> None:
    if qualifiedExtensionNameList.AllowAllExtensions:
        log.LogPrintWarning("SetExtensionSupport should not be called when all extensions are allowed")
        return
    log.LogPrint("Setting supported extensions")

    verboseBasicConfig = None if log.Verbosity <= 1 else log

    # Set all extension nodes to unsupported
    __RecursiveSetExtensionSupport(rootNode, False)

    for qualifiedExtensionNameRecord in qualifiedExtensionNameList.Content:
        if qualifiedExtensionNameRecord.FeatureName in featureToNodeDict:
            log.LogPrint("- Enabling extensions using {0}".format(qualifiedExtensionNameRecord))
            # we found the feature
            foundFeatureNode = featureToNodeDict[qualifiedExtensionNameRecord.FeatureName]
            if not __RecursiveEnableExtension(foundFeatureNode, qualifiedExtensionNameRecord.ExtensionName, verboseBasicConfig, "  - "):
                log.LogPrint("WARNING: Unknown extension name {0} used in {1}".format(qualifiedExtensionNameRecord.ExtensionName, qualifiedExtensionNameRecord))
        else:
            log.LogPrint("WARNING: Unknown feature name {0} used in {1}".format(qualifiedExtensionNameRecord.FeatureName, qualifiedExtensionNameRecord))



T = TypeVar('T')

@overload
def TryLocateExtensionNode(featureToNodeDict: Dict[str, RequirementTreeNode], featureName: str, extensionName: str) -> Optional[T]:
    pass

@overload
def TryLocateExtensionNode(featureToNodeDict: Dict[str, AppInfoGlobalRequirementTreeNode], featureName: str, extensionName: str) -> Optional[T]:
    pass

def TryLocateExtensionNode(featureToNodeDict: Union[Dict[str, RequirementTreeNode], Dict[str, AppInfoGlobalRequirementTreeNode]], featureName: str, extensionName: str) -> Optional[T]:
    if not featureName in featureToNodeDict:
        return None
    currentNode = featureToNodeDict[featureName]  # type: Optional[Union[RequirementTreeNode, AppInfoGlobalRequirementTreeNode]]
    while currentNode is not None:
        for child in currentNode.Children:
            if child.Content is not None and child.Content.Type == PackageRequirementTypeString.Extension and child.Content.Name == extensionName:
                return cast(Optional[T], child)
        currentNode = currentNode.Parent
    return None
