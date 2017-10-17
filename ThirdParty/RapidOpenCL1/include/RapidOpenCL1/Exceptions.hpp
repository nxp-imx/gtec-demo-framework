#ifndef RAPIDOPENCL1_EXCEPTIONS_HPP
#define RAPIDOPENCL1_EXCEPTIONS_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2016, Rene Thrane
//* All rights reserved.
//*
//* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//*
//* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the distribution.
//* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
//*    software without specific prior written permission.
//*
//* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//***************************************************************************************************************************************************

#include <stdexcept>
#include <string>
#include <CL/cl.h>

namespace RapidOpenCL1
{
  class OpenCLException : public std::runtime_error
  {
    std::string m_fileName;
    int m_lineNumber;
  public:
    explicit OpenCLException(const std::string& whatArg)
      : std::runtime_error(whatArg)
      , m_fileName()
      , m_lineNumber(0)
    {
    }

    explicit OpenCLException(const std::string& whatArg, const std::string& fileName, const int lineNumber)
      : std::runtime_error(whatArg)
      , m_fileName(fileName)
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



  class OpenCLErrorException : public OpenCLException
  {
    cl_int m_errorCode;
  public:
    explicit OpenCLErrorException(const std::string& whatArg, const cl_int errorCode)
      : OpenCLException(whatArg)
      , m_errorCode(errorCode)
    {
    }

    explicit OpenCLErrorException(const std::string& whatArg, const cl_int errorCode, const std::string& fileName, const int lineNumber)
      : OpenCLException(whatArg, fileName, lineNumber)
      , m_errorCode(errorCode)
    {
    }

    cl_int GetErrorCode() const
    {
      return m_errorCode;
    }
  };
}

#endif
