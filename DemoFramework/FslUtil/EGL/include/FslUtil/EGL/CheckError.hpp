#ifndef FSLUTIL_EGL_CHECKERROR_HPP
#define FSLUTIL_EGL_CHECKERROR_HPP
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

#include <FslUtil/EGL/Exceptions.hpp>
#include <EGL/egl.h>
#include <cassert>

namespace Fsl
{
  namespace EGL
  {
    inline void CheckError(const EGLenum errorCode)
    {
      if (errorCode != EGL_SUCCESS)
      {
        throw EGLGraphicsException("", errorCode);
      }
    }

    inline void CheckError(const EGLenum errorCode, const char* const pszMessage)
    {
      if (errorCode != EGL_SUCCESS)
      {
        assert(pszMessage != nullptr);
        throw EGLGraphicsException(pszMessage, errorCode);
      }
    }


    inline void CheckError(const EGLenum errorCode, const std::string& message)
    {
      if (errorCode != EGL_SUCCESS)
      {
        throw EGLGraphicsException(message, errorCode);
      }
    }


    inline void CheckError(const EGLenum errorCode, const char* const pszFilename, const int lineNumber)
    {
      if (errorCode != EGL_SUCCESS)
      {
        assert(pszFilename != nullptr);
        throw EGLGraphicsException("", errorCode, pszFilename, lineNumber);
      }
    }


    inline void CheckError(const EGLenum errorCode, const std::string& filename, const int lineNumber)
    {
      if (errorCode != EGL_SUCCESS)
      {
        throw EGLGraphicsException("", errorCode, filename, lineNumber);
      }
    }


    inline void CheckError(const EGLenum errorCode, const char* const pszMessage, const char* const pszFilename, const int lineNumber)
    {
      if (errorCode != EGL_SUCCESS)
      {
        assert(pszMessage != nullptr);
        assert(pszFilename != nullptr);
        throw EGLGraphicsException(pszMessage, errorCode, pszFilename, lineNumber);
      }
    }

    inline void CheckError(const EGLenum errorCode, const char* const pszMessage, const std::string& filename, const int lineNumber)
    {
      if (errorCode != EGL_SUCCESS)
      {
        assert(pszMessage != nullptr);
        throw EGLGraphicsException(pszMessage, errorCode, filename, lineNumber);
      }
    }


    inline void CheckError(const EGLenum errorCode, const std::string& message, const char* const pszFilename, const int lineNumber)
    {
      if (errorCode != EGL_SUCCESS)
      {
        assert(pszFilename != nullptr);
        throw EGLGraphicsException(message, errorCode, pszFilename, lineNumber);
      }
    }

    inline void CheckError(const EGLenum errorCode, const std::string& message, const std::string& filename, const int lineNumber)
    {
      if (errorCode != EGL_SUCCESS)
      {
        throw EGLGraphicsException(message, errorCode, filename, lineNumber);
      }
    }
  }
}

#endif
