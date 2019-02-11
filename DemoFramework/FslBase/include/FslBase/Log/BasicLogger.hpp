#ifndef FSLBASE_LOG_BASICLOGGER_HPP
#define FSLBASE_LOG_BASICLOGGER_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    * Neither the name of the NXP. nor the names of
 *      its contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************************************************************************************/

#include <FslBase/Log/LogLocation.hpp>
#include <string>

namespace Fsl
{
  enum class LogType
  {
    Error = 0,
    Warning = 1,
    Info = 2,
    // Things below require 'verbose'
    Verbose = 3,
    Verbose2 = 4,
    Verbose3 = 5,
    Verbose4 = 6,
  };


  // WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
  class BasicLogger
  {
    static LogType g_logLevel;

  public:
    BasicLogger(const BasicLogger&) = delete;
    BasicLogger& operator=(const BasicLogger&) = delete;


    static LogType GetLogLevel()
    {
      return g_logLevel;
    }

    static void SetLogLevel(const LogType level)
    {
      g_logLevel = level;
    }

    static void WriteLine(const LogType logType, const char* const psz, const LogLocation& location);
    static void WriteLine(const LogType logType, const std::string& str, const LogLocation& location);
  };
}

#endif
