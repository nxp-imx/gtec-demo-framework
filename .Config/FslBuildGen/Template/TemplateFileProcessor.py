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

from typing import Callable
from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.Config import Config
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Template.TemplateEnvironment import TemplateEnvironment
from FslBuildGen.Template.TemplateFileRecord import TemplateFileRecord
from FslBuildGen.Template.TemplateFileRecordManager import TemplateFileRecordManager


class TemplateFileProcessor(object):
    def __init__(self, config: Config, platformName: str, genFileOnly: bool = False) -> None:
        super().__init__()
        self.GenFileOnly = genFileOnly
        self.Environment = TemplateEnvironment(config, platformName)


    def Process(self, config: Config,
                templateFileRecordManager: TemplateFileRecordManager,
                dstPath: str,
                package: Optional[Package],
                dstFilenameModifier: Optional[Callable[[str], str]] = None) -> None:
        if package is not None:
            self.Environment.SetPackage(package, dstPath)

        # create folder structure
        IOUtil.SafeMakeDirs(dstPath)
        # do the actual copy
        self.__CopyFiles(config, dstPath, templateFileRecordManager.FilesToCopy)
        self.__CopyAndModifyFiles(config, dstPath, templateFileRecordManager.FilesToModify, dstFilenameModifier)


    def __CopyAndModifyFiles(self, config: Config,
                             dstPath: str,
                             filesToModify: List[TemplateFileRecord],
                             dstFilenameModifier: Optional[Callable[[str], str]]) -> None:
        for file in filesToModify:
            if not self.GenFileOnly or file.FileName == config.ToolConfig.GenFileName:
                dstFilename = IOUtil.Join(dstPath, file.RelativeDestPath)
                content = IOUtil.ReadFile(file.AbsoluteSourcePath)

                # Replace various strings using the 'template environment'
                for key, value in self.Environment.Dict.items():
                    strValue = "" if value is None else value
                    content = content.replace(key, strValue)
                    dstFilename = dstFilename.replace(key, strValue)

                if dstFilenameModifier is not None:
                    dstFilename = dstFilenameModifier(dstFilename)

                dirName = IOUtil.GetDirectoryName(dstFilename)
                if not config.DisableWrite:
                    IOUtil.SafeMakeDirs(dirName)
                    IOUtil.WriteFileIfChanged(dstFilename, content)


    def __CopyFiles(self, config: Config, dstPath: str, filesToCopy: List[TemplateFileRecord]) -> None:
        for file in filesToCopy:
            if not self.GenFileOnly or file.FileName == config.ToolConfig.GenFileName:
                dst = IOUtil.Join(dstPath, file.RelativeDestPath)
                dirName = IOUtil.GetDirectoryName(dst)
                if not config.DisableWrite:
                    IOUtil.SafeMakeDirs(dirName)
                    IOUtil.CopySmallFile(file.AbsoluteSourcePath, dst)
