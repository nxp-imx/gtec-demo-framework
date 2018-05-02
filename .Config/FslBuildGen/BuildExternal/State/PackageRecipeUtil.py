#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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
from typing import Union
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.DataTypes import BuildRecipePipelineCommand
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineBuildCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommandCombine

class PackageRecipeUtil(object):

    @staticmethod
    def HasBuildPipeline(sourcePackage: Package) -> bool:
        if sourcePackage is None or sourcePackage.ResolvedDirectExperimentalRecipe is None or sourcePackage.ResolvedDirectExperimentalRecipe.ResolvedInstallPath is None:
            return False
        return not sourcePackage.ResolvedDirectExperimentalRecipe.Pipeline is None

    @staticmethod
    def CommandListContainsBuildCMake(commandList: Union[List[XmlRecipePipelineCommand], List[XmlRecipePipelineBuildCommand]]) -> bool:
        for command in commandList:
            if command.CommandType == BuildRecipePipelineCommand.CMakeBuild:
                return True
            if command.CommandType == BuildRecipePipelineCommand.Combine:
                if not isinstance(command, XmlRecipePipelineCommandCombine):
                    raise Exception("BuildRecipePipelineCommand.Combine was not of type XmlRecipePipelineCommandCombine")
                return PackageRecipeUtil.CommandListContainsBuildCMake(command.CommandList)
        return False

    @staticmethod
    def ContainsBuildCMake(recipe: PackageExperimentalRecipe) -> bool:
        if recipe.Pipeline is None or recipe.Pipeline.CommandList is None:
            return False
        return PackageRecipeUtil.CommandListContainsBuildCMake(recipe.Pipeline.CommandList)
