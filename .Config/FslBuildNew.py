#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

from FslBuildGen import IOUtil, PluginConfig, ParseUtil, Util, PackageListUtil
from FslBuildGen.XmlNewTemplateFile import XmlNewTemplateFile
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.Exceptions import *
from FslBuildGen.Main import *
from FslBuildGen.PlatformUtil import *
from FslBuildGen.PackageListUtil import GetTopLevelPackage
from FslBuildGen.Build import Builder
from FslBuildGen.Tool import ToolProgramFlow
from FslBuildGen.Tool.ToolProgramFlowConfig import ProgramFlowConfig
from FslBuildGen.BuildConfig import ScanSourceFiles
import datetime
import re
import uuid


g_templatePath = "Templates/Projects"
g_templateFileName = "Template.xml"

g_defaultVCID = 'F73214FE-7A4B-4D7D-89EC-416B25E643BF'
g_templatePathCode = "Code"
g_templatePathFslGen = "FslGen"
g_projectInclude = "include"
g_projectSource = "source"


def __GetTitle():
    return 'FslBuildNew V0.2.3 - SDK New project wizard'


class ArgumentError(Exception):
  """Indicate that a invalid argument was supplied 
  """

class FileConfig():
    def __init__(self, srcFile, dstFile):
        self.SrcFile = srcFile
        self.DstFile = dstFile

class ConfigCode():
    def __init__(self, templatePath, projectPath, projectName, prefixedProjectName):
        f1 = 'source/%s_Register.cpp' % (projectName)
        f2 = 'source/%s.hpp' % (projectName)
        f3 = 'source/%s.cpp' % (projectName)

        self.Files = []
        # quick fix to keep it compatible with the old tool
        self.__AddSpecialFile(self.Files, IOUtil.Join(templatePath, 'source/DemoAppTemplate_Register.cpp'), IOUtil.Join(projectPath, f1))
        self.__AddSpecialFile(self.Files, IOUtil.Join(templatePath, 'source/DemoAppTemplate.hpp'), IOUtil.Join(projectPath, f2))
        self.__AddSpecialFile(self.Files, IOUtil.Join(templatePath, 'source/DemoAppTemplate.cpp'), IOUtil.Join(projectPath, f3))

        sourcePath = IOUtil.Join(templatePath, 'source')
        projectSourcePath = IOUtil.Join(projectPath, 'source')
        self.__AddFiles(self.Files, sourcePath, projectSourcePath)

    def __AddSpecialFile(self, files, srcPath, dstPath):
        if IOUtil.IsFile(srcPath):
            files.append(FileConfig(srcPath, dstPath))

    def __IsRegistered(self, fileConfigs, fileName):
        for fileConfig in fileConfigs:
            if fileName == fileConfig.SrcFile:
                return True
        return False        

    def __AddFiles(self, fileConfigs, sourcePath, projectSourcePath):
        if not IOUtil.IsDirectory(sourcePath):
            return
        files = IOUtil.GetFilesAt(sourcePath, None)
        newFiles = []
        for file in files:
            if not self.__IsRegistered(fileConfigs, IOUtil.Join(sourcePath,file)):
                newFiles.append(file)
        for newFile in newFiles:
            self.Files.append(FileConfig(IOUtil.Join(sourcePath,newFile), IOUtil.Join(projectSourcePath,newFile)))

class ConfigFslGen():
    def __init__(self, genFileName, templatePath, projectPath, projectName, prefixedProjectName):
        f1 = genFileName
        self.Files = [FileConfig(os.path.join(templatePath, f1), os.path.join(projectPath, f1))]


class ConfigVariant():
    def __init__(self, genFileName, template, projectPath, projectName):
        self.Template = template
        self.ProjectName = projectName
        self.ProjectPath = projectPath
        self.PrefixedProjectName = template.Prefix + projectName
        self.ProjectPrefix = template.Prefix
        self.ProjectPathInclude = os.path.join(projectPath, g_projectInclude)
        self.ProjectPathSource = os.path.join(projectPath, g_projectSource)
        self.TemplatePathCode = os.path.join(template.Path, g_templatePathCode)
        self.TemplatePathFslGen = os.path.join(template.Path, g_templatePathFslGen)
        self.ConfigCode = ConfigCode(self.TemplatePathCode, projectPath, projectName, self.PrefixedProjectName)
        self.ConfigFslGen = ConfigFslGen(genFileName, self.TemplatePathFslGen, projectPath, projectName, self.PrefixedProjectName)


class LocalConfig():
    def __init__(self, fslGraphicsSDKPath, genFileName, projectPath, projectName, projectType, forced, templates, reservedProjectNames):
        dateNow = datetime.datetime.now();
        self.CurrentYear = "%s" % (dateNow.year)

        self.ValidateProjectName(projectName, reservedProjectNames)
        self.FslGraphicsSDKPath = fslGraphicsSDKPath
        self.ProjectName = projectName;
        self.ProjectPath = os.path.join(projectPath, projectName)
        self.TemplatePath = os.path.join(fslGraphicsSDKPath, g_templatePath)
        self.TemplatePathProjectType = os.path.join(self.TemplatePath, projectType)
        # Call this as the last thing since it references member variables
        self.ConfigVariant = self.PrepareProjectType(genFileName, templates, projectType, forced)


    def ValidateProjectName(self, projectName, reservedProjectNames):
        if len(projectName) < 1:
            raise ArgumentError("A project name needs to contain atleast one character");
        if re.match("[a-zA-Z0-9_]", projectName) == None:
            raise ArgumentError("A project name can only contain alpha numeric characters, digits and underscores '%s'" % projectName);
        if not projectName[0].isalpha():
            raise ArgumentError("A project name needs to start with a alpha character '%s'" % projectName);
        for name in reservedProjectNames:
            if projectName == name:
                raise ArgumentError("The given project name '%s' is reserved" % projectName);


    def PrepareProjectType(self, genFileName, templates, projectType, forced):
        templateDict = {template.Id:template for template in templates}
        projectTypeId = projectType.lower();

        if projectTypeId in templateDict.keys():
            template = templateDict[projectTypeId]
            if template.Template.Force:
                if len(template.Template.Warning) > 0:
                    print("WARNING: %s" % template.Template.Warning)
                if not forced:
                    raise ArgumentError("Use --Force to confirm you indeed wish to create such a project");
            return ConfigVariant(genFileName, template, self.ProjectPath, self.ProjectName)
        else:
             templateNames = [template.Name for template in templates]
             raise ArgumentError("Unknown project type '%s'. Expected: %s." % (projectType, ", ".join(templateNames)));


def ProcessCodeTemplate(filename, projectName, prefixedProjectName, strCurrentYear):
    content = IOUtil.ReadFile(filename)
    content = content.replace("%AG_PROJECT_NAME%", projectName)
    content = content.replace("%AG_YEAR%", strCurrentYear)
    return content.replace("%AG_PROJECT_NAME_U%", projectName.upper())


def ProcessFslGenTemplate(filename, projectName, prefixedProjectName, strCurrentYear, projectId):
    content = IOUtil.ReadFile(filename)
    content = content.replace("%AG_PROJECT_NAME%", projectName)
    content = content.replace("%AG_VS_PROJECT_ID%", projectId)
    content = content.replace("%AG_YEAR%", strCurrentYear)
    return content



def CmdCode(configVariant, strCurrentYear):
    print('Preparing project template code')
    for fileConfig in configVariant.ConfigCode.Files:
        content = ProcessCodeTemplate(fileConfig.SrcFile, configVariant.ProjectName, configVariant.PrefixedProjectName, strCurrentYear)
        IOUtil.WriteFile(fileConfig.DstFile, content)


def CmdFslGen(configVariant, strCurrentYear, projectId):
    print('Preparing FslGen project')
    for fileConfig in configVariant.ConfigFslGen.Files:
        content = ProcessFslGenTemplate(fileConfig.SrcFile, configVariant.ProjectName, configVariant.PrefixedProjectName, strCurrentYear, projectId)
        IOUtil.WriteFile(fileConfig.DstFile, content)


def CmdAll(configVariant, strCurrentYear, projectId):
    CmdFslGen(configVariant, strCurrentYear, projectId)


def ParsePackages(config, currentDir, platform):
    theFiles = DoGetFiles(config, currentDir)
    return DoGetPackages(config, theFiles, platform)


def __GenerateGUID(config, packages, guid):
    if guid != g_defaultVCID:
        return guid
    used = set()
    for package in packages:
        winPlatform = package.GetPlatform(PLATFORM_WINDOWS)
        if winPlatform:
            used.add(winPlatform.ProjectId)
    
    count = 0
    newGuid = ("%s" % (uuid.uuid4())).upper()
    while newGuid in used:
        if count > 100000:
            config.LogPrint("Failed to generate unique GUID in allocated time using default, please add a unique GUID manually");
            return guid
        newGuid = ("%s" % (uuid.uuid4())).upper()
        count = count + 1
    return newGuid
    

def ToolMain(funcGetTitle, args, currentDir, toolConfig, userTag):
    templates = userTag

    allowDevelopmentPlugins = True if args.dev else False
    config = Config(toolConfig, args.verbosity, 'sdk', None, allowDevelopmentPlugins)
    config.ForceDisableAllWrite()

    config.LogPrint(funcGetTitle())

    reservedProjectNames = set()
    packages = None
    if not args.NoParse:
        # Get the platform and see if its supported
        platform = PluginConfig.GetGeneratorPluginById(args.platform)
        CheckBuildPlatform(platform.Name)
        config.LogPrint("Active platform: %s" % (platform.Name))
        packages = ParsePackages(config, currentDir, platform)
        
    # Reserve the name of all packages
    for package in packages:
        reservedProjectNames.add(package.Name)


    localConfig = LocalConfig(config.SDKPath, config.GenFileName, currentDir, args.projectName, args.template, args.Force, templates, reservedProjectNames)
    configVariant = localConfig.ConfigVariant;

    if os.path.isdir(configVariant.ProjectPath):
        raise EnvironmentError("The project directory already exist: '%s'" % (configVariant.ProjectPath));
    elif os.path.exists(configVariant.ProjectPath):
        raise EnvironmentError("A file named '%s' already exist." % (configVariant.ProjectPath));
    else:
        IOUtil.SafeMakeDirs(configVariant.ProjectPath)
        IOUtil.SafeMakeDirs(configVariant.ProjectPathSource)
        if not localConfig.ConfigVariant.Template.Template.NoInclude:
            IOUtil.SafeMakeDirs(configVariant.ProjectPathInclude)

    visualStudioGUID = args.i
    if packages:
        visualStudioGUID = __GenerateGUID(config, packages, visualStudioGUID)

    CmdCode(configVariant, localConfig.CurrentYear)
    CmdAll(configVariant, localConfig.CurrentYear, visualStudioGUID)
    
    if not args.NoBuildGen:
        config.DoPrint("Generating build files");
        PluginConfig.SetVSVersion(args.VSVersion)
        projectConfig = Config(toolConfig, args.verbosity, PluginConfig.TYPE_DEFAULT, None, allowDevelopmentPlugins)
        theFiles = DoGetFiles(projectConfig, configVariant.ProjectPath)
        DoGenerateBuildFiles(projectConfig, theFiles, args.platform)


def __ScanTemplates():
    sdkPath = IOUtil.GetEnvironmentVariableForDirectory('FSL_GRAPHICS_SDK')
    templateRootPath = IOUtil.Join(sdkPath, g_templatePath)
    dirs = IOUtil.GetDirectoriesAt(templateRootPath, None)

    templates = []
    for possibleDir in dirs:
        dirPath = IOUtil.Join(templateRootPath, possibleDir)
        templatePath = IOUtil.Join(dirPath, g_templateFileName)
        if IOUtil.IsFile(templatePath):
            templates.append(XmlNewTemplateFile(templatePath))
    return templates


def __ParserAddCustomArguments(parser, userTag):
    templateList = userTag
    templateNames = [entry.Name for entry in templateList]
    templates = ", ".join(templateNames)
    parser.add_argument("template", help="The template for the new project: %s" % (templates))
    parser.add_argument("projectName", help="The name of the new project")
    parser.add_argument('--NoParse', action='store_true',  help='Disable the package parsing and validation')
    parser.add_argument('--NoBuildGen', action='store_true',  help='Disable platform build file generation')
    parser.add_argument('--Force', action='store_true',  help='Force the project type to be created')
    parser.add_argument('-i', default=g_defaultVCID, help='Visual studio project id')
    parser.add_argument('--VSVersion', default=VisualStudioVersion.VS2015,  help='Choose a specific visual studio version (2013,2015,2017)')


def Main():
    programFlowConfig = ProgramFlowConfig()
    programFlowConfig.AddPlatformArg = True
    ToolProgramFlow.ProgramMain(__GetTitle, __ParserAddCustomArguments, ToolMain, programFlowConfig, __ScanTemplates)


Main()
