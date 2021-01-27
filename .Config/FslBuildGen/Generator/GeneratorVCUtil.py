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

from typing import Callable
from typing import List
from typing import Optional
from typing import Set
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
import uuid

DefaultVCID = 'F73214FE-7A4B-4D7D-89EC-416B25E643BF'


def _CreateGUID() -> str:
    return "{0}".format(uuid.uuid4()).upper()


def CreateGUID(usedGUIds: Set[str], maxRetries: int = 1000000) -> str:
    count = 0
    newGuid = _CreateGUID()
    while newGuid in usedGUIds:
        if count > maxRetries:
            raise Exception("Failed to generate new GUID")
        newGuid = _CreateGUID()
        count = count + 1
    return newGuid

def CreateGUID2(fnExist: Callable[[str], bool], maxRetries: int = 1000000) -> str:
    count = 0
    newGuid = _CreateGUID()
    while fnExist(newGuid):
        if count > maxRetries:
            raise Exception("Failed to generate new GUID")
        newGuid = _CreateGUID()
        count = count + 1
    return newGuid

def TryGenerateGUID(log: Log, packages: List[Package], guid: Optional[str] = None, maxRetries: int = 1000000) -> str:
    """
    If this returns DefaultVCID then it failed to
    """
    guid = guid if guid is not None else DefaultVCID
    if guid != DefaultVCID:
        return guid

    used = set()
    for package in packages:
        if package.CustomInfo.VisualStudioProjectGUID is not None:
            used.add(package.CustomInfo.VisualStudioProjectGUID)

    count = 0
    newGuid = _CreateGUID()
    while newGuid in used:
        if count > maxRetries:
            log.LogPrint("Failed to generate unique GUID in allocated time using default, please add a unique GUID manually")
            return guid
        newGuid = _CreateGUID()
        count = count + 1
    return newGuid


def GenerateGUID(log: Log, packages: List[Package], guid: Optional[str] = None, maxRetries: int = 1000000) -> str:
    newGuid = TryGenerateGUID(log, packages, guid, maxRetries)
    if newGuid == DefaultVCID:
        raise Exception("Failed to generate new GUID")
    return newGuid
