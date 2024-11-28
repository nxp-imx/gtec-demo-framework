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

#include <FslBase/Transition/EasingFunctionUtil.hpp>
#include <FslBase/Transition/TransitionPxRectangle.hpp>
#include <FslBase/Transition/TransitionTimeSpanHelper.hpp>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl
{
  TransitionPxRectangle::TransitionPxRectangle() noexcept = default;


  TransitionPxRectangle::TransitionPxRectangle(const TimeSpan time)
    : m_endTime(time)
    , m_currentTime(time)
  {
    SetTransitionTime(time, m_transitionType);
  }


  TransitionPxRectangle::TransitionPxRectangle(const TimeSpan time, const TransitionType type)
    : m_transitionType(type)
    , m_fnEasingFunction(EasingFunctionUtil::GetEasingFunction(type))
    , m_endTime(time)
    , m_currentTime(time)
  {
    SetTransitionTime(time, type);
  }


  TimeSpan TransitionPxRectangle::GetStartDelay() const noexcept
  {
    return m_startDelay;
  }


  void TransitionPxRectangle::SetStartDelay(const TimeSpan value) noexcept
  {
    if (value != m_startDelay)
    {
      if (!IsCompleted())
      {
        m_currentTime = -value;
      }
      m_startDelay = value;
    }
  }

  void TransitionPxRectangle::SetValue(const PxRectangle value)
  {
    if (value != m_target)
    {
      m_from = m_val;
      m_target = value;
      CalcTransition();
    }
  }


  void TransitionPxRectangle::SetActualValue(const PxRectangle value) noexcept
  {
    m_from = value;
    m_target = value;
    m_val = value;
    m_currentTime = m_endTime;
  }


  void TransitionPxRectangle::ForceComplete() noexcept
  {
    m_from = m_target;
    m_val = m_target;
    m_currentTime = m_endTime;
  }


  TimeSpan TransitionPxRectangle::GetTransitionTime() const noexcept
  {
    return TimeSpan(m_endTime + m_startDelay);
  }


  void TransitionPxRectangle::SetTransitionTime(const TimeSpan time)
  {
    SetTransitionTime(time, m_transitionType);
  }


  void TransitionPxRectangle::SetTransitionTime(const TimeSpan time, const TransitionType type)
  {
    if (m_endTime != time || type != m_transitionType)
    {
      m_endTime = time >= TimeSpan(0) ? time : TimeSpan();

      if (type != m_transitionType)
      {
        m_transitionType = type;
        m_fnEasingFunction = EasingFunctionUtil::GetEasingFunction(type);
      }

      if (m_target != m_val)
      {
        CalcTransition();
      }
      else
      {
        ForceComplete();
      }
    }
  }


  TransitionState TransitionPxRectangle::Update(const TimeSpan deltaTime) noexcept
  {
    if (m_currentTime < m_endTime)
    {
      // We do the increase here because we want to skip the zero starting point
      m_currentTime += deltaTime;
      if (m_currentTime <= TimeSpan(0))
      {
        return TransitionState::StartDelay;
      }
      if (m_currentTime < m_endTime)
      {
        const auto progress = static_cast<float>(static_cast<double>(m_currentTime.Ticks()) / static_cast<double>(m_endTime.Ticks()));
        assert(progress >= 0.0f && progress <= 1.0f);

        const float weight = m_fnEasingFunction(progress);

        const auto fromLeftPxf = static_cast<float>(m_from.RawLeft());
        const auto fromTopPxf = static_cast<float>(m_from.RawTop());
        const auto fromWidthPxf = static_cast<float>(m_from.RawWidth());
        const auto fromHeightPxf = static_cast<float>(m_from.RawHeight());

        const auto targetLeftPxf = static_cast<float>(m_target.RawLeft());
        const auto targetTopPxf = static_cast<float>(m_target.RawTop());
        const auto targetWidthPxf = static_cast<float>(m_target.RawWidth());
        const auto targetHeightPxf = static_cast<float>(m_target.RawHeight());

        const float leftPxf = std::round(fromLeftPxf + ((targetLeftPxf - fromLeftPxf) * weight));
        const float topPxf = std::round(fromTopPxf + ((targetTopPxf - fromTopPxf) * weight));
        const float widthPxf = std::round(fromWidthPxf + ((targetWidthPxf - fromWidthPxf) * weight));
        const float heightPxf = std::round(fromHeightPxf + ((targetHeightPxf - fromHeightPxf) * weight));

        m_val = PxRectangle::Create(static_cast<int32_t>(leftPxf), static_cast<int32_t>(topPxf), static_cast<int32_t>(widthPxf),
                                    static_cast<int32_t>(heightPxf));

        return TransitionState::Running;
      }

      m_currentTime = m_endTime;
      m_val = m_target;
      return TransitionState::Idle;
    }

    m_currentTime = m_endTime;
    m_val = m_target;
    return TransitionState::Idle;
  }


  void TransitionPxRectangle::CalcTransition() noexcept
  {
    m_currentTime = -m_startDelay;
  }
}
