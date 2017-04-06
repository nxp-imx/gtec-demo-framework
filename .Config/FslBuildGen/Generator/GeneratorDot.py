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
from FslBuildGen import IOUtil, Util
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from subprocess import call

class GeneratorDot(object):
    def __init__(self, config, packages, platformName):
        super(GeneratorDot, self).__init__()

        self.UseVariantNames = True
        self.RequireExecutable = True
        self.ShowExternals = False

        packageList = packages
        if self.RequireExecutable:
            packageList = self.FilterBasedOnExecutableDemand(packages)
            if len(packageList) <= 0:
                packageList = packages

        #dotFile = self.CreateDirectDependencies(config, packageList, platformName);
        #dotFile = self.CreateAllDependencies(config, packageList, platformName);
        #dotFile = self.CreateSimpleDependencies(config, packageList, platformName);
        dotFile = self.CreateSimpleDependencies2(config, packageList, platformName);

        content = "\n".join(dotFile)

        tmpFile = "TmpDependencies_%s.dot" % (platformName)
        outputFile = "Dependencies_%s.png" % (platformName)

        IOUtil.WriteFile(tmpFile, content)

        # "dot -Tpng -o test.png Dependencies_Yocto.dot"
        try:
            call(["dot", "-Tpng", "-o%s" % outputFile, tmpFile])
            os.remove(tmpFile)
        except Exception as ex:
            print("WARNING: Failed to execute dot, is it part of the path?");
            os.remove(tmpFile)
            raise


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}


    def CreateDirectDependencies(self, config, packages, platformName):
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        laterPrivate = []
        laterLink = []

        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep in package.ResolvedDirectDependencies:
                    if dep.Access == AccessType.Link:
                        laterLink.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                    elif dep.Access == AccessType.Private:
                        laterPrivate.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                    else:
                        dotFile.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                if self.ShowExternals:
                    for dep in package.ResolvedDirectExternalDependencies:
                        dotFile.append('  "%s" -> "%s"' % (self.GetName(package), dep.Name))

        if len(laterPrivate):
            dotFile.append('  edge [color="#2F4F4F", style=dashed]')
            for entry in laterPrivate:
                dotFile.append(entry)

        if len(laterLink):
            dotFile.append('  edge [color=Blue, style=dotted]')
            for entry in laterLink:
                dotFile.append(entry)

        dotFile.append('}')
        return dotFile;


    def CreateAllDependencies(self, config, packages, platformName):
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        laterPrivate = []
        laterLink = []

        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep in package.ResolvedAllDependencies:
                    if dep.Access == AccessType.Link:
                        laterLink.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                    elif dep.Access == AccessType.Private:
                        laterPrivate.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                    else:
                        dotFile.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                if self.ShowExternals:
                    for dep in package.ResolvedDirectExternalDependencies:
                        dotFile.append('  "%s" -> "%s"' % (self.GetName(package), dep.Name))

        if len(laterPrivate):
            dotFile.append('  edge [color="#2F4F4F", style=dashed]')
            for entry in laterPrivate:
                dotFile.append(entry)

        if len(laterLink):
            dotFile.append('  edge [color=Blue, style=dotted]')
            for entry in laterLink:
                dotFile.append(entry)

        dotFile.append('}')
        return dotFile;


    def CreateSimpleDependencies(self, config, packages, platformName):
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep in package.ResolvedAllDependencies:
                    if not self.IsAvailableFromDependentPackage(dep, package):
                        dotFile.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                if self.ShowExternals:
                    for dep in package.ResolvedDirectExternalDependencies:
                        if not self.IsExternalAvailableFromDependentPackage(dep, package.ResolvedDirectDependencies):
                            dotFile.append('  "%s" -> "ext: %s"' % (self.GetName(package), dep.Name))

        dotFile.append('}')
        return dotFile;

    def IsAvailableFromDependentPackage(self, dep, package):
        for entry in package.ResolvedAllDependencies:
            if entry.Name != dep.Name:
                if self.ExistIn(entry.Package.ResolvedAllDependencies, dep.Name):
                    return True;
        return False;


    def IsExternalAvailableFromDependentPackage(self, dep, otherDeps):
        for entry in otherDeps:
            if self.ExistIn(entry.Package.ResolvedDirectExternalDependencies, dep.Name):
                return True;
        return False;


    def ExistIn(self, dependencies, name):
        for dep in dependencies:
            if dep.Name == name:
                return True;
        return False;

    def CreateSimpleDependencies2(self, config, packages, platformName):
        dotFile = []
        dotFile.append('digraph xmlTest')
        dotFile.append('{')
        dotFile.append('  overlap=scale;')
        dotFile.append('  splines=true;')
        dotFile.append('  edge [len=1];')

        laterPrivate = []
        laterLink = []

        for package in packages:
            if not package.Name.startswith('SYS_'):
                for dep in package.ResolvedDirectDependencies:
                    if dep.Access == AccessType.Private:
                        laterPrivate.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                    elif dep.Access == AccessType.Link:
                        laterLink.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                    else:
                        if not self.IsAvailableFromDependentPackage2(dep, package):
                            dotFile.append('  "%s" -> "%s"' % (self.GetName(package), self.GetName(dep.Package)))
                if self.ShowExternals:
                    for dep in package.ResolvedDirectExternalDependencies:
                        if not self.IsExternalAvailableFromDependentPackage(dep, package.ResolvedDirectDependencies):
                            dotFile.append('  "%s" -> "ext: %s"' % (self.GetName(package), dep.Name))

        if len(laterPrivate):
            dotFile.append('  edge [color="#2F4F4F", style=dashed]')
            for entry in laterPrivate:
                dotFile.append(entry)

        if len(laterLink):
            dotFile.append('  edge [color=Blue, style=dotted]')
            for entry in laterLink:
                dotFile.append(entry)

        dotFile.append('}')
        return dotFile;


    def IsAvailableFromDependentPackage2(self, dep, package):
        for entry in package.ResolvedAllDependencies:
            if entry.Name != dep.Name and entry.Access == AccessType.Public:
                if self.ExistIn(entry.Package.ResolvedAllDependencies, dep.Name):
                    return True;
        return False;


    def ExistIn(self, dependencies, name):
        for dep in dependencies:
            if dep.Name == name and dep.Access == AccessType.Public:
                return True;
        return False;

    def GetName(self, package):
      if self.UseVariantNames:
        return package.Name + package.ResolvedVariantName
      return Package.Name

    def FilterBasedOnExecutableDemand(self, packages):
        executablePackages = []
        for package in packages:
         if package.Type == PackageType.Executable:
             executablePackages.append(package)

        packageList = [] + executablePackages
        interestingPackages = set()
        for package in executablePackages:
            for dep in package.ResolvedAllDependencies:
                if not dep.Name in interestingPackages:
                    interestingPackages.add(dep.Name)
                    packageList.append(dep.Package)
        return packageList


