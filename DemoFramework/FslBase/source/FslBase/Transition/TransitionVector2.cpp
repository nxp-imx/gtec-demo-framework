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

#include <FslBase/Math/EqualHelper_Vector2.hpp>
#include <FslBase/Transition/EasingFunctionUtil.hpp>
#include <FslBase/Transition/TransitionTimeSpanHelper.hpp>
#include <FslBase/Transition/TransitionVector2.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  TransitionVector2::TransitionVector2() noexcept = default;


  TransitionVector2::TransitionVector2(const TimeSpan time)
    : m_endTime(time)
    , m_currentTime(time)
  {
    SetTransitionTime(time, m_transitionType);
  }


  TransitionVector2::TransitionVector2(const TimeSpan time, const TransitionType type)
    : m_transitionType(type)
    , m_fnEasingFunction(EasingFunctionUtil::GetEasingFunction(type))
    , m_endTime(time)
    , m_currentTime(time)
  {
    SetTransitionTime(time, type);
  }


  TimeSpan TransitionVector2::GetStartDelay() const noexcept
  {
    return m_startDelay;
  }


  void TransitionVector2::SetStartDelay(const TimeSpan value) noexcept
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

  void TransitionVector2::SetValue(const Vector2& value)
  {
    if (!EqualHelper::IsAlmostEqual(value, m_target))
    {
      m_from = m_val;
      m_target = value;
      CalcTransition();
    }
  }


  void TransitionVector2::SetActualValue(const Vector2& value) noexcept
  {
    m_from = value;
    m_target = value;
    m_val = value;
    m_currentTime = m_endTime;
  }


  void TransitionVector2::ForceComplete() noexcept
  {
    m_from = m_target;
    m_val = m_target;
    m_currentTime = m_endTime;
  }


  TimeSpan TransitionVector2::GetTransitionTime() const noexcept
  {
    return TimeSpan(m_endTime + m_startDelay);
  }


  void TransitionVector2::SetTransitionTime(const TimeSpan time)
  {
    SetTransitionTime(time, m_transitionType);
  }


  void TransitionVector2::SetTransitionTime(const TimeSpan time, const TransitionType type)
  {
    if (m_endTime != time || type != m_transitionType)
    {
      m_endTime = time >= TimeSpan(0) ? time : TimeSpan();

      if (type != m_transitionType)
      {
        m_transitionType = type;
        m_fnEasingFunction = EasingFunctionUtil::GetEasingFunction(type);
      }

      if (!EqualHelper::IsAlmostEqual(m_target, m_val))
      {
        CalcTransition();
      }
      else
      {
        ForceComplete();
      }
    }
  }


  TransitionState TransitionVector2::Update(const TimeSpan deltaTime) noexcept
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
        m_val = m_from + ((m_target - m_from) * m_fnEasingFunction(progress));
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


  void TransitionVector2::CalcTransition() noexcept
  {
    m_currentTime = -m_startDelay;
  }
}
