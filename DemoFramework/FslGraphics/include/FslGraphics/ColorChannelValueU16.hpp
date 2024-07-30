#ifndef FSLGRAPHICS_COLORCHANNELVALUEU16_HPP
#define FSLGRAPHICS_COLORCHANNELVALUEU16_HPP
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
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <cassert>

namespace Fsl
{
  struct ColorChannelValueU16
  {
    uint16_t RawValue{0};

    constexpr ColorChannelValueU16() noexcept = default;

    constexpr explicit ColorChannelValueU16(const uint16_t value) noexcept
      : RawValue(value)
    {
    }

    static constexpr ColorChannelValueU16 Min() noexcept
    {
      return ColorChannelValueU16(0);
    }

    static constexpr ColorChannelValueU16 Max() noexcept
    {
      return ColorChannelValueU16(0xFFFF);
    }

  private:
    static constexpr uint16_t ExpandToUInt16(const uint8_t value) noexcept
    {
      return static_cast<uint32_t>(value) * 257;
    }

    static constexpr uint8_t ReduceToUInt8(const uint32_t value) noexcept
    {
      return static_cast<uint8_t>(value / 257);
    }

    static constexpr inline uint16_t ToUInt16(const float value) noexcept
    {
      const auto asInt = static_cast<int32_t>(value * 65535.0f);
      return static_cast<uint16_t>(MathHelper::Clamp(asInt, 0, 65535));
    }

    static constexpr inline uint32_t ToUInt32(const float value) noexcept
    {
      const auto asInt = static_cast<int32_t>(value * 65535.0f);
      return static_cast<uint32_t>(MathHelper::Clamp(asInt, 0, 65535));
    }

  public:
    constexpr float AsFloat() const noexcept
    {
      return static_cast<float>(RawValue) / static_cast<float>(0xFFFF);
    }

    static constexpr ColorChannelValueU16 Create(const uint16_t value) noexcept
    {
      return ColorChannelValueU16(value);
    }

    static constexpr ColorChannelValueU16 Create(const int32_t value) noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>(MathHelper::Clamp(value, 0, 0xFFFF)));
    }

    static constexpr ColorChannelValueU16 Create(const uint32_t value) noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>(MathHelper::Clamp(value, 0u, 0xFFFFu)));
    }

    static constexpr ColorChannelValueU16 Create(const int64_t value) noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>(MathHelper::Clamp(value, static_cast<int64_t>(0), static_cast<int64_t>(0xFFFF))));
    }

    static constexpr ColorChannelValueU16 Create(const uint64_t value) noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>(MathHelper::Clamp(value, static_cast<uint64_t>(0), static_cast<uint64_t>(0xFFFF))));
    }

    static constexpr ColorChannelValueU16 Create(const float value) noexcept
    {
      return ColorChannelValueU16(ToUInt16(value));
    }

    static constexpr ColorChannelValueU16 UncheckedCreate(const uint16_t value) noexcept
    {
      return ColorChannelValueU16(value);
    }

    static constexpr ColorChannelValueU16 UncheckedCreate(const int32_t value) noexcept
    {
      assert(value >= 0 && value <= 0xFFFF);
      return ColorChannelValueU16(static_cast<uint16_t>(value & 0xFFFF));
    }

    static constexpr ColorChannelValueU16 UncheckedCreate(const uint32_t value) noexcept
    {
      assert(value <= 0xFFFF);
      return ColorChannelValueU16(static_cast<uint16_t>(value & 0xFFFF));
    }

    static constexpr ColorChannelValueU16 UncheckedCreate(const int64_t value) noexcept
    {
      assert(value >= 0 && value <= 0xFFFF);
      return ColorChannelValueU16(static_cast<uint16_t>(value & 0xFFFF));
    }

    static constexpr ColorChannelValueU16 UncheckedCreate(const uint64_t value) noexcept
    {
      assert(value <= 0xFFFF);
      return ColorChannelValueU16(static_cast<uint16_t>(value & 0xFFFF));
    }

    static constexpr ColorChannelValueU16 UncheckedCreate(const float value) noexcept
    {
      return ColorChannelValueU16(ToUInt16(value));
    }
  };


  constexpr bool operator==(const ColorChannelValueU16 lhs, const ColorChannelValueU16 rhs) noexcept
  {
    return lhs.RawValue == rhs.RawValue;
  }

  constexpr bool operator!=(const ColorChannelValueU16 lhs, const ColorChannelValueU16 rhs) noexcept
  {
    return lhs.RawValue != rhs.RawValue;
  }
}

#endif
