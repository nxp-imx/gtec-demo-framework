#!/usr/bin/env python3

#****************************************************************************************************************************************************
# Copyright 2019 NXP
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
#    * Neither the name of the NXP. nor the names of
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
from typing import List
from typing import Optional
from typing import Set
from enum import Enum
import json
import shlex
from FslBuildGen import IOUtil
from FslBuildGen import Util
from FslBuildGen.Log import Log
from FslBuildGen.Exceptions import InvalidPackageNameException

class CompileCommandDefine(object):
    PackageName = "FSLPACKAGENAME__"


class ParseState(Enum):
    Normal = 0
    Skip = 1
    SystemInclude = 2

class LocalVerbosityLevel(object):
    Info = 3
    Debug = 4
    Trace = 5

#  "directory": "E:/Work/DemoFramework/build/Windows/fsl/cmakeTidy/1",
#  "command": "C:\\PROGRA~1\\LLVM\\bin\\CLANG_~1.EXE  -DFSL_ENABLE_FMT -DFSL_ENABLE_OPENCV4 -DFSL_FEATURE_FSLBASE -DFSL_FEATURE_FSLBASE_LOG3 -DFSL_FEATURE_OPENCV -DFSL_PLATFORM_WINDOWS -DNOMINMAX -DUNICODE -DVC_EXTRALEAN -DWIN32_LEAN_AND_MEAN -D_UNICODE -Ie:/Work/DemoFramework/DemoApps/OpenCV/OpenCV101/source -Ie:/Work/DemoFramework/DemoFramework/FslDemoApp/Console/include -Ie:/Work/DemoFramework/DemoFramework/FslDemoApp/Base/include -Ie:/Work/DemoFramework/DemoFramework/FslBase/include -Ie:/Work/DFLibs/Windows/Ninja_3_1/fmt-7.0.3/include -Ie:/Work/DemoFramework/DemoFramework/FslDemoApp/Shared/include -Ie:/Work/DemoFramework/DemoFramework/FslService/Consumer/include -Ie:/Work/DemoFramework/DemoFramework/FslDemoService/Graphics/include -Ie:/Work/DemoFramework/DemoFramework/FslDemoService/Profiler/include -Ie:/Work/DemoFramework/DemoFramework/FslGraphics/include -Ie:/Work/DemoFramework/DemoFramework/FslNativeWindow/Base/include -Ie:/_sdk/opencv-4.2.0/build/include  -O3 -DNDEBUG -D_DLL -D_MT -Xclang --dependent-lib=msvcrt   -W -Wall -Wtype-limits -Wuninitialized -o DemoApps\\OpenCV\\OpenCV101\\CMakeFiles\\OpenCV.OpenCV101.dir\\E_\\Work\\DemoFramework\\DemoApps\\OpenCV\\OpenCV101\\source\\OpenCV101.cpp.obj -c E:\\Work\\DemoFramework\\DemoApps\\OpenCV\\OpenCV101\\source\\OpenCV101.cpp",
#  "file": "E:\\Work\\DemoFramework\\DemoApps\\OpenCV\\OpenCV101\\source\\OpenCV101.cpp"

class CMakeCompileCommandsBasicRecord(object):
    def __init__(self, directory: str, command: str, file: str) -> None:
        super().__init__()
        self.Directory = directory
        self.Command = command
        self.File = file

    def __str__(self) -> str:
        return 'Directory:"{0}" Command:"{1}" File:"{2}"'.format(self.Directory, self.Command, self.File)

    def __repr__(self) -> str:
        return self.__str__()

class CMakeCompileCommandsRecord(object):
    def __init__(self, packageName: str, directory: str, sourceCommand: str, file: str, defines: Set[str], includes: List[str],
                 systemIncludes: List[str], compilerFlags: List[str], otherArguments: List[str]) -> None:
        super().__init__()
        self.PackageName = packageName
        self.Directory = IOUtil.NormalizePath(directory)
        self.SourceCommand = sourceCommand
        self.File = IOUtil.NormalizePath(file)
        self.Defines = defines
        self.Includes = includes
        self.SystemIncludes = systemIncludes
        self.CompilerFlags = compilerFlags
        self.OtherArguments = otherArguments

    def __str__(self) -> str:
        return 'PackageName:"{0}" Directory:"{1}" SourceCommand:"{2}" File:"{3}"'.format(self.PackageName, self.Directory, self.SourceCommand, self.File)

    def __repr__(self) -> str:
        return self.__str__()


class CMakeCompileCommandsJson(object):
    JSON_KEY_DIRECTORY = 'directory'
    JSON_KEY_COMMAND = 'command'
    JSON_KEY_FILE = 'file'

    @staticmethod
    def Load(log: Log, cacheFilename: str) -> List[CMakeCompileCommandsBasicRecord]:
        strJson = IOUtil.ReadFile(cacheFilename)
        jsonList = json.loads(strJson)
        if not isinstance(jsonList, List):
            raise Exception("Unsupported format the file did not contain a list")

        result = [] # type: List[CMakeCompileCommandsBasicRecord]
        for jsonRecord in jsonList:
            if not isinstance(jsonRecord, Dict):
                raise Exception("Unsupported format the entry was not a dictionary as expected")
            if CMakeCompileCommandsJson.JSON_KEY_DIRECTORY not in jsonRecord:
                raise Exception("Entry did not contain the expected '{0}' key".format(CMakeCompileCommandsJson.JSON_KEY_DIRECTORY))
            if CMakeCompileCommandsJson.JSON_KEY_COMMAND not in jsonRecord:
                raise Exception("Entry did not contain the expected '{0}' key".format(CMakeCompileCommandsJson.JSON_KEY_COMMAND))
            if CMakeCompileCommandsJson.JSON_KEY_FILE not in jsonRecord:
                raise Exception("Entry did not contain the expected '{0}' key".format(CMakeCompileCommandsJson.JSON_KEY_FILE))
            directory = jsonRecord[CMakeCompileCommandsJson.JSON_KEY_DIRECTORY]
            command = jsonRecord[CMakeCompileCommandsJson.JSON_KEY_COMMAND]
            trace = jsonRecord[CMakeCompileCommandsJson.JSON_KEY_FILE]

            if not isinstance(directory, str):
                raise Exception("Unsupported format the record entry '{0}' was not a string".format(CMakeCompileCommandsJson.JSON_KEY_DIRECTORY))
            if not isinstance(command, str):
                raise Exception("Unsupported format the record entry '{0}' was not a string".format(CMakeCompileCommandsJson.JSON_KEY_COMMAND))
            if not isinstance(trace, str):
                raise Exception("Unsupported format the record entry '{0}' was not a string".format(CMakeCompileCommandsJson.JSON_KEY_FILE))


            result.append(CMakeCompileCommandsBasicRecord(directory, command, trace))
        return result

    @staticmethod
    def Parse(log: Log, source: List[CMakeCompileCommandsBasicRecord]) -> List[CMakeCompileCommandsRecord]:
        result = [] # type: List[CMakeCompileCommandsRecord]

        for entry in source:
            commands = shlex.split(entry.Command)
            defines = set()
            includes = []
            systemIncludes = []
            compilerFlags = []
            otherArguments = []
            packageName = None # type: Optional[str]
            if len(commands) < 1:
                raise Exception("The command list did not contain the expected amount of elements")
            parseState = ParseState.Skip # We skip the first since its the compiler
            for command in commands:
                if parseState == ParseState.Skip:
                    parseState = ParseState.Normal
                elif parseState == ParseState.SystemInclude:
                    systemIncludes.append(command)
                    parseState = ParseState.Normal
                else:
                    if command.startswith('-D'):
                        # Extract defines and handle the special package name tag define
                        defineName = command[2:]
                        if defineName.startswith(CompileCommandDefine.PackageName):
                            if packageName is not None:
                                raise Exception("Package name was already defined as: {0}".format(packageName))
                            packageName = defineName[len(CompileCommandDefine.PackageName):]
                        else:
                            defines.add(defineName)
                    elif command.startswith('-I'):
                        # Extract include paths
                        includes.append(IOUtil.NormalizePath(command[2:]))
                    elif command == '-isystem':
                        # Extract system include paths
                        parseState = ParseState.SystemInclude
                    elif command.startswith('-o') or command.startswith('-c'):
                        # Skip the input file and the output file
                        parseState = ParseState.Skip
                    elif len(command) >= 2 and command[0] == '-' and command[1] != '-':
                        # Extract simple compiler flags
                        compilerFlags.append(command)
                    else:
                        # Everything else
                        otherArguments.append(command)

            if packageName is None:
                raise Exception("The compile command was not tagged with a package name define starting with: {0}".format(CompileCommandDefine.PackageName))

            result.append(CMakeCompileCommandsRecord(packageName, entry.Directory, entry.Command, entry.File, defines, includes, systemIncludes, compilerFlags, otherArguments))

        return result





