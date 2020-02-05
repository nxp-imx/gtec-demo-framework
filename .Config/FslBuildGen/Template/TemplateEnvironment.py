#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2017 NXP
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

from typing import Dict
from typing import Optional
from FslBuildGen.Config import Config
from FslBuildGen.Packages.Package import Package

class TemplateEnvironment(object):
    def __init__(self, config: Config, platformName: str) -> None:
        super().__init__()
        self.Dict = {}  # type: Dict[str, Optional[str]]
        self.Dict["##PLATFORM_NAME##"] = platformName
        self.Dict["##CURRENT_YEAR##"] = config.CurrentYearString


    def Set(self, key: str, value: str) -> None:
        self.Dict[key] = value


    def SetPackage(self, package: Package, androidProjectPath: Optional[str]) -> None:
        if package.PackageLocation is None or package.ShortName is None or package.ContentPath is None or package.AbsolutePath is None:
            raise Exception("Invalid package")

        location = package.PackageLocation.Name  # type: str
        targetName = package.Name  # type: str
        platformProjectId = None # type: Optional[str]
        if package.ResolvedPlatform is not None:
            platformProjectId = package.ResolvedPlatform.ProjectId
        creationYear = package.CreationYear  # type: Optional[str]
        companyName = package.CompanyName  # type: str

        packageContentPath = package.ContentPath.AbsoluteDirPath
        #androidProjectDir = buildCMakeFile.replace("##PACKAGE_ANDROID_PROJECT_PATH##", androidProjectDir)
        self.SetPackageValues(location, package.Name, package.ShortName, package.AbsolutePath, targetName, packageContentPath, platformProjectId, 
                              creationYear, companyName, androidProjectPath)


    def SetPackageValues(self, packageLocation: str, packageName: str, packageShortName: str, packagePath: str, 
                         packageTargetName: str, packageContentPath: Optional[str],
                         platformProjectId: Optional[str], creationYear: Optional[str], companyName: str,
                         androidProjectPath: Optional[str] = None) -> None:
        platformProjectId = platformProjectId if platformProjectId != None else "ERROR_PLATFORM_PROJECT_ID_NOT_DEFINED"
        self.Dict["##PACKAGE_LOCATION##"] = packageLocation
        self.Dict["##PACKAGE_NAME##"] = packageName
        self.Dict["##DIRS_PACKAGE_NAME##"] = packageName.replace('.','/')
        self.Dict["##PACKAGE_SHORT_NAME##"] = packageShortName
        self.Dict["##PACKAGE_TARGET_NAME##"] = packageTargetName
        self.Dict["##PACKAGE_PATH##"] = packagePath
        if packageContentPath is not None:
            self.Dict["##PACKAGE_CONTENT_PATH##"] = packageContentPath
        self.Dict["##PACKAGE_VISUAL_STUDIO_PROJECT_ID##"] = platformProjectId
        self.Dict["##PACKAGE_CREATION_YEAR##"] = creationYear if creationYear != None else self.Dict["##CURRENT_YEAR##"]
        self.Dict["##PACKAGE_COMPANY_NAME##"] = companyName
        self.Dict["##{$(PACKAGE_NAME).upper().replace('.','_')}##"] = packageName.upper().replace('.', '_')
        if androidProjectPath is not None:
            self.Dict["##PACKAGE_ANDROID_PROJECT_PATH##"] = androidProjectPath
        # deprecated
        self.Dict["##PACKAGE_PLATFORM_PROJECT_ID##"] = platformProjectId
