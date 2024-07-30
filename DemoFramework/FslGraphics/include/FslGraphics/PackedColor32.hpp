#ifndef FSLGRAPHICS_PACKEDCOLOR32_HPP
#define FSLGRAPHICS_PACKEDCOLOR32_HPP
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
#include <FslBase/NumericCast.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorChannelValueU8.hpp>

namespace Fsl
{
  struct PackedColor32
  {
    // color channels are stored like this: AARRGGBB
    uint32_t RawValue{0};

    constexpr PackedColor32() noexcept = default;

    constexpr explicit PackedColor32(const uint32_t value) noexcept
      : RawValue(value)
    {
    }

    constexpr PackedColor32(const ColorChannelValueU8 r, const ColorChannelValueU8 g, const ColorChannelValueU8 b,
                            const ColorChannelValueU8 a) noexcept
      : RawValue((static_cast<uint32_t>(r.RawValue) << 16) | (static_cast<uint32_t>(g.RawValue) << 8) | (static_cast<uint32_t>(b.RawValue)) |
                 (static_cast<uint32_t>(a.RawValue) << 24))
    {
    }

    constexpr PackedColor32(const ColorChannelValueU16 r, const ColorChannelValueU16 g, const ColorChannelValueU16 b,
                            const ColorChannelValueU16 a) noexcept
      : PackedColor32(ColorChannelConverter::ToU8(r), ColorChannelConverter::ToU8(g), ColorChannelConverter::ToU8(b), ColorChannelConverter::ToU8(a))
    {
    }

  private:
    constexpr PackedColor32(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
      : RawValue((static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(b)) | (static_cast<uint32_t>(a) << 24))
    {
    }

  public:
    constexpr ColorChannelValueU8 R() const noexcept
    {
      return ColorChannelValueU8(static_cast<uint8_t>((RawValue >> 16) & 0xFF));
    }

    constexpr ColorChannelValueU8 G() const noexcept
    {
      return ColorChannelValueU8(static_cast<uint8_t>((RawValue >> 8) & 0xFF));
    }

    constexpr ColorChannelValueU8 B() const noexcept
    {
      return ColorChannelValueU8(static_cast<uint8_t>((RawValue) & 0xFF));
    }

    constexpr ColorChannelValueU8 A() const noexcept
    {
      return ColorChannelValueU8(static_cast<uint8_t>((RawValue >> 24) & 0xFF));
    }

    constexpr uint8_t RawR() const noexcept
    {
      return static_cast<uint8_t>((RawValue >> 16) & 0xFF);
    }

    constexpr uint8_t RawG() const noexcept
    {
      return static_cast<uint8_t>((RawValue >> 8) & 0xFF);
    }

    constexpr uint8_t RawB() const noexcept
    {
      return static_cast<uint8_t>((RawValue) & 0xFF);
    }

    constexpr uint8_t RawA() const noexcept
    {
      return static_cast<uint8_t>((RawValue >> 24) & 0xFF);
    }

    static constexpr PackedColor32 CreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr PackedColor32 CreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    {
      return {NumericCast<uint8_t>(r), NumericCast<uint8_t>(g), NumericCast<uint8_t>(b), NumericCast<uint8_t>(a)};
    }

    static constexpr PackedColor32 CreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    {
      return {NumericCast<uint8_t>(r), NumericCast<uint8_t>(g), NumericCast<uint8_t>(b), NumericCast<uint8_t>(a)};
    }

    static constexpr PackedColor32 CreateR16G16B16A16UNorm(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
    {
      return {ColorChannelValueU16(r), ColorChannelValueU16(g), ColorChannelValueU16(b), ColorChannelValueU16(a)};
    }

    static constexpr PackedColor32 CreateR16G16B16A16UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    {
      return {ColorChannelValueU16(NumericCast<uint16_t>(r)), ColorChannelValueU16(NumericCast<uint16_t>(g)),
              ColorChannelValueU16(NumericCast<uint16_t>(b)), ColorChannelValueU16(NumericCast<uint16_t>(a))};
    }

    static constexpr PackedColor32 CreateR16G16B16A16UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    {
      return {ColorChannelValueU16(NumericCast<uint16_t>(r)), ColorChannelValueU16(NumericCast<uint16_t>(g)),
              ColorChannelValueU16(NumericCast<uint16_t>(b)), ColorChannelValueU16(NumericCast<uint16_t>(a))};
    }

    static constexpr PackedColor32 Create(const uint32_t value) noexcept
    {
      return PackedColor32(value);
    }

    static constexpr PackedColor32 UncheckedCreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr PackedColor32 UncheckedCreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      assert(r >= 0 && r <= 255);
      assert(g >= 0 && g <= 255);
      assert(b >= 0 && b <= 255);
      assert(a >= 0 && a <= 255);
      return {static_cast<uint8_t>(r & 0xFF), static_cast<uint8_t>(g & 0xFF), static_cast<uint8_t>(b & 0xFF), static_cast<uint8_t>(a & 0xFF)};
    }

    static constexpr PackedColor32 UncheckedCreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      assert(r <= 255);
      assert(g <= 255);
      assert(b <= 255);
      assert(a <= 255);
      return {static_cast<uint8_t>(r & 0xFF), static_cast<uint8_t>(g & 0xFF), static_cast<uint8_t>(b & 0xFF), static_cast<uint8_t>(a & 0xFF)};
    }

    static constexpr PackedColor32 UncheckedCreateR16G16B16A16UNorm(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
    {
      return {ColorChannelConverter::FromRawU16ToU8(r), ColorChannelConverter::FromRawU16ToU8(g), ColorChannelConverter::FromRawU16ToU8(b),
              ColorChannelConverter::FromRawU16ToU8(a)};
    }

    static constexpr PackedColor32 UncheckedCreateR16G16B16A16UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      assert(r >= 0 && r <= 0xFFFF);
      assert(g >= 0 && g <= 0xFFFF);
      assert(b >= 0 && b <= 0xFFFF);
      assert(a >= 0 && a <= 0xFFFF);
      return {ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(r & 0xFFFF)),
              ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(g & 0xFFFF)),
              ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(b & 0xFFFF)),
              ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(a & 0xFFFF))};
    }

    static constexpr PackedColor32 UncheckedCreateR16G16B16A16UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      assert(r <= 0xFFFF);
      assert(g <= 0xFFFF);
      assert(b <= 0xFFFF);
      assert(a <= 0xFFFF);
      return {ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(r & 0xFFFF)),
              ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(g & 0xFFFF)),
              ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(b & 0xFFFF)),
              ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(a & 0xFFFF))};
    }

    static constexpr PackedColor32 UncheckedCreate(const uint32_t value) noexcept
    {
      return PackedColor32(value);
    }
  };


  constexpr bool operator==(const PackedColor32 lhs, const PackedColor32 rhs) noexcept
  {
    return lhs.RawValue == rhs.RawValue;
  }

  constexpr bool operator!=(const PackedColor32 lhs, const PackedColor32 rhs) noexcept
  {
    return lhs.RawValue != rhs.RawValue;
  }
}

#endif
