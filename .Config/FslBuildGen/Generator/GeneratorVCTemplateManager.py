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
from FslBuildGen.XmlNewVSProjectTemplateFile import XmlNewVSProjectTemplateFile

class VSVersionLanguageTemplates(object):
    def __init__(self, packageLanguage):
        super(VSVersionLanguageTemplates, self).__init__()
        self.PackageLanguage = packageLanguage
        self.TemplateDict = {}

    def TryGet(self, templateName):
        templateId = templateName.lower()
        return self.TemplateDict[templateId] if templateId in self.TemplateDict else None
        

class GeneratorVCTemplateManager(object):
    def __init__(self, pathTemplateRoot, vsVersion):
        super(GeneratorVCTemplateManager, self).__init__()
        self.TemplateDict = self.__LoadTemplates(pathTemplateRoot.ResolvedPath, vsVersion)


    def GetLanguageTemplates(self, packageLanguage):
        return self.TemplateDict[packageLanguage]


    def TryGetLanguageTemplates(self, packageLanguage):
        return self.TemplateDict[packageLanguage] if packageLanguage in self.TemplateDict else None


    def __LoadTemplates(self, path, vsVersion):    
        pathVS = IOUtil.Join(path, "VS{0}".format(vsVersion))
        vsDirs = self.__ScanDir(pathVS)

        templateIds = set()
        templateDict = {}
        for entry in vsDirs:
            template = self.__TryLoadTemplate(entry)
            if template != None:
                if template.Id in templateIds:                
                    raise Exception("Template id already defined: '{0}'".format(template.Id));
               
                languageTemplateDict = None
                if not template.Template.PackageLanguage in templateDict:
                    languageTemplates = VSVersionLanguageTemplates(template.Template.PackageLanguage)
                    templateDict[template.Template.PackageLanguage] = languageTemplates
                else:
                    languageTemplates = templateDict[template.Template.PackageLanguage]  

                templateIds.add(template.Id)
                languageTemplates.TemplateDict[template.Id] = template

        return templateDict


    def __ScanDir(self, path):
        return IOUtil.GetDirectoriesAt(path, True)


    def __TryLoadTemplate(self, path):
        filename = IOUtil.Join(path, "Template.xml")
        if IOUtil.IsFile(filename):
            return XmlNewVSProjectTemplateFile(filename)
        return None
