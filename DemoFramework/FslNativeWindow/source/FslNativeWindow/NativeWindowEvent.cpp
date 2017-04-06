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

#include <FslNativeWindow/NativeWindowEvent.hpp>

namespace Fsl
{

  NativeWindowEvent::NativeWindowEvent()
    : Type(NativeWindowEventType::NOP)
    , Arg1(0)
    , Arg2(0)
    , Arg3(0)
  {
  }


  NativeWindowEvent::NativeWindowEvent(const NativeWindowEventType type)
    : Type(type)
    , Arg1(0)
    , Arg2(0)
    , Arg3(0)
  {
  }


  NativeWindowEvent::NativeWindowEvent(const NativeWindowEventType type, const int32_t arg1)
    : Type(type)
    , Arg1(arg1)
    , Arg2(0)
    , Arg3(0)
  {
  }


  NativeWindowEvent::NativeWindowEvent(const NativeWindowEventType type, const int32_t arg1, const int32_t arg2)
    : Type(type)
    , Arg1(arg1)
    , Arg2(arg2)
    , Arg3(0)
  {
  }


  NativeWindowEvent::NativeWindowEvent(const NativeWindowEventType type, const int32_t arg1, const int32_t arg2, const int32_t arg3)
    : Type(type)
    , Arg1(arg1)
    , Arg2(arg2)
    , Arg3(arg3)
  {
  }
}
