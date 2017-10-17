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
from typing import Tuple
import os
import os.path
from FslBuildGen import IOUtil
from FslBuildGen import MakeFileHelper
from FslBuildGen import Util
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import InternalErrorException
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Generator.GeneratorBase import GeneratorBase

# FIX:
# - This generator does not support Variants

class GeneratorQNXmakefile(GeneratorBase):
    def __init__(self, config: Config, packages: List[Package], dstMakeFilename: str, templateExe: str, templateLib: str) -> None:
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
            #elif package.Type == PackageType.Executable:
            #    config.LogPrint("WARNING: Package %s marked as not supported" % (package.Name))


    def __GenerateFolderStructure(self, config: Config, package: Package) -> None:
        #don't generate anything for unsuported packages
        if package.ResolvedPlatformNotSupported:
            return
        if (package.AbsolutePath is None or package.ResolvedBuildPath is None or
            package.ResolvedBuildAllIncludeDirs is None or
            package.ResolvedBuildSourceFiles is None or package.ShortName is None):
           raise Exception("Invalid package")

        # Use a standard build path
        buildBasePath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        newDir = "arm"
        relativePathBuildArm = IOUtil.Join(package.ResolvedBuildPath, newDir)
        buildArmPath = IOUtil.Join(buildBasePath, newDir)
        buildPath = buildBasePath
        #relativePathBuild = ""

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
            raise InternalErrorException("Unknown package type: {0}".format(package.Name))

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
        for includeDir in package.ResolvedBuildAllIncludeDirs:
            if not os.path.dirname(includeDir):
                updatedIncludedirs.append("$(project_root)/" + includeDir)
            else:
                updatedIncludedirs.append(includeDir)
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

        sl = self.__GetStaticLibs(package)
        content = content.replace("##STATIC_LIBS##", sl)

        name = package.ShortName if package.Type == PackageType.Executable else package.Name
        content = content.replace("##PINFO_DESC##", name)
        content = content.replace("##NAME##", name)

        # Local CPP defines
        localDefines = Util.ExtractNames(package.ResolvedBuildAllPrivateDefines)
        localDefines += Util.ExtractNames(package.ResolvedBuildAllPublicDefines)
        localDefineNames = MakeFileHelper.CreateList(localDefines)

        content = content.replace("##PACKAGE_DEFINES##", localDefineNames)

        #if package.Type == PackageType.Executable:
        #libraryDependencies = self.__GetLibraryDependencies(config, package)
        #libraryDependencies = MakeFileHelper.CreateList(libraryDependencies)
        #build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES##", libraryDependencies)

        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, content)

        #remove this if statement if build scripts are required also for libraries
        if package.Type == PackageType.Executable:
            self.__GenerateBuildScript(config, package, self.BldTemplate, buildBasePath)


    def __ExtractUniqueDirectoriesFromList(self, fileList: List[str]) -> List[str]:
        dirList = []  # type: List[str]
        if len(fileList) > 0:
            for entry in fileList:
                dirName = os.path.dirname(entry)
                if dirName not in dirList:
                    dirList.append(dirName)

        return dirList


    def __GetStaticLibs(self, package: Package) -> str:
        """ returns one-line string with the list of dependency packages """
        libs = ""
        buildOrder = list(package.ResolvedBuildOrder)
        #buildOrder.reverse() # GCC apparently needs the list to be in reverse order
        for entry in buildOrder:
            if entry.Type == PackageType.Library:
                if entry.Name != package.Name: #TODO FIXME HACK ??
                    libs = entry.Name + " " + libs
        return libs


    def __GetStaticLibsPaths(self, config: Config, package: Package, relativePathBuildLib: str) -> List[str]:
        """ return list of unique directories for dependencies. folders are SDK-root based """
        libDirs = []  # type: List[str]
        for entry in package.ResolvedBuildOrder:
            if entry.Type == PackageType.Library:
                if entry.AbsolutePath != package.AbsolutePath and entry.AbsolutePath is not None: #skip package own directory
                    # FIX: for proper variant support use "package.ResolvedMakeObjectPath" instead of the line below
                    fullPath = IOUtil.Join(entry.AbsolutePath, relativePathBuildLib)
                    fullPathSDK = config.ToPath(fullPath) # convert to SDK-based path
                    if fullPathSDK not in libDirs: #append only unique paths
                        libDirs.append(fullPathSDK)
        return libDirs


    def __FormatListToString(self, linePrefix: str, incList: List[str]) -> str:
        listStr = ""
        if len(incList) > 0:
            for inc in incList:
                listStr = listStr + linePrefix + inc + "\n"
        return listStr


    def __GenerateBuildScript(self, config: Config, package: Package, template: str, buildBasePath: str) -> None:
        if package.AbsolutePath is None or package.ResolvedBuildPath is None:
            raise Exception("Invalid package")

        strContent = ""
        for depPackage in package.ResolvedBuildOrder:
            if not depPackage.IsVirtual:
                if depPackage.AbsolutePath is None or depPackage.ResolvedBuildPath is None:
                    raise Exception("Invalid package")
                path = IOUtil.Join(depPackage.AbsolutePath, depPackage.ResolvedBuildPath)
                path = config.ToBashPath(path)
                strContent += "pushd " + path + " > /dev/null\n"
                strContent += 'make "$@"\n'
                strContent += "popd > /dev/null\n"
        path = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        path = config.ToBashPath(path)
        strContent += 'pushd '+ path +' > /dev/null\n'
        strContent += 'make "$@"\n'
        strContent += "popd > /dev/null\n"

        build = template
        build = build.replace("##PACKAGE_BUILD_COMMANDS##", strContent)


        # This file has been superseded by the 'FslBuild.py' script
        # so for now we just write it inside the build dir to keep it around if needed
        dstFile = IOUtil.Join(buildBasePath, "build_qnx.sh")
        #dstFile = IOUtil.Join(package.AbsolutePath, "build_qnx.sh")

        if not config.DisableWrite:
            IOUtil.WriteFileIfChanged(dstFile, build)
            IOUtil.SetFileExecutable(dstFile)

    def __GetExternalLibraryDependencies(self, package: Package) -> List[Tuple[str, str]]:
        libsAndPaths = []  # type: List[Tuple[str, str]]
        buildOrder = list(package.ResolvedBuildOrder)
        for entry in buildOrder:
            depList = Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
            for dep in depList:
                libName = dep.Name
                libPath = dep.Location
                libsAndPaths.append((libName, libPath))

        return libsAndPaths
