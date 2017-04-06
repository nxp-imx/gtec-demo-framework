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

import itertools
import os
import os.path
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen.Generator.GeneratorVCTemplate import GeneratorVCTemplate
from FslBuildGen.Generator.GeneratorVCTemplateManager import GeneratorVCTemplateManager
from FslBuildGen import IOUtil, MakeFileHelper, Util, PackageConfig
from FslBuildGen.DataTypes import *
from FslBuildGen.Exceptions import *
from FslBuildGen.SharedGeneration import *
from FslBuildGen.PackageBuildReport import *

def GetVCBuildConfigurationName(entry):
    return "-".join(entry)

#class GeneratorVCMode:
#    Normal = 0
#    LinuxTools = 1


class GeneratorVC(object):
    def __init__(self, config, packages, platformName, vsVersion):
        super(GeneratorVC, self).__init__()
        

        packageLanguage = config.ToolConfig.DefaultPackageLanguage
        templateManger = GeneratorVCTemplateManager(config.ToolConfig.TemplateFolder, vsVersion)
        # for now we assume all packages are using the same language
        languageTemplates = templateManger.TryGetLanguageTemplates(packageLanguage)
        if languageTemplates == None:
            raise UnsupportedException("No visual studio generator templates found for language: {0}".format(PackageLanguage.ToString(packageLanguage)))

        template = GeneratorVCTemplate(config, platformName, vsVersion, languageTemplates)
        self.UsingLinuxTools = template.UsingLinuxTools

        self.__CheckProjectIds(packages)

        for package in packages:
            if package.Type == PackageType.Library:
                self.__GenerateLibraryBuildFile(config, package, platformName, template.Lib, template.Bat, vsVersion)
            elif package.Type == PackageType.Executable:
                self.__GenerateLibraryBuildFile(config, package, platformName, template.Exe, template.Bat, vsVersion)
            elif template.HeaderLib and package.Type == PackageType.HeaderLibrary:
                self.__GenerateLibraryBuildFile(config, package, platformName, template.HeaderLib, template.Bat, vsVersion)

        self.__ValidateProjectIds(packages);


    def GetPackageGitIgnoreDict(self):
        """ Return a dictionary of packages and a list of strings that should be added to git ignore for it """
        return {}


    def __CheckProjectIds(self, packages):
        for package in packages:
            if package.Type == PackageType.Library or package.Type == PackageType.Executable or package.Type == PackageType.HeaderLibrary:
                if package.ResolvedPlatform == None or package.ResolvedPlatform.ProjectId == None:
                    raise XmlFormatException("Missing project id for windows platform for package %s" % (package.Name))


    def __ValidateProjectIds(self, packages):
        idDict = {}
        for package in packages:
            if package.Type == PackageType.Executable and package.ResolvedPlatform != None:
                idDict[package.ResolvedPlatform.ProjectId] = package

        for package in packages:
            if package.Type == PackageType.Library or package.Type == PackageType.HeaderLibrary:
                if package.ResolvedPlatform.ProjectId in idDict:
                    raise PackageDuplicatedWindowsVisualStudioProjectIdException(package, idDict[package.ResolvedPlatform.ProjectId], package.ResolvedPlatform.ProjectId)
                else:
                    idDict[package.ResolvedPlatform.ProjectId] = package


    def __GenerateLibraryBuildFile(self, config, package, platformName, template, batTemplate, vsVersion):
        platformProjectId = 'ErrorNotDefined'
        if package.ResolvedPlatform == None or package.ResolvedPlatform.ProjectId == None:
            raise XmlFormatException("Missing project id for windows platform for package %s" % (package.Name))

        platformProjectId = package.ResolvedPlatform.ProjectId
        variantHelper = VariantHelper(package)
        libName = package.Name

        slnSnippet1 = self.__GenerateSLNPackageVariantConfig(variantHelper, template.SLNSnippet1, package)
        slnSnippet2 = self.__GenerateSLNPackageVariants(variantHelper, template.SLNSnippet2)
        libDep1SLN = self.__GenerateSLNDependencies1(package)
        libDep2SLN = self.__GenerateSLNDependencies2(config, package)
        libDep3SLN = self.__GenerateSLNDependencies3(variantHelper, template.SLNSnippet2, package)
        solutionFolderDefinitions = self.__GenerateFolderDefinitions(config, package, template.SLNSnippet3)
        solutionFolderEntries = self.__GenerateFolderEntries(config, package, template.SLNSnippet4, template.SLNSnippet4_1)

        build = template.TemplateSLN
        build = build.replace("##PACKAGE_TARGET_NAME##", libName)
        build = build.replace("##PACKAGE_PLATFORM_PROJECT_ID##", platformProjectId)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES1##", libDep1SLN)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES2##", libDep2SLN)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES3##", libDep3SLN)
        build = build.replace("##PACKAGE_SOLUTION_FOLDER_DEFINITIONS##", solutionFolderDefinitions)
        build = build.replace("##PACKAGE_SOLUTION_FOLDER_ENTRIES##", solutionFolderEntries)
        build = build.replace("##SNIPPET1##", slnSnippet1)
        build = build.replace("##SNIPPET2##", slnSnippet2)
        buildSLN = build

        projectionConfigurations = self.__GenerateProjectConfigurations(variantHelper, template.Snippet1, package)

        libDepVC = self.__GenerateVCDependencies(template.Snippet2_1, config, package)
        if len(libDepVC) > 0:
            libDepVC = template.Snippet2.replace("##SNIPPET##", libDepVC)
        
        resolvedBuildAllIncludeFiles = self.__GetPackageResolvedBuildAllIncludeFiles(config.GenFileName, package)
        includeFiles = self.__CreateVisualStudioStyleFileList(template.Snippet3, resolvedBuildAllIncludeFiles)
        sourceFiles = self.__CreateVisualStudioStyleFileList(template.Snippet4, package.ResolvedBuildSourceFiles)
        contentSourceFiles = self.__CreateContentSourceFilesList(template.Snippet9, template.Snippet9_1, package.ResolvedBuildContentSourceFiles)

        snippet5 = self.__VariantSimpleReplace(variantHelper, template.Snippet5, package)
        importGroups = self.__GenerateImportGroups(variantHelper, template.Snippet6, package)
        variantPropertyGroups = self.__GenerateVariantPropertyGroups(config, variantHelper, template.Snippet7, package)
        compilerSettingsGroups = self.__GenerateCompilerSettingsGroups(config, variantHelper, template.Snippet8, template.Snippet8_1, template.Snippet8_2, package)
        compilerSettingsGroups = self.__ApplyOptimizations(compilerSettingsGroups, config, template, package) 

        featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
        featureList = ",".join(featureList)

        build = template.Master
        build = build.replace("##SNIPPET1##", projectionConfigurations)
        build = build.replace("##SNIPPET2##", libDepVC)
        build = build.replace("##SNIPPET3##", includeFiles)
        build = build.replace("##SNIPPET4##", sourceFiles)
        build = build.replace("##SNIPPET5##", snippet5)
        build = build.replace("##SNIPPET6##", importGroups)
        build = build.replace("##SNIPPET7##", variantPropertyGroups)
        build = build.replace("##SNIPPET8##", compilerSettingsGroups)
        build = build.replace("##SNIPPET9##", contentSourceFiles)
        build = build.replace("##PACKAGE_PLATFORM_PROJECT_ID##", platformProjectId)
        build = build.replace("##PACKAGE_TARGET_NAME##", libName)
        build = build.replace("##FEATURE_LIST##", featureList)
        build = build.replace("##PLATFORM_NAME##", platformName)

        #build = build.replace("##VARIANT##", "")
        buildVC = build


        # generate bat file
        startProjectFile = self.__GenerateProjectBatFile(config, package, batTemplate.TemplateStartBat, batTemplate.TemplateSnippetErrorCheck, vsVersion, platformName, False)
        buildProjectFile = self.__PrepareProjectBatFile(config, package, batTemplate.TemplateBuildBat, batTemplate.TemplateSnippetErrorCheck, vsVersion, libName, featureList, platformName)
        runProjectFile = self.__PrepareProjectBatFile(config, package, batTemplate.TemplateRunBat, batTemplate.TemplateSnippetErrorCheck, vsVersion, libName, featureList, platformName)


        dstName = package.Name
        if (package.ResolvedPlatform == None or package.ResolvedPlatform.ProjectId == None):
            raise InternalErrorException("Could not find project name");
            #dstName = 'test'
        #dstName = 'test'
        dstFileSLN = IOUtil.Join(package.AbsolutePath, dstName + ".sln")
        dstFileVC = IOUtil.Join(package.AbsolutePath, dstName + ".vcxproj")

        dstFileStartProject = IOUtil.Join(package.AbsolutePath, ".StartProject.bat")

        buildBasePath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        dstFileBuildProject = IOUtil.Join(buildBasePath, "BuildProject.bat")
        dstFileRunProject = IOUtil.Join(buildBasePath, "RunProject.bat")
        if not config.DisableWrite:
            IOUtil.SafeMakeDirs(buildBasePath)
            IOUtil.WriteFileIfChanged(dstFileSLN, buildSLN)
            IOUtil.WriteFileIfChanged(dstFileVC, buildVC)

            if package.Type == PackageType.Executable:
                manifestFile = IOUtil.Join(package.AbsolutePath, dstName + ".manifest")
                IOUtil.WriteFileIfChanged(manifestFile, template.Manifest)

            if template.FilterFile != None:
                filterFile = IOUtil.Join(package.AbsolutePath, dstName + ".vcxproj.filters")
                IOUtil.WriteFileIfChanged(filterFile, template.FilterFile)

            IOUtil.WriteFileIfChanged(dstFileStartProject, startProjectFile)
            IOUtil.WriteFileIfChanged(dstFileBuildProject, buildProjectFile)
            IOUtil.WriteFileIfChanged(dstFileRunProject, runProjectFile)


        if package.Type == PackageType.Executable:
            self.__AttachBuildReport(package, variantHelper, libName)

            if not config.DisableWrite:
                content = template.ResouceFile
                rcFileName = IOUtil.Join(package.AbsolutePath, dstName + ".rc")
                IOUtil.WriteFileIfChanged(rcFileName, content)

                for fileName in template.ExeCopyFiles:
                    copyToFileName = IOUtil.Join(package.AbsolutePath, IOUtil.GetFileName(fileName))
                    IOUtil.CopySmallFile(fileName, copyToFileName)

            for fileName in template.ExeProcessTextFiles:
                content = IOUtil.ReadFile(fileName)
                content = content.replace("##PACKAGE_PLATFORM_PROJECT_ID##", platformProjectId)
                content = content.replace("##PACKAGE_TARGET_NAME##", libName)
                content = content.replace("##FEATURE_LIST##", featureList)
                content = content.replace("##PLATFORM_NAME##", platformName)
                writeToFileName = IOUtil.Join(package.AbsolutePath, IOUtil.GetFileName(fileName))
                if not config.DisableWrite:
                        IOUtil.WriteFileIfChanged(writeToFileName, content)


    def __GetPackageResolvedBuildAllIncludeFiles(self, genFileName, package):
        if not self.UsingLinuxTools:
            return package.ResolvedBuildAllIncludeFiles
        files = list(package.ResolvedBuildAllIncludeFiles)
        # we add 'fsl.gen' to ensure that the 'remote' copy process doesn't remove any dirs 
        files.append(genFileName)
        return files;


    def __PrepareProjectBatFile(self, config, package, templateBat, templateSnippetErrorCheck, vsVersion, libName, featureList, platformName):
        buildProjectFile = self.__GenerateProjectBatFile(config, package, templateBat, templateSnippetErrorCheck, vsVersion, platformName, True)
        buildProjectFile = buildProjectFile.replace("##PACKAGE_TARGET_NAME##", libName)
        buildProjectFile = buildProjectFile.replace("##FEATURE_LIST##", featureList)
        return buildProjectFile


    def __AttachBuildReport(self, package, variantHelper, libName):
        # $(SolutionDir)\build\##PLATFORM_NAME##\##PACKAGE_TARGET_NAME##\$(Configuration)##PROJECT_VARIANT_NAME##\
        # $(SolutionDir)\build\Windows\S06_Texturing\$(Configuration)_$(FSL_GLES_NAME)\
        allVariantNames = self.__GenerateSLNVariantNames(variantHelper) 
        if len(allVariantNames) < 1:
            allVariantNames.append('')
        allVariantPathDict = {}
        for variantName in allVariantNames:
            foundBuildPath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
            foundBuildPath = IOUtil.Join(foundBuildPath, libName)
            foundConfiguration = "%sDebug" % (variantName)
            foundVariantName = package.ResolvedVirtualVariantName
            foundFullVariantName = foundConfiguration + foundVariantName
            foundExecutableName = "%s/%s/%s.exe" % (foundBuildPath, foundFullVariantName, libName)
            # we are using the configuration as the virtual variant is part of the path
            allVariantPathDict[foundConfiguration] = foundExecutableName
           
        package.BuildReport = PackageBuildReport(allVariantPathDict)


    def __GenerateProjectBatFile(self, config, package, strBatTemplate, snippetErrorCheck, vsVersion, platformName, useEnvironmentConfig):
        projectPath = config.ToDosPath(package.AbsolutePath)

        featureList = []
        if not useEnvironmentConfig:
            batCount = 0
            skipEGLHack = self.__ContainsOverrideFeature(package.ResolvedAllUsedFeatures);
            for feature in package.ResolvedAllUsedFeatures:
                if not skipEGLHack or feature.Id != 'egl':
                    scriptName = "Configure%s.bat" % (feature.Name)
                    scriptPath = IOUtil.Join(config.SDKPath, ".Config")
                    scriptPath = IOUtil.Join(scriptPath, scriptName)
                    if IOUtil.IsFile(scriptPath):
                        batCount = batCount + 1
                        featureList.append("call %s %%%s" % (scriptName, batCount))
                        featureList.append(snippetErrorCheck)
        else:
            featureToVariantDict = self.__MatchFeaturesToVariants(config, package)
            skipEGLHack = self.__ContainsOverrideFeature(package.ResolvedAllUsedFeatures);
            for feature in package.ResolvedAllUsedFeatures:
                if not skipEGLHack or feature.Id != 'egl':
                    scriptName = "Configure%s.bat" % (feature.Name)
                    scriptPath = IOUtil.Join(config.SDKPath, ".Config")
                    scriptPath = IOUtil.Join(scriptPath, scriptName)
                    if IOUtil.IsFile(scriptPath):
                        featureEnvName = "%%%s%s%%" % (GEN_BUILD_ENV_FEATURE_SETTING, feature.Name.upper())
                        if feature.Name in featureToVariantDict:
                            variant = featureToVariantDict[feature.Name]
                            variantEnvName = "%%%s%s%%" % (GEN_BUILD_ENV_VARIANT_SETTING, variant.PurifiedName.upper())
                            featureList.append("call %s %s %s" % (scriptName, variantEnvName, featureEnvName))
                        else:
                            featureList.append("call %s %s" % (scriptName, featureEnvName))
                        featureList.append(snippetErrorCheck)

        strVersion = "%s" % (vsVersion)
        result = strBatTemplate
        result = result.replace("##PLATFORM_NAME##", platformName)
        result = result.replace("##VS_VERSION##", strVersion)
        result = result.replace("##PROJECT_NAME##", package.Name)
        result = result.replace("##CONFIG_SCRIPTS##", "\n".join(featureList))
        result = result.replace("##VS_VERSION##", strVersion)
        result = result.replace("##PROJECT_PATH##", projectPath)
        return result

    def __ContainsOverrideFeature(self, allUsedFeatures):
        """ Hack to work around the EGL / GLES or VG issues, because VG has a custom EGL """
        """ The main issue is from the fact that there is no proper link between the batch files and the features (or feature and the variant)"""
        for feature in allUsedFeatures:
            if feature.Id.startswith("opengles") or feature.Name.lower().startswith("openvg"):
                return True
        return False
        


    def __MatchFeaturesToVariants(self, config, package):
        virtualVariants = self.__GetVirtualVariants(package)
        featureToVariantDict = {}
        for feature in package.ResolvedAllUsedFeatures:
            variant = self.__LocateFeatureOwningVariant(config, package, virtualVariants, feature)
            if variant:
                featureToVariantDict[feature.Name] = variant

        # Log warnings for failed matches
        if len(virtualVariants) > 0 and len(virtualVariants) != len(featureToVariantDict):
            variantToFeatureDict = {}
            for feaure, variant in featureToVariantDict.items():
                variantToFeatureDict[variant.Name] = feature
            for entry in virtualVariants:
                if not entry.Name in variantToFeatureDict:
                    config.DoPrint("WARNING: Failed to match variant '%s' with a feature" % (entry.Name));
        return featureToVariantDict


    def __LocateFeatureOwningVariant(self, config, package, virtualVariants, feature):
        # Try a simple lookup first
        for variant in virtualVariants:
            if variant.IntroducedByPackageName == feature.IntroducedByPackageName:
                return variant
        # No such luck
        # Build a package name -> package lookup dict
        packageDict = {}
        for entry in package.ResolvedBuildOrder:
            packageDict[entry.Name] = entry

        if not feature.IntroducedByPackageName in packageDict:
            raise Exception("Could not locate package '%s' during feature resolve" % (feature.IntroducedByPackageName))

        featurePackage = packageDict[feature.IntroducedByPackageName]
        variants = self.__LocateVarientsInPackageDependencies(featurePackage.ResolvedBuildOrder)
        if len(variants) == 1:
            return variants.values()[0]
        if config.Verbosity > 1 and len(variants) > 1:
            config.DoPrint("WARNING: Could not determine which variant %s introduced the feature: %s" % (variants.keys(), feature.Name))
        return None


    def __LocateVarientsInPackageDependencies(self, packages):
        result = {}
        for entry in packages:
            for variant in entry.ResolvedDirectVariants:
                result[variant.Name] = variant
        return result



    def __GetVirtualVariants(self, package):
        virtualVariants = []
        for entry in package.ResolvedAllVariantDict.values():
            if entry.Type == VariantType.Virtual:
                virtualVariants.append(entry)
        return virtualVariants


    def __GetExternalLibraryDependencies(self, package):
        libPaths = []
        buildOrder = list(package.ResolvedBuildOrder)
        buildOrder.reverse()
        for entry in buildOrder:
            externalList = Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
            libPaths += externalList
        return libPaths


    def __CreateVisualStudioStyleFileList(self, snippet, entries):
        if entries == None:
            return ""
        res = []
        for entry in entries:
            str = snippet.replace("##FILE_PATH##", entry.replace('/', '\\'))
            res.append(str)
        return "\n".join(res)


    def __CreateContentSourceFilesList(self, snippet, snippetEntry, entries):
        if len(entries) <= 0:
            return ""
        fileEntries = self.__CreateVisualStudioStyleFileList(snippetEntry, entries)
        result = snippet;
        result = result.replace("##SNIPPET##", fileEntries)
        return result


    def __CreateVisualStudioStyleList(self, entries):
        if entries != None and len(entries) > 0:
            res = ";".join(entries)
            if not self.UsingLinuxTools:
                res = res.replace("/", "\\")
            return res
        else:
            return ""


    def __CreateVisualStudioStyleDefineList(self, entries):
        if entries != None and len(entries) > 0:
            return ";".join(entries).replace("/", "\\") + ';'
        else:
            return ""


    def __GenerateVariantPostfix(self, package):
        variants = package.ResolvedAllVariantDict.keys()
        if len(variants) > 0:
            return "_" + "_".join(variants)
        else:
            return ""


    def FilterVariants(self, variants):
        dict = {}
        for entry in variants:
            if not entry.Type in dict:
                dict[entry.Type] = []
            dict[entry.Type].append(entry)
        return dict


    def __ContainsVariant(self, list, variantName):
        for entry in list:
            if entry.Name == variantName:
                return True
        return False


    def __BuildMatchedVariantName(self, parentVariantHelper, currentConfiguration, variantHelper):
        parentVariantList = parentVariantHelper.NormalVariants
        variantList = variantHelper.NormalVariants
        filteredVariantList = []
        parentVariantIndex = 0
        for entry in currentConfiguration:
            if self.__ContainsVariant(variantList, parentVariantList[parentVariantIndex].Name):
                filteredVariantList.append(entry)
            parentVariantIndex = parentVariantIndex + 1
        return "-".join(filteredVariantList)


    def __GenerateSLNVariantNames(self, variantHelper):
        variantNames = []
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                variantNames.append(GetVCBuildConfigurationName(entry))
        return variantNames

    def __GenerateSLNPackageVariantConfig(self, variantHelper, snippet, package):
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                sectionVariantName = GetVCBuildConfigurationName(entry)
                section = snippet.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        else:
            return snippet.replace("##VARIANT##", '')


    def __VariantSimpleReplace(self, variantHelper, snippet, package):
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                section = snippet
                sectionVariantName = GetVCBuildConfigurationName(entry)
                section = section.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        else:
            return snippet.replace("##VARIANT##", '')

    def __GenerateProjectConfigurations(self, variantHelper, snippet, package):
        return self.__VariantSimpleReplace(variantHelper, snippet, package)


    def __GenerateImportGroups(self, variantHelper, snippet, package):
        return self.__VariantSimpleReplace(variantHelper, snippet, package)


    def __GenerateVariantPropertyGroups(self, config, variantHelper, snippet, package):
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                dynamicName = package.ResolvedVirtualVariantName
                section = self.__GetVariantPropertySections(config, snippet, dynamicName, package)
                sectionVariantName = GetVCBuildConfigurationName(entry)
                section = section.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        else:
            section = self.__GetVariantPropertySections(config, snippet, package.ResolvedVariantName, package)
            return section.replace("##VARIANT##", '')


    def __BuildGenerateCompilerSettingsGroupSection(self, config, snippet, subSnippet1, subSnippet2, package, includeDirs, cppDefines, variantExtDeps):
        includeDirs = self.__FixIncludes(includeDirs)
        if self.UsingLinuxTools:
            includeDirs = self.__FixIncludesForLinuxTools(config, package, includeDirs)
        includeDirs = self.__CreateVisualStudioStyleList(includeDirs)

        # FIX: variantExtDeps contains items already referenced by things we are dependent upon.
        extLinkLibDepVC = ''
        extLinkDebugLibDepVC = ''
        extLibDepVC = ''
        extLibDebugDepVC = ''

        if package.Type == PackageType.Executable:
            extLinkLibDepVC = self.__GenerateVCExternalLinkDependencies(subSnippet1, subSnippet2, package, variantExtDeps, False)
            extLinkDebugLibDepVC = self.__GenerateVCExternalLinkDependencies(subSnippet1, subSnippet2, package, variantExtDeps, True)
            extLibDepVC = self.__GenerateVCExternalLibDependencies(package, variantExtDeps)
            extLibDebugDepVC = self.__GenerateVCExternalLibDependencies(package, variantExtDeps)

        cppLocalDefines = Util.ExtractNames(cppDefines)
        cppLocalDefineNames = self.__CreateVisualStudioStyleDefineList(cppLocalDefines)

        section = snippet
        section = section.replace("##PACKAGE_INCLUDE_DIRS##", includeDirs)
        section = section.replace("##PACKAGE_CPP_FLAGS##", cppLocalDefineNames)
        section = section.replace('##PACKAGE_EXTERNAL_LINK_LIBRARY_DEPENDENCIES##', extLinkLibDepVC);
        section = section.replace('##PACKAGE_EXTERNAL_LINK_DEBUG_LIBRARY_DEPENDENCIES##', extLinkDebugLibDepVC);
        section = section.replace("##PACKAGE_EXTERNAL_LIBRARY_DEPENDENCIES##", extLibDepVC)
        section = section.replace("##PACKAGE_EXTERNAL_DEBUG_LIBRARY_DEPENDENCIES##", extLibDebugDepVC)
        return section


    def __FindByName(self, variantExtDeps, name):
        for entry in variantExtDeps:
            if entry.Name == name:
                return True
        return False


    def AddOption(self, option, includeDirs, variantExtDeps, cppDefines):
        # add external dependencies
        for entry in option.ExternalDependencies:
            if entry.Include != None and not entry.Include in includeDirs:
                includeDirs.append(entry.Include)
                # disabled since we did the exe links all fix
                #if entry.IsFirstActualUse:
                #variantExtDeps.append(entry)
            # due to the exe links all fix
            if not self.__FindByName(variantExtDeps, entry.Name):
                variantExtDeps.append(entry)

        # add cpp defines
        for entry in option.DirectCPPDefines:
            cppDefines.append(entry)


    def __GenerateCompilerSettingsGroups(self, config, variantHelper, snippet, subSnippet1, subSnippet2, package):
        includeDirs = package.ResolvedBuildAllIncludeDirs
        cppDefines = package.ResolvedBuildAllCPPDefines

        # Process virtual variants
        variantExtDeps = self.__GetExternalLibraryDependencies(package)
        if VariantType.Virtual in variantHelper.VariantTypeDict:
            for variant in variantHelper.VariantTypeDict[VariantType.Virtual]:
                if not len(variant.Options) == 1:
                    raise InternalErrorException("The virtual variant does not contain the expected option");
                self.AddOption(variant.Options[0], includeDirs, variantExtDeps, cppDefines)

        if len(variantHelper.CartesianProduct) > 0:
            orgIncludeDirs = includeDirs
            orgVariantExtDeps = variantExtDeps
            orgCppDefines = cppDefines

            lines = []
            for entry in variantHelper.CartesianProduct:
                includeDirs = list(orgIncludeDirs)
                variantExtDeps = list(orgVariantExtDeps)
                cppDefines = list(orgCppDefines)

                variantList = variantHelper.NormalVariants
                variantIdx = 0
                for optionName in entry:
                    variant = variantList[variantIdx]
                    option = variant.OptionDict[optionName]
                    variantIdx = variantIdx + 1
                    self.AddOption(option, includeDirs, variantExtDeps, cppDefines)

                dynamicName = package.ResolvedVirtualVariantName
                section = self.__BuildGenerateCompilerSettingsGroupSection(config, snippet, subSnippet1, subSnippet2, package, includeDirs, cppDefines, variantExtDeps)
                sectionVariantName = GetVCBuildConfigurationName(entry)
                section = section.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        else:
            section = self.__BuildGenerateCompilerSettingsGroupSection(config, snippet, subSnippet1, subSnippet2, package, includeDirs, cppDefines, variantExtDeps)
            return section.replace("##VARIANT##", '')


    def __ApplyOptimizations(self, compilerSettingsGroups, config, template, package):
        debugTemplate = template.DebugOptimizations[package.BuildCustomization.Debug.Optimization]
        compilerSettingsGroups = compilerSettingsGroups.replace("##DEBUG_OPTIMIZATION_TYPE##", debugTemplate.SnippetOptimizationType)
        compilerSettingsGroups = compilerSettingsGroups.replace("##DEBUG_OPTIMIZATION_OPTIONS##", debugTemplate.SnippetOptimizationOptions)
        return compilerSettingsGroups


    def __GetVariantPropertySections(self, config, snippet, variantName, package):
        str = snippet.replace("##PROJECT_VARIANT_NAME##", variantName)
        str = str.replace("##RELATIVE_PACKAGE_PATH##", self.__ToRemotePath(config, package.AbsolutePath))
        return str


    def __FixIncludes(self, entries):
        res = []
        for entry in entries:
            if entry == 'include':
                entry = "$(ProjectDir)\\include" if not self.UsingLinuxTools else "include"
            res.append(entry)
        return res


    def __GenerateSLNDependencies1(self, package):
        str = ""
        if len(package.ResolvedDirectDependencies) > 0:
            str += '\tProjectSection(ProjectDependencies) = postProject\n'
            for dep in package.ResolvedDirectDependencies:
                if self.__IsProject(dep.Package) and dep.Package.ResolvedPlatform != None:
                    str += '\t\t{%s} = {%s}\n' % (dep.Package.ResolvedPlatform.ProjectId, dep.Package.ResolvedPlatform.ProjectId)
            str += '\tEndProjectSection\n'
        return str


    def __IsProject(self, package):
        return not package.IsVirtual or (self.UsingLinuxTools and package.Type == PackageType.HeaderLibrary)


    def __GenerateSLNDependencies2(self, config, package):
        str = ""
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                projectName = entry.Name
                projectPath = config.ToDosPathDirectConversion(entry.AbsolutePath)
                # To use relative paths instead of absolute
                # NOTE: This was disabled because it doesn't play well with 'msbuild' (even though visual studio has no problems with it).
                #projectPath = config.ToDosPath(entry.AbsolutePath)
                projectPath = "%s\\%s.vcxproj" % (projectPath, projectName)
                projectId = entry.ResolvedPlatform.ProjectId
                str += '\nProject("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "%s", "%s", "{%s}"\n' % (projectName, projectPath, projectId)
                str += "EndProject"
        return str


    def __HasSubProjects(self, package):
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                return True
        return False

    def __GenerateFolderDefinitions(self, config, package, snippet):
        if not self.__HasSubProjects(package):
            return ""
        return "\n" + snippet


    def __GenerateFolderEntries(self, config, package, snippet, subSnippet):
        if not self.__HasSubProjects(package) or len(snippet) <= 0:
            return ""

        subContent = ""
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                section = "\n" + subSnippet
                section = section.replace("##PACKAGE_PLATFORM_PROJECT_ID##", entry.ResolvedPlatform.ProjectId)
                subContent += section

        content = snippet
        content = content.replace('##SNIPPET##', subContent)
        return "\n" + content


    def __GenerateSLNPackageDependency(XmlUnsupportedPlatformException, snippet, package, sectionVariantName1, sectionVariantName2):
        section = snippet
        section = section.replace("##PACKAGE_PLATFORM_PROJECT_ID##",  package.ResolvedPlatform.ProjectId)
        section = section.replace("##VARIANT1##", sectionVariantName1)
        section = section.replace("##VARIANT2##", sectionVariantName2)
        return section


    def __GenerateSLNPackageVariants(self, variantHelper, snippet):
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                sectionVariantName = GetVCBuildConfigurationName(entry)
                section = self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, sectionVariantName, sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        else:
            return self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, '', '')

    def __GenerateAndMatchSLNPackageVariants(self, parentVariantHelper, variantHelper, snippet):
        if len(parentVariantHelper.CartesianProduct) > 0:
            lines = []
            for entry in parentVariantHelper.CartesianProduct:
                sectionVariantName = GetVCBuildConfigurationName(entry)
                matchedVariantName = self.__BuildMatchedVariantName(parentVariantHelper, entry, variantHelper)
                section = self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, sectionVariantName, matchedVariantName)
                lines.append(section)
            return "\n".join(lines)
        else:
            return self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, '', '')



    def __GenerateSLNDependencies3(self, variantHelper, snippet, package):
        res = []
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                entryVariantHelper = VariantHelper(entry)
                section = self.__GenerateAndMatchSLNPackageVariants(variantHelper, entryVariantHelper, snippet)
                res.append(section)
        return "\n".join(res) + "\n" if len(res) > 0 else ""


    def __GenerateVCDependencies(self, snippet, config, package):
        res = []
        if len(package.ResolvedBuildOrder) > 1:
            for entry in package.ResolvedBuildOrder:
                if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                    projectPath = "%s\\%s.vcxproj" % (config.ToPath(entry.AbsolutePath).replace('/', '\\'), entry.Name)
                    projectId = entry.ResolvedPlatform.ProjectId
                    str = snippet.replace("##PACKAGE_DEPENDENCY_PROJECT_PATH##",projectPath)
                    str = str.replace("##PACKAGE_DEPENDENCY_PROJECTID##", projectId.lower())
                    res.append(str)
        return "\n".join(res)


    def __GenerateVCExternalLinkDependencies(self, snippet1, snippet2, package, variantExtDeps, useDebugLibs):
        str = ""
        extDeps = Util.FilterByType(package.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
        extDeps2 = Util.FilterByType(variantExtDeps, ExternalDependencyType.StaticLib)
        extDeps += extDeps2
        if len(extDeps) > 0:
            additionalDependencies = set()
            additionalLibraryDirectories = set()
            for entry in extDeps:
                additionalDependencies.add(entry.Name if not useDebugLibs else entry.DebugName)
                if entry.Location != None:
                    additionalLibraryDirectories.add(entry.Location)
            if len(additionalDependencies) > 0:
                additionalDependencies = self.__CreateVisualStudioStyleList(additionalDependencies)
                str += "\n" + snippet1.replace("##PACKAGE_EXTERNAL_LINK_DEPENDENCIES##", additionalDependencies)
            if len(additionalLibraryDirectories) > 0:
                additionalLibraryDirectories = self.__CreateVisualStudioStyleList(additionalLibraryDirectories)
                str += "\n" + snippet2.replace("##PACKAGE_EXTERNAL_LIBRARY_DIRECTORIES##", additionalLibraryDirectories)
        return str


    def __GenerateVCExternalLibDependencies(self, package, variantExtDeps):
        str = ""
        extDeps = Util.FilterByType(package.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
        extDeps2 = Util.FilterByType(variantExtDeps, ExternalDependencyType.StaticLib)
        extDeps += extDeps2
        if len(extDeps) > 0:
            additionalDependencies = set()
            additionalLibraryDirectories = set()
            for entry in extDeps:
                additionalDependencies.add(entry.Name)
                if entry.Location != None:
                    additionalLibraryDirectories.add(entry.Location)
            str += '\n    <Lib>\n'
            if len(additionalDependencies) > 0:
                additionalDependencies = self.__CreateVisualStudioStyleList(additionalDependencies)
                str += '      <AdditionalDependencies>%s</AdditionalDependencies>\n' % (additionalDependencies)
            if len(additionalLibraryDirectories) > 0:
                additionalLibraryDirectories = self.__CreateVisualStudioStyleList(additionalLibraryDirectories)
                str += '      <AdditionalLibraryDirectories>%s</AdditionalLibraryDirectories>\n' % (additionalLibraryDirectories)
            str += '    </Lib>'
        return str


    def __GetExternalLibraryDependencies(self, package):
        libPaths = []
        for entry in package.ResolvedBuildOrder:
            externalList = Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
            libPaths += externalList
        return libPaths


    def __FixIncludesForLinuxTools(self, config, package, includeDirs):
        packagePath = config.ToBashPath(package.AbsolutePath)
        packagePath = packagePath.replace('$', '__')
        basePath = self.__GenerateBackpedalToRoot(packagePath)

        newDirs = []
        for entry in includeDirs:
            if entry == package.BaseIncludePath:
                newDirs.append(entry)
            else:
                newName = Util.ChangeToBashEnvVariables(entry)
                newName = newName.replace('$', '__')
                newName = IOUtil.Join(basePath, newName)
                newDirs.append(newName)
        return newDirs

    def __CountDirectories(self, path, count = 1):
        name = IOUtil.GetDirectoryName(path)
        if len(name) <= 0 or name == '.':
            return count
        return self.__CountDirectories(name, count + 1)


    def __GenerateBackpedalToRoot(self, path):
        directoryCount = self.__CountDirectories(path)
        backPedal = []
        for i in range(0,directoryCount):
            backPedal.append('..')
        return '/'.join(backPedal)

    def __ToRemotePath(self, config, path):
        path = config.ToBashPath(path)
        path = path.replace('$', '__')
        path = IOUtil.Join("$(RemoteRootDir)", path)
        return path
