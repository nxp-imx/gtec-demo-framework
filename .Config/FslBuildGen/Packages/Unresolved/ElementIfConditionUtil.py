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

#from typing import Dict
#from typing import Optional
from FslBuildGen.DataTypes import DependencyCondition
from FslBuildGen.Generator.GeneratorInfo import GeneratorInfo
from FslBuildGen.BuildContent.ConditionInterpreter import EvaluateConditionInterpreter

class MagicCommands(object):
    CommandEvaluate = 'Evaluate'

class Magic(object):
    CommandEvaluatePre = MagicCommands.CommandEvaluate + '('
    CommandEvaluatePost = ')'


class ElementIfConditionUtil(object):

    @staticmethod
    def CheckCondition(condition: str, generatorInfo: GeneratorInfo, debugHelp: str) -> bool:
        if condition == DependencyCondition.FindPackageAllowed:
            return generatorInfo.AllowFindPackage
        elif condition == DependencyCondition.FindPackageNotAllowed:
            return not generatorInfo.AllowFindPackage
        elif (generatorInfo.ValidVariabelDict is not None and condition.startswith(Magic.CommandEvaluatePre) and condition.endswith(Magic.CommandEvaluatePost) and len(condition) > (len(Magic.CommandEvaluatePre)+len(Magic.CommandEvaluatePost))):
            evalCondition = condition[len(Magic.CommandEvaluatePre):-len(Magic.CommandEvaluatePost)]
            #raise Exception("Evaluate '{0}' not implemented".format(condition))
            return EvaluateConditionInterpreter.Evaluate(evalCondition, generatorInfo.ValidVariabelDict, debugHelp)

        raise Exception("Unsupported condition '{0}' expected one of these {1}".format(condition, [DependencyCondition.FindPackageAllowed, DependencyCondition.FindPackageNotAllowed]))

