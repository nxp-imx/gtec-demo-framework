#ifndef FSLBASE_TIME_MILLISECONDTICKCOUNT32_HPP
#define FSLBASE_TIME_MILLISECONDTICKCOUNT32_HPP
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
  struct MillisecondTickCount32
  {
  private:
    //! The number of ticks that represent the value of the current MillisecondTickCount32 structure.
    //! The smallest unit of time is the tick, which is equal to 1 millisecond
    //! We store this internally as a unsigned uint32_t even though this represents a signed tick count.
    //! This is done to ensure we get properly defined overflow and underflow behavior.
    uint32_t m_tickCount{0};

  public:
    //! The number of nanoseconds per tick
    static constexpr int32_t NanoSecondsPerTick = 1000000;
    //! The number of microseconds per tick
    static constexpr uint16_t MicrosecondPerTick = 1000;
    //! The number of ticks per millisecond
    static constexpr uint16_t TicksPerMillisecond = 1;
    //! The number of ticks per second
    static constexpr int64_t TicksPerSecond = TicksPerMillisecond * 1000;
    //! The number of ticks per minute
    static constexpr int64_t TicksPerMinute = TicksPerSecond * 60;
    //! The number of ticks per hour
    static constexpr int64_t TicksPerHour = TicksPerMinute * 60;
    //! The number of ticks per day
    static constexpr int64_t TicksPerDay = TicksPerHour * 24;


    static constexpr int32_t MinDays = std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerDay;
    static constexpr int32_t MaxDays = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerDay;
    static constexpr int32_t MinHours = std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerHour;
    static constexpr int32_t MaxHours = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerHour;
    static constexpr int32_t MinMinutes = std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerMinute;
    static constexpr int32_t MaxMinutes = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerMinute;
    static constexpr int32_t MinSeconds = std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerSecond;
    static constexpr int32_t MaxSeconds = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerSecond;
    static constexpr int32_t MinMilliseconds = std::numeric_limits<int32_t>::min() / MillisecondTickCount32::TicksPerMillisecond;
    static constexpr int32_t MaxMilliseconds = std::numeric_limits<int32_t>::max() / MillisecondTickCount32::TicksPerMillisecond;

    constexpr MillisecondTickCount32() noexcept = default;

    constexpr explicit MillisecondTickCount32(const int32_t tickCount) noexcept
      : m_tickCount(static_cast<uint32_t>(tickCount))
    {
    }

    //! Get the current tick count which is in milliseconds, do not confuse this with the TimeSpan.Ticks
    constexpr int32_t Ticks() const noexcept
    {
      return static_cast<int32_t>(m_tickCount);
    }

    constexpr uint32_t UnsignedTicks() const noexcept
    {
      return static_cast<uint32_t>(m_tickCount);
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


    constexpr double TotalNanoseconds() const noexcept
    {
      return static_cast<double>(Ticks()) * static_cast<double>(NanoSecondsPerTick);
    }

    constexpr double TotalMicroseconds() const noexcept
    {
      return static_cast<double>(Ticks()) * static_cast<double>(MicrosecondPerTick);
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


    constexpr MillisecondTickCount32& operator+=(const TimeSpan rhs) noexcept
    {
      static_assert(MillisecondTickCount32::TicksPerMillisecond == 1);
      const int64_t rhsAsMilliseconds = rhs.Ticks() / TimeSpan::TicksPerMillisecond;
      // Allow the add to wraparound the ticks counter
      m_tickCount = static_cast<uint32_t>(m_tickCount + static_cast<uint64_t>(rhsAsMilliseconds));
      return *this;
    }

    constexpr MillisecondTickCount32& operator-=(const TimeSpan rhs) noexcept
    {
      static_assert(MillisecondTickCount32::TicksPerMillisecond == 1);
      const int64_t lhsAsMilliseconds = Ticks();
      const int64_t rhsAsMilliseconds = rhs.Ticks() / TimeSpan::TicksPerMillisecond;
      const auto result32 = static_cast<int32_t>(lhsAsMilliseconds - rhsAsMilliseconds);
      m_tickCount = static_cast<uint32_t>(result32);
      return *this;
    }


    static inline constexpr MillisecondTickCount32 FromDays(const int32_t value)
    {
      if (value < MinDays)
      {
        throw UnderflowException("Days can not be negative");
      }
      if (value > MaxDays)
      {
        throw OverflowException("Days exceeded maximum allowed value");
      }
      return MillisecondTickCount32(UncheckedNumericCast<int32_t>(value * TicksPerDay));
    }

    static inline constexpr MillisecondTickCount32 FromHours(const int32_t value)
    {
      if (value < MinHours)
      {
        throw UnderflowException("Hours can not be negative");
      }
      if (value > MaxHours)
      {
        throw OverflowException("Hours exceeded maximum allowed value");
      }
      return MillisecondTickCount32(UncheckedNumericCast<int32_t>(value * TicksPerHour));
    }

    static inline constexpr MillisecondTickCount32 FromMinutes(const int32_t value)
    {
      if (value < MinMinutes)
      {
        throw UnderflowException("Minutes can not be negative");
      }
      if (value > MaxMinutes)
      {
        throw OverflowException("Minutes exceeded maximum allowed value");
      }
      return MillisecondTickCount32(UncheckedNumericCast<int32_t>(value * TicksPerMinute));
    }

    static inline constexpr MillisecondTickCount32 FromSeconds(const int32_t value)
    {
      if (value < MinSeconds)
      {
        throw UnderflowException("Seconds can not be negative");
      }
      if (value > MaxSeconds)
      {
        throw OverflowException("Seconds exceeded maximum allowed value");
      }
      return MillisecondTickCount32(UncheckedNumericCast<int32_t>(value * TicksPerSecond));
    }

    static inline constexpr MillisecondTickCount32 FromMilliseconds(const int32_t value) noexcept
    {
      static_assert(TicksPerMillisecond == 1u);
      return MillisecondTickCount32(value);
    }

    //! Warning using this will cause precision loss as MillisecondTickCount32 operates in milliseconds internally
    static inline constexpr MillisecondTickCount32 FromMicroseconds(const int32_t value) noexcept
    {
      return MillisecondTickCount32(value / MicrosecondPerTick);
    }

    //! Warning using this will cause precision loss as MillisecondTickCount32 operates in milliseconds internally
    static inline constexpr MillisecondTickCount32 FromNanoseconds(const int32_t value) noexcept
    {
      return MillisecondTickCount32(value / NanoSecondsPerTick);
    }
  };

  // Operator ==
  constexpr inline bool operator==(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    return lhs.Ticks() == rhs.Ticks();
  }

  // Operator !=
  constexpr inline bool operator!=(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    return lhs.Ticks() != rhs.Ticks();
  }

  // Operator <
  constexpr inline bool operator<(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    return lhs.Ticks() < rhs.Ticks();
  }

  // Operator <=
  constexpr inline bool operator<=(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    return lhs.Ticks() <= rhs.Ticks();
  }

  // Operator >
  constexpr inline bool operator>(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    return lhs.Ticks() > rhs.Ticks();
  }

  // Operator >=
  constexpr inline bool operator>=(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    return lhs.Ticks() >= rhs.Ticks();
  }

  // Timespan support

  constexpr inline MillisecondTickCount32 operator+(const MillisecondTickCount32 lhs, const TimeSpan rhs) noexcept
  {
    static_assert(MillisecondTickCount32::TicksPerMillisecond == 1);
    const int64_t rhsAsMilliseconds = rhs.Ticks() / TimeSpan::TicksPerMillisecond;
    // Allow the add to wraparound the ticks counter
    return MillisecondTickCount32(static_cast<int32_t>(lhs.UnsignedTicks() + static_cast<uint64_t>(rhsAsMilliseconds)));
  }

  constexpr inline MillisecondTickCount32 operator-(const MillisecondTickCount32 lhs, const TimeSpan rhs) noexcept
  {
    static_assert(MillisecondTickCount32::TicksPerMillisecond == 1);
    const int64_t lhsAsMilliseconds = lhs.Ticks();
    const int64_t rhsAsMilliseconds = rhs.Ticks() / TimeSpan::TicksPerMillisecond;
    return MillisecondTickCount32(static_cast<int32_t>(lhsAsMilliseconds - rhsAsMilliseconds));
  }

  constexpr inline TimeSpan operator-(const MillisecondTickCount32 lhs, const MillisecondTickCount32 rhs) noexcept
  {
    static_assert(MillisecondTickCount32::TicksPerMillisecond == 1);

    const auto result = static_cast<int32_t>(lhs.UnsignedTicks() - rhs.UnsignedTicks());
    // Regular subtraction
    return TimeSpan(static_cast<int64_t>(result) * TimeSpan::TicksPerMillisecond);
  }
}

#endif
