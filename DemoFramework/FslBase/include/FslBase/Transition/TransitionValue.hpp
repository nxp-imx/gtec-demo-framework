#ifndef FSLBASE_TRANSITION_TRANSITIONVALUE_HPP
#define FSLBASE_TRANSITION_TRANSITIONVALUE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

//! This code is inspired by the Mana Battery C# transition classes.

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/Transition/EasingFunctionUtil.hpp>
#include <FslBase/Transition/TransitionState.hpp>
#include <FslBase/Transition/TransitionType.hpp>

namespace Fsl
{
  class TransitionValue
  {
    TransitionType m_transitionType{TransitionType::Smooth};
    EasingFunctionUtil::FNEasingFunction m_fnEasingFunction{EasingFunctionUtil::GetEasingFunction(m_transitionType)};

    float m_val{0};
    float m_from{0};
    float m_target{0};

    TimeSpan m_endTime;
    TimeSpan m_currentTime;
    TimeSpan m_startDelay;

  public:
    TransitionValue() noexcept;
    explicit TransitionValue(const TimeSpan time) noexcept;
    TransitionValue(const TimeSpan time, const TransitionType type) noexcept;

    //! @brief The timespan that we will wait before we start the actual animation (however the animation is considered in progresses while waiting)
    TimeSpan GetStartDelay() const noexcept;

    void SetStartDelay(const TimeSpan value) noexcept;

    //! @brief Check if the animation is completed
    bool IsCompleted() const noexcept
    {
      return m_currentTime >= m_endTime;
    }

    //! @brief Get the current value
    float GetValue() const noexcept
    {
      return m_val;
    }
    void SetValue(const float value);

    //! @brief Get the actual value (the value the animation will finish at)
    float GetActualValue() const noexcept
    {
      return m_target;
    }

    //! @brief Set the actual value, this force completes the animation
    void SetActualValue(const float value) noexcept;

    void ForceComplete() noexcept;

    TimeSpan GetTransitionTime() const noexcept;
    void SetTransitionTime(const TimeSpan time) noexcept;
    void SetTransitionTime(const TimeSpan time, const TransitionType type) noexcept;
    TransitionState Update(const TimeSpan deltaTime) noexcept;

  private:
    void CalcTransition() noexcept;
  };
}

#endif
