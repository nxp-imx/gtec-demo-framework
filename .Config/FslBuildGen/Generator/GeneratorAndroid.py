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

import os
import os.path
import shutil
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.Generator import AndroidGeneratorUtil
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *


class GeneratorAndroid(object):
    def __init__(self, config, packages, platformName):
        super(GeneratorAndroid, self).__init__()

        appTemplateManager = AndroidGeneratorUtil.AppTemplateManager(config, "Android")

        self.LibTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "Android_lib.mk"))
        for package in packages:
            if not package.ResolvedPlatformNotSupported:
                if package.Type == PackageType.Library:
                    self.__GenerateLibraryBuildFile(config, package, platformName)
                elif package.Type == PackageType.Executable:
                    # For now we generate a library build file for android
                    self.__GenerateLibraryBuildFile(config, package, platformName)
                    self.__UpdateExeBuilder(config, package, platformName, appTemplateManager)
            elif package.Type == PackageType.Executable:
                config.LogPrint("WARNING: Package %s marked as not supported" % (package.Name))


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}


    # This is basically a workaround because the resolved data doesnt make it possible to determine if this is the first use of a include dir
    def __IncludeDirResolve(self, config, package, includeDirsPublic):
        allDirs = set(package.ResolvedBuildPublicIncludeDirs)
        for dir in includeDirsPublic:
            if dir in allDirs:
                allDirs.remove(dir)

        for dependency in package.ResolvedBuildOrder:
            if not dependency.IsVirtual and dependency != package:
                for dir in dependency.ResolvedBuildPublicIncludeDirs:
                    if dir in allDirs:
                        allDirs.remove(dir)
                absDir = config.ToPath(dependency.AbsoluteIncludePath)
                if absDir in allDirs:
                   allDirs.remove(absDir)
        return allDirs


    def __GenerateLibraryBuildFile(self, config, package, platformName):
        libName = package.ShortName
        files = MakeFileHelper.CreateList(package.ResolvedBuildSourceFiles)
        libraryDendencies = Util.ExtractNonVirtualNames(package.ResolvedDirectDependencies)

        externalLibraryDependencies = Util.ExtractNames(Util.FilterByType(package.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib))

        if package.Type == PackageType.Executable:
            # FIX: since adding this library causes a setup method to be removed during optimzation
            # But the android 'executable' support is experimental
           if "FslDemoPlatform" in libraryDendencies: libraryDendencies.remove("FslDemoPlatform")

        libraryNames = externalLibraryDependencies + libraryDendencies
        libraryNames = MakeFileHelper.CreateList(libraryNames)


        firstOnly = [entry for entry in package.ResolvedBuildAllPublicCPPDefines if entry.IsFirstActualUse]
        cppDefines = Util.ExtractNames(firstOnly)
        cppDefineNames = MakeFileHelper.CreateDefineList(cppDefines)

        firstOnly = [entry for entry in package.ResolvedBuildAllCPPDefines if entry.IsFirstActualUse]
        cppLocalDefines = Util.ExtractNames(firstOnly)
        cppLocalDefineNames = MakeFileHelper.CreateDefineList(cppLocalDefines)

        includeDirsPublic = []
        includeDirs = []

        for extDependency in package.ResolvedDirectExternalDependencies:
            if extDependency.Include != None:
                if extDependency.Access == AccessType.Public:
                    includeDirsPublic.append(extDependency.Include)
                    includeDirs.append(extDependency.Include)
                elif extDependency.Access == AccessType.Private:
                    includeDirs.append(extDependency.Include)
                else:
                    print("ERROR: Unknown access type")

        if package.AbsoluteIncludePath != None:
            includeDirsPublic.append('$(LOCAL_PATH)/include')
            includeDirs.append('$(LOCAL_PATH)/include')

        left =  self.__IncludeDirResolve(config, package, includeDirsPublic)
        includeDirsPublic += list(left)
        includeDirs += list(left)

        if len(includeDirsPublic) > 0:
            includeDirsPublic = ' ' + MakeFileHelper.CreateList2(includeDirsPublic)
        else:
            includeDirsPublic = ''
        if len(includeDirs) > 0:
            includeDirs = ' ' + MakeFileHelper.CreateList2(includeDirs)
        else:
            includeDirs = ''


        build = self.LibTemplate
        build = build.replace("##PACKAGE_TARGET_NAME##", libName)
        build = build.replace("##PACKAGE_SOURCE_FILES##", files)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES##", libraryNames)
        build = build.replace("##PACKAGE_PUBLIC_CPP_FLAGS##", cppDefineNames)
        build = build.replace("##PACKAGE_CPP_FLAGS##", cppLocalDefineNames)
        build = build.replace("##PACKAGE_PUBLIC_INCLUDE_DIRS##", includeDirsPublic)
        build = build.replace("##PACKAGE_INCLUDE_DIRS##", includeDirs)

        dstFile = IOUtil.Join(package.AbsolutePath, "Android.mk")
        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, build)


    def __UpdateExeBuilder(self, config, package, platformName, appTemplateManager):
        if config.DisableWrite:
            return

        dstPath = IOUtil.Join(package.AbsolutePath, "Android")
        dstPathJNI = IOUtil.Join(dstPath, "jni")

        # create folder structure
        appTemplateManager.CreateTemplate(dstPath, package)
        
        appPackageTemplateInfo = AndroidGeneratorUtil.AppPackageTemplateInfo(package)

        packagesByLocation = self.__OrderDependenciesByLocation(package.ResolvedAllDependencies)
        packageLibDeps = self.__BuildLibDeps(package);

        depString = ""
        for entry in packagesByLocation:
            depString += "\n$(call import-add-path,%s)\n" % (entry.Location.Name)
            for depPackage in entry.Packages:
                depString += "$(call import-module,%s)\n" % (depPackage.ShortName)

        packageAppABI = self.__DetermineAppABI(package)
        allExternStaticLibs = self.__DetermineAppExternalStaticLibs(package)

        for file in appTemplateManager.FilesToModify:
            dst = IOUtil.Join(dstPath, file.RelativeDestPath)
            content = IOUtil.ReadFile(file.AbsoluteSourcePath)
            content = content.replace("##FSL_PACKAGE_GLES_VERSION##", appPackageTemplateInfo.MinGLESVersion)
            content = content.replace("##FSL_PACKAGE_MIN_ANDROID_SDK_VERSION##", appPackageTemplateInfo.MinSDKVersion.VersionString)
            content = content.replace("##FSL_PACKAGE_TARGET_ANDROID_SDK_VERSION##", appPackageTemplateInfo.TargetSDKVersion.VersionString)
            content = content.replace("##PACKAGE_APP_PLATFORM##", appPackageTemplateInfo.MinSDKVersion.AppPlatform)
            content = content.replace("##PACKAGE_DEPENDENCY_IMPORT##", depString)
            content = content.replace("##PACKAGE_LOCATION##", package.PackageLocation.Name)
            content = content.replace("##PACKAGE_LIBRARY_DEPENDENCIES##", packageLibDeps)
            content = content.replace("##PACKAGE_TARGET_NAME##", package.ShortName)
            content = content.replace("##PREFIXED_PROJECT_NAME##", appPackageTemplateInfo.PrefixedProjectName)
            content = content.replace("##PREFIXED_PROJECT_NAME_L##", appPackageTemplateInfo.PrefixedProjectName.lower())
            content = content.replace("##PACKAGE_APP_ABI##", packageAppABI)
            content = content.replace("##ALL_EXTERNAL_STATIC_LIBS##", allExternStaticLibs)

            dst = appPackageTemplateInfo.UpdateFileName(dst)
            dirName = IOUtil.GetDirectoryName(dst)
            IOUtil.SafeMakeDirs(dirName)
            IOUtil.WriteFileIfChanged(dst, content)


    def __BuildLibDeps(self, package):
        libraryDendencies = Util.ExtractNonVirtualNames(package.ResolvedAllDependencies)
        externalLibraryDependencies = Util.ExtractNames(Util.FilterByType(package.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib))
        libraryNames = externalLibraryDependencies + libraryDendencies
        return MakeFileHelper.CreateList(libraryNames)

    def __OrderDependenciesByLocation(self, dependencies):
        dict = {}
        for dep in dependencies:
            if not dep.Package.IsVirtual:
                if not dep.Package.PackageLocation.Name in dict:
                    record = AndroidGeneratorUtil.PackageDepByLocation(dep.Package.PackageLocation);
                    dict[dep.Package.PackageLocation.Name] = record
                else:
                    record = dict[dep.Package.PackageLocation.Name]
                record.Packages.append(dep.Package)
        return dict.values()




    def __DetermineAppABI(self, package):
#        namesOnly = [entry.Name for entry in package.ResolvedAllUsedFeatures]
#        if "Vulkan" in namesOnly:
#            return "armeabi-v7a"
#        else:
#            return "all"
        return "all"

    def __DetermineAppExternalStaticLibs(self, package):
        #self.__FindAllExternalStaticLibs(package)
        namesOnly = [entry.Name for entry in package.ResolvedAllUsedFeatures]
        if "Vulkan" in namesOnly:
            return "-lvulkan"
        else:
            return ""

    def __FindAllExternalStaticLibs(self, package):
        dict = {}
        for dependency in package.ResolvedAllDependencies:
            for externalDep in dependency.Package.ResolvedDirectExternalDependencies:
                if externalDep.Type == ExternalDependencyType.StaticLib:
                    pass

