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

from FslBuildGen import Util
from FslBuildGen.Packages.PackageInstanceName import PackageInstanceName
from FslBuildGen.Packages.PackageNamespaceName import PackageNamespaceName
from FslBuildGen.Packages.PackageShortName import PackageShortName

class PackageNameInfo(object):
    def __init__(self, fullName: PackageInstanceName) -> None:
        super().__init__()

        self.FullName = fullName

        shortName, namespace = Util.GetPackageNames(fullName.Value)
        sourceName, flavorName = Util.GetPackageSourceAndFlavorNames(fullName.Value)

        self.SourceName = sourceName
        self.FlavorName = flavorName
        self.ShortName = PackageShortName(shortName)
        self.Namespace = PackageNamespaceName(namespace)
        self.PrintName = fullName.Value

    def PatchPrintName(self, name: str) -> None:
        self.PrintName = name

    def __str__(self) -> str:
        return "FullName:{0} ShortName:{1} Namespace:{2} sourceName:{3} flavorName{4}".format(self.FullName, self.ShortName, self.Namespace, self.SourceName, self.FlavorName)

    def __repr__(self) -> str:
        return "FullName:{0} ShortName:{1} Namespace:{2} sourceName:{3} flavorName{4}".format(self.FullName, self.ShortName, self.Namespace, self.SourceName, self.FlavorName)
