#ifndef FSLBASE_TIME_TIMESPAN_HPP
#define FSLBASE_TIME_TIMESPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslBase/Time/TimeInfo.hpp>
#include <cassert>

namespace Fsl
{
  struct TimeSpan
  {
  private:
    //! The number of ticks that represent the value of the current TimeSpan structure.
    //! The smallest unit of time is the tick, which is equal to 100 nanoseconds or one ten-millionth of a second.
    //! There are 10,000 ticks in a millisecond. The value of the Ticks property can be negative or positive to represent
    //! a negative or positive time interval
    int64_t m_ticks{0};

  public:
    constexpr TimeSpan() noexcept = default;

    constexpr explicit TimeSpan(const int64_t ticks) noexcept
      : m_ticks(ticks)
    {
    }

    constexpr int64_t Ticks() const noexcept
    {
      return m_ticks;
    }

    constexpr int32_t Days() const noexcept
    {
      return UncheckedNumericCast<int32_t>(m_ticks / TimeInfo::TicksPerDay);
    }

    constexpr int32_t Hours() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TimeInfo::TicksPerHour) % 24);
    }

    constexpr int32_t Minutes() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TimeInfo::TicksPerMinute) % 60);
    }

    constexpr int32_t Seconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TimeInfo::TicksPerSecond) % 60);
    }

    constexpr int32_t Milliseconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TimeInfo::TicksPerMillisecond) % 1000);
    }

    constexpr int32_t Microseconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TimeInfo::TicksPerMicrosecond) % 1000);
    }

    constexpr TimeSpan& operator+=(const TimeSpan arg) noexcept
    {
      m_ticks += arg.m_ticks;
      return *this;
    }

    constexpr TimeSpan& operator-=(const TimeSpan arg) noexcept
    {
      m_ticks -= arg.m_ticks;
      return *this;
    }

    constexpr TimeSpan& operator*=(const int32_t arg) noexcept
    {
      m_ticks *= arg;
      return *this;
    }

    constexpr TimeSpan& operator/=(const int32_t arg) noexcept
    {
      m_ticks /= arg;
      return *this;
    }
  };

  constexpr inline TimeSpan operator+(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return TimeSpan(lhs.Ticks() + rhs.Ticks());
  }

  constexpr inline TimeSpan operator-(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return TimeSpan(lhs.Ticks() - rhs.Ticks());
  }

  constexpr inline TimeSpan operator*(const TimeSpan lhs, const int32_t rhs) noexcept
  {
    return TimeSpan(lhs.Ticks() * rhs);
  }

  constexpr inline TimeSpan operator*(const int32_t lhs, const TimeSpan rhs) noexcept
  {
    return TimeSpan(lhs * rhs.Ticks());
  }

  constexpr inline TimeSpan operator/(const TimeSpan lhs, const int32_t rhs) noexcept
  {
    return TimeSpan(lhs.Ticks() / rhs);
  }


  // Operator ==
  constexpr inline bool operator==(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return lhs.Ticks() == rhs.Ticks();
  }

  // Operator !=
  constexpr inline bool operator!=(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return lhs.Ticks() != rhs.Ticks();
  }

  // Operator <
  constexpr inline bool operator<(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return lhs.Ticks() < rhs.Ticks();
  }

  // Operator <=
  constexpr inline bool operator<=(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return lhs.Ticks() <= rhs.Ticks();
  }

  // Operator >
  constexpr inline bool operator>(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return lhs.Ticks() > rhs.Ticks();
  }

  // Operator >=
  constexpr inline bool operator>=(const TimeSpan lhs, const TimeSpan rhs) noexcept
  {
    return lhs.Ticks() >= rhs.Ticks();
  }
}

#endif
