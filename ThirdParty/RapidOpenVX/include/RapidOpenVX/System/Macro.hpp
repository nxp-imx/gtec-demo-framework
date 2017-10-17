#ifndef RAPIDOPENVX_SYSTEM_MACRO_HPP
#define RAPIDOPENVX_SYSTEM_MACRO_HPP
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

#define RAPIDOPENVX_PARAM_NOT_USED(pARAM)    ((void)(pARAM))


#ifdef FSL_DEMOFRAMEWORK

// Use the FslBase implementation
#include <FslBase/Attributes.hpp>
#define RAPIDOPENVX_ATTR_DEPRECATED                            FSL_ATTR_DEPRECATED
#define RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT            FSL_FUNC_POSTFIX_WARN_UNUSED_RESULT

#else

  #ifdef __GNUC__

    // GCC
    #if __cplusplus > 201103 // Check if its C++14
      #define RAPIDOPENVX_ATTR_DEPRECATED                        [[deprecated]]
    #else
      #define RAPIDOPENVX_ATTR_DEPRECATED
    #endif
    #define RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT          __attribute__((warn_unused_result))

  #elif defined(_MSC_VER)

    // Visual studio
    #define RAPIDOPENVX_ATTR_DEPRECATED                          __declspec(deprecated)
    #define RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT

  #else

    #pragma message("WARNING: RAPIDOPENVX_ATTR_DEPRECATED, RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT not implemented for this compiler")
    #define RAPIDOPENVX_ATTR_DEPRECATED
    #define RAPIDOPENVX_FUNC_POSTFIX_WARN_UNUSED_RESULT
  
  #endif

#endif

#endif
