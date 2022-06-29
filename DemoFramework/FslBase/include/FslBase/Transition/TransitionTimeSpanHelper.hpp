#ifndef FSLBASE_TRANSITION_TRANSITIONTIMESPANHELPER_HPP
#define FSLBASE_TRANSITION_TRANSITIONTIMESPANHELPER_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Time/TimeInfo.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/Transition/TransitionTimeUnit.hpp>
#include <cassert>
#include <limits>

namespace Fsl::TimeSpanHelper
{
  inline constexpr TimeSpan Create(const int32_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      return TimeSpan(static_cast<int64_t>(time) * TimeInfo::TicksPerMillisecond);
    case TransitionTimeUnit::Microseconds:
      return TimeSpan(static_cast<int64_t>(time) * TimeInfo::TicksPerMicrosecond);
      break;
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  inline constexpr TimeSpan Create(const uint32_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      return TimeSpan(static_cast<int64_t>(time) * TimeInfo::TicksPerMillisecond);
    case TransitionTimeUnit::Microseconds:
      return TimeSpan(static_cast<int64_t>(time) * TimeInfo::TicksPerMicrosecond);
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  inline constexpr TimeSpan Create(const int64_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      assert(time <= (std::numeric_limits<int64_t>::max() / TimeInfo::TicksPerMillisecond));
      return TimeSpan(time * TimeInfo::TicksPerMillisecond);
      break;
    case TransitionTimeUnit::Microseconds:
      assert(time <= (std::numeric_limits<int64_t>::max() / TimeInfo::TicksPerMicrosecond));
      return TimeSpan(time * TimeInfo::TicksPerMicrosecond);
      break;
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  inline constexpr TimeSpan Create(const uint64_t time, TransitionTimeUnit unit)
  {
    switch (unit)
    {
    case TransitionTimeUnit::Milliseconds:
      assert(time <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max() / TimeInfo::TicksPerMillisecond));
      return TimeSpan(UncheckedNumericCast<int64_t>(time * TimeInfo::TicksPerMillisecond));
    case TransitionTimeUnit::Microseconds:
      assert(time <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max() / TimeInfo::TicksPerMicrosecond));
      return TimeSpan(UncheckedNumericCast<int64_t>(time * TimeInfo::TicksPerMicrosecond));
    default:
      throw NotSupportedException("The given time unit has not been implemented");
    }
  }

  inline constexpr int32_t AsSecondsRoundedUp(const TimeSpan timeSpan)
  {
    return static_cast<int32_t>((timeSpan.Ticks() / TimeInfo::TicksPerSecond) + ((timeSpan.Ticks() % TimeInfo::TicksPerSecond) != 0 ? 1 : 0));
  }
}

#endif
