#ifndef FSLGRAPHICS_TRANSITION_TRANSITIONCOLOR_HPP
#define FSLGRAPHICS_TRANSITION_TRANSITIONCOLOR_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Transition/TransitionState.hpp>
#include <FslBase/Transition/TransitionTimeSpan.hpp>
#include <FslBase/Transition/TransitionType.hpp>
#include <FslGraphics/Color.hpp>
#include <memory>
#include <vector>

namespace Fsl
{
  class TransitionCache;

  class TransitionColor
  {
    std::shared_ptr<std::vector<float>> m_transition;
    TransitionType m_transitionType{TransitionType::Smooth};
    Color m_val;
    Color m_from;
    Color m_target;

    int64_t m_currentTime{0};
    int64_t m_endTime{0};
    int32_t m_startDelay{0};

  public:
    TransitionColor();
    TransitionColor(TransitionCache& rTransitionCache, const TransitionTimeSpan& time);
    TransitionColor(TransitionCache& rTransitionCache, const TransitionTimeSpan& time, const TransitionType type);

    //! @brief The timespan that we will wait before we start the actual animation (however the animation is considered in progresses while waiting)
    TransitionTimeSpan GetStartDelay() const;

    void SetStartDelay(const TransitionTimeSpan& value);

    //! @brief Check if the animation is completed
    bool IsCompleted() const noexcept
    {
      return m_currentTime >= m_endTime;
    }

    //! @brief Get the current value
    Color GetValue() const noexcept
    {
      return m_val;
    }

    void SetValue(const Color& value);

    //! @brief Get the actual value (the value the animation will finish at)
    Color GetActualValue() const noexcept
    {
      return m_target;
    }

    //! @brief Set the actual value, this force completes the animation
    void SetActualValue(const Color& value);

    void ForceComplete();

    TransitionTimeSpan GetTransitionTime() const;
    void SetTransitionTime(TransitionCache& rTransitionCache, const TransitionTimeSpan& time);
    void SetTransitionTime(TransitionCache& rTransitionCache, const TransitionTimeSpan& time, const TransitionType type);
    TransitionState Update(const TransitionTimeSpan& deltaTime);

  private:
    void CalcTransition();
  };
}

#endif
