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
from FslBuildGen.BuildContent.ToolFinder import ToolFinder
from FslBuildGen.BuildContent.BasicContentProcessor import BasicContentProcessor
from FslBuildGen.BuildContent.VulkanContentProcessor import VulkanContentProcessor
from FslBuildGen.BuildContent import BuildState
import subprocess

__CONTENT_BUILD_DIR = "Content.bld"
__CONTENT_OUTPUT_DIR = "Content"

class Features(object):
    def __init__(self, config, features):
        super(Features, self).__init__()
        featureIds = [feature.lower() for feature in features]
        self.Features = features
        self.FeaturesIds = featureIds
        self.UseVulkan = "vulkan" in featureIds
        self.UseOpenGLES = "opengles2" in features or "opengles3" in featureIds


class Builder(object):
    def __init__(self, config, packageBuildPath, contentBuildPath, contentOutputPath, features, toolFinder):
        super(Builder, self).__init__()
        contentProcessors = []
#        contentProcessors = [VulkanContentProcessor()]
        contentProcessors += self.__AddBasicContentProcessors(config, toolFinder, config.ToolConfig.ContentBuilderConfiguration)
        contentProcessors = self.__FilterProcessorsBasedOnFeatures(contentProcessors, features)

        absoluteCacheFileName = IOUtil.Join(packageBuildPath, "_ContentBuildCache.fsl")
        absoluteOutputCacheFileName = IOUtil.Join(packageBuildPath, "_ContentBuildCacheOutput.fsl")
        syncState = BuildState.GenerateSyncState(config, absoluteCacheFileName, contentBuildPath, True)
        outputSyncState =  BuildState.GenerateOutputSyncState(config, absoluteOutputCacheFileName, contentOutputPath, True)

        contentFiles = IOUtil.GetFilePaths(contentBuildPath, None)
        if len(contentFiles) <= 0:
            config.LogPrint("No files found");
            return

        if not config.DisableWrite:
            IOUtil.SafeMakeDirs(contentOutputPath)

        self.__ProcessFiles(config, contentBuildPath, contentOutputPath, toolFinder, contentProcessors, contentFiles, syncState, outputSyncState)
        syncState.Save()
        outputSyncState.Save()


    def __AddBasicContentProcessors(self, config, toolFinder, contentBuilderConfiguration):
        """ Add basic content builders from the tool config XML file """
        contentBuilders = []
        for contentBuilder in contentBuilderConfiguration.ContentBuilders:
            contentBuilders.append(BasicContentProcessor(config, toolFinder, contentBuilder))
        return contentBuilders


    def __FilterProcessorsBasedOnFeatures(self, contentProcessors, features):
        return [contentProcessor for contentProcessor in contentProcessors if contentProcessor.CheckFeatureRequirements(features.FeaturesIds)]


    def __GetSyncStateFileName(self, contentBuildPath, contentFile):
        if contentFile.startswith(contentBuildPath):
            contentFile = contentFile[len(contentBuildPath):]
            if contentFile.startswith('/'):
                contentFile = contentFile[1:]
        return contentFile


    def __ProcessFiles(self, config, contentBuildPath, contentOutputPath, toolFinder, contentProcessors, contentFiles, syncState, outputSyncState):
        for contentFile in contentFiles:
            processors = self.__FindProcessors(contentProcessors, contentFile)
            if len(processors) > 1:
                contentProcessorNames = [processor.Name for processor in processors]
                raise Exception("Multiple content processors '%s' available for '%s'" % (contentProcessorNames, contentFile));
            if len(processors) == 1:
                # Query the processor for the output filename
                outputFileName = processors[0].GetOutputFileName(config, contentBuildPath, contentOutputPath, contentFile)

                # Query the sync state of the content file
                syncStateFileName = self.__GetSyncStateFileName(contentBuildPath, contentFile)
                contentState = syncState.GetFileStateByFileName(syncStateFileName)
                buildResource = not contentState or contentState.CacheState != BuildState.CacheState.Unmodified
                if not buildResource:
                    # It was unmodified, so we need to examine the state of the output file to 
                    # determine if its safe to skip the building
                    syncStateOutputFileName = self.__GetSyncStateFileName(contentOutputPath, outputFileName)
                    outputContentState = outputSyncState.GetFileStateByFileName(syncStateOutputFileName)
                    buildResource = not outputContentState or outputContentState.CacheState != BuildState.CacheState.Unmodified

                if buildResource:
                    try:
                        processors[0].Process(config, contentBuildPath, contentOutputPath, contentFile, toolFinder)
                    except:
                        # Save if a exception occured to prevent reprocessing the working files
                        outputSyncState.Save()
                        syncState.Save()
                        raise

                    # Add a entry for the output file
                    outputFileState = outputSyncState.BuildContentState(config, outputFileName, True, True)
                    outputSyncState.Add(outputFileState)


    def __FindProcessors(self, contentProcessors, contentFile):
        extension = IOUtil.GetFileNameExtension(contentFile)[1:].lower()
        return [contentProcessor for contentProcessor in contentProcessors if extension in contentProcessor.FileExtensionSet]


def Build(config, currentPath, featureList):
    contentBuildDir = __CONTENT_BUILD_DIR
    contentOutputDir = __CONTENT_OUTPUT_DIR
    contentBuildPath = IOUtil.Join(currentPath, contentBuildDir)
    contentOutputPath = IOUtil.Join(currentPath, contentOutputDir)
    if not IOUtil.IsDirectory(contentBuildPath):
        config.LogPrint("No '%s' directory present at '%s' so there is no content to process." % (contentBuildDir, currentPath));
        return

    packageBuildPath = IOUtil.Join(currentPath, config.GetBuildDir())
    if not config.DisableWrite:
        IOUtil.SafeMakeDirs(packageBuildPath)

    toolFinder = ToolFinder(config)
    features = Features(config, featureList)
    Builder(config, packageBuildPath, contentBuildPath, contentOutputPath, features, toolFinder)
