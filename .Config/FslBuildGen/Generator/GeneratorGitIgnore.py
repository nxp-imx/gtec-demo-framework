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
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Packages.Package import Package

class GeneratorGitIgnore(GeneratorBase):
    def __init__(self, config: Config, packages: List[Package], platformName: str, activeGenerator: GeneratorBase) -> None:
        super(GeneratorGitIgnore, self).__init__()

        headerLibTemplate = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_gitignore_headerlib.txt"))
        libTemplate = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_gitignore_lib.txt"))
        exeTemplate = IOUtil.TryReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Template_gitignore_exe.txt"))

        generatorIgnoreDict = activeGenerator.GetPackageGitIgnoreDict()

        for package in packages:
            if package.Type == PackageType.Library:
                self.__GenerateLibraryBuildFile(config, package, platformName, libTemplate, generatorIgnoreDict)
            elif package.Type == PackageType.Executable:
                self.__GenerateLibraryBuildFile(config, package, platformName, exeTemplate, generatorIgnoreDict)
            elif package.Type == PackageType.HeaderLibrary:
                self.__GenerateLibraryBuildFile(config, package, platformName, headerLibTemplate, generatorIgnoreDict)


    def __GenerateLibraryBuildFile(self, config: Config, package: Package,
                                   platformName: str, template: Optional[str],
                                   generatorIgnoreDict: Dict[str, Set[str]]) -> None:
        if template is None or package.AbsolutePath is None:
            return
        template = template.replace("##PROJECT_NAME##", package.Name)
        targetFilePath = IOUtil.Join(package.AbsolutePath, ".gitignore")


        targetContent = IOUtil.TryReadFile(targetFilePath)

        targetArray = self.__ToArray(targetContent)
        templateArray = self.__ToArray(template)

        templateArray = [('/' + entry if not entry.startswith('/') else entry) for entry in templateArray if len(entry) > 0]


        targetArray = [_f for _f in targetArray if _f]
        templateArray = [_f for _f in templateArray if _f]

        # Add the missing dependencies
        for entry in templateArray:
            if not entry in targetArray:
                targetArray.append(entry)

        # Allow each generator to add things that should be ignored
        if package.Name in generatorIgnoreDict:
            ignoreList = generatorIgnoreDict[package.Name]
            for entry in ignoreList:
                # Remove the old legacy entries
                if entry in targetArray:
                    targetArray.remove(entry)
                entry = '/' + entry if not entry.startswith('/') and len(entry) > 0 else entry
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

        targetArraySet = set()  # type: Set[str]

        # ensure no duplicates exist
        finalTargetArray = []
        for entry in targetArray:
            if len(entry) > 0 and entry != '/' and entry not in targetArraySet:
                targetArraySet.add(entry)
                finalTargetArray.append(entry)

        finalTargetArray.sort()

        finalContent = "\n".join(finalTargetArray) + '\n'

        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(targetFilePath, finalContent)


    def __ToArray(self, content: Optional[str]) -> List[str]:
        if content is None:
            return []
        content = content.replace('\r', '')
        return content.split('\n')
