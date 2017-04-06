#!/usr/bin/env python
# -*- coding: utf-8 -*-

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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

import argparse
import datetime
import ntpath
import os
import re
import shutil

g_templatePath = "Templates/Projects"

g_templatePathGLES2 = "GLES2"
g_templatePathGLES3 = "GLES3"
g_templatePathOpenVG = "OpenVG"
g_templatePathG2D = "G2D"
g_templatePathVulkan = "Vulkan"
g_templatePathWindowVulkan = "WindowVulkan"
g_templatePathWillemsVulkan = "WillemsVulkan"
g_templatePathWillemsMeshVulkan = "WillemsMeshVulkan"
g_templatePathOpenCL = "OpenCL"
g_templatePathOpenCL1_1 = "OpenCL1_1"
g_templatePathOpenCL1_2 = "OpenCL1_2"
g_templatePathOpenCV2_4 = "OpenCV2_4"
g_templatePathOpenCV3_1 = "OpenCV3_1"
g_templatePathOpenCV3_2 = "OpenCV3_2"
g_templatePathOpenVX = "OpenVX"
g_templatePathOpenVX1_0_1 = "OpenVX1_0_1"
g_templatePathOpenVX1_1 = "OpenVX1_1"
g_templatePathConsole = "Console"
g_templatePathWindow = "Window"

g_defaultVCID = 'F73214FE-7A4B-4D7D-89EC-416B25E643BF'
g_templatePathCode = "Code"
g_replaceCODE_PROJECT_NAME = "%AG_PROJECT_NAME%"
g_replaceCODE_PROJECT_NAME_U = "%AG_PROJECT_NAME_U%"

g_templatePathFslGen = "FslGen"
g_replaceFslGen_PROJECT_NAME = "%AG_PROJECT_NAME%"
g_replaceFslGen_PROJECT_ID = "%AG_VS_PROJECT_ID%"

g_replace_YEAR = "%AG_YEAR%"

g_projectSource = "source"
g_currentYear = g_replace_YEAR

g_reservedProjectNames = [
    "FslBase",
    "FslGraphics",
    "FslGraphicsEGL",
    "FslDemoPlatform",
    "FslDemoHostEGL",
    "FslGraphicsGLES2",
    "FslGraphicsGLES3",
    "FslDemoApp",
    "FslDemoHost",
    "DemoNativeActivity",
    "ndk_helper",
    "native_app_glue",
    "cpufeatures",
]

def EnsureUTF8(str):
    try:
        return unicode(str.encode('utf-8'))
    except ValueError:
        return str

def GetFilesAt(directory, absolutePaths = None):
    if absolutePaths:
        return [ Join(directory,f) for f in os.listdir(directory) if os.path.isfile(os.path.join(directory,f)) ]
    else:
        return [ f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory,f)) ]

def ToUnixStylePath(path):
    if path == None:
        return None
    return path.replace("\\", "/")

def Join(path1, path2):
    return ToUnixStylePath(os.path.join(path1, path2))


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

        self.Files = [ FileConfig(Join(templatePath, 'source/DemoAppTemplate_Register.cpp'), Join(projectPath, f1)),
                       FileConfig(Join(templatePath, 'source/DemoAppTemplate.hpp'), Join(projectPath, f2)),
                       FileConfig(Join(templatePath, 'source/DemoAppTemplate.cpp'), Join(projectPath, f3)),
                     ]

        sourcePath = Join(templatePath, 'source')
        projectSourcePath = Join(projectPath, 'source')
        self.__AddFiles(self.Files, sourcePath, projectSourcePath)

    def __IsRegistered(self, fileConfigs, fileName):
        for fileConfig in fileConfigs:
            if fileName == fileConfig.SrcFile:
                return True
        return False        

    def __AddFiles(self, fileConfigs, sourcePath, projectSourcePath):
        files = GetFilesAt(sourcePath)
        newFiles = []
        for file in files:
            if not self.__IsRegistered(fileConfigs, Join(sourcePath,file)):
                newFiles.append(file)
        for newFile in newFiles:
            self.Files.append(FileConfig(Join(sourcePath,newFile), Join(projectSourcePath,newFile)))

class ConfigFslGen():
    def __init__(self, templatePath, projectPath, projectName, prefixedProjectName):
        f1 = 'Fsl.gen'
        self.Files = [ FileConfig(os.path.join(templatePath, 'Fsl.gen'), os.path.join(projectPath, f1)),
                     ]

class ConfigVariant():
    def __init__(self, templatePath, projectPath, projectName, projectPrefix):
        self.ProjectName = projectName
        self.ProjectPath = projectPath
        self.PrefixedProjectName = projectPrefix + projectName
        self.ProjectPrefix = projectPrefix
        self.ProjectPathSource = os.path.join(projectPath, g_projectSource)
        self.TemplatePathCode = os.path.join(templatePath, g_templatePathCode)
        self.TemplatePathFslGen = os.path.join(templatePath, g_templatePathFslGen)
        self.ConfigCode = ConfigCode(self.TemplatePathCode, projectPath, projectName, self.PrefixedProjectName)
        self.ConfigFslGen = ConfigFslGen(self.TemplatePathFslGen, projectPath, projectName, self.PrefixedProjectName)

class Config():
    def __init__(self, fslGraphicsSDKPath, projectPath, projectName, projectType, forced):
        self.ValidateProjectName(projectName)
        self.FslGraphicsSDKPath = fslGraphicsSDKPath
        self.ProjectName = projectName;
        self.ProjectPath = os.path.join(projectPath, projectName)
        self.TemplatePath = os.path.join(fslGraphicsSDKPath, g_templatePath)
        self.TemplatePathGLES2 = os.path.join(self.TemplatePath, g_templatePathGLES2)
        self.TemplatePathGLES3 = os.path.join(self.TemplatePath, g_templatePathGLES3)
        self.TemplatePathOpenVG = os.path.join(self.TemplatePath, g_templatePathOpenVG)
        self.TemplatePathG2D = os.path.join(self.TemplatePath, g_templatePathG2D)
        self.TemplatePathVulkan = os.path.join(self.TemplatePath, g_templatePathVulkan)
        self.TemplatePathWindowVulkan = os.path.join(self.TemplatePath, g_templatePathWindowVulkan)
        self.TemplatePathWillemsVulkan = os.path.join(self.TemplatePath, g_templatePathWillemsVulkan)
        self.TemplatePathWillemsMeshVulkan = os.path.join(self.TemplatePath, g_templatePathWillemsMeshVulkan)
        self.TemplatePathOpenCL = os.path.join(self.TemplatePath, g_templatePathOpenCL)
        self.TemplatePathOpenCL1_1 = os.path.join(self.TemplatePath, g_templatePathOpenCL1_1)
        self.TemplatePathOpenCL1_2 = os.path.join(self.TemplatePath, g_templatePathOpenCL1_2)
        self.TemplatePathOpenCV2_4 = os.path.join(self.TemplatePath, g_templatePathOpenCV2_4)
        self.TemplatePathOpenCV3_1 = os.path.join(self.TemplatePath, g_templatePathOpenCV3_1)
        self.TemplatePathOpenCV3_2 = os.path.join(self.TemplatePath, g_templatePathOpenCV3_2)
        self.TemplatePathOpenVX = os.path.join(self.TemplatePath, g_templatePathOpenVX)
        self.TemplatePathOpenVX1_0_1 = os.path.join(self.TemplatePath, g_templatePathOpenVX1_0_1)
        self.TemplatePathOpenVX1_1 = os.path.join(self.TemplatePath, g_templatePathOpenVX1_1)
        self.TemplatePathConsole = os.path.join(self.TemplatePath, g_templatePathConsole)
        self.TemplatePathWindow = os.path.join(self.TemplatePath, g_templatePathWindow)
        # Call this as the last thing since it references member variables
        self.ConfigVariant = self.PrepareProjectType(projectType, forced)


    def ValidateProjectName(self, projectName):
        if len(projectName) < 1:
            raise ArgumentError("A project name needs to contain atleast one character");
        if re.match("[a-zA-Z0-9_]", projectName) == None:
            raise ArgumentError("A project name can only contain alpha numeric characters, digits and underscores '%s'" % projectName);
        if not projectName[0].isalpha():
            raise ArgumentError("A project name needs to start with a alpha character '%s'" % projectName);
        for name in g_reservedProjectNames:
            if projectName == name:
                raise ArgumentError("The given project name '%s' is reserved" % projectName);


    def PrepareProjectType(self, projectType, forced):
        projectTypeLower = projectType.lower();
        if projectTypeLower == "gles2":
            return ConfigVariant(self.TemplatePathGLES2, self.ProjectPath, self.ProjectName, "GLES2_")
        elif projectTypeLower == "gles3":
            return ConfigVariant(self.TemplatePathGLES3, self.ProjectPath, self.ProjectName, "GLES3_")
        elif projectTypeLower == "openvg":
            return ConfigVariant(self.TemplatePathOpenVG, self.ProjectPath, self.ProjectName, "OPENVG_")
        elif projectTypeLower == "g2d":
            return ConfigVariant(self.TemplatePathG2D, self.ProjectPath, self.ProjectName, "G2D_")
        elif projectTypeLower == "vulkan":
            return ConfigVariant(self.TemplatePathVulkan, self.ProjectPath, self.ProjectName, "VULKAN_")
        elif projectTypeLower == "windowvulkan":
            return ConfigVariant(self.TemplatePathWindowVulkan, self.ProjectPath, self.ProjectName, "VULKAN_")
        elif projectTypeLower == "willemsvulkan":
            return ConfigVariant(self.TemplatePathWillemsVulkan, self.ProjectPath, self.ProjectName, "VULKAN_")
        elif projectTypeLower == "willemsmeshvulkan":
            return ConfigVariant(self.TemplatePathWillemsMeshVulkan, self.ProjectPath, self.ProjectName, "VULKAN_")
        elif projectTypeLower == "opencl":
            print("WARNING: Are you sure you inteded to create a generic OpenCL project instead of a version specific?");
            print("         Such a project must be able to compile with all versions of OpenCL. Its recommended to use OpenCL1.1 or OpenCL1.2");
            if not forced:
                raise ArgumentError("Use --Force to confirm you indeed wish to create such a project");
            return ConfigVariant(self.TemplatePathOpenCL, self.ProjectPath, self.ProjectName, "OPENCL_")
        elif projectTypeLower == "opencl1.1":
            return ConfigVariant(self.TemplatePathOpenCL1_1, self.ProjectPath, self.ProjectName, "OPENCL_")
        elif projectTypeLower == "opencl1.2":
            return ConfigVariant(self.TemplatePathOpenCL1_2, self.ProjectPath, self.ProjectName, "OPENCL_")
        elif projectTypeLower == "opencv2.4":
            return ConfigVariant(self.TemplatePathOpenCV2_4, self.ProjectPath, self.ProjectName, "OPENCV_")
        elif projectTypeLower == "opencv3.1":
            return ConfigVariant(self.TemplatePathOpenCV3_1, self.ProjectPath, self.ProjectName, "OPENCV_")
        elif projectTypeLower == "opencv3.2":
            return ConfigVariant(self.TemplatePathOpenCV3_2, self.ProjectPath, self.ProjectName, "OPENCV_")
        elif projectTypeLower == "openvx":
            print("WARNING: Are you sure you inteded to create a generic OpenVX project instead of a version specific?");
            print("         Such a project must be able to compile with all versions of OpenVX. Its recommended to use OpenVX1.0.1 or OpenVX1.1");
            if not forced:
                raise ArgumentError("Use --Force to confirm you indeed wish to create such a project");
            return ConfigVariant(self.TemplatePathOpenVX, self.ProjectPath, self.ProjectName, "OPENVX_")
        elif projectTypeLower == "openvx1.0.1":
            return ConfigVariant(self.TemplatePathOpenVX1_0_1, self.ProjectPath, self.ProjectName, "OPENVX_")
        elif projectTypeLower == "openvx1.1":
            return ConfigVariant(self.TemplatePathOpenVX1_1, self.ProjectPath, self.ProjectName, "OPENVX_")
        elif projectTypeLower == "console":
            return ConfigVariant(self.TemplatePathConsole, self.ProjectPath, self.ProjectName, "CONSOLE_")
        elif projectTypeLower == "window":
            return ConfigVariant(self.TemplatePathWindow, self.ProjectPath, self.ProjectName, "WINDOW_")
        else:
             raise ArgumentError("Unknown project type '%s'. Expected: GLES2, GLES3, OpenVG, Vulkan, WindowVulkan, WillemsVulkan, WillemsMeshVulkan, OpenCL, OpenCL1.1, OpenCL1.2, OpenCV2.4, OpenCV3.1, OpenVX, OpenVX1.0.1, OpenVX1.1, G2D, Console, or Window." % projectType);


def ReadFile(filename):
    content = None
    with open (filename, "r") as theFile:
        content=theFile.read()
    return content


def SaveFile(filename, content):
    with open (filename, "w") as theFile:
        theFile.write(content)


def ProcessCodeTemplate(filename, projectName, prefixedProjectName):
    content = ReadFile(filename)
    content = content.replace(g_replaceCODE_PROJECT_NAME, projectName)
    content = content.replace(g_replace_YEAR, g_currentYear)
    return content.replace(g_replaceCODE_PROJECT_NAME_U, projectName.upper())


def ProcessFslGenTemplate(filename, projectName, prefixedProjectName, projectId):
    content = ReadFile(filename)
    content = content.replace(g_replaceFslGen_PROJECT_NAME, projectName)
    content = content.replace(g_replaceFslGen_PROJECT_ID, projectId)
    content = content.replace(g_replace_YEAR, g_currentYear)
    return content


def CmdCode(configVariant):
    print('Preparing project template code')
    for fileConfig in configVariant.ConfigCode.Files:
        content = ProcessCodeTemplate(fileConfig.SrcFile, configVariant.ProjectName, configVariant.PrefixedProjectName)
        SaveFile(fileConfig.DstFile, content)


def CmdFslGen(configVariant, projectId):
    print('Preparing FslGen project')
    for fileConfig in configVariant.ConfigFslGen.Files:
        content = ProcessFslGenTemplate(fileConfig.SrcFile, configVariant.ProjectName, configVariant.PrefixedProjectName, projectId)
        SaveFile(fileConfig.DstFile, content)

def CmdAll(configVariant, projectId):
    CmdFslGen(configVariant, projectId)

def AddArgSubParser(subparsers, cmdName, cmdHelp):
    parser = subparsers.add_parser(cmdName, help=cmdHelp)
    parser.add_argument("projectName", help="The name of the new project")
    parser.add_argument('-t', '--type', required=True,  help='Select demo type: GLES2, GLES3, OpenVG, G2D, Vulkan, WindowVulkan, WillemsVulkan, WillemsMeshVulkan, OpenCL, OpenCL1.1, OpenCL1.2, OpenCV2.4, OpenCV3.1, OpenVX, OpenVX1.0.1, OpenVX1.1, Console, Window')
    parser.add_argument('--Force', action='store_true',  help='Force the project type to be created')
    parser.set_defaults(which=cmdName)
    return parser

def Main():
    global g_currentYear
    currentDir = os.getcwd()
    currentDir = EnsureUTF8(currentDir)

    dateNow = datetime.datetime.now();
    g_currentYear = "%s" % (dateNow.year)

    cmdAll = 'all'
    cmdFslGen = 'fslgen'

    parser = argparse.ArgumentParser(description='Create the project files for a DemoApp for the various supported platforms.')
    subparsers = parser.add_subparsers(title='subcommands',  description='valid subcommands', help='additional help')
    allParser = AddArgSubParser(subparsers, cmdAll, 'Create all build configurations')
    allParser.add_argument('-i', default=g_defaultVCID, help='Visual studio project id')

    fslGenParser = AddArgSubParser(subparsers, cmdFslGen, 'Create project files for FslGen')

    print('FslNewDemoProject V0.3.7')
    print("DEPRECATED: This tool has been deprecated please use 'FslBuildNew.py' instead.")
    try:
        fslGraphicsSDKPath = os.environ.get('FSL_GRAPHICS_SDK')
        if fslGraphicsSDKPath == None:
            raise EnvironmentError("FSL_GRAPHICS_SDK environment variable not set");
        fslGraphicsSDKPath = EnsureUTF8(fslGraphicsSDKPath)

        args = parser.parse_args()

        config = Config(fslGraphicsSDKPath, currentDir, args.projectName, args.type, args.Force)
        configVariant = config.ConfigVariant;

        if os.path.isdir(configVariant.ProjectPath):
            raise EnvironmentError("The project directory already exist: '%s'" % (configVariant.ProjectPath));
        elif os.path.exists(configVariant.ProjectPath):
            raise EnvironmentError("A file named '%s' already exist." % (configVariant.ProjectPath));
        else:
            os.mkdir(configVariant.ProjectPath)
            os.mkdir(configVariant.ProjectPathSource)

        if args.which == cmdAll:
            CmdCode(configVariant)
            CmdAll(configVariant, args.i)
        if args.which == cmdFslGen:
            CmdCode(configVariant)
            CmdFslGen(configVariant, args.i)

    except (ArgumentError) as ex:
        print "ERROR: %s" % ex.message
    except (EnvironmentError) as ex:
        print "ERROR: %s" % ex.message
        raise ex

Main()
