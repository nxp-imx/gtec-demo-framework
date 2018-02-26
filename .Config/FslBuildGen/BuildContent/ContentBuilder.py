#!/usr/bin/env python3

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

from typing import List
import shutil
from FslBuildGen import IOUtil
from FslBuildGen.Config import Config
from FslBuildGen.Log import Log
from FslBuildGen.BuildContent.ToolFinder import ToolFinder
from FslBuildGen.BuildContent.BasicContentProcessor import BasicContentProcessor
from FslBuildGen.BuildContent.ContentProcessor import ContentProcessor
from FslBuildGen.BuildContent.ContentRootRecord import ContentRootRecord
from FslBuildGen.BuildContent.PathRecord import PathRecord
from FslBuildGen.BuildContent.PathVariables import PathVariables
from FslBuildGen.BuildContent.Processor.ContentBuildCommandFile import ContentBuildCommandFile
from FslBuildGen.BuildContent.Processor.Commands import Command
from FslBuildGen.BuildContent.Processor.Commands import CommandContentBuildSync
from FslBuildGen.BuildContent.Processor.SourceContent import SourceContent
from FslBuildGen.BuildContent.Sync import BuildState
from FslBuildGen.BuildContent.Sync.Content import Content
from FslBuildGen.ToolConfig import ToolConfigContentBuilderConfiguration
#from FslBuildGen.BuildContent.VulkanContentProcessor import VulkanContentProcessor

__CONTENT_BUILD_DIR = "Content.bld"
__CONTENT_OUTPUT_DIR = "Content"

class Features(object):
    def __init__(self, config: Config, features: List[str]) -> None:
        super(Features, self).__init__()
        featureIds = [feature.lower() for feature in features]
        self.Features = features
        self.FeaturesIds = featureIds  # type: List[str]
        self.UseVulkan = "vulkan" in featureIds
        self.UseOpenGLES = "opengles2" in features or "opengles3" in featureIds or "opengles3.1" in featureIds


class Builder(object):
    def __init__(self, config: Config, packageBuildPath: str, contentBuildPath: str, contentOutputPath: str, features: Features, toolFinder: ToolFinder) -> None:
        super(Builder, self).__init__()
        configPathVariables = PathVariables(config, packageBuildPath, contentBuildPath, contentOutputPath)
        commandFilename = IOUtil.Join(contentBuildPath, "Content.json")
        commandFile = ContentBuildCommandFile(config, commandFilename, configPathVariables)

        # We don't include the files at 'Content' (contentOutputPath)
        sourceContent = SourceContent(config, contentOutputPath, contentBuildPath, commandFile, False, commandFilename)

        contentProcessors = []  # type: List[BasicContentProcessor]
#        contentProcessors = [VulkanContentProcessor()]
        contentProcessors += self.__AddBasicContentProcessors(config, toolFinder, config.ToolConfig.ContentBuilderConfiguration)
        contentProcessors = self.__FilterProcessorsBasedOnFeatures(contentProcessors, features)

        absoluteCacheFileName = IOUtil.Join(packageBuildPath, "_ContentBuildCache.fsl")
        absoluteOutputCacheFileName = IOUtil.Join(packageBuildPath, "_ContentBuildCacheOutput.fsl")

        srcsSyncState = BuildState.GenerateSyncState(config, absoluteCacheFileName, sourceContent.AllContentSource, True)
        outputSyncState = BuildState.GenerateOutputSyncState(config, absoluteOutputCacheFileName, contentOutputPath, True)

        if sourceContent.IsEmpty:
            config.LogPrint("No files found")
            return

        if not config.DisableWrite:
            IOUtil.SafeMakeDirs(contentOutputPath)

        self.__ProcessSyncFiles(config, contentBuildPath, contentOutputPath, sourceContent.ContentSource, srcsSyncState, outputSyncState)
        self.__ProcessContentFiles(config, contentBuildPath, contentOutputPath, toolFinder, contentProcessors, sourceContent.ContentBuildSource, srcsSyncState, outputSyncState)
        srcsSyncState.Save()
        outputSyncState.Save()


    def __AddBasicContentProcessors(self, config: Config, toolFinder: ToolFinder, contentBuilderConfiguration: ToolConfigContentBuilderConfiguration) -> List[BasicContentProcessor]:
        """ Add basic content builders from the tool config XML file """
        contentBuilders = []  # type: List[BasicContentProcessor]
        for contentBuilder in contentBuilderConfiguration.ContentBuilders:
            contentBuilders.append(BasicContentProcessor(config, toolFinder, contentBuilder))
        return contentBuilders


    def __FilterProcessorsBasedOnFeatures(self, contentProcessors: List[BasicContentProcessor], features: Features) -> List[BasicContentProcessor]:
        return [contentProcessor for contentProcessor in contentProcessors if contentProcessor.CheckFeatureRequirements(features.FeaturesIds)]


    def __GetSyncStateFileName(self, contentBuildPath: str, contentFile: str) -> str:
        if contentFile.startswith(contentBuildPath):
            contentFile = contentFile[len(contentBuildPath):]
            if contentFile.startswith('/'):
                contentFile = contentFile[1:]
        return contentFile


    def __ProcessSyncFiles(self, log: Log,
                           contentBuildPath: str,
                           contentOutputPath: str,
                           srcContent: Content,
                           syncState: BuildState.SyncState,
                           outputSyncState: BuildState.SyncState) -> None:
        dstRoot = ContentRootRecord(log, contentOutputPath)
        for contentFile in srcContent.Files:
            # Generate the output file record
            outputFileName = contentFile.RelativePath
            outputFileRecord = PathRecord(log, dstRoot, outputFileName)

            ## Query the sync state of the content file
            syncStateFileName = self.__GetSyncStateFileName(contentFile.SourceRoot.ResolvedPath, contentFile.RelativePath)
            contentState = syncState.TryGetFileStateByFileName(syncStateFileName)
            buildResource = not contentState or contentState.CacheState != BuildState.CacheState.Unmodified
            if not buildResource:
                # It was unmodified, so we need to examine the state of the output file to
                # determine if its safe to skip the building
                syncStateOutputFileName = self.__GetSyncStateFileName(contentOutputPath, outputFileName)
                outputContentState = outputSyncState.TryGetFileStateByFileName(syncStateOutputFileName)
                buildResource = not outputContentState or outputContentState.CacheState != BuildState.CacheState.Unmodified

            if buildResource:
                try:
                    log.LogPrintVerbose(2, "Copying '{0}' to '{1}'".format(contentFile.ResolvedPath, outputFileRecord.ResolvedPath))
                    dstDirPath = IOUtil.GetDirectoryName(outputFileRecord.ResolvedPath)
                    IOUtil.SafeMakeDirs(dstDirPath)
                    shutil.copy(contentFile.ResolvedPath, outputFileRecord.ResolvedPath)
                except:
                    # Save if a exception occured to prevent reprocessing the working files
                    outputSyncState.Save()
                    syncState.Save()
                    raise

                # Add a entry for the output file
                outputFileState = outputSyncState.BuildContentState(log, outputFileRecord, True, True)
                outputSyncState.Add(outputFileState)


    def __ProcessContentFiles(self, config: Config,
                              contentBuildPath: str,
                              contentOutputPath: str,
                              toolFinder: ToolFinder,
                              contentProcessors: List[BasicContentProcessor],
                              srcContent: Content,
                              syncState: BuildState.SyncState,
                              outputSyncState: BuildState.SyncState) -> None:
        dstRoot = ContentRootRecord(config, contentOutputPath)
        for contentFile in srcContent.Files:
            processors = self.__FindProcessors(contentProcessors, contentFile.ResolvedPath)
            if len(processors) > 1:
                contentProcessorNames = [processor.Name for processor in processors]
                raise Exception("Multiple content processors '%s' available for '%s'" % (contentProcessorNames, contentFile.ResolvedPath))
            if len(processors) == 1:
                # Query the processor for the output filename
                outputFileName = processors[0].GetOutputFileName(config, contentOutputPath, contentFile)
                outputFileRecord = PathRecord(config, dstRoot, outputFileName[len(dstRoot.ResolvedPath)+1:])

                # Query the sync state of the content file
                syncStateFileName = self.__GetSyncStateFileName(contentBuildPath, contentFile.RelativePath)
                contentState = syncState.TryGetFileStateByFileName(syncStateFileName)
                buildResource = contentState is None or contentState.CacheState != BuildState.CacheState.Unmodified
                if buildResource is not None:
                    # It was unmodified, so we need to examine the state of the output file to
                    # determine if its safe to skip the building
                    syncStateOutputFileName = self.__GetSyncStateFileName(contentOutputPath, outputFileName)
                    outputContentState = outputSyncState.TryGetFileStateByFileName(syncStateOutputFileName)
                    buildResource = (outputContentState is None or outputContentState.CacheState != BuildState.CacheState.Unmodified or
                                     (contentState is None or contentState.Checksum != outputContentState.TagChecksum))

                if buildResource:
                    try:
                        processors[0].Process(config, contentBuildPath, contentOutputPath, contentFile, toolFinder)
                    except:
                        # Save if a exception occured to prevent reprocessing the working files, but we invalidate
                        outputSyncState.Save()
                        syncState.Save()
                        raise

                    # Add a entry for the output file
                    outputFileState = outputSyncState.BuildContentState(config, outputFileRecord, True, True)
                    # Tag it with the source file checksum so we have another way to detect changes
                    if contentState is not None:
                        outputFileState.TagChecksum = contentState.Checksum;
                    outputSyncState.Add(outputFileState)


    def __FindProcessors(self, contentProcessors: List[BasicContentProcessor], contentFile: str) -> List[BasicContentProcessor]:
        extension = IOUtil.GetFileNameExtension(contentFile)[1:].lower()
        return [contentProcessor for contentProcessor in contentProcessors if extension in contentProcessor.FileExtensionSet]


def Build(config: Config, currentPath: str, featureList: List[str]) -> None:
    contentBuildDir = __CONTENT_BUILD_DIR
    contentOutputDir = __CONTENT_OUTPUT_DIR
    contentBuildPath = IOUtil.Join(currentPath, contentBuildDir)
    contentOutputPath = IOUtil.Join(currentPath, contentOutputDir)
    if not IOUtil.IsDirectory(contentBuildPath):
        config.LogPrintVerbose(1, "No '{0}' directory present at '{1}' so there is no content to process.".format(contentBuildDir, currentPath))
        return

    packageBuildPath = IOUtil.Join(currentPath, config.GetBuildDir())
    if not config.DisableWrite:
        IOUtil.SafeMakeDirs(packageBuildPath)

    toolFinder = ToolFinder(config)
    features = Features(config, featureList)
    Builder(config, packageBuildPath, contentBuildPath, contentOutputPath, features, toolFinder)
