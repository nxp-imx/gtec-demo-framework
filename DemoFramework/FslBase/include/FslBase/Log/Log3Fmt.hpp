#ifndef FSLBASE_LOG_LOG3FMT_HPP
#define FSLBASE_LOG_LOG3FMT_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Log/Logger1.hpp>
#include <fmt/format.h>
#include <exception>

namespace Fsl
{
  namespace Logger
  {
    extern void WriteLine(const LogType logType, const char* const pszFormat, const char* const pszArg0) noexcept;
    extern void WriteLine(const LogType logType, const char* const pszFormat, const std::string& strArg0) noexcept;

    template <typename... Args>
    void WriteLine(const LogType logType, const char* const pszFormat, const Args&... args) noexcept
    {
      try
      {
        fmt::memory_buffer buf;
        fmt::format_to(buf, pszFormat, args...);
        buf.push_back(0);
        WriteLine(logType, buf.data());
      }
      catch (const std::exception&)
      {
      }
    }

    template <typename... Args>
    void WriteLine(const LogType logType, const std::string& strFormat, const Args&... args) noexcept
    {
      try
      {
        fmt::memory_buffer buf;
        fmt::format_to(buf, strFormat, args...);
        buf.push_back(0);
        WriteLine(logType, buf.data());
      }
      catch (const std::exception&)
      {
      }
    }

    extern void WriteLine(const LogLocation& logLocation, const LogType logType, const char* const pszFormat, const char* const pszArg0) noexcept;
    extern void WriteLine(const LogLocation& logLocation, const LogType logType, const char* const pszFormat, const std::string& strArg0) noexcept;

    template <typename... Args>
    void WriteLine(const LogLocation& logLocation, const LogType logType, const char* const pszFormat, const Args&... args) noexcept
    {
      try
      {
        fmt::memory_buffer buf;
        fmt::format_to(buf, pszFormat, args...);
        buf.push_back(0);
        WriteLine(logLocation, logType, buf.data());
      }
      catch (const std::exception&)
      {
      }
    }

    template <typename... Args>
    void WriteLine(const LogLocation& logLocation, const LogType logType, const std::string& strFormat, const Args&... args) noexcept
    {
      try
      {
        fmt::memory_buffer buf;
        fmt::format_to(buf, strFormat, args...);
        buf.push_back(0);
        WriteLine(logLocation, logType, buf.data());
      }
      catch (const std::exception&)
      {
      }
    }
  }
}

#endif
