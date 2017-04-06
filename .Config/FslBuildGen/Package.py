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

from FslBuildGen.DataTypes import *
from FslBuildGen.DependencyGraph import DependencyGraph
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import XmlGenFile
from FslBuildGen.PackagePlatform import PackagePlatform
from FslBuildGen.PackageBuildCustomization import PackageBuildCustomization


class Package(object):
    def __init__(self, genFile):
        super(Package, self).__init__()
        self.GenFile = genFile
        self.XMLElement = genFile.XMLElement

        # Clone all attributes that exist on a genFile
        self.BaseIncludePath = genFile.BaseIncludePath
        self.BaseSourcePath = genFile.BaseSourcePath
        self.PackageLocation = genFile.PackageLocation
        self.Name = genFile.Name
        self.ShortName = genFile.ShortName
        self.Namespace = genFile.Namespace
        self.Type = genFile.Type
        self.IsVirtual = genFile.IsVirtual
        self.__DirectDependencies = genFile.DirectDependencies
        self.DirectCPPDefines = genFile.DirectCPPDefines
        self.Platforms = genFile.Platforms
        self.AbsolutePath = genFile.AbsolutePath
        self.AbsoluteIncludePath = genFile.AbsoluteIncludePath
        self.AbsoluteSourcePath = genFile.AbsoluteSourcePath
        self.AbsoluteContentPath = genFile.AbsoluteContentPath
        self.AbsoluteContentSourcePath = genFile.AbsoluteContentSourcePath
        self.AllowDependencyOnThis = self.__ResolveAllowDependencyOnThis(self.Type)
        self.PackageLanguage = genFile.PackageLanguage
        self.BuildCustomization = PackageBuildCustomization(genFile.BuildCustomization)

        # Fill all the package attributes that will be resolved with a initial value
        self.ResolvedPlatform = None
        self.ResolvedPlatformNotSupported = False
        self.ResolvedPlatformDirectNotSupported = False
        self.ResolvedPlatformName = None
        # All direct dependencies of this package
        self.ResolvedDirectDependencies = []
        # All dependencies both direct and indirect
        self.ResolvedAllDependencies = []
        # The build order of dependent packages
        self.ResolvedBuildOrder = []
        self.ResolvedBuildSourceFiles = None
        # The content files in this package
        self.ResolvedBuildContentFiles = None
        # The content source files in this package (the content files that need to be build)
        self.ResolvedBuildContentSourceFiles = None
        # The public include files in this package
        self.ResolvedBuildPublicIncludeFiles = None
        # All include files in this package (public+private)
        self.ResolvedBuildAllIncludeFiles = None
        self.ResolvedBuildAllIncludeDirs = None
        self.ResolvedBuildPublicIncludeDirs = None
        self.ResolvedBuildPrivateIncludeDirs = None
        # All direct featue uses of this package
        self.ResolvedDirectUsedFeatures = []
        # All feature uses both direct and indirect
        self.ResolvedAllUsedFeatures = []
        # All the direct cpp defines of this package
        self.ResolvedBuildDirectCPPDefines = None
        # All the cpp defines that touch this package (direct and inherited)
        self.ResolvedBuildAllCPPDefines = None
        # All the public cpp defines that touch this package (direct and inherited)
        self.ResolvedBuildAllPublicCPPDefines = None
        # All the private cpp defines that touch this package (direct and inherited)
        self.ResolvedBuildAllPrivateCPPDefines = None
        self.ResolvedDirectExternalDependencies = None
        self.ResolvedDirectVariants = None
        self.ResolvedAllVariantDict = None
        self.ResolvedMakeConfigName = None
        self.ResolvedMakeObjectPath = None
        # The complete variant name containing both the normal and virtual variant names
        self.ResolvedVariantName = None
        self.ResolvedNormalVariantName = None
        self.ResolvedVirtualVariantName = None
        # All variant dependencies (this is the dependencies pulled in by all variants)
        # Since we do a simple static evaluation of all variant dependencies that results in a fixed
        # build order for all variants we do a very strict variant dependency evaluation.
        # This means that while it could easly be possible for one variant to require
        # A->B->C->D and another to require D->C->B->A its simply not supported
        # since we require the global build order to stay fixed so we can do a static
        # build order setup
        #self.ResolvedDirectVariantDependencies = []
        # This is up the the build generator to set, it will be a PackageBuildReport object or None if the builder doesn't support it
        self.BuildReport = None


    def __ResolveAllowDependencyOnThis(self, type):
        if type == PackageType.Library:
            return True
        elif type == PackageType.Executable:
            return False
        elif type == PackageType.ExternalLibrary:
            return True
        elif type == PackageType.HeaderLibrary:
            return True
        elif type == PackageType.TopLevel:
            return False
        else:
            raise XmlException2(self.XMLElement, "Unknown package type: %s" % type);


    def GetUnsupportedPlatformList(self):
        unsupportedList = []
        for entry in self.GenFile.Platforms.values():
            if entry.NotSupported:
                unsupportedList.append(entry)
        return unsupportedList


    def GetPlatform(self, platformName):
        if platformName in self.GenFile.Platforms:
            return PackagePlatform(platformName, self.GenFile.Platforms[platformName])
        else:
            return PackagePlatform(platformName, None)

    def GetDirectDependencies(self, platformName):
        if not platformName in self.GenFile.Platforms:
            return self.GenFile.DirectDependencies
        else:
            return self.GenFile.DirectDependencies + self.GenFile.Platforms[platformName].DirectDependencies


    def GetExternalDependencies(self, platformName):
        if not platformName in self.GenFile.Platforms:
            return self.GenFile.ExternalDependencies
        else:
            return self.GenFile.ExternalDependencies + self.GenFile.Platforms[platformName].ExternalDependencies


    def GetVariants(self, platformName):
        if not platformName in self.GenFile.Platforms:
            return []
        else:
            return self.GenFile.Platforms[platformName].Variants


    def GetDirectCPPDefines(self, platformName):
        if not platformName in self.GenFile.Platforms:
            return self.GenFile.DirectCPPDefines
        else:
            return self.GenFile.DirectCPPDefines + self.GenFile.Platforms[platformName].DirectCPPDefines

    def GetDirectUsedFeatures(self, platformName):
        if not platformName in self.GenFile.Platforms:
            return self.GenFile.DirectUsedFeatures
        else:
            return self.GenFile.DirectUsedFeatures + self.GenFile.Platforms[platformName].DirectUsedFeatures
