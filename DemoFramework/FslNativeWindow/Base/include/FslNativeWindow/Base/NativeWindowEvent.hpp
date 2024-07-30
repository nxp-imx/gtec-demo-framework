#ifndef FSLNATIVEWINDOW_BASE_NATIVEWINDOWEVENT_HPP
#define FSLNATIVEWINDOW_BASE_NATIVEWINDOWEVENT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Time/MillisecondTickCount32.hpp>
#include <FslNativeWindow/Base/NativeWindowEventType.hpp>

namespace Fsl
{
  struct NativeWindowEvent final
  {
    MillisecondTickCount32 Timestamp;
    NativeWindowEventType Type{NativeWindowEventType::NOP};
    int32_t Arg1{0};
    int32_t Arg2{0};
    int32_t Arg3{0};
    int32_t Arg4{0};

    constexpr NativeWindowEvent() noexcept = default;

    constexpr NativeWindowEvent(const MillisecondTickCount32 timestamp, const NativeWindowEventType type) noexcept
      : Timestamp(timestamp)
      , Type(type)
    {
    }

    constexpr NativeWindowEvent(const MillisecondTickCount32 timestamp, const NativeWindowEventType type, const int32_t arg1) noexcept
      : Timestamp(timestamp)
      , Type(type)
      , Arg1(arg1)
    {
    }

    constexpr NativeWindowEvent(const MillisecondTickCount32 timestamp, const NativeWindowEventType type, const int32_t arg1,
                                const int32_t arg2) noexcept
      : Timestamp(timestamp)
      , Type(type)
      , Arg1(arg1)
      , Arg2(arg2)
    {
    }

    constexpr NativeWindowEvent(const MillisecondTickCount32 timestamp, const NativeWindowEventType type, const int32_t arg1, const int32_t arg2,
                                const int32_t arg3) noexcept
      : Timestamp(timestamp)
      , Type(type)
      , Arg1(arg1)
      , Arg2(arg2)
      , Arg3(arg3)
    {
    }

    constexpr NativeWindowEvent(const MillisecondTickCount32 timestamp, const NativeWindowEventType type, const int32_t arg1, const int32_t arg2,
                                const int32_t arg3, const int32_t arg4) noexcept
      : Timestamp(timestamp)
      , Type(type)
      , Arg1(arg1)
      , Arg2(arg2)
      , Arg3(arg3)
      , Arg4(arg4)
    {
    }
  };
}

#endif
