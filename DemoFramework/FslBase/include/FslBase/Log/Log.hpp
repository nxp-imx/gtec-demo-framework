#ifndef FSLBASE_LOG_LOG_HPP
#define FSLBASE_LOG_LOG_HPP
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

#include <FslBase/Log/Logger.hpp>

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLLOG(lINE)                                                                                          \
  {                                                                                                           \
    if (Fsl::Logger::GetLogLevel() >= Fsl::LogType::Info)                                                     \
    {                                                                                                         \
      std::stringstream sTREAM;                                                                               \
      sTREAM << lINE;                                                                                         \
      Fsl::Logger::WriteLine(Fsl::LogType::Info, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                         \
  }

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLLOG_IF(cOND, lINE)                                                                                 \
  {                                                                                                           \
    if ((cOND) && Fsl::Logger::GetLogLevel() >= Fsl::LogType::Info)                                           \
    {                                                                                                         \
      std::stringstream sTREAM;                                                                               \
      sTREAM << lINE;                                                                                         \
      Fsl::Logger::WriteLine(Fsl::LogType::Info, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                         \
  }

#define FSLLOG_WARNING(lINE)                                                                                     \
  {                                                                                                              \
    if (Fsl::Logger::GetLogLevel() >= Fsl::LogType::Warning)                                                     \
    {                                                                                                            \
      std::stringstream sTREAM;                                                                                  \
      sTREAM << "WARNING: " << lINE;                                                                             \
      Fsl::Logger::WriteLine(Fsl::LogType::Warning, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                            \
  }

#ifdef NDEBUG
//! Log in debug builds only
#define FSLLOG_DEBUG(lINE) \
  {                        \
  }
#define FSLLOG_DEBUG_IF(cONDITION, lINE) \
  {                                      \
  }
#define FSLLOG_DEBUG_WARNING(lINE) \
  {                                \
  }
#define FSLLOG_DEBUG_WARNING_IF(cONDITION, lINE) \
  {                                              \
  }
#else
//! Log in debug builds only
#define FSLLOG_DEBUG(lINE)                                                                                  \
  {                                                                                                         \
    std::stringstream sTREAM;                                                                               \
    sTREAM << lINE;                                                                                         \
    Fsl::Logger::WriteLine(Fsl::LogType::Info, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
  }
//! Log in debug builds only
#define FSLLOG_DEBUG_IF(cONDITION, lINE)                                                                      \
  {                                                                                                           \
    if (cONDITION)                                                                                            \
    {                                                                                                         \
      std::stringstream sTREAM;                                                                               \
      sTREAM << lINE;                                                                                         \
      Fsl::Logger::WriteLine(Fsl::LogType::Info, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                         \
  }

//! Log in debug builds only
#define FSLLOG_DEBUG_WARNING(lINE)                                                                             \
  {                                                                                                            \
    std::stringstream sTREAM;                                                                                  \
    sTREAM << "WARNING: " << lINE;                                                                             \
    Fsl::Logger::WriteLine(Fsl::LogType::Warning, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
  }

//! Log in debug builds only
#define FSLLOG_DEBUG_WARNING_IF(cONDITION, lINE)                                                                 \
  {                                                                                                              \
    if (cONDITION)                                                                                               \
    {                                                                                                            \
      std::stringstream sTREAM;                                                                                  \
      sTREAM << "WARNING: " << lINE;                                                                             \
      Fsl::Logger::WriteLine(Fsl::LogType::Warning, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                            \
  }
#endif


// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLLOG_WARNING_IF(cOND, lINE)                                                                            \
  {                                                                                                              \
    if ((cOND) && Fsl::Logger::GetLogLevel() >= Fsl::LogType::Warning)                                           \
    {                                                                                                            \
      std::stringstream sTREAM;                                                                                  \
      sTREAM << "WARNING: " << lINE;                                                                             \
      Fsl::Logger::WriteLine(Fsl::LogType::Warning, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
    }                                                                                                            \
  }

#define FSLLOG_ERROR(lINE)                                                                                       \
  {                                                                                                              \
    if (Fsl::Logger::GetLogLevel() >= Fsl::LogType::Error)                                                       \
    {                                                                                                            \
      try                                                                                                        \
      {                                                                                                          \
        std::stringstream sTREAM;                                                                                \
        sTREAM << "ERROR: " << lINE;                                                                             \
        Fsl::Logger::WriteLine(Fsl::LogType::Error, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
      }                                                                                                          \
      catch (const std::exception&)                                                                              \
      {                                                                                                          \
      }                                                                                                          \
    }                                                                                                            \
  }

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLLOG_ERROR_IF(cOND, lINE)                                                                              \
  {                                                                                                              \
    if ((cOND) && Fsl::Logger::GetLogLevel() >= Fsl::LogType::Error)                                             \
    {                                                                                                            \
      try                                                                                                        \
      {                                                                                                          \
        std::stringstream sTREAM;                                                                                \
        sTREAM << "ERROR: " << lINE;                                                                             \
        Fsl::Logger::WriteLine(Fsl::LogType::Error, sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
      }                                                                                                          \
      catch (const std::exception&)                                                                              \
      {                                                                                                          \
      }                                                                                                          \
    }                                                                                                            \
  }


// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLLOG2(tYPE, lINE)                                                                         \
  {                                                                                                 \
    if (Fsl::Logger::GetLogLevel() >= (tYPE))                                                       \
    {                                                                                               \
      try                                                                                           \
      {                                                                                             \
        std::stringstream sTREAM;                                                                   \
        sTREAM << lINE;                                                                             \
        Fsl::Logger::WriteLine((tYPE), sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
      }                                                                                             \
      catch (const std::exception&)                                                                 \
      {                                                                                             \
      }                                                                                             \
    }                                                                                               \
  }

// WARNING: It is not a good idea to utilize this code before 'main' has been hit (so don't use it from static object constructors)
#define FSLLOG2_IF(cOND, tYPE, lINE)                                                                \
  {                                                                                                 \
    if ((cOND) && Fsl::Logger::GetLogLevel() >= (tYPE))                                             \
    {                                                                                               \
      try                                                                                           \
      {                                                                                             \
        std::stringstream sTREAM;                                                                   \
        sTREAM << lINE;                                                                             \
        Fsl::Logger::WriteLine((tYPE), sTREAM, Fsl::LogLocation(__FILE__, __FUNCTION__, __LINE__)); \
      }                                                                                             \
      catch (const std::exception&)                                                                 \
      {                                                                                             \
      }                                                                                             \
    }                                                                                               \
  }

#endif
