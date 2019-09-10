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

from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import BuildRecipeValidateCommand
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommand
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlRecipeValidateCommandAddTool


class PackageToolRecord(object):
    def __init__(self, package: Package, resolvedInstallPath: str, addToolCommand: XmlRecipeValidateCommandAddTool) -> None:
        super().__init__()
        self.Package = package
        self.ToolName = IOUtil.GetFileNameWithoutExtension(addToolCommand.Name)
        self.RelativeToolPath = IOUtil.GetDirectoryName(addToolCommand.Name)
        self.AbsoluteToolPath = IOUtil.Join(resolvedInstallPath, self.RelativeToolPath)


class PackageToolFinder(object):
    def __init__(self, toolPackages: List[Package]) -> None:
        super().__init__()
        toolPackageByPackageNameDict = {}  # type: Dict[str, Package]
        for package in toolPackages:
            toolPackageByPackageNameDict[package.Name] = package

        self.__ToolPackageByPackageNameDict = toolPackageByPackageNameDict
        self.__ToolPackageByToolNameDict = self.__BuildToolPackageByName(toolPackages)
        self.ToolPaths = [package.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation.ResolvedPath
                          for package in toolPackages
                          if package.ResolvedDirectExperimentalRecipe is not None and package.ResolvedDirectExperimentalRecipe.ResolvedInstallLocation is not None]


    def TryGetToolPackageByPackageName(self, packageName: str) -> Optional[Package]:
        if packageName in self.__ToolPackageByPackageNameDict:
            return self.__ToolPackageByPackageNameDict[packageName]
        return None


    def GetToolPackageByPackageName(self, packageName: str) -> Package:
        package = self.TryGetToolPackageByPackageName(packageName)
        if package is None:
            raise Exception("Could not find tool package '{0}'".format(packageName))
        return package


    def TryGetToolPackageByToolName(self, toolName: str) -> Optional[PackageToolRecord]:
        if toolName in self.__ToolPackageByToolNameDict:
            return self.__ToolPackageByToolNameDict[toolName]
        return None

    def GetToolPackageByToolName(self, toolName: str) -> PackageToolRecord:
        package = self.TryGetToolPackageByToolName(toolName)
        if package is None:
            raise Exception("Could not find tool package by name '{0}'".format(toolName))
        return package


    def __BuildToolPackageByName(self, toolPackages: List[Package]) -> Dict[str, PackageToolRecord]:
        resDict = {} # type: Dict[str, PackageToolRecord]
        for package in toolPackages:
            recipe = package.ResolvedDirectExperimentalRecipe
            if recipe is not None and recipe.ResolvedInstallLocation is not None and recipe.ValidateInstallation is not None:
                addToolList = [] # type: List[XmlRecipeValidateCommand]
                validation = recipe.ValidateInstallation

                for command in validation.CommandList:
                    if command.CommandType == BuildRecipeValidateCommand.AddTool:
                        commandEx = cast(XmlRecipeValidateCommandAddTool, command)
                        toolName = IOUtil.GetFileNameWithoutExtension(commandEx.Name)
                        if toolName in resDict:
                            if resDict[toolName] != package:
                                raise Exception("Tool already registered by {0}".format(package.Name))
                        else:
                            resDict[toolName] = PackageToolRecord(package, recipe.ResolvedInstallLocation.ResolvedPath, commandEx)
        return resDict


