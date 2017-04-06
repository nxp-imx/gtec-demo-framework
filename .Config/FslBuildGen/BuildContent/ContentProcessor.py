#!/usr/bin/env python

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

from FslBuildGen import IOUtil, Config, PlatformUtil
from FslBuildGen.DataTypes import BuildPlatformType
from FslBuildGen.BuildContent.ConditionInterpreter import ConditionInterpreter
from FslBuildGen.BuildContent.ToolFinder import *


class ContentProcessor(object):
    def __init__(self, name, featureRequirements, fileExtensionSet):
        super(ContentProcessor, self).__init__()
        self.Name = name
        self.FileExtensionSet = fileExtensionSet
        self.__FeatureConditionInterpreter = ConditionInterpreter(name, featureRequirements);

    def GetTempFileName(self, contentPath, contentFile):
        fileName = IOUtil.GetFileName(contentFile) + ".tmp"
        tempFileName = IOUtil.Join(contentPath, fileName)
        while IOUtil.IsFile(tempFileName):
            fileName = '_' + fileName
            tempFileName = IOUtil.Join(contentPath, fileName)
        return tempFileName


    def GetOutputFileName(self, config, contentBuildPath, contentOutputPath, contentFile, removeExtension):
        relativePathToContentFile = contentFile[len(contentBuildPath)+1:]
        if removeExtension:
            extension = IOUtil.GetFileNameExtension(relativePathToContentFile)
            relativePathToContentFile = relativePathToContentFile[:-len(extension)]
        return IOUtil.Join(contentOutputPath, relativePathToContentFile)


    def EnsureDirectoryExist(self, config, outputFileName):
        outputDirectory = IOUtil.GetDirectoryName(outputFileName)
        if not config.DisableWrite:
            IOUtil.SafeMakeDirs(outputDirectory)

    def CheckFeatureRequirements(self, featuresIds):
        return self.__FeatureConditionInterpreter.CheckFeatureRequirements(featuresIds)
