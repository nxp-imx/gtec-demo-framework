#!/usr/bin/env python

#****************************************************************************************************************************************************
# Copyright (c) 2016 Freescale Semiconductor, Inc.
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

import argparse
import os
import shlex
import sys
import subprocess
from FslBuildGen.BuildConfig import Validate
from FslBuildGen.PackageConfig import *
from FslBuildGen.SharedGeneration import *
from FslBuildGen.DataTypes import *
from FslBuildGen.PackageListUtil import GetTopLevelPackage
from FslBuildGen import Util, IOUtil, PlatformUtil, PluginConfig
from FslBuildGen.SharedGeneration import *
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen.Generator.GeneratorAndroid import GeneratorAndroid
from FslBuildGen.Generator.GeneratorAndroidGradleCMake import GeneratorAndroidGradleCMake
from FslBuildGen.Generator.GeneratorVC import GetVCBuildConfigurationName
from FslBuildGen.BuildContent import ContentBuilder
from FslBuildGen.BuildContent.SharedValues import CONFIG_FSLBUILDCONTENT_ENABLED


class BuildVariantConfig:
    Debug = 0
    Release = 1


class BuildConfig(object):
    def __init__(self, platformName, featureNameList, requiredFeatureNameList, variantSettingsDict, buildArgs, runCommand, generator):
        super(BuildConfig, self).__init__()
        self.PlatformName = platformName
        self.FeatureNameList = featureNameList
        self.RequiredFeatureNameList = requiredFeatureNameList
        self.VariantSettingsDict = variantSettingsDict
        self.BuildArgs = buildArgs
        self.RunCommand = runCommand
        self.Generator = generator
        if GEN_MAGIC_VARIANT in variantSettingsDict:
            self.ActiveBuildVariantConfig = self.__GetBuildConfig(variantSettingsDict[GEN_MAGIC_VARIANT])
        else:
            self.ActiveBuildVariantConfig = BuildVariantConfig.Debug

    def __GetBuildConfig(self, config):
        config = config.lower()
        if config == "release":
            return BuildVariantConfig.Release
        return BuildVariantConfig.Debug
    

class Builder(object):
    # requestedPackages is the packages specifically requested by the user or None for SDK builds.
    def __init__(self, config, packages, buildConfig, requestedPackages, enableContentBuilder):
        super(Builder, self).__init__()
        buildMethod = self.__GetBuildMethod(buildConfig.PlatformName)
        topLevelPackage = GetTopLevelPackage(packages)

        resolvedBuildOrder = topLevelPackage.ResolvedBuildOrder
        allowedPackages = packages
        allowedPackages = self.__FiltersPackagesByRequiredFeature(config, allowedPackages, buildConfig.RequiredFeatureNameList, resolvedBuildOrder)
        allowedPackages = self.__FiltersPackagesByFeatures(config, allowedPackages, buildConfig.FeatureNameList)
        allowedPackages = self.__FiltersPackagesBySupported(config, allowedPackages)

        if requestedPackages:
            referencedPackageSet = self.__BuildReferencedPackageSet(requestedPackages);
            resolvedBuildOrder = self.__FilterPackages(resolvedBuildOrder, referencedPackageSet, True);
            if len(resolvedBuildOrder) < 1:
                config.DoPrint("Nothing build!");
                return

            if self.__ContainsExecutablePackage(packages) and not self.__ContainsExecutablePackage(resolvedBuildOrder):
                config.DoPrint("No executables left, skipping all")
                self.__PrintExecutableSkipReason(config, packages, resolvedBuildOrder)
                return
        else:
            if self.__ContainsExecutablePackage(packages) and not self.__ContainsExecutablePackage(allowedPackages):
                config.DoPrint("No executables left, skipping all")
                return

        resolvedBuildOrder = self.__ApplyBuildFilters(resolvedBuildOrder, allowedPackages)
        packageCount = len(resolvedBuildOrder)

        resolvedBuildOrder = self.__ApplyPlatformOrderChanges(resolvedBuildOrder, buildConfig.PlatformName)

        originalBuildArgs = buildConfig.BuildArgs

        # Default content building for all platform (for those generators that don't add it to the build file)
        if enableContentBuilder:
            for package in resolvedBuildOrder:
                if package.Type == PackageType.Executable:
                    featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
                    ContentBuilder.Build(config, package.AbsolutePath, featureList)

        # Windows runs its validation checks slightly differently
        runValidationChecks = (buildConfig.PlatformName != PLATFORM_WINDOWS)

        for package in resolvedBuildOrder:
            config.LogPrint("Building package: %s" % (package.Name))
            config.LogPrint("Package location: %s" % (package.AbsolutePath))
            if not config.IsDryRun:
                buildEnv = os.environ.copy()
                buildEnv[CONFIG_FSLBUILDCONTENT_ENABLED] = "false"
                self.__ExtendEnvironmentDictWithVariants(config, buildEnv, package, buildConfig.VariantSettingsDict)
                buildConfig.BuildArgs = list(originalBuildArgs)
                activeVariantName = self.__ExtendBuildArgsWithVariants(config, buildConfig.BuildArgs, package, buildConfig.VariantSettingsDict, buildConfig.PlatformName)
                if config.Verbosity > 4:
                    config.DoPrint("Package build arguments1: %s" % buildConfig.BuildArgs);
                    config.DoPrint("General build arguments2: %s" % originalBuildArgs);
                runCommands = buildConfig.RunCommand
                if runCommands:
                    runCommands = buildConfig.RunCommand if not buildConfig.RunCommand else shlex.split(buildConfig.RunCommand)
                    runCommands = self.__GenerateRunCommandForExecutable(config, package, runCommands, activeVariantName)
                if runValidationChecks:
                    featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
                    Validate.ValidatePlatform(config, buildConfig.PlatformName, featureList)
                buildMethod(config, package, buildConfig, buildEnv, runCommands)

        if packageCount > 0:
            config.LogPrint("Build %s packages" % (packageCount));
        else:
            config.DoPrint("Nothing build!");


    def __ApplyBuildFilters(self, resolvedBuildOrder, allowedPackages):
        packageCount = 0
        result = [];
        for package in resolvedBuildOrder:
            if package in allowedPackages and not package.IsVirtual and (package.Type == PackageType.Library or package.Type == PackageType.Executable):
                result.append(package)
                packageCount = packageCount + 1
        return result


    def __ApplyPlatformOrderChanges(self, resolvedBuildOrder, platformName):
        if platformName != PLATFORM_WINDOWS:
            return resolvedBuildOrder;

        # Since visual studio does its own dependency management and building
        # we dont need to request packages to be build multiple times.
        markedForBuild = set()
        result = [];
        for package in reversed(resolvedBuildOrder):
            if not package.Name in markedForBuild:
                markedForBuild.add(package.Name)
                result.append(package)
                for entry in package.ResolvedAllDependencies:
                    markedForBuild.add(entry.Name)

        return result


    def __PrintExecutableSkipReason(self, config, fullPackageList, filteredPackageList):
        for package in fullPackageList:
            if package.Type == PackageType.Executable:
                if package.ResolvedPlatformNotSupported:
                    notSupported = self.__BuildListOfDirectlyNotSupported(package)
                    notSupportedNames = Util.ExtractNames(notSupported)
                    config.DoPrint("%s was marked as not supported on this platform by package: %s" % (package.Name, notSupportedNames))


    def __BuildListOfDirectlyNotSupported(self, package):
        notSupported = []
        for dependency in package.ResolvedBuildOrder:
            if dependency.ResolvedPlatformDirectNotSupported:
                notSupported.append(dependency)
        return notSupported


    def __BuildReferencedPackageSet(self, requestedPackages):
        referencedPackageSet = set()
        for package in requestedPackages:
            referencedPackageSet.add(package)
            for dependency in package.ResolvedBuildOrder:
                referencedPackageSet.add(dependency)
        return referencedPackageSet


    def __FilterPackages(self, resolvedBuildOrder, referencedPackageSet, removeNotSupported):
        filteredResolvedBuildOrder = []
        for package in resolvedBuildOrder:
            if package in referencedPackageSet and (not removeNotSupported or not package.ResolvedPlatformNotSupported):
                filteredResolvedBuildOrder.append(package)
        return filteredResolvedBuildOrder


    def __ContainsExecutablePackage(self, packages):
        for package in packages:
            if package.Type == PackageType.Executable:
                return True
        return False


    def __FiltersPackagesByFeatures(self, config, packages, featureNameList):
        allowAllFeatures = '*' in featureNameList

        if allowAllFeatures:
            config.LogPrint("Filtering by features: All");
        else:
            config.LogPrint("Filtering by features: %s" % (", ".join(featureNameList)))

        packageList = []
        for package in packages:
            if allowAllFeatures or self.__FeaturesAvailable(package, featureNameList):
                packageList.append(package)
            elif package.Type == PackageType.Library or package.Type == PackageType.Executable:
                missingFeatures = self.__GetCompleteMissingFeatureNames(package,featureNameList)
                config.LogPrint("Could not build package '%s' due to missing features '%s'" % (package.Name, ", ".join(missingFeatures)))
        return packageList


    def __ContainsFeature(self, resolvedFeatureList, featureName):
        for feature in resolvedFeatureList:
            if feature.Name == featureName:
                return True
        return False


    def __UsesFeatures(self, package, requiredFeatureNameList):
        for featureName in requiredFeatureNameList:
            if not self.__ContainsFeature(package.ResolvedAllUsedFeatures, featureName):
                return False
        return True


    def __FiltersPackagesByRequiredFeature(self, config, packages, requiredFeatureNameList, allPackages):
        allowAllFeatures = '*' in requiredFeatureNameList
        if allowAllFeatures:
            return packages

        # Count and filter executables
        hasExecutables = False
        exePackages = []
        for package in packages:
            if package.Type == PackageType.Executable:
                hasExecutables = True
                if self.__UsesFeatures(package, requiredFeatureNameList):
                    exePackages.append(package)
                else:
                    config.LogPrint("Skipping '%s' since it did not use the features '%s'" % (package.Name, ", ".join(requiredFeatureNameList)))

        # Only apply this filter if there is at least one executable package to begin with
        if not hasExecutables:
            return packages

        # Filter the list based on what the remaining executables reference
        referencedPackageSet = self.__BuildReferencedPackageSet(exePackages);
        return self.__FilterPackages(allPackages, referencedPackageSet, True);
        

    def __FiltersPackagesBySupported(self, config, packages):
        packageList = []
        for package in packages:
            if not package.ResolvedPlatformNotSupported:
                packageList.append(package)
            elif not package.Type == PackageType.TopLevel and config.IsVerbose:
                notSupported = self.__BuildListOfDirectlyNotSupported(package)
                notSupportedNames = Util.ExtractNames(notSupported)
                config.DoPrint("Skipping %s since its marked as not supported on this platform by package: %s" % (package.Name, notSupportedNames))
        return packageList



    def __GetCompleteMissingFeatureNames(self, package, featureNameList):
        missing = []
        for feature in package.ResolvedAllUsedFeatures:
            if not feature.Name in featureNameList:
                missing.append(feature.Name)
        return missing


    def __FeaturesAvailable(self, package, featureNameList):
        for feature in package.ResolvedAllUsedFeatures:
            if not feature.Name in featureNameList:
                return False
        return True


    def __GetBuildMethod(self, platformName):
        generator = PluginConfig.GetGeneratorPluginById(platformName.lower())
        platformName = generator.OriginalName
        if platformName == PLATFORM_ANDROID:
            return self.__BuildPackageAndroid
        elif platformName == PLATFORM_QNX:
            return self.__BuildPackageQnx
        elif platformName == PLATFORM_UBUNTU:
            return self.__BuildPackageUbuntu
        elif platformName == PLATFORM_WINDOWS:
            return self.__BuildPackageWindows
        elif platformName == PLATFORM_YOCTO:
            return self.__BuildPackageYocto
        else:
            raise EnvironmentError("Build platform not supported %s" % (platformName));


    def __GetPlatformAntCommand(self, platformName):
        if os.name == 'posix':
            return 'ant'
        elif os.name == 'nt':
            return 'ant.bat'
        else:
            raise EnvironmentError("Unsupported build environment for '%s'" % (platform));

    def __GetPlatformGradleCommand(self, platformName):
        if os.name == 'posix':
            return 'gradlew'
        elif os.name == 'nt':
            return 'gradlew.bat'
        else:
            raise EnvironmentError("Unsupported build environment for '%s'" % (platform));


    def __BuildPackageAndroid(self, config, package, buildConfig, buildEnv, runCommands):
        if package.Type != PackageType.Executable:
            config.LogPrint("Android build only supports Executable packages for now");
            return
        if isinstance(buildConfig.Generator.LastActiveGenerator, GeneratorAndroid):
            antCommand = self.__GetPlatformAntCommand(buildConfig.PlatformName)
            buildCommand = [antCommand] + buildConfig.BuildArgs
            path = package.AbsolutePath
            path = IOUtil.Join(path, 'Android')
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                sys.exit(result)
        elif isinstance(buildConfig.Generator.LastActiveGenerator, GeneratorAndroidGradleCMake):
            path = buildConfig.Generator.LastActiveGenerator.GetAndroidProjectDir(config, package)
            gradleCommand = self.__GetPlatformGradleCommand(buildConfig.PlatformName)
            gradleCommand = IOUtil.Join(path, gradleCommand)
            localBuildArgs = []
            if len(buildConfig.BuildArgs) <= 0:
                if buildConfig.ActiveBuildVariantConfig == BuildVariantConfig.Debug:
                    localBuildArgs.append("assembleDebug")
                elif buildConfig.ActiveBuildVariantConfig == BuildVariantConfig.Release:
                    localBuildArgs.append("assembleRelease")
            buildCommand = [gradleCommand] + buildConfig.BuildArgs + localBuildArgs
            result = subprocess.call(buildCommand, cwd=path, env=buildEnv)
            if result != 0:
                sys.exit(result)
        else:
            raise Exception("Unknown android build generator");

    def __BuildPackageQnx(self, config, package, buildConfig, buildEnv, runCommands):
        buildCommand = ['make'] + buildConfig.BuildArgs
        absBuildPath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        result = subprocess.call(buildCommand, cwd=absBuildPath, env=buildEnv)
        if result != 0:
            sys.exit(result)
        if runCommands:
            result = subprocess.call(runCommands, cwd=package.AbsolutePath, env=buildEnv)
            if result != 0:
                sys.exit(result)

    def __BuildPackageUbuntu(self, config, package, buildConfig, buildEnv, runCommands):
        buildCommand = ['make'] + buildConfig.BuildArgs
        result = subprocess.call(buildCommand, cwd=package.AbsolutePath, env=buildEnv)
        if result != 0:
            sys.exit(result)
        if runCommands:
            result = subprocess.call(runCommands, cwd=package.AbsolutePath, env=buildEnv)
            if result != 0:
                sys.exit(result)


    def __BuildPackageWindows(self, config, package, buildConfig, buildEnv, runCommands):
        buildBasePath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        buildBatFile = IOUtil.Join(buildBasePath, 'BuildProject.bat')
        buildCommand = [buildBatFile] + buildConfig.BuildArgs
        # Unfortunately this causes the libs to not be copied from src -> target location too :(
        # buildCommand += ["/p:BuildProjectReferences=false"]
        result = subprocess.call(buildCommand, cwd=package.AbsolutePath, env=buildEnv)
        if result != 0:
            sys.exit(result)
        
        if runCommands:
            runBatFile = IOUtil.Join(buildBasePath, 'RunProject.bat')
            runCommand = [runBatFile] + runCommands
            result = subprocess.call(runCommand, cwd=package.AbsolutePath, env=buildEnv)
            if result != 0:
                sys.exit(result)


    def __BuildPackageYocto(self, config, package, buildConfig, buildEnv, runCommands):
        buildCommand = ['make', '-f', 'GNUmakefile_Yocto'] + buildConfig.BuildArgs
        result = subprocess.call(buildCommand, cwd=package.AbsolutePath, env=buildEnv)
        if result != 0:
            sys.exit(result)
        if runCommands:
            result = subprocess.call(runCommands, cwd=package.AbsolutePath, env=buildEnv)
            if result != 0:
                sys.exit(result)


    def __LocateVariant(self, package, key):
        if key in package.ResolvedAllVariantDict:
            return package.ResolvedAllVariantDict[key]
        # try a manual search for 'virtual keys'
        for entry in package.ResolvedAllVariantDict.values():
            if key in entry.PurifiedName:
                return entry
        return None


    def __ExtendEnvironmentDictWithVariants(self, config, buildEnv, package, userVariantSettingDict):
        for key, value in userVariantSettingDict.items():
            variant = self.__LocateVariant(package, key)
            if variant:
                if variant.Type == VariantType.Virtual or (value in variant.OptionDict):
                    envName = "%s%s" % (GEN_BUILD_ENV_VARIANT_SETTING, key.upper())
                    if envName in buildEnv:
                        raise Exception("The environment variable %s has allready been defined" % (envName))
                    buildEnv[envName] = value
                else:
                    validValues = list(variant.OptionDict.keys())
                    validValues.sort()
                    config.LogPrint("WARNING: Variant '%s' expects one of the following values: '%s' not '%s'" % (key, ','.join(validValues), value))


    def __ExtendBuildArgsWithVariants(self, config, buildArgs, package, userVariantSettingDict, platformName):
        if platformName == PLATFORM_UBUNTU or platformName == PLATFORM_YOCTO or platformName == PLATFORM_QNX:
            return self.__ExtendBuildArgsWithVariantsMake(config, buildArgs, package, userVariantSettingDict)
        elif platformName == PLATFORM_WINDOWS:
            return self.__ExtendBuildArgsWithVariantsMsBuild(config, buildArgs, package, userVariantSettingDict)
        elif platformName == PLATFORM_ANDROID:
            config.LogPrint("INFO: platform has limited support fo build variant settings");
            return ""
        if len(userVariantSettingDict) > 0:
            config.DoPrint("WARNING: build variants settings not supported");
        return ""


    def __ExtractRelevantVariantSettingsDict(self, config, package, userVariantSettingDict):
        dictVariantSettings = {}
        for key, value in userVariantSettingDict.items():
            if key in package.ResolvedAllVariantDict:
                variant = package.ResolvedAllVariantDict[key]
                if value in variant.OptionDict:
                    dictVariantSettings[key] = value
                else:
                    validValues = list(variant.OptionDict.keys())
                    validValues.sort()
                    config.LogPrint("WARNING: Variant '%s' expects one of the following values: '%s' not '%s'" % (key, ','.join(validValues), value))
        return dictVariantSettings


    def __ExtendBuildArgsWithVariantsMake(self, config, buildArgs, package, userVariantSettingDict):
        dictVariantSettings = self.__ExtractRelevantVariantSettingsDict(config, package, userVariantSettingDict)
        activeVariantName = ''
        for key, value in userVariantSettingDict.items():
            buildArgs.append("%s=%s" % (key,value))
            activeVariantName += value
        return activeVariantName


    def __ExtendBuildArgsWithVariantsMsBuild(self, config, buildArgs, package, userVariantSettingDict):
        variantHelper = VariantHelper(package)
        dictVariantSettings = self.__ExtractRelevantVariantSettingsDict(config, package, userVariantSettingDict)
        list = []
        for variant in variantHelper.NormalVariants:
            if variant.Name in dictVariantSettings:
                list.append(dictVariantSettings[variant.Name])
            else:
                option = variant.Options[0]
                list.append(option.Name)
                config.LogPrint("WARNING: No option specified for '%s' using '%s'" % (variant.Name, option.Name) )

        activeConfig = "Debug"
        if GEN_MAGIC_VARIANT in userVariantSettingDict:
            activeConfig = userVariantSettingDict[GEN_MAGIC_VARIANT]

        configName = GetVCBuildConfigurationName(list)
        activeVariantName = "%s%s" % (configName, activeConfig)
        res = "/p:Configuration=%s" % (activeVariantName)
        buildArgs.append(res)
        return activeVariantName

    def __GenerateRunCommandForExecutable(self, config, package, runCommands, activeVariantName):
        if package.Type != PackageType.Executable or not runCommands:
            return None
        
        commands = []
        for commandToRun in runCommands:
            fullPathExe = 'NotSupportedByBuilder'
            exeName = 'NotSupportedByBuilder'
            exePath = 'NotSupportedByBuilder'
            if package.BuildReport:
                fullPathExe = package.BuildReport.GetExecutableName(activeVariantName)
                exeName = IOUtil.GetFileName(fullPathExe)
                exePath = IOUtil.GetDirectoryName(fullPathExe)
            packagePath = package.AbsolutePath
            contentPath = IOUtil.Join(packagePath, "Content")
            fullBuildDirPath = IOUtil.Join(packagePath, package.ResolvedBuildPath)
            fullBuildDirPath = config.ToCurrentOSPathDirectConversion(fullBuildDirPath)
            fullPathExe = config.ToCurrentOSPathDirectConversion(fullPathExe)
            exeName = config.ToCurrentOSPathDirectConversion(exeName)
            exePath = config.ToCurrentOSPathDirectConversion(exePath)
            packagePath = config.ToCurrentOSPathDirectConversion(packagePath)
            contentPath = config.ToCurrentOSPathDirectConversion(contentPath)
            command = commandToRun
            command = command.replace("(EXE)", fullPathExe)
            command = command.replace("(EXE_NAME)", exeName)
            command = command.replace("(EXE_PATH)", exePath)
            command = command.replace("(PACKAGE_PATH)", packagePath)
            command = command.replace("(CONTENT_PATH)", contentPath)
            command = command.replace("(BUILD_PATH)", fullBuildDirPath)
            commands.append(command)
        return commands


def __GetPackageFromFileName(topLevelPackage, filename):
    for entry in topLevelPackage.ResolvedAllDependencies:
        if entry.Package.GenFile and entry.Package.GenFile.SourcePackageFile and entry.Package.GenFile.SourcePackageFile.FileName == filename:
            return entry.Package
    raise Exception("Could not find package for %s" % (filename))


def __GetPackageListFromFileNames(topLevelPackage, filenames):
    if not filenames:
        return None
    packages = []
    for file in filenames:
        packages.append(__GetPackageFromFileName(topLevelPackage, file))
    return packages


def __BuildCompleteVariantDict(topLevelPackage, requestedFiles):
    variantDict = {}
    if not requestedFiles:
        # SDK build, just list all resolved features
        for variant in topLevelPackage.ResolvedAllVariantDict.values():
            variantDict[variant.Name] = variant
    else:
        # File build just list all resolved features for the combined packages
        for file in requestedFiles:
            filePackage = __GetPackageFromFileName(topLevelPackage, file)
            for variant in filePackage.ResolvedAllVariantDict.values():
                variantDict[variant.Name] = variant
    return variantDict


def __ValidateUserVariantSettings(config, topLevelPackage, requestedFiles, userVariantSettingDict):
    variantDict = __BuildCompleteVariantDict(topLevelPackage, requestedFiles)
    for key, value in userVariantSettingDict.items():
        if key in variantDict:
            variant = variantDict[key]
            if not value in variant.OptionDict:
                validValues = list(variant.OptionDict.keys())
                validValues.sort()
                raise Exception("Variant '%s' expects one of the following values: '%s' not '%s'" % (key, ','.join(validValues), value))
        elif key != GEN_MAGIC_VARIANT:
            config.LogPrint("WARNING: Unused variant setting '%s'" % (key));


def __LogVariantSettings(config, variantSettingsDict):
       if len(variantSettingsDict) <= 0:
           return
       names = variantSettingsDict.keys()
       names.sort()
       result = []
       for name in names:
           result.append("%s=%s" % (name, variantSettingsDict[name]))
       config.LogPrint("Variant settings: %s" % (", ".join(result)))

 


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
# generator = the generator that was used to build the files
def BuildPackages(config, packages, platformName, requestedFiles, featureNameList, requiredFeatureNameList, variantSettingsDict, buildArgs, buildForAllExe, generator, enableContentBuilder):
    PlatformUtil.CheckBuildPlatform(platformName)
    topLevelPackage = GetTopLevelPackage(packages)
    requestedPackages = __GetPackageListFromFileNames(topLevelPackage, requestedFiles)

    __ValidateUserVariantSettings(config, topLevelPackage, requestedFiles, variantSettingsDict)
    __LogVariantSettings(config, variantSettingsDict)

    buildConfig = BuildConfig(platformName, featureNameList, requiredFeatureNameList, variantSettingsDict, buildArgs, buildForAllExe, generator)
    Builder(config, packages, buildConfig, requestedPackages, enableContentBuilder)


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowFeatureList(topLevelPackage, requestedFiles):
    featureDict = {}
    if not requestedFiles:
        # SDK build, just list all resolved features
        for feature in topLevelPackage.ResolvedAllUsedFeatures:
            featureDict[feature.Name] = feature
    else:
        # File build just list all resolved features for the combined packages
        for file in requestedFiles:
            filePackage = __GetPackageFromFileName(topLevelPackage, file)
            featureNames = set()
            for feature in filePackage.ResolvedAllUsedFeatures:
                featureDict[feature.Name] = feature

    # Pretty print useful information
    print("Features:")
    featureNames = list(featureDict.keys())
    featureNames.sort();
    for featureName in featureNames:
        feature = featureDict[featureName]
        print("  '%s' introduced by package: %s" % (featureName, ", ".join(feature.IntroducedByPackages)))


# requestedFiles is None for SDK builds else its the list of specifically requested files by the user
def ShowVariantList(topLevelPackage, requestedFiles, generator):
    variantDict = __BuildCompleteVariantDict(topLevelPackage, requestedFiles)

    # Pretty print useful information
    variantNames = list(variantDict.keys())
    variantNames.sort();
    print("Variants:")

    # This is kind of a hack to list this here (its also not a real variant inside our model)
    generatorVariants = generator.GetVariants()
    generatorVariants.sort(key=lambda s: s.Name.lower())
    for variantInfo in generatorVariants:
        if variantInfo.Type == BuildVariantType.Static:
            print("  %s=%s (Introduced by native build system generator)" % (variantInfo.Name, variantInfo.Description))
        else:
            print("  %s=%s (Introduced by native build system)" % (variantInfo.Name, variantInfo.Description))

    for variantName in variantNames:
        variant = variantDict[variantName]
        optionNames = list(variant.OptionDict.keys())
        optionNames.sort()
        if variant.Type == VariantType.Virtual:
            print("  %s=%s *Virtual* (Introduced by package: %s)" % (variant.PurifiedName, ', '.join(optionNames), variant.IntroducedByPackageName))
        else:
            print("  %s=%s (Introduced by package: %s)" % (variant.PurifiedName, ', '.join(optionNames), variant.IntroducedByPackageName))


def ShowBuildVariantList(generator):
    print("Build variants:")
    # This is kind of a hack to list this here (its also not a real variant inside our model)
    generatorVariants = generator.GetVariants()
    generatorVariants.sort(key=lambda s: s.Name.lower())
    
    for variantInfo in generatorVariants:
        if variantInfo.Type == BuildVariantType.Static:
            print("  %s=%s (Introduced by native build system generator)" % (variantInfo.Name, variantInfo.Description))
