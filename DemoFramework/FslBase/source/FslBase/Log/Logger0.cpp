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

// The android logging is just hacked in here to get something to work.
// This is something we will need to expand upon later.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Log/Logger0.hpp>
#include <exception>

#ifdef __ANDROID__
#include <android/log.h>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define IDE_LOG(PSZfORMAT, ...) \
  {                             \
  }
#elif defined(FSL_PLATFORM_FREERTOS)
#include <Platform/FreeRTOS/Stuff/PlatformStuff.hpp>
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define IDE_LOG(PSZfORMAT, ...) \
  {                             \
  }
#else
#include <fmt/format.h>
#if defined(_WIN32) && !defined(NDEBUG)
#include <windows.h>

namespace
{
  template <typename... Args>
  inline void IdeLog(const char* const pszFormat, const Args&... args)
  {
    fmt::memory_buffer buf;
    fmt::format_to(std::back_inserter(buf), pszFormat, args...);
    buf.push_back(0);
    OutputDebugStringA(buf.data());
  }
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define IDE_LOG(PSZfORMAT, ...) IdeLog((PSZfORMAT), __VA_ARGS__)
#else
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define IDE_LOG(PSZfORMAT, ...) \
  {                             \
  }
#endif
#endif

namespace Fsl
{
  namespace
  {
#ifndef __ANDROID__
#if defined(FSL_PLATFORM_FREERTOS)
    inline void SafePrint(const LogType logType, const char* const psz)
    {
      PlatformPrintF(psz);
    }
#else
    void SafePrint(const LogType logType, const char* const psz)
    {
      switch (logType)
      {
      case LogType::Warning:
        fmt::print("WARNING: {}\n", psz);
        break;
      case LogType::Error:
        fmt::print("ERROR: {}\n", psz);
        break;
      default:
        fmt::print("{}\n", psz);
        break;
      }
    }
#endif
#endif
  }

  LogType LogConfig::g_logLevel = LogType::Info;

  namespace Logger
  {
    void WriteLine(const LogType logType, const char* const psz) noexcept
    {
      try
      {
        if (psz == nullptr)
        {
          return;
        }

#ifdef __ANDROID__
        auto androidLogType = ANDROID_LOG_DEBUG;
        switch (logType)
        {
        case LogType::Warning:
          androidLogType = ANDROID_LOG_WARN;
          break;
        case LogType::Error:
          androidLogType = ANDROID_LOG_ERROR;
          break;
        default:
          break;
        }
        __android_log_print(androidLogType, "FSL_LOG_TAG", "%s", psz);
#else
        SafePrint(logType, psz);
        switch (logType)
        {
        case LogType::Warning:
          IDE_LOG("WARNING: {}\n", psz);
          break;
        case LogType::Error:
          IDE_LOG("ERROR: {}\n", psz);
          break;
        default:
          IDE_LOG("{}\n", psz);
          break;
        }
#endif
      }
      catch (const std::exception&)
      {
        /// the logging functionality should never kill the program
      }
    }

    void WriteLine(const LogLocation& location, const LogType logType, const char* const psz) noexcept
    {
      FSL_PARAM_NOT_USED(location);
      try
      {
        if (psz == nullptr)
        {
          return;
        }

#ifdef __ANDROID__
        auto androidLogType = ANDROID_LOG_DEBUG;
        switch (logType)
        {
        case LogType::Warning:
          androidLogType = ANDROID_LOG_WARN;
          break;
        case LogType::Error:
          androidLogType = ANDROID_LOG_ERROR;
          break;
        default:
          break;
        }
        __android_log_print(androidLogType, "FSL_LOG_TAG", "%s", psz);
#else
        SafePrint(logType, psz);
        switch (logType)
        {
        case LogType::Warning:
          IDE_LOG("{}({}): WARNING: {}\n", location.pszFile, location.line, psz);
          break;
        case LogType::Error:
          IDE_LOG("{}({}): ERROR: {}\n", location.pszFile, location.line, psz);
          break;
        default:
          IDE_LOG("{}({}): {}\n", location.pszFile, location.line, psz);
          break;
        }
#endif
      }
      catch (const std::exception&)
      {
        /// the logging functionality should never kill the program
      }
    }
  }
}
