#!/usr/bin/env python3

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

from typing import List
from typing import Optional
from typing import Set
import os
import os.path
import subprocess
from FslBuildGen import IOUtil
from FslBuildGen.Log import Log
from FslBuildGen.BuildConfig.CustomPackageFileFilter import CustomPackageFileFilter
from FslBuildGen.BuildConfig.PerformClangUtil import PerformClangUtil
from FslBuildGen.DataTypes import CheckType
from FslBuildGen.DataTypes import PackageType
from FslBuildGen.Exceptions import ExitException
from FslBuildGen.Packages.Package import Package

__g_includeExtensionList = [".h", ".hpp"]
__g_sourceExtensionList = [".cpp", ".c"]
__g_shaderExtensionList = [".frag", ".vert", ".geom", ".tesc", ".tese"]

_g_companyName = " NXP"
_g_copyright = "Copyright "

#__g_thirdParty = '/ThirdParty/'

class SourceFile(object):
    def __init__(self, package: Package, fileName: str) -> None:
        super().__init__()
        self.Package = package
        self.FileName = fileName
        self.Content = IOUtil.ReadFile(fileName)
        lines = self.Content.split('\n')
        self.LinesOriginal = [line.rstrip() for line in lines]
        self.BasePath = self.__TryDetermineFileBasePath(package, fileName) # type: Optional[str]
        self.LinesModded = list(self.LinesOriginal)
        self.__NormalizeTrailingEndingLines(self.LinesModded)
        if len(self.LinesModded[len(self.LinesModded)-1]) != 0:
            raise Exception("Not ending with a empty line")


    def __TryDetermineFileBasePath(self, package: Package, fileName: str) -> Optional[str]:
        if package.AbsoluteIncludePath and fileName.startswith(package.AbsoluteIncludePath):
            return package.AbsoluteIncludePath
        elif package.AbsoluteSourcePath and fileName.startswith(package.AbsoluteSourcePath):
            return package.AbsoluteSourcePath
        return None


    def __CountTrailingEmpty(self, inputList: List[str]) -> int:
        numEmptyLines = len(inputList)-1
        i = numEmptyLines
        while len(inputList[i]) == 0:
            i = i - 1
        return numEmptyLines - i


    def __NormalizeTrailingEndingLines(self, listToMod: List[str]) -> None:
        count = self.__CountTrailingEmpty(listToMod)
        if count == 1:
            return
        elif count < 1:
            listToMod.append('')
        else:
            count = count - 1
            while count > 0:
                del listToMod[len(listToMod)-1]
                count = count - 1



#def ValidateShaderVersionTag(file, shortFile):
#    lines = [line.strip() for line in open(file)]

#    for idx, line in enumerate(lines):
#        if idx != 0 and line.startswith("#version"):
#            # "#version must be in the first line of the file not at line: %s" % (idx);
#            return False
#    return True




#def ScanSource():
#    for rootPath in appDirectoryRoot:
#        appDirs = GetDirectories(rootPath)
#        for path in appDirs:
#            path += "/"
#            files = ScanContent(path)
#            pathLen = len(path);
#            for file in files:
#                shortFile = file[pathLen:].replace('\\','/');
#                if not ValidateShaderVersionTag(file, shortFile):
#                    print("Failed: %s", os.path.normpath(file))

def __Decode(s: str) -> bytes:
    try:
        return s.encode("utf-8-sig")
    except UnicodeDecodeError:
        pass
    return s.encode("latin-1")


def __Decoded(s: str) -> str:
    decodedStr = __Decode(s)
    return decodedStr.decode("utf-8")


def __GenerateIncludeGuardName(package: Package, fileName: str) -> str:
    segments = fileName.split('/')

    name = ''
    for i in range(1, len(segments)-1):
        name += segments[i].upper() + '_'

    finalName = segments[len(segments)-1]
    if package.Type == PackageType.Executable:
        finalName = "%s_%s" % (package.Name, finalName)

    return name + finalName.replace('.', '_').upper()


def __ValidateIncludeGuard(log: Log, sourceFile: SourceFile, shortFile: str, repairEnabled: bool) -> bool:
    if len(sourceFile.LinesModded) < 2:
        return False

    currentLine0 = sourceFile.LinesModded[0].strip()
    currentLine1 = sourceFile.LinesModded[1].strip()

    guard = __GenerateIncludeGuardName(sourceFile.Package, shortFile)
    line0Valid = "#ifndef %s" % (guard)
    line1Valid = "#define %s" % (guard)
    if currentLine0 == line0Valid and currentLine1 == line1Valid:
        return True

    # check that the file starts with the guard statements
    prefix0 = "#ifndef "
    prefix1 = "#define "
    if not currentLine0.startswith(prefix0):
        log.DoPrint("Line 0 does not start with '%s' in '%s'" % (prefix0, os.path.normpath(sourceFile.FileName)))
        if repairEnabled:
            log.LogPrint("Because of this repair was not attempted.")
        return False
    if not currentLine1.startswith(prefix1):
        log.DoPrint("Line 1 does not start with '%s' in '%s'" % (prefix1, os.path.normpath(sourceFile.FileName)))
        if repairEnabled:
            log.LogPrint("Because of this repair was not attempted.")
        return False

    # validate that the #ifndef and define works on the same string
    userDef0 = currentLine0[len(prefix0):].strip()
    userDef1 = currentLine1[len(prefix1):].strip()
    if userDef0 != userDef1:
        log.DoPrint("The include guards do not appear to match '%s' != '%s' in '%s'" % (userDef0, userDef1, os.path.normpath(sourceFile.FileName)))
        log.LogPrint("- Line 0 '%s'" % (userDef0))
        log.LogPrint("- Line 1 '%s'" % (userDef1))
        if repairEnabled:
            log.LogPrint("Because of this repair was not attempted.")
        return False

    # So we should be sure that the guard is just the incorrect name, so list it
    log.DoPrint("Wrong include guard: '%s' expected '%s'" % (os.path.normpath(sourceFile.FileName), guard))
    if currentLine0 != line0Valid:
        log.LogPrint("- Expected '%s'" % (line0Valid))
        log.LogPrint("- Was      '%s'" % (currentLine0))
    elif currentLine1 != line1Valid:
        log.LogPrint("- Expected '%s'" % (line1Valid))
        log.LogPrint("- Was      '%s'" % (currentLine1))

    if not repairEnabled:
        return False

    log.DoPrint("Include guard corrected")

    # We are allowed to repair the content, so lets do that
    sourceFile.LinesModded[0] = line0Valid
    sourceFile.LinesModded[1] = line1Valid
    return False


def __IsAscii(srcStr: str) -> bool:
    """ Check if a string only contains ASCII characters """
    try:
        srcStr.encode('ascii')
        return True
    except UnicodeDecodeError:
        return False
    except UnicodeEncodeError:
        return False


def __IndexOfNonAscii(srcStr: str, startIndex: int = 0) -> int:
    for index in range(startIndex, len(srcStr)):
        if ord(srcStr[index]) >= 128:
            return index
    return -1


def __CheckASCII(log: Log, sourceFile: SourceFile, repairEnabled: bool) -> bool:
    errorCount = 0
    for index, line in enumerate(sourceFile.LinesOriginal):
        if not __IsAscii(line):
            posX = __IndexOfNonAscii(line, 0)
            while posX >= 0:
                ch = hex(ord(line[posX])) if index >= 0 else '-failed-'
                log.DoPrint("Non ASCII character '{0}' encountered at X:{1}, Y:{2} in '{3}'".format(ch, posX+1, index+1, os.path.normpath(sourceFile.FileName)))
                errorCount = errorCount + 1
                #if repairEnabled:
                #    line[posX] = ' '  # disabled because its too dangerous
                posX = __IndexOfNonAscii(line, posX+1)
    return errorCount == 0


def __IsValidExtension(fileName: str, validExtensions: List[str]) -> bool:
    fileNameId = fileName.lower()
    for entry in validExtensions:
        if fileNameId.endswith(entry):
            return True
    return False


def __CheckIncludeGuard(log: Log, sourceFile: SourceFile, repairEnabled: bool) -> bool:
    if not sourceFile.BasePath:
        return True

    pathLen = len(sourceFile.BasePath)
    shortFile = sourceFile.FileName[pathLen:].replace('\\', '/')
    return __ValidateIncludeGuard(log, sourceFile, shortFile, repairEnabled)


def __CheckTabs(log: Log, sourceFile: SourceFile, repairEnabled: bool, thirdpartyExceptionDir: Optional[str]) -> bool:
    #if __g_thirdParty in sourceFile.FileName or (thirdpartyExceptionDir is not None and sourceFile.BasePath is not None and sourceFile.BasePath.startswith(thirdpartyExceptionDir)):
    if (thirdpartyExceptionDir is not None and sourceFile.BasePath is not None and sourceFile.BasePath.startswith(thirdpartyExceptionDir)):
        return True

    tabCount = 0
    for line in sourceFile.LinesModded:
        tabCount += line.count('\t')
    if tabCount == 0:
        return True

    log.DoPrint("Found %s tab characters in '%s'" % (tabCount, os.path.normpath(sourceFile.FileName)))
    return False


def __Repair(log: Log, sourceFile: SourceFile, asciiRepair: bool, disableWrite: bool) -> None:

    strContent = "\n".join(sourceFile.LinesModded)
    if asciiRepair:
        strContent = __Decoded(strContent)

    if strContent != sourceFile.Content:
        log.DoPrint("Repaired '%s'" % (os.path.normpath(sourceFile.FileName)))
        if not disableWrite:
            IOUtil.WriteFile(sourceFile.FileName, strContent)

def __IndexOfCopyrightLine(lines: List[str], maxLinesToCheck: int) -> int:
    linesToCheck = min(len(lines), maxLinesToCheck)
    for index in range(linesToCheck):
        lineContent = lines[index]
        if lineContent.endswith(_g_companyName) and _g_copyright in lineContent:
            return index
    return -1


def __TryExtractCopyrightYears(log: Log, copyrightYears: str, debugInfo: str) -> Optional[Set[int]]:
    copyrightYearsStringArray = copyrightYears.split(',')
    copyrightYearsArray = set() # type: Set[int]
    for index in range(len(copyrightYearsStringArray)):
        strYear = copyrightYearsStringArray[index].strip()
        if '-' in strYear:
            yearRange = strYear.split('-')
            if len(yearRange) != 2:
                log.DoPrintError("{0}: Year range not in the expected format 'start-end' got '{1}'".format(debugInfo, strYear))
                return None
            yearRangeStart = int(yearRange[0])
            yearRangeEnd = int(yearRange[1])
            if yearRangeStart >= yearRangeEnd:
                log.DoPrintError("{0}: year range begin must be less than end. Got '{1}'".format(debugInfo, strYear))
                return None
            for yearInRange in range(yearRangeStart, yearRangeEnd + 1):
                copyrightYearsArray.add(yearInRange)
        else:
            copyrightYearsArray.add(int(strYear))
    return copyrightYearsArray

def __TryToCopyrightYearString(yearsSet: Set[int]) -> Optional[str]:
    if len(yearsSet) < 1:
        return None

    years = list(yearsSet)
    years.sort()

    yearRanges = [] # type: List[str]

    startYear = years[0]
    previousYear = startYear
    for index in range(1, len(years)):
        year = years[index]
        if (previousYear + 1) != year:
            # new year is not part of the current range
            if startYear == previousYear:
                yearRanges.append(str(startYear))
            else:
                yearRanges.append("{0}-{1}".format(startYear, previousYear))
            startYear = year
        previousYear = year

    if startYear == previousYear:
        yearRanges.append(str(startYear))
    else:
        yearRanges.append("{0}-{1}".format(startYear, previousYear))
    return ", ".join(yearRanges)


def __SafeJoinCommandArguments(strings: List[str]) -> str:
    res = []
    for entry in strings:
        if ' ' in entry:
            entry = '"{0}"'.format(entry)
        res.append(entry)
    return " ".join(res)


def __TryCheckForFileModifications(log: Log, gitExeName: str, filename: str, yearsSet: Set[int], minimumLinesChanged: int) -> Optional[int]:
    try:
        # diff  --stat release/5.6.0 master VertexMatrix.hpp
        #latestBranch = 'release/5.6.0'
        oldestBranch = 'master'
        #runCommands = [gitExeName, 'diff', '--stat', latestBranch, oldestBranch, '--', filename]
        runCommands = [gitExeName, 'diff', '--stat', oldestBranch, '--', filename]
        currentWorkingDirectory = IOUtil.GetDirectoryName(filename)
        if log.Verbosity >= 1:
            log.LogPrint("Running run command '{0}' in '{1}'".format(__SafeJoinCommandArguments(runCommands), currentWorkingDirectory))
        res = subprocess.check_output(runCommands, cwd=currentWorkingDirectory, universal_newlines=True)
        searchString = ' 1 file changed, '
        index = res.find(searchString)
        if index >= 0:
            res = res[index + len(searchString):]
            endIndex = res.find('insertions')
            if endIndex >= 0:
                res = res[:endIndex].strip()
                linesChanged = int(res)
                if linesChanged > minimumLinesChanged:
                    # git log -1 --format="%as" -- filename
                    runCommands = [gitExeName, 'log', '-1', '--format="%as"', '--', filename]
                    res = subprocess.check_output(runCommands, cwd=currentWorkingDirectory, universal_newlines=True).strip()
                    if res.startswith('"'):
                        res = res[1:]
                    if res.endswith('"'):
                        res = res[:-1]
                    dateInfo = res.split('-')
                    if len(dateInfo) == 3:
                        lastModificationYear = int(dateInfo[0].strip())
                        return lastModificationYear
        return None
    except FileNotFoundError:
        log.LogPrintWarning("The run command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(__SafeJoinCommandArguments(runCommands), currentWorkingDirectory))
        raise
    except subprocess.CalledProcessError:
        log.LogPrintWarning("The run command '{0}' failed with 'file not found'. It was run with CWD: '{1}'".format(__SafeJoinCommandArguments(runCommands), currentWorkingDirectory))
        raise

def __CheckCopyright(log: Log, sourceFile: SourceFile, gitExeName: str) -> bool:
    copyRightLineIndex = __IndexOfCopyrightLine(sourceFile.LinesModded, 10)
    if copyRightLineIndex < 0:
        return True
    copyrightLine = sourceFile.LinesModded[copyRightLineIndex]

    startIndex = copyrightLine.index(_g_copyright) + len(_g_copyright)
    endIndex = len(copyrightLine) - len(_g_companyName)

    copyrightYears = copyrightLine[startIndex:endIndex].strip()
    copyrightYearsSet = __TryExtractCopyrightYears(log, copyrightYears, sourceFile.FileName)
    if copyrightYearsSet is None:
        return False

    lastModifiedYear = __TryCheckForFileModifications(log, gitExeName, sourceFile.FileName, copyrightYearsSet, 10)
    if lastModifiedYear is not None:
        copyrightYearsSet.add(lastModifiedYear)

    finalYearString = __TryToCopyrightYearString(copyrightYearsSet)
    if finalYearString is None:
        return False

    newCopyrightLine = copyrightLine[:startIndex] + finalYearString + copyrightLine[endIndex:]
    if copyrightLine != newCopyrightLine:
        sourceFile.LinesModded[copyRightLineIndex] = newCopyrightLine
    return True


def __ProcessIncludeFile(log: Log, package: Package, fullPath: str, repairEnabled: bool, thirdpartyExceptionDir: Optional[str],
                         disableWrite: bool, gitExeName: str, checkCopyright: bool) -> bool:
    log.LogPrintVerbose(10, "- Scanning '{0}'".format(fullPath))
    noErrors = True
    asciiRepair = False
    sourceFile = SourceFile(package, fullPath)
    #if not __g_thirdParty in sourceFile.FileName or (thirdpartyExceptionDir is not None and sourceFile.BasePath is not None and sourceFile.BasePath.startswith(thirdpartyExceptionDir)):
    if not __CheckIncludeGuard(log, sourceFile, repairEnabled):
        noErrors = False
    if not __CheckASCII(log, sourceFile, repairEnabled):
        # The ASCII repair is not safe, so dont do it
        #asciiRepair = True
        noErrors = False
    if not __CheckTabs(log, sourceFile, repairEnabled, thirdpartyExceptionDir):
        noErrors = False
    if checkCopyright:
        if not __CheckCopyright(log, sourceFile, gitExeName):
            noErrors = False
    if repairEnabled:
        __Repair(log, sourceFile, asciiRepair, disableWrite)
    return noErrors


def __ProcessSourceFile(log: Log, package: Package, fullPath: str, repairEnabled: bool, thirdpartyExceptionDir: Optional[str],
                        disableWrite: bool, gitExeName: str, checkCopyright: bool) -> bool:
    log.LogPrintVerbose(10, "- Scanning '{0}'".format(fullPath))
    noErrors = True
    asciiRepair = False
    sourceFile = SourceFile(package, fullPath)
    if not __CheckASCII(log, sourceFile, repairEnabled):
        # The ASCII repair is not safe, so dont do it
        #asciiRepair = True
        noErrors = False
    if not __CheckTabs(log, sourceFile, repairEnabled, thirdpartyExceptionDir):
        noErrors = False
    if checkCopyright:
        if not __CheckCopyright(log, sourceFile, gitExeName):
            noErrors = False
    if repairEnabled:
        __Repair(log, sourceFile, asciiRepair, disableWrite)
    return noErrors


def __ScanFiles(log: Log, package: Package, filteredFiles: Optional[List[str]],
                repairEnabled: bool, thirdpartyExceptionDir: Optional[str], checkType: CheckType, disableWrite: bool,
                gitExeName: str, checkCopyright: bool) -> int:
    """
    :param filteredFiles: a optional list of specifc files to scan in this package (if supplied the rest should be ignored)
    """
    if not package.AllowCheck and checkType == CheckType.Normal:
        return 0

    if package.AbsolutePath is None:
        log.DoPrintWarning("package did not contain a abs path")
        return 0

    allowedFileSet = None if filteredFiles is None else set(filteredFiles)

    errorCount = 0
    if package.ResolvedBuildAllIncludeFiles is not None:
        for fileName in package.ResolvedBuildAllIncludeFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            # Only process files with the expected extension
            if allowedFileSet is None or fullPath in allowedFileSet:
                if __IsValidExtension(fileName, __g_includeExtensionList):
                    if not __ProcessIncludeFile(log, package, fullPath, repairEnabled, thirdpartyExceptionDir, disableWrite, gitExeName, checkCopyright):
                        errorCount += 1


    if package.ResolvedBuildSourceFiles is not None:
        for fileName in package.ResolvedBuildSourceFiles:
            fullPath = IOUtil.Join(package.AbsolutePath, fileName)
            if allowedFileSet is None or fullPath in allowedFileSet:
                if __IsValidExtension(fileName, __g_includeExtensionList):
                    if not __ProcessIncludeFile(log, package, fullPath, repairEnabled, thirdpartyExceptionDir, disableWrite, gitExeName, checkCopyright):
                        errorCount += 1
                elif __IsValidExtension(fileName, __g_sourceExtensionList):
                    if not __ProcessSourceFile(log, package, fullPath, repairEnabled, thirdpartyExceptionDir, disableWrite, gitExeName, checkCopyright):
                        errorCount += 1
    return errorCount


def Scan(log: Log, scanPackageList: List[Package], customPackageFileFilter: Optional[CustomPackageFileFilter],
         repairEnabled: bool, thirdpartyExceptionDir: Optional[str], checkType: CheckType,
         disableWrite: bool, gitExeName: str, checkCopyright: bool) -> None:
    """
    Run through all source files that are part of the packages and check for common errors
    :param scanPackageList: the packages that will be scanned.
    """

    log.LogPrint("Running source scan")

    extensionList = __g_includeExtensionList + __g_sourceExtensionList

    # GitUtil.GetExecutableName(generatorContext.Generator.PlatformName)

    # Filter the package list so it only contains things we can process
    finalPackageList = [package for package in scanPackageList if PerformClangUtil.CanProcessPackage(package)]

    totalErrorCount = 0
    for package in finalPackageList:
        filteredFiles = None if customPackageFileFilter is None else customPackageFileFilter.TryLocateFilePatternInPackage(log, package, extensionList)
        if customPackageFileFilter is None or filteredFiles is not None:
            totalErrorCount += __ScanFiles(log, package, filteredFiles, repairEnabled, thirdpartyExceptionDir, checkType, disableWrite, gitExeName, checkCopyright)

    if totalErrorCount > 0 and not repairEnabled:
        log.DoPrint("BEWARE: If you have made a backup of your files you can try to auto correct the errors with '--Repair' but do so at your own peril")
