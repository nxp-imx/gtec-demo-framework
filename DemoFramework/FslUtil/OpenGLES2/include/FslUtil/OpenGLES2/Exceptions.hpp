#ifndef FSLUTIL_OPENGLES2_EXCEPTIONS_HPP
#define FSLUTIL_OPENGLES2_EXCEPTIONS_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslGraphics/Exceptions.hpp>
#include <FslUtil/OpenGLES2/Common.hpp>
#include <utility>

namespace Fsl::GLES2
{
  class GLESGraphicsException : public GraphicsException
  {
    int32_t m_error{};
    std::string m_filename;
    int32_t m_lineNumber;

  public:
    explicit GLESGraphicsException(const char* const psz)
      : GraphicsException(psz)
      , m_lineNumber(0)
    {
    }

    explicit GLESGraphicsException(const std::string& str)
      : GraphicsException(str)
      , m_lineNumber(0)
    {
    }

    GLESGraphicsException(const char* const psz, int32_t error)
      : GraphicsException(psz)
      , m_error(error)
      , m_lineNumber(0)
    {
    }

    GLESGraphicsException(const std::string& str, int32_t error)
      : GraphicsException(str)
      , m_error(error)
      , m_lineNumber(0)
    {
    }

    GLESGraphicsException(const char* const psz, int32_t error, const char* const pszFilename, const int32_t line)
      : GraphicsException(psz)
      , m_error(error)
      , m_filename(pszFilename)
      , m_lineNumber(line)
    {
    }

    GLESGraphicsException(const std::string& str, int32_t error, const char* const pszFilename, const int32_t line)
      : GraphicsException(str)
      , m_error(error)
      , m_filename(pszFilename)
      , m_lineNumber(line)
    {
    }

    GLESGraphicsException(const char* const psz, int32_t error, std::string filename, const int32_t line)
      : GraphicsException(psz)
      , m_error(error)
      , m_filename(std::move(filename))
      , m_lineNumber(line)
    {
    }

    GLESGraphicsException(const std::string& str, int32_t error, std::string filename, const int32_t line)
      : GraphicsException(str)
      , m_error(error)
      , m_filename(std::move(filename))
      , m_lineNumber(line)
    {
    }

    ~GLESGraphicsException() noexcept override = default;

    int32_t GetError() const noexcept
    {
      return m_error;
    }


    const std::string& GetFilename() const
    {
      return m_filename;
    }

    int32_t GetLineNumber() const noexcept
    {
      return m_lineNumber;
    }
  };
}

#endif
