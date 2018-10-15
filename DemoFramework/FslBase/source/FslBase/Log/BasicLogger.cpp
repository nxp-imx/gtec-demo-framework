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

#include <FslBase/Log/BasicLogger.hpp>

#ifdef __ANDROID__
#include <android/log.h>
#define IDE_LOG(lINE) \
  {                   \
  }
#else
#include <iostream>
#include <sstream>
#if defined(_WIN32) && !defined(NDEBUG)
#include <windows.h>
#define IDE_LOG(lINE)                        \
  {                                          \
    std::wstringstream sTREAM;               \
    sTREAM << lINE << "\n";                  \
    OutputDebugString(sTREAM.str().c_str()); \
  }
#else
#define IDE_LOG(lINE) \
  {                   \
  }
#endif
#endif

namespace Fsl
{
  namespace
  {
#ifndef __ANDROID__
    template <typename T>
    void SafePrint(const T& str)
    {
      // Done this way to ensure that string and the std::endl gets written to cout in one go
      std::stringstream tmp;
      tmp << str << std::endl;

      std::cout << tmp.str();
    }
#endif
  }

  LogType BasicLogger::g_logLevel = LogType::Info;


  void BasicLogger::WriteLine(const LogType logType, const char* const psz, const LogLocation& location)
  {
    try
    {
      if (psz == nullptr)
      {
        return;
      }

#ifdef __ANDROID__
      __android_log_print(ANDROID_LOG_DEBUG, "FSL_LOG_TAG", "%s", psz);
#else
      SafePrint(psz);
      IDE_LOG(location.pszFile << "(" << location.line << "): " << psz);
#endif
    }
    catch (const std::exception&)
    {
      /// the logging functionality should never kill the program
    }
  }

  void BasicLogger::WriteLine(const LogType logType, const std::string& str, const LogLocation& location)
  {
    try
    {
#ifdef __ANDROID__
      __android_log_print(ANDROID_LOG_DEBUG, "FSL_LOG_TAG", "%s", str.c_str());
#else
      SafePrint(str);
      IDE_LOG(location.pszFile << "(" << location.line << "): " << str.c_str());
#endif
    }
    catch (const std::exception&)
    {
      /// the logging functionality should never kill the program, so eat the exception silently
    }
  }
}
