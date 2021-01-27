#ifndef FSLBASE_LOG_LOGGER0_HPP
#define FSLBASE_LOG_LOGGER0_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
    Verbose5 = 7,
    Verbose6 = 8,
  };

  //! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
  class LogConfig
  {
    static LogType g_logLevel;

  public:
    LogConfig(const LogConfig&) = delete;
    LogConfig& operator=(const LogConfig&) = delete;
    LogConfig() = delete;
    ~LogConfig() = delete;

    inline static LogType GetLogLevel() noexcept
    {
      return g_logLevel;
    }

    static void SetLogLevel(const LogType level) noexcept
    {
      g_logLevel = level;
    }
  };


  namespace Logger
  {
    //! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
    extern void WriteLine(const LogType logType, const char* const psz) noexcept;

    //! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
    extern void WriteLine(const LogLocation& location, const LogType logType, const char* const psz) noexcept;
  }
}

#endif
