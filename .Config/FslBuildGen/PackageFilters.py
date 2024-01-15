#!/usr/bin/env python3

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
from FslBuildGen.DataTypes import FilterMethod
from FslBuildGen.ExtensionListManager2 import ExtensionListManager2
from FslBuildGen.RecipeFilterManager import RecipeFilterManager

class PackageFilters(object):
    def __init__(self, featureNameList: Optional[List[str]] = None,
                 extensionNameList: Optional[ExtensionListManager2] = None,
                 requiredFeatureNameList: Optional[List[str]] = None,
                 recipeList: Optional[RecipeFilterManager] = None) -> None:
        self.FeatureNameList = ['*'] if featureNameList is None else featureNameList                                                    # type: List[str]
        self.ExtensionNameList = ExtensionListManager2(FilterMethod.AllowAll, []) if extensionNameList is None else extensionNameList   # type: ExtensionListManager2
        self.RequiredFeatureNameList = ['*'] if requiredFeatureNameList is None else requiredFeatureNameList                            # type: List[str]
        self.RecipeFilterManager = RecipeFilterManager(True, []) if recipeList is None else recipeList                                  # type: RecipeFilterManager
        self.ExePackageNameFilter = None # type: Optional[str]

    def ContainsRequirementTypeFilters(self) -> bool:
        return (not self.ExtensionNameList.FilterMethod == FilterMethod.AllowAll or
                not '*' in self.FeatureNameList or
                not '*' in self.RequiredFeatureNameList or
                not self.RecipeFilterManager.AllRecipesEnabled)
