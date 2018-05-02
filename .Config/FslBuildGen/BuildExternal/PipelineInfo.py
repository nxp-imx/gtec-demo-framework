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
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildExternal.PipelineTasks import PipelineTasks
from FslBuildGen.BuildExternal.RecipePathBuilder import RecipePathBuilder
from FslBuildGen.Packages.Package import Package


class PipelineInfo(object):
    def __init__(self, tasks: PipelineTasks, sourcePackage: Package,
                 pathBuilder: RecipePathBuilder,
                 srcRootPath: str, dstRootPath: str, allowDownloads: bool = True,
                 combinedDstRootPath: Optional[str] = None) -> None:
        super().__init__()
        if sourcePackage.ResolvedDirectExperimentalRecipe is None:
            raise Exception("Invalid recipe")
        self.SourcePackage = sourcePackage
        self.SourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
        self.SourcePipeline = sourcePackage.ResolvedDirectExperimentalRecipe.Pipeline
        self.PathBuilder = pathBuilder
        self.RecipeAbsolutePath = sourcePackage.AbsolutePath
        self.SrcRootPath = srcRootPath
        self.DstRootPath = dstRootPath
        self.CombinedDstRootPath = combinedDstRootPath if combinedDstRootPath is not None else dstRootPath
        self.Tasks = tasks
        self.AllowDownloads = allowDownloads
