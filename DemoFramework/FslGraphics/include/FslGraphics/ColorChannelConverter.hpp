#ifndef FSLGRAPHICS_COLORCHANNELCONVERTER_HPP
#define FSLGRAPHICS_COLORCHANNELCONVERTER_HPP
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

#include <FslBase/UncheckedNumericCast.hpp>
#include <FslGraphics/ColorChannelValueU16.hpp>
#include <FslGraphics/ColorChannelValueU8.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::ColorChannelConverter
{
  inline constexpr uint16_t ExpandRawU8ToRawU16(const uint8_t value) noexcept
  {
    return static_cast<uint16_t>(value * 257);
  }

  inline constexpr uint8_t ReduceRawU16ToRawU8(const uint16_t value) noexcept
  {
    return static_cast<uint8_t>((((static_cast<uint32_t>(value) << 16) / 257u) + (static_cast<uint32_t>(1) << 15)) >> 16u);
  }


  inline constexpr ColorChannelValueU8 ReduceRawU16ToU8(const uint16_t value) noexcept
  {
    return ColorChannelValueU8(ReduceRawU16ToRawU8(value));
  }


  inline constexpr ColorChannelValueU16 ExpandRawU8ToU16(const uint8_t value) noexcept
  {
    return ColorChannelValueU16(static_cast<uint16_t>(value * 257));
  }


  inline constexpr ColorChannelValueU16 ToU16(const ColorChannelValueU8 channelValue) noexcept
  {
    return ExpandRawU8ToU16(channelValue.RawValue);
  }

  inline constexpr ColorChannelValueU8 ToU8(const ColorChannelValueU16 channelValue) noexcept
  {
    return ReduceRawU16ToU8(channelValue.RawValue);
  }

  inline constexpr uint16_t ToRawU16(const ColorChannelValueU8 channelValue) noexcept
  {
    return ExpandRawU8ToRawU16(channelValue.RawValue);
  }

  inline constexpr uint8_t ToRawU8(const ColorChannelValueU16 channelValue) noexcept
  {
    return ReduceRawU16ToRawU8(channelValue.RawValue);
  }

  inline constexpr ColorChannelValueU16 FromRawU8ToU16(const uint8_t rawValue) noexcept
  {
    return ExpandRawU8ToU16(rawValue);
  }

  inline constexpr ColorChannelValueU8 FromRawU16ToU8(const uint16_t rawValue) noexcept
  {
    return ReduceRawU16ToU8(rawValue);
  }


  inline constexpr uint16_t UncheckedRawF32ToRawU16(const float value) noexcept
  {
    assert(value >= 0.0f && value <= 1.0f);
    auto result = static_cast<int32_t>(value * static_cast<float>(0xFFFFu << 2));
    result += 1u << 1;
    return UncheckedNumericCast<uint16_t>(result >> 2);
  }

  inline constexpr uint8_t UncheckedRawF32ToRawU8(const float value) noexcept
  {
    assert(value >= 0.0f && value <= 1.0f);
    auto result = static_cast<int32_t>(value * static_cast<float>(0xFFu << 2));
    result += 1u << 1;
    return UncheckedNumericCast<uint8_t>(result >> 2);
  }

  inline constexpr uint16_t RawF32ToRawU16(const float value) noexcept
  {
    return UncheckedRawF32ToRawU16(MathHelper::Clamp(value, 0.0f, 1.0f));
  }

  inline constexpr uint8_t RawF32ToRawU8(const float value) noexcept
  {
    return UncheckedRawF32ToRawU8(MathHelper::Clamp(value, 0.0f, 1.0f));
  }

  inline constexpr float RawU16ToRawF32(const uint16_t valueLinear) noexcept
  {
    return static_cast<float>(valueLinear) / static_cast<float>(0xFFFF);
  }

  inline constexpr float RawU8ToRawF32(const uint8_t valueLinear) noexcept
  {
    return static_cast<float>(valueLinear) / static_cast<float>(0xFF);
  }
}

#endif
