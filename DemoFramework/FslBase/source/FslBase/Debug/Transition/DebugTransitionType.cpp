/****************************************************************************************************************************************************
 * Copyright 2018, 2022, 2024 NXP
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

#include <FslBase/Debug/Transition/DebugTransitionType.hpp>

namespace Fsl::Debug
{
  const char* TryToString(const TransitionType value) noexcept
  {
    switch (value)
    {
    case TransitionType::Linear:
      return "TransitionType::Linear";
    case TransitionType::EaseInSine:
      return "TransitionType::EaseInSine";
    case TransitionType::EaseOutSine:
      return "TransitionType::EaseOutSine";
    case TransitionType::EaseInOutSine:
      return "TransitionType::EaseInOutSine";
    case TransitionType::EaseInQuad:
      return "TransitionType::EaseInQuad";
    case TransitionType::EaseOutQuad:
      return "TransitionType::EaseOutQuad";
    case TransitionType::EaseInOutQuad:
      return "TransitionType::EaseInOutQuad";
    case TransitionType::EaseInCubic:
      return "TransitionType::EaseInCubic";
    case TransitionType::EaseOutCubic:
      return "TransitionType::EaseOutCubic";
    case TransitionType::EaseInOutCubic:
      return "TransitionType::EaseInOutCubic";
    case TransitionType::EaseInQuart:
      return "TransitionType::EaseInQuart";
    case TransitionType::EaseOutQuart:
      return "TransitionType::EaseOutQuart";
    case TransitionType::EaseInOutQuart:
      return "TransitionType::EaseInOutQuart";
    case TransitionType::EaseInQuint:
      return "TransitionType::EaseInQuint";
    case TransitionType::EaseOutQuint:
      return "TransitionType::EaseOutQuint";
    case TransitionType::EaseInOutQuint:
      return "TransitionType::EaseInOutQuint";
    case TransitionType::EaseInExpo:
      return "TransitionType::EaseInExpo";
    case TransitionType::EaseOutExpo:
      return "TransitionType::EaseOutExpo";
    case TransitionType::EaseInOutExpo:
      return "TransitionType::EaseInOutExpo";
    case TransitionType::EaseInCirc:
      return "TransitionType::EaseInCirc";
    case TransitionType::EaseOutCirc:
      return "TransitionType::EaseOutCirc";
    case TransitionType::EaseInOutCirc:
      return "TransitionType::EaseInOutCirc";
    case TransitionType::EaseInBack:
      return "TransitionType::EaseInBack";
    case TransitionType::EaseOutBack:
      return "TransitionType::EaseOutBack";
    case TransitionType::EaseInOutBack:
      return "TransitionType::EaseInOutBack";
    case TransitionType::EaseInElastic:
      return "TransitionType::EaseInElastic";
    case TransitionType::EaseOutElastic:
      return "TransitionType::EaseOutElastic";
    case TransitionType::EaseInOutElastic:
      return "TransitionType::EaseInOutElastic";
    case TransitionType::EaseInBounce:
      return "TransitionType::EaseInBounce";
    case TransitionType::EaseOutBounce:
      return "TransitionType::EaseOutBounce";
    case TransitionType::EaseInOutBounce:
      return "TransitionType::EaseInOutBounce";
    }
    return nullptr;
  }

  const char* ToString(const TransitionType value) noexcept
  {
    const auto* const psz = TryToString(value);
    return psz != nullptr ? psz : "Unknown";
  }
}
