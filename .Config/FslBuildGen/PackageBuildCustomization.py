#!/usr/bin/env python

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

from FslBuildGen.DataTypes import *
from FslBuildGen.DependencyGraph import DependencyGraph
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import XmlGenFile

class PackageBuildCustomizationBase(object):
    def __init__(self):
        super(PackageBuildCustomizationBase, self).__init__()


    def ReadValue(self, dict, name, defaultValue):
        if dict != None and name in dict:
            return dict[name].Value
        else:
            return defaultValue


class PackageBuildCustomization_Debug(PackageBuildCustomizationBase):
    def __init__(self, dict):
        super(PackageBuildCustomization_Debug, self).__init__()
        self.Optimization = self.ReadValue(dict, 'BuildCustomization.Debug.Optimization', OptimizationType.Disabled)


class PackageBuildCustomization(PackageBuildCustomizationBase):
    def __init__(self, xmlBuildCustomizationDict):
        super(PackageBuildCustomization, self).__init__()
        self.Debug = PackageBuildCustomization_Debug(xmlBuildCustomizationDict)
