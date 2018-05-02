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

import json
from FslBuildGen import IOUtil
from FslBuildGen.BuildExternal.State.BuildAreaInfoFile import BuildAreaInfoFile
from FslBuildGen.BuildExternal.State.JsonDictType import JsonDictType
from FslBuildGen.Log import Log


class BuildAreaInfoFileUtil(object):
    @staticmethod
    def SaveInstallAreaInfo(dstFilePath: str, sdkPath: str) -> JsonDictType:
        jsonRootDict = BuildAreaInfoFile.CreateDict(sdkPath)
        jsonText = json.dumps(jsonRootDict, ensure_ascii=False, sort_keys=True, indent=2)
        IOUtil.WriteFileIfChanged(dstFilePath, jsonText)
        return jsonRootDict

    @staticmethod
    def ClaimInstallDirNow(log: Log,
                           targetPath: str,
                           dstFilePath: str,
                           sdkPath: str,
                           forceClaimInstallArea: bool,
                           logWarning: bool = True) -> JsonDictType:
        # Since we are claiming the install area, we check to see if its empty as expected
        files = IOUtil.GetFilePaths(targetPath, None)
        # Then give the user error about the files at the install area can be lost unless it is changed
        if len(files) > 0:
            if not forceClaimInstallArea:
                raise Exception("The install area at '{0}' was unclaimed, but it was not empty. To allow the tool to use the directory and do with its content as it see fit.\nYou need to rerun the command with the --ForceClaimInstallArea parameter to allow it, but BEWARE that doing so means the content of '{0}' can be lost".format(targetPath))
            if logWarning:
                log.DoPrintWarning("The install area was not empty but the user enabled --ForceClaimInstallArea and allowed the tool to use it, the files there could be lost because of it.")

        # then save the claim file
        return BuildAreaInfoFileUtil.SaveInstallAreaInfo(dstFilePath, sdkPath)


    @staticmethod
    def ProcessInstallDirClaim(log: Log,
                               targetPath: str,
                               sdkPath: str,
                               forceClaimInstallArea: bool, 
                               installAreaInfoPath: str) -> None:
        filePath = IOUtil.Join(targetPath, installAreaInfoPath)

        # Beware that this method to claim a area is in no way secure
        # It will basically only be useful after the claim file has been created
        # So we start with a possible race condition, but any future attempts to use the directory
        # will catch that multiple repos are trying to reuse the same install area
        fileContent = IOUtil.TryReadFile(filePath)
        if fileContent is None:
            log.LogPrint("Install area '{0}' is unclaimed, claiming it".format(targetPath))
            BuildAreaInfoFileUtil.ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea)
            return

        jsonBuildInfoDict = json.loads(fileContent)
        if not BuildAreaInfoFile.IsDictValid(jsonBuildInfoDict):
            if not forceClaimInstallArea:
                raise Exception("Install area '{0}' contained an invalid file '{1}', did you try to run concurrent builds using the same install directory or did it get corrupted? Delete the file to allow the build to continue or use --ForceClaimInstallArea to do so automatically. You could also set up a readonly cache area to reuse between repos see the documentation for more info.".format(targetPath, filePath))
            log.DoPrintWarning("Install area '{0}' contained an invalid file '{1}', as --ForceClaimInstallArea was specified it was overwritten and '{2}' now controls it".format(targetPath, filePath, sdkPath))
            jsonBuildInfoDict = BuildAreaInfoFileUtil.ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea, logWarning=False)

        buildAreaInfoFile = BuildAreaInfoFile(jsonBuildInfoDict)
        if buildAreaInfoFile.SDKPath != sdkPath:
            if not forceClaimInstallArea:
                raise Exception("The Install area at '{0}' is already claimed by the sdk at '{1}' so the sdk at '{2}' can not reuse it as it could give concurrency issues if multiple builds execute at the same time. If you are sure that you are not doing concurrent builds and you just want to use the area for a new SDK you can force claim it with --ForceClaimInstallArea. You could also set up a readonly cache area to reuse between repos see the documentation for more info.".format(targetPath, buildAreaInfoFile.SDKPath, sdkPath))
            log.DoPrintWarning("The Install area at '{0}' was already claimed by the sdk at '{1}' but '{2}' took control of it as --ForceClaimInstallArea was specified.".format(targetPath, buildAreaInfoFile.SDKPath, sdkPath))
            jsonBuildInfoDict = BuildAreaInfoFileUtil.ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea, logWarning=False)
            #jsonBuildInfoDict = BuildAreaInfoFileUtil.ClaimInstallDirNow(log, targetPath, filePath, sdkPath, forceClaimInstallArea, logWarning=False)
            #buildAreaInfoFile = BuildAreaInfoFile(jsonBuildInfoDict)
