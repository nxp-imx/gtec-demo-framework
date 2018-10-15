#ifndef FSLBASE_LOG_BASICLOG_HPP
#define FSLBASE_LOG_BASICLOG_HPP
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
// Beware that it can only log a string (zero terminated or std::string), while the FSLLOG utilizes streams.

#include <FslBase/Log/BasicLogger.hpp>

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLBASICLOG(sTRING)                                                                                          \
  {                                                                                                                  \
    if (Fsl::BasicLogger::GetLogLevel() >= Fsl::LogType::Info)                                                       \
    {                                                                                                                \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Info, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                \
  }

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLBASICLOG_IF(cOND, sTRING)                                                                                 \
  {                                                                                                                  \
    if ((cOND) && Fsl::BasicLogger::GetLogLevel() >= Fsl::LogType::Info)                                             \
    {                                                                                                                \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Info, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                \
  }

#define FSLBASICLOG_WARNING(sTRING)                                                                                     \
  {                                                                                                                     \
    if (Fsl::BasicLogger::GetLogLevel() >= Fsl::LogType::Warning)                                                       \
    {                                                                                                                   \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Warning, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                   \
  }

#ifdef NDEBUG
//! Log in debug builds only
#define FSLBASICLOG_DEBUG(sTRING) \
  {                               \
  }
#define FSLBASICLOG_DEBUG_IF(cONDITION, sTRING) \
  {                                             \
  }
#define FSLBASICLOG_DEBUG_WARNING(sTRING) \
  {                                       \
  }
#define FSLBASICLOG_DEBUG_WARNING_IF(cONDITION, sTRING) \
  {                                                     \
  }
#else
//! Log in debug builds only
#define FSLBASICLOG_DEBUG(sTRING)                                                                                  \
  {                                                                                                                \
    Fsl::BasicLogger::WriteLine(Fsl::LogType::Info, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
  }
//! Log in debug builds only
#define FSLBASICLOG_DEBUG_IF(cONDITION, sTRING)                                                                      \
  {                                                                                                                  \
    if (cONDITION)                                                                                                   \
    {                                                                                                                \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Info, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                \
  }

//! Log in debug builds only
#define FSLBASICLOG_DEBUG_WARNING(sTRING)                                                                             \
  {                                                                                                                   \
    Fsl::BasicLogger::WriteLine(Fsl::LogType::Warning, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
  }

//! Log in debug builds only
#define FSLBASICLOG_DEBUG_WARNING_IF(cONDITION, sTRING)                                                                 \
  {                                                                                                                     \
    if (cONDITION)                                                                                                      \
    {                                                                                                                   \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Warning, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                   \
  }
#endif


// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLBASICLOG_WARNING_IF(cOND, sTRING)                                                                            \
  {                                                                                                                     \
    if ((cOND) && Fsl::BasicLogger::GetLogLevel() >= Fsl::LogType::Warning)                                             \
    {                                                                                                                   \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Warning, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                   \
  }

#define FSLBASICLOG_ERROR(sTRING)                                                                                     \
  {                                                                                                                   \
    if (Fsl::BasicLogger::GetLogLevel() >= Fsl::LogType::Error)                                                       \
    {                                                                                                                 \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Error, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                 \
  }

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLBASICLOG_ERROR_IF(cOND, sTRING)                                                                            \
  {                                                                                                                   \
    if ((cOND) && Fsl::BasicLogger::GetLogLevel() >= Fsl::LogType::Error)                                             \
    {                                                                                                                 \
      Fsl::BasicLogger::WriteLine(Fsl::LogType::Error, (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                                 \
  }


// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLBASICLOG2(tYPE, sTRING)                                                                       \
  {                                                                                                      \
    if (Fsl::BasicLogger::GetLogLevel() >= (tYPE))                                                       \
    {                                                                                                    \
      Fsl::BasicLogger::WriteLine((tYPE), (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                    \
  }

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLBASICLOG2_IF(cOND, tYPE, sTRING)                                                              \
  {                                                                                                      \
    if ((cOND) && Fsl::BasicLogger::GetLogLevel() >= (tYPE))                                             \
    {                                                                                                    \
      Fsl::BasicLogger::WriteLine((tYPE), (sTRING), Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                    \
  }

#endif
