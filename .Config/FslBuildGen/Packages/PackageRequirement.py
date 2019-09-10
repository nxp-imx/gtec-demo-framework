#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright (c) 2014 Freescale Semiconductor, Inc.
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

from typing import Set
from FslBuildGen.Config import Config
from FslBuildGen.Packages.PackageElement import PackageElement
from FslBuildGen.Xml.XmlGenFileRequirement import XmlGenFileRequirement


class PackageRequirement(PackageElement):
    def __init__(self, base: XmlGenFileRequirement, introducedByPackageName: str) -> None:
        super().__init__(base.Name, base.XMLElement)
        self.Id = self.Name.lower()
        # type#extends
        self.GroupId = "{0}#{1}".format(base.Type.lower(), base.Extends.lower())
        # type#extends#id
        self.FullId = "{0}#{1}".format(self.GroupId, self.Id)
        self.Extends = base.Extends
        self.Type = base.Type
        self.Version = base.Version
        self.IntroducedByPackageName = introducedByPackageName
        #self.Access = base.Access
        self.IsFirstActualUse = False
        self.IntroducedByPackages = set()  # type: Set[str]
        self.IntroducedByPackages.add(introducedByPackageName)
        #self.ConsumedBy = base.ConsumedBy
        # the access to the package this was received from
        #self.FromPackageAccess = fromPackageAccess
