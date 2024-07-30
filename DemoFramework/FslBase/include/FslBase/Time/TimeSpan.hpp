#ifndef FSLBASE_TIME_TIMESPAN_HPP
#define FSLBASE_TIME_TIMESPAN_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022, 2024 NXP
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
#include <FslBase/NumericCast.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <cassert>
#include <limits>

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
    //! The number of nanoseconds per tick
    static constexpr uint16_t NanoSecondsPerTick = 100;
    //! The number of ticks per microsecond
    static constexpr uint16_t TicksPerMicrosecond = 10;
    //! The number of ticks per millisecond
    static constexpr uint16_t TicksPerMillisecond = 10000;
    //! The number of ticks per second
    static constexpr int64_t TicksPerSecond = TicksPerMillisecond * 1000;
    //! The number of ticks per minute
    static constexpr int64_t TicksPerMinute = TicksPerSecond * 60;
    //! The number of ticks per hour
    static constexpr int64_t TicksPerHour = TicksPerMinute * 60;
    //! The number of ticks per day
    static constexpr int64_t TicksPerDay = TicksPerHour * 24;


    static constexpr int32_t MinDays = NumericCast<int32_t>(std::numeric_limits<int64_t>::min() / TicksPerDay);
    static constexpr int32_t MaxDays = NumericCast<int32_t>(std::numeric_limits<int64_t>::max() / TicksPerDay);
    static constexpr int32_t MinHours = NumericCast<int32_t>(std::numeric_limits<int64_t>::min() / TicksPerHour);
    static constexpr int32_t MaxHours = NumericCast<int32_t>(std::numeric_limits<int64_t>::max() / TicksPerHour);
    static constexpr int64_t MinMinutes = std::numeric_limits<int64_t>::min() / TicksPerMinute;
    static constexpr int64_t MaxMinutes = std::numeric_limits<int64_t>::max() / TicksPerMinute;
    static constexpr int64_t MinSeconds = std::numeric_limits<int64_t>::min() / TicksPerSecond;
    static constexpr int64_t MaxSeconds = std::numeric_limits<int64_t>::max() / TicksPerSecond;
    static constexpr int64_t MinMilliseconds = std::numeric_limits<int64_t>::min() / TicksPerMillisecond;
    static constexpr int64_t MaxMilliseconds = std::numeric_limits<int64_t>::max() / TicksPerMillisecond;
    static constexpr int64_t MinMicroseconds = std::numeric_limits<int64_t>::min() / TicksPerMicrosecond;
    static constexpr int64_t MaxMicroseconds = std::numeric_limits<int64_t>::max() / TicksPerMicrosecond;

    constexpr TimeSpan() noexcept = default;

    constexpr explicit TimeSpan(const int64_t ticks) noexcept
      : m_ticks(ticks)
    {
    }

    constexpr explicit TimeSpan(const int32_t hours, const int32_t minutes, const int32_t seconds) noexcept
      : TimeSpan((hours * TicksPerHour) + (minutes * TicksPerMinute) + (seconds * TicksPerSecond))
    {
    }

    constexpr explicit TimeSpan(const int32_t days, const int32_t hours, const int32_t minutes, const int32_t seconds) noexcept
      : TimeSpan((days * TicksPerDay) + (hours * TicksPerHour) + (minutes * TicksPerMinute) + (seconds * TicksPerSecond))
    {
    }

    constexpr explicit TimeSpan(const int32_t days, const int32_t hours, const int32_t minutes, const int32_t seconds,
                                const int32_t milliseconds) noexcept
      : TimeSpan((days * TicksPerDay) + (hours * TicksPerHour) + (minutes * TicksPerMinute) + (seconds * TicksPerSecond) +
                 (milliseconds * TicksPerMillisecond))
    {
    }


    constexpr float DeltaTime() const
    {
      return static_cast<float>(static_cast<double>(m_ticks) / 10000000.0);
    }

    constexpr int64_t Ticks() const noexcept
    {
      return m_ticks;
    }

    constexpr int32_t Days() const noexcept
    {
      return UncheckedNumericCast<int32_t>(m_ticks / TicksPerDay);
    }

    constexpr int32_t Hours() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TicksPerHour) % 24);
    }

    constexpr int32_t Minutes() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TicksPerMinute) % 60);
    }

    constexpr int32_t Seconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TicksPerSecond) % 60);
    }

    constexpr int32_t Milliseconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TicksPerMillisecond) % 1000);
    }

    constexpr int32_t Microseconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((m_ticks / TicksPerMicrosecond) % 1000);
    }

    constexpr double TotalNanoseconds() const noexcept
    {
      return static_cast<double>(Ticks()) * static_cast<double>(NanoSecondsPerTick);
    }

    constexpr double TotalMicroseconds() const noexcept
    {
      return static_cast<double>(Ticks()) / static_cast<double>(TicksPerMicrosecond);
    }

    constexpr double TotalMilliseconds() const noexcept
    {
      return static_cast<double>(Ticks()) / static_cast<double>(TicksPerMillisecond);
    }

    constexpr double TotalSeconds() const noexcept
    {
      return static_cast<double>(Ticks()) / static_cast<double>(TicksPerSecond);
    }

    constexpr double TotalMinutes() const noexcept
    {
      return static_cast<double>(Ticks()) / static_cast<double>(TicksPerMinute);
    }

    constexpr double TotalHours() const noexcept
    {
      return static_cast<double>(Ticks()) / static_cast<double>(TicksPerHour);
    }

    constexpr double TotalDays() const noexcept
    {
      return static_cast<double>(Ticks()) / static_cast<double>(TicksPerDay);
    }


    constexpr TimeSpan operator-() const noexcept
    {
      return TimeSpan(-m_ticks);
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

    static inline constexpr TimeSpan FromDays(const int32_t value)
    {
      static_assert(std::numeric_limits<int32_t>::min() < MinDays);
      static_assert(std::numeric_limits<int32_t>::max() > MaxDays);

      if (value < MinDays)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxDays)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      return TimeSpan(value * TicksPerDay);
    }

    static inline constexpr TimeSpan FromHours(const int32_t value)
    {
      static_assert(std::numeric_limits<int32_t>::min() < MinHours);
      static_assert(std::numeric_limits<int32_t>::max() > MaxHours);

      if (value < MinHours)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxHours)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      return TimeSpan(value * TicksPerHour);
    }

    static inline constexpr TimeSpan FromMinutes(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinMinutes);
      static_assert(std::numeric_limits<int64_t>::max() > MaxMinutes);

      if (value < MinMinutes)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxMinutes)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      return TimeSpan(UncheckedNumericCast<int64_t>(value * TicksPerMinute));
    }

    static inline constexpr TimeSpan FromSeconds(const int32_t value)
    {
      return FromSeconds(UncheckedNumericCast<int64_t>(value));
    }

    static inline constexpr TimeSpan FromSeconds(const uint32_t value)
    {
      return FromSeconds(UncheckedNumericCast<int64_t>(value));
    }

    static inline constexpr TimeSpan FromSeconds(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinSeconds);
      static_assert(std::numeric_limits<int64_t>::max() > MaxSeconds);

      if (value < MinSeconds)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxSeconds)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      return TimeSpan(value * TicksPerSecond);
    }

    static inline constexpr TimeSpan FromSeconds(const double value)
    {
      if (value < MinSeconds)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxSeconds)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      double result = static_cast<double>(value) * static_cast<double>(TicksPerSecond);
      return TimeSpan(static_cast<int64_t>(result));
    }


    static inline constexpr TimeSpan FromMilliseconds(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinMilliseconds);
      static_assert(std::numeric_limits<int64_t>::max() > MaxMilliseconds);

      if (value < MinMilliseconds)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxMilliseconds)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      return TimeSpan(UncheckedNumericCast<int64_t>(value * TicksPerMillisecond));
    }

    static inline constexpr TimeSpan FromMicroseconds(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinMicroseconds);
      static_assert(std::numeric_limits<int64_t>::max() > MaxMicroseconds);

      if (value < MinMicroseconds)
      {
        throw UnderflowException("below minimum allowed value");
      }
      if (value > MaxMicroseconds)
      {
        throw OverflowException("exceeded maximum allowed value");
      }
      return TimeSpan(UncheckedNumericCast<int64_t>(value * TicksPerMicrosecond));
    }

    //! There will be a sligth precission loss when converting to the internal tick resolution
    static inline constexpr TimeSpan FromNanoseconds(const int64_t value) noexcept
    {
      return TimeSpan(value / NanoSecondsPerTick);
    }
  };

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
}

#endif
