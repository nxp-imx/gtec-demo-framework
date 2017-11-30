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
from FslBuildGen.Log import Log
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildContent.Processor.Commands import CommandContentBuildSync
from FslBuildGen.BuildContent.Processor.Commands import CommandContentSync
from FslBuildGen.BuildContent.Processor.ContentBuildCommandFile import ContentBuildCommandFile
from FslBuildGen.BuildContent.Sync.Content import Content

class SourceContent(object):
    def __init__(self, log: Log, contentPath: str, contentSourcePath: str, contentBuildCommandFile: ContentBuildCommandFile,
                 includeContentPathContent: bool,
                 removeCommandFilename: Optional[str] = None) -> None:
        super(SourceContent, self).__init__()

        contentSourceFiles = []  # type: List[PathRecord]
        contentBuildSourceFiles = []  # type: List[PathRecord]
        for command in contentBuildCommandFile.Commands:
            if isinstance(command, CommandContentBuildSync):
                for entry in command.Files:
                    contentBuildSourceFiles.append(entry)
            elif isinstance(command, CommandContentSync):
                for entry in command.Files:
                    contentSourceFiles.append(entry)
            else:
                log.LogPrint("WARNING: Unknown command type: {0}".format(command))

        self.ContentSource = Content(log, contentPath, includeContentPathContent, contentSourceFiles)
        self.ContentBuildSource = Content(log, contentSourcePath, True, contentBuildSourceFiles)
        self.AllContentSource = Content(log, contentSourcePath, True, contentBuildSourceFiles + contentSourceFiles)

        if removeCommandFilename is not None:
            self.ContentBuildSource.RemoveFileByResolvedSourcePath(removeCommandFilename)
            self.AllContentSource.RemoveFileByResolvedSourcePath(removeCommandFilename)

        self.IsEmpty = len(self.ContentSource.Files) <= 0 and len(self.ContentBuildSource.Files) <= 0
