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

from typing import cast
from typing import List
from typing import Optional
import xml.etree.ElementTree as ET
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.DataTypes import BuildRecipePipelineCommand
from FslBuildGen.DataTypes import BuildRecipeValidateCommand
from FslBuildGen.DataTypes import BuildRecipeValidateMethod
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import CMakeTargetType
#from FslBuildGen.Xml.Exceptions import XmlUnsupportedPlatformException
from FslBuildGen.Xml.XmlBase import XmlBase
#from FslBuildGen.Xml.XmlBase2 import XmlBase2
#from FslBuildGen import PackageConfig

g_validJoinCommands = ["Copy", "Unpack", "GitApply", "Delete"]
g_validFetchCommands = ["GitClone", "Download", "Source"]
g_validCommands = ["Unpack", "CMakeBuild", "Combine", "Copy"]
g_validValidateCommands = ["EnvironmentVariable", "Path", "FindFileInPath", "FindExecutableFileInPath", "AddHeaders", "AddLib", "AddDLL", "AddTool"]
g_validValidCombineCommands = ["CMakeBuild"]

g_CMAKE_PACKAGE_NAME = "Recipe.BuildTool.CMake"
g_GIT_PACKAGE_NAME = "Recipe.BuildTool.Git"

class XmlRecipeFileDependency(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name')


class XmlRecipeValidateCommand(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element, commandName: str, commandType: int) -> None:
        super().__init__(basicConfig, xmlElement)
        self.CommandName = commandName
        self.CommandType = commandType
        self.Help = self._TryReadAttrib(xmlElement, 'Help')


class XmlRecipeValidateCommandEnvironmentVariable(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "EnvironmentVariable", BuildRecipeValidateCommand.EnvironmentVariable)
        self.Name = self._ReadAttrib(xmlElement, 'Name')  # type: str
        method = self._ReadAttrib(xmlElement, 'Method')
        self.AllowEndSlash = self._ReadBoolAttrib(xmlElement, 'AllowEndSlash', False)  # type: bool

        self.Method = BuildRecipeValidateMethod.FromString(method)  # type: int


class XmlRecipeValidateCommandPath(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Path", BuildRecipeValidateCommand.Path)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        method = self._ReadAttrib(xmlElement, 'Method')

        self.Method = BuildRecipeValidateMethod.FromString(method)

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))



class XmlRecipeValidateCommandFindFileInPath(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "FindFileInPath", BuildRecipeValidateCommand.FindFileInPath)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.ExpectedPath = self._TryReadAttrib(xmlElement, 'ExpectedPath')

        if '\\' in self.Name or '/' in self.Name:
            raise Exception("A filename can not contain backslash '\\' or slash '/': '{0}'".format(self.Name))

        if not self.ExpectedPath is None:
            if '\\' in self.ExpectedPath:
                raise Exception("A path can not contain backslash '\\': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.startswith('/'):
                raise Exception("A path can not start with a slash '/': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.endswith('/'):
                raise Exception("A path can not end with a slash '/': '{0}'".format(self.ExpectedPath))


class XmlRecipeValidateCommandFindExecutableFileInPath(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "FindFileInPath", BuildRecipeValidateCommand.FindExecutableFileInPath)
        self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.ExpectedPath = self._TryReadAttrib(xmlElement, 'ExpectedPath')

        if '\\' in self.Name or '/' in self.Name:
            raise Exception("A filename can not contain backslash '\\' or slash '/': '{0}'".format(self.Name))

        if not self.ExpectedPath is None:
            if '\\' in self.ExpectedPath:
                raise Exception("A path can not contain backslash '\\': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.startswith('/'):
                raise Exception("A path can not start with a slash '/': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.endswith('/'):
                raise Exception("A path can not end with a slash '/': '{0}'".format(self.ExpectedPath))


class XmlRecipeValidateCommandAddHeaders(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "AddHeaders", BuildRecipeValidateCommand.AddHeaders)
        self.Name = self._ReadAttrib(xmlElement, 'Name')

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))


class XmlRecipeValidateCommandAddLib(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "AddLib", BuildRecipeValidateCommand.AddLib)
        self.Name = self._ReadAttrib(xmlElement, 'Name')  # type:str
        self.DebugName = self._ReadAttrib(xmlElement, 'DebugName', self.Name)  # type:str

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        if '\\' in self.DebugName:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.DebugName.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))


class XmlRecipeValidateCommandAddDLL(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "AddDLL", BuildRecipeValidateCommand.AddDLL)
        self.Name = self._ReadAttrib(xmlElement, 'Name')  # type:str
        self.DebugName = self._ReadAttrib(xmlElement, 'DebugName', self.Name)  # type:str

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        if '\\' in self.DebugName:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.DebugName.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))


class XmlRecipeValidateCommandAddTool(XmlRecipeValidateCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "AddTool", BuildRecipeValidateCommand.AddTool)
        self.Name = self._ReadAttrib(xmlElement, 'Name')  # type:str
        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))

class XmlRecipeInstallation(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement)
        self.CommandList = self.__GetCommandList(basicConfig, xmlElement)

    def __GetCommandList(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> List[XmlRecipeValidateCommand]:
        elements = [] # type: List[XmlRecipeValidateCommand]
        if xmlElement != None:
            for child in xmlElement:
                if child.tag == 'EnvironmentVariable':
                    elements.append(XmlRecipeValidateCommandEnvironmentVariable(basicConfig, child))
                elif child.tag == 'Path':
                    elements.append(XmlRecipeValidateCommandPath(basicConfig, child))
                elif child.tag == 'FindFileInPath':
                    elements.append(XmlRecipeValidateCommandFindFileInPath(basicConfig, child))
                elif child.tag == 'FindExecutableFileInPath':
                    elements.append(XmlRecipeValidateCommandFindExecutableFileInPath(basicConfig, child))
                elif child.tag == 'AddHeaders':
                    elements.append(XmlRecipeValidateCommandAddHeaders(basicConfig, child))
                elif child.tag == 'AddLib':
                    elements.append(XmlRecipeValidateCommandAddLib(basicConfig, child))
                elif child.tag == 'AddDLL':
                    elements.append(XmlRecipeValidateCommandAddDLL(basicConfig, child))
                elif child.tag == 'AddTool':
                    elements.append(XmlRecipeValidateCommandAddTool(basicConfig, child))
                else:
                    raise Exception("Unknown validation command element '{0}' found. Valid commands: {1}".format(child.tag, g_validValidateCommands))
        return elements


class XmlRecipePipelineBasicCommand(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element, commandName: str, commandType: int, buildToolPackageName: Optional[str] = None) -> None:
        super().__init__(basicConfig, xmlElement)
        self.CommandName = commandName  # type: str
        self.CommandType = commandType  # type: int
        self.BuildToolPackageNames = None if buildToolPackageName is None else [buildToolPackageName] # type: Optional[List[str]]


class XmlRecipePipelineJoinCommand(XmlRecipePipelineBasicCommand):
    pass
    #def __init__(self, basicConfig, xmlElement: ET.Element, commandName, commandType, buildToolPackageName=None) -> None:
    #    super().__init__(basicConfig, xmlElement, commandName, commandType, buildToolPackageName)



class XmlRecipePipelineJoinCommandCopy(XmlRecipePipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Copy", BuildRecipePipelineCommand.JoinCopy)
        self.From = self._ReadAttrib(xmlElement, 'From')
        self.To = self._ReadAttrib(xmlElement, 'To')
        self.Overwrite = self._ReadBoolAttrib(xmlElement, 'Overwrite', False)


class XmlRecipePipelineJoinCommandDelete(XmlRecipePipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Delete", BuildRecipePipelineCommand.JoinDelete)
        self.Path = self._ReadAttrib(xmlElement, 'Path')


class XmlRecipePipelineJoinCommandUnpack(XmlRecipePipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Unpack", BuildRecipePipelineCommand.JoinUnpack)
        self.From = self._ReadAttrib(xmlElement, 'From')
        self.To = self._ReadAttrib(xmlElement, 'To', '')


class XmlRecipePipelineJoinCommandGitApply(XmlRecipePipelineJoinCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "GitApply", BuildRecipePipelineCommand.JoinGitApply, buildToolPackageName=g_GIT_PACKAGE_NAME)
        self.From = self._ReadAttrib(xmlElement, 'From')


class XmlRecipePipelineCommand(XmlRecipePipelineBasicCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element, commandName: str, commandType: int,
                 outputPathAllowed: bool = True, buildToolPackageName: Optional[str] = None, allowJoinCommandList: bool = True) -> None:
        super().__init__(basicConfig, xmlElement, commandName, commandType, buildToolPackageName)
        self.JoinCommandList = self.__GetJoinCommandList(basicConfig, xmlElement) if allowJoinCommandList else []
        self.OutputPath = None  # type: Optional[str]
        if outputPathAllowed:
            self.OutputPath = self._ReadAttrib(xmlElement, 'OutputPath', '')


    def __GetJoinCommandList(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> List[XmlRecipePipelineJoinCommand]:
        elements = []  # type: List[XmlRecipePipelineJoinCommand]
        if xmlElement != None:
            for child in xmlElement:
                if child.tag == 'Copy':
                    elements.append(XmlRecipePipelineJoinCommandCopy(basicConfig, child))
                elif child.tag == 'Unpack':
                    elements.append(XmlRecipePipelineJoinCommandUnpack(basicConfig, child))
                elif child.tag == 'GitApply':
                    elements.append(XmlRecipePipelineJoinCommandGitApply(basicConfig, child))
                elif child.tag == 'Delete':
                    elements.append(XmlRecipePipelineJoinCommandDelete(basicConfig, child))
                else:
                    raise Exception("Unknown join command element '{0}' found in command {1}. Valid join commands: {2}".format(child.tag, self.CommandName, g_validJoinCommands))

        return elements


class XmlRecipePipelineFetchCommand(XmlRecipePipelineCommand):
    def __init__(self, basicConfig: BasicConfig,
                 xmlElement: ET.Element,
                 commandName: str,
                 commandType: int,
                 outputPathAllowed: bool = True,
                 buildToolPackageName: Optional[str] = None,
                 allowJoinCommandList: bool = True) -> None:
        super().__init__(basicConfig, xmlElement, commandName, commandType, outputPathAllowed, buildToolPackageName, allowJoinCommandList)
        self.Hash = None  # type: Optional[str]


class XmlRecipePipelineFetchCommandGitClone(XmlRecipePipelineFetchCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "GitClone", BuildRecipePipelineCommand.GitClone, buildToolPackageName=g_GIT_PACKAGE_NAME, allowJoinCommandList=False)
        self.URL = self._ReadAttrib(xmlElement, 'URL')
        self.Tag = self._ReadAttrib(xmlElement, 'Tag', '')
        self.Hash = self._TryReadAttrib(xmlElement, 'Hash')

        #if len(self.URL) <= 0:
        #    raise Exception


class XmlRecipePipelineFetchCommandDownload(XmlRecipePipelineFetchCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Download", BuildRecipePipelineCommand.Download, allowJoinCommandList=False)
        self.URL = self._ReadAttrib(xmlElement, 'URL')
        self.To = self._TryReadAttrib(xmlElement, 'To')
        self.Hash = self._TryReadAttrib(xmlElement, 'Hash')


class XmlRecipePipelineFetchCommandSource(XmlRecipePipelineFetchCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Source", BuildRecipePipelineCommand.Source, allowJoinCommandList=False)


class XmlRecipePipelineBuildCommand(XmlRecipePipelineCommand):
    pass


class XmlRecipePipelineCommandUnpack(XmlRecipePipelineBuildCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Unpack", BuildRecipePipelineCommand.Unpack)
        self.File = self._ReadAttrib(xmlElement, 'File')


class XmlRecipePipelineCommandCMakeBuild(XmlRecipePipelineBuildCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "CMakeBuild", BuildRecipePipelineCommand.CMakeBuild, False, buildToolPackageName=g_CMAKE_PACKAGE_NAME)
        self.Source = self._TryReadAttrib(xmlElement, 'Source')
        self.Project = self._ReadAttrib(xmlElement, 'Project')
        target = self._ReadAttrib(xmlElement, 'Target')
        configuration = self._ReadAttrib(xmlElement, 'Configuration')
        self.Options = self._ReadAttrib(xmlElement, 'Options', '')
        self.Target = CMakeTargetType.FromString(target)
        self.OutputPath = self._ReadAttrib(xmlElement, 'OutputPath')

        self.ConfigurationList = self.__ParseConfiguration(configuration)


    def __ParseConfiguration(self, configuration: str) -> List[int]:
        configurations = configuration.split(';')
        configurationList = []
        for entry in configurations:
            config = BuildVariantConfig.FromString(entry)
            configurationList.append(config)
        return configurationList


class XmlRecipePipelineCommandCombine(XmlRecipePipelineBuildCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Combine", BuildRecipePipelineCommand.Combine, False, allowJoinCommandList=False)
        self.OutputPath = self._TryReadAttrib(xmlElement, 'OutputPath')
        self.CommandList = self.__GetCombineCommandList(basicConfig, xmlElement)
        if len(self.CommandList) <= 0:
            raise Exception("A Combine command must contain at least one commands")
        self.BuildToolPackageNames = []
        for command in self.CommandList:
            if command.BuildToolPackageNames is not None:
                for buildToolPackageName in command.BuildToolPackageNames:
                    if not buildToolPackageName in self.BuildToolPackageNames:
                        self.BuildToolPackageNames.append(buildToolPackageName)


    def __GetCombineCommandList(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> List[XmlRecipePipelineBuildCommand]:
        elements = []  # type: List[XmlRecipePipelineBuildCommand]
        if xmlElement != None:
            isFirstCommand = True
            for child in xmlElement:
                command = _TryAllocatePipelineCombineCommand(basicConfig, child)
                if command is not None:
                    elements.append(command)
                else:
                    if not child.tag in g_validValidCombineCommands:
                        raise Exception("Unknown element '{0}' found in combine command list. Valid commands: {1}".format(child.tag, g_validValidCombineCommands))
                isFirstCommand = False
        return elements


class XmlRecipePipelineCommandCopy(XmlRecipePipelineBuildCommand):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement, "Copy", BuildRecipePipelineCommand.Copy)


def _TryAllocatePipelineFetchCommand(basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlRecipePipelineFetchCommand]:
    if xmlElement.tag == 'GitClone':
        return XmlRecipePipelineFetchCommandGitClone(basicConfig, xmlElement)
    elif xmlElement.tag == 'Download':
        return XmlRecipePipelineFetchCommandDownload(basicConfig, xmlElement)
    elif xmlElement.tag == 'Source':
        return XmlRecipePipelineFetchCommandSource(basicConfig, xmlElement)
    return None


def _TryAllocatePipelineCommand(basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlRecipePipelineBuildCommand]:
    if xmlElement.tag == 'Unpack':
        return XmlRecipePipelineCommandUnpack(basicConfig, xmlElement)
    elif xmlElement.tag == 'CMakeBuild':
        return XmlRecipePipelineCommandCMakeBuild(basicConfig, xmlElement)
    elif xmlElement.tag == 'Combine':
        return XmlRecipePipelineCommandCombine(basicConfig, xmlElement)
    elif xmlElement.tag == 'Copy':
        return XmlRecipePipelineCommandCopy(basicConfig, xmlElement)
    return None


def _TryAllocatePipelineCombineCommand(basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlRecipePipelineBuildCommand]:
    if xmlElement.tag == 'CMakeBuild':
        return XmlRecipePipelineCommandCMakeBuild(basicConfig, xmlElement)
    return None


class XmlRecipePipeline(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> None:
        super().__init__(basicConfig, xmlElement)
        #self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.CommandList = self.__GetCommandList(basicConfig, xmlElement)


    def __GetCommandList(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> List[XmlRecipePipelineCommand]:
        elements = []  # type: List[XmlRecipePipelineCommand]
        if xmlElement != None:
            isFirstCommand = True
            for child in xmlElement:
                command = _TryAllocatePipelineCommand(basicConfig, child) if not isFirstCommand else _TryAllocatePipelineFetchCommand(basicConfig, child)
                if command is not None:
                    elements.append(command)
                else:
                    if isFirstCommand:
                        if not child.tag in g_validFetchCommands:
                            raise Exception("A pipeline is expected to start with a fetch element not '{0}'. Valid fetch commands {1}".format(child.tag, g_validFetchCommands))
                        else:
                            raise Exception("Unknown element '{0}' found in pipeline command list, this should never happen".format(child.tag))
                    else:
                        if child.tag in g_validFetchCommands:
                            raise Exception("A pipeline can only start with a fetch element '{0}'".format(child.tag))
                        else:
                            raise Exception("Unknown element '{0}' found in pipeline command list. Valid commands: {1}".format(child.tag, g_validCommands))
                isFirstCommand = False
        return elements


class XmlExperimentalRecipe(XmlBase):
    def __init__(self, basicConfig: BasicConfig, xmlElement: ET.Element, defaultName: str) -> None:
        super().__init__(basicConfig, xmlElement)
        self.Name = self._ReadAttrib(xmlElement, 'Name', defaultName)
        self.Pipeline = self.__TryGetPipeline(xmlElement)
        self.ValidateInstallation = self.__TryGetValidateInstallation(basicConfig, xmlElement)
        self.ExternalInstallDirectory = self._TryReadAttrib(xmlElement, 'ExternalInstallDirectory')

        #if self.Pipeline is None and self.ExternalInstallDirectory is None:
        #    raise Exception("Recipe '{0}' has to have either a pipeline or a ExternalInstallDirectory defined.".format(self.Name))
        if not self.Pipeline is None and not self.ExternalInstallDirectory is None:
            raise Exception("Recipe '{0}' can only a pipeline or a ExternalInstallDirectory defined not both.".format(self.Name))


    def __TryGetPipeline(self, xmlElement: ET.Element) -> Optional[XmlRecipePipeline]:
        child = self._TryGetElement(xmlElement, 'Pipeline')
        if child is None:
            return None
        return XmlRecipePipeline(self.BasicConfig, child)


    def __TryGetValidateInstallation(self, basicConfig: BasicConfig, xmlElement: ET.Element) -> Optional[XmlRecipeInstallation]:
        child = self._TryGetElement(xmlElement, 'Installation')
        if child is None:
            if basicConfig.Verbosity >= 2:
                basicConfig.LogPrint("The Installation element is missing for recipe {0}".format(self.Name))
            return None
        return XmlRecipeInstallation(basicConfig, child)
