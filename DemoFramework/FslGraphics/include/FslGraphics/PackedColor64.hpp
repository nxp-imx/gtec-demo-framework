#ifndef FSLGRAPHICS_PACKEDCOLOR64_HPP
#define FSLGRAPHICS_PACKEDCOLOR64_HPP
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
#include <FslGraphics/ColorChannelValueU16.hpp>

namespace Fsl
{
  struct PackedColor64
  {
    // Stored as AAAA RRRR GGGG BBBB
    uint64_t RawValue{0};

    constexpr PackedColor64() noexcept = default;

    constexpr explicit PackedColor64(const uint64_t value) noexcept
      : RawValue(value)
    {
    }

    constexpr PackedColor64(const ColorChannelValueU8 r, const ColorChannelValueU8 g, const ColorChannelValueU8 b,
                            const ColorChannelValueU8 a) noexcept
      : PackedColor64(ColorChannelConverter::ToU16(r), ColorChannelConverter::ToU16(g), ColorChannelConverter::ToU16(b),
                      ColorChannelConverter::ToU16(a))
    {
    }

    constexpr PackedColor64(const ColorChannelValueU16 r, const ColorChannelValueU16 g, const ColorChannelValueU16 b,
                            const ColorChannelValueU16 a) noexcept
      : RawValue((static_cast<uint64_t>(r.RawValue) << 32) | (static_cast<uint64_t>(g.RawValue) << 16) | (static_cast<uint64_t>(b.RawValue)) |
                 (static_cast<uint64_t>(a.RawValue) << 48))
    {
    }

  private:
    constexpr PackedColor64(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
      : RawValue((static_cast<uint64_t>(r) << 32) | (static_cast<uint64_t>(g) << 16) | (static_cast<uint64_t>(b)) | (static_cast<uint64_t>(a) << 48))
    {
    }

  public:
    constexpr ColorChannelValueU16 R() const noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>((RawValue >> 32) & 0xFFFF));
    }

    constexpr ColorChannelValueU16 G() const noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>((RawValue >> 16) & 0xFFFF));
    }

    constexpr ColorChannelValueU16 B() const noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>((RawValue) & 0xFFFF));
    }

    constexpr ColorChannelValueU16 A() const noexcept
    {
      return ColorChannelValueU16(static_cast<uint16_t>((RawValue >> 48) & 0xFFFF));
    }

    constexpr uint16_t RawR() const noexcept
    {
      return static_cast<uint16_t>((RawValue >> 32) & 0xFFFF);
    }

    constexpr uint16_t RawG() const noexcept
    {
      return static_cast<uint16_t>((RawValue >> 16) & 0xFFFF);
    }

    constexpr uint16_t RawB() const noexcept
    {
      return static_cast<uint16_t>(RawValue & 0xFFFF);
    }

    constexpr uint16_t RawA() const noexcept
    {
      return static_cast<uint16_t>((RawValue >> 48) & 0xFFFF);
    }


    static constexpr PackedColor64 CreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {ColorChannelConverter::FromRawU8ToU16(r), ColorChannelConverter::FromRawU8ToU16(g), ColorChannelConverter::FromRawU8ToU16(b),
              ColorChannelConverter::FromRawU8ToU16(a)};
    }

    static constexpr PackedColor64 CreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    {
      return {ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(r)), ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(g)),
              ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(b)), ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(a))};
    }

    static constexpr PackedColor64 CreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    {
      return {ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(r)), ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(g)),
              ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(b)), ColorChannelConverter::FromRawU8ToU16(NumericCast<uint8_t>(a))};
    }

    static constexpr PackedColor64 CreateR16G16B16A16UNorm(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr PackedColor64 CreateR16G16B16A16UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    {
      return {NumericCast<uint16_t>(r), NumericCast<uint16_t>(g), NumericCast<uint16_t>(b), NumericCast<uint16_t>(a)};
    }

    static constexpr PackedColor64 CreateR16G16B16A16UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    {
      return {NumericCast<uint16_t>(r), NumericCast<uint16_t>(g), NumericCast<uint16_t>(b), NumericCast<uint16_t>(a)};
    }

    static constexpr PackedColor64 Create(const uint64_t value) noexcept
    {
      return PackedColor64(value);
    }

    static constexpr PackedColor64 UncheckedCreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {ColorChannelConverter::FromRawU8ToU16(r), ColorChannelConverter::FromRawU8ToU16(g), ColorChannelConverter::FromRawU8ToU16(b),
              ColorChannelConverter::FromRawU8ToU16(a)};
    }

    static constexpr PackedColor64 UncheckedCreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      assert(r >= 0 && r <= 255);
      assert(g >= 0 && g <= 255);
      assert(b >= 0 && b <= 255);
      assert(a >= 0 && a <= 255);
      return {
        ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(r & 0xFF)), ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(g & 0xFF)),
        ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(b & 0xFF)), ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(a & 0xFF))};
    }

    static constexpr PackedColor64 UncheckedCreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      assert(r <= 255);
      assert(g <= 255);
      assert(b <= 255);
      assert(a <= 255);
      return {
        ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(r & 0xFF)), ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(g & 0xFF)),
        ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(b & 0xFF)), ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(a & 0xFF))};
    }


    static constexpr PackedColor64 UncheckedCreateR16G16B16A16UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr PackedColor64 UncheckedCreateR16G16B16A16UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      assert(r >= 0 && r <= 0xFFFF);
      assert(g >= 0 && g <= 0xFFFF);
      assert(b >= 0 && b <= 0xFFFF);
      assert(a >= 0 && a <= 0xFFFF);
      return {static_cast<uint16_t>(r & 0xFFFF), static_cast<uint16_t>(g & 0xFFFF), static_cast<uint16_t>(b & 0xFFFF),
              static_cast<uint16_t>(a & 0xFFFF)};
    }

    static constexpr PackedColor64 UncheckedCreateR16G16B16A16UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      assert(r <= 0xFFFF);
      assert(g <= 0xFFFF);
      assert(b <= 0xFFFF);
      assert(a <= 0xFFFF);
      return {static_cast<uint16_t>(r & 0xFFFF), static_cast<uint16_t>(g & 0xFFFF), static_cast<uint16_t>(b & 0xFFFF),
              static_cast<uint16_t>(a & 0xFFFF)};
    }

    static constexpr PackedColor64 UncheckedCreate(const uint64_t value) noexcept
    {
      return PackedColor64(value);
    }
  };


  constexpr bool operator==(const PackedColor64 lhs, const PackedColor64 rhs) noexcept
  {
    return lhs.RawValue == rhs.RawValue;
  }

  constexpr bool operator!=(const PackedColor64 lhs, const PackedColor64 rhs) noexcept
  {
    return lhs.RawValue != rhs.RawValue;
  }
}

#endif
