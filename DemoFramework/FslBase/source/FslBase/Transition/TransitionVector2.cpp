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
#include <FslBase/Transition/TransitionCache.hpp>
#include <FslBase/Transition/TransitionConfig.hpp>
#include <FslBase/Transition/TransitionTimeSpanHelper.hpp>
#include <FslBase/Transition/TransitionVector2.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  TransitionVector2::TransitionVector2() = default;


  TransitionVector2::TransitionVector2(TransitionCache& rTransitionCache, const TimeSpan& time)
    : m_currentTime(time.Ticks())
    , m_endTime(time.Ticks())

  {
    SetTransitionTime(rTransitionCache, time, m_transitionType);
  }


  TransitionVector2::TransitionVector2(TransitionCache& rTransitionCache, const TimeSpan& time, const TransitionType type)
    : m_transitionType(type)
    , m_currentTime(time.Ticks())
    , m_endTime(time.Ticks())

  {
    SetTransitionTime(rTransitionCache, time, type);
  }


  TimeSpan TransitionVector2::GetStartDelay() const
  {
    return TimeSpan(m_startDelay);
  }


  void TransitionVector2::SetStartDelay(const TimeSpan& value)
  {
    auto ticks = value.Ticks();
    if (ticks != m_startDelay)
    {
      assert(ticks >= 0 && ticks <= std::numeric_limits<int32_t>::max());
      auto startDelay = static_cast<int32_t>(ticks);
      if (!IsCompleted())
      {
        m_currentTime = -startDelay;
      }
      m_startDelay = startDelay;
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


  void TransitionVector2::SetActualValue(const Vector2& value)
  {
    m_from = value;
    m_target = value;
    m_val = value;
    m_currentTime = m_endTime;
  }


  void TransitionVector2::ForceComplete()
  {
    m_from = m_target;
    m_val = m_target;
    m_currentTime = m_endTime;
  }


  TimeSpan TransitionVector2::GetTransitionTime() const
  {
    return TimeSpan(m_endTime + m_startDelay);
  }


  void TransitionVector2::SetTransitionTime(TransitionCache& rTransitionCache, const TimeSpan& time)
  {
    SetTransitionTime(rTransitionCache, time, m_transitionType);
  }


  void TransitionVector2::SetTransitionTime(TransitionCache& rTransitionCache, const TimeSpan& time, const TransitionType type)
  {
    if (!m_transition || m_endTime != time.Ticks() || type != m_transitionType)
    {
      m_endTime = time.Ticks() >= 0 ? time.Ticks() : 0;

      m_transitionType = type;
      int numSeconds = TimeSpanHelper::AsSecondsRoundedUp(time);
      m_transition = rTransitionCache.GetLookupTable((numSeconds * TransitionConfig::InternalResolutionPerSecond) + 1, type);
      assert(m_transition);

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


  TransitionState TransitionVector2::Update(const TimeSpan& deltaTime)
  {
    if (m_transition && m_currentTime < m_endTime)
    {
      // We do the increase here because the first entry in the m_transition table is zero which we want to skip
      m_currentTime += deltaTime.Ticks();
      if (m_currentTime < 0)
      {
        return TransitionState::StartDelay;
      }
      if (m_currentTime < m_endTime)
      {
        const auto toIndex = static_cast<int32_t>((static_cast<int64_t>(m_transition->size()) * m_currentTime) / m_endTime);
        assert(toIndex >= 0);
        assert(static_cast<uint32_t>(toIndex) < m_transition->size());
        m_val = m_from + ((m_target - m_from) * (*m_transition)[toIndex]);
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


  void TransitionVector2::CalcTransition()
  {
    m_currentTime = -m_startDelay;
  }
}
