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
from FslBuildGen.PackageBuilder import PackageBuilder
from FslBuildGen.DependencyGraph import DependencyGraph
from FslBuildGen.Exceptions import *
from FslBuildGen import IOUtil, Util, PackageConfig
from FslBuildGen.PackagePlatformVariant import PackagePlatformVariant, PackageCPPDefine, PackageFeature
from FslBuildGen.XmlStuff import XmlGenFile


class PackageResolvedInclude(object):
    def __init__(self, path, fromPackageAccess):
        super(PackageResolvedInclude, self).__init__()
        self.Path = path;
        # the access to the package this was received from
        self.FromPackageAccess = fromPackageAccess


class PackageResolver(object):
    def __init__(self, config, platformName, genFiles, markExternalLibFirstUse):
        super(PackageResolver, self).__init__()
        config.LogPrint("- Processing platform: %s" % (platformName))
        # Do the initial dependency checks
        packageBuilder = PackageBuilder(config, platformName, genFiles)
        packages = packageBuilder.AllPackages

        config.LogPrint("  Resolving")
        # FIX:
        # If we resolve the packages using the top level package build order we can do everything a lot smarter
        # than we currently do
        finalResolveOrder = packageBuilder.TopLevelPackage.ResolvedBuildOrder
        #config.LogPrint("Order: " + ", ".join(Util.ExtractNames(finalResolveOrder)))

        self.__ResolvePlatform(platformName, packages, config.IgnoreNotSupported)
        self.__ResolveDirectExternalDependencies(platformName, packages)
        self.__ResolveDirectVariants(finalResolveOrder, platformName)
        self.__CheckVariants(config, finalResolveOrder)
        self.__ResolveAllVariants(finalResolveOrder)
        if markExternalLibFirstUse:
            self.__MarkExternalLibFirstUse(packageBuilder.TopLevelPackage)
        # Everything checks out, so we can now start resolving files for the packages
        self.__ResolveBuildIncludeDirs(config, finalResolveOrder)
        self.__ResolveBuildSourceFiles(packages)
        self.__ResolveBuildIncludeFiles(packages)
        self.__ResolveBuildContentFiles(packages)
        self.__ResolveBuildCPPDefines(config, platformName, finalResolveOrder)
        self.__ResolveBuildUsedFeatures(config, platformName, finalResolveOrder)
        self.__ResolveBuildPlatformNotSupported(config, platformName, finalResolveOrder)
        self.__ResolveMakeConfig(config, packages)
        self.Packages = packages


    def __ResolveMakeConfig(self, config, packages):
        for package in packages:
            normalVariants = []
            virtualVariants = []
            for entry in package.ResolvedAllVariantDict.values():
                if entry.Type == VariantType.Normal:
                    normalVariants.append(entry)
                elif entry.Type == VariantType.Virtual:
                    virtualVariants.append(entry)
                else:
                    raise NotImplementedError("unknown variant type");

            normalVariantNames = Util.ExtractNamesAsMakeEnvironmentVariables(normalVariants)
            virtualVariantNames = Util.ExtractNamesAsMakeEnvironmentVariables(virtualVariants)
            if len(normalVariantNames) > 0 or len(virtualVariantNames) > 0:
                normalVariantNames.sort(key=lambda s: s.lower())
                virtualVariantNames.sort(key=lambda s: s.lower())
                strNormal = "_".join(normalVariantNames)
                strVirtual = "_".join(virtualVariantNames)
                package.ResolvedNormalVariantName = "_" + strNormal if len(strNormal) > 0 else ""
                package.ResolvedVirtualVariantName = "_" + strVirtual if len(strVirtual) > 0 else ""
                package.ResolvedVariantName = package.ResolvedNormalVariantName + package.ResolvedVirtualVariantName
                package.ResolvedMakeConfigName = "%s%s" % (package.ResolvedPlatformName, package.ResolvedVariantName)
            else:
                package.ResolvedMakeConfigName = package.ResolvedPlatformName
                package.ResolvedVariantName = ""
                package.ResolvedNormalVariantName = ""
                package.ResolvedVirtualVariantName = ""
            package.ResolvedBuildPath = "%s/%s" % (config.GetBuildDir(), package.ResolvedPlatformName)
            package.ResolvedMakeObjectPath = "%s/obj%s/$(config)" % (package.ResolvedBuildPath, package.ResolvedVariantName)

    def __GetExtensions(self, packageLanguage):
        if packageLanguage == PackageLanguage.CPP:
            return (".cpp", ".c")
        elif packageLanguage == PackageLanguage.CSharp:
            return (".cs")
        else:
            raise UnsupportedException("Unsupported package language {0}".format(PackageLanguage.ToString(packageLanguage)));


    def __ResolveBuildSourceFiles(self, packages):
        for package in packages:
            if not package.IsVirtual:
                startIdx = len(package.AbsolutePath) + 1
                languageSourceExtensions = self.__GetExtensions(package.PackageLanguage)
                files = IOUtil.GetFilePaths(package.AbsoluteSourcePath, languageSourceExtensions)
                files = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in files]
                files.sort(key=lambda s: s.lower())
                package.ResolvedBuildSourceFiles = files
            else:
                package.ResolvedBuildSourceFiles = []


    def __ResolveBuildIncludeFiles(self, packages):
        for package in packages:
            if not package.IsVirtual or package.Type == PackageType.HeaderLibrary:
                startIdx = len(package.AbsolutePath) + 1
                filesPub = IOUtil.GetFilePaths(package.AbsoluteIncludePath, (".hpp", ".h", ".inl")) if package.AbsoluteIncludePath != None else []
                filesAll = list(filesPub)
                if package.AbsoluteSourcePath:
                    filesAll += IOUtil.GetFilePaths(package.AbsoluteSourcePath, (".hpp", ".h", ".inl"))
                filesPub = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in filesPub]
                filesAll = [Util.UTF8ToAscii(file[startIdx:].replace("\\", "/")) for file in filesAll]
                filesPub.sort(key=lambda s: s.lower())
                filesAll.sort(key=lambda s: s.lower())
                package.ResolvedBuildPublicIncludeFiles = filesPub
                package.ResolvedBuildAllIncludeFiles = filesAll
            else:
                package.ResolvedBuildPublicIncludeFiles = []
                package.ResolvedBuildAllIncludeFiles = []


    def __ResolveBuildContentFiles(self, packages):
        for package in packages:
            if not package.IsVirtual:
                package.ResolvedBuildContentFiles = self.__ResolveFiles(package.AbsolutePath, package.AbsoluteContentPath, None)
                package.ResolvedBuildContentSourceFiles = self.__ResolveFiles(package.AbsolutePath, package.AbsoluteContentSourcePath, None)
            else:
                package.ResolvedBuildContentFiles = []
                package.ResolvedBuildContentSourceFiles = []


    def __ResolveFiles(self, absPath, path, filters):
        startIdx = len(absPath) + 1
        files = IOUtil.GetFilePaths(path,filters)
        files = [file[startIdx:].replace("\\", "/") for file in files]
        files.sort(key=lambda s: s.lower())
        return files


    def __AddBuildIncludeDir(self, dir, currentAccess, fromPackageAccess, includeDirs, privateIncludeDirs, publicIncludeDirs):
        includeDirs[dir] = PackageResolvedInclude(dir, fromPackageAccess)
        if currentAccess == AccessType.Private:
            privateIncludeDirs.append(dir)
        else:
            publicIncludeDirs.append(dir)


    def __RemoveBuildIncludeDir(self, resolvedDir, includeDirs, privateIncludeDirs, publicIncludeDirs):
        includeDirs.pop(resolvedDir.Path, None)
        if resolvedDir.Path in privateIncludeDirs:
            privateIncludeDirs.remove(resolvedDir.Path)
        if resolvedDir.Path in publicIncludeDirs:
            publicIncludeDirs.remove(resolvedDir.Path)


    def __ResolveBuildIncludeDirs(self, config, finalResolveOrder):
        for package in finalResolveOrder:
            hasLocalIncludeDir = False
            includeDirs = {}
            publicIncludeDirs = []
            privateIncludeDirs = []

            # First process the include paths present in this package
            if package.AbsoluteIncludePath != None:
                hasLocalIncludeDir = True
            for extDependency in package.ResolvedDirectExternalDependencies:
                if extDependency.Include != None:
                    if extDependency.Include in includeDirs:
                        raise Exception("External include dir defined multiple times: '%s'" % (extDependency.Include));
                    self.__AddBuildIncludeDir(extDependency.Include, extDependency.Access, AccessType.Public, includeDirs, privateIncludeDirs, publicIncludeDirs)

            # Then pull in the dependencies from the packages we depend upon
            # here we take advantage of the fact that all packages we are dependent upon
            # have been resolved.
            for dep in package.ResolvedDirectDependencies:
                if dep.Access != AccessType.Link:
                    for dir in dep.Package.ResolvedBuildPublicIncludeDirs:
                        if dir == dep.Package.BaseIncludePath:
                            dir = self.__ExtractIncludePath(config, dep.Package)
                        if not dir in includeDirs:
                            self.__AddBuildIncludeDir(dir, dep.Access, dep.Access, includeDirs, privateIncludeDirs, publicIncludeDirs)
                        elif dep.Access < includeDirs[dir].FromPackageAccess:
                            self.__RemoveBuildIncludeDir(includeDirs[dir], includeDirs, privateIncludeDirs, publicIncludeDirs)
                            self.__AddBuildIncludeDir(dir, dep.Access, dep.Access, includeDirs, privateIncludeDirs, publicIncludeDirs)

            allIncludeDirs = includeDirs.keys()
            allIncludeDirs.sort(key=lambda s: s.lower())
            publicIncludeDirs.sort(key=lambda s: s.lower())
            privateIncludeDirs.sort(key=lambda s: s.lower())
            if hasLocalIncludeDir:
                allIncludeDirs.insert(0, package.BaseIncludePath)
                publicIncludeDirs.insert(0, package.BaseIncludePath)
            package.ResolvedBuildPublicIncludeDirs = publicIncludeDirs
            package.ResolvedBuildPrivateIncludeDirs = privateIncludeDirs
            package.ResolvedBuildAllIncludeDirs = allIncludeDirs


    def __ResolveAddCPPDefine(self, package, processedCPPDefine, allCPPDefines, publicCPPDefines, privateCPPDefines):
        allCPPDefines[processedCPPDefine.Name] = processedCPPDefine
        if processedCPPDefine.FromPackageAccess == AccessType.Private:
            privateCPPDefines.append(processedCPPDefine)
        else:
            publicCPPDefines.append(processedCPPDefine)
        # If the cpp define has'nt been marked as consumed yet and this package isn't virtual
        # then consume it
        if processedCPPDefine.ConsumedBy == None and not package.IsVirtual:
            processedCPPDefine.ConsumedBy = package
            processedCPPDefine.IsFirstActualUse = True


    def __ResolveRemoveCPPDefine(self, packageCPPDefine, allCPPDefines, publicCPPDefines, privateCPPDefines):
        if packageCPPDefine in privateCPPDefines:
            privateCPPDefines.remove(packageCPPDefine)
        if packageCPPDefine in publicCPPDefines:
            publicCPPDefines.remove(packageCPPDefine)
        allCPPDefines.pop(packageCPPDefine.Name, None)


    def __ResolveBuildCPPDefines(self, config, platformName, finalResolveOrder):
        # We proces the packages in the correct resolve order
        # This ensure that all dependencies have been processed
        for package in finalResolveOrder:
            allCPPDefines = {}
            publicCPPDefines = []
            privateCPPDefines = []

            # Find the direct cpp defines in this package
            depCPPDefine = package.GetDirectCPPDefines(platformName)
            for cppDefine in depCPPDefine:
                processedCPPDefine = PackageCPPDefine(cppDefine, package.Name, AccessType.Public)
                if not package.IsVirtual:
                    processedCPPDefine.ConsumedBy = package
                    processedCPPDefine.IsFirstActualUse = True
                allCPPDefines[processedCPPDefine.Name] = processedCPPDefine
                if cppDefine.Access == AccessType.Public:
                    publicCPPDefines.append(processedCPPDefine)
                elif cppDefine.Access == AccessType.Private: # and depPackage.Name == package.Name:
                    privateCPPDefines.append(processedCPPDefine)

            package.ResolvedBuildDirectCPPDefines = allCPPDefines.values()

            directCPPDefines = dict(allCPPDefines)
            # Add the cpp defines from all direct dependencies
            for depPackage in package.ResolvedDirectDependencies:
                if depPackage.Access != AccessType.Link:
                    for entry in depPackage.Package.ResolvedBuildAllPublicCPPDefines:
                        processedCPPDefine = PackageCPPDefine(entry, entry.IntroducedByPackageName, depPackage.Access)
                        if not processedCPPDefine.Name in allCPPDefines:
                            self.__ResolveAddCPPDefine(package, processedCPPDefine, allCPPDefines, publicCPPDefines, privateCPPDefines)
                        elif processedCPPDefine.Name in directCPPDefines:
                            raise UsageErrorException("CPPDefine: %s was already defined by %s" % (processedCPPDefine.Name, processedCPPDefine.IntroducedByPackageName));
                        elif processedCPPDefine.FromPackageAccess < allCPPDefines[processedCPPDefine.Name].FromPackageAccess:
                            # We have access to the define but at more open access level, so adopt that instead
                            self.__ResolveRemoveCPPDefine(allCPPDefines[processedCPPDefine.Name], allCPPDefines, publicCPPDefines, privateCPPDefines)
                            self.__ResolveAddCPPDefine(package, processedCPPDefine, allCPPDefines, publicCPPDefines, privateCPPDefines)

            allCPPDefines = list(allCPPDefines.values())
            allCPPDefines.sort(key=lambda s: s.Name.lower())
            publicCPPDefines.sort(key=lambda s: s.Name.lower())
            privateCPPDefines.sort(key=lambda s: s.Name.lower())
            package.ResolvedBuildAllCPPDefines = allCPPDefines
            package.ResolvedBuildAllPublicCPPDefines = publicCPPDefines
            package.ResolvedBuildAllPrivateCPPDefines = privateCPPDefines


    def __ResolveBuildUsedFeatures(self, config, platformName, finalResolveOrder):
        for package in finalResolveOrder:
            uniqueDict = set()
            # Resolve the direct used features
            # package.ResolvedDirectUsedFeatures
            for feature in package.GetDirectUsedFeatures(platformName):
                lowerName = feature.Name.lower()
                if lowerName in uniqueDict:
                    raise FeatureUseDuplicatedException(package, feature.Name)
                packageFeature = PackageFeature(feature, package.Name)
                package.ResolvedDirectUsedFeatures.append(packageFeature)
                uniqueDict.add(lowerName)

            # Resolve all used features
            # package.ResolvedAllUsedFeatures
            uniqueFeatureNames = {}
            lowercaseNameDict = {}
            for depPackage in package.ResolvedDirectDependencies:
                for feature in depPackage.Package.ResolvedAllUsedFeatures:
                    if not feature.Name in uniqueFeatureNames:
                        # ensure that we dont have two feature names with different casing
                        lowerName = feature.Name.lower()
                        if lowerName in lowercaseNameDict:
                            raise FeatureNameCollisionException(depPackage.Package, feature.Name, lowercaseNameDict[lowerName][0], lowercaseNameDict[lowerName][1])
                        lowercaseNameDict[lowerName] = [depPackage.Package, feature.Name]
                        package.ResolvedAllUsedFeatures.append(feature)
                        uniqueFeatureNames[feature.Name] = feature
                    else:
                        for introPackageName in feature.IntroducedByPackages:
                             uniqueIntroPackage = uniqueFeatureNames[feature.Name]
                             if not introPackageName in uniqueIntroPackage.IntroducedByPackages:
                                uniqueIntroPackage.IntroducedByPackages.add(introPackageName)

            # Add this package's direct used features to
            # package.ResolvedAllUsedFeatures
            for feature in package.ResolvedDirectUsedFeatures:
                if not feature.Name in uniqueFeatureNames:
                    # ensure that we dont have two feature names with different casing
                    lowerName = feature.Name.lower()
                    if lowerName in lowercaseNameDict:
                        raise FeatureNameCollisionException(package, feature.Name, lowercaseNameDict[lowerName][0], lowercaseNameDict[lowerName][1])
                    lowercaseNameDict[lowerName] = [package, feature.Name]
                    package.ResolvedAllUsedFeatures.append(feature)
                    uniqueFeatureNames[feature.Name] = feature
                else:
                    for introPackageName in feature.IntroducedByPackages:
                            uniqueIntroPackage = uniqueFeatureNames[feature.Name]
                            if not introPackageName in uniqueIntroPackage.IntroducedByPackages:
                                uniqueIntroPackage.IntroducedByPackages.add(introPackageName)

            package.ResolvedAllUsedFeatures.sort(key=lambda s: s.Name.lower())


    def __ResolveBuildPlatformNotSupported(self, config, platformName, finalResolveOrder):
        for package in finalResolveOrder:
            notSupported = package.ResolvedPlatformDirectNotSupported
            if not notSupported:
                # Ensure that if one package dependency is marked as NotSupported
                # this package gets marked as not supported too
                for depPackage in package.ResolvedDirectDependencies:
                    if depPackage.Package.ResolvedPlatformNotSupported:
                        notSupported = True
            package.ResolvedPlatformNotSupported = notSupported


    def __ExtractIncludePath(self, config, package):
        path = config.ToPath(package.AbsoluteIncludePath)
        return Util.UTF8ToAscii(path)


    def __ResolveDirectExternalDependencies(self, platformName, packages):
        for package in packages:
            package.ResolvedDirectExternalDependencies = package.GetExternalDependencies(platformName)
            #print("Deps: " + ", ".join(Util.ExtractNames(package.ResolvedDirectExternalDependencies)))


    def __ResolveDirectVariants(self, finalResolveOrder, platformName):
        # First we resolve all direct variants introduced by a package
        for package in finalResolveOrder:
            package.ResolvedDirectVariants = package.GetVariants(platformName)


    def __GetVariantUndefinedOptions(self, baseVariant, extendingVariant):
        res = []
        for entry in extendingVariant.Options:
            if not entry.Name in baseVariant.OptionDict:
                res.append(entry)
        return res

    # This is a fast way to validate various variant rules
    def __CheckVariants(self, config, finalResolveOrder):
        variantDict = {}
        for package in finalResolveOrder:
            # collect the variants the we encounter and check if any
            # later encounter of it tries to extend it with new options
            for variant in package.ResolvedDirectVariants:
                if not variant.Name in variantDict:
                    variantDict[variant.Name] = [package, variant];
                else:
                    record = variantDict[variant.Name]
                    if not config.AllowVariantExtension:
                        raise VariantExtensionNotSupportedException(package, variant, record[0])
                    undefinedOptions = self.__GetVariantUndefinedOptions(record[1], variant)
                    if len(undefinedOptions) > 0:
                        raise ExtendingVariantCanNotIntroduceNewOptionsException(package, variant, record[0], record[1], undefinedOptions)

        # Ensure that the variant names are unique when casing is ignored
        uniqueNames = {}
        for key, value in variantDict.items():
            variantName = key.lower()
            if not variantName in uniqueNames:
                uniqueNames[variantName] = value
            else:
                record = uniqueNames[variantName]
                raise VariantNameCollisionException(value[0], value[1], record[0], record[1])


    def __ResolveAllVariants(self, finalResolveOrder):
        # For each package resolve all variants that it depends upon
        # NOTE: this resolver can be optimized by utilizing the fact that we resolve things in the correct order
        #       so all dependencies will have been resolved before the current package
        for package in finalResolveOrder:
            variantDict = {}
            for depPackage in package.ResolvedBuildOrder:
                for variant in depPackage.ResolvedDirectVariants:
                    clonedVariant = PackagePlatformVariant(variant, depPackage == package)
                    if not variant.Name in variantDict:
                        variantDict[variant.Name] = clonedVariant
                    else:
                        variantDict[variant.Name] = variantDict[variant.Name].Extend(clonedVariant, depPackage.Name)

            package.ResolvedAllVariantDict = variantDict

        # Update the directly resolved variants so they match the ones we utilize in the all list
        # we do this after everything has been resolved to prevent problems with modifying the ResolvedDirectVariants
        # that the above algorithm depends on
        for package in finalResolveOrder:
            variantDict = {}
            for variant in package.ResolvedAllVariantDict.values():
                variantDict[variant.Name] = variant
            for i in range(len(package.ResolvedDirectVariants)):
                variantName = package.ResolvedDirectVariants[i].Name
                if variantName in variantDict:
                    package.ResolvedDirectVariants[i] = variantDict[variantName]


    def __MarkExternalLibFirstUse(self, topLevelPackage):
#        self.__MarkExternalLibFirstUse2(topLevelPackage)
        for variant in topLevelPackage.ResolvedAllVariantDict.values():
            for option in variant.Options:
                for dep in topLevelPackage.ResolvedDirectDependencies:
                    self.__MarkExternalLibFirstUse2(dep.Package, variant.Name, option.Name)


    def __MarkExternalLibFirstUse2(self, package, variantName, optionName):
        if package.IsVirtual or not variantName in package.ResolvedAllVariantDict.keys():
            return set()

        usedSet = set()
        for dep in package.ResolvedDirectDependencies:
            usedSet |= self.__MarkExternalLibFirstUse2(dep.Package, variantName, optionName)

        option = package.ResolvedAllVariantDict[variantName].OptionDict[optionName]
        for entry in option.ExternalDependencies:
            if not entry.Name in usedSet:
                usedSet.add(entry.Name)
                entry.IsFirstActualUse = True
        return usedSet


    def __ResolvePlatform(self, platformName, packages, IgnoreNotSupported):
        for package in packages:
            package.ResolvedPlatformName = platformName
            package.ResolvedPlatform = package.GetPlatform(platformName)
            if not IgnoreNotSupported:
                if package.ResolvedPlatform != None:
                    # use the flag set in the xml
                    package.ResolvedPlatformDirectNotSupported = package.ResolvedPlatform.NotSupported
                else:
                    package.ResolvedPlatformDirectNotSupported = False
            else:
                package.ResolvedPlatformDirectNotSupported = False

        if platformName == PackageConfig.PLATFORM_WINDOWS:
            for package in packages:
                if package.Type == PackageType.Library or package.Type == PackageType.Executable: # or package.Type == PackageType.ExeLibCombo:
                    if package.ResolvedPlatform != None and package.ResolvedPlatform.ProjectId == None:
                        raise XmlMissingWindowsVisualStudioProjectIdException(package.XMLElement)
