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

import itertools
import os
import os.path
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen import IOUtil, MakeFileHelper, Util, PackageConfig
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.SharedGeneration import *
from FslBuildGen.PackageBuildReport import *

class TemplateOptimizationSetting(object):
    def __init__(self, optimizationType, snippetOptimizationType, snippetOptimizationOptions):
        self.OptimizationType = optimizationType
        self.SnippetOptimizationType = snippetOptimizationType
        self.SnippetOptimizationOptions = snippetOptimizationOptions


class CodeTemplateVC(object):
    def __init__(self, config, template, filePrefix, hasManifest, vsVersion, useLinuxTools):
        super(CodeTemplateVC, self).__init__()
        strVSPath = template.Path


        if hasManifest:
            self.Manifest = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}.manifest".format(filePrefix)))
        else:
            self.Manifest = ""
        self.TemplateSLN = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}.sln".format(filePrefix)))
        self.SLNSnippet1 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet1.sln".format(filePrefix)))
        self.SLNSnippet2 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet2.sln".format(filePrefix)))
        self.Master = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Master.vcxproj".format(filePrefix)))
        self.Snippet1 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet1.vcxproj".format(filePrefix)))
        self.Snippet2 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet2.vcxproj".format(filePrefix)))
        self.Snippet2_1 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet2_1.vcxproj".format(filePrefix)))
        self.Snippet3 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet3.vcxproj".format(filePrefix)))
        self.Snippet4 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet4.vcxproj".format(filePrefix)))
        self.Snippet5 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet5.vcxproj".format(filePrefix)))
        self.Snippet6 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet6.vcxproj".format(filePrefix)))
        self.Snippet7 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet7.vcxproj".format(filePrefix)))
        self.Snippet8 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet8.vcxproj".format(filePrefix)))
        self.Snippet8_1 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet8_1.vcxproj".format(filePrefix)))
        self.Snippet8_2 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet8_2.vcxproj".format(filePrefix)))
        self.Snippet9 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet9.vcxproj".format(filePrefix)))
        self.Snippet9_1 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet9_1.vcxproj".format(filePrefix)))

        self.DebugOptimizations = {}
        self.__LoadOptimization(self.DebugOptimizations, OptimizationType.Disabled, strVSPath, "DEBUG", "disabled")
        self.__LoadOptimization(self.DebugOptimizations, OptimizationType.Default, strVSPath, "DEBUG", "disabled")
        self.__LoadOptimization(self.DebugOptimizations, OptimizationType.Full, strVSPath, "DEBUG", "full")

        snippet3FileName = IOUtil.Join(strVSPath, "{0}Snippet3.sln".format(filePrefix))
        if IOUtil.IsFile(snippet3FileName):
            self.SLNSnippet3 = IOUtil.ReadFile(snippet3FileName)
            self.SLNSnippet4 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet4.sln".format(filePrefix)))
            self.SLNSnippet4_1 = IOUtil.ReadFile(IOUtil.Join(strVSPath, "{0}Snippet4_1.sln".format(filePrefix)))
        else:
            self.SLNSnippet3 = ""
            self.SLNSnippet4 = ""
            self.SLNSnippet4_1 = ""

        filterFileName = IOUtil.Join(strVSPath, "##PACKAGE_TARGET_NAME##.vcxproj.filters")
        self.FilterFile = IOUtil.ReadFile(filterFileName) if IOUtil.IsFile(filterFileName) else None

        self.ResouceFile = IOUtil.ReadFile(IOUtil.Join(strVSPath, "ExeIconResource.rc"))
        self.ExeProcessTextFiles = [IOUtil.Join(strVSPath, "resource.h")]

        self.ExeCopyFiles = [IOUtil.Join(config.SDKConfigTemplatePath, "Icons/FslSDKIcon.ico")]

    def __LoadOptimization(self, dict, optimizationType, strVSPath, prefix, postfix):
        snippetOptimizationType = self.SafeReadFile(IOUtil.Join(strVSPath, "{0}_OPTIMIZATION_TYPE_{1}.vcxproj".format(prefix, postfix)), "")
        snippetOptimizationOptions = self.SafeReadFile2(IOUtil.Join(strVSPath, "{0}_OPTIMIZATION_OPTIONS_{1}.vcxproj".format(prefix, postfix)), "")
        dict[optimizationType] = TemplateOptimizationSetting(optimizationType, snippetOptimizationType, snippetOptimizationOptions)

    def SafeReadFile(self, filename, defaultContent):
        content = IOUtil.TryReadFile(filename)
        return content if content != None else defaultContent

    def SafeReadFile2(self, filename, defaultContent):
        content = self.SafeReadFile(filename, defaultContent)
        if len(content) > 0:
            content = '\n' + content
        return content

class CodeTemplateProjectBatFiles(object):
    def __init__(self, config):
        super(CodeTemplateProjectBatFiles, self).__init__()
        self.TemplateStartBat = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_WinStartProject.txt"))
        self.TemplateBuildBat = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_WinBuildProject.txt"))
        self.TemplateRunBat = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_WinRunProject.txt"))
        self.TemplateSnippetErrorCheck = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "TemplateSnippet_WinBatErrorCheck.txt"))



class GeneratorVCTemplate(object):
    def __init__(self, config, platformName, vsVersion, languageTemplates):
        super(GeneratorVCTemplate, self).__init__()

        packageLanguage = config.ToolConfig.DefaultPackageLanguage
        projectConfig = config.ToolConfig.ProjectRootConfig        
        activeTemplate = projectConfig.DefaultTemplate

        self.UsingLinuxTools = (platformName != PackageConfig.PLATFORM_WINDOWS) and activeTemplate == MagicStrings.VSDefaultCPPTemplate
        if self.UsingLinuxTools:
            activeTemplate = MagicStrings.VSDefaultCPPLinuxTemplate
        
        template = languageTemplates.TryGet(activeTemplate)
        if template == None:
            raise UnsupportedException("No template found for '{0}' named '{1}'".format(PackageLanguage.ToString(packageLanguage), activeTemplate));
        
        self.Lib = CodeTemplateVC(config, template, "Lib", False, vsVersion, self.UsingLinuxTools)
        self.Exe = CodeTemplateVC(config, template, "Exe", True, vsVersion, self.UsingLinuxTools)
        self.Bat = CodeTemplateProjectBatFiles(config)
        self.HeaderLib = self.__LoadHeaderLib(config, template, vsVersion, self.UsingLinuxTools)


    def __LoadHeaderLib(self, config, template, vsVersion, usingLinuxTools):
        name = "HeaderLib"
        path = IOUtil.Join(template.Path, "{0}.sln".format(name)) 
        if not IOUtil.IsFile(path):
            return None
        return CodeTemplateVC(config, template, name, False, vsVersion, usingLinuxTools)

