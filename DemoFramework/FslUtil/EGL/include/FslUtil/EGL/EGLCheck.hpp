#ifndef FSLUTIL_EGL_EGLCHECK_HPP
#define FSLUTIL_EGL_EGLCHECK_HPP
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

#include <FslUtil/EGL/CheckError.hpp>
#include <FslUtil/EGL/DebugStrings.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <EGL/egl.h>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_CHECK_FOR_ERROR()                                \
  {                                                          \
    Fsl::EGL::CheckError(eglGetError(), __FILE__, __LINE__); \
  }


// TODO: implement a better version of this macro as it can give unexpected behavior
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_CHECK(X)                                             \
  X;                                                             \
  {                                                              \
    Fsl::EGL::CheckError(eglGetError(), #X, __FILE__, __LINE__); \
  }


// TODO: implement a better version of this macro as it can give unexpected behavior
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EGL_LOG_ERROR(X)                                                                                                                      \
  X;                                                                                                                                          \
  {                                                                                                                                           \
    const auto rESULT = eglGetError();                                                                                                        \
    if (rESULT != EGL_SUCCESS)                                                                                                                \
    {                                                                                                                                         \
      FSLLOG3_ERROR("{} failed with error code {} ({}) at {} ({})", #X, Fsl::EGL::Debug::ErrorCodeToString(rESULT), static_cast<int>(rESULT), \
                    __FILE__, __LINE__);                                                                                                      \
    }                                                                                                                                         \
  }

#endif
