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
import os
import os.path
import xml.etree.ElementTree as ET
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.XmlStuff import *
from FslBuildGen import IOUtil, Util, PackageConfig
from FslBuildGen.XmlToolConfigFile import XmlToolConfigFile


class ToolConfigTemplateFolder(object):
    def __init__(self, basedUponXML):
        super(ToolConfigTemplateFolder, self).__init__()
        self.BasedOn = basedUponXML
        self.Name = self.BasedOn.Name
        self.ResolvedPath = IOUtil.ToUnixStylePath(self.__ResolvePath(self.Name))


    def __ExtractEnvironmentVariablesFromName(self, entry):
        if entry.startswith("$("):
            endIndex = entry.find(")")
            if endIndex < 0:
                raise XmlException2(entry.XmlElement, "Incomplete environment variable found");
            return entry[2:endIndex]
        elif entry.find("$(") >= 0:
            raise XmlException2(entry.XmlElement, "Environment variables can not be located in the middle of a string");
        return None

    def __ResolvePath(self, entry):
        name = self.Name
        environmentName = self.__ExtractEnvironmentVariablesFromName(name)
        if environmentName != None:
            path = IOUtil.GetEnvironmentVariableForDirectory(environmentName)
            name = name.replace("$("+environmentName+")", path)
        elif not os.path.isabs(name):
            raise EnvironmentError("%s template folder path is not absolute" % (name))

        if not os.path.isdir(name):
            raise EnvironmentError("%s template folder is not a valid directory" % name)
        return name


class ToolConfigRootDirectory(object):
    def __init__(self, basedUponXML, dynamicSourceRootDir=None, dynamicRootName=None, dynamicPath=None):
        super(ToolConfigRootDirectory, self).__init__()
        if basedUponXML != None:
            self.BasedOn = basedUponXML
            self.Name = basedUponXML.Name
            self.DynamicName = basedUponXML.Name
        else:
            self.BasedOn = dynamicSourceRootDir
            self.Name = dynamicRootName
            self.DynamicName = dynamicPath

        envAndPath = self.__ExtractEnvironmentVariablesFromName(self.DynamicName, dynamicSourceRootDir != None)
        if envAndPath == None:
            raise XmlException2(self.BasedOn, "Root dirs are expected to contain environment variables");

        resolvedPath = IOUtil.GetEnvironmentVariableForDirectory(envAndPath[0]) + envAndPath[1]
        self.BashName = '${0}{1}'.format(envAndPath[0], envAndPath[1])
        self.DosName = '%{0}%{1}'.format(envAndPath[0], envAndPath[1])
        self.ResolvedPath = IOUtil.ToUnixStylePath(resolvedPath)
        self.ResolvedPathEx = self.ResolvedPath + '/'


    def __ExtractEnvironmentVariablesFromName(self, entry, allowCombinedPath):
        if entry.startswith("$("):
            endIndex = entry.find(")")
            if endIndex < 0:
                raise XmlException2(entry.XmlElement, "Incomplete environment variable found");
            elif not allowCombinedPath and endIndex != (len(entry)-1):
                raise XmlException2(entry.XmlElement, "The environment variable can not be combined with another path");
            return (entry[2:endIndex], entry[endIndex+1:])
        elif entry.find("$(") >= 0:
            raise XmlException2(entry.XmlElement, "Environment variables can not be located in the middle of a string");
        return None


class ToolConfigDirectory(object):
    def __init__(self, basedUponXML):
        super(ToolConfigDirectory, self).__init__()

        self.BasedOn = basedUponXML
        self.Name = self.BasedOn.Name

        envName, rest  = self.__SplitEnvironmentVariablesFromName(self.Name)
        if envName == None:
            raise XmlException2(basedUponXML.XmlElement, "Template import dirs are expected to contain environment variables");

        self.DecodedName = envName
        self.BashName = IOUtil.Join('$' + self.DecodedName, rest)
        self.DosName = IOUtil.Join('%' + self.DecodedName + '%', rest)
        if self.Name == None:
            raise XmlException2(basedUponXML.XmlElement, "Dirs are expected to contain environment variables");
        self.ResolvedPath = IOUtil.Join(IOUtil.GetEnvironmentVariableForDirectory(self.DecodedName), rest)
        self.ResolvedPathEx = self.ResolvedPath + '/'


    def __SplitEnvironmentVariablesFromName(self, entry):
        if entry.startswith("$("):
            endIndex = entry.find(")")
            if endIndex < 0:
                raise XmlException2(entry.XmlElement, "Incomplete environment variable found");

            env = entry[2:endIndex]
            rest = entry[endIndex+1:] if (endIndex+1) <= len(entry) else ''
            if rest.startswith('/') or rest.startswith('\\'):
                rest = rest[1:]
            return env, rest
        elif entry.find("$(") >= 0:
            raise XmlException2(entry.XmlElement, "Environment variables can not be located in the middle of a string");
        return None, None


class ToolConfigPackageLocation(object):
    def __init__(self, rootDirs, basedUponXML, projectRootDirectory, resolvedPath = None):
        super(ToolConfigPackageLocation, self).__init__()
        self.BasedOn = basedUponXML
        self.Name = basedUponXML.Name
        self.Blacklist = basedUponXML.Blacklist
        self.ResolvedPath = resolvedPath if resolvedPath != None else self.__ResolvePath(rootDirs, self, projectRootDirectory)

    def __ResolvePath(self, rootDirs, entry, projectRootDirectory):
        rootDir = self.__LocateRootDir(rootDirs, entry, projectRootDirectory)
        return entry.Name.replace(rootDir.Name, rootDir.ResolvedPath)

    def __LocateRootDir(self, rootDirs, entry, projectRootDirectory):
        if projectRootDirectory == None or not entry.Name.startswith(MagicStrings.ProjectRoot):
            for rootDir in rootDirs:
                if entry.Name.startswith(rootDir.Name):
                    return rootDir
        else:
            # Lets try to locate a root directory which the project is based in then use
            # it to dynamically add a new allowed root directory based on the project file location
            for rootDir in rootDirs:
                if projectRootDirectory.startswith(rootDir.ResolvedPathEx):
                    dynamicRootDir = projectRootDirectory[len(rootDir.ResolvedPathEx):]
                    dynamicRootDir = "{0}/{1}".format(rootDir.Name, dynamicRootDir)
                    return ToolConfigRootDirectory(None, rootDir, MagicStrings.ProjectRoot, dynamicRootDir)
        raise XmlException2(entry.BasedOn.XMLElement, "Path '%s' is not based on one of the valid root directories %s" % (entry.Name, ", ".join(Util.ExtractNames(rootDirs))))


class ToolConfigPackageConfiguration(object):
    def __init__(self, rootDirs, basedUponXML, configFileName, projectRootDirectory):
        super(ToolConfigPackageConfiguration, self).__init__()
        self.BasedOn = basedUponXML
        self.Name = basedUponXML.Name
        self.Locations = self.__ResolveLocations(rootDirs, basedUponXML.Locations, configFileName, projectRootDirectory)

    def __ResolveLocations(self, rootDirs, locations, configFileName, projectRootDirectory):
        uniqueNames = set()
        res = []
        for location in locations:
            packageLocation = ToolConfigPackageLocation(rootDirs, location, projectRootDirectory)
            if not packageLocation.Name in uniqueNames:
                uniqueNames.add(packageLocation.Name)
                res.append(packageLocation)
            else:
                raise DuplicatedConfigPackageLocation(packageLocation.Name, configFileName)
        return res


class ToolContentBuilder(object):
    def __init__(self, basedUponXML):
        super(ToolContentBuilder, self).__init__()
        self.BasedOn = basedUponXML
        self.Name = basedUponXML.Name
        self.Executable = basedUponXML.Executable
        self.Parameters = basedUponXML.Parameters
        self.FeatureRequirements = basedUponXML.FeatureRequirements
        self.DefaultExtensions = basedUponXML.DefaultExtensions
        self.Description = basedUponXML.Description


class ToolConfigContentBuilderConfiguration(object):
    def __init__(self, basedUponXML, configFileName):
        super(ToolConfigContentBuilderConfiguration, self).__init__()
        self.BasedOn = basedUponXML
        self.ContentBuilders = self.__ResolveContentBuilders(basedUponXML.ContentBuilders, configFileName) if basedUponXML else []

    def __ResolveContentBuilders(self, contentBuilders, configFileName):
        uniqueNames = set()
        res = []
        for contentBuilder in contentBuilders:
            contentBuilder = ToolContentBuilder(contentBuilder)
            if not contentBuilder.Name in uniqueNames:
                uniqueNames.add(contentBuilder.Name)
                res.append(contentBuilder)
            else:
                raise DuplicatedConfigContentBuilder(contentBuilder.Name, configFileName)
        return res


class ToolConfig(object):
    def __init__(self, filename, projectRootConfig):
        super(ToolConfig, self).__init__()
        basedUponXML = XmlToolConfigFile(filename, projectRootConfig)
        self.BasedOn = basedUponXML
        self.GenFileName = basedUponXML.GenFileName.Name
        self.RootDirectories = self.__ResolveRootDirectories(basedUponXML.RootDirectories, filename)
        self.TemplateImportDirectories = self.__ResolveDirectories(basedUponXML.TemplateImportDirectories)
        self.PackageConfiguration = self.__ResolvePackageConfiguration(self.RootDirectories, basedUponXML.PackageConfiguration, filename, projectRootConfig.RootDirectory)
        self.TemplateFolder = ToolConfigTemplateFolder(basedUponXML.TemplateFolder)
        self.ContentBuilderConfiguration = ToolConfigContentBuilderConfiguration(basedUponXML.ContentBuilderConfiguration, filename)
        self.UnitTestPath = self.__ResolveUnitTestPath()
        self.DefaultPackageLanguage = projectRootConfig.DefaultPackageLanguage
        self.ProjectRootConfig = projectRootConfig


    def ToPath(self, path):
        if path == None:
            return None
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '%s'" % (path))
        for rootDir in self.RootDirectories:
            if path.startswith(rootDir.ResolvedPathEx):
                lenRootPath = len(rootDir.ResolvedPathEx)
                path = path[lenRootPath:]
                return rootDir.Name + "/" + Util.UTF8ToAscii(path)
        raise UsageErrorException("ERROR: the folder '%s' does not reside inside one of the root dirs" % (path))


    def ToBashPath(self, path):
        if path == None:
            return None
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '%s'" % (path))
        for rootDir in self.RootDirectories:
            if path.startswith(rootDir.ResolvedPathEx):
                lenRootPath = len(rootDir.ResolvedPathEx)
                path = path[lenRootPath:]
                return rootDir.BashName + "/" + Util.UTF8ToAscii(path)
        raise UsageErrorException("ERROR: the folder '%s' does not reside inside one of the root dirs" % (path))

    def ToBashPathDirectConversion(self, path):
        """ This does not make the path relative to a root path """
        if path == None:
            return None
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '%s'" % (path))
        path = self.__ChangeToBashEnvironmentVariables(path)
        return Util.UTF8ToAscii(path).replace('\\', '/')


    def ToDosPath(self, path):
        if path == None:
            return None
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '%s'" % (path))
        for rootDir in self.RootDirectories:
            if path.startswith(rootDir.ResolvedPathEx):
                lenRootPath = len(rootDir.ResolvedPathEx)
                path = path[lenRootPath:]
                tmp = rootDir.DosName + "/" + Util.UTF8ToAscii(path)
                return tmp.replace('/', '\\')
        raise UsageErrorException("ERROR: the folder '%s' does not reside inside one of the root dirs" % (path))


    def ToDosPathDirectConversion(self, path):
        """ This does not make the path relative to a root path """
        if path == None:
            return None
        if path.find("\\") >= 0:
            raise UsageErrorException("Backslash found in the supplied path '%s'" % (path))
        path = self.__ChangeToDosEnvironmentVariables(path)
        return Util.UTF8ToAscii(path).replace('/', '\\')


    def __ChangeToBashEnvironmentVariables(self, path):
        index = path.find("$(")
        if index < 0:
            return path
        endIndex = path.find(")")
        if endIndex < 0:
            return path
        start = path[:index]
        envName = path[index+2:endIndex]
        end = path[endIndex+1:]
        path = "%s$%s%s" % (start, envName, end)
        return self.__ChangeToBashEnvironmentVariables(path)


    def __ChangeToDosEnvironmentVariables(self, path):
        index = path.find("$(")
        if index < 0:
            return path
        endIndex = path.find(")")
        if endIndex < 0:
            return path
        start = path[:index]
        envName = path[index+2:endIndex]
        end = path[endIndex+1:]
        path = "%s%%%s%%%s" % (start, envName, end)
        return self.__ChangeToDosEnvironmentVariables(path)

    def __ResolveRootDirectories(self, rootDirectories, configFileName):
        uniqueNames = set()        
        rootDirs = []
        for rootDir in rootDirectories:
            toolRootDir = ToolConfigRootDirectory(rootDir)
            if not toolRootDir.Name in uniqueNames:
                uniqueNames.add(toolRootDir.Name)
                rootDirs.append(toolRootDir)
            else:
                raise DuplicatedConfigRootPath(toolRootDir.Name, configFileName)
        return rootDirs

    def __ResolveDirectories(self, directories):
        dirs = []
        for dir in directories:
            dirs.append(ToolConfigDirectory(dir))
        return dirs


    def __ResolvePackageConfiguration(self, rootDirs, packageConfiguration, configFileName, projectRootDirectory):
        configs = {}
        for packageConfig in packageConfiguration.values():
            resolvedConfig = ToolConfigPackageConfiguration(rootDirs, packageConfig, configFileName, projectRootDirectory)
            configs[resolvedConfig.Name] = resolvedConfig
        return configs

    def __ResolveUnitTestPath(self):
        path = os.environ.get("FSL_GRAPHICS_INTERNAL")
        if path == None:
            return None
        return IOUtil.Join(path, "Tools/FslBuildGen/FslBuildGen/UnitTest/TestFiles")

