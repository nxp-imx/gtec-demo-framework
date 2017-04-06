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

import os
import os.path
from FslBuildGen import IOUtil, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from subprocess import call

class GeneratorGitIgnore(object):
    def __init__(self, config, packages, platformName, activeGenerator):
        super(GeneratorGitIgnore, self).__init__()

        libTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_gitignore_lib.txt"))
        exeTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_gitignore_exe.txt"))

        generatorIgnoreDict = activeGenerator.GetPackageGitIgnoreDict()

        for package in packages:
            if package.Type == PackageType.Library:
                self.__GenerateLibraryBuildFile(config, package, platformName, libTemplate, generatorIgnoreDict)
            elif package.Type == PackageType.Executable:
                self.__GenerateLibraryBuildFile(config, package, platformName, exeTemplate, generatorIgnoreDict)

    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}

    def __GenerateLibraryBuildFile(self, config, package, platformName, template, generatorIgnoreDict):
        template = template.replace("##PROJECT_NAME##", package.Name)
        targetFilePath = IOUtil.Join(package.AbsolutePath, ".gitignore")
        targetContent = IOUtil.TryReadBinaryFile(targetFilePath)
        fileLineBreakStyle = self.__DetectLineBreakStyle(targetContent)

        targetArray = self.__ToArray(targetContent)
        templateArray = self.__ToArray(template)

        targetArray = filter(None, targetArray)
        templateArray = filter(None, templateArray)

        # Add the missing dependencies
        for entry in templateArray:
            if not entry in targetArray:
                targetArray.append(entry)

        # Allow each generator to add things that should be ignored
        if package.Name in generatorIgnoreDict:
            ignoreList = generatorIgnoreDict[package.Name]
            for entry in ignoreList:
                if not entry in targetArray:
                    targetArray.append(entry)

        # Remove stuff
        #remove = []
        #legacyName = package.ShortName + '.'
        #for entry in targetArray:
        #    if entry.startswith(legacyName) or entry == package.ShortName:
        #        remove.append(entry)
        #for entry in remove:
        #    targetArray.remove(entry)

        #if 'GNUmakefile_yocto' in targetArray:
        #    targetArray.remove('GNUmakefile_yocto')

        # sort the content to ensure that there are minimal changes
        targetArray.sort()

        finalContent = fileLineBreakStyle.join(targetArray) + fileLineBreakStyle

        if not config.DisableWrite:
            IOUtil.WriteBinaryFileIfChanged(targetFilePath, finalContent)


    def __DetectLineBreakStyle(self, content):
        if content == None:
            return os.linesep
        elif '\r\n' in content:
            return '\r\n'
        else:
            return '\n'

    def __ToArray(self, content):
        if content == None:
            return []
        content = content.replace('\r', '')
        return content.split('\n')
