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
from FslBuildGen import IOUtil, MakeFileHelper, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *

def Contains(list, name):
    for entry in list:
        if entry.Name == name:
            return True
    return False

# FIX:
# - This generator does not support Variants

class GeneratorQNXmakefile(object):
    def __init__(self, config, packages, dstMakeFilename, templateExe, templateLib):
        super(GeneratorQNXmakefile, self).__init__()
        self.BldTemplate = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "build.sh"))
        self.TemplateMakefileCPU = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "QNXmakefileCPU"))
        self.TemplateMakefileVariant = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "QNXmakefileVariant"))
        self.TemplateCommonMk = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "QNXcommonmk"))
        self.TemplateMakefileTop = IOUtil.ReadFile(IOUtil.Join(config.SDKConfigTemplatePath, "QNXmakefileTop"))
        for package in packages:
            if not package.ResolvedPlatformNotSupported:
                if package.Type == PackageType.Executable or package.Type == PackageType.Library:
                    self.__GenerateFolderStructure(config, package)
            elif package.Type == PackageType.Executable:
                config.LogPrint("WARNING: Package %s marked as not supported" % (package.Name))


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}


    def __GenerateFolderStructure(self, config, package):
        #don't generate anything for unsuported packages
        unsupportedPlatformList =  package.GetUnsupportedPlatformList()
        if Contains(unsupportedPlatformList, "QNX"):
            return

        # Use a standard build path
        buildBasePath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        newDir = "arm"
        relativePathBuildArm = IOUtil.Join(package.ResolvedBuildPath, newDir)
        buildArmPath = IOUtil.Join(buildBasePath, newDir)
        buildPath = buildBasePath
        relativePathBuild = ""

        newDir = "a-le-v7"
        relativePathBuildLib = IOUtil.Join(relativePathBuildArm, newDir)
        buildPathLib = IOUtil.Join(buildArmPath, newDir)

        #check package type and prepare folder name
        if package.Type == PackageType.Library:
            relativePathBuild = relativePathBuildLib
            buildPath = buildPathLib
        elif package.Type == PackageType.Executable:
            newDir = "o-le-v7"
            relativePathBuild = IOUtil.Join(relativePathBuildArm, newDir)
            buildPath = IOUtil.Join(buildArmPath, newDir)
        else:
            raise InternalErrorException("Unknown package type: " % (package.Name));

        if not config.DisableWrite:
            # create folder structure
            IOUtil.SafeMakeDirs(buildPath)

            #copy make files that are not modified
            IOUtil.CopySmallFile(IOUtil.Join(config.SDKConfigTemplatePath, ".qnx_internal.mk"), IOUtil.Join(buildBasePath, ".qnx_internal.mk"))

            dstFile = IOUtil.Join(buildBasePath, "Makefile")
            content = self.TemplateMakefileCPU
            IOUtil.WriteFileIfChanged(dstFile, content)

            dstFile = IOUtil.Join(buildArmPath, "Makefile")
            content = self.TemplateMakefileVariant
            IOUtil.WriteFileIfChanged(dstFile, content)

            dstFile = IOUtil.Join(buildPath, "Makefile")
            content = self.TemplateMakefileTop
            IOUtil.WriteFileIfChanged(dstFile, content)

        #create common.mk with package-specific content
        dstFile = IOUtil.Join(buildBasePath, "common.mk")
        content = self.TemplateCommonMk

        #add "$(project_root)/" to local includes like "include", keep others as they are ("$(FSL_GRAPHICS_SDK)/a/b/")
        updatedIncludedirs = []
        for dir in package.ResolvedBuildAllIncludeDirs:
            if not os.path.dirname(dir):
                updatedIncludedirs.append("$(project_root)/" + dir)
            else:
                updatedIncludedirs.append(dir)
        includes = self.__FormatListToString("EXTRA_INCVPATH += ", updatedIncludedirs)
        content = content.replace("##EXTRA_INCVPATHS##", includes)

        srcDirsList = self.__ExtractUniqueDirectoriesFromList(package.ResolvedBuildSourceFiles)
        srcDirs = self.__FormatListToString("EXTRA_SRCVPATH += $(project_root)/", srcDirsList)
        content = content.replace("##EXTRA_SRCVPATHS##", srcDirs)

        # QNX takes all sources in folder, just like this generator, so there is no need to
        # list all files in makefile.
        #sourceObjs = []
        #for sourceFile in package.ResolvedBuildSourceFiles:
        #    sourceObjs.append(os.path.splitext(sourceFile)[0] + ".o")
        #sourceObjsString = self.__FormatListToString("SOURCE_OBJECTS += $(project_root)/", sourceObjs)
        #content = content.replace("##EXTRA_SOURCE_OBJS##", sourceObjsString)
        content = content.replace("##EXTRA_SOURCE_OBJS##", "")

        libDirsList = self.__GetStaticLibsPaths(config, package, relativePathBuildLib)
        extLibraryDependencies = self.__GetExternalLibraryDependencies(package)
        libsExtern = ""
        for lib in extLibraryDependencies:
            libsExtern = libsExtern + lib[0] + " "
            if lib[1] is not None and lib[1] not in libDirsList:
                libDirsList.append(lib[1])
        if libsExtern:
            libsExtern = "LIBS += " + libsExtern + "\n"
        content = content.replace("##PACKAGE_EXTERNAL_LIBRARY_DEPENDENCIES##", libsExtern)

        libDirs = self.__FormatListToString("EXTRA_LIBVPATH += ", libDirsList)
        content = content.replace("##EXTRA_LIBVPATHS##", libDirs)

        sl = self.__GetStaticLibs(package);
        content = content.replace("##STATIC_LIBS##", sl)

        name = package.ShortName if package.Type == PackageType.Executable else package.Name
        content = content.replace("##PINFO_DESC##", name)
        content = content.replace("##NAME##", name)

        # Local CPP defines
        cppLocalDefines = Util.ExtractNames(package.ResolvedBuildAllPrivateCPPDefines)
        cppLocalDefines += Util.ExtractNames(package.ResolvedBuildAllPublicCPPDefines)
        cppLocalDefineNames = MakeFileHelper.CreateList(cppLocalDefines)

        content = content.replace("##PACKAGE_CPP_FLAGS##", cppLocalDefineNames)

        #if package.Type == PackageType.Executable:
        #libraryDependencies = self.__GetLibraryDependencies(config, package)
        #libraryDependencies = MakeFileHelper.CreateList(libraryDependencies)
        #build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES##", libraryDependencies)

        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, content)

        #remove this if statement if build scripts are required also for libraries
        if package.Type == PackageType.Executable:
            self.__GenerateBuildScript(config, package, self.BldTemplate, buildBasePath)

    def __ExtractUniqueDirectoriesFromList(self, fileList):
        dirList = []
        if len(fileList) > 0:
            for entry in fileList:
                dir = os.path.dirname(entry)
                if dir not in dirList:
                    dirList.append(dir)

        return dirList

    #returns one-line string with the list of dependency packages
    def __GetStaticLibs(self, package):
        libs = ""
        buildOrder = list(package.ResolvedBuildOrder)
        #buildOrder.reverse() # GCC apparently needs the list to be in reverse order
        for entry in buildOrder:
            if entry.Type == PackageType.Library:
                if entry.Name <> package.Name: #TODO FIXME HACK ??
                    libs = entry.Name + " " + libs
        return libs

    #return list of unique directories for dependencies. folders are SDK-root based
    def __GetStaticLibsPaths(self, config, package, relativePathBuildLib):
        libDirs = []
        for entry in package.ResolvedBuildOrder:
            if entry.Type == PackageType.Library:
                if entry.AbsolutePath <> package.AbsolutePath: #skip package own directory
                    # FIX: for proper variant support use "package.ResolvedMakeObjectPath" instead of the line below
                    fullPath =  IOUtil.Join(entry.AbsolutePath, relativePathBuildLib)
                    fullPathSDK = config.ToPath(fullPath) # convert to SDK-based path
                    if fullPathSDK not in libDirs: #append only unique paths
                        libDirs.append(fullPathSDK)
        return libDirs

    def __FormatListToString(self, linePrefix, incList):
        listStr = ""
        if len(incList) > 0:
            for inc in incList:
                listStr = listStr + linePrefix + inc + "\n"
        return listStr

    def __GenerateBuildScript(self, config, package, template, buildBasePath):
        str = ""
        for depPackage in package.ResolvedBuildOrder:
            if not depPackage.IsVirtual:
                path = IOUtil.Join(depPackage.AbsolutePath, depPackage.ResolvedBuildPath)
                path = config.ToBashPath(path)
                str += "pushd " + path + " > /dev/null\n"
                str += 'make "$@"\n'
                str += "popd > /dev/null\n"
        path = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        path = config.ToBashPath(path)
        str += 'pushd '+ path +' > /dev/null\n'
        str += 'make "$@"\n'
        str += "popd > /dev/null\n"

        build = template
        build = build.replace("##PACKAGE_BUILD_COMMANDS##", str)


        # This file has been superseded by the 'FslBuild.py' script
        # so for now we just write it inside the build dir to keep it around if needed
        dstFile = IOUtil.Join(buildBasePath, "build_qnx.sh")
        #dstFile = IOUtil.Join(package.AbsolutePath, "build_qnx.sh")

        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, build)
            IOUtil.SetFileExecutable(dstFile)

    def __GetExternalLibraryDependencies(self, package):
        libsAndPaths = []
        buildOrder = list(package.ResolvedBuildOrder)
        for entry in buildOrder:
            depList = Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
            for dep in depList:
                libName = dep.Name
                libPath = dep.Location
                libsAndPaths.append([libName, libPath])

        return libsAndPaths


    def __ExtractInclude(self, entries):
        list = []
        for entry in entries:
            if entry.Include != None:
                list.append(entry.Include)
        return list
