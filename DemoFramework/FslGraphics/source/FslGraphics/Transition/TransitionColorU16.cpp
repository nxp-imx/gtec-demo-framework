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

#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Transition/TransitionTimeSpanHelper.hpp>
#include <FslGraphics/Transition/TransitionColorU16.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  TransitionColorU16::TransitionColorU16() noexcept = default;


  TransitionColorU16::TransitionColorU16(const TimeSpan time) noexcept
    : m_endTime(time)
    , m_currentTime(time)
  {
    SetTransitionTime(time, m_transitionType);
  }


  TransitionColorU16::TransitionColorU16(const TimeSpan time, const TransitionType type) noexcept
    : m_transitionType(type)
    , m_fnEasingFunction(EasingFunctionUtil::GetEasingFunction(type))
    , m_endTime(time)
    , m_currentTime(time)
  {
    SetTransitionTime(time, type);
  }


  TimeSpan TransitionColorU16::GetStartDelay() const noexcept
  {
    return m_startDelay;
  }


  void TransitionColorU16::SetStartDelay(const TimeSpan value) noexcept
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


  void TransitionColorU16::SetValue(const ColorU16 value) noexcept
  {
    if (value != m_target)
    {
      m_from = m_val;
      m_target = value;
      CalcTransition();
    }
  }


  void TransitionColorU16::SetActualValue(const ColorU16 value) noexcept
  {
    m_from = value;
    m_target = value;
    m_val = value;
    m_currentTime = m_endTime;
  }


  void TransitionColorU16::ForceComplete() noexcept
  {
    m_from = m_target;
    m_val = m_target;
    m_currentTime = m_endTime;
  }


  TimeSpan TransitionColorU16::GetTransitionTime() const noexcept
  {
    return TimeSpan(m_endTime + m_startDelay);
  }


  void TransitionColorU16::SetTransitionTime(const TimeSpan time) noexcept
  {
    SetTransitionTime(time, m_transitionType);
  }


  void TransitionColorU16::SetTransitionTime(const TimeSpan time, const TransitionType type) noexcept
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


  TransitionState TransitionColorU16::Update(const TimeSpan deltaTime) noexcept
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

        const float factor = m_fnEasingFunction(progress);

        int32_t valR = m_from.RawR() + static_cast<int32_t>(std::round(static_cast<float>(m_target.RawR() - m_from.RawR()) * factor));
        int32_t valG = m_from.RawG() + static_cast<int32_t>(std::round(static_cast<float>(m_target.RawG() - m_from.RawG()) * factor));
        int32_t valB = m_from.RawB() + static_cast<int32_t>(std::round(static_cast<float>(m_target.RawB() - m_from.RawB()) * factor));
        int32_t valA = m_from.RawA() + static_cast<int32_t>(std::round(static_cast<float>(m_target.RawA() - m_from.RawA()) * factor));
        m_val = ColorU16::UncheckedCreateR16G16B16A16UNorm(MathHelper::Clamp(valR, 0, 0xFFFF), MathHelper::Clamp(valG, 0, 0xFFFF),
                                                           MathHelper::Clamp(valB, 0, 0xFFFF), MathHelper::Clamp(valA, 0, 0xFFFF));
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


  void TransitionColorU16::CalcTransition() noexcept
  {
    m_currentTime = -m_startDelay;
  }
}
