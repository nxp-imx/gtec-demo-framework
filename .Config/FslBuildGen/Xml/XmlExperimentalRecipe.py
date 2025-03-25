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

#from typing import cast
from typing import List
from typing import Optional
import xml.etree.ElementTree as ET
from FslBuildGen import IOUtil
from FslBuildGen.BannedCommands import BannedCommands
from FslBuildGen.DataTypes import BuildRecipePipelineCommand
from FslBuildGen.DataTypes import BuildRecipeValidateCommand
from FslBuildGen.DataTypes import BuildRecipeValidateMethod
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import CMakeTargetType
from FslBuildGen.Version import Version
from FslBuildGen.Log import Log
from FslBuildGen import Util
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
    __AttribName = 'Name'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)


class XmlRecipeValidateCommand(XmlBase):
    _AttribHelp = 'Help'

    def __init__(self, log: Log, xmlElement: ET.Element, commandName: str, commandType: BuildRecipeValidateCommand) -> None:
        super().__init__(log, xmlElement)
        self.CommandName = commandName
        self.CommandType = commandType
        self.Help = self._TryReadAttrib(xmlElement, self._AttribHelp)


class XmlRecipeValidateCommandEnvironmentVariable(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribMethod = 'Method'
    __AttribAllowEndSlash = 'AllowEndSlash'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "EnvironmentVariable", BuildRecipeValidateCommand.EnvironmentVariable)
        self._CheckAttributes({self.__AttribName, self.__AttribMethod, self.__AttribAllowEndSlash, self._AttribHelp})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)  # type: str
        method = self._ReadAttrib(xmlElement, self.__AttribMethod)
        self.AllowEndSlash = self._ReadBoolAttrib(xmlElement, self.__AttribAllowEndSlash, False)  # type: bool

        self.Method = BuildRecipeValidateMethod.FromString(method)  # type: int


class XmlRecipeValidateCommandPath(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribMethod = 'Method'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Path", BuildRecipeValidateCommand.Path)
        self._CheckAttributes({self.__AttribName, self.__AttribMethod, self._AttribHelp})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        method = self._ReadAttrib(xmlElement, self.__AttribMethod)

        self.Method = BuildRecipeValidateMethod.FromString(method)

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        self.Name = IOUtil.NormalizePath(self.Name)



class XmlRecipeValidateCommandFindFileInPath(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribExpectedPath = 'ExpectedPath'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "FindFileInPath", BuildRecipeValidateCommand.FindFileInPath)
        self._CheckAttributes({self.__AttribName, self.__AttribExpectedPath, self._AttribHelp})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        self.ExpectedPath = self._TryReadAttrib(xmlElement, self.__AttribExpectedPath)

        if '\\' in self.Name or '/' in self.Name:
            raise Exception("A filename can not contain backslash '\\' or slash '/': '{0}'".format(self.Name))

        if not self.ExpectedPath is None:
            if '\\' in self.ExpectedPath:
                raise Exception("A path can not contain backslash '\\': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.startswith('/'):
                raise Exception("A path can not start with a slash '/': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.endswith('/'):
                raise Exception("A path can not end with a slash '/': '{0}'".format(self.ExpectedPath))
            self.ExpectedPath = IOUtil.NormalizePath(self.ExpectedPath)


class XmlRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning(XmlBase):
    __AttribStartVersion = 'StartVersion'
    __AttribEndVersion = 'EndVersion'
    __AttribHelp = 'Help'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribStartVersion, self.__AttribEndVersion, self.__AttribHelp})
        self.StartVersion = self._ReadAttrib(xmlElement, self.__AttribStartVersion)
        self.EndVersion = self._TryReadAttrib(xmlElement, self.__AttribEndVersion)
        self.Help = self._ReadAttrib(xmlElement, self.__AttribHelp)

        trimmed = self.StartVersion.strip()
        if trimmed != self.StartVersion:
            raise Exception("StartVersion contained leading or ending whitespaces")
        if self.EndVersion is not None:
            trimmed = self.EndVersion.strip()
            if trimmed != self.EndVersion:
                raise Exception("EndVersion contained leading or ending whitespaces")


class XmlRecipeValidateCommandFindExecutableFileInPath(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribAlternatives = 'Alternatives'
    __AttribExpectedPath = 'ExpectedPath'
    __AttribMinVersion = 'MinVersion'
    __AttribVersionCommand = 'VersionCommand'
    __AttribVersionRegEx = 'VersionRegEx'
    __AttribVersionSplitChar = 'VersionSplitChar'


    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "FindFileInPath", BuildRecipeValidateCommand.FindExecutableFileInPath)
        self._CheckAttributes({self.__AttribName, self.__AttribAlternatives, self.__AttribExpectedPath, self.__AttribMinVersion, self.__AttribVersionCommand, self.__AttribVersionRegEx, self._AttribHelp, self.__AttribVersionSplitChar})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)
        alternatives = self._TryReadAttrib(xmlElement, self.__AttribAlternatives)
        self.ExpectedPath = self._TryReadAttrib(xmlElement, self.__AttribExpectedPath)
        self.MinVersion = self._TryReadAttrib(xmlElement, self.__AttribMinVersion)
        self.VersionCommand = self._TryReadAttrib(xmlElement, self.__AttribVersionCommand)
        self.VersionRegEx = self._TryReadAttrib(xmlElement, self.__AttribVersionRegEx)
        self.VersionSplitChar = self._ReadAttrib(xmlElement, self.__AttribVersionSplitChar, ".")

        self.AddOnErrorWarning = self.__ParseAddOnErrorWarning(log, xmlElement)

        self.Alternatives = self.__ParseAlternatives(alternatives)

        if '\\' in self.Name or '/' in self.Name:
            raise Exception("A filename can not contain backslash '\\' or slash '/': '{0}'".format(self.Name))

        if not self.ExpectedPath is None:
            if '\\' in self.ExpectedPath:
                raise Exception("A path can not contain backslash '\\': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.startswith('/'):
                raise Exception("A path can not start with a slash '/': '{0}'".format(self.ExpectedPath))
            if self.ExpectedPath.endswith('/'):
                raise Exception("A path can not end with a slash '/': '{0}'".format(self.ExpectedPath))
            self.ExpectedPath = IOUtil.NormalizePath(self.ExpectedPath)

        self.__ValidateName()
        self.__ValidateVersionCheck()

    def __ParseAddOnErrorWarning(self, log: Log, xmlElement: ET.Element) -> List[XmlRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning]:
        entries = xmlElement.findall('AddOnErrorWarning')
        return [XmlRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning(log, entry) for entry in entries]

    def __ParseAlternatives(self, alternatives: Optional[str]) -> List[str]:
        if alternatives is None:
            return []
        return alternatives.split(",")

    def __ValidateName(self) -> None:
        name = self.Name
        trimmed = name.strip()
        if trimmed != name:
            raise Exception("Name contained leading or ending whitespaces'{0}'".format(name))
        if len(name) <= 0:
            raise Exception("Name length must be greater than zero")
        if not Util.IsValidCommandName(name):
            raise Exception("Name must start with a a-z or A-Z and can only contain a-z,A-Z,0-9,_ and - '{0}'".format(name))
        if name.lower() in BannedCommands.Commands:
            raise Exception("The command '{0}' is banned".format(name))

    def __ValidateVersionCheck(self) -> None:
        if self.MinVersion is None and self.VersionCommand is None and self.VersionRegEx is None:
            return
        if (self.MinVersion is None and len(self.AddOnErrorWarning) == 0) or self.VersionCommand is None or self.VersionRegEx is None:
            missingAttribs = []
            if self.MinVersion is None:
                missingAttribs.append("MinVersion")
            if self.VersionCommand is None:
                missingAttribs.append("VersionCommand")
            if self.VersionRegEx is None:
                missingAttribs.append("VersionRegEx")
            raise Exception("{0} are not defined".format(", ".join(missingAttribs)))
        if self.MinVersion is not None:
            trimmed = self.MinVersion.strip()
            if trimmed != self.MinVersion:
                raise Exception("MinVersion contained leading or ending whitespaces")
        trimmed = self.VersionCommand.strip()
        if trimmed != self.VersionCommand:
            raise Exception("VersionCommand contained leading or ending whitespaces")
        trimmed = self.VersionRegEx.strip()
        if trimmed != self.VersionRegEx:
            raise Exception("VersionRegEx contained leading or ending whitespaces")


class XmlRecipeValidateCommandAddHeaders(XmlRecipeValidateCommand):
    __AttribName = 'Name'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "AddHeaders", BuildRecipeValidateCommand.AddHeaders)
        self._CheckAttributes({self.__AttribName, self._AttribHelp})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        self.Name = IOUtil.NormalizePath(self.Name)


class XmlRecipeValidateCommandAddLib(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribDebugName = 'DebugName'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "AddLib", BuildRecipeValidateCommand.AddLib)
        self._CheckAttributes({self.__AttribName, self.__AttribDebugName, self._AttribHelp})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)  # type:str
        self.DebugName = self._ReadAttrib(xmlElement, self.__AttribDebugName, self.Name)  # type:str

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        if '\\' in self.DebugName:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.DebugName))
        if self.DebugName.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.DebugName))


class XmlRecipeValidateCommandAddDLL(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribDebugName = 'DebugName'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "AddDLL", BuildRecipeValidateCommand.AddDLL)
        self._CheckAttributes({self.__AttribName, self.__AttribDebugName, self._AttribHelp})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)  # type:str
        self.DebugName = self._ReadAttrib(xmlElement, self.__AttribDebugName, self.Name)  # type:str

        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        if '\\' in self.DebugName:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.DebugName))
        if self.DebugName.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.DebugName))


class XmlRecipeValidateCommandAddTool(XmlRecipeValidateCommand):
    __AttribName = 'Name'
    __AttribMinVersion = 'MinVersion'
    __AttribVersionCommand = 'VersionCommand'
    __AttribVersionRegEx = 'VersionRegEx'
    __AttribVersionSplitChar = 'VersionSplitChar'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "AddTool", BuildRecipeValidateCommand.AddTool)
        self._CheckAttributes({self.__AttribName, self.__AttribMinVersion, self.__AttribVersionCommand, self.__AttribVersionRegEx, self._AttribHelp, self.__AttribVersionSplitChar})
        self.Name = self._ReadAttrib(xmlElement, self.__AttribName)  # type:str
        self.MinVersion = self._TryReadAttrib(xmlElement, self.__AttribMinVersion)
        self.VersionCommand = self._TryReadAttrib(xmlElement, self.__AttribVersionCommand)
        self.VersionRegEx = self._TryReadAttrib(xmlElement, self.__AttribVersionRegEx)
        self.VersionSplitChar = self._ReadAttrib(xmlElement, self.__AttribVersionSplitChar, ".")
        if '\\' in self.Name:
            raise Exception("A path can not contain backslash '\\': '{0}'".format(self.Name))
        if self.Name.endswith('/'):
            raise Exception("A path can not end with a slash '/': '{0}'".format(self.Name))
        self.__ValidateVersionCheck()

    def __ValidateVersionCheck(self) -> None:
        if self.MinVersion is None and self.VersionCommand is None and self.VersionRegEx is None:
            return
        if self.MinVersion is None or self.VersionCommand is None or self.VersionRegEx is None:
            missingAttribs = []
            if self.MinVersion is None:
                missingAttribs.append("MinVersion")
            if self.VersionCommand is None:
                missingAttribs.append("VersionCommand")
            if self.VersionRegEx is None:
                missingAttribs.append("VersionRegEx")
            raise Exception("{0} are not defined".format(", ".join(missingAttribs)))
        if self.MinVersion is not None:
            trimmed = self.MinVersion.strip()
            if trimmed != self.MinVersion:
                raise Exception("MinVersion contained leading or ending whitespaces")
        trimmed = self.VersionCommand.strip()
        if trimmed != self.VersionCommand:
            raise Exception("VersionCommand contained leading or ending whitespaces")
        trimmed = self.VersionRegEx.strip()
        if trimmed != self.VersionRegEx:
            raise Exception("VersionRegEx contained leading or ending whitespaces")

class XmlRecipeInstallation(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes(set())
        self.CommandList = self.__GetCommandList(log, xmlElement)

    def __GetCommandList(self, log: Log, xmlElement: ET.Element) -> List[XmlRecipeValidateCommand]:
        elements = [] # type: List[XmlRecipeValidateCommand]
        if xmlElement is not None:
            for child in xmlElement:
                if child.tag == 'EnvironmentVariable':
                    elements.append(XmlRecipeValidateCommandEnvironmentVariable(log, child))
                elif child.tag == 'Path':
                    elements.append(XmlRecipeValidateCommandPath(log, child))
                elif child.tag == 'FindFileInPath':
                    elements.append(XmlRecipeValidateCommandFindFileInPath(log, child))
                elif child.tag == 'FindExecutableFileInPath':
                    elements.append(XmlRecipeValidateCommandFindExecutableFileInPath(log, child))
                elif child.tag == 'AddHeaders':
                    elements.append(XmlRecipeValidateCommandAddHeaders(log, child))
                elif child.tag == 'AddLib':
                    elements.append(XmlRecipeValidateCommandAddLib(log, child))
                elif child.tag == 'AddDLL':
                    elements.append(XmlRecipeValidateCommandAddDLL(log, child))
                elif child.tag == 'AddTool':
                    elements.append(XmlRecipeValidateCommandAddTool(log, child))
                else:
                    raise Exception("Unknown validation command element '{0}' found. Valid commands: {1}".format(child.tag, g_validValidateCommands))
        return elements


class XmlRecipePipelineBasicCommand(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element, commandName: str, commandType: BuildRecipePipelineCommand, buildToolPackageName: Optional[str] = None) -> None:
        super().__init__(log, xmlElement)
        self.CommandName = commandName  # type: str
        self.CommandType = commandType  # type: BuildRecipePipelineCommand
        self.BuildToolPackageNames = None if buildToolPackageName is None else [buildToolPackageName] # type: Optional[List[str]]


class XmlRecipePipelineJoinCommand(XmlRecipePipelineBasicCommand):
    pass
    #def __init__(self, log, xmlElement: ET.Element, commandName, commandType, buildToolPackageName=None) -> None:
    #    super().__init__(log, xmlElement, commandName, commandType, buildToolPackageName)



class XmlRecipePipelineJoinCommandCopy(XmlRecipePipelineJoinCommand):
    __AttribFrom = 'From'
    __AttribTo = 'To'
    __AttribOverwrite = 'Overwrite'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Copy", BuildRecipePipelineCommand.JoinCopy)
        self._CheckAttributes({self.__AttribFrom, self.__AttribTo, self.__AttribOverwrite})
        self.From = self._ReadAttrib(xmlElement, self.__AttribFrom)
        self.To = self._ReadAttrib(xmlElement, self.__AttribTo)
        self.Overwrite = self._ReadBoolAttrib(xmlElement, self.__AttribOverwrite, False)


class XmlRecipePipelineJoinCommandDelete(XmlRecipePipelineJoinCommand):
    __AttribPath = 'Path'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Delete", BuildRecipePipelineCommand.JoinDelete)
        self._CheckAttributes({self.__AttribPath})
        self.Path = self._ReadAttrib(xmlElement, self.__AttribPath)


class XmlRecipePipelineJoinCommandUnpack(XmlRecipePipelineJoinCommand):
    __AttribFrom = 'From'
    __AttribTo = 'To'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Unpack", BuildRecipePipelineCommand.JoinUnpack)
        self._CheckAttributes({self.__AttribFrom, self.__AttribTo})
        self.From = self._ReadAttrib(xmlElement, self.__AttribFrom)
        self.To = self._ReadAttrib(xmlElement, self.__AttribTo, '')


class XmlRecipePipelineJoinCommandGitApply(XmlRecipePipelineJoinCommand):
    __AttribFrom = 'From'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "GitApply", BuildRecipePipelineCommand.JoinGitApply, buildToolPackageName=g_GIT_PACKAGE_NAME)
        self._CheckAttributes({self.__AttribFrom})
        self.From = self._ReadAttrib(xmlElement, self.__AttribFrom)


class XmlRecipePipelineCommand(XmlRecipePipelineBasicCommand):
    _AttribOutputPath = 'OutputPath'

    def __init__(self, log: Log, xmlElement: ET.Element, commandName: str, commandType: BuildRecipePipelineCommand,
                 outputPathAllowed: bool = True, buildToolPackageName: Optional[str] = None, allowJoinCommandList: bool = True) -> None:
        super().__init__(log, xmlElement, commandName, commandType, buildToolPackageName)
        self.JoinCommandList = self.__GetJoinCommandList(log, xmlElement) if allowJoinCommandList else []
        self.OutputPath = None  # type: Optional[str]
        if outputPathAllowed:
            self.OutputPath = self._ReadAttrib(xmlElement, self._AttribOutputPath, '')


    def __GetJoinCommandList(self, log: Log, xmlElement: ET.Element) -> List[XmlRecipePipelineJoinCommand]:
        elements = []  # type: List[XmlRecipePipelineJoinCommand]
        if xmlElement is not None:
            for child in xmlElement:
                if child.tag == 'Copy':
                    elements.append(XmlRecipePipelineJoinCommandCopy(log, child))
                elif child.tag == 'Unpack':
                    elements.append(XmlRecipePipelineJoinCommandUnpack(log, child))
                elif child.tag == 'GitApply':
                    elements.append(XmlRecipePipelineJoinCommandGitApply(log, child))
                elif child.tag == 'Delete':
                    elements.append(XmlRecipePipelineJoinCommandDelete(log, child))
                else:
                    raise Exception("Unknown join command element '{0}' found in command {1}. Valid join commands: {2}".format(child.tag, self.CommandName, g_validJoinCommands))

        return elements


class XmlRecipePipelineFetchCommand(XmlRecipePipelineCommand):
    def __init__(self, log: Log,
                 xmlElement: ET.Element,
                 commandName: str,
                 commandType: BuildRecipePipelineCommand,
                 outputPathAllowed: bool = True,
                 buildToolPackageName: Optional[str] = None,
                 allowJoinCommandList: bool = True) -> None:
        super().__init__(log, xmlElement, commandName, commandType, outputPathAllowed, buildToolPackageName, allowJoinCommandList)
        self.Hash = None  # type: Optional[str]


class XmlRecipePipelineFetchCommandGitClone(XmlRecipePipelineFetchCommand):
    __AttribURL = 'URL'
    __AttribTag = 'Tag'
    __AttribHash = 'Hash'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "GitClone", BuildRecipePipelineCommand.GitClone, buildToolPackageName=g_GIT_PACKAGE_NAME, allowJoinCommandList=False)
        self._CheckAttributes({self._AttribOutputPath, self.__AttribURL, self.__AttribTag, self.__AttribHash})
        self.URL = self._ReadAttrib(xmlElement, self.__AttribURL)
        self.Tag = self._ReadAttrib(xmlElement, self.__AttribTag, '')
        self.Hash = self._TryReadAttrib(xmlElement, self.__AttribHash)

        #if len(self.URL) <= 0:
        #    raise Exception


class XmlRecipePipelineFetchCommandDownload(XmlRecipePipelineFetchCommand):
    __AttribURL = 'URL'
    __AttribTo = 'To'
    __AttribHash = 'Hash'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Download", BuildRecipePipelineCommand.Download, allowJoinCommandList=False)
        self._CheckAttributes({self._AttribOutputPath, self.__AttribURL, self.__AttribTo, self.__AttribHash})
        self.URL = self._ReadAttrib(xmlElement, self.__AttribURL)
        self.To = self._TryReadAttrib(xmlElement, self.__AttribTo)
        self.Hash = self._TryReadAttrib(xmlElement, self.__AttribHash)


class XmlRecipePipelineFetchCommandSource(XmlRecipePipelineFetchCommand):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Source", BuildRecipePipelineCommand.Source, allowJoinCommandList=False)
        self._CheckAttributes({self._AttribOutputPath})


class XmlRecipePipelineBuildCommand(XmlRecipePipelineCommand):
    pass


class XmlRecipePipelineCommandUnpack(XmlRecipePipelineBuildCommand):
    __AttribFile = 'File'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Unpack", BuildRecipePipelineCommand.Unpack)
        self._CheckAttributes({self._AttribOutputPath, self.__AttribFile})
        self.File = self._ReadAttrib(xmlElement, self.__AttribFile)


class XmlRecipePipelineCommandCMakeBuild(XmlRecipePipelineBuildCommand):
    __AttribSource = 'Source'
    __AttribProject = 'Project'
    __AttribTarget = 'Target'
    __AttribConfiguration = 'Configuration'
    __AttribOptions = 'Options'
    __AttribOutputPath = 'OutputPath'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "CMakeBuild", BuildRecipePipelineCommand.CMakeBuild, False, buildToolPackageName=g_CMAKE_PACKAGE_NAME)
        self._CheckAttributes({self._AttribOutputPath, self.__AttribSource, self.__AttribProject, self.__AttribTarget, self.__AttribConfiguration, self.__AttribOptions, self.__AttribOutputPath})
        self.Source = self._TryReadAttrib(xmlElement, self.__AttribSource)
        self.Project = self._ReadAttrib(xmlElement, self.__AttribProject)
        target = self._ReadAttrib(xmlElement, self.__AttribTarget)
        configuration = self._ReadAttrib(xmlElement, self.__AttribConfiguration)
        self.Options = self._ReadAttrib(xmlElement, self.__AttribOptions, '')
        self.Target = CMakeTargetType.FromString(target)
        self.OutputPath = self._ReadAttrib(xmlElement, self.__AttribOutputPath)

        self.ConfigurationList = self.__ParseConfiguration(configuration)


    def __ParseConfiguration(self, configuration: str) -> List[BuildVariantConfig]:
        configurations = configuration.split(';')
        configurationList = []
        for entry in configurations:
            config = BuildVariantConfig.FromString(entry)
            configurationList.append(config)
        return configurationList


class XmlRecipePipelineCommandCombine(XmlRecipePipelineBuildCommand):
    __AttribOutputPath = 'OutputPath'

    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Combine", BuildRecipePipelineCommand.Combine, False, allowJoinCommandList=False)
        self._CheckAttributes({self._AttribOutputPath, self.__AttribOutputPath})
        self.OutputPath = self._TryReadAttrib(xmlElement, self.__AttribOutputPath)
        self.CommandList = self.__GetCombineCommandList(log, xmlElement)
        if len(self.CommandList) <= 0:
            raise Exception("A Combine command must contain at least one commands")
        self.BuildToolPackageNames = []
        for command in self.CommandList:
            if command.BuildToolPackageNames is not None:
                for buildToolPackageName in command.BuildToolPackageNames:
                    if not buildToolPackageName in self.BuildToolPackageNames:
                        self.BuildToolPackageNames.append(buildToolPackageName)


    def __GetCombineCommandList(self, log: Log, xmlElement: ET.Element) -> List[XmlRecipePipelineBuildCommand]:
        elements = []  # type: List[XmlRecipePipelineBuildCommand]
        if xmlElement is not None:
            #isFirstCommand = True
            for child in xmlElement:
                command = _TryAllocatePipelineCombineCommand(log, child)
                if command is not None:
                    elements.append(command)
                else:
                    if not child.tag in g_validValidCombineCommands:
                        raise Exception("Unknown element '{0}' found in combine command list. Valid commands: {1}".format(child.tag, g_validValidCombineCommands))
                #isFirstCommand = False
        return elements


class XmlRecipePipelineCommandCopy(XmlRecipePipelineBuildCommand):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement, "Copy", BuildRecipePipelineCommand.Copy)
        self._CheckAttributes({self._AttribOutputPath})


def _TryAllocatePipelineFetchCommand(log: Log, xmlElement: ET.Element) -> Optional[XmlRecipePipelineFetchCommand]:
    if xmlElement.tag == 'GitClone':
        return XmlRecipePipelineFetchCommandGitClone(log, xmlElement)
    elif xmlElement.tag == 'Download':
        return XmlRecipePipelineFetchCommandDownload(log, xmlElement)
    elif xmlElement.tag == 'Source':
        return XmlRecipePipelineFetchCommandSource(log, xmlElement)
    return None


def _TryAllocatePipelineCommand(log: Log, xmlElement: ET.Element) -> Optional[XmlRecipePipelineBuildCommand]:
    if xmlElement.tag == 'Unpack':
        return XmlRecipePipelineCommandUnpack(log, xmlElement)
    elif xmlElement.tag == 'CMakeBuild':
        return XmlRecipePipelineCommandCMakeBuild(log, xmlElement)
    elif xmlElement.tag == 'Combine':
        return XmlRecipePipelineCommandCombine(log, xmlElement)
    elif xmlElement.tag == 'Copy':
        return XmlRecipePipelineCommandCopy(log, xmlElement)
    return None


def _TryAllocatePipelineCombineCommand(log: Log, xmlElement: ET.Element) -> Optional[XmlRecipePipelineBuildCommand]:
    if xmlElement.tag == 'CMakeBuild':
        return XmlRecipePipelineCommandCMakeBuild(log, xmlElement)
    return None


class XmlRecipePipeline(XmlBase):
    def __init__(self, log: Log, xmlElement: ET.Element) -> None:
        super().__init__(log, xmlElement)
        #self.Name = self._ReadAttrib(xmlElement, 'Name')
        self.CommandList = self.__GetCommandList(log, xmlElement)


    def __GetCommandList(self, log: Log, xmlElement: ET.Element) -> List[XmlRecipePipelineCommand]:
        elements = []  # type: List[XmlRecipePipelineCommand]
        if xmlElement is not None:
            isFirstCommand = True
            for child in xmlElement:
                command = _TryAllocatePipelineCommand(log, child) if not isFirstCommand else _TryAllocatePipelineFetchCommand(log, child)
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
    __AttribName = 'Name'
    __AttribVersion = 'Version'
    __AttribExternalInstallDirectory = 'ExternalInstallDirectory'
    __AttribFindVersion = 'FindVersion'
    __AttribFindTargetName = 'FindTargetName'
    __AttribFind = 'Find'

    def __init__(self, log: Log, xmlElement: ET.Element, defaultName: str) -> None:
        super().__init__(log, xmlElement)
        self._CheckAttributes({self.__AttribName, self.__AttribVersion, self.__AttribExternalInstallDirectory, self.__AttribFindVersion, self.__AttribFindTargetName, self.__AttribFind})
        self.ShortName = self._ReadAttrib(xmlElement, self.__AttribName, defaultName)
        self.Version = self._TryReadAttribAsVersion(xmlElement, self.__AttribVersion)  # type: Optional[Version]
        self.Pipeline = self.__TryGetPipeline(xmlElement)
        self.ValidateInstallation = self.__TryGetValidateInstallation(log, xmlElement)
        self.ExternalInstallDirectory = self._TryReadAttrib(xmlElement, self.__AttribExternalInstallDirectory)
        self.FindVersion = self._TryReadAttribAsVersion(xmlElement, self.__AttribFindVersion) # type: Optional[Version]
        self.FindTargetName = self._TryReadAttrib(xmlElement, self.__AttribFindTargetName)
        findResult = self._ReadBoolAttrib(xmlElement, self.__AttribFind, False)
        self.Find = False if findResult is None else findResult

        if self.FindVersion is not None:
            if findResult is not None and not findResult:
                self.FindVersion = None
                log.LogPrintVerbose(2, "Recipe specified Find=False, so discarding the specified FindVersion '{0}'".format(self.FindVersion))
            else:
                self.Find = True
                if self.Version is not None and not self.FindVersion.IsCompatible(self.Version):
                    raise Exception("Recipe '{0}' version {1} is not compatible with the specified FindVersion '{2}'.".format(self.ShortName, self.Version, self.FindVersion))

        if self.FindTargetName is not None:
            if findResult is not None and not findResult:
                self.FindTargetName = None
                log.LogPrintVerbose(2, "Recipe specified Find=False, so discarding the specified FindTargetName '{0}'".format(self.FindTargetName))
            else:
                self.Find = True

        #if self.Pipeline is None and self.ExternalInstallDirectory is None:
        #    raise Exception("Recipe '{0}' has to have either a pipeline or a ExternalInstallDirectory defined.".format(self.ShortName))
        if not self.Pipeline is None and not self.ExternalInstallDirectory is None:
            raise Exception("Recipe '{0}' can only a pipeline or a ExternalInstallDirectory defined not both.".format(self.ShortName))

        self.FullName = self.__GenerateName(self.ShortName, self.Version)

    def __GenerateName(self, name: str, version: Optional[Version]) -> str:
        return name if version is None else "{0}-{1}".format(name, version)


    def __TryGetPipeline(self, xmlElement: ET.Element) -> Optional[XmlRecipePipeline]:
        child = self._TryGetElement(xmlElement, 'Pipeline')
        if child is None:
            return None
        return XmlRecipePipeline(self.Log, child)


    def __TryGetValidateInstallation(self, log: Log, xmlElement: ET.Element) -> Optional[XmlRecipeInstallation]:
        child = self._TryGetElement(xmlElement, 'Installation')
        if child is None:
            if log.Verbosity >= 2:
                log.LogPrint("The Installation element is missing for recipe {0}".format(self.ShortName))
            return None
        return XmlRecipeInstallation(log, child)
