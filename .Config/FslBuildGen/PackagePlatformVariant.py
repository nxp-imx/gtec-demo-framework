#!/usr/bin/env python

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

import copy
from FslBuildGen.DataTypes import *
from FslBuildGen.DependencyGraph import DependencyGraph
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import XmlGenFile

def IndexOf(entries, entryName):
    for i in range(len(entries)):
        if entries[i].Name == entryName:
            return i
    return -1

class PackageElement(object):
    def __init__(self, base):
        self.Name = base.Name
        self.XMLElement = base.XMLElement


class PackageCPPDefine(PackageElement):
    def __init__(self, base, introducedByPackageName, fromPackageAccess):
        super(PackageCPPDefine, self).__init__(base)
        self.IntroducedByPackageName = introducedByPackageName
        self.Access = base.Access
        self.IsFirstActualUse = False
        self.ConsumedBy = base.ConsumedBy
        # the access to the package this was received from
        self.FromPackageAccess = fromPackageAccess


class PackageFeature(PackageElement):
    def __init__(self, base, introducedByPackageName):
        super(PackageFeature, self).__init__(base)
        self.Id = self.Name.lower()
        self.IntroducedByPackageName = introducedByPackageName
        #self.Access = base.Access
        self.IsFirstActualUse = False
        self.IntroducedByPackages = set()
        self.IntroducedByPackages.add(introducedByPackageName)
        #self.ConsumedBy = base.ConsumedBy
        # the access to the package this was received from
        #self.FromPackageAccess = fromPackageAccess


class PackagePlatformExternalDependency(PackageElement):
    def __init__(self, base, allowPrivate):
        super(PackagePlatformExternalDependency, self).__init__(base)
        self.DebugName = base.DebugName
        self.Include = base.Include if base.Access != AccessType.Private or allowPrivate else None
        self.Location = base.Location
        self.Access = base.Access
        self.Type = base.Type
        self.IsFirstActualUse = False


class PackagePlatformVariantOption(PackageElement):
    def __init__(self, base, allowPrivate):
        super(PackagePlatformVariantOption, self).__init__(base)
        self.IntroducedByPackageName = base.IntroducedByPackageName
        self.ExtensionInfo = base.IntroducedByPackageName
        self.ExternalDependencies = self.__CloneExtDeps(base.ExternalDependencies, allowPrivate)
        if not allowPrivate:
            self.DirectCPPDefines = [entry for entry in base.DirectCPPDefines if entry.Access != AccessType.Private]
        else:
            self.DirectCPPDefines = base.DirectCPPDefines

    def __CloneExtDeps(self, externalDependencies, allowPrivate):
        res = []
        for entry in externalDependencies:
            res.append(PackagePlatformExternalDependency(entry, allowPrivate))
        return res


    def Extend(self, srcOption, extendingPackageName):
        extendedOption = PackagePlatformVariantOption(self, False)
        extendedOption.ExtensionInfo = "%s<-%s" % (self.IntroducedByPackageName, extendingPackageName)

        dst = extendedOption.ExternalDependencies
        src = srcOption.ExternalDependencies

        for entry in src:
            index = IndexOf(dst, entry.Name)
            if index >= 0:
                raise VariantExtensionCanNotOverwriteExistingExternalDependencyException(dst[index], self.IntroducedByPackageName, entry, self.ExtensionInfo)
            else:
                dst.append(entry)

        dst = extendedOption.DirectCPPDefines
        src = srcOption.DirectCPPDefines
        for entry in src:
            if entry.Access == AccessType.Public or (entry.Access == AccessType.Private and src.IntroducedByPackageName == extendingPackageName):
                index = IndexOf(dst, entry.Name)
                if index >= 0:
                    raise VariantExtensionCanNotOverwriteExistingCPPDefineException(dst[index], self.IntroducedByPackageName, entry, self.ExtensionInfo)
                else:
                    dst.append(entry)

        return extendedOption


class PackagePlatformVariant(PackageElement):
    def __init__(self, base, allowPrivate):
        super(PackagePlatformVariant, self).__init__(base)
        self.IntroducedByPackageName = base.IntroducedByPackageName
        self.ExtensionInfo = base.IntroducedByPackageName
        self.Options = self.__ProcessOptions(base.Options, allowPrivate)
        self.Type = base.Type
        self.__BuildOptionDict(self.Options)
        self.AllowExtend = base.AllowExtend
        self.PurifiedName = self.__PurifyName(base.Type, self.Name)

    def __PurifyName(self, type, variantName):
        if type == VariantType.Virtual:
            variantName=Util.RemoveEnvironmentVariablePadding(variantName)
        return variantName


    def __ProcessOptions(self, options, allowPrivate):
        res = []
        for entry in options:
            res.append(PackagePlatformVariantOption(entry, allowPrivate))
        return res

    def __BuildOptionDict(self, options):
        optionDict = {}
        for option in options:
            optionDict[option.Name] = option
        self.OptionDict = optionDict

    def Extend(self, variant, extendingPackageName):
        if not variant.AllowExtend:
            raise VariantNotMarkedAsExtendingException(self, variant);

        extendedVariant = PackagePlatformVariant(self, False)
        extendedVariant.ExtensionInfo = "%s<-%s" % (self.IntroducedByPackageName, extendingPackageName)

        dstOptions = extendedVariant.Options
        srcOptions = variant.Options
        for entry in srcOptions:
            index = IndexOf(dstOptions, entry.Name)
            if index >= 0:
                dstOptions[index] = dstOptions[index].Extend(entry, extendingPackageName)
            else:
                raise UsageErrorException("A extending variant can not introduce new options");
        extendedVariant.__BuildOptionDict(dstOptions)
        return extendedVariant

