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

from typing import List
from typing import Optional
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Build.RequirementTreeNode import RequirementTreeNode
from FslBuildGen.Build import RequirementTreeUtil
from FslBuildGen.DataTypes import PackageRequirementTypeString
from FslBuildGen.ExtensionListManager import ExtensionListManager
from FslBuildGen.Log import Log
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.QualifiedRequirementExtensionName import QualifiedRequirementExtensionName


# TODO: much of the requirement code can be made simpler by using the 'supported' flag on the nodes instead
#       So we might as well do that in the future
class RequirementTree(object):
    def __init__(self, requirements: List[PackageRequirement]) -> None:
        # Create the various nodes and a
        rootNode = RequirementTreeNode(None)
        featureNodeDict = {}
        featureNodeDict[""] = rootNode
        for requirement in requirements:
            if requirement.Type == PackageRequirementTypeString.Feature:
                featureNodeDict[requirement.Name] = RequirementTreeNode(requirement)
        # Build the tree
        for requirement in requirements:
            node = featureNodeDict[requirement.Name] if requirement.Type == PackageRequirementTypeString.Feature else RequirementTreeNode(requirement)
            parentNode = featureNodeDict[requirement.Extends]
            parentNode.AddChild(node)
        self.RootNode = rootNode
        self.FeatureToNodeDict = featureNodeDict
        self.SourceRequirements = requirements


    def SetExtensionSupport(self, log: Log, qualifiedExtensionNameList: ExtensionListManager) -> None:
        RequirementTreeUtil.SetExtensionSupport(log, self.RootNode, self.FeatureToNodeDict, qualifiedExtensionNameList)


    def TryLocateExtensionNode(self, featureName: str, extensionName: str) -> Optional[RequirementTreeNode]:
        return RequirementTreeUtil.TryLocateExtensionNode(self.FeatureToNodeDict, featureName, extensionName)
