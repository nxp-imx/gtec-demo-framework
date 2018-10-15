#ifndef FSLUTIL_OPENGLES2_CHECKERROR_HPP
#define FSLUTIL_OPENGLES2_CHECKERROR_HPP
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

#include <FslUtil/OpenGLES2/Exceptions.hpp>
#include <GLES2/gl2.h>
#include <cassert>

namespace Fsl
{
  namespace GLES2
  {
    inline void CheckError(const GLenum errorCode)
    {
      if (errorCode != GL_NO_ERROR)
      {
        throw GLESGraphicsException("", errorCode);
      }
    }

    inline void CheckError(const GLenum errorCode, const char* const pszMessage)
    {
      if (errorCode != GL_NO_ERROR)
      {
        assert(pszMessage != nullptr);
        throw GLESGraphicsException(pszMessage, errorCode);
      }
    }


    inline void CheckError(const GLenum errorCode, const std::string& message)
    {
      if (errorCode != GL_NO_ERROR)
      {
        throw GLESGraphicsException(message, errorCode);
      }
    }


    inline void CheckError(const GLenum errorCode, const char* const pszFilename, const int lineNumber)
    {
      if (errorCode != GL_NO_ERROR)
      {
        assert(pszFilename != nullptr);
        throw GLESGraphicsException("", errorCode, pszFilename, lineNumber);
      }
    }


    inline void CheckError(const GLenum errorCode, const std::string& filename, const int lineNumber)
    {
      if (errorCode != GL_NO_ERROR)
      {
        throw GLESGraphicsException("", errorCode, filename, lineNumber);
      }
    }


    inline void CheckError(const GLenum errorCode, const char* const pszMessage, const char* const pszFilename, const int lineNumber)
    {
      if (errorCode != GL_NO_ERROR)
      {
        assert(pszMessage != nullptr);
        assert(pszFilename != nullptr);
        throw GLESGraphicsException(pszMessage, errorCode, pszFilename, lineNumber);
      }
    }

    inline void CheckError(const GLenum errorCode, const char* const pszMessage, const std::string& filename, const int lineNumber)
    {
      if (errorCode != GL_NO_ERROR)
      {
        assert(pszMessage != nullptr);
        throw GLESGraphicsException(pszMessage, errorCode, filename, lineNumber);
      }
    }


    inline void CheckError(const GLenum errorCode, const std::string& message, const char* const pszFilename, const int lineNumber)
    {
      if (errorCode != GL_NO_ERROR)
      {
        assert(pszFilename != nullptr);
        throw GLESGraphicsException(message, errorCode, pszFilename, lineNumber);
      }
    }

    inline void CheckError(const GLenum errorCode, const std::string& message, const std::string& filename, const int lineNumber)
    {
      if (errorCode != GL_NO_ERROR)
      {
        throw GLESGraphicsException(message, errorCode, filename, lineNumber);
      }
    }
  }
}

#endif
