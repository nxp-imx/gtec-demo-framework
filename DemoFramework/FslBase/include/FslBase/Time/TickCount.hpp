#ifndef FSLBASE_TIME_TICKCOUNT_HPP
#define FSLBASE_TIME_TICKCOUNT_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/NumericCast.hpp>
#include <FslBase/Time/TimeSpan.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <limits>

namespace Fsl
{
  struct TickCount
  {
  private:
    //! The number of ticks that represent the value of the current TimeSpan structure.
    //! The smallest unit of time is the tick, which is equal to 100 nanoseconds or one ten-millionth of a second.
    //! There are 10,000 ticks in a millisecond.
    //! We store this internally as a unsigned uint64_t even though this represents a signed tick count.
    //! This is done to ensure we get properly defined overflow and underflow behavior.
    uint64_t m_tickCount{0};

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

    static constexpr int32_t MinDays = NumericCast<int32_t>(std::numeric_limits<int64_t>::min() / TickCount::TicksPerDay);
    static constexpr int32_t MaxDays = NumericCast<int32_t>(std::numeric_limits<int64_t>::max() / TickCount::TicksPerDay);
    static constexpr int32_t MinHours = NumericCast<int32_t>(std::numeric_limits<int64_t>::min() / TickCount::TicksPerHour);
    static constexpr int32_t MaxHours = NumericCast<int32_t>(std::numeric_limits<int64_t>::max() / TickCount::TicksPerHour);
    static constexpr int64_t MinMinutes = std::numeric_limits<int64_t>::min() / TickCount::TicksPerMinute;
    static constexpr int64_t MaxMinutes = std::numeric_limits<int64_t>::max() / TickCount::TicksPerMinute;
    static constexpr int64_t MinSeconds = std::numeric_limits<int64_t>::min() / TickCount::TicksPerSecond;
    static constexpr int64_t MaxSeconds = std::numeric_limits<int64_t>::max() / TickCount::TicksPerSecond;
    static constexpr int64_t MinMilliseconds = std::numeric_limits<int64_t>::min() / TickCount::TicksPerMillisecond;
    static constexpr int64_t MaxMilliseconds = std::numeric_limits<int64_t>::max() / TickCount::TicksPerMillisecond;
    static constexpr int64_t MinMicroseconds = std::numeric_limits<int64_t>::min() / TickCount::TicksPerMicrosecond;
    static constexpr int64_t MaxMicroseconds = std::numeric_limits<int64_t>::max() / TickCount::TicksPerMicrosecond;

    constexpr TickCount() noexcept = default;

    constexpr explicit TickCount(const int64_t tickCount) noexcept
      : m_tickCount(static_cast<uint64_t>(tickCount))
    {
    }

    constexpr int64_t Ticks() const noexcept
    {
      return static_cast<int64_t>(m_tickCount);
    }

    constexpr uint64_t UnsignedTicks() const noexcept
    {
      return static_cast<uint64_t>(m_tickCount);
    }

    constexpr int32_t Days() const noexcept
    {
      return UncheckedNumericCast<int32_t>(Ticks() / TicksPerDay);
    }

    constexpr int32_t Hours() const noexcept
    {
      return UncheckedNumericCast<int32_t>((Ticks() / TicksPerHour) % 24);
    }

    constexpr int32_t Minutes() const noexcept
    {
      return UncheckedNumericCast<int32_t>((Ticks() / TicksPerMinute) % 60);
    }

    constexpr int32_t Seconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((Ticks() / TicksPerSecond) % 60);
    }

    constexpr int32_t Milliseconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((Ticks() / TicksPerMillisecond) % 1000);
    }

    constexpr int32_t Microseconds() const noexcept
    {
      return UncheckedNumericCast<int32_t>((Ticks() / TicksPerMicrosecond) % 1000);
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

    constexpr int64_t TotalMicrosecondsInt64() const noexcept
    {
      return Ticks() / TicksPerMicrosecond;
    }

    constexpr TickCount& operator+=(const TimeSpan rhs) noexcept
    {
      m_tickCount += static_cast<uint64_t>(rhs.Ticks());
      return *this;
    }

    constexpr TickCount& operator-=(const TimeSpan rhs) noexcept
    {
      m_tickCount -= static_cast<uint64_t>(rhs.Ticks());
      return *this;
    }


    static inline constexpr TickCount FromDays(const int32_t value)
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
      return TickCount(UncheckedNumericCast<int64_t>(value * TicksPerDay));
    }

    static inline constexpr TickCount FromHours(const int32_t value)
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
      return TickCount(UncheckedNumericCast<int64_t>(value * TicksPerHour));
    }

    static inline constexpr TickCount FromMinutes(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinMinutes);
      static_assert(std::numeric_limits<int64_t>::max() > MaxMinutes);

      if (value < MinMinutes)
      {
        throw UnderflowException("Minutes can not be negative");
      }
      if (value > MaxMinutes)
      {
        throw OverflowException("Minutes exceeded maximum allowed value");
      }
      return TickCount(UncheckedNumericCast<int64_t>(value * TicksPerMinute));
    }


    static inline constexpr TickCount FromSeconds(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinSeconds);
      static_assert(std::numeric_limits<int64_t>::max() > MaxSeconds);

      if (value < MinSeconds)
      {
        throw UnderflowException("Seconds can not be negative");
      }
      if (value > MaxSeconds)
      {
        throw OverflowException("Seconds exceeded maximum allowed value");
      }
      return TickCount(UncheckedNumericCast<int64_t>(value * TicksPerSecond));
    }


    static inline constexpr TickCount FromMilliseconds(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinMilliseconds);
      static_assert(std::numeric_limits<int64_t>::max() > MaxMilliseconds);

      if (value < MinMilliseconds)
      {
        throw UnderflowException("Milliseconds can not be negative");
      }
      if (value > MaxMilliseconds)
      {
        throw OverflowException("Milliseconds exceeded maximum allowed value");
      }
      return TickCount(UncheckedNumericCast<int64_t>(value * TicksPerMillisecond));
    }

    static inline constexpr TickCount FromMicroseconds(const int64_t value)
    {
      static_assert(std::numeric_limits<int64_t>::min() < MinMicroseconds);
      static_assert(std::numeric_limits<int64_t>::max() > MaxMicroseconds);

      if (value < MinMicroseconds)
      {
        throw UnderflowException("Microseconds can not be negative");
      }
      if (value > MaxMicroseconds)
      {
        throw OverflowException("Microseconds exceeded maximum allowed value");
      }
      return TickCount(UncheckedNumericCast<int64_t>(value * TicksPerMicrosecond));
    }

    //! There will be a sligth precission loss when converting to the internal tick resolution
    static inline constexpr TickCount FromNanoseconds(const int64_t value) noexcept
    {
      return TickCount(value / NanoSecondsPerTick);
    }
  };

  // Operator ==
  constexpr inline bool operator==(const TickCount lhs, const TickCount rhs) noexcept
  {
    return lhs.Ticks() == rhs.Ticks();
  }

  // Operator !=
  constexpr inline bool operator!=(const TickCount lhs, const TickCount rhs) noexcept
  {
    return lhs.Ticks() != rhs.Ticks();
  }

  // Operator <
  constexpr inline bool operator<(const TickCount lhs, const TickCount rhs) noexcept
  {
    return lhs.Ticks() < rhs.Ticks();
  }

  // Operator <=
  constexpr inline bool operator<=(const TickCount lhs, const TickCount rhs) noexcept
  {
    return lhs.Ticks() <= rhs.Ticks();
  }

  // Operator >
  constexpr inline bool operator>(const TickCount lhs, const TickCount rhs) noexcept
  {
    return lhs.Ticks() > rhs.Ticks();
  }

  // Operator >=
  constexpr inline bool operator>=(const TickCount lhs, const TickCount rhs) noexcept
  {
    return lhs.Ticks() >= rhs.Ticks();
  }

  // Timespan support

  constexpr inline TickCount operator+(const TickCount lhs, const TimeSpan rhs) noexcept
  {
    // Allow the add to wraparound the ticks counter
    return TickCount(static_cast<int64_t>(lhs.UnsignedTicks() + static_cast<uint64_t>(rhs.Ticks())));
  }

  constexpr inline TickCount operator-(const TickCount lhs, const TimeSpan rhs) noexcept
  {
    // Allow the add to wraparound the ticks counter
    return TickCount(static_cast<int64_t>(lhs.UnsignedTicks() - static_cast<uint64_t>(rhs.Ticks())));
  }

  constexpr inline TimeSpan operator-(const TickCount lhs, const TickCount rhs) noexcept
  {
    return TimeSpan(static_cast<int64_t>(lhs.UnsignedTicks() - rhs.UnsignedTicks()));
  }
}

#endif
