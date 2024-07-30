#ifndef FSLSIMPLEUI_BASE_GESTURE_EVENT_GESTUREBASICEVENT_HPP
#define FSLSIMPLEUI_BASE_GESTURE_EVENT_GESTUREBASICEVENT_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslSimpleUI/Base/Gesture/Event/GestureEventType.hpp>

namespace Fsl::UI
{
  struct GestureBasicEvent
  {
    GestureEventType Type{GestureEventType::Nop};
    int32_t Param0{0};
    int32_t Param1{0};
    // int32_t Param2{0};
    // int32_t Param3{0};
    // int32_t Param4{0};
    // int32_t Param5{0};
    // int32_t Param6{0};
    float Param0F{0};
    float Param1F{0};
    // float Param2F{0};
    // float Param3F{0};

    constexpr GestureBasicEvent() noexcept = default;


    constexpr explicit GestureBasicEvent(const GestureEventType type) noexcept
      : Type(type)
    {
    }

    constexpr GestureBasicEvent(const GestureEventType type, const int32_t param0, const int32_t param1) noexcept
      : Type(type)
      , Param0(param0)
      , Param1(param1)
    {
    }

    constexpr GestureBasicEvent(const GestureEventType type, const int32_t param0, const int32_t param1, const float param0F,
                                const float param1F) noexcept
      : Type(type)
      , Param0(param0)
      , Param1(param1)
      , Param0F(param0F)
      , Param1F(param1F)
    {
    }
  };
}

#endif
