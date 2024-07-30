#ifndef FSLGRAPHICS_COLORCHANNELVALUEU8_HPP
#define FSLGRAPHICS_COLORCHANNELVALUEU8_HPP
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
  struct ColorChannelValueU8
  {
    uint8_t RawValue{0};

    constexpr ColorChannelValueU8() noexcept = default;

    constexpr explicit ColorChannelValueU8(const uint8_t value) noexcept
      : RawValue(value)
    {
    }


    static constexpr ColorChannelValueU8 Min() noexcept
    {
      return ColorChannelValueU8(0);
    }

    static constexpr ColorChannelValueU8 Max() noexcept
    {
      return ColorChannelValueU8(0xFF);
    }


  private:
    static constexpr inline uint8_t ToUInt8(const float value) noexcept
    {
      const auto asInt = static_cast<int32_t>(value * 255.0f);
      return static_cast<uint8_t>(MathHelper::Clamp(asInt, 0, 255));
    }

    static constexpr inline uint32_t ToUInt32(const float value) noexcept
    {
      const auto asInt = static_cast<int32_t>(value * 255.0f);
      return static_cast<uint32_t>(MathHelper::Clamp(asInt, 0, 255));
    }

  public:
    constexpr float AsFloat() const noexcept
    {
      return static_cast<float>(RawValue) / static_cast<float>(0xFF);
    }

    static constexpr ColorChannelValueU8 Create(const uint8_t value)
    {
      return ColorChannelValueU8(value);
    }

    static constexpr ColorChannelValueU8 Create(const int32_t value)
    {
      return ColorChannelValueU8(static_cast<uint8_t>(MathHelper::Clamp(value, static_cast<int32_t>(0), static_cast<int32_t>(0xFF))));
    }

    static constexpr ColorChannelValueU8 Create(const uint32_t value)
    {
      return ColorChannelValueU8(static_cast<uint8_t>(MathHelper::Clamp(value, static_cast<uint32_t>(0), static_cast<uint32_t>(0xFF))));
    }

    static constexpr ColorChannelValueU8 Create(const int64_t value)
    {
      return ColorChannelValueU8(static_cast<uint8_t>(MathHelper::Clamp(value, static_cast<int64_t>(0), static_cast<int64_t>(0xFF))));
    }

    static constexpr ColorChannelValueU8 Create(const uint64_t value)
    {
      return ColorChannelValueU8(static_cast<uint8_t>(MathHelper::Clamp(value, static_cast<uint64_t>(0), static_cast<uint64_t>(0xFF))));
    }

    static constexpr ColorChannelValueU8 Create(const float value)
    {
      return ColorChannelValueU8(ToUInt8(value));
    }

    static constexpr ColorChannelValueU8 UncheckedCreate(const uint8_t value) noexcept
    {
      return ColorChannelValueU8(value);
    }

    static constexpr ColorChannelValueU8 UncheckedCreate(const int32_t value) noexcept
    {
      assert(value >= 0 && value <= 0xFF);
      return ColorChannelValueU8(static_cast<uint8_t>(value & 0xFF));
    }

    static constexpr ColorChannelValueU8 UncheckedCreate(const uint32_t value) noexcept
    {
      assert(value <= 0xFF);
      return ColorChannelValueU8(static_cast<uint8_t>(value & 0xFF));
    }

    static constexpr ColorChannelValueU8 UncheckedCreate(const int64_t value) noexcept
    {
      assert(value >= 0 && value <= 0xFF);
      return ColorChannelValueU8(static_cast<uint8_t>(value & 0xFF));
    }

    static constexpr ColorChannelValueU8 UncheckedCreate(const uint64_t value) noexcept
    {
      assert(value <= 0xFF);
      return ColorChannelValueU8(static_cast<uint8_t>(value & 0xFF));
    }

    static constexpr ColorChannelValueU8 UncheckedCreate(const float value) noexcept
    {
      return ColorChannelValueU8(ToUInt8(value));
    }
  };


  constexpr bool operator==(const ColorChannelValueU8 lhs, const ColorChannelValueU8 rhs) noexcept
  {
    return lhs.RawValue == rhs.RawValue;
  }

  constexpr bool operator!=(const ColorChannelValueU8 lhs, const ColorChannelValueU8 rhs) noexcept
  {
    return lhs.RawValue != rhs.RawValue;
  }
}

#endif
