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

class PackageType:
    TopLevel = 0          # a top level package, this is a internal thing (used for total
                          # verification)
    Library = 1           # a static library
    Executable = 2        # a executable
    ExternalLibrary = 3   # a unmanaged static library
    HeaderLibrary = 4     # a header only library
    #ExeLibCombo =  5      # a executeable and static library combo (not supported yet)

# Beware that code relies on the more accessible accesstype being smaller value (so public < private < link)
class AccessType:
    Public = 0
    Private = 1
    Link = 2 # (only valid for Dependency)

class ExternalDependencyType:
    StaticLib = 1
    Headers = 2
    # CMake specific
    Find = 3

class VariantType:
    Normal = 1
    Virtual = 2

class SubPackageSupport:
    Disabled = 1
    Enabled = 2
    ExecutableOnly = 3

class VisualStudioVersion:
    VS2013 = 2013
    VS2015 = 2015
    VS2017 = 2017
    Latest = 2015

class BuildPlatformType:
    Unix = 0
    Windows = 1
    Unknown = -1

class GeneratorType:
    Default = 0
    Experimental = 1
    Deprecated = 2

class BuildVariantType:
    Static = 0
    Dynamic = 1

class PackageLanguage:
    CPP = 0
    CSharp = 1

    @staticmethod
    def ToString(value):
        if value == PackageLanguage.CPP:
            return "C++"
        elif value == PackageLanguage.CSharp:
            return "C#"
        return "Unknown"

    @staticmethod
    def FromString(strPackageLanguage):
        if strPackageLanguage == "C++":
            return PackageLanguage.CPP
        elif strPackageLanguage == "C#":
            return PackageLanguage.CSharp
        else:
            raise Exception("Unsupported package language {0}".format(strPackageLanguage));


class OptimizationType:
    Disabled = 0
    Default = 1
    Full = 2


class MagicStrings:
    ProjectRoot = "{PROJECT_ROOT}"
    VSDefaultCPPTemplate = "DF"
    VSDefaultCPPLinuxTemplate = "DF-Linux"

#
#class ProjectType:
#  DemoFramework = 0
#  CSharpXNA = 1
#  CSharpMonoGame = 2
#  CSharpLatest = 3
