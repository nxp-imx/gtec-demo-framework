#ifndef FSLBASE_TIME_TIMESPANUTIL_HPP
#define FSLBASE_TIME_TIMESPANUTIL_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslBase/NumericCast.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <limits>

namespace Fsl::TimeSpanUtil
{
  inline constexpr TimeSpan FromMicroseconds(const int32_t microseconds) noexcept
  {
    return TimeSpan(UncheckedNumericCast<int64_t>(microseconds) * TimeInfo::TicksPerMicrosecond);
  }

  inline constexpr TimeSpan FromMicroseconds(const uint32_t microseconds) noexcept
  {
    return TimeSpan(UncheckedNumericCast<int64_t>(microseconds) * TimeInfo::TicksPerMicrosecond);
  }

  inline constexpr TimeSpan FromMicroseconds(const int64_t microseconds)
  {
    if (microseconds < (std::numeric_limits<int64_t>::min() / TimeInfo::TicksPerMicrosecond))
    {
      throw UnderflowException("ticks underflow");
    }
    if (microseconds > (std::numeric_limits<int64_t>::max() / TimeInfo::TicksPerMicrosecond))
    {
      throw OverflowException("ticks overflow");
    }
    return TimeSpan(microseconds * TimeInfo::TicksPerMicrosecond);
  }

  inline constexpr TimeSpan FromMicroseconds(const uint64_t microseconds)
  {
    if (microseconds > (std::numeric_limits<uint64_t>::max() / TimeInfo::TicksPerMicrosecond))
    {
      throw OverflowException("ticks overflow");
    }
    return TimeSpan(NumericCast<int64_t>(microseconds * TimeInfo::TicksPerMicrosecond));
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr TimeSpan FromMilliseconds(const int32_t milliseconds) noexcept
  {
    return TimeSpan(UncheckedNumericCast<int64_t>(milliseconds) * TimeInfo::TicksPerMillisecond);
  }

  inline constexpr TimeSpan FromMilliseconds(const uint32_t milliseconds) noexcept
  {
    return TimeSpan(UncheckedNumericCast<int64_t>(milliseconds) * TimeInfo::TicksPerMillisecond);
  }

  inline constexpr TimeSpan FromMilliseconds(const int64_t milliseconds)
  {
    if (milliseconds < (std::numeric_limits<int64_t>::min() / TimeInfo::TicksPerMillisecond))
    {
      throw UnderflowException("ticks underflow");
    }
    if (milliseconds > (std::numeric_limits<int64_t>::max() / TimeInfo::TicksPerMillisecond))
    {
      throw OverflowException("ticks overflow");
    }

    const int64_t ticks = milliseconds * TimeInfo::TicksPerMillisecond;
    return TimeSpan(NumericCast<int64_t>(ticks));
  }

  inline constexpr TimeSpan FromMilliseconds(const uint64_t milliseconds)
  {
    if (milliseconds > (std::numeric_limits<uint64_t>::max() / TimeInfo::TicksPerMillisecond))
    {
      throw OverflowException("ticks overflow");
    }
    return TimeSpan(NumericCast<int64_t>(milliseconds * TimeInfo::TicksPerMillisecond));
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr TimeSpan FromSeconds(const uint64_t seconds)
  {
    if (seconds > (std::numeric_limits<uint64_t>::max() / TimeInfo::TicksPerSecond))
    {
      throw OverflowException("ticks overflow");
    }
    const uint64_t ticks = seconds * TimeInfo::TicksPerSecond;
    return TimeSpan(NumericCast<int64_t>(ticks));
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr int32_t ToMicrosecondsInt32(const TimeSpan transitionTime)
  {
    return NumericCast<int32_t>(transitionTime.Ticks() / TimeInfo::TicksPerMicrosecond);
  }

  inline constexpr uint64_t ToMicrosecondsUInt64(const TimeSpan transitionTime)
  {
    if (transitionTime.Ticks() < 0)
    {
      throw UnderflowException("ticks underflow");
    }
    const auto ticks = UncheckedNumericCast<uint64_t>(transitionTime.Ticks());
    return ticks / TimeInfo::TicksPerMicrosecond;
  }

  inline constexpr int64_t ToMicrosecondsInt64(const TimeSpan transitionTime) noexcept
  {
    return transitionTime.Ticks() / TimeInfo::TicksPerMicrosecond;
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr int32_t ToClampedMicrosecondsInt32(const TimeSpan transitionTime) noexcept
  {
    return UncheckedNumericCast<int32_t>(MathHelper::Clamp(transitionTime.Ticks() / TimeInfo::TicksPerMicrosecond,
                                                           UncheckedNumericCast<int64_t>(std::numeric_limits<int32_t>::min()),
                                                           UncheckedNumericCast<int64_t>(std::numeric_limits<int32_t>::max())));
  }

  inline constexpr uint64_t ToClampedMicrosecondsUInt64(const TimeSpan transitionTime) noexcept
  {
    const uint64_t ticks = transitionTime.Ticks() >= 0 ? UncheckedNumericCast<uint64_t>(transitionTime.Ticks()) : 0;
    return ticks / TimeInfo::TicksPerMicrosecond;
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr uint64_t ToMillisecondsUInt64(const TimeSpan transitionTime)
  {
    if (transitionTime.Ticks() < 0)
    {
      throw UnderflowException("ticks underflow");
    }
    const auto ticks = UncheckedNumericCast<uint64_t>(transitionTime.Ticks());
    return ticks / TimeInfo::TicksPerMillisecond;
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr uint64_t ToClampedMillisecondsUInt64(const TimeSpan transitionTime) noexcept
  {
    const uint64_t ticks = transitionTime.Ticks() >= 0 ? UncheckedNumericCast<uint64_t>(transitionTime.Ticks()) : 0;
    return ticks / TimeInfo::TicksPerMillisecond;
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr float ToMicrosecondsF(const TimeSpan transitionTime)
  {
    return static_cast<float>(static_cast<double>(transitionTime.Ticks()) / static_cast<double>(TimeInfo::TicksPerMicrosecond));
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr float ToMillisecondsF(const TimeSpan transitionTime)
  {
    return static_cast<float>(static_cast<double>(transitionTime.Ticks()) / static_cast<double>(TimeInfo::TicksPerMillisecond));
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr float ToSecondsF(const TimeSpan transitionTime)
  {
    return static_cast<float>(static_cast<double>(transitionTime.Ticks()) / static_cast<double>(TimeInfo::TicksPerSecond));
  }
}

#endif
