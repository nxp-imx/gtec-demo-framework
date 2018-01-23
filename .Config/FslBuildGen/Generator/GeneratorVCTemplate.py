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

from typing import cast
from typing import Dict
from typing import List
from typing import Optional
from FslBuildGen import IOUtil
from FslBuildGen import PackageConfig
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import OptimizationType
from FslBuildGen.DataTypes import MagicStrings
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Generator.VSVersionLanguageTemplates import VSVersionLanguageTemplates
from FslBuildGen.Packages.Package import Package
#from FslBuildGen.SharedGeneration import *
#from FslBuildGen.PackageGeneratorReport import *
from FslBuildGen.Template.TemplateFileRecordManager import TemplateFileRecordManager
#from FslBuildGen.Template.TemplateFileProcessor import TemplateFileProcessor
from FslBuildGen.Xml.XmlNewVSProjectTemplateFile import XmlNewVSProjectTemplateFile


class TemplateOptimizationSetting(object):
    def __init__(self, optimizationType: int, snippetOptimizationType: str, snippetOptimizationOptions: str) -> None:
        super(TemplateOptimizationSetting, self).__init__()
        self.OptimizationType = optimizationType
        self.SnippetOptimizationType = snippetOptimizationType
        self.SnippetOptimizationOptions = snippetOptimizationOptions


class NuGetPackageConfigSnippets(object):
    def __init__(self, config: Config, path: str, master: str) -> None:
        super(NuGetPackageConfigSnippets, self).__init__()
        self.Master = master
        self.PackageEntry = IOUtil.ReadFile(IOUtil.Join(path, "PackageEntry.txt"))


class CodeTemplateVC(object):
    def __init__(self, config: Config, template: XmlNewVSProjectTemplateFile,
                subDirectory: str, vsVersion: int, useLinuxTools: bool) -> None:
        super(CodeTemplateVC, self).__init__()
        strVSPath = template.Path

        strTemplatePath = IOUtil.Join(strVSPath, subDirectory)
        strTemplateSolutionPath = IOUtil.Join(strTemplatePath, "Template_sln")
        strTemplateProjectPath = IOUtil.Join(strTemplatePath, "Template_{0}".format(template.Template.ProjectExtension))

        strTemplateNuGetPackageConfigPath = IOUtil.Join(strTemplatePath, "Template_packages_config")

        self.SolutionExtension = "sln"
        self.ProjectExtension = template.Template.ProjectExtension

        self.TemplateFileRecordManager = TemplateFileRecordManager(strTemplatePath)

        nuGetPackageConfig = IOUtil.TryReadFile(IOUtil.Join(strTemplateNuGetPackageConfigPath, "Master.txt"))
        self.NuGetPackageConfig = NuGetPackageConfigSnippets(config, strTemplateNuGetPackageConfigPath, nuGetPackageConfig) if nuGetPackageConfig is not None else None

        self.TemplateSLN = IOUtil.ReadFile(IOUtil.Join(strTemplateSolutionPath, "Master.txt"))
        self.SLNAddProject = IOUtil.ReadFile(IOUtil.Join(strTemplateSolutionPath, "AddProject.txt"))
        self.SLNSnippet1 = IOUtil.ReadFile(IOUtil.Join(strTemplateSolutionPath, "Snippet1.txt"))
        self.SLNSnippet2 = IOUtil.ReadFile(IOUtil.Join(strTemplateSolutionPath, "Snippet2.txt"))
        self.Master = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "Master.txt"))
        self.VariantProjectConfiguration = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantProjectConfiguration.txt"))
        self.ProjectReferences = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "ProjectReferences.txt"))
        self.ProjectReferences_1 = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "ProjectReferences_1.txt"))
        self.PackageReferences = IOUtil.TryReadFile(IOUtil.Join(strTemplateProjectPath, "PackageReferences.txt"))
        self.PackageReferences_1 = IOUtil.TryReadFile(IOUtil.Join(strTemplateProjectPath, "PackageReferences_1.txt"))
        self.AssemblyReferenceSimple = IOUtil.TryReadFile(IOUtil.Join(strTemplateProjectPath, "AssemblyReferenceSimple.txt"))
        self.AssemblyReferenceComplex = IOUtil.TryReadFile(IOUtil.Join(strTemplateProjectPath, "AssemblyReferenceComplex.txt"))
        self.AssemblyReferenceComplex_Private = IOUtil.TryReadFile(IOUtil.Join(strTemplateProjectPath, "AssemblyReferenceComplex_Private.txt"))
        self.AddHeaderFile = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "AddHeaderFile.txt"))
        self.AddSourceFile = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "AddSourceFile.txt"))
        self.VariantConfiguration = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantConfiguration.txt"))
        self.VariantPropertySheets = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantPropertySheets.txt"))
        self.VariantPropertyGroups = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantPropertyGroups.txt"))
        self.VariantCompilerSettings = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantCompilerSettings.txt"))
        self.VariantCompilerSettings_1 = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantCompilerSettings_1.txt"))
        self.VariantCompilerSettings_2 = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "VariantCompilerSettings_2.txt"))
        externalFileToOutput = IOUtil.TryReadFile(IOUtil.Join(strTemplateProjectPath, "ExternalFileToOutput.txt"))
        self.ExternalFileToOutput = "" if externalFileToOutput is None else externalFileToOutput
        self.Snippet9 = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "CustomBuildFiles.txt"))
        self.Snippet9_1 = IOUtil.ReadFile(IOUtil.Join(strTemplateProjectPath, "CustomBuildFiles_1.txt"))

        self.DebugOptimizations = {}  # type: Dict[int, TemplateOptimizationSetting]
        self.__LoadOptimization(self.DebugOptimizations, OptimizationType.Disabled, strVSPath, "DEBUG", "disabled")
        self.__LoadOptimization(self.DebugOptimizations, OptimizationType.Default, strVSPath, "DEBUG", "disabled")
        self.__LoadOptimization(self.DebugOptimizations, OptimizationType.Full, strVSPath, "DEBUG", "full")

        snippet3FileName = IOUtil.Join(strTemplateSolutionPath, "Snippet3.txt")
        if IOUtil.IsFile(snippet3FileName):
            self.SLNSnippet3 = IOUtil.ReadFile(snippet3FileName)
            self.SLNSnippet4 = IOUtil.ReadFile(IOUtil.Join(strTemplateSolutionPath, "Snippet4.txt"))
            self.SLNSnippet4_1 = IOUtil.ReadFile(IOUtil.Join(strTemplateSolutionPath, "Snippet4_1.txt"))
        else:
            self.SLNSnippet3 = ""
            self.SLNSnippet4 = ""
            self.SLNSnippet4_1 = ""


    def __LoadOptimization(self, rDict: Dict[int, TemplateOptimizationSetting],
                           optimizationType: int, strVSPath: str, prefix: str, postfix: str) -> None:
        snippetOptimizationType = self.SafeReadFile(IOUtil.Join(strVSPath, "{0}_OPTIMIZATION_TYPE_{1}.txt".format(prefix, postfix)), "")
        snippetOptimizationOptions = self.SafeReadFile2(IOUtil.Join(strVSPath, "{0}_OPTIMIZATION_OPTIONS_{1}.txt".format(prefix, postfix)), "")
        rDict[optimizationType] = TemplateOptimizationSetting(optimizationType, snippetOptimizationType, snippetOptimizationOptions)


    def SafeReadFile(self, filename: str, defaultContent: str) -> str:
        content = IOUtil.TryReadFile(filename)
        return content if content is not None else defaultContent


    def SafeReadFile2(self, filename: str, defaultContent: str) -> str:
        content = self.SafeReadFile(filename, defaultContent)
        if len(content) > 0:
            content = '\n' + content
        return content


class CodeTemplateProjectBatFiles(object):
    def __init__(self, config: Config) -> None:
        super(CodeTemplateProjectBatFiles, self).__init__()
        self.TemplateStartBat = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_WinStartProject.txt"))
        self.TemplateBuildBat = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_WinBuildProject.txt"))
        self.TemplateRunBat = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_WinRunProject.txt"))
        templateSnippetErrorCheck = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "TemplateSnippet_WinBatErrorCheck.txt"))
        self.TemplateSnippetErrorCheck = "" if templateSnippetErrorCheck is None else templateSnippetErrorCheck


class GeneratorVCTemplate(object):
    def __init__(self, config: Config, platformName: str, vsVersion: int, languageTemplates: VSVersionLanguageTemplates) -> None:
        super(GeneratorVCTemplate, self).__init__()
        self.__TemplateExecutablePrefix = "Executable"
        self.__TemplateLibraryPrefix = "Library"

        packageLanguage = config.ToolConfig.DefaultPackageLanguage
        projectConfig = config.ToolConfig.ProjectRootConfig
        activeTemplate = projectConfig.DefaultTemplate

        self.UsingLinuxTools = (platformName != PackageConfig.PlatformNameString.WINDOWS) and activeTemplate == MagicStrings.VSDefaultCPPTemplate
        if self.UsingLinuxTools:
            activeTemplate = MagicStrings.VSDefaultCPPLinuxTemplate

        template = languageTemplates.TryGet(activeTemplate)
        if template is None:
            raise UnsupportedException("No template found for '{0}' named '{1}'".format(PackageLanguage.ToString(packageLanguage), activeTemplate))

        # ScanForTemplates
        templateDirectories = IOUtil.GetDirectoriesAt(template.Path, False)
        self.__ExecutableTemplateDict = self.__BuildTemplateDict(templateDirectories, self.__TemplateExecutablePrefix, config, template, vsVersion, self.UsingLinuxTools)
        self.__LibraryTemplateDict = self.__BuildTemplateDict(templateDirectories, self.__TemplateLibraryPrefix, config, template, vsVersion, self.UsingLinuxTools)

        self.__Bat = CodeTemplateProjectBatFiles(config)
        self.__HeaderLib = self.__LoadHeaderLib(config, template, vsVersion, self.UsingLinuxTools)


    def GetLibraryTemplate(self, package: Package) -> CodeTemplateVC:
        if not package.TemplateType in self.__LibraryTemplateDict:
            raise Exception("The generator does not support the requested library template type '{0}' requested by package '{1}' supported types: {2}".format(package.TemplateType, package.Name, ", ".join(list(self.__LibraryTemplateDict.keys()))))
        return self.__LibraryTemplateDict[package.TemplateType]


    def GetExecutableTemplate(self, package: Package) -> CodeTemplateVC:
        if not package.TemplateType in self.__ExecutableTemplateDict:
            raise Exception("The generator does not support the requested executable template type '{0}' requested by package '{1}' supported types: {2}".format(package.TemplateType, package.Name, ", ".join(list(self.__ExecutableTemplateDict.keys()))))
        return self.__ExecutableTemplateDict[package.TemplateType]


    def GetBatTemplate(self) -> CodeTemplateProjectBatFiles:
        return self.__Bat


    def TryGetHeaderLibraryTemplate(self) -> Optional[CodeTemplateVC]:
        return self.__HeaderLib


    def __CheckPrefix(self, entry: str, prefix: str) -> bool:
        return entry == prefix or entry.startswith(prefix+'_')


    def __BuildTemplateDict(self, templateDirectories: List[str], templatePrefix: str,
                            config: Config, template: XmlNewVSProjectTemplateFile,
                            vsVersion: int, usingLinuxTools: bool) -> Dict[str, CodeTemplateVC]:
        templateDict = {}  # type: Dict[str, CodeTemplateVC]
        for entry in templateDirectories:
            if self.__CheckPrefix(entry, templatePrefix):
                skip = len(templatePrefix)+1 if len(templatePrefix) != len(entry) else len(templatePrefix)
                subType = entry[skip:]
                templateDict[subType] = CodeTemplateVC(config, template, entry, vsVersion, usingLinuxTools)
        return templateDict


    def __LoadHeaderLib(self, config: Config, template: XmlNewVSProjectTemplateFile, vsVersion: int, usingLinuxTools: bool) -> Optional[CodeTemplateVC]:
        # FIX: this might not work correctly after the recent template changes
        #      but the headerlib is only used by the experimental visual studio linux tools support
        #      so its not critical to fix it now
        name = "HeaderLib"
        path = IOUtil.Join(template.Path, "{0}.sln".format(name))
        if not IOUtil.IsFile(path):
            return None
        return CodeTemplateVC(config, template, name, vsVersion, usingLinuxTools)
