#ifndef FSLBASE_TRANSITION_TRANSITIONPXRECTANGLE_HPP
#define FSLBASE_TRANSITION_TRANSITIONPXRECTANGLE_HPP
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

//! This code is inspired by the Mana Battery C# transition classes.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/Transition/EasingFunctionUtil.hpp>
#include <FslBase/Transition/TransitionState.hpp>
#include <FslBase/Transition/TransitionType.hpp>

namespace Fsl
{
  class TransitionPxRectangle
  {
    TransitionType m_transitionType{TransitionType::Smooth};
    EasingFunctionUtil::FNEasingFunction m_fnEasingFunction{EasingFunctionUtil::GetEasingFunction(m_transitionType)};

    PxRectangle m_val;
    PxRectangle m_from;
    PxRectangle m_target;

    TimeSpan m_endTime;
    TimeSpan m_currentTime;
    TimeSpan m_startDelay;

  public:
    TransitionPxRectangle() noexcept;
    explicit TransitionPxRectangle(const TimeSpan time);
    TransitionPxRectangle(const TimeSpan time, const TransitionType type);

    //! @brief The timespan that we will wait before we start the actual animation (however the animation is considered in progresses while waiting)
    TimeSpan GetStartDelay() const noexcept;

    void SetStartDelay(const TimeSpan value) noexcept;

    //! @brief Check if the animation is completed
    bool IsCompleted() const noexcept
    {
      return m_currentTime >= m_endTime;
    }

    //! @brief Get the current value
    PxRectangle GetValue() const noexcept
    {
      return m_val;
    }
    void SetValue(const PxRectangle value);

    //! @brief Get the actual value (the value the animation will finish at)
    PxRectangle GetActualValue() const noexcept
    {
      return m_target;
    }

    //! @brief Set the actual value, this force completes the animation
    void SetActualValue(const PxRectangle value) noexcept;

    void ForceComplete() noexcept;

    TimeSpan GetTransitionTime() const noexcept;
    void SetTransitionTime(const TimeSpan time);
    void SetTransitionTime(const TimeSpan time, const TransitionType type);
    TransitionState Update(const TimeSpan deltaTime) noexcept;

  private:
    void CalcTransition() noexcept;
  };
}

#endif
