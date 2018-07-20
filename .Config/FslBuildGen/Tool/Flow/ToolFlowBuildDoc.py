#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

from typing import Any
from typing import Callable
from typing import cast
from typing import Dict
from typing import Optional
from typing import List
from typing import Set
import argparse
import json
import subprocess
from FslBuildGen import IOUtil
from FslBuildGen import Main as MainFlow
#from FslBuildGen import ParseUtil
from FslBuildGen.Generator import PluginConfig
from FslBuildGen import PluginSharedValues
from FslBuildGen.BasicConfig import BasicConfig
from FslBuildGen.BuildConfig.BuildDocConfiguration import BuildDocConfiguration
from FslBuildGen.Config import Config
from FslBuildGen.Context.GeneratorContext import GeneratorContext
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Log import Log
from FslBuildGen.PackageConfig import PlatformNameString
from FslBuildGen.PackageFilters import PackageFilters
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.PlatformUtil import PlatformUtil
from FslBuildGen.Tool.AToolAppFlow import AToolAppFlow
from FslBuildGen.Tool.AToolAppFlowFactory import AToolAppFlowFactory
from FslBuildGen.Tool.ToolAppConfig import ToolAppConfig
from FslBuildGen.Tool.ToolAppContext import ToolAppContext
from FslBuildGen.Tool.ToolCommonArgConfig import ToolCommonArgConfig
from FslBuildGen.ToolConfig import ToolConfig
from FslBuildGen.ToolConfigRootDirectory import ToolConfigRootDirectory
from FslBuildGen.Tool.Flow import ToolFlowBuild


JsonDictType = Dict[str, Any]


def ExtractPackages(packages: List[Package], packageType: int) -> List[Package]:
    res = []  # type: List[Package]
    for package in packages:
        if package.Type == packageType:
            res.append(package)
    return res


def IndexOf(lines: List[str], magicCommentContent: str) -> int:
    actualSearchString = "<!-- #{0}# -->".format(magicCommentContent)
    for idx, val in enumerate(lines):
        if val.strip() == actualSearchString:
            return idx
    return -1


def RightStripLines(lines: List[str]) -> None:
    for index, entry in enumerate(lines):
        lines[index] = entry.rstrip()


def TryReplaceSection(basicConfig: BasicConfig,
                      lines: List[str],
                      sectionName: str,
                      replacementLines: List[str],
                      path: str) -> Optional[List[str]]:
    sectionBegin = "{0}_BEGIN".format(sectionName)
    sectionEnd = "{0}_END".format(sectionName)
    startIndex = IndexOf(lines, sectionBegin)
    if startIndex < 0:
        basicConfig.LogPrint("WARNING: {0} did not contain a {1} section".format(path, sectionBegin))
        return None
    endIndex = IndexOf(lines, sectionEnd)
    if endIndex < 0:
        basicConfig.LogPrint("WARNING: {0} did not contain a {1} section".format(path, sectionEnd))
        return None
    if startIndex >= endIndex:
        basicConfig.LogPrint("WARNING: {0} {1} must appear before {2}".format(path, sectionBegin, sectionEnd))
        return None

    start = lines[0:startIndex+1]
    end = lines[endIndex:]
    return start + replacementLines + end


def TocEntryName(line: str) -> str:
    return line.strip()


def TocEntryLink(line: str) -> str:
    line = line.strip()
    line = line.replace(' ', '-')
    line = line.lower()
    return line


def TryTocPrepareLine(line: str) -> Optional[str]:
    line = line.strip()
    if line.startswith('['):
        index = line.find(']')
        if index == -1:
            return None
        return line[1:index]
    return line


def BuildTableOfContents(lines: List[str], depth: int) -> List[str]:
    # Dumb but simple TOC genration
    startIndex = IndexOf(lines, 'AG_TOC_END')
    if startIndex < 0:
        startIndex = 0

    tocLines = []
    for index in range(startIndex, len(lines)):
        line = lines[index]
        if line.startswith("# "):
            line = line[2:]
            resLine = TryTocPrepareLine(line)
            if resLine is not None:
                tocLines.append('* [{0}](#{1})'.format(TocEntryName(resLine), TocEntryLink(resLine)))
        elif line.startswith("## ") and depth >= 2:
            line = line[3:]
            resLine = TryTocPrepareLine(line)
            if resLine is not None:
                tocLines.append('  * [{0}](#{1})'.format(TocEntryName(resLine), TocEntryLink(resLine)))
        elif line.startswith("### ") and depth >= 3:
            line = line[4:]
            resLine = TryTocPrepareLine(line)
            if resLine is not None:
                tocLines.append('    * [{0}](#{1})'.format(TocEntryName(resLine), TocEntryLink(resLine)))
        elif line.startswith("#### ") and depth >= 4:
            line = line[5:]
            resLine = TryTocPrepareLine(line)
            if resLine is not None:
                tocLines.append('      * [{0}](#{1})'.format(TocEntryName(resLine), TocEntryLink(resLine)))
    return tocLines


def TryInsertTableOfContents(basicConfig: BasicConfig, lines: List[str], depth: int, path: str) -> Optional[List[str]]:
    tocLines = BuildTableOfContents(lines, depth)
    return TryReplaceSection(basicConfig, lines, "AG_TOC", tocLines, path)


# AG_DEMOAPP_HEADER_BEGIN
def BuildDemoAppHeader(package: Package) -> List[str]:
    result = []  # type: List[str]
    result.append("# {0}".format(package.ShortName))
    result.append('<img src="./Example.jpg" height="135px" style="float:right">')
    result.append("")
    return result


def TryLoadTextFileAsLines(path: str) -> Optional[List[str]]:
    result = IOUtil.TryReadFile(path)
    if result is None:
        return None
    lines = result.split('\n')
    return lines


def TryLoadReadMe(basicConfig: BasicConfig, path: str) -> Optional[List[str]]:
    lines = TryLoadTextFileAsLines(path)
    if lines is None:
        return None

    RightStripLines(lines)
    return lines


class OptionArgument:
    OptionNone = 0
    # The option requires a argument
    OptionRequired = 1

class OptionGroup:
    Default = 0x00000001
    Host = 0x00000002
    Demo = 0x00000004
    Custom0 = 0x00001000
    Custom1 = 0x00002000
    Custom2 = 0x00004000
    Custom3 = 0x00008000
    Custom4 = 0x00010000
    Custom5 = 0x00020000
    Custom6 = 0x00040000
    Custom7 = 0x00080000
    Custom8 = 0x00100000
    Custom9 = 0x00200000
    Hidden = 0x40000000


class ProgramArgument(object):
    def __init__(self, basicConfig: BasicConfig, entry: Dict[str, Any]) -> None:
        super(ProgramArgument, self).__init__()
        self.SourceName = self.__ReadEntry(entry, "SourceName")
        self.Description = self.__ReadEntry(entry, "Description")
        self.Group = self.__ReadOptionGroup(entry, "Group")
        self.HasArg = self.__ReadOptionArgument(entry, "HasArg")
        self.IsPositional = self.__ReadEntry(entry, "IsPositional")
        self.Name = self.__ReadEntry(entry, "Name", "")
        self.ShortName = self.__ReadEntry(entry, "ShortName", "")
        self.Type = self.__ReadEntry(entry, "Type",  "")
        self.Help_FormattedName = self.__ReadEntry(entry, "Help_FormattedName", "")

        strEnding = "OptionParser"
        if self.SourceName.endswith(strEnding):
            self.SourceName = self.SourceName[0:-len(strEnding)]

    def __filterNonPrintable(self, strSrc: str) -> str:
        return ''.join([c for c in strSrc if ord(c) > 31 or ord(c) == 9])


    def __ReadEntry(self, srcDict: Dict[str, Any], name: str, defaultValue: Optional[str] = None) -> str:
        if name in srcDict:
            return self.__filterNonPrintable(srcDict[name].strip())
        elif defaultValue is not None:
            return defaultValue
        raise Exception("command line argument entry missing attribute: '{0}'".format(name))


    def __TryReadEntry(self, srcDict: Dict[Any, str], name: str, defaultValue: Optional[str] = None) -> Optional[str]:
        if name in srcDict:
            return self.__filterNonPrintable(srcDict[name].strip())
        return defaultValue


    def __ReadOptionArgument(self, srcDict: Dict[str, Any], name: str) -> int:
        result = self.__ReadEntry(srcDict, name)
        if result == '0':
            return OptionArgument.OptionNone
        elif result == '1':
            return OptionArgument.OptionRequired
        else:
            raise Exception("command line argument OptionArgument '{0}' has unsupported value of '{1}'".format(name, result))


    def __ReadOptionGroup(self, srcDict: Dict[str, Any], name: str) -> int:
        result = self.__ReadEntry(srcDict, name)
        return int(result)



def DecodeJsonArgumentList(basicConfig: BasicConfig, arguments: List[Dict[str, Any]]) -> List[ProgramArgument]:
    res = []  # type: List[ProgramArgument]
    for entry in arguments:
        res.append(ProgramArgument(basicConfig, entry))
    return res


def GetMaxFormattedNameLength(entries: List[ProgramArgument]) -> int:
    count = 0
    for entry in entries:
        if len(entry.Help_FormattedName) > count:
            count = len(entry.Help_FormattedName)
    return count


def GetMaxDescriptionLength(entries: List[ProgramArgument]) -> int:
    count = 0
    for entry in entries:
        if len(entry.Description) > count:
            count = len(entry.Description)
    return count


def GetMaxSourceNameLength(entries: List[ProgramArgument]) -> int:
    count = 0
    for entry in entries:
        if len(entry.SourceName) > count:
            count = len(entry.Description)
    return count


def SafeMarkdownString(strSrc: str) -> str:
    return strSrc.replace('<', '\\<')


#def GroupArgumentsBySourceName(basicConfig, arguments):
#    dict = {}
#    for entry in arguments:
#        if not entry.SourceName in dict:
#            dict[entry.SourceName] = []
#        dict[entry.SourceName].append(entry)

#    for entry in dict.values():
#        entry.sort(key=lambda s: s.Help_FormattedName.lower())
#    return dict


def TryBuildArgumentTableLines(basicConfig: BasicConfig, argumentDict: JsonDictType) -> Optional[List[str]]:
    mainKey = "arguments"
    if not mainKey in argumentDict:
        return None

    srcArguments = argumentDict[mainKey] # type: List[Dict[str, Any]]
    arguments = DecodeJsonArgumentList(basicConfig, srcArguments)  # type: List[ProgramArgument]

    maxNameLength = GetMaxFormattedNameLength(arguments)
    maxDescLength = GetMaxDescriptionLength(arguments)
    maxSourceNameLength = GetMaxSourceNameLength(arguments)

    arguments.sort(key=lambda s: (s.SourceName, s.Help_FormattedName.lower()))

    #groupedArgumentDict = GroupArgumentsBySourceName(basicConfig, arguments)

    #sortedKeys = list(arguments.keys())
    #sortedKeys.sort()

    #Command line arguments:
    #
    #Argument            |Description
    #--------------------|---

    formatString = "{{0:<{0}}}|{{1:<{1}}}|{{2}}".format(maxNameLength, maxDescLength)

    result = []  # type: List[str]

#    for key, argumentList in groupedArgumentDict.items():
    result.append("")
    result.append("Command line arguments':")
    result.append("")
    result.append(formatString.format("Argument", "Description", "Source"))
    result.append("{0}|{1}|{2}".format("-"*maxNameLength, "-"*maxDescLength, "-"*maxSourceNameLength))
    for entry in arguments:
        if entry.Group != OptionGroup.Hidden:
            result.append(formatString.format(SafeMarkdownString(entry.Help_FormattedName), SafeMarkdownString(entry.Description), SafeMarkdownString(entry.SourceName)))
    return result



def UpdatePackageReadMe(basicConfig: BasicConfig,
                        package: Package,
                        lines: List[str],
                        packageArgumentsDict: Dict[Package, JsonDictType],
                        path: str) -> List[str]:
    newHeader = BuildDemoAppHeader(package)
    lines2 = TryReplaceSection(basicConfig, lines, "AG_DEMOAPP_HEADER", newHeader, path)
    if lines2 is not None:
        lines = lines2
    if package in packageArgumentsDict:
        argumentsLines = TryBuildArgumentTableLines(basicConfig, packageArgumentsDict[package])
        if argumentsLines is not None:
            newLines = TryReplaceSection(basicConfig, lines, "AG_DEMOAPP_COMMANDLINE_ARGUMENTS", argumentsLines, path)
            if newLines is not None:
                lines = newLines
    return lines


def SaveReadMe(config: Config, path: str, lines: List[str]) -> None:
    text = "\n".join(lines)
    if not config.DisableWrite:
        IOUtil.WriteFileIfChanged(path, text)


def TryExtractBrief(basicConfig: BasicConfig, lines: List[str], path: str) -> Optional[List[str]]:
    startIndex = IndexOf(lines, "AG_BRIEF_BEGIN")
    if startIndex < 0:
        basicConfig.LogPrint("WARNING: {0} did not contain a AG_BRIEF_BEGIN section".format(path))
        return None
    endIndex = IndexOf(lines, "AG_BRIEF_END")
    if endIndex < 0:
        basicConfig.LogPrint("WARNING: {0} did not contain a AG_BRIEF_END section".format(path))
        return None
    if startIndex >= endIndex:
        basicConfig.LogPrint("WARNING: {0} AG_BRIEF_BEGIN must appear before AG_BRIEF_END".format(path))
        return None

    result = lines[startIndex+1:endIndex]

    # remove all empty lines at the end
    index = len(result)-1
    while index >= 0 and len(result[index]) == 0:
        result.pop(index)
        index = index - 1

    return result


def ReadJsonFile(filename: str) -> JsonDictType:
    content = IOUtil.ReadBinaryFile(filename)
    return cast(JsonDictType, json.loads(content))


def TryBuildAndRun(toolAppContext: ToolAppContext, config: Config, package: Package) -> Optional[JsonDictType]:
    if package.ResolvedPlatformNotSupported:
        return None
    if package.AbsolutePath is None:
        raise Exception("Invalid package")
    workDir = package.AbsolutePath
    tmpOutputFilename = IOUtil.Join(workDir, 'FslBuildDoc_AppArguments.json')
    try:
        # FslBuild.py --ForAllExe "(EXE) --System.Arguments.Save <filename>"

        toolFlowConfig = ToolFlowBuild.GetDefaultLocalConfig()
        toolFlowConfig.SetToolAppConfigValues(toolAppContext.ToolAppConfig)
        toolFlowConfig.ForAllExe = '(EXE) --System.Arguments.Save {0} -h'.format(tmpOutputFilename)
        buildFlow = ToolFlowBuild.ToolFlowBuild(toolAppContext)
        buildFlow.Process(workDir, config.ToolConfig, toolFlowConfig)

        return ReadJsonFile(tmpOutputFilename)
    except (Exception) as ex:
        if toolAppContext.LowLevelToolConfig.DebugEnabled:
            raise
        config.LogPrint("Failed to build and run '{0}' due to exception {1}".format(package.Name, ex))
        return None
    finally:
        IOUtil.RemoveFile(tmpOutputFilename)


def ExtractArguments(toolAppContext: ToolAppContext, config: Config, exePackages: List[Package], extractArguments: str) -> Dict[Package, JsonDictType]:
    config.LogPrint("Building all executable packages to extract their command line arguments")

    filterDir = None if extractArguments == '*' else IOUtil.GetCurrentWorkingDirectory()

    res = {}  # type: Dict[Package, JsonDictType]
    for package in exePackages:
        if filterDir is None or package.AbsolutePath == filterDir:
            config.LogPrint("- Building and running {0}".format(package.Name))
            arguments = TryBuildAndRun(toolAppContext, config, package)
            if arguments is not None:
                res[package] = arguments

        # quick exit
        #return res
    return res

def __TryFindRequirementInSet(requirements: List[PackageRequirement], ignoreRequirementSet: Set[str]) -> Optional[PackageRequirement]:
    for requirement in requirements:
        if requirement.Name in ignoreRequirementSet:
            return requirement
    return None

def __RemoveIgnored(log: Log, packages: List[Package], ignoreRequirementSet: Set[str]) -> List[Package]:
    if len(ignoreRequirementSet) <= 0:
        return packages
    filteredPackages = [] # type: List[Package]
    for package in packages:
        skipRequirement = __TryFindRequirementInSet(package.ResolvedAllUsedFeatures, ignoreRequirementSet)
        if skipRequirement is None:
            filteredPackages.append(package)
        else:
            log.LogPrint("Skipping '{0}' because requirement '{1}' was set to skip".format(package.Name, skipRequirement.Name))
    return filteredPackages

def ProcessPackages(toolAppContext: ToolAppContext, config: Config, packages: List[Package], activeRootDir: ToolConfigRootDirectory,
                    extractArguments: Optional[str], buildDocConfiguration: BuildDocConfiguration) -> List[str]:
    log = config # type: Log
    ignoreRequirementSet = set() # type: Set[str]
    for requirement in buildDocConfiguration.Requirements:
        if requirement.Skip:
            ignoreRequirementSet.add(requirement.Name)

    exePackages = ExtractPackages(packages, PackageType.Executable)
    exePackages = __RemoveIgnored(log, exePackages, ignoreRequirementSet)
    exePackages.sort(key=lambda s: None if s.AbsolutePath is None else s.AbsolutePath.lower())

    packageArgumentsDict = {}  # type: Dict[Package, JsonDictType]
    if extractArguments is not None:
        packageArgumentsDict = ExtractArguments(toolAppContext, config, exePackages, extractArguments)

    uniqueDict = {}  # type: Dict[str, List[Package]]
    for package in exePackages:
        if package.Namespace is None:
            raise Exception("Invalid package")
        if not package.Namespace in uniqueDict:
            uniqueDict[package.Namespace] = []
        uniqueDict[package.Namespace].append(package)

    # sort the content
    for packageList in list(uniqueDict.values()):
        packageList.sort(key=lambda s: s.Name.lower())
    sortedKeys = list(uniqueDict.keys())
    sortedKeys.sort()

    # Format it
    isFirst = True
    result = []
    for key in sortedKeys:
        if isFirst:
            isFirst = False
        else:
            result.append("")
        result.append("## {0}".format(key))
        result.append("")
        for package in uniqueDict[key]:
            if package.AbsolutePath is None:
                raise Exception("Invalid package")
            rootDir = config.ToolConfig.TryFindRootDirectory(package.AbsolutePath)
            if rootDir == activeRootDir:
                config.LogPrintVerbose(4, "Processing package '{0}'".format(package.Name))
                packageDir = package.AbsolutePath[len(rootDir.ResolvedPath)+1:]
                result.append("### [{0}]({1})".format(package.ShortName, packageDir))
                exampleImagePath = IOUtil.Join(package.AbsolutePath, "Example.jpg")
                if IOUtil.IsFile(exampleImagePath):
                    exampleImagePath = exampleImagePath[len(rootDir.ResolvedPath)+1:]
                    result.append('<a href="{0}">'.format(packageDir))
                    result.append('<img src="{0}" height="108px" style="float:right;clear:both;display:table;margin:1px">'.format(exampleImagePath))
                    result.append('</a>')
                    result.append("")

                readmePath = IOUtil.Join(package.AbsolutePath, "README.md")
                packageReadMeLines = TryLoadReadMe(config, readmePath)
                if packageReadMeLines is not None:
                    packageReadMeLines = UpdatePackageReadMe(config, package, packageReadMeLines, packageArgumentsDict, readmePath)
                    SaveReadMe(config, readmePath, packageReadMeLines)
                    brief = TryExtractBrief(config, packageReadMeLines, readmePath)
                    if brief is not None:
                        result = result + brief
                        result.append("")

                result.append("")
            #else:
            #    config.LogPrintVerbose(4, "Skipping package '{0}' with rootDir '{1}' is not part of the activeRootDir '{2}'".format(package.Name, rootDir.ResolvedPath, activeRootDir.ResolvedPath))

    return result

class DefaultValue(object):
    DryRun = False
    ToCDepth = 2
    ExtractArguments = None # type: Optional[str]


class LocalToolConfig(ToolAppConfig):
    def __init__(self) -> None:
        super(LocalToolConfig, self).__init__()

        self.DryRun = DefaultValue.DryRun
        self.ToCDepth = DefaultValue.ToCDepth
        self.ExtractArguments = DefaultValue.ExtractArguments


def GetDefaultLocalConfig() -> LocalToolConfig:
    return LocalToolConfig()


class ToolFlowBuildDoc(AToolAppFlow):
    def __init__(self, toolAppContext: ToolAppContext) -> None:
        super(ToolFlowBuildDoc, self).__init__(toolAppContext)


    def ProcessFromCommandLine(self, args: Any, currentDirPath: str, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        # Process the input arguments here, before calling the real work function

        localToolConfig = LocalToolConfig()

        # Configure the ToolAppConfig part
        localToolConfig.SetToolAppConfigValues(self.ToolAppContext.ToolAppConfig)

        # Configure the local part
        localToolConfig.DryRun = args.DryRun
        localToolConfig.ToCDepth = int(args.ToCDepth)
        localToolConfig.ExtractArguments = args.ExtractArguments

        self.Process(currentDirPath, toolConfig, localToolConfig)


    def Process(self, currentDirPath: str, toolConfig: ToolConfig, localToolConfig: LocalToolConfig) -> None:
        config = Config(self.Log, toolConfig, 'sdk', localToolConfig.BuildVariantsDict, localToolConfig.AllowDevelopmentPlugins)
        if localToolConfig.DryRun:
            config.ForceDisableAllWrite()

        if localToolConfig.ToCDepth < 1:
            localToolConfig.ToCDepth = 1
        elif localToolConfig.ToCDepth > 4:
            localToolConfig.ToCDepth = 4

        config.PrintTitle()

        # Get the platform and see if its supported
        platform = PluginConfig.GetGeneratorPluginById(localToolConfig.PlatformName, False)
        PlatformUtil.CheckBuildPlatform(platform.Name)

        config.LogPrint("Active platform: {0}".format(platform.Name))

        packageFilters = localToolConfig.BuildPackageFilters

        theFiles = MainFlow.DoGetFiles(config, toolConfig.GetMinimalConfig(), currentDirPath, localToolConfig.Recursive)
        generatorContext = GeneratorContext(config, config.ToolConfig.Experimental, platform)
        packages = MainFlow.DoGetPackages(generatorContext, config, theFiles, packageFilters)
        #topLevelPackage = PackageListUtil.GetTopLevelPackage(packages)
        #featureList = [entry.Name for entry in topLevelPackage.ResolvedAllUsedFeatures]

        for rootDir in config.ToolConfig.RootDirectories:
            readmePath = IOUtil.Join(rootDir.ResolvedPath, "README.md")
            packageReadMeLines = TryLoadReadMe(config, readmePath)
            result = ProcessPackages(self.ToolAppContext, config, packages, rootDir, localToolConfig.ExtractArguments,
                                     toolConfig.BuildDocConfiguration)
            if packageReadMeLines is not None:
                packageReadMeLinesNew = TryReplaceSection(config, packageReadMeLines, "AG_DEMOAPPS", result, readmePath)
                if packageReadMeLinesNew is not None:
                    packageReadMeLines = packageReadMeLinesNew

                packageReadMeLinesNew = TryInsertTableOfContents(config, packageReadMeLines, localToolConfig.ToCDepth, readmePath)
                if packageReadMeLinesNew is not None:
                    packageReadMeLines = packageReadMeLinesNew

                SaveReadMe(config, readmePath, packageReadMeLines)
            elif config.Verbosity > 2:
                config.LogPrintWarning("No README.md found in {0}".format(rootDir.ResolvedPath))


class ToolAppFlowFactory(AToolAppFlowFactory):
    def __init__(self) -> None:
        pass


    def GetTitle(self) -> str:
        return 'FslBuilDoc'


    def GetToolCommonArgConfig(self) -> ToolCommonArgConfig:
        argConfig = ToolCommonArgConfig()
        argConfig.AddPlatformArg = True
        argConfig.SupportBuildTime = True
        #argConfig.AllowVSVersion = True
        # These are used when: --ExtractArguments is enabled
        argConfig.AddBuildFiltering = True
        argConfig.AddBuildThreads = True
        argConfig.AddBuildVariants = True
        argConfig.AllowRecursive = True
        return argConfig


    def AddCustomArguments(self, parser: argparse.ArgumentParser, toolConfig: ToolConfig, userTag: Optional[object]) -> None:
        parser.add_argument('--DryRun', action='store_true', help='No files will be created')
        parser.add_argument('--ToCDepth', default=str(DefaultValue.ToCDepth), help='The headline depth to include, defaults to {0} (1-4)'.format(DefaultValue.ToCDepth))
        parser.add_argument('--ExtractArguments', default=DefaultValue.ExtractArguments, help='Build the app and execute it to extract the command line arguments ("*" for all or "." for current dirs package)')


    def Create(self, toolAppContext: ToolAppContext) -> AToolAppFlow:
        return ToolFlowBuildDoc(toolAppContext)