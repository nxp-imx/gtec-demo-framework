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

from FslBuildGen.Log import Log

class BasicConfig(Log):
    def __init__(self, log: Log) -> None:
        super(BasicConfig, self).__init__(log.Title, log.Verbosity, not log.GetTitlePrinted())
        self.__Log = log

    # We override all the log functions to make sure that all output goes to the same log
    # The only reason it's done this way is because the config objects was introduced before the log class and
    # there is a lot of code that needs to be updated to make it call the log instead of the config class,
    # futhermore we need to pass it around to all the places that need it -> so that a lot of work

    def GetTitlePrinted(self) -> bool:
        return self.__Log.GetTitlePrinted()


    def SetTitlePrinted(self, value: bool) -> None:
        self.__Log.SetTitlePrinted(value)


    def PrintTitle(self) -> None:
        self.__Log.PrintTitle()


    def PushIndent(self) -> None:
        self.__Log.PushIndent()


    def PopIndent(self) -> None:
        self.__Log.PopIndent()


    def LogPrint(self, message: str) -> None:
        self.__Log.LogPrint(message)


    def LogPrintWarning(self, message: str) -> None:
        self.__Log.LogPrintWarning(message)


    def LogPrintVerbose(self, verbosityLevel: int, message: str) -> None:
        self.__Log.LogPrintVerbose(verbosityLevel, message)


    def DoPrint(self, message: str) -> None:
        self.__Log.DoPrint(message)


    def DoPrintError(self, message: str) -> None:
        self.__Log.DoPrintError(message)


    def DoPrintWarning(self, message: str) -> None:
        self.__Log.DoPrintWarning(message)
