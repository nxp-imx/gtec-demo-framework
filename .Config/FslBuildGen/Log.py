#!/usr/bin/env python3
#****************************************************************************************************************************************************
# Copyright 2017 NXP
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

from typing import List
import sys

class Log(object):
    def __init__(self, title: str, verbosityLevel: int, showAppTitleIfVerbose: bool = False) -> None:
        super().__init__()
        self.Title = title
        self.Verbosity = verbosityLevel
        self.IsVerbose = verbosityLevel > 0
        self.__CurrentIndent = 0
        self.__CurrentSpaces = ""
        self.__CurrentSpaceCache = []  # type: List[str]
        self.__TitlePrinted = False

        for i in range(0, 8):
            self.__CurrentSpaceCache.append("  " * i)

        if self.IsVerbose and showAppTitleIfVerbose:
            self.PrintTitle()


    def GetTitlePrinted(self) -> bool:
        return self.__TitlePrinted


    def SetTitlePrinted(self, value: bool) -> None:
        self.__TitlePrinted = value


    def PrintTitle(self) -> None:
        """ Prints the title if it has not been printed already """
        if self.__TitlePrinted:
            return
        self.__TitlePrinted = True
        self.DoPrint(self.Title)


    def PushIndent(self) -> None:
        self.__CurrentIndent += 1
        if self.__CurrentIndent >= len(self.__CurrentSpaceCache):
            self.__CurrentSpaceCache.append("  " * self.__CurrentIndent)
        self.__CurrentSpaces = self.__CurrentSpaceCache[self.__CurrentIndent]


    def PopIndent(self) -> None:
        if self.__CurrentIndent <= 0:
            if self.IsVerbose:
                self._PrintNow("ERROR: PopIndent called while indent level is zero, call ignored")
            return

        self.__CurrentIndent -= 1
        self.__CurrentSpaces = self.__CurrentSpaceCache[self.__CurrentIndent]


    def LogPrint(self, message: str) -> None:
        if self.IsVerbose:
            self.DoPrint(message)


    def LogPrintWarning(self, message: str) -> None:
        if self.IsVerbose:
            self.DoPrintWarning(message)


    def LogPrintVerbose(self, verbosityLevel: int, message: str) -> None:
        if self.Verbosity >= verbosityLevel:
            self.DoPrint(message)


    def DoPrint(self, message: str) -> None:
        if self.__CurrentIndent <= 0:
            self._PrintNow(message)
        else:
            self._PrintNow("{0}{1}".format(self.__CurrentSpaces, message))


    def DoPrintError(self, message: str) -> None:
        self._PrintNow("ERROR: {0}".format(message))


    def DoPrintWarning(self, message: str) -> None:
        self._PrintNow("WARNING: {0}".format(message))

    def _PrintNow(self, message: str) -> None:
        print(message)
        sys.stdout.flush()


