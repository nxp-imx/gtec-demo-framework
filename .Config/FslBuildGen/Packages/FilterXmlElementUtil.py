#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from typing import List
from FslBuildGen.Log import Log
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.Packages.ElementIfConditionUtil import ElementIfConditionUtil
from FslBuildGen.Xml.XmlGenFileDependency import XmlGenFileDependency
from FslBuildGen.Xml.XmlGenFileExternalDependency import XmlGenFileExternalDependency

class FilterXmlElementUtil(object):
    @staticmethod
    def FilterOnConditionsExternalDependency(log: Log, generatorInfo: GeneratorInfo, sourceList: List[XmlGenFileExternalDependency]) -> List[XmlGenFileExternalDependency]:
        result = [] # type: List[XmlGenFileExternalDependency]
        for element in sourceList:
            if element.IfCondition is None or ElementIfConditionUtil.CheckCondition(element.IfCondition, generatorInfo):
                result.append(element)
            else:
                log.LogPrintVerbose(2, "Skipped ExternalDependency name '{0}' because of condition '{1}'".format(element.Name, element.IfCondition))
        return result;

    @staticmethod
    def FilterOnConditionsDependency(log: Log, generatorInfo: GeneratorInfo, sourceList: List[XmlGenFileDependency]) -> List[XmlGenFileDependency]:
        result = [] # type: List[XmlGenFileDependency]
        for element in sourceList:
            if element.IfCondition is None or ElementIfConditionUtil.CheckCondition(element.IfCondition, generatorInfo):
                result.append(element)
            else:
                log.LogPrintVerbose(2, "Skipped Dependency name '{0}' because of condition '{1}'".format(element.Name, element.IfCondition))
        return result;

