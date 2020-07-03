#ifndef FSLBASE_LOG_LOG3CORE_HPP
#define FSLBASE_LOG_LOG3CORE_HPP
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

// The basic log macro pulls in a minimum amount of dependencies, so its better to use it if you need to log from a header file
// Beware that it can only log a zero terminated string by default. If you need to log more advanced things then include
// - Logger1.hpp to enable logging of std::string
// - Log.hpp to enable fmt powered logging
#include <FslBase/Log/Logger0.hpp>

#ifdef NDEBUG

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3(tYPE, ...)                         \
  {                                                \
    if (Fsl::LogConfig::GetLogLevel() >= (tYPE))   \
    {                                              \
      Fsl::Logger::WriteLine((tYPE), __VA_ARGS__); \
    }                                              \
  }

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_IF(cONDITION, tYPE, ...)                        \
  {                                                             \
    if ((cONDITION) && Fsl::LogConfig::GetLogLevel() >= (tYPE)) \
    {                                                           \
      Fsl::Logger::WriteLine((tYPE), __VA_ARGS__);              \
    }                                                           \
  }

//! Log in debug builds only
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE6(...) \
  {                                 \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE5(...) \
  {                                 \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE4(...) \
  {                                 \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE3(...) \
  {                                 \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE2(...) \
  {                                 \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE(...) \
  {                                \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_INFO(...) \
  {                             \
  }
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_WARNING(...) \
  {                                \
  }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_INFO_IF(cONDITION, ...) \
  {                                           \
  }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_WARNING_IF(cONDITION, ...) \
  {                                              \
  }

#else

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3(tYPE, ...)                                                                             \
  {                                                                                                    \
    if (Fsl::LogConfig::GetLogLevel() >= (tYPE))                                                       \
    {                                                                                                  \
      Fsl::Logger::WriteLine(Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__), (tYPE), __VA_ARGS__); \
    }                                                                                                  \
  }

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_IF(cONDITION, tYPE, ...)                                                               \
  {                                                                                                    \
    if ((cONDITION) && Fsl::LogConfig::GetLogLevel() >= (tYPE))                                        \
    {                                                                                                  \
      Fsl::Logger::WriteLine(Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__), (tYPE), __VA_ARGS__); \
    }                                                                                                  \
  }

//! Log in debug builds only

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE6(...) FSLLOG3(Fsl::LogType::Verbose6, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE5(...) FSLLOG3(Fsl::LogType::Verbose5, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE4(...) FSLLOG3(Fsl::LogType::Verbose4, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE3(...) FSLLOG3(Fsl::LogType::Verbose3, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE2(...) FSLLOG3(Fsl::LogType::Verbose2, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_VERBOSE(...) FSLLOG3(Fsl::LogType::Verbose, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_INFO(...) FSLLOG3(Fsl::LogType::Info, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_WARNING(...) FSLLOG3(Fsl::LogType::Warning, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_INFO_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Info, __VA_ARGS__)

//! Log in debug builds only
//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_DEBUG_WARNING_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Warning, __VA_ARGS__)

#endif


//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE(...) FSLLOG3(Fsl::LogType::Verbose, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE2(...) FSLLOG3(Fsl::LogType::Verbose2, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE3(...) FSLLOG3(Fsl::LogType::Verbose3, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE4(...) FSLLOG3(Fsl::LogType::Verbose4, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE5(...) FSLLOG3(Fsl::LogType::Verbose5, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE6(...) FSLLOG3(Fsl::LogType::Verbose6, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_INFO(...) FSLLOG3(Fsl::LogType::Info, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_WARNING(...) FSLLOG3(Fsl::LogType::Warning, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_ERROR(...) FSLLOG3(Fsl::LogType::Error, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE6_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Verbose6, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE5_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Verbose5, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE4_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Verbose4, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE3_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Verbose3, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE2_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Verbose2, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_VERBOSE_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Verbose, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_INFO_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Info, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_WARNING_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Warning, __VA_ARGS__)

//! WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define FSLLOG3_ERROR_IF(cONDITION, ...) FSLLOG3_IF((cONDITION), Fsl::LogType::Error, __VA_ARGS__)

#endif
