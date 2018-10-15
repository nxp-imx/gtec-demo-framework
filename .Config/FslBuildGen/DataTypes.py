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

from typing import List
from typing import Optional
from FslBuildGen.Exceptions import UnknownTypeException

class PackageType:
    TopLevel = 0          # a top level package, this is a internal thing (used for total
                          # verification)
    Library = 1           # a static library
    Executable = 2        # a executable
    ExternalLibrary = 3   # a unmanaged static library
    HeaderLibrary = 4     # a header only library
    ToolRecipe = 5        # a external tool recipe
    #ExeLibCombo =  5      # a executeable and static library combo (not supported yet)


    @staticmethod
    def ToString(value: int) -> str:
        if value == PackageType.TopLevel:
            return "TopLevel"
        elif value == PackageType.Library:
            return "Library"
        elif value == PackageType.Executable:
            return "Executable"
        elif value == PackageType.ExternalLibrary:
            return "ExternalLibrary"
        elif value == PackageType.HeaderLibrary:
            return "HeaderLibrary"
        elif value == PackageType.ToolRecipe:
            return "ToolRecipe"
        raise UnknownTypeException("Unknown PackageType: {0}".format(value))

    @staticmethod
    def FromString(value: str) -> int:
        if value == "TopLevel":
            return PackageType.TopLevel
        elif value == "Library":
            return PackageType.Library
        elif value == "Executable":
            return PackageType.Executable
        elif value == "ExternalLibrary":
            return PackageType.ExternalLibrary
        elif value == "HeaderLibrary":
            return PackageType.HeaderLibrary
        elif value == "ToolRecipe":
            return PackageType.ToolRecipe
        raise Exception("Unknown PackageType: {0}".format(value))

    @staticmethod
    def AllStrings() -> List[str]:
        return ["TopLevel", "Library", "Executable", "ExternalLibrary", "HeaderLibrary", "ToolRecipe"]


# Beware that code relies on the more accessible accesstype being smaller value (so public < private < link)
class AccessType:
    Public = 0
    Private = 1
    Link = 2  # (only valid for Dependency)

    @staticmethod
    def ToString(value: int) -> str:
        if value == AccessType.Public:
            return "Public"
        elif value == AccessType.Private:
            return "Private"
        elif value == AccessType.Link:
            return "Link"
        raise Exception("Unknown AccessType: {0}".format(value))


class ExternalDependencyType:
    StaticLib = 1
    DLL = 2
    Headers = 3
    # C# specific
    Assembly = 4
    PackageReference = 5
    # CMake specific
    Find = 6

    @staticmethod
    def ToString(value: int) -> str:
        if value == ExternalDependencyType.StaticLib:
            return 'StaticLib'
        elif value == ExternalDependencyType.DLL:
            return 'DLL'
        elif value == ExternalDependencyType.Headers:
            return 'Headers'
        elif value == ExternalDependencyType.Assembly:
            return 'Assembly'
        elif value == ExternalDependencyType.PackageReference:
            return 'PackageReference'
        elif value == ExternalDependencyType.Find:
            return 'Find'
        raise Exception("Unknown ExternalDependencyType: {0}".format(value))

    @staticmethod
    def TryFromString(value: str) -> Optional[int]:
        if value == 'StaticLib':
            return ExternalDependencyType.StaticLib
        elif value == 'DLL':
            return ExternalDependencyType.DLL
        elif value == 'Headers':
            return ExternalDependencyType.Headers
        elif value == 'Assembly':
            return ExternalDependencyType.Assembly
        elif value == 'PackageReference':
            return ExternalDependencyType.PackageReference
        elif value == 'Find':
            return ExternalDependencyType.Find
        return None


    @staticmethod
    def FromString(value: str) -> int:
        result = ExternalDependencyType.TryFromString(value)
        if result is not None:
            return result
        raise Exception("Unknown external dependency type: '{0}' expected: StaticLib, DLL, Headers, Assembly, Find".format(value))


class VariantType:
    Normal = 1
    Virtual = 2


class SubPackageSupport:
    Disabled = 1
    Enabled = 2
    ExecutableOnly = 3


class CompilerNames:
    VisualStudio = "VisualStudio"


class VisualStudioVersion:
    VS2015 = 2015
    VS2017 = 2017
    DEFAULT = VS2015

    AllEntries = [VS2015, VS2017]

    @staticmethod
    def ToString(value: int) -> str:
        if value == VisualStudioVersion.VS2015:
            return "2015"
        elif value == VisualStudioVersion.VS2017:
            return "2017"
        return "Unknown"

    @staticmethod
    def FromString(strValue: str) -> int:
        result = VisualStudioVersion.TryParse(strValue)
        if result is not None:
            return result
        else:
            raise Exception("Unsupported visual studio version {0}".format(strValue))


    @staticmethod
    def TryParse(strValue: str) -> Optional[int]:
        if strValue == "2015":
            return VisualStudioVersion.VS2015
        elif strValue == "2017":
            return VisualStudioVersion.VS2017
        return None


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
    def ToString(value: int) -> str:
        if value == PackageLanguage.CPP:
            return "C++"
        elif value == PackageLanguage.CSharp:
            return "C#"
        return "Unknown"

    @staticmethod
    def FromString(strPackageLanguage: str) -> int:
        if strPackageLanguage == "C++":
            return PackageLanguage.CPP
        elif strPackageLanguage == "C#":
            return PackageLanguage.CSharp
        else:
            raise Exception("Unsupported package language {0}".format(strPackageLanguage))


class OptimizationType:
    Disabled = 0
    Default = 1
    Full = 2

    @staticmethod
    def ToString(value: int) -> str:
        if value == OptimizationType.Disabled:
            return "Disabled"
        elif value == OptimizationType.Default:
            return "Default"
        elif value == OptimizationType.Full:
            return "Full"
        else:
            raise Exception("Unsupported OptimizationType {0}".format(value))


class MagicStrings:
    ProjectRoot = "${PROJECT_ROOT}"
    VSDefaultCPPTemplate = "DF"
    VSDefaultCPPLinuxTemplate = "DF-Linux"
    DefaultSDKConfiguration = "sdk"


class CheckType:
    Normal = 1
    Forced = 2


class ScanMethod:
    Directory = 0
    OneSubDirectory = 1
    AllSubDirectories = 2

    @staticmethod
    def TryToString(value: int, returnValueStringIfUnknown: bool = False) -> Optional[str]:
        if value == ScanMethod.Directory:
            return "Directory"
        elif value == ScanMethod.OneSubDirectory:
            return "OneSubDirectory"
        elif value == ScanMethod.AllSubDirectories:
            return "AllSubDirectories"
        else:
            return None if not returnValueStringIfUnknown else "{0}".format(value)

    @staticmethod
    def ToString(value: int) -> str:
        if value == ScanMethod.Directory:
            return "Directory"
        elif value == ScanMethod.OneSubDirectory:
            return "OneSubDirectory"
        elif value == ScanMethod.AllSubDirectories:
            return "AllSubDirectories"
        else:
            raise Exception("Unsupported ScanMethod {0}".format(value))

    @staticmethod
    def FromString(value: str) -> int:
        if value == "Directory":
            return ScanMethod.Directory
        elif value == "OneSubDirectory":
            return ScanMethod.OneSubDirectory
        elif value == "AllSubDirectories":
            return ScanMethod.AllSubDirectories
        else:
            raise Exception("Unsupported ScanMethod {0}".format(value))



class PackageRequirementTypeString:
    Invalid = "invalid"
    Feature = "feature"
    Extension = "extension"


class PackageRequirementType:
    Invalid = 0
    Feature = 1
    Extension = 2

    @staticmethod
    def TryFromString(value: str) -> int:
        if value == PackageRequirementTypeString.Feature:
            return PackageRequirementType.Feature
        elif value == PackageRequirementTypeString.Extension:
            return PackageRequirementType.Extension
        return PackageRequirementType.Invalid

    @staticmethod
    def FromString(value: str) -> int:
        result = PackageRequirementType.TryFromString(value)
        if result != PackageRequirementType.Invalid:
            return result
        raise Exception("Unsupported PackageRequirementType {0}".format(value))


class PackageCreationYearString:
    NotDefined = "NotDefined"


class BuildVariantConfig:
    Debug = 0
    Release = 1

    @staticmethod
    def ToString(value: int) -> str:
        if value == BuildVariantConfig.Release:
            return "release"
        elif value == BuildVariantConfig.Debug:
            return "debug"
        else:
            raise Exception("Unsupported BuildVariantConfig '{0}'".format(value))

    @staticmethod
    def FromString(value: str) -> int:
        if value == "release":
            return BuildVariantConfig.Release
        elif value == "debug":
            return BuildVariantConfig.Debug
        else:
            raise Exception("Unsupported BuildVariantConfig '{0}'".format(value))


class CMakeTargetType:
    Project = 0
    Install = 1

    @staticmethod
    def FromString(value: str) -> int:
        if value == "project":
            return CMakeTargetType.Project
        elif value == "install":
            return CMakeTargetType.Install
        else:
            raise Exception("Unsupported CMakeTargetType '{0}'".format(value))


class BuildRecipePipelineCommand:
    Invalid = 0
    Download = 1
    GitClone = 2
    Unpack = 3
    CMakeBuild = 4
    Source = 5
    Combine = 6
    Copy = 7
    JoinCopy = 8
    JoinUnpack = 9
    JoinGitApply = 10
    JoinDelete = 11


class BuildRecipeValidateCommand:
    Invalid = 0
    EnvironmentVariable = 1
    Path = 2
    FindFileInPath = 3
    FindExecutableFileInPath = 4
    AddHeaders = 5
    AddLib = 6
    AddDLL = 7
    AddTool = 8


class BuildRecipeValidateMethod:
    IsDirectory = 0
    IsFile = 1
    Exists = 2

    @staticmethod
    def FromString(value: str) -> int:
        if value == "IsDirectory":
            return BuildRecipeValidateMethod.IsDirectory
        elif value == "IsFile":
            return BuildRecipeValidateMethod.IsFile
        elif value == "Exists":
            return BuildRecipeValidateMethod.Exists
        raise Exception("Unsupported BuildRecipeValidateMethod '{0}'".format(value))

    @staticmethod
    def ToString(value: int) -> str:
        result = BuildRecipeValidateMethod.TryToString(value)
        if result is None:
            raise Exception("Unsupported BuildRecipeValidateMethod '{0}'".format(value))
        return result

    @staticmethod
    def TryToString(value: int, returnValueStringIfUnknown: bool = False) -> Optional[str]:
        if value == BuildRecipeValidateMethod.IsDirectory:
            return "IsDirectory"
        elif value == BuildRecipeValidateMethod.IsFile:
            return "IsFile"
        elif value == BuildRecipeValidateMethod.Exists:
            return "Exists"
        return None if not returnValueStringIfUnknown else "{0}".format(value)

class BuildThreads:
    NotDefined = 0
    Auto = -1

    @staticmethod
    def FromString(value: str) -> int:
        if value == "auto" or value == "Auto":
            return BuildThreads.Auto
        try:
            return int(value)
        except ValueError:
            raise Exception("Unsupported BuildThreads '{0}'".format(value))


class BoolStringHelper(object):
    @staticmethod
    def FromString(value: str) -> bool:
        result = BoolStringHelper.TryFromString(value)
        if result is None:
            raise Exception("Failed to parse boolean string '{0}' to bool value".format(value))
        return result


    @staticmethod
    def TryFromString(value: str) -> Optional[bool]:
        if value == "true":
            return True
        elif value == "false":
            return False
        return None

class PackageString(object):
    PLATFORM_WILDCARD = '*'
