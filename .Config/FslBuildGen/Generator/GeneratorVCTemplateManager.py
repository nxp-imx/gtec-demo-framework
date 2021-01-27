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

from typing import Dict
from typing import List
from typing import Optional
from typing import Set
from FslBuildGen import IOUtil
from FslBuildGen.DataTypes import PackageLanguage
#from FslBuildGen.DataTypes import *
#from FslBuildGen.Exceptions import *
#from FslBuildGen.SharedGeneration import *
#from FslBuildGen.PackageGeneratorReport import *
from FslBuildGen.Generator.VSVersionLanguageTemplates import VSVersionLanguageTemplates
from FslBuildGen.Log import Log
from FslBuildGen.ToolConfig import ToolConfigTemplateFolder
from FslBuildGen.Xml.XmlNewVSProjectTemplateFile import XmlNewVSProjectTemplateFile


class GeneratorVCTemplateManager(object):
    def __init__(self, log: Log, pathTemplateRoot: ToolConfigTemplateFolder, vsVersion: int) -> None:
        super().__init__()
        self.TemplateFileName = "Template.xml"
        # Language to template lookup
        # key =  the packageLanguage
        # value = VSVersionLanguageTemplates
        self.LanguageToTemplatesDict = self.__LoadTemplates(log, pathTemplateRoot.ResolvedPath, vsVersion)  # type: Dict[PackageLanguage, VSVersionLanguageTemplates]


    def GetLanguageTemplates(self, packageLanguage: PackageLanguage) -> VSVersionLanguageTemplates:
        return self.LanguageToTemplatesDict[packageLanguage]


    def TryGetLanguageTemplates(self, packageLanguage: PackageLanguage) -> Optional[VSVersionLanguageTemplates]:
        return self.LanguageToTemplatesDict[packageLanguage] if packageLanguage in self.LanguageToTemplatesDict else None


    def __LoadTemplates(self, log: Log, path: str, vsVersion: int) -> Dict[PackageLanguage, VSVersionLanguageTemplates]:
        pathVS = IOUtil.Join(path, "VS{0}".format(vsVersion))
        vsDirs = self.__ScanDir(pathVS)

        templateIds = set()  # type: Set[str]
        languageToTemplatesDict = {}  # type: Dict[PackageLanguage, VSVersionLanguageTemplates]
        for entry in vsDirs:
            templateList = self.__ScanForTemplates(log, entry)
            for template in templateList:
                if template != None:
                    if template.Id in templateIds:
                        raise Exception("Template id already defined: '{0}'".format(template.Id))

                    #languageTemplateDict = None
                    if not template.Template.PackageLanguage in languageToTemplatesDict:
                        languageTemplates = VSVersionLanguageTemplates(template.Template.PackageLanguage)
                        languageToTemplatesDict[template.Template.PackageLanguage] = languageTemplates
                    else:
                        languageTemplates = languageToTemplatesDict[template.Template.PackageLanguage]

                    templateIds.add(template.Id)
                    languageTemplates.AddTemplate(template)

        return languageToTemplatesDict


    def __ScanForTemplates(self, log: Log, path: str) -> List[XmlNewVSProjectTemplateFile]:
        files = IOUtil.GetFilePaths(path, self.TemplateFileName)
        templateList = []  # type: List[XmlNewVSProjectTemplateFile]
        for filename in files:
            template = self.__TryLoadTemplate(log, filename)
            if template is not None:
                templateList.append(template)
            else:
                log.LogPrint("Failed to load template '{0}'".format(filename))
        return templateList


    def __ScanDir(self, path: str) -> List[str]:
        return IOUtil.GetDirectoriesAt(path, True)


    def __TryLoadTemplate(self, log: Log, path: str) -> Optional[XmlNewVSProjectTemplateFile]:
        if IOUtil.IsFile(path):
            return XmlNewVSProjectTemplateFile(log, path)
        return None
