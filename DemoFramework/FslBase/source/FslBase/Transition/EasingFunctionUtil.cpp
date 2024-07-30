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

#include <FslBase/Transition/EasingFunction.hpp>
#include <FslBase/Transition/EasingFunctionUtil.hpp>

namespace Fsl::EasingFunctionUtil
{
  float LinearDefault(const float progress)
  {
    return progress;
  }


  float EasingFunction(const float progress, const TransitionType type)
  {
    switch (type)
    {
    case TransitionType::Linear:
      return EasingFunction::Linear(progress);
    case TransitionType::EaseInSine:
      return EasingFunction::EaseInSine(progress);
    case TransitionType::EaseOutSine:
      return EasingFunction::EaseOutSine(progress);
    case TransitionType::EaseInOutSine:
      return EasingFunction::EaseInOutSine(progress);
    case TransitionType::EaseInQuad:
      return EasingFunction::EaseInQuad(progress);
    case TransitionType::EaseOutQuad:
      return EasingFunction::EaseOutQuad(progress);
    case TransitionType::EaseInOutQuad:
      return EasingFunction::EaseInOutQuad(progress);
    case TransitionType::EaseInCubic:
      return EasingFunction::EaseInCubic(progress);
    case TransitionType::EaseOutCubic:
      return EasingFunction::EaseOutCubic(progress);
    case TransitionType::EaseInOutCubic:
      return EasingFunction::EaseInOutCubic(progress);
    case TransitionType::EaseInQuart:
      return EasingFunction::EaseInQuart(progress);
    case TransitionType::EaseOutQuart:
      return EasingFunction::EaseOutQuart(progress);
    case TransitionType::EaseInOutQuart:
      return EasingFunction::EaseInOutQuart(progress);
    case TransitionType::EaseInQuint:
      return EasingFunction::EaseInQuint(progress);
    case TransitionType::EaseOutQuint:
      return EasingFunction::EaseOutQuint(progress);
    case TransitionType::EaseInOutQuint:
      return EasingFunction::EaseInOutQuint(progress);
    case TransitionType::EaseInExpo:
      return EasingFunction::EaseInExpo(progress);
    case TransitionType::EaseOutExpo:
      return EasingFunction::EaseOutExpo(progress);
    case TransitionType::EaseInOutExpo:
      return EasingFunction::EaseInOutExpo(progress);
    case TransitionType::EaseInCirc:
      return EasingFunction::EaseInCirc(progress);
    case TransitionType::EaseOutCirc:
      return EasingFunction::EaseOutCirc(progress);
    case TransitionType::EaseInOutCirc:
      return EasingFunction::EaseInOutCirc(progress);
    case TransitionType::EaseInBack:
      return EasingFunction::EaseInBack(progress);
    case TransitionType::EaseOutBack:
      return EasingFunction::EaseOutBack(progress);
    case TransitionType::EaseInOutBack:
      return EasingFunction::EaseInOutBack(progress);
    case TransitionType::EaseInElastic:
      return EasingFunction::EaseInElastic(progress);
    case TransitionType::EaseOutElastic:
      return EasingFunction::EaseOutElastic(progress);
    case TransitionType::EaseInOutElastic:
      return EasingFunction::EaseInOutElastic(progress);
    case TransitionType::EaseInBounce:
      return EasingFunction::EaseInBounce(progress);
    case TransitionType::EaseOutBounce:
      return EasingFunction::EaseOutBounce(progress);
    case TransitionType::EaseInOutBounce:
      return EasingFunction::EaseInOutBounce(progress);
    }
    return EasingFunction::Linear(progress);
  }

  FNEasingFunction GetEasingFunction(const TransitionType type) noexcept
  {
    switch (type)
    {
    case TransitionType::Linear:
      return EasingFunction::Linear;
    case TransitionType::EaseInSine:
      return EasingFunction::EaseInSine;
    case TransitionType::EaseOutSine:
      return EasingFunction::EaseOutSine;
    case TransitionType::EaseInOutSine:
      return EasingFunction::EaseInOutSine;
    case TransitionType::EaseInQuad:
      return EasingFunction::EaseInQuad;
    case TransitionType::EaseOutQuad:
      return EasingFunction::EaseOutQuad;
    case TransitionType::EaseInOutQuad:
      return EasingFunction::EaseInOutQuad;
    case TransitionType::EaseInCubic:
      return EasingFunction::EaseInCubic;
    case TransitionType::EaseOutCubic:
      return EasingFunction::EaseOutCubic;
    case TransitionType::EaseInOutCubic:
      return EasingFunction::EaseInOutCubic;
    case TransitionType::EaseInQuart:
      return EasingFunction::EaseInQuart;
    case TransitionType::EaseOutQuart:
      return EasingFunction::EaseOutQuart;
    case TransitionType::EaseInOutQuart:
      return EasingFunction::EaseInOutQuart;
    case TransitionType::EaseInQuint:
      return EasingFunction::EaseInQuint;
    case TransitionType::EaseOutQuint:
      return EasingFunction::EaseOutQuint;
    case TransitionType::EaseInOutQuint:
      return EasingFunction::EaseInOutQuint;
    case TransitionType::EaseInExpo:
      return EasingFunction::EaseInExpo;
    case TransitionType::EaseOutExpo:
      return EasingFunction::EaseOutExpo;
    case TransitionType::EaseInOutExpo:
      return EasingFunction::EaseInOutExpo;
    case TransitionType::EaseInCirc:
      return EasingFunction::EaseInCirc;
    case TransitionType::EaseOutCirc:
      return EasingFunction::EaseOutCirc;
    case TransitionType::EaseInOutCirc:
      return EasingFunction::EaseInOutCirc;
    case TransitionType::EaseInBack:
      return EasingFunction::EaseInBack;
    case TransitionType::EaseOutBack:
      return EasingFunction::EaseOutBack;
    case TransitionType::EaseInOutBack:
      return EasingFunction::EaseInOutBack;
    case TransitionType::EaseInElastic:
      return EasingFunction::EaseInElastic;
    case TransitionType::EaseOutElastic:
      return EasingFunction::EaseOutElastic;
    case TransitionType::EaseInOutElastic:
      return EasingFunction::EaseInOutElastic;
    case TransitionType::EaseInBounce:
      return EasingFunction::EaseInBounce;
    case TransitionType::EaseOutBounce:
      return EasingFunction::EaseOutBounce;
    case TransitionType::EaseInOutBounce:
      return EasingFunction::EaseInOutBounce;
    }
    return EasingFunction::Linear;
  }
}
