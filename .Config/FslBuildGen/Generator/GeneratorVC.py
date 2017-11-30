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

from typing import Dict
from typing import Iterable
from typing import List
from typing import Optional
from typing import Set
from typing import Tuple
from typing import Union
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.Config import Config
from FslBuildGen.DataTypes import AccessType
from FslBuildGen.DataTypes import BuildVariantConfig
from FslBuildGen.DataTypes import ExternalDependencyType
from FslBuildGen.DataTypes import PackageLanguage
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.DataTypes import VariantType
from FslBuildGen.Exceptions import InternalErrorException
from FslBuildGen.Exceptions import UnsupportedException
from FslBuildGen.Generator.ExceptionsVC import PackageDuplicatedWindowsVisualStudioProjectIdException
from FslBuildGen.Generator.GeneratorBase import GeneratorBase
from FslBuildGen.Generator.GeneratorVCTemplate import CodeTemplateVC
from FslBuildGen.Generator.GeneratorVCTemplate import CodeTemplateProjectBatFiles
from FslBuildGen.Generator.GeneratorVCTemplate import GeneratorVCTemplate
from FslBuildGen.Generator.GeneratorVCTemplate import NuGetPackageConfigSnippets
from FslBuildGen.Generator.GeneratorVCTemplateManager import GeneratorVCTemplateManager
from FslBuildGen.Generator.Report.Datatypes import FormatStringEnvironmentVariableResolveMethod
from FslBuildGen.Generator.Report.GeneratorBuildReport import GeneratorBuildReport
from FslBuildGen.Generator.Report.GeneratorCommandReport import GeneratorCommandReport
from FslBuildGen.Generator.Report.GeneratorExecutableReport import GeneratorExecutableReport
from FslBuildGen.Generator.Report.GeneratorVariableReport import GeneratorVariableReport
from FslBuildGen.Generator.Report.PackageGeneratorReport import PackageGeneratorReport
from FslBuildGen.Generator.VariantHelper import VariantHelper
from FslBuildGen.Log import Log
from FslBuildGen.Packages.Package import Package
from FslBuildGen.Packages.Package import PackageDefine
from FslBuildGen.Packages.Package import PackageExternalDependency
from FslBuildGen.Packages.Package import PackagePlatformVariant
from FslBuildGen.Packages.Package import PackagePlatformVariantOption
from FslBuildGen.Packages.PackageRequirement import PackageRequirement
from FslBuildGen.SharedGeneration import ToolAddedVariant
from FslBuildGen.SharedGeneration import GEN_BUILD_ENV_FEATURE_SETTING
from FslBuildGen.SharedGeneration import GEN_BUILD_ENV_VARIANT_SETTING
from FslBuildGen.Template.TemplateFileProcessor import TemplateFileProcessor
from FslBuildGen.Xml.Exceptions import XmlFormatException


#class GeneratorVCMode:
#    Normal = 0
#    LinuxTools = 1

class LocalMagicFilenames:
    BuildProject = "BuildProject.bat"
    StartProject = ".StartProject.bat"
    RunProject = "RunProject.bat"

class LocalMagicBuildVariants:
    GeneratorConfig = "FSLGEN_GENERATOR_Config"

class LocalMagicBuildVariantOption:
    Debug = "Debug"
    Release = "Release"


class GeneratorVC(GeneratorBase):
    def __init__(self, config: Config, packages: List[Package], platformName: str, vsVersion: int, activeThirdPartyLibsDir: Optional[str]) -> None:
        super(GeneratorVC, self).__init__()
        self.__ActiveThirdPartyLibsDir = activeThirdPartyLibsDir

        packageLanguage = config.ToolConfig.DefaultPackageLanguage
        templateManger = GeneratorVCTemplateManager(config, config.ToolConfig.TemplateFolder, vsVersion)
        # for now we assume all packages are using the same language
        languageTemplates = templateManger.TryGetLanguageTemplates(packageLanguage)
        if languageTemplates is None:
            raise UnsupportedException("No visual studio generator templates found for language: {0}".format(PackageLanguage.ToString(packageLanguage)))

        template = GeneratorVCTemplate(config, platformName, vsVersion, languageTemplates)
        self.UsingLinuxTools = template.UsingLinuxTools

        self.__CheckProjectIds(packages)

        for package in packages:
            if package.Type == PackageType.Library:
                self.__GenerateLibraryBuildFile(config, package, platformName, template.GetLibraryTemplate(package), template.GetBatTemplate(), vsVersion)
            elif package.Type == PackageType.Executable:
                self.__GenerateLibraryBuildFile(config, package, platformName, template.GetExecutableTemplate(package), template.GetBatTemplate(), vsVersion)
            elif package.Type == PackageType.HeaderLibrary:
                headerLibTemplate = template.TryGetHeaderLibraryTemplate()
                if headerLibTemplate is not None:
                    self.__GenerateLibraryBuildFile(config, package, platformName, headerLibTemplate, template.GetBatTemplate(), vsVersion)

        self.__ValidateProjectIds(packages)


    def __CheckProjectIds(self, packages: List[Package]) -> None:
        for package in packages:
            if package.Type == PackageType.Library or package.Type == PackageType.Executable or package.Type == PackageType.HeaderLibrary:
                if package.ResolvedPlatform is None or package.ResolvedPlatform.ProjectId is None:
                    raise XmlFormatException("Missing project id for windows platform for package {0}".format(package.Name))


    def __ValidateProjectIds(self, packages: List[Package]) -> None:
        idDict = {}
        for package in packages:
            if package.Type == PackageType.Executable and package.ResolvedPlatform != None:
                if package.ResolvedPlatform is None:
                    raise Exception("Invalid package")
                idDict[package.ResolvedPlatform.ProjectId] = package

        for package in packages:
            if package.Type == PackageType.Library or package.Type == PackageType.HeaderLibrary:
                if package.ResolvedPlatform is None or package.ResolvedPlatform.ProjectId is None:
                    raise Exception("Invalid package")
                if package.ResolvedPlatform.ProjectId in idDict:
                    raise PackageDuplicatedWindowsVisualStudioProjectIdException(package, idDict[package.ResolvedPlatform.ProjectId], package.ResolvedPlatform.ProjectId)
                else:
                    idDict[package.ResolvedPlatform.ProjectId] = package


    def __GenerateLibraryBuildFile(self, config: Config,
                                   package: Package,
                                   platformName: str,
                                   template : CodeTemplateVC,
                                   batTemplate: CodeTemplateProjectBatFiles,
                                   vsVersion: int) -> None:

        if package.ResolvedPlatform is None or package.ResolvedPlatform.ProjectId is None:
            raise XmlFormatException("Missing project id for windows platform for package {0}".format(package.Name))
        if (package.ResolvedBuildSourceFiles is None or package.AbsolutePath is None or package.ResolvedBuildContentSourceFiles is None or
            package.ResolvedBuildPath is None):
            raise Exception("Invalid package")

        variantHelper = VariantHelper(package)
        targetName = GeneratorVCUtil.GetTargetName(package)

        strCurrentYear = config.CurrentYearString
        strPackageCreationYear = strCurrentYear if package.CreationYear is None else package.CreationYear
        strPackageCompanyName = package.CompanyName

        slnSnippet1 = self.__GenerateSLNPackageVariantConfig(variantHelper, template.SLNSnippet1, package)
        slnSnippet2 = self.__GenerateSLNPackageVariants(variantHelper, template.SLNSnippet2)
        libDep1SLN = self.__GenerateSLNDependencies1(package)
        libDep2SLN = self.__GenerateSLNDependencies2(config, package, template.SLNAddProject, template.ProjectExtension)
        libDep3SLN = self.__GenerateSLNDependencies3(variantHelper, template.SLNSnippet2, package)
        solutionFolderDefinitions = self.__GenerateFolderDefinitions(config, package, template.SLNSnippet3)
        solutionFolderEntries = self.__GenerateFolderEntries(config, package, template.SLNSnippet4, template.SLNSnippet4_1)

        build = template.TemplateSLN
        build = build.replace("##PACKAGE_TARGET_NAME##", targetName)
        build = build.replace("##PACKAGE_PLATFORM_PROJECT_ID##", package.ResolvedPlatform.ProjectId)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES1##", libDep1SLN)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES2##", libDep2SLN)
        build = build.replace("##PACKAGE_LIBRARY_DEPENDENCIES3##", libDep3SLN)
        build = build.replace("##PACKAGE_SOLUTION_FOLDER_DEFINITIONS##", solutionFolderDefinitions)
        build = build.replace("##PACKAGE_SOLUTION_FOLDER_ENTRIES##", solutionFolderEntries)
        build = build.replace("##SNIPPET1##", slnSnippet1)
        build = build.replace("##SNIPPET2##", slnSnippet2)
        build = build.replace("##CURRENT_YEAR##", strCurrentYear)
        build = build.replace("##PACKAGE_CREATION_YEAR##", strPackageCreationYear)
        build = build.replace("##PACKAGE_COMPANY_NAME##", strPackageCompanyName)
        buildSLN = build

        projectionConfigurations = self.__GenerateProjectConfigurations(variantHelper, template.VariantProjectConfiguration, package)

        libDepVC = self.__GenerateVCDependencies(template.ProjectReferences_1, config, package, template.ProjectExtension)
        if len(libDepVC) > 0:
            libDepVC = template.ProjectReferences.replace("##SNIPPET##", libDepVC)

        resolvedBuildAllIncludeFiles = self.__GetPackageResolvedBuildAllIncludeFiles(config.GenFileName, package)
        includeFiles = self.__CreateVisualStudioStyleFileList(template.AddHeaderFile, resolvedBuildAllIncludeFiles)
        sourceFiles = self.__CreateVisualStudioStyleFileList(template.AddSourceFile, package.ResolvedBuildSourceFiles)
        contentSourceFiles = self.__CreateContentSourceFilesList(template.Snippet9, template.Snippet9_1, package.ResolvedBuildContentSourceFiles)


        variantConfigurations = self.__VariantSimpleReplace(variantHelper, template.VariantConfiguration, package)
        importGroups = self.__GenerateImportGroups(variantHelper, template.VariantPropertySheets, package)
        variantPropertyGroups = self.__GenerateVariantPropertyGroups(config, variantHelper, template.VariantPropertyGroups, package)
        compilerSettingsGroups = self.__GenerateCompilerSettingsGroups(config, variantHelper, template.VariantCompilerSettings, template.VariantCompilerSettings_1, template.VariantCompilerSettings_2, package)
        compilerSettingsGroups = self.__ApplyOptimizations(compilerSettingsGroups, config, template, package)

        externalFilesToOutput = self.__GenerateExternalFilesToOutput(config, variantHelper, template.ExternalFileToOutput, package)

        featureList = [entry.Name for entry in package.ResolvedAllUsedFeatures]
        strFeatureList = ",".join(featureList)

        build = template.Master
        build = build.replace("##ADD_PROJECT_CONFIGURATIONS##", projectionConfigurations)
        build = build.replace("##ADD_PROJECT_REFERENCES##", libDepVC)
        build = build.replace("##ADD_INCLUDE_FILES##", includeFiles)
        build = build.replace("##ADD_SOURCE_FILES##", sourceFiles)
        build = build.replace("##ADD_CONFIGURATIONS##", variantConfigurations)
        build = build.replace("##ADD_PROPERTY_SHEETS##", importGroups)
        build = build.replace("##ADD_PROPERTY_GROUPS##", variantPropertyGroups)
        build = build.replace("##ADD_EXTENAL_FILES_TO_OUTPUT##", externalFilesToOutput)
        build = build.replace("##SNIPPET8##", compilerSettingsGroups)
        build = build.replace("##ADD_CUSTOM_BUILD_FILES##", contentSourceFiles)
        build = build.replace("##PACKAGE_PLATFORM_PROJECT_ID##", package.ResolvedPlatform.ProjectId)
        build = build.replace("##PACKAGE_TARGET_NAME##", targetName)
        build = build.replace("##FEATURE_LIST##", strFeatureList)
        build = build.replace("##PLATFORM_NAME##", platformName)
        build = build.replace("##CURRENT_YEAR##", strCurrentYear)
        build = build.replace("##PACKAGE_CREATION_YEAR##", strPackageCreationYear)
        build = build.replace("##PACKAGE_COMPANY_NAME##", strPackageCompanyName)

        addConfigGroup = ""
        addConfigs = ""
        buildNuGetPackageConfigFile = None
        if package.PackageLanguage == PackageLanguage.CSharp:
            assemblyReferences = self.__GetAssemblyReferences(config, package, template.AssemblyReferenceSimple, template.AssemblyReferenceComplex, template.AssemblyReferenceComplex_Private)
            build = build.replace("##ADD_ASSEMBLY_REFERENCES##", assemblyReferences)
            buildNuGetPackageConfigFile = self.__TryGenerateNuGetPackageConfig(config, package, template.NuGetPackageConfig)
            if buildNuGetPackageConfigFile is not None:
                addConfigGroup = '\n  <ItemGroup>##ADD_CONFIGS##'
                addConfigGroup += '\n  </ItemGroup>'
                addConfigs = '\n    <None Include="packages.config" />'

        build = build.replace("##ADD_CONFIG_GROUP##", addConfigGroup)
        build = build.replace("##ADD_CONFIGS##", addConfigs)

        #build = build.replace("##VARIANT##", "")
        buildVC = build


        # generate bat file
        startProjectFile = self.__TryGenerateProjectBatFile(config, package, batTemplate.TemplateStartBat, batTemplate.TemplateSnippetErrorCheck, vsVersion, platformName, False)
        buildProjectFile = self.__TryPrepareProjectBatFile(config, package, batTemplate.TemplateBuildBat, batTemplate.TemplateSnippetErrorCheck, vsVersion, targetName, strFeatureList, platformName)
        runProjectFile = self.__TryPrepareProjectBatFile(config, package, batTemplate.TemplateRunBat, batTemplate.TemplateSnippetErrorCheck, vsVersion, targetName, strFeatureList, platformName)


        dstName = package.Name
        if (package.ResolvedPlatform is None or package.ResolvedPlatform.ProjectId is None):
            raise InternalErrorException("Could not find project name")
            #dstName = 'test'
        #dstName = 'test'
        dstFileSLN = IOUtil.Join(package.AbsolutePath, dstName + ".{0}".format(template.SolutionExtension))
        dstFileVC = IOUtil.Join(package.AbsolutePath, dstName + ".{0}".format(template.ProjectExtension))

        dstFileNuGetConfig = IOUtil.Join(package.AbsolutePath, "packages.config")

        dstFileStartProject = IOUtil.Join(package.AbsolutePath, LocalMagicFilenames.StartProject)

        buildBasePath = IOUtil.Join(package.AbsolutePath, package.ResolvedBuildPath)
        dstFileBuildProject = IOUtil.Join(buildBasePath, LocalMagicFilenames.BuildProject)
        dstFileRunProject = IOUtil.Join(buildBasePath, LocalMagicFilenames.RunProject)
        if not config.DisableWrite:
            IOUtil.SafeMakeDirs(buildBasePath)
            IOUtil.WriteFileIfChanged(dstFileSLN, buildSLN)
            IOUtil.WriteFileIfChanged(dstFileVC, buildVC)

            if startProjectFile is not None:
                IOUtil.WriteFileIfChanged(dstFileStartProject, startProjectFile)
            if buildProjectFile is not None:
                IOUtil.WriteFileIfChanged(dstFileBuildProject, buildProjectFile)
            if runProjectFile is not None:
                IOUtil.WriteFileIfChanged(dstFileRunProject, runProjectFile)
            if buildNuGetPackageConfigFile is not None:
                IOUtil.WriteFileIfChanged(dstFileNuGetConfig, buildNuGetPackageConfigFile)


        templateFileProcessor = TemplateFileProcessor(config, platformName)
        templateFileProcessor.Environment.Set("##FEATURE_LIST##", strFeatureList)
        templateFileProcessor.Process(config, template.TemplateFileRecordManager, package.AbsolutePath, package)


    def __GenerateExternalFilesToOutput(self, config: Config, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        if snippet is None or len(snippet) <= 0 or package.Type != PackageType.Executable:
            return ""
        if package.ResolvedBuildAllExternalDependencies is None:
            raise Exception("Invalid package")

        extDeps = Util.FilterByType(package.ResolvedBuildAllExternalDependencies, ExternalDependencyType.DLL)
        if len(extDeps) <= 0:
            return ""

        allVariantNames = GeneratorVCUtil.GenerateSLNVariantNames(variantHelper)
        if len(allVariantNames) < 1:
            allVariantNames.append('')

        result = ""
        for variantName in allVariantNames:
            for entry in extDeps:
                debugFilePath = IOUtil.Join(entry.ResolvedLocation.ResolvedPath, entry.DebugName)
                filePath = IOUtil.Join(entry.ResolvedLocation.ResolvedPath, entry.Name)

                content = snippet
                content = content.replace("##DEBUG_FILE_PATH##", debugFilePath)
                content = content.replace("##FILE_PATH##", filePath)
                content = content.replace("##VARIANT##", variantName)
                result += "\n" + content
        return result


    def __GetPackageResolvedBuildAllIncludeFiles(self, genFileName: str, package: Package) -> List[str]:
        if package.ResolvedBuildAllIncludeFiles is None:
            raise Exception("Invalid package")

        if not self.UsingLinuxTools:
            return package.ResolvedBuildAllIncludeFiles
        files = list(package.ResolvedBuildAllIncludeFiles)
        # we add 'fsl.gen' to ensure that the 'remote' copy process doesn't remove any dirs
        files.append(genFileName)
        return files


    def __TryPrepareProjectBatFile(self, config: Config, package: Package,
                                   templateBat: Optional[str],
                                   templateSnippetErrorCheck: str,
                                   vsVersion: int,
                                   targetName: str,
                                   strFeatureList: str,
                                   platformName: str) -> Optional[str]:
        buildProjectFile = self.__TryGenerateProjectBatFile(config, package, templateBat, templateSnippetErrorCheck, vsVersion, platformName, True)
        if buildProjectFile is None:
            return None

        buildProjectFile = buildProjectFile.replace("##PACKAGE_TARGET_NAME##", targetName)
        buildProjectFile = buildProjectFile.replace("##FEATURE_LIST##", strFeatureList)
        return buildProjectFile


    def __TryGenerateProjectBatFile(self, config: Config,
                                    package: Package,
                                    strBatTemplate: Optional[str],
                                    snippetErrorCheck: str,
                                    vsVersion: int,
                                    platformName: str,
                                    useEnvironmentConfig: bool) -> Optional[str]:
        if strBatTemplate is None or package.AbsolutePath is None:
            return None
        projectPath = config.ToDosPath(package.AbsolutePath)

        featureList = []
        if not useEnvironmentConfig:
            batCount = 0
            skipEGLHack = self.__ContainsOverrideFeature(package.ResolvedAllUsedFeatures)
            for feature in package.ResolvedAllUsedFeatures:
                if not skipEGLHack or feature.Id != 'egl':
                    scriptName = "Configure{0}.bat".format(feature.Name)
                    scriptPath = IOUtil.Join(config.SDKPath, ".Config")
                    scriptPath = IOUtil.Join(scriptPath, scriptName)
                    if IOUtil.IsFile(scriptPath):
                        batCount = batCount + 1
                        featureList.append("call {0} %{1}".format(scriptName, batCount))
                        featureList.append(snippetErrorCheck)
        else:
            featureToVariantDict = self.__MatchFeaturesToVariants(config, package)
            skipEGLHack = self.__ContainsOverrideFeature(package.ResolvedAllUsedFeatures)
            for feature in package.ResolvedAllUsedFeatures:
                if not skipEGLHack or feature.Id != 'egl':
                    scriptName = "Configure{0}.bat".format(feature.Name)
                    scriptPath = IOUtil.Join(config.SDKPath, ".Config")
                    scriptPath = IOUtil.Join(scriptPath, scriptName)
                    if IOUtil.IsFile(scriptPath):
                        featureEnvName = "%{0}{1}%".format(GEN_BUILD_ENV_FEATURE_SETTING, feature.Name.upper())
                        if feature.Name in featureToVariantDict:
                            variant = featureToVariantDict[feature.Name]
                            variantEnvName = "%{0}{1}%".format(GEN_BUILD_ENV_VARIANT_SETTING, variant.PurifiedName.upper())
                            featureList.append("call {0} {1} {2}".format(scriptName, variantEnvName, featureEnvName))
                        else:
                            featureList.append("call {0} {1}".format(scriptName, featureEnvName))
                        featureList.append(snippetErrorCheck)

        activeThirdPartyLibsDir = "" if self.__ActiveThirdPartyLibsDir is None else self.__ActiveThirdPartyLibsDir

        strVersion = "{0}".format(vsVersion)
        result = strBatTemplate
        result = result.replace("##PLATFORM_NAME##", platformName)
        result = result.replace("##VS_VERSION##", strVersion)
        result = result.replace("##PROJECT_NAME##", package.Name)
        result = result.replace("##CONFIG_SCRIPTS##", "\n".join(featureList))
        result = result.replace("##VS_VERSION##", strVersion)
        result = result.replace("##PROJECT_PATH##", projectPath)
        result = result.replace("##ACTIVE_THIRD_PARTY_LIBS_DIRECTORY##", activeThirdPartyLibsDir)
        return result

    def __ContainsOverrideFeature(self, allUsedFeatures: List[PackageRequirement]) -> bool:
        """ Hack to work around the EGL / GLES or VG issues, because VG has a custom EGL
            The main issue is from the fact that there is no proper link between the batch files and the features (or feature and the variant)
        """
        for feature in allUsedFeatures:
            if feature.Id.startswith("opengles") or feature.Id.startswith("openvg"):
                return True
        return False


    def __MatchFeaturesToVariants(self, config: Config, package: Package) -> Dict[str, PackagePlatformVariant]:
        virtualVariants = self.__GetVirtualVariants(package)
        featureToVariantDict = {}  # type: Dict[str, PackagePlatformVariant]
        for feature1 in package.ResolvedAllUsedFeatures:
            variant1 = self.__TryLocateFeatureOwningVariant(config, package, virtualVariants, feature1)
            if variant1:
                featureToVariantDict[feature1.Name] = variant1

        # Log warnings for failed matches
        if len(virtualVariants) > 0 and len(virtualVariants) != len(featureToVariantDict):
            variantToFeatureDict = {}  # type: Dict[str, str]
            for feature2, variant2 in list(featureToVariantDict.items()):
                variantToFeatureDict[variant2.Name] = feature2
            for entry in virtualVariants:
                if not entry.Name in variantToFeatureDict:
                    config.DoPrint("WARNING: Failed to match variant '{0}' with a feature".format(entry.Name))
        return featureToVariantDict


    def __TryLocateFeatureOwningVariant(self, config: Config,
                                        package: Package,
                                        virtualVariants: List[PackagePlatformVariant],
                                        feature: PackageRequirement) -> Optional[PackagePlatformVariant]:
        # Try a simple lookup first
        for variant in virtualVariants:
            if variant.IntroducedByPackageName == feature.IntroducedByPackageName:
                return variant
        # No such luck
        # Build a package name -> package lookup dict
        packageDict = {}  # type: Dict[str, Package]
        for entry in package.ResolvedBuildOrder:
            packageDict[entry.Name] = entry

        if not feature.IntroducedByPackageName in packageDict:
            raise Exception("Could not locate package '{0}' during feature resolve".format(feature.IntroducedByPackageName))

        featurePackage = packageDict[feature.IntroducedByPackageName]
        variants = self.__LocateVarientsInPackageDependencies(featurePackage.ResolvedBuildOrder)
        if len(variants) == 1:
            return list(variants.values())[0]
        if config.Verbosity > 1 and len(variants) > 1:
            config.DoPrint("WARNING: Could not determine which variant %s introduced the feature: %s" % (list(variants.keys()), feature.Name))
        return None


    def __LocateVarientsInPackageDependencies(self, packages: List[Package]) -> Dict[str, PackagePlatformVariant]:
        result = {}
        for entry in packages:
            for variant in entry.ResolvedDirectVariants:
                result[variant.Name] = variant
        return result



    def __GetVirtualVariants(self, package: Package) -> List[PackagePlatformVariant]:
        virtualVariants = []  # type: List[PackagePlatformVariant]
        for entry in list(package.ResolvedAllVariantDict.values()):
            if entry.Type == VariantType.Virtual:
                virtualVariants.append(entry)
        return virtualVariants


    # FIX: method defined twice (so disabled this one as the code has been calling the other one)
    #def __GetExternalLibraryDependencies(self, package):
    #    libPaths = []
    #    buildOrder = list(package.ResolvedBuildOrder)
    #    buildOrder.reverse()
    #    for entry in buildOrder:
    #        externalList = Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
    #        libPaths += externalList
    #    return libPaths


    def __CreateVisualStudioStyleFileList(self, snippet: str, entries: List[str]) -> str:
        if entries is None:
            return ""
        res = []
        for entry in entries:
            strContent = snippet.replace("##FILE_PATH##", entry.replace('/', '\\'))
            res.append(strContent)
        return "\n".join(res)


    def __CreateContentSourceFilesList(self, snippet: str, snippetEntry: str, entries: List[str]) -> str:
        if len(entries) <= 0:
            return ""
        fileEntries = self.__CreateVisualStudioStyleFileList(snippetEntry, entries)
        result = snippet
        result = result.replace("##SNIPPET##", fileEntries)
        return result


    def __CreateVisualStudioStyleList(self, entries: Union[List[str], Set[str]]) -> str:
        if entries is not None and len(entries) > 0:
            res = ";".join(entries)
            if not self.UsingLinuxTools:
                res = res.replace("/", "\\")
            return res
        else:
            return ""


    def __CreateVisualStudioStyleDefineList(self, entries: List[str]) -> str:
        if entries is not None and len(entries) > 0:
            return ";".join(entries).replace("/", "\\") + ';'
        return ""


    def __GenerateVariantPostfix(self, package : Package) -> str:
        variants = list(package.ResolvedAllVariantDict.keys())
        if len(variants) > 0:
            return "_" + "_".join(variants)
        return ""


    #def FilterVariants(self, variants):
    #    resDict = {}
    #    for entry in variants:
    #        if not entry.Type in resDict:
    #            resDict[entry.Type] = []
    #        resDict[entry.Type].append(entry)
    #    return resDict


    def __ContainsVariant(self, srcList: List[PackagePlatformVariant], variantName: str) -> bool:
        for entry in srcList:
            if entry.Name == variantName:
                return True
        return False


    def __BuildMatchedVariantName(self, parentVariantHelper: VariantHelper,
                                  currentConfiguration: Tuple[str, ...],
                                  variantHelper: VariantHelper) -> str:
        parentVariantList = parentVariantHelper.NormalVariants
        variantList = variantHelper.NormalVariants
        filteredVariantList = []
        parentVariantIndex = 0
        for entry in currentConfiguration:
            if self.__ContainsVariant(variantList, parentVariantList[parentVariantIndex].Name):
                filteredVariantList.append(entry)
            parentVariantIndex = parentVariantIndex + 1
        return "-".join(filteredVariantList)


    def __GenerateSLNPackageVariantConfig(self, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                sectionVariantName = GeneratorVCUtil.GetVCBuildConfigurationName(entry)
                section = snippet.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        return snippet.replace("##VARIANT##", '')


    def __VariantSimpleReplace(self, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        if len(variantHelper.CartesianProduct) > 0:
            lines = []
            for entry in variantHelper.CartesianProduct:
                section = snippet
                sectionVariantName = GeneratorVCUtil.GetVCBuildConfigurationName(entry)
                section = section.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)
        return snippet.replace("##VARIANT##", '')

    def __GenerateProjectConfigurations(self, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        return self.__VariantSimpleReplace(variantHelper, snippet, package)


    def __GenerateImportGroups(self, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        return self.__VariantSimpleReplace(variantHelper, snippet, package)


    def __GenerateVariantPropertyGroups(self, config: Config, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        defines = package.ResolvedBuildAllDefines
        if len(variantHelper.CartesianProduct) > 0:
            orgDefines = defines
            lines = []
            for entry in variantHelper.CartesianProduct:
                defines = list(orgDefines)
                localDefines = Util.ExtractNames(defines)
                localDefineNames = self.__CreateVisualStudioStyleDefineList(localDefines)

                if package.ResolvedVirtualVariantNameHint is None:
                    raise Exception("Invalid package")
                dynamicName = package.ResolvedVirtualVariantNameHint
                section = self.__GetVariantPropertySections(config, snippet, dynamicName, package)
                sectionVariantName = GeneratorVCUtil.GetVCBuildConfigurationName(entry)
                section = section.replace("##VARIANT##", sectionVariantName)
                section = section.replace("##PACKAGE_DEFINES##", localDefineNames)
                lines.append(section)
            return "\n".join(lines)

        localDefines = Util.ExtractNames(defines)
        localDefineNames = self.__CreateVisualStudioStyleDefineList(localDefines)

        if package.ResolvedMakeVariantNameHint is None:
            raise Exception("Invalid package")

        section = self.__GetVariantPropertySections(config, snippet, package.ResolvedMakeVariantNameHint, package)
        section = section.replace("##VARIANT##", '')
        section = section.replace("##PACKAGE_DEFINES##", localDefineNames)
        return section


    def __BuildGenerateCompilerSettingsGroupSection(self,
                                                    config: Config,
                                                    snippet: str, subSnippet1: str, subSnippet2: str,
                                                    package: Package,
                                                    includeDirs: List[str],
                                                    defines: List[PackageDefine],
                                                    variantExtDeps: List[PackageExternalDependency]) -> str:
        includeDirs = self.__FixIncludes(includeDirs)
        if self.UsingLinuxTools:
            includeDirs = self.__FixIncludesForLinuxTools(config, package, includeDirs)
        strIncludeDirs = self.__CreateVisualStudioStyleList(includeDirs)

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

        localDefines = Util.ExtractNames(defines)
        localDefineNames = self.__CreateVisualStudioStyleDefineList(localDefines)

        section = snippet
        section = section.replace("##PACKAGE_INCLUDE_DIRS##", strIncludeDirs)
        section = section.replace("##PACKAGE_DEFINES##", localDefineNames)
        section = section.replace('##PACKAGE_EXTERNAL_LINK_LIBRARY_DEPENDENCIES##', extLinkLibDepVC)
        section = section.replace('##PACKAGE_EXTERNAL_LINK_DEBUG_LIBRARY_DEPENDENCIES##', extLinkDebugLibDepVC)
        section = section.replace("##PACKAGE_EXTERNAL_LIBRARY_DEPENDENCIES##", extLibDepVC)
        section = section.replace("##PACKAGE_EXTERNAL_DEBUG_LIBRARY_DEPENDENCIES##", extLibDebugDepVC)
        return section


    def __FindByName(self, variantExtDeps: List[PackageExternalDependency], name: str) -> bool:
        for entry in variantExtDeps:
            if entry.Name == name:
                return True
        return False


    def AddOption(self, option: PackagePlatformVariantOption,
                  includeDirs: List[str],
                  rVariantExtDeps: List[PackageExternalDependency],
                  defines: List[PackageDefine]) -> None:
        # add external dependencies
        for entry1 in option.ExternalDependencies:
            if entry1.Include is not None and not entry1.Include in includeDirs:
                includeDirs.append(entry1.Include)
                # disabled since we did the exe links all fix
                #if entry1.IsFirstActualUse:
                #rVariantExtDeps.append(entry1)
            # due to the exe links all fix
            if not self.__FindByName(rVariantExtDeps, entry1.Name):
                rVariantExtDeps.append(entry1)

        # add cpp defines
        for entry2 in option.DirectDefines:
            defines.append(entry2)


    def __GenerateCompilerSettingsGroups(self, config: Config,
                                         variantHelper: VariantHelper,
                                         snippet: str, subSnippet1: str, subSnippet2: str,
                                         package: Package) -> str:
        if package.ResolvedBuildAllIncludeDirs is None:
            raise Exception("Invalid package")
        includeDirs = package.ResolvedBuildAllIncludeDirs  # type: List[str]
        defines = package.ResolvedBuildAllDefines  # type: List[PackageDefine]

        # Process virtual variants
        variantExtDeps = self.__GetExternalLibraryDependencies(package)
        if VariantType.Virtual in variantHelper.VariantTypeDict:
            for variant in variantHelper.VariantTypeDict[VariantType.Virtual]:
                if not len(variant.Options) == 1:
                    raise InternalErrorException("The virtual variant does not contain the expected option")
                self.AddOption(variant.Options[0], includeDirs, variantExtDeps, defines)

        if len(variantHelper.CartesianProduct) > 0:
            orgIncludeDirs = includeDirs
            orgVariantExtDeps = variantExtDeps
            orgDefines = defines

            lines = []
            for entry in variantHelper.CartesianProduct:
                includeDirs = list(orgIncludeDirs)
                variantExtDeps = list(orgVariantExtDeps)
                defines = list(orgDefines)

                variantList = variantHelper.NormalVariants
                variantIdx = 0
                for optionName in entry:
                    variant = variantList[variantIdx]
                    option = variant.OptionDict[optionName]
                    variantIdx = variantIdx + 1
                    self.AddOption(option, includeDirs, variantExtDeps, defines)

                #dynamicName = package.ResolvedVirtualVariantNameHint
                section = self.__BuildGenerateCompilerSettingsGroupSection(config, snippet, subSnippet1, subSnippet2, package, includeDirs, defines, variantExtDeps)
                sectionVariantName = GeneratorVCUtil.GetVCBuildConfigurationName(entry)
                section = section.replace("##VARIANT##", sectionVariantName)
                lines.append(section)
            return "\n".join(lines)

        section = self.__BuildGenerateCompilerSettingsGroupSection(config, snippet, subSnippet1, subSnippet2, package, includeDirs, defines, variantExtDeps)
        return section.replace("##VARIANT##", '')


    def __ApplyOptimizations(self, compilerSettingsGroups: str,
                             config: Config,
                             template: CodeTemplateVC,
                             package: Package) -> str:
        debugTemplate = template.DebugOptimizations[package.BuildCustomization.Debug.Optimization]
        compilerSettingsGroups = compilerSettingsGroups.replace("##DEBUG_OPTIMIZATION_TYPE##", debugTemplate.SnippetOptimizationType)
        compilerSettingsGroups = compilerSettingsGroups.replace("##DEBUG_OPTIMIZATION_OPTIONS##", debugTemplate.SnippetOptimizationOptions)
        return compilerSettingsGroups


    def __GetVariantPropertySections(self, config: Config, snippet: str, variantName: str, package: Package) -> str:
        if package.AbsolutePath is None:
            raise Exception("Invalid package")
        strContent = snippet.replace("##PROJECT_VARIANT_NAME##", variantName)
        strContent = strContent.replace("##RELATIVE_PACKAGE_PATH##", self.__ToRemotePath(config, package.AbsolutePath))
        return strContent


    def __FixIncludes(self, entries: List[str]) -> List[str]:
        res = []  # type: List[str]
        for entry in entries:
            if entry == 'include':
                entry = "$(ProjectDir)\\include" if not self.UsingLinuxTools else "include"
            res.append(entry)
        return res


    def __GenerateSLNDependencies1(self, package: Package) -> str:
        strContent = ""
        if len(package.ResolvedDirectDependencies) > 0:
            strContent += '\tProjectSection(ProjectDependencies) = postProject\n'
            for dep in package.ResolvedDirectDependencies:
                if self.__IsProject(dep.Package) and dep.Package.ResolvedPlatform is not None and dep.Package.ResolvedPlatform.ProjectId is not None:
                    strContent += '\t\t{%s} = {%s}\n' % (dep.Package.ResolvedPlatform.ProjectId, dep.Package.ResolvedPlatform.ProjectId)
            strContent += '\tEndProjectSection\n'
        return strContent


    def __IsProject(self, package: Package) -> bool:
        return not package.IsVirtual or (self.UsingLinuxTools and package.Type == PackageType.HeaderLibrary)


    def __GenerateSLNDependencies2(self, config: Config, package: Package, templateAddProject: str, projectExtension: str) -> str:
        strContent = ""
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform is not None and package != entry:
                projectName = entry.Name
                projectPath = config.TryLegacyToDosPathDirectConversion(entry.AbsolutePath)
                # To use relative paths instead of absolute
                # NOTE: This was disabled because it doesn't play well with 'msbuild' (even though visual studio has no problems with it).
                #projectPath = config.TryLegacyToDosPath(entry.AbsolutePath)
                projectPath = "{0}\\{1}.{2}".format(projectPath, projectName, projectExtension)
                if entry.ResolvedPlatform.ProjectId is None:
                    raise Exception("Invalid package")
                projectId = entry.ResolvedPlatform.ProjectId
                content = templateAddProject
                content = content.replace("##PACKAGE_TARGET_NAME##", projectName)
                content = content.replace("##PACKAGE_PROJECT_FILE##", projectPath)
                content = content.replace("##PACKAGE_PLATFORM_PROJECT_ID##", projectId)
                strContent += "\n" + content
        return strContent


    def __HasSubProjects(self, package: Package) -> bool:
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                return True
        return False

    def __GenerateFolderDefinitions(self, config: Config, package: Package, snippet: str) -> str:
        if not self.__HasSubProjects(package):
            return ""
        return "\n" + snippet


    def __GenerateFolderEntries(self, config: Config, package: Package, snippet: str, subSnippet: str) -> str:
        if not self.__HasSubProjects(package) or len(snippet) <= 0:
            return ""

        subContent = ""
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform is not None and entry.ResolvedPlatform.ProjectId is not None and package != entry:
                section = "\n" + subSnippet
                section = section.replace("##PACKAGE_PLATFORM_PROJECT_ID##", entry.ResolvedPlatform.ProjectId)
                subContent += section

        content = snippet
        content = content.replace('##SNIPPET##', subContent)
        return "\n" + content


    def __GenerateSLNPackageDependency(self, snippet: str, package: Package, sectionVariantName1: str, sectionVariantName2: str) -> str:
        if package.ResolvedPlatform is None or package.ResolvedPlatform.ProjectId is None:
            raise Exception("Invalid package")
        section = snippet
        section = section.replace("##PACKAGE_PLATFORM_PROJECT_ID##", package.ResolvedPlatform.ProjectId)
        section = section.replace("##VARIANT1##", sectionVariantName1)
        section = section.replace("##VARIANT2##", sectionVariantName2)
        return section


    def __GenerateSLNPackageVariants(self, variantHelper: VariantHelper, snippet: str) -> str:
        if len(variantHelper.CartesianProduct) <= 0:
            return self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, '', '')

        lines = []
        for entry in variantHelper.CartesianProduct:
            sectionVariantName = GeneratorVCUtil.GetVCBuildConfigurationName(entry)
            section = self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, sectionVariantName, sectionVariantName)
            lines.append(section)
        return "\n".join(lines)


    def __GenerateAndMatchSLNPackageVariants(self, parentVariantHelper: VariantHelper, variantHelper: VariantHelper, snippet: str) -> str:
        if len(parentVariantHelper.CartesianProduct) <= 0:
            return self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, '', '')

        lines = []
        for entry in parentVariantHelper.CartesianProduct:
            sectionVariantName = GeneratorVCUtil.GetVCBuildConfigurationName(entry)
            matchedVariantName = self.__BuildMatchedVariantName(parentVariantHelper, entry, variantHelper)
            section = self.__GenerateSLNPackageDependency(snippet, variantHelper.Package, sectionVariantName, matchedVariantName)
            lines.append(section)
        return "\n".join(lines)


    def __GenerateSLNDependencies3(self, variantHelper: VariantHelper, snippet: str, package: Package) -> str:
        res = []
        for entry in package.ResolvedBuildOrder:
            if self.__IsProject(entry) and entry.ResolvedPlatform != None and package != entry:
                entryVariantHelper = VariantHelper(entry)
                section = self.__GenerateAndMatchSLNPackageVariants(variantHelper, entryVariantHelper, snippet)
                res.append(section)
        return "\n".join(res) + "\n" if len(res) > 0 else ""


    def __GenerateVCDependencies(self, snippet: str, config: Config, package: Package, projectExtension: str) -> str:
        res = []
        if len(package.ResolvedBuildOrder) > 1:
            for entry in package.ResolvedBuildOrder:
                if self.__IsProject(entry) and entry.ResolvedPlatform is not None and package != entry:
                    if entry.AbsolutePath is None or entry.ResolvedPlatform.ProjectId is None:
                        raise Exception("Invalid package")
                    projectPath = "{0}\\{1}.{2}".format(config.ToPath(entry.AbsolutePath).replace('/', '\\'), entry.Name, projectExtension)
                    projectId = entry.ResolvedPlatform.ProjectId
                    strContent = snippet.replace("##PACKAGE_DEPENDENCY_PROJECT_PATH##", projectPath)
                    strContent = strContent.replace("##PACKAGE_DEPENDENCY_PROJECTID##", projectId.lower())
                    strContent = strContent.replace("##PACKAGE_NAME##", entry.Name)
                    res.append(strContent)
        return "\n".join(res)


    def __GenerateVCExternalLinkDependencies(self, snippet1: str, snippet2: str,
                                             package: Package,
                                             variantExtDeps: List[PackageExternalDependency],
                                             useDebugLibs: bool) -> str:
        strContent = ""
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
                # sort these to ensure we get the same order everytime.
                sortedAdditionalDependenciesList = list(additionalDependencies)
                sortedAdditionalDependenciesList.sort()
                strAdditionalDependencies = self.__CreateVisualStudioStyleList(sortedAdditionalDependenciesList)
                strContent += "\n" + snippet1.replace("##PACKAGE_EXTERNAL_LINK_DEPENDENCIES##", strAdditionalDependencies)
            if len(additionalLibraryDirectories) > 0:
                # sort these to ensure we get the same order everytime.
                sortedAdditionalDependencyList = list(additionalLibraryDirectories)
                sortedAdditionalDependencyList.sort()
                strAdditionalLibraryDirectories = self.__CreateVisualStudioStyleList(sortedAdditionalDependencyList)
                strContent += "\n" + snippet2.replace("##PACKAGE_EXTERNAL_LIBRARY_DIRECTORIES##", strAdditionalLibraryDirectories)
        return strContent


    def __GenerateVCExternalLibDependencies(self, package: Package, variantExtDeps: List[PackageExternalDependency]) -> str:
        strContent = ""
        extDeps = Util.FilterByType(package.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
        extDeps2 = Util.FilterByType(variantExtDeps, ExternalDependencyType.StaticLib)
        extDeps += extDeps2
        if len(extDeps) > 0:
            additionalDependencies = set()
            additionalLibraryDirectories = set()
            for entry in extDeps:
                additionalDependencies.add(entry.Name)
                if entry.Location is not None:
                    additionalLibraryDirectories.add(entry.Location)
            strContent += '\n    <Lib>\n'
            if len(additionalDependencies) > 0:
                # sort these to ensure we get the same order everytime.
                sortedAdditionalDependenciesList = list(additionalDependencies)
                sortedAdditionalDependenciesList.sort()
                strAdditionalDependencies = self.__CreateVisualStudioStyleList(sortedAdditionalDependenciesList)
                strContent += '      <AdditionalDependencies>{0}</AdditionalDependencies>\n'.format(strAdditionalDependencies)
            if len(additionalLibraryDirectories) > 0:
                # sort these to ensure we get the same order everytime.
                sortedAdditionalDependencyList = list(additionalLibraryDirectories)
                sortedAdditionalDependencyList.sort()
                strAdditionalLibraryDirectories = self.__CreateVisualStudioStyleList(sortedAdditionalDependencyList)
                strContent += '      <AdditionalLibraryDirectories>%s</AdditionalLibraryDirectories>\n' % (strAdditionalLibraryDirectories)
            strContent += '    </Lib>'
        return strContent


    def __GetExternalLibraryDependencies(self, package: Package) -> List[PackageExternalDependency]:
        libPaths = []  # type: List[PackageExternalDependency]
        for entry in package.ResolvedBuildOrder:
            externalList = Util.FilterByType(entry.ResolvedDirectExternalDependencies, ExternalDependencyType.StaticLib)
            libPaths += externalList
        return libPaths


    def __GetExternalAssemblyDependencies(self, package: Package) -> List[PackageExternalDependency]:
        externalAssemblyDeps = Util.FilterByType(package.ResolvedDirectExternalDependencies, ExternalDependencyType.Assembly)

        for entry in package.ResolvedAllDependencies:
            if entry.Package.IsVirtual and entry.Access == AccessType.Public:
                externalList = Util.FilterByType(entry.Package.ResolvedDirectExternalDependencies, ExternalDependencyType.Assembly)
                externalAssemblyDeps += externalList

        for entry in package.ResolvedDirectDependencies:
            if entry.Package.IsVirtual and entry.Access != AccessType.Public:
                externalList = Util.FilterByType(entry.Package.ResolvedDirectExternalDependencies, ExternalDependencyType.Assembly)
                externalAssemblyDeps += externalList

        uniqueEntries = {entry.Name: entry for entry in externalAssemblyDeps}

        return list(uniqueEntries.values())


    def __GetAssemblyName(self, entry: PackageExternalDependency) -> str:
        name = entry.Name
        if entry.Version != None:
            name += ", Version={0}".format(entry.Version)
        if entry.Culture != None:
            name += ", Culture={0}".format(entry.Culture)
        if entry.PublicKeyToken != None:
            name += ", PublicKeyToken={0}".format(entry.PublicKeyToken)
        if entry.ProcessorArchitecture != None:
            name += ", processorArchitecture={0}".format(entry.ProcessorArchitecture)
        return name


    def __GetAssemblyReferences(self, config: Config, package: Package, snippetSimple: Optional[str], snippetComplex: Optional[str], snippetComplexPrivate: Optional[str]) -> str:
        assemblyList = self.__GetExternalAssemblyDependencies(package)  # type: List[PackageExternalDependency]
        if len(assemblyList) <= 0:
            return ""
        if snippetSimple is None or snippetComplex is None or snippetComplexPrivate is None:
            raise Exception("We have external assembly refernces but no valid code snippet to insert.")

        result = ""
        for entry in assemblyList:
            strPrivate = "" if entry.Access == AccessType.Public else snippetComplexPrivate
            content = "\n"
            if entry.HintPath is not None:
                content += snippetComplex
                content = content.replace("##PACKAGE_DEPENDENCY_HINT_PATH##", entry.HintPath)
            else:
                content += snippetSimple
            content = content.replace("##PACKAGE_DEPENDENCY_INCLUDE##", self.__GetAssemblyName(entry))
            content = content.replace("##PACKAGE_DEPENDENCY_PRIVATE##", strPrivate)
            result += content
        return result


    def __TryGenerateNuGetPackageConfig(self, config: Config, package: Package, nuGetPackageConfig: Optional[NuGetPackageConfigSnippets]) -> Optional[str]:
        externalAssemblyList = self.__GetExternalAssemblyDependencies(package)  # type: List[PackageExternalDependency]

        externalDependencies = []
        for entry in externalAssemblyList:
            if entry.PackageManager is not None and entry.PackageManager.Name == "NuGet":
                externalDependencies.append(entry)

        if len(externalDependencies) <= 0:
            return None
        if nuGetPackageConfig is None:
            raise Exception("No PackageConfig snippet supplied")

        contentPackageList = ""
        for entry in externalDependencies:
            if entry.PackageManager is None:
                raise Exception("Invalid package")
            content = "\n" + nuGetPackageConfig.PackageEntry
            content = content.replace("##PACKAGE_NAME##", entry.Name)
            if entry.PackageManager.PackageVersion is None or entry.PackageManager.PackageTargetFramework is None:
                raise Exception("Not supported")
            content = content.replace("##PACKAGE_VERSION##", entry.PackageManager.PackageVersion)
            content = content.replace("##PACKAGE_TARGET_FRAMEWORK##", entry.PackageManager.PackageTargetFramework)
            contentPackageList += content

        content = nuGetPackageConfig.Master
        content = content.replace("##PACKAGE_LIST##", contentPackageList)
        return content

    def __FixIncludesForLinuxTools(self, config: Config, package: Package, includeDirs: List[str]) -> List[str]:
        if package.AbsolutePath is None:
            raise Exception("Invalid package")
        packagePath = config.ToBashPath(package.AbsolutePath)
        packagePath = packagePath.replace('$', '__')
        basePath = self.__GenerateBackpedalToRoot(packagePath)

        newDirs = []  # type: List[str]
        for entry in includeDirs:
            if entry == package.BaseIncludePath:
                newDirs.append(entry)
            else:
                newName = Util.ChangeToBashEnvVariables(entry)
                newName = newName.replace('$', '__')
                newName = IOUtil.Join(basePath, newName)
                newDirs.append(newName)
        return newDirs


    def __CountDirectories(self, path: str, count: int = 1) -> int:
        name = IOUtil.GetDirectoryName(path)
        if len(name) <= 0 or name == '.':
            return count
        return self.__CountDirectories(name, count + 1)


    def __GenerateBackpedalToRoot(self, path: str) -> str:
        directoryCount = self.__CountDirectories(path)
        backPedal = []
        for i in range(0, directoryCount):
            backPedal.append('..')
        return '/'.join(backPedal)


    def __ToRemotePath(self, config: Config, path: str) -> str:
        path = config.ToBashPath(path)
        path = path.replace('$', '__')
        path = IOUtil.Join("$(RemoteRootDir)", path)
        return path


class GeneratorVCUtil(object):
    @staticmethod
    def GetVCBuildConfigurationName(entry: Iterable[str]) -> str:
        return "-".join(entry)


    @staticmethod
    def GenerateSLNVariantNames(variantHelper: VariantHelper) -> List[str]:
        variantNames = []
        if len(variantHelper.CartesianProduct) > 0:
            #lines = []
            for entry in variantHelper.CartesianProduct:
                variantNames.append(GeneratorVCUtil.GetVCBuildConfigurationName(entry))
        return variantNames


    @staticmethod
    def GetTargetName(package: Package) -> str:
        return package.Name


    @staticmethod
    def TryGenerateBuildReport(log: Log, generatorName: str, package: Package, variantHelper: VariantHelper) -> Optional[GeneratorBuildReport]:
        if package.IsVirtual:
            return None

        if package.ResolvedBuildPath is None:
            raise Exception("Invalid package")

        activeVariantName = "{0}${{{1}}}".format(variantHelper.ResolvedNormalVariantNameHint, LocalMagicBuildVariants.GeneratorConfig)
        buildCommandArguments = ["/p:Configuration={0}".format(activeVariantName)]

        # Unfortunately this causes the libs to not be copied from src -> target location too :(
        # buildCommandArguments += ["/p:BuildProjectReferences=false"]

        buildCommand = IOUtil.Join(package.ResolvedBuildPath, LocalMagicFilenames.BuildProject)
        buildCommandReport = GeneratorCommandReport(False, buildCommand, buildCommandArguments)
        return GeneratorBuildReport(buildCommandReport)


    @staticmethod
    def TryGenerateExecutableReport(log: Log, generatorName: str, package: Package, variantHelper: VariantHelper) -> Optional[GeneratorExecutableReport]:
        if package.Type != PackageType.Executable or package.IsVirtual:
            return None

        if package.ResolvedBuildPath is None:
            raise Exception("Invalid package")

        targetName = GeneratorVCUtil.GetTargetName(package)

        # $(SolutionDir)\build\##PLATFORM_NAME##\##PACKAGE_TARGET_NAME##\$(Configuration)##PROJECT_VARIANT_NAME##\
        # $(SolutionDir)\build\Windows\S06_Texturing\$(Configuration)_$(FSL_GLES_NAME)\
        allVariantNames = GeneratorVCUtil.GenerateSLNVariantNames(variantHelper)
        if len(allVariantNames) < 1:
            allVariantNames.append('')

        configVariantName = "${{{0}}}".format(LocalMagicBuildVariants.GeneratorConfig)

        foundBuildPath = IOUtil.Join(package.ResolvedBuildPath, targetName)
        normalVariantFormatString = variantHelper.ResolvedNormalVariantNameHint
        exeFormatString = "{0}/{1}{2}{3}/{4}.exe".format(foundBuildPath, normalVariantFormatString, configVariantName, package.ResolvedVirtualVariantNameHint, targetName)
        runScript = "{0}/{1}".format(package.ResolvedBuildPath, LocalMagicFilenames.RunProject)

        return GeneratorExecutableReport(False, exeFormatString, runScript, FormatStringEnvironmentVariableResolveMethod.OSShellEnvironmentVariable)


    @staticmethod
    def TryGenerateGeneratorPackageReport(log: Log, generatorName: str, package: Package) -> Optional[PackageGeneratorReport]:
        if package.IsVirtual:
            return None

        variableReport = GeneratorVariableReport(log)
        variableReport.Add(LocalMagicBuildVariants.GeneratorConfig, [LocalMagicBuildVariantOption.Debug, LocalMagicBuildVariantOption.Release], ToolAddedVariant.CONFIG)

        variantHelper = VariantHelper(package)
        buildReport = GeneratorVCUtil.TryGenerateBuildReport(log, generatorName, package, variantHelper)
        executableReport = GeneratorVCUtil.TryGenerateExecutableReport(log, generatorName, package, variantHelper)
        return PackageGeneratorReport(buildReport, executableReport, variableReport)
