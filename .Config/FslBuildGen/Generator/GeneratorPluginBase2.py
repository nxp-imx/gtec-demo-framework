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
from typing import List
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.GeneratorPluginBase import GeneratorPluginBase
from FslBuildGen.Generator.Report.PackageGeneratorConfigReport import PackageGeneratorConfigReport
from FslBuildGen.Generator.Report.PackageGeneratorBuildExecutableInfo import PackageGeneratorBuildExecutableInfo
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.Report.TheGeneratorBuildReport import TheGeneratorBuildReport
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package


class GeneratorVariant(object):
    def __init__(self, name: str, options: List[str], description: str, variantType: int) -> None:
        super().__init__()
        self.Name = name
        self.Options = list(options)
        self.Description = description.replace("##OPTIONS##", ",".join(options))
        self.Type = variantType


class GeneratorPluginBase2(GeneratorPluginBase):
    def __init__(self, platformName: str) -> None:
        super().__init__(platformName)
        self.GeneratorVariants = {}  # type: Dict[str, GeneratorVariant]
        self.LastActiveGenerator = None  # type: Optional[GeneratorBase]
        self.OriginalPlatformName = self.PlatformName


    def GetVariants(self) -> List[GeneratorVariant]:
        """ Get information about the variants introduced by this generator """
        return list(self.GeneratorVariants.values())


    def GenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> TheGeneratorBuildReport:
        """ Run through the packages and generate a generator specific report """
        return self._DoGenerateReport(log, generatorConfig, packageList)

    def TryGenerateConfigReport(self, log: Log, generatorConfig: GeneratorConfig, topLevelPackage: Package) -> Optional[PackageGeneratorConfigReport]:
        """ Run through the top-level package and generate a generator specific report """
        if topLevelPackage is None or topLevelPackage.Type != PackageType.TopLevel:
            raise Exception("Usage error package is not a top level package")
        return self._DoTryGenerateConfigReport(log, generatorConfig, topLevelPackage)

    def TryGetBuildExecutableInfo(self, log: Log, generatorConfig: GeneratorConfig,
                                  package: Package, generatorReport: PackageGeneratorReport,
                                  variantSettingsDict: Dict[str, str]) -> Optional[PackageGeneratorBuildExecutableInfo]:
        """ Get information about the executable build during 'development' (and not the final installed one) """
        return self._DoTryGetBuildExecutableInfo(log, generatorConfig, package, generatorReport, variantSettingsDict)

    def _DoGenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> TheGeneratorBuildReport:
        log.LogPrintWarning("Generator {0} does not support build reports".format(self.PlatformName))
        return TheGeneratorBuildReport({})

    def _DoTryGenerateConfigReport(self, log: Log, generatorConfig: GeneratorConfig, topLevelPackage: Package) -> Optional[PackageGeneratorConfigReport]:
        log.LogPrintWarning("Generator {0} does not support config reports".format(self.PlatformName))
        return None

    def _DoTryGetBuildExecutableInfo(self, log: Log, generatorConfig: GeneratorConfig,
                                     package: Package, generatorReport: PackageGeneratorReport,
                                     variantSettingsDict: Dict[str, str]) -> Optional[PackageGeneratorBuildExecutableInfo]:
        return None
