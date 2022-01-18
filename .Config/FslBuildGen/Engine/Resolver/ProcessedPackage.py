#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2020 NXP
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
from typing import List
from typing import Optional
#from FslBuildGen import Util
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Engine.Resolver.ProcessedPackageDependency import ProcessedPackageDependency
from FslBuildGen.PackageFile import PackageFile
from FslBuildGen.PackagePath import PackagePath
from FslBuildGen.Packages.CompanyName import CompanyName
from FslBuildGen.Packages.PackageCustomInfo import PackageCustomInfo
from FslBuildGen.Packages.PackageNameInfo import PackageNameInfo
from FslBuildGen.Packages.PackagePlatform import PackagePlatform
from FslBuildGen.Packages.PackageProjectContext import PackageProjectContext
#from FslBuildGen.Packages.PackageShortName import PackageShortName
from FslBuildGen.Packages.PackageTraceContext import PackageTraceContext
from FslBuildGen.Packages.Unresolved.UnresolvedExternalDependency import UnresolvedExternalDependency
from FslBuildGen.Packages.Unresolved.UnresolvedPackageDefine import UnresolvedPackageDefine
from FslBuildGen.Packages.Unresolved.UnresolvedPackageGenerate import UnresolvedPackageGenerate
from FslBuildGen.Packages.Unresolved.UnresolvedPackageRequirement import UnresolvedPackageRequirement
from FslBuildGen.Xml.XmlExperimentalRecipe import XmlExperimentalRecipe
from FslBuildGen.Xml.XmlStuff import XmlGenFileBuildCustomization


class ProcessedPackageFlags(object):
    def __init__(self, isVirtual: bool, allowCheck: bool, unitTest: bool, showInMainReadme: bool,
                 packageNameBasedIncludePath: bool, platformDefaultSupportedValue: bool, enableExtendedSourceExtensions: bool) -> None:
        super().__init__()
        self.IsVirtual = isVirtual
        self.AllowCheck = allowCheck
        self.UnitTest = unitTest
        self.ShowInMainReadme = showInMainReadme
        self.PackageNameBasedIncludePath = packageNameBasedIncludePath
        self.PlatformDefaultSupportedValue = platformDefaultSupportedValue
        self.EnableExtendedSourceExtensions = enableExtendedSourceExtensions


class ProcessedPackagePaths(object):
    def __init__(self, includePath: Optional[PackagePath], sourcePath: Optional[PackagePath], contentPath: Optional[PackagePath],
                 contentSourcePath: Optional[PackagePath], baseIncludePath: str, baseSourcePath: str) -> None:
        super().__init__()
        self.IncludePath = includePath
        self.SourcePath = sourcePath
        self.ContentPath = contentPath
        self.ContentSourcePath = contentSourcePath
        self.BaseIncludePath = baseIncludePath
        self.BaseSourcePath = baseSourcePath


class ProcessedPackage(object):
    def __init__(self, projectContext: PackageProjectContext, nameInfo: PackageNameInfo, companyName: CompanyName, creationYear: Optional[str],
                 packageFile: Optional[PackageFile], sourceFileHash: str, packageType: PackageType, packageFlags: ProcessedPackageFlags,
                 packageLanguage: PackageLanguage, generateList: List[UnresolvedPackageGenerate], directDependencies: List[ProcessedPackageDependency],
                 directRequirements: List[UnresolvedPackageRequirement], directDefines: List[UnresolvedPackageDefine],
                 externalDependencies: List[UnresolvedExternalDependency], path: ProcessedPackagePaths, templateType: str,
                 buildCustomization: Dict[str, XmlGenFileBuildCustomization], directExperimentalRecipe: Optional[XmlExperimentalRecipe],
                 resolvedPlatform: PackagePlatform, directPlatformSupported: bool, customInfo: PackageCustomInfo,
                 traceContext: PackageTraceContext) -> None:
        super().__init__()
        self.ProjectContext = projectContext
        self.NameInfo = nameInfo

        self.CompanyName = companyName
        self.CreationYear = creationYear

        self.PackageFile = packageFile
        self.PackageLocation = packageFile.PackageRootLocation if packageFile is not None else None
        self.SourceFilename = packageFile.AbsoluteFilePath if packageFile is not None else None
        self.SourceFileHash = sourceFileHash
        self.Type = packageType
        self.Flags = packageFlags
        self.PackageLanguage = packageLanguage

        self.GenerateList = generateList

        self.DirectDependencies = directDependencies
        self.DirectRequirements = directRequirements
        self.DirectDefines = directDefines
        self.ExternalDependencies = externalDependencies

        self.Path = path
        self.TemplateType = templateType
        self.BuildCustomization = buildCustomization
        self.DirectExperimentalRecipe = directExperimentalRecipe

        self.ResolvedPlatform = resolvedPlatform
        # This setting only indicates what this package tries to set the flag to (it is not the final dependency based resolved result)
        self.DirectPlatformSupported = directPlatformSupported

        self.CustomInfo = customInfo
        self.TraceContext = traceContext
