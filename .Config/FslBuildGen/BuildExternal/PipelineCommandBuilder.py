#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from typing import Optional
from typing import List
from typing import Union
import hashlib
import shutil
import shlex
import time
from FslBuildGen import IOUtil
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.BuildExternal.PackageExperimentalRecipe import PackageExperimentalRecipe
from FslBuildGen.BuildExternal.RecipePathBuilder import RecipePathBuilder
from FslBuildGen.BuildExternal.Tasks import BasicTask
from FslBuildGen.BuildExternal.Tasks import CMakeAndBuildTask
from FslBuildGen.BuildExternal.Tasks import DownloadTask
from FslBuildGen.BuildExternal.Tasks import GitCloneTask
from FslBuildGen.BuildExternal.Tasks import GitApplyTask
from FslBuildGen.BuildExternal.Tasks import UnpackAndRenameTask
from FslBuildGen.DataTypes import BuildRecipePipelineCommand
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineBasicCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineFetchCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineFetchCommandDownload
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineFetchCommandGitClone
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineFetchCommandSource
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommandUnpack
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommandCMakeBuild
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineCommandClone
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandCopy
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandGitApply
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipePipelineJoinCommandUnpack



class PipelineTasks:
    def __init__(self, log: Log, generatorContext: GeneratorContext, checkBuildCommands: bool, buildThreads: int) -> None:
        self.__Log = log
        # Add the task objects
        self.TaskCMakeAndBuild = self.__TryAllocateCMakeAndBuildTask(generatorContext, checkBuildCommands, buildThreads)
        self.TaskGitClone = self.__TryAllocateGitCloneTask(generatorContext, checkBuildCommands)
        self.TaskGitApply = self.__TryAllocateGitApplyTask(generatorContext, checkBuildCommands)
        self.TaskDownload = DownloadTask(generatorContext)
        self.TaskUnpackAndRename = UnpackAndRenameTask(generatorContext)

    def __TryAllocateCMakeAndBuildTask(self, generatorContext: GeneratorContext, checkBuildCommands: bool, buildThreads: int) -> Optional[CMakeAndBuildTask]:
        try:
            return CMakeAndBuildTask(generatorContext, buildThreads)
        except Exception as ex:
            if checkBuildCommands:
                raise
            if self.__Log.Verbosity >= 1:
                self.__Log.LogPrintWarning("CMakeAndBuild is unavailable: {0}".format(str(ex)))
            return None


    def __TryAllocateGitCloneTask(self, generatorContext: GeneratorContext, checkBuildCommands: bool) -> Optional[GitCloneTask]:
        try:
            return GitCloneTask(generatorContext)
        except Exception as ex:
            if checkBuildCommands:
                raise
            if self.__Log.Verbosity > 1:
                self.__Log.LogPrintWarning("GitClone is unavailable: {0}".format(str(ex)))
            return None


    def __TryAllocateGitApplyTask(self, generatorContext: GeneratorContext, checkBuildCommands: bool) -> Optional[GitApplyTask]:
        try:
            return GitApplyTask(generatorContext)
        except Exception as ex:
            if checkBuildCommands:
                raise
            if self.__Log.Verbosity > 1:
                self.__Log.LogPrintWarning("GitApply is unavailable: {0}".format(str(ex)))
            return None


class PipelineInfo(object):
    def __init__(self, tasks: PipelineTasks, sourcePackage: Package,
                 pathBuilder: RecipePathBuilder,
                 srcRootPath: str, dstRootPath: str, allowDownloads: bool = True) -> None:
        super(PipelineInfo, self).__init__()
        if sourcePackage.ResolvedDirectExperimentalRecipe is None:
            raise Exception("Invalid recipe")
        self.SourcePackage = sourcePackage
        self.SourceRecipe = sourcePackage.ResolvedDirectExperimentalRecipe
        self.SourcePipeline = sourcePackage.ResolvedDirectExperimentalRecipe.Pipeline
        self.PathBuilder = pathBuilder
        self.RecipeAbsolutePath = sourcePackage.AbsolutePath
        self.SrcRootPath = srcRootPath
        self.DstRootPath = dstRootPath
        self.Tasks = tasks
        self.AllowDownloads = allowDownloads


class PipelineBasicCommand(object):
    def __init__(self, basicConfig: BasicConfig,
                 sourceCommand: Optional[XmlRecipePipelineBasicCommand],
                 pipelineInfo: PipelineInfo, finalDstPath: Optional[str] = None) -> None:
        super(PipelineBasicCommand, self).__init__()
        self.BasicConfig = basicConfig
        self.SourceCommand = sourceCommand
        self.Info = pipelineInfo
        self.FinalDstPath = finalDstPath if not finalDstPath is None else self.Info.DstRootPath


    def LogPrint(self, message: str) -> None:
        self.BasicConfig.LogPrint(message)


    def _CreateDirectory(self, path: str) -> None:
        if not IOUtil.IsDirectory(path):
            self.LogPrint("- Creating '{0}' as it was missing".format(path))
            IOUtil.SafeMakeDirs(path)


    def TryResolveSrcPathString(self, path: str) -> Optional[str]:
        if path.startswith('{RECIPE_PATH}/'):
            srcPath = path[len('{RECIPE_PATH}/'):]
            if self.Info.RecipeAbsolutePath is None:
                raise Exception("{RECIPE_PATH} could not be resolved as RecipeAbsolutePath was None")
            return IOUtil.Join(self.Info.RecipeAbsolutePath, srcPath)
        elif path.startswith('{DST_PATH}/'):
            srcPath = path[len('{DST_PATH}/'):]
            return IOUtil.Join(self.Info.DstRootPath, srcPath)
        elif path.startswith('{SRC_PATH}/'):
            srcPath = path[len('{SRC_PATH}/'):]
            return IOUtil.Join(self.Info.SrcRootPath, srcPath)
        elif path.startswith('{OUTPUT_PATH}/'):
            srcPath = path[len('{OUTPUT_PATH}/'):]
            return IOUtil.Join(self.FinalDstPath, srcPath)
        return None


    def TryResolveDstPathString(self, path: str) -> Optional[str]:
        if path.startswith('{DST_PATH}/'):
            srcPath = path[len('{DST_PATH}/'):]
            return IOUtil.Join(self.Info.DstRootPath, srcPath)
        elif path.startswith('{OUTPUT_PATH}/'):
            srcPath = path[len('{OUTPUT_PATH}/'):]
            return IOUtil.Join(self.FinalDstPath, srcPath)
        return None


class PipelineJoinCommand(PipelineBasicCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineJoinCommand, pipelineInfo: PipelineInfo, finalDstPath: str) -> None:
        super(PipelineJoinCommand, self).__init__(basicConfig, sourceCommand, pipelineInfo, finalDstPath)
        if self.Info.SrcRootPath is None:
            raise Exception("Source root path can not be null")
        if self.Info.DstRootPath is None:
            raise Exception("Destination root path can not be null")

    def Execute(self) -> None:
        self.DoExecute()

    def DoExecute(self) -> None:
        """ The base implementation does nothing """
        pass


class PipelineJoinCommandCopy(PipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig,
                 sourceCommand: XmlRecipePipelineJoinCommandCopy,
                 pipelineInfo: PipelineInfo, finalDstPath: str) -> None:
        super(PipelineJoinCommandCopy, self).__init__(basicConfig, sourceCommand, pipelineInfo, finalDstPath)
        self.__SourceCommand = sourceCommand


    def DoExecute(self) -> None:
        """ Copy a file or directory to the destination """
        # Try to do a lookup
        srcPath = self.TryResolveSrcPathString(self.__SourceCommand.From)
        if srcPath is None:
            srcPath = IOUtil.Join(self.Info.SrcRootPath, self.__SourceCommand.From)

        dstPath = self.TryResolveDstPathString(self.__SourceCommand.To)
        if dstPath is None:
            dstPath = IOUtil.Join(self.Info.DstRootPath, self.__SourceCommand.To) if len(self.__SourceCommand.To) > 0 else self.Info.DstRootPath

        if not IOUtil.Exists(dstPath):
            self.LogPrint("Copying from '{0}' to '{1}'".format(srcPath, dstPath))
            if IOUtil.IsFile(srcPath):
                self._CreateDirectory(IOUtil.GetDirectoryName(dstPath))
                shutil.copy2(srcPath, dstPath)
            elif IOUtil.IsDirectory(srcPath):
                self._CreateDirectory(IOUtil.GetDirectoryName(dstPath))
                shutil.copytree(srcPath, dstPath)
            else:
                raise Exception("Copy source '{0}' not found".format(srcPath))
        else:
            self.LogPrint("Copying from '{0}' to '{1}' skipped as target exist".format(srcPath, dstPath))


class PipelineJoinCommandGitApply(PipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig,
                 sourceCommand: XmlRecipePipelineJoinCommandGitApply,
                 pipelineInfo: PipelineInfo,
                 finalDstPath: str,
                 task: Optional[GitApplyTask]) -> None:
        super(PipelineJoinCommandGitApply, self).__init__(basicConfig, sourceCommand, pipelineInfo, finalDstPath)
        if task is None:
            raise Exception("The '{0}' operation has not been enabled for this platform".format(sourceCommand.CommandName))
        self.Task = task
        self.__SourceCommand = sourceCommand


    def DoExecute(self) -> None:
        """ Copy a file or directory to the destination """
        # Try to do a lookup
        srcPath = self.TryResolveSrcPathString(self.__SourceCommand.From)
        if srcPath is None:
            srcPath = IOUtil.Join(self.Info.SrcRootPath, self.__SourceCommand.From)

        self.Task.RunGitApply(srcPath, self.Info.DstRootPath)


class PipelineCommand(PipelineBasicCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: Optional[XmlRecipePipelineCommand], pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommand, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        self.AutoCreateDstDirectory = True
        if sourceCommand is None or sourceCommand.OutputPath is None or len(sourceCommand.OutputPath) <= 0:
            self.FinalDstPath = self.Info.DstRootPath
        else:
            self.FinalDstPath = IOUtil.Join(self.Info.DstRootPath, sourceCommand.OutputPath)
        self.JoinCommandList = [] if sourceCommand is None else self.__CreateJoinCommandList(sourceCommand.JoinCommandList)


    def Execute(self) -> None:
        if self.AutoCreateDstDirectory:
            self._CreateDirectory(self.Info.DstRootPath)
        self.DoExecute()
        self.__ExcuteJoin()


    def DoExecute(self) -> None:
        """ The base implementation does nothing """
        pass


    def __ExcuteJoin(self) -> None:
        for joinCommand in self.JoinCommandList:
            joinCommand.Execute()


    def __CreateJoinCommandList(self, sourceJoinCommandList: List[XmlRecipePipelineJoinCommand]) -> List[PipelineJoinCommand]:
        commandList = []  # type: List[PipelineJoinCommand]
        for sourceJoinCommand in sourceJoinCommandList:
            command = self.__JoinCommand(sourceJoinCommand)
            commandList.append(command)
        return commandList


    def __JoinCommand(self, sourceCommand: XmlRecipePipelineJoinCommand) -> PipelineJoinCommand:
        if sourceCommand.CommandType == BuildRecipePipelineCommand.JoinCopy:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandCopy):
                raise Exception("Invalid command type")
            return PipelineJoinCommandCopy(self.BasicConfig, sourceCommand, self.Info, self.FinalDstPath)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.JoinUnpack:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandUnpack):
                raise Exception("Invalid command type")
            # TODO: implement
            raise Exception("JoinUnpack not implemented")
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.JoinGitApply:
            if not isinstance(sourceCommand, XmlRecipePipelineJoinCommandGitApply):
                raise Exception("Invalid command type")
            return PipelineJoinCommandGitApply(self.BasicConfig, sourceCommand, self.Info, self.FinalDstPath, self.Info.Tasks.TaskGitApply)
        raise Exception("Unsupported join command '{0}'({1}) in '{2}'".format(sourceCommand.CommandName, sourceCommand.CommandType, self.Info.SourceRecipe.Name))


class PipelineCommandFetch(PipelineCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineFetchCommand, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandFetch, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        self.Hash = "NotDefined" if sourceCommand is None or sourceCommand.Hash is None else sourceCommand.Hash


    def _GenerateFileHash(self, filename: str) -> str:
        m = hashlib.sha256()
        with open(filename, "rb") as f:
            for chunk in iter(lambda: f.read(1024*64), b""):
                m.update(chunk)
        return m.hexdigest()


class PipelineCommandNOP(PipelineCommandFetch):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineFetchCommand, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandNOP, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        self.AutoCreateDstDirectory = False


class PipelineCommandDownload(PipelineCommandFetch):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineFetchCommandDownload, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandDownload, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        if pipelineInfo.Tasks.TaskDownload is None:
            raise Exception("The '{0}' operation has not been enabled for this platform".format(sourceCommand.CommandName))
        self.Task = pipelineInfo.Tasks.TaskDownload
        self.__SourceCommand = sourceCommand


    def DoExecute(self) -> None:
        try:
            targetFilename = PipelineCommandDownload.GetTargetFilename(self.__SourceCommand)
            archiveFilePath = IOUtil.Join(self.Info.DstRootPath, targetFilename)

            if not self.Info.AllowDownloads and not IOUtil.IsFile(archiveFilePath):
                raise Exception("Could not download {0} to {1} as downloads have been disabled. Enable downloads or download it manually.".format(self.__SourceCommand.URL, archiveFilePath))

            self.Task.DownloadFromUrl(self.__SourceCommand.URL, archiveFilePath)
            # Generate file hash
            filehash = self._GenerateFileHash(archiveFilePath)
            if self.__SourceCommand.Hash is None:
                if self.BasicConfig.Verbosity >= 1:
                    self.BasicConfig.LogPrintWarning("No hash value defined for file {0} which has a hash value of {1}".format(archiveFilePath, filehash))
            elif filehash != self.__SourceCommand.Hash:
                raise Exception("The downloaded file {0} has a hash of {1} which did not match the expected value of {2}".format(archiveFilePath, filehash, self.__SourceCommand.Hash))
            elif self.BasicConfig.Verbosity >= 2:
                self.LogPrint("The downloaded file {0} hash was {1} as expected.".format(archiveFilePath, filehash))
        except Exception:
            # A error occurred removing the targetPath
            if IOUtil.IsFile(archiveFilePath):
                self.LogPrint("* A error occurred removing '{0}' to be safe.".format(archiveFilePath))
                IOUtil.RemoveFile(archiveFilePath)
            raise

    @staticmethod
    def __GetFileNameFromUrl(url: str) -> str:
        index = url.rindex('/')
        if index < 0:
            raise Exception("Could not extract the filename from URL '{0}'".format(url))
        return url[index+1:]


    @staticmethod
    def GetTargetFilename(sourceCommand: XmlRecipePipelineFetchCommandDownload) -> str:
        return PipelineCommandDownload.__GetFileNameFromUrl(sourceCommand.URL) if sourceCommand.To is None else sourceCommand.To


class PipelineCommandGitClone(PipelineCommandFetch):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineFetchCommandGitClone, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandGitClone, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        if pipelineInfo.Tasks.TaskGitClone is None:
            raise Exception("The '{0}' operation has not been enabled for this platform".format(sourceCommand.CommandName))
        self.Task = pipelineInfo.Tasks.TaskGitClone
        self.__SourceCommand = sourceCommand
        self.AutoCreateDstDirectory = False


    def DoExecute(self) -> None:
        try:
            dstPath = self.Info.DstRootPath
            if not self.Info.AllowDownloads and not IOUtil.IsDirectory(dstPath):
                raise Exception("Could not git clone {0} to {1} as downloads have been disabled. Enable downloads or clone it manually.".format(self.__SourceCommand.URL, dstPath))
            self.Task.RunGitClone(self.__SourceCommand.URL, self.__SourceCommand.Tag, dstPath)
            # get the repo hash
            hashStr = self.Task.GetCurrentHash(dstPath)
            if self.__SourceCommand.Hash is None:
                if self.BasicConfig.Verbosity >= 1:
                    self.BasicConfig.LogPrintWarning("No hash value defined for repo {0} which has a hash value of {1}".format(dstPath, hashStr))
            elif hashStr != self.__SourceCommand.Hash:
                raise Exception("The repo {0} has a hash of {1} which did not match the expected value of {2}".format(dstPath, hashStr, self.__SourceCommand.Hash))
            elif self.BasicConfig.Verbosity >= 2:
                self.LogPrint("The cloned repo {0} hash was {1} as expected.".format(dstPath, hashStr))
        except Exception:
            self.__DoSafeRemoveDirectoryTree(dstPath, 0)
            raise


    def __DoSafeRemoveDirectoryTree(self, dstPath: str, retryCount: int) -> None:
        # A error occurred removing the targetPath
        if not IOUtil.IsDirectory(dstPath) or retryCount >= 2:
            return

        try:
            self.LogPrint("* A error occurred removing '{0}' to be safe.".format(dstPath))
            IOUtil.SafeRemoveDirectoryTree(dstPath, False)
        except Exception:
            self.LogPrint("* Failed to remove '{0}', trying again in 1sec.".format(dstPath))
            time.sleep(1)
            self.__DoSafeRemoveDirectoryTree(dstPath, retryCount + 1)
            raise



class PipelineCommandUnpack(PipelineCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineCommandUnpack, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandUnpack, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        self.AutoCreateDstDirectory = False
        if pipelineInfo.Tasks.TaskUnpackAndRename is None:
            raise Exception("The '{0}' operation has not been enabled for this platform".format(sourceCommand.CommandName))
        self.Task = pipelineInfo.Tasks.TaskUnpackAndRename
        self.__SourceCommand = sourceCommand

    def DoExecute(self) -> None:
        packedFilePath = IOUtil.Join(self.Info.SrcRootPath, self.__SourceCommand.File)
        self.Task.RunUnpack(packedFilePath, self.Info.DstRootPath)


class PipelineCommandCMakeBuild(PipelineCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineCommandCMakeBuild, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandCMakeBuild, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        if pipelineInfo.Tasks.TaskCMakeAndBuild is None:
            raise Exception("The '{0}' operation has not been enabled for this platform".format(sourceCommand.CommandName))
        self.Task = pipelineInfo.Tasks.TaskCMakeAndBuild
        self.__SourceCommand = sourceCommand


    def DoExecute(self) -> None:
        installedDependencies = self.__BuildDependencyPathString(self.Info.SourcePackage.ResolvedExperimentalRecipeBuildOrder)
        optionList = shlex.split(self.__SourceCommand.Options)
        if installedDependencies:
            optionList.append(installedDependencies)
        target = self.__SourceCommand.Target
        installPath = self.FinalDstPath
        self.Task.RunCMakeAndBuild(self.Info.SrcRootPath, installPath, self.Info.DstRootPath, target, self.__SourceCommand.Project, self.__SourceCommand.ConfigurationList, optionList)


    def __BuildDependencyPathString(self, resolvedBuildOrder: List[Package]) -> str:
        packageInstallDirs = self.__BuildDependencyPathList(resolvedBuildOrder)
        if not packageInstallDirs:
            return ""
        strDirs = ";".join(packageInstallDirs)
        return "-DCMAKE_PREFIX_PATH={0}".format(strDirs)


    def __BuildDependencyPathList(self, resolvedBuildOrder: List[Package]) -> List[str]:
        dependencyDirList = []
        for package in resolvedBuildOrder:
            if package != self.Info.SourcePackage:
                if package.ResolvedDirectExperimentalRecipe is not None and package.ResolvedDirectExperimentalRecipe.ResolvedInstallPath is not None:
                    dependencyDirList.append(package.ResolvedDirectExperimentalRecipe.ResolvedInstallPath)
        return dependencyDirList


class PipelineCommandClone(PipelineCommand):
    def __init__(self, basicConfig: BasicConfig, sourceCommand: XmlRecipePipelineCommandClone, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandClone, self).__init__(basicConfig, sourceCommand, pipelineInfo)
        self.AutoCreateDstDirectory = False

    def IsCompleted(self) -> bool:
        return IOUtil.Exists(self.Info.DstRootPath)


    def DoExecute(self) -> None:
        if self.IsCompleted():
            self.LogPrint("Clone from '{0}' to '{1}' skipped as target exist".format(self.Info.SrcRootPath, self.Info.DstRootPath))
            return
        self.LogPrint("Cloning from '{0}' to '{1}'".format(self.Info.SrcRootPath, self.Info.DstRootPath))

        # TODO: we could allow the clone command to have a 'patterns' string that can be forwarded to shutil.ignore_patterns
        shutil.copytree(self.Info.SrcRootPath, self.Info.DstRootPath, ignore=shutil.ignore_patterns('.git'))



class PipelineCommandInstall(PipelineCommand):
    def __init__(self, basicConfig: BasicConfig, pipelineInfo: PipelineInfo) -> None:
        super(PipelineCommandInstall, self).__init__(basicConfig, None, pipelineInfo)
        self.AutoCreateDstDirectory = False

    def IsCompleted(self) -> bool:
        return IOUtil.Exists(self.Info.DstRootPath)

    def DoExecute(self) -> None:
        if self.IsCompleted():
            self.LogPrint("Installing from '{0}' to '{1}' skipped as target exist".format(self.Info.SrcRootPath, self.Info.DstRootPath))
            return
        self.LogPrint("Installing from '{0}' to '{1}'".format(self.Info.SrcRootPath, self.Info.DstRootPath))

        shutil.move(self.Info.SrcRootPath, self.Info.DstRootPath)


class PipelineCommandBuilder(object):
    def __init__(self, generatorContext: GeneratorContext, checkBuildCommands: bool, buildThreads: int) -> None:
        super(PipelineCommandBuilder, self).__init__()
        self.__Log = generatorContext.Log
        self.__BasicConfig = generatorContext.BasicConfig
        self.__AllowDownloads = generatorContext.AllowDownloads

        # Add the task objects
        self.PipelineTasks = PipelineTasks(self.__Log, generatorContext, checkBuildCommands, buildThreads)

        # TODO: enable this once we fixed the path issue (beware code moved to path builder now)
        #if not self.TaskCMakeAndBuild is None:
        #    shortId = self.TaskCMakeAndBuild.CompilerShortId
        #    installRootPath = IOUtil.Join(installRootPath, shortId)

        self.__PathBuilder = generatorContext.RecipePathBuilder
        self.__CommandList = None  # type: Optional[List[PipelineCommand]]
        self.__SourcePackage = None  # type: Optional[Package]
        self.__SourceRecipe = None  # type: Optional[PackageExperimentalRecipe]
        self.__CommandInputRootPath = None  # type: Optional[str]
        self.__PipelineInstallPath = None  # type: Optional[str]


    def GetBuildPath(self, sourceRecipe: PackageExperimentalRecipe) -> str:
        return self.__PathBuilder.GetBuildPath(sourceRecipe)


    def Begin(self, sourcePackage: Package, sourceRecipe: PackageExperimentalRecipe) -> None:
        """ Starts a new list, even if the previous begin operation wasn't ended """
        self.__CommandList = []
        self.__SourcePackage = sourcePackage
        self.__SourceRecipe = sourceRecipe
        self.__CommandInputRootPath = sourcePackage.AbsolutePath
        self.__PipelineInstallPath = sourceRecipe.ResolvedInstallPath


    def Add(self, sourceCommand: XmlRecipePipelineCommand) -> None:
        if self.__CommandList is None or self.__CommandInputRootPath is None:
            raise Exception("Usage error, Add called outside begin/end block")

        command = self.__CreateCommand(sourceCommand, self.__CommandInputRootPath)
        self.__CommandList.append(command)
        # The next command in the pipeline uses the 'output' from the previous command as input
        self.__CommandInputRootPath = command.FinalDstPath


    def End(self) -> List[PipelineCommand]:
        if self.__SourcePackage is None or self.__PathBuilder is None or self.__CommandInputRootPath is None or self.__PipelineInstallPath is None or self.__CommandList is None or self.__SourceRecipe is None:
            raise Exception("Usage error, End called outside begin/end block")

        # Add a final install command to finish the pipe
        installInfo = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, self.__CommandInputRootPath, self.__PipelineInstallPath)
        pipelineInstallCommand = PipelineCommandInstall(self.__BasicConfig, installInfo)
        self.__CommandList.append(pipelineInstallCommand)

        result = self.__CommandList

        if pipelineInstallCommand.IsCompleted():
            self.__Log.LogPrint("  Pipeline '{0}' skipped as target path '{1}' exists.".format(self.__SourceRecipe.Name, installInfo.DstRootPath))
            result = []

        self.__CommandList = None
        self.__SourcePackage = None
        self.__SourceRecipe = None
        self.__CommandInputRootPath = None
        self.__PipelineInstallPath = None
        return result


    def __GetTempDirectoryName(self, sourceCommand: XmlRecipePipelineCommand) -> str:
        if self.__SourcePackage is None or self.__CommandList is None:
            raise Exception("__GetTempDirectoryName called outside begin/end block")
        if self.__SourcePackage.ResolvedDirectExperimentalRecipe is None:
            raise Exception("Invalid package")

        rootPath = self.GetBuildPath(self.__SourcePackage.ResolvedDirectExperimentalRecipe)
        stepString = "{0:0>4}_{1}".format(len(self.__CommandList), sourceCommand.CommandName)
        return IOUtil.Join(rootPath, stepString)


    def __CreateCommand(self, sourceCommand: XmlRecipePipelineCommand, srcRootPath: str) -> PipelineCommand:
        if self.__SourcePackage is None or self.__SourceRecipe is None:
            raise Exception("Invalid state")

        if sourceCommand.CommandType == BuildRecipePipelineCommand.Download:
            if not isinstance(sourceCommand, XmlRecipePipelineFetchCommandDownload):
                raise Exception("Internal error, sourceCommand was not XmlRecipePipelineFetchCommandDownload")
            return self.__CreateCommandDownload(sourceCommand, srcRootPath)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.GitClone:
            if not isinstance(sourceCommand, XmlRecipePipelineFetchCommandGitClone):
                raise Exception("Internal error, sourceCommand was not XmlRecipePipelineFetchCommandGitClone")
            return self.__CreateCommandGitClone(sourceCommand, srcRootPath)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.Source:
            if not isinstance(sourceCommand, XmlRecipePipelineFetchCommandSource):
                raise Exception("Internal error, sourceCommand was not XmlRecipePipelineFetchCommandSource")
            return self.__CreateCommandSource(sourceCommand, srcRootPath)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.Unpack:
            if not isinstance(sourceCommand, XmlRecipePipelineCommandUnpack):
                raise Exception("Internal error, sourceCommand was not XmlRecipePipelineCommandUnpack")
            info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, srcRootPath, self.__GetTempDirectoryName(sourceCommand))
            return PipelineCommandUnpack(self.__BasicConfig, sourceCommand, info)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.CMakeBuild:
            if not isinstance(sourceCommand, XmlRecipePipelineCommandCMakeBuild):
                raise Exception("Internal error, sourceCommand was not XmlRecipePipelineCommandCMakeBuild")
            info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, srcRootPath, self.__GetTempDirectoryName(sourceCommand))
            return PipelineCommandCMakeBuild(self.__BasicConfig, sourceCommand, info)
        elif sourceCommand.CommandType == BuildRecipePipelineCommand.Clone:
            if not isinstance(sourceCommand, XmlRecipePipelineCommandClone):
                raise Exception("Internal error, sourceCommand was not XmlRecipePipelineCommandClone")
            info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, srcRootPath, self.__GetTempDirectoryName(sourceCommand))
            return PipelineCommandClone(self.__BasicConfig, sourceCommand, info)
        raise Exception("Unsupported command '{0}' in '{1}'".format(sourceCommand.CommandType, self.__SourceRecipe.Name))


    def __CreateCommandDownload(self, sourceCommand: XmlRecipePipelineFetchCommandDownload, srcRootPath: str) -> PipelineCommand:
        if self.__SourcePackage is None or self.__SourceRecipe is None:
            raise Exception("Invalid state")

        readonlyCacheRootDir = self.__PathBuilder.ReadonlyCache_DownloadCacheRootPath
        if not readonlyCacheRootDir is None:
            # If we have a download cache and the directory exists there then setup a void fetch command
            targetFilename = PipelineCommandDownload.GetTargetFilename(sourceCommand)
            cachePath = IOUtil.Join(readonlyCacheRootDir, targetFilename)
            if IOUtil.IsFile(cachePath):
                info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, readonlyCacheRootDir, readonlyCacheRootDir)
                return PipelineCommandNOP(self.__BasicConfig, sourceCommand, info)

        if self.__PathBuilder.DownloadCacheRootPath is None:
            raise Exception("Invalid State")

        info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, srcRootPath, self.__PathBuilder.DownloadCacheRootPath, allowDownloads=self.__AllowDownloads)
        return PipelineCommandDownload(self.__BasicConfig, sourceCommand, info)


    def __CreateCommandGitClone(self, sourceCommand: XmlRecipePipelineFetchCommandGitClone, srcRootPath: str) -> PipelineCommand:
        if self.__SourcePackage is None or self.__SourceRecipe is None:
            raise Exception("Invalid state")

        readonlyCacheRootDir = self.__PathBuilder.ReadonlyCache_DownloadCacheRootPath
        if not readonlyCacheRootDir is None:
            cachePath = IOUtil.Join(readonlyCacheRootDir, self.__SourceRecipe.Name)
            if IOUtil.IsDirectory(cachePath):
                info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, cachePath, cachePath)
                return PipelineCommandNOP(self.__BasicConfig, sourceCommand, info)

        if self.__PathBuilder.DownloadCacheRootPath is None:
            raise Exception("Invalid State")

        dstPath = IOUtil.Join(self.__PathBuilder.DownloadCacheRootPath, self.__SourceRecipe.Name)
        info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, srcRootPath, dstPath, allowDownloads=self.__AllowDownloads)
        return PipelineCommandGitClone(self.__BasicConfig, sourceCommand, info)


    def __CreateCommandSource(self, sourceCommand: XmlRecipePipelineFetchCommandSource, srcRootPath: str) -> PipelineCommand:
        if self.__SourcePackage is None or self.__SourceRecipe is None:
            raise Exception("Invalid state")

        # We basically setup a NOP command that points to the source package location which will allow the pipeline to work with that
        info = PipelineInfo(self.PipelineTasks, self.__SourcePackage, self.__PathBuilder, srcRootPath, srcRootPath, allowDownloads=self.__AllowDownloads)
        return PipelineCommandNOP(self.__BasicConfig, sourceCommand, info)

