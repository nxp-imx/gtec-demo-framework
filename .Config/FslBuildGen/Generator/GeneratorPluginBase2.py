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
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorConfig import GeneratorConfig
from FslBuildGen.Generator.GeneratorPluginBase import GeneratorPluginBase
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package


class GeneratorVariant(object):
    def __init__(self, name: str, options: List[str], description: str, variantType: int) -> None:
        super(GeneratorVariant, self).__init__()
        self.Name = name
        self.Options = list(options)
        self.Description = description.replace("##OPTIONS##", ",".join(options))
        self.Type = variantType


class GeneratorPluginBase2(GeneratorPluginBase):
    def __init__(self, name: str) -> None:
        super(GeneratorPluginBase2, self).__init__(name)
        self.GeneratorVariants = {}  # type: Dict[str, GeneratorVariant]
        self.LastActiveGenerator = None  # type: Optional[GeneratorBase]
        self.OriginalName = self.Name


    def GetVariants(self) -> List[GeneratorVariant]:
        """ Get information about the variants introduced by this generator """
        return list(self.GeneratorVariants.values())


    def GenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> Dict[Package, PackageGeneratorReport]:
        """ Run through the packages and generate a generator specific report """
        return self._DoGenerateReport(log, generatorConfig, packageList)


    def _DoGenerateReport(self, log: Log, generatorConfig: GeneratorConfig, packageList: List[Package]) -> Dict[Package, PackageGeneratorReport]:
        log.LogPrintWarning("Generator {0} does not support build reports".format(self.Name))
        return {}

