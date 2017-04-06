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
from FslBuildGen.Exceptions import *
from FslBuildGen import IOUtil

def LocateToplevelPackage(packages):
    for package in packages:
        if package.Type == PackageType.TopLevel:
            return package
    return None

def ToIfString(entries):
    res = []
    for entry in entries:
        res.append('[ "$FSL_PLATFORM_NAME" != "%s" ]' % entry.Name)
    return res

def GeneratePlatformCheckString(entries):
    return " && ".join(ToIfString(entries))


def UsesFeatue(package, featureFilters):
    if featureFilters == None:
        return True
    for filter in featureFilters:
        for feature in package.ResolvedAllUsedFeatures:
            if feature.Name == filter:
                return True
    return False


def LocateAllExecutables(packages, featureFilters, featureFilterBlacklist):
    exePackages = []
    for package in packages:
        if package.Type == PackageType.Executable and UsesFeatue(package, featureFilters) and (not featureFilterBlacklist or not UsesFeatue(package, featureFilterBlacklist)):
            exePackages.append(package)
    return exePackages


def BuildPackageWhiteList(packages):
    whitelist = set()
    for package in packages:
        for currentPackage in package.ResolvedBuildOrder:
            if not currentPackage in whitelist:
                whitelist.add(currentPackage)
    return whitelist


def GenerateBuildScriptAllWithFilterUnix(config, packages, featureFilters, featureFilterBlacklist, dstFilename):
    template = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "build.sh"))
    sdkPath = config.SDKPath + '/'
    str = ""

    whitelist = None
    if featureFilters:
        executablePackages = LocateAllExecutables(packages, featureFilters, featureFilterBlacklist)
        whitelist = BuildPackageWhiteList(executablePackages)
    else:
        whitelist = packages

    totalPackages = 0
    topLevelPackage = LocateToplevelPackage(packages)
    for package in topLevelPackage.ResolvedBuildOrder:
        if package in whitelist:
            unsupportedPlatformList = package.GetUnsupportedPlatformList()
            if package.Type == PackageType.Library:
                totalPackages += 1
                if len(unsupportedPlatformList) > 0:
                    str += 'if %s; then\n' % GeneratePlatformCheckString(unsupportedPlatformList)
                    str += "  pushd " + config.ToBashPath(package.AbsolutePath) + " > /dev/null\n"
                    str += '  make "$@"\n'
                    str += "  popd > /dev/null\n"
                    str += "fi\n"
                else:
                    str += "pushd " + config.ToBashPath(package.AbsolutePath) + " > /dev/null\n"
                    str += 'make "$@"\n'
                    str += "popd > /dev/null\n"
            if package.Type == PackageType.Executable:
                totalPackages += 1
                if len(unsupportedPlatformList) > 0:
                    str += 'if %s; then\n' % GeneratePlatformCheckString(unsupportedPlatformList)
                    str += "  pushd " + config.ToBashPath(package.AbsolutePath) + " > /dev/null\n"
                    str += '  make "$@"\n'
                    str += "  popd > /dev/null\n"
                    str += "fi\n"
                else:
                    str += "pushd " + config.ToBashPath(package.AbsolutePath) + " > /dev/null\n"
                    str += 'make "$@"\n'
                    str += "popd > /dev/null\n"
    build = template
    build = build.replace("##PACKAGE_BUILD_COMMANDS##", str)
    dstFile = IOUtil.Join(sdkPath, dstFilename)
    if not config.DisableWrite:
        if totalPackages > 0:
            IOUtil.WriteFileIfChanged(dstFile, build)
            IOUtil.SetFileExecutable(dstFile)
        else:
            IOUtil.RemoveFile(dstFile)

def Contains(list, name):
    for entry in list:
        if entry.Name == name:
            return True
    return False


def GetConfigScript(featureFilters):
    str = "call %FSL_GRAPHICS_SDK%\.Config\ConfigureES2.bat\n"
    if not featureFilters:
        return str
    elif "OpenGLES2" in featureFilters:
        str = "call %FSL_GRAPHICS_SDK%\.Config\ConfigureES2.bat\n"
    elif "OpenGLES3" in featureFilters or "OpenGLES3.1" in featureFilters:
        str = "call %FSL_GRAPHICS_SDK%\.Config\ConfigureES3.bat\n"
    elif "OpenVG" in featureFilters:
        str = "call %FSL_GRAPHICS_SDK%\.Config\ConfigureVG.bat\n"
    return str

def GenerateBuildScriptAllWithFilterWindows(config, packages, featureFilters, featureFilterBlacklist, dstFilename):
    template = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "build.bat"))
    sdkPath = config.SDKPath + '/'
    str = ""
    str += GetConfigScript(featureFilters)

    whitelist = LocateAllExecutables(packages, featureFilters, featureFilterBlacklist)

    topLevelPackage = LocateToplevelPackage(packages)
    for package in topLevelPackage.ResolvedBuildOrder:
        if package in whitelist:
            unsupportedPlatformList = package.GetUnsupportedPlatformList()
            if package.Type == PackageType.Executable:
                if not Contains(unsupportedPlatformList, "Windows"):
                    str += "pushd " + config.ToDosPath(package.AbsolutePath) + " > nul\n"
                    str += 'msbuild /m %s.vcxproj\n' % (package.ShortName)
                    str += 'if %errorlevel% neq 0 exit /b %errorlevel%\n'
                    str += "popd > nul\n"
    build = template
    build = build.replace("##PACKAGE_BUILD_COMMANDS##", str)
    dstFile = IOUtil.Join(sdkPath, dstFilename)
    if not config.DisableWrite:
        IOUtil.WriteFileIfChanged(dstFile, build)


def GenerateBuildScriptAllWithFilter(config, packages, featureFilters, featureFilterBlacklist, dstFilename):
    GenerateBuildScriptAllWithFilterUnix(config, packages, featureFilters, featureFilterBlacklist, dstFilename + '.sh')
    GenerateBuildScriptAllWithFilterWindows(config, packages, featureFilters, featureFilterBlacklist, 'Win' + dstFilename + '.bat')


def GenerateSceeenshotScriptAllWithFilter(config, packages, featureFilters, featureFilterBlacklist, dstFilename):
    template = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "build.bat"))
    sdkPath = config.SDKPath + '/'
    str = "@echo off\n"
    str += GetConfigScript(featureFilters)

    whitelist = LocateAllExecutables(packages, featureFilters, featureFilterBlacklist)

    topLevelPackage = LocateToplevelPackage(packages)
    for package in topLevelPackage.ResolvedBuildOrder:
        if package in whitelist:
            unsupportedPlatformList = package.GetUnsupportedPlatformList()
            if package.Type == PackageType.Executable:
                if not Contains(unsupportedPlatformList, "Windows"):
                    print(package.AbsolutePath)
                    #str += "pushd " + config.ToDosPath(package.AbsolutePath) + " > nul\n"
                    #str += "call %%1 %s\n" % (package.ShortName)
                    #str += "if %errorlevel% neq 0 exit /b %errorlevel%\n"
                    #str += "popd > nul\n"
    #build = template
    #build = build.replace("##PACKAGE_BUILD_COMMANDS##", str)
    #dstFile = IOUtil.Join(sdkPath, dstFilename)
    #if not config.DisableWrite:
    #    IOUtil.WriteFileIfChanged(dstFile, build)


def Answer(config, packages, platformName):
    GenerateSceeenshotScriptAllWithFilter(config, packages, ["OpenGLES2"], None, ".Config/WinProcess_GLES2.bat")
#    GenerateSceeenshotScriptAllWithFilter(config, packages, ["OpenGLES3"], ["OpenGLES3.1"], ".Config/WinProcess_GLES3.bat")
#    GenerateSceeenshotScriptAllWithFilter(config, packages, ["OpenGLES3.1"], None, ".Config/WinProcess_GLES3_1.bat")
#    GenerateSceeenshotScriptAllWithFilter(config, packages, ["OpenVG"], None, ".Config/WinProcess_OpenVG.bat")
    #GenerateSceeenshotScriptAllWithFilter(config, packages, ["G2D"], None, ".Config/WinProcess_G2D.bat")
    pass
