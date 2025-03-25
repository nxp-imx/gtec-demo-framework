#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#****************************************************************************************************************************************************
# Copyright 2021 NXP
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
from FslBuildGen.BuildExternal.PackageRecipeInstallation import PackageRecipeInstallation
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommand import PackageRecipeValidateCommand
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandAddHeaders import PackageRecipeValidateCommandAddHeaders
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandAddDLL import PackageRecipeValidateCommandAddDLL
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandAddLib import PackageRecipeValidateCommandAddLib
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandAddTool import PackageRecipeValidateCommandAddTool
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandEnvironmentVariable import PackageRecipeValidateCommandEnvironmentVariable
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandFindExecutableFileInPath import PackageRecipeValidateCommandFindExecutableFileInPath
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning import PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandFindFileInPath import PackageRecipeValidateCommandFindFileInPath
from FslBuildGen.BuildExternal.Commands.PackageRecipeValidateCommandPath import PackageRecipeValidateCommandPath
from FslBuildGen.Location.PathBuilder import PathBuilder
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeInstallation
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddHeaders
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddLib
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddDLL
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddTool
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandEnvironmentVariable
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandFindFileInPath
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandFindExecutableFileInPath
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandPath

class PackageRecipeInstallationUtil(object):

    @staticmethod
    def ToPackageRecipeValidateCommandAddHeaders(pathBuilder: PathBuilder, command: XmlRecipeValidateCommandAddHeaders) -> PackageRecipeValidateCommandAddHeaders:
        originalName = command.Name
        resolvedName = pathBuilder.ResolveFilenameWithVariables(originalName)
        return PackageRecipeValidateCommandAddHeaders(originalName, resolvedName, command.Help)

    @staticmethod
    def ToPackageRecipeValidateCommandAddDLL(pathBuilder: PathBuilder, command: XmlRecipeValidateCommandAddDLL) -> PackageRecipeValidateCommandAddDLL:
        originalName = command.Name
        originalDebugName = command.DebugName
        resolvedName = pathBuilder.ResolveFilenameWithVariables(originalName)
        resolvedDebugName = pathBuilder.ResolveFilenameWithVariables(originalDebugName)
        return PackageRecipeValidateCommandAddDLL(originalName, originalDebugName, resolvedName, resolvedDebugName, command.Help)

    @staticmethod
    def ToPackageRecipeValidateCommandAddLib(pathBuilder: PathBuilder, command: XmlRecipeValidateCommandAddLib) -> PackageRecipeValidateCommandAddLib:
        originalName = command.Name
        originalDebugName = command.DebugName
        resolvedName = pathBuilder.ResolveFilenameWithVariables(originalName)
        resolvedDebugName = pathBuilder.ResolveFilenameWithVariables(originalDebugName)
        return PackageRecipeValidateCommandAddLib(originalName, originalDebugName, resolvedName, resolvedDebugName, command.Help)

    @staticmethod
    def ToPackageRecipeValidateCommandAddTool(pathBuilder: PathBuilder, command: XmlRecipeValidateCommandAddTool) -> PackageRecipeValidateCommandAddTool:
        return PackageRecipeValidateCommandAddTool(command.Name, command.MinVersion, command.VersionCommand, command.VersionRegEx, command.VersionSplitChar, command.Help)

    @staticmethod
    def ToPackageRecipeValidateCommandEnvironmentVariable(command: XmlRecipeValidateCommandEnvironmentVariable) -> PackageRecipeValidateCommandEnvironmentVariable:
        return PackageRecipeValidateCommandEnvironmentVariable(command.Name, command.Method, command.AllowEndSlash, command.Help)


    @staticmethod
    def ToPackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning(entry: XmlRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning) -> PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning:
        return PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning(entry.StartVersion, entry.EndVersion, entry.Help)

    @staticmethod
    def ToPackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarningList(sourceList: List[XmlRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning]) -> List[PackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning]:
        resultList = []
        for entry in sourceList:
            resultList.append(PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarning(entry))
        return resultList

    @staticmethod
    def ToPackageRecipeValidateCommandFindExecutableFileInPath(command: XmlRecipeValidateCommandFindExecutableFileInPath) -> PackageRecipeValidateCommandFindExecutableFileInPath:
        addOnErrorWarningList = PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandFindExecutableFileInPathAddOnErrorWarningList(command.AddOnErrorWarning)
        return PackageRecipeValidateCommandFindExecutableFileInPath(command.Name, command.Alternatives, command.ExpectedPath,
                                                                    command.MinVersion, command.VersionCommand, command.VersionRegEx,
                                                                    command.VersionSplitChar, addOnErrorWarningList, command.Help)

    @staticmethod
    def ToPackageRecipeValidateCommandFindFileInPath(command: XmlRecipeValidateCommandFindFileInPath) -> PackageRecipeValidateCommandFindFileInPath:
        return PackageRecipeValidateCommandFindFileInPath(command.Name, command.ExpectedPath, command.Help)


    @staticmethod
    def ToPackageRecipeValidateCommandPath(command: XmlRecipeValidateCommandPath) -> PackageRecipeValidateCommandPath:
        return PackageRecipeValidateCommandPath(command.Name, command.Method, command.Help)


    @staticmethod
    def ToPackageRecipeValidateCommand(pathBuilder: PathBuilder, command: XmlRecipeValidateCommand) -> PackageRecipeValidateCommand:
        if isinstance(command, XmlRecipeValidateCommandAddHeaders):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandAddHeaders(pathBuilder, command)
        if isinstance(command, XmlRecipeValidateCommandAddDLL):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandAddDLL(pathBuilder, command)
        if isinstance(command, XmlRecipeValidateCommandAddLib):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandAddLib(pathBuilder, command)
        if isinstance(command, XmlRecipeValidateCommandAddTool):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandAddTool(pathBuilder, command)
        if isinstance(command, XmlRecipeValidateCommandEnvironmentVariable):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandEnvironmentVariable(command)
        if isinstance(command, XmlRecipeValidateCommandFindExecutableFileInPath):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandFindExecutableFileInPath(command)
        if isinstance(command, XmlRecipeValidateCommandFindFileInPath):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandFindFileInPath(command)
        if isinstance(command, XmlRecipeValidateCommandPath):
            return PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandPath(command)
        raise Exception("Unknown validation command '{0}'".format(command.CommandName))


    @staticmethod
    def ToPackageRecipeValidateCommandList(pathBuilder: PathBuilder, commandList: List[XmlRecipeValidateCommand]) -> List[PackageRecipeValidateCommand]:
        resultList = []
        for command in commandList:
            resultList.append(PackageRecipeInstallationUtil.ToPackageRecipeValidateCommand(pathBuilder, command))
        return resultList

    @staticmethod
    def ToPackageRecipeInstallation(pathBuilder: PathBuilder, recipeInstallation: XmlRecipeInstallation) -> PackageRecipeInstallation:
        commandList = PackageRecipeInstallationUtil.ToPackageRecipeValidateCommandList(pathBuilder, recipeInstallation.CommandList)
        return PackageRecipeInstallation(commandList)
