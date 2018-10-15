#ifndef FSLBASE_EXCEPTIONS_HPP
#define FSLBASE_EXCEPTIONS_HPP
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

#include <stdexcept>
#include <string>

namespace Fsl
{
  class UsageErrorException : public std::logic_error
  {
  public:
    UsageErrorException()
      : std::logic_error("UsageErrorException")
    {
    }

    explicit UsageErrorException(const std::string& whatArg)
      : std::logic_error(whatArg)
    {
    }
  };


  class UnknownTypeException : public std::runtime_error
  {
  public:
    explicit UnknownTypeException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  class NotFoundException : public std::runtime_error
  {
  public:
    explicit NotFoundException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  class DirectoryNotFoundException : public NotFoundException
  {
  public:
    explicit DirectoryNotFoundException(const std::string& whatArg)
      : NotFoundException(whatArg)
    {
    }
  };


  class InitFailedException : public std::runtime_error
  {
  public:
    explicit InitFailedException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  class NotImplementedException : public std::logic_error
  {
  public:
    NotImplementedException()
      : std::logic_error("Not implemented")
    {
    }

    explicit NotImplementedException(const std::string& whatArg)
      : std::logic_error(whatArg)
    {
    }
  };

  class NotSupportedException : public std::logic_error
  {
  public:
    NotSupportedException()
      : std::logic_error("Not supported")
    {
    }

    explicit NotSupportedException(const std::string& whatArg)
      : std::logic_error(whatArg)
    {
    }
  };


  class IndexOutOfRangeException : public std::logic_error
  {
  public:
    IndexOutOfRangeException()
      : std::logic_error("Index out of range")
    {
    }

    explicit IndexOutOfRangeException(const std::string& whatArg)
      : std::logic_error(whatArg)
    {
    }
  };


  class GraphicsException : public std::runtime_error
  {
  public:
    explicit GraphicsException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  class IOException : public std::runtime_error
  {
  public:
    explicit IOException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };

  class OverflowException : public std::runtime_error
  {
  public:
    explicit OverflowException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };

  class FormatException : public std::runtime_error
  {
  public:
    explicit FormatException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  class InvalidUTF8StringException : public std::runtime_error
  {
  public:
    explicit InvalidUTF8StringException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  class InvalidFormatException : public std::runtime_error
  {
  public:
    explicit InvalidFormatException(const std::string& whatArg)
      : std::runtime_error(whatArg)
    {
    }
  };


  // std::invalid_argument
}

#endif
