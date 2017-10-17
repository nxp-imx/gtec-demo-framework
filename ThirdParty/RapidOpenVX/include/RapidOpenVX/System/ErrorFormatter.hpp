#ifndef RAPIDOPENVX_SYSTEM_ERRORFORMATTER_HPP
#define RAPIDOPENVX_SYSTEM_ERRORFORMATTER_HPP
//***************************************************************************************************************************************************
//* BSD 3-Clause License
//*
//* Copyright (c) 2017, Rene Thrane
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

#include <RapidOpenVX/System/Macro.hpp>
#include <string>
#include <VX/vx.h>

namespace RapidOpenVX
{
  namespace ErrorFormatter
  {
#ifdef RAPIDOPENVX_ERRORFORMATTER_EXTERN
    extern std::string Format(const std::string& message, const vx_status errorCode);
    extern std::string Format(const std::string& message, const vx_status errorCode, const std::string& fileName, const int lineNumber);
#else

    inline std::string Format(const std::string& message, const vx_status errorCode)
    {
      RAPIDOPENVX_PARAM_NOT_USED(errorCode);
      return message;
    }

    inline std::string Format(const std::string& message, const vx_status errorCode, const std::string& fileName, const int lineNumber)
    {
      RAPIDOPENVX_PARAM_NOT_USED(errorCode);
      RAPIDOPENVX_PARAM_NOT_USED(fileName);
      RAPIDOPENVX_PARAM_NOT_USED(lineNumber);
      return message;
    }      

    #endif
  }
}

#endif
