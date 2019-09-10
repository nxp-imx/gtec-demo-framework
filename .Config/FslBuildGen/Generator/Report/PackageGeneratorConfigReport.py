#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2019 NXP
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
from typing import Set
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorConfigReport import GeneratorConfigReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport


class PackageGeneratorConfigReport(object):
    def __init__(self, configReport: GeneratorConfigReport, variableReport: GeneratorVariableReport,
                 generatedFileSet: Set[str], canBuildContent: bool,
                 masterBuildReport: Optional[GeneratorBuildReport],
                 masterBuildVariableReport: Optional[GeneratorVariableReport]) -> None:
        super().__init__()
        self.ConfigReport  = configReport                   # type: GeneratorConfigReport
        self.VariableReport = variableReport                # type: GeneratorVariableReport
        self.GeneratedFileSet = generatedFileSet            # type: Set[str]
        # Set to true when the generated build files will build the Content files.
        self.CanBuildContent = canBuildContent
        self.MasterBuildReport = masterBuildReport
        self.MasterBuildVariableReport = masterBuildVariableReport
        if ((masterBuildReport is None and masterBuildVariableReport is not None) or
            (masterBuildReport is not None and masterBuildVariableReport is None)):
            raise Exception("MasterBuildReport and MasterBuildVariableReport must both be valid or both be invalid")
