#ifndef FSLSIMPLEUI_BASE_CONTROL_SCROLLGESTUREANIMATIONCONFIG_HPP
#define FSLSIMPLEUI_BASE_CONTROL_SCROLLGESTUREANIMATIONCONFIG_HPP
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

#include <FslBase/Math/Dp/DpValueF.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/Transition/TransitionType.hpp>

namespace Fsl::UI
{
  struct ScrollGestureAnimationConfig final
  {
    DpValueF DragFlickDeceleration;
    TransitionType DragFlickTransitionType{};
    float DragEndAnimTimeMultiplier{};
    Vector2 BounceSpringStiffness;
    TimeSpan BounceAnimationTime;
    TransitionType BounceTransitionType{};

    constexpr ScrollGestureAnimationConfig() noexcept = default;

    constexpr ScrollGestureAnimationConfig(const DpValueF dragFlickDeceleration, const TransitionType dragFlickTransitionType,
                                           const float dragEndAnimTimeMultiplier, const Vector2 bounceSpringStiffness,
                                           const TimeSpan bounceAnimationTime, const TransitionType bounceTransitionType) noexcept
      : DragFlickDeceleration(dragFlickDeceleration.Value >= 0 ? dragFlickDeceleration : -dragFlickDeceleration)
      , DragFlickTransitionType(dragFlickTransitionType)
      , DragEndAnimTimeMultiplier(dragEndAnimTimeMultiplier >= 0 ? dragEndAnimTimeMultiplier : -dragEndAnimTimeMultiplier)
      , BounceSpringStiffness(bounceSpringStiffness)
      , BounceAnimationTime(bounceAnimationTime)
      , BounceTransitionType(bounceTransitionType)
    {
    }
  };
}


#endif
