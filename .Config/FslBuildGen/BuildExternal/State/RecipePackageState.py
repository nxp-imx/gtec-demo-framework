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
import hashlib
from FslBuildGen import IOUtil
from FslBuildGen.BuildContent.Sync.Content import Content
from FslBuildGen.BuildContent.Sync import BuildState
from FslBuildGen.BuildExternal.State.JsonRecipePackageContentState import JsonRecipePackageContentState
from FslBuildGen.Log import Log


class RecipePackageState(object):
    def __init__(self, log: Log, packageName: str, path: str, cacheFilename: str, sourceHash: str,
                 cachedContentState: Optional[JsonRecipePackageContentState]) -> None:
        super().__init__()
        # Reuse the BuildContent.Sync
        content = Content(log, path, True)
        cacheFile = IOUtil.Join(path, cacheFilename)

        cachedSyncState = self.__GetCachedSyncState(cachedContentState)
        syncState = BuildState.GenerateSyncState2(log, cacheFile, content, cachedSyncState, True)
        self.PackageName = packageName
        self.Path = path
        self.ContentState = self.__ConvertFromSync(syncState.ExtractContent())
        self.ContentStateHash = self.__GenerateHash(sourceHash, self.ContentState)

    def  __GetCachedSyncState(self, cachedContentState: Optional[JsonRecipePackageContentState]) -> Optional[BuildState.SyncState]:
        if cachedContentState is None:
            return None

        basicContent = self.__ConvertToSync(cachedContentState)
        cachedSyncState = BuildState.SyncState("dummy")
        cachedSyncState.RestoreContent(basicContent)
        return cachedSyncState


    def __ConvertToSync(self, contentState: JsonRecipePackageContentState) -> BuildState.BasicContent:
        contentState.Sort()
        # From local state objects to BuildState.BasicContent
        res = BuildState.BasicContent()
        for entry in contentState.Directories:
            res.Directories.append(entry)
        for state in contentState.Files:
            res.AddFile(state.Name, state.Length, state.ModifiedDate, state.Checksum, state.TagChecksum)
        return res


    def __ConvertFromSync(self, contentState: BuildState.BasicContent) -> JsonRecipePackageContentState:
        # Convert to local state objects so our saved json dont accidentially get affected by changes to
        # BuildState.BasicContent and BuildState.BasicContentState
        res = JsonRecipePackageContentState()
        for entry in contentState.Directories:
            res.Directories.append(entry)
        for state in contentState.Files:
            res.AddFile(state.Name, state.Length, state.ModifiedDate, state.Checksum, state.TagChecksum)
        res.Sort()
        return res


    def __GenerateHash(self, sourceHash: str, contentState: JsonRecipePackageContentState) -> str:
        allFilesChecksum = sourceHash
        for fileState in contentState.Files:
            allFilesChecksum += fileState.Checksum
        encoded = allFilesChecksum.encode();
        #hashed = hashlib.sha512(encoded)
        hashed = hashlib.sha256(encoded)
        return hashed.hexdigest()

