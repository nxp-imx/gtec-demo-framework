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

from typing import Optional
from typing import List
from FslBuildGen import PackageUtil
from FslBuildGen.BuildExternal.DataTypes import RecipeType
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package

class RecipeInfo(object):
    @staticmethod
    def ShowRecipeList(log: Log, topLevelPackage: Package, requestedFiles: Optional[List[str]]) -> None:
        requestedPackages = PackageUtil.GetPackageListFromFilenames(topLevelPackage, requestedFiles)

        recipeBuildOrder = topLevelPackage.ResolvedExperimentalRecipeBuildOrder
        if recipeBuildOrder is None or len(recipeBuildOrder) <= 0:
            log.LogPrint("No recipes found")
            return

        recipePackages = list(topLevelPackage.ResolvedExperimentalRecipeBuildOrder)
        recipePackages.sort(key=lambda s: s.ResolvedDirectExperimentalRecipe.Name.lower() if s.ResolvedDirectExperimentalRecipe is not None else s.Name.lower())
        strAddIndent = "  "
        print("Recipes")
        for package in recipePackages:
            packageRecipe = package.ResolvedDirectExperimentalRecipe
            if packageRecipe is not None and packageRecipe.Type != RecipeType.External:
                print("{0}{1} (introduced by: {2}), type: {3}".format(strAddIndent, packageRecipe.Name, package.Name, RecipeType.ToString(packageRecipe.Type)))

        print("External")
        for package in recipePackages:
            packageRecipe = package.ResolvedDirectExperimentalRecipe
            if packageRecipe is not None and packageRecipe.Type == RecipeType.External:
                print("{0}{1} (introduced by: {2})".format(strAddIndent, packageRecipe.Name, package.Name))
