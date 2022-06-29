#ifndef FSLUTIL_OPENVG_EXCEPTIONS_HPP
#define FSLUTIL_OPENVG_EXCEPTIONS_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Exceptions.hpp>
#include <VG/openvg.h>
#include <string>
#include <utility>

namespace Fsl::OpenVG
{
  class OpenVGException : public std::runtime_error
  {
    std::string m_fileName;
    int m_lineNumber;

  public:
    explicit OpenVGException(const char* const psz)
      : std::runtime_error(psz)
      , m_lineNumber(0)
    {
    }

    explicit OpenVGException(const std::string& whatArg)
      : std::runtime_error(whatArg)
      , m_lineNumber(0)
    {
    }

    explicit OpenVGException(const char* const psz, std::string fileName, const int lineNumber)
      : std::runtime_error(psz)
      , m_fileName(std::move(fileName))
      , m_lineNumber(lineNumber)
    {
    }

    explicit OpenVGException(const std::string& whatArg, std::string fileName, const int lineNumber)
      : std::runtime_error(whatArg)
      , m_fileName(std::move(fileName))
      , m_lineNumber(lineNumber)
    {
    }

    std::string GetFileName() const
    {
      return m_fileName;
    }


    int GetLineNumber() const
    {
      return m_lineNumber;
    }
  };


  class OpenVGErrorException : public OpenVGException
  {
    VGErrorCode m_errorCode;

  public:
    explicit OpenVGErrorException(const char* const psz, const VGErrorCode errorCode)
      : OpenVGException(psz)
      , m_errorCode(errorCode)
    {
    }

    explicit OpenVGErrorException(const std::string& whatArg, const VGErrorCode errorCode)
      : OpenVGException(whatArg)
      , m_errorCode(errorCode)
    {
    }

    explicit OpenVGErrorException(const char* const psz, const VGErrorCode errorCode, const std::string& fileName, const int lineNumber)
      : OpenVGException(psz, fileName, lineNumber)
      , m_errorCode(errorCode)
    {
    }

    explicit OpenVGErrorException(const std::string& whatArg, const VGErrorCode errorCode, const std::string& fileName, const int lineNumber)
      : OpenVGException(whatArg, fileName, lineNumber)
      , m_errorCode(errorCode)
    {
    }

    VGErrorCode GetErrorCode() const
    {
      return m_errorCode;
    }
  };
}

#endif
