#ifndef FSLGRAPHICS_COLOR_HPP
#define FSLGRAPHICS_COLOR_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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
#include <FslBase/Math/MathHelper_Bilinear.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorChannelValueU16.hpp>
#include <FslGraphics/ColorChannelValueU8.hpp>
#include <FslGraphics/PackedColor32.hpp>
#include <FslGraphics/PackedColor64.hpp>
#include <cmath>

namespace Fsl
{
  struct Color
  {
  private:
    uint8_t m_r{0};
    uint8_t m_g{0};
    uint8_t m_b{0};
    uint8_t m_a{0};

  public:
    constexpr Color() noexcept = default;


    constexpr Color(const ColorChannelValueU8 r, const ColorChannelValueU8 g, const ColorChannelValueU8 b, const ColorChannelValueU8 a) noexcept
      : m_r(r.RawValue)
      , m_g(g.RawValue)
      , m_b(b.RawValue)
      , m_a(a.RawValue)
    {
    }

    constexpr Color(const ColorChannelValueU16 r, const ColorChannelValueU16 g, const ColorChannelValueU16 b, const ColorChannelValueU16 a) noexcept
      : Color(ColorChannelConverter::ToU8(r), ColorChannelConverter::ToU8(g), ColorChannelConverter::ToU8(b), ColorChannelConverter::ToU8(a))
    {
    }

    constexpr explicit Color(const PackedColor32 value) noexcept
      : m_r(value.RawR())
      , m_g(value.RawG())
      , m_b(value.RawB())
      , m_a(value.RawA())
    {
    }


    constexpr explicit Color(const uint32_t value) noexcept
      : m_r((value >> 16) & 0xFF)
      , m_g((value >> 8) & 0xFF)
      , m_b((value) & 0xFF)
      , m_a((value >> 24) & 0xFF)
    {
    }

    explicit constexpr Color(const Vector4& col) noexcept
      : m_r(ColorChannelConverter::RawF32ToRawU8(col.X))
      , m_g(ColorChannelConverter::RawF32ToRawU8(col.Y))
      , m_b(ColorChannelConverter::RawF32ToRawU8(col.Z))
      , m_a(ColorChannelConverter::RawF32ToRawU8(col.W))
    {
    }

    constexpr Color(const float r, const float g, const float b, const float a) noexcept
      : m_r(ColorChannelConverter::RawF32ToRawU8(r))
      , m_g(ColorChannelConverter::RawF32ToRawU8(g))
      , m_b(ColorChannelConverter::RawF32ToRawU8(b))
      , m_a(ColorChannelConverter::RawF32ToRawU8(a))
    {
    }

    constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
      : m_r(r)
      , m_g(g)
      , m_b(b)
      , m_a(a)
    {
    }

    constexpr Color(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
      : Color(ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a))
    {
    }

    constexpr Color(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
      : Color(ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a))
    {
    }


    constexpr bool IsOpaque() const noexcept
    {
      return m_a == 0xFF;
    }

    constexpr Vector4 ToVector4() const noexcept
    {
      return {static_cast<float>(m_r) / 255.0f, static_cast<float>(m_g) / 255.0f, static_cast<float>(m_b) / 255.0f, static_cast<float>(m_a) / 255.0f};
    }


    constexpr PackedColor32 AsPackedColor32() const noexcept
    {
      return PackedColor32::CreateR8G8B8A8UNorm(m_r, m_g, m_b, m_a);
    }

    //! get the packed value
    constexpr PackedColor64 AsPackedColor64() const noexcept
    {
      return {R(), G(), B(), A()};
    }

    //! @brief get the red component
    constexpr ColorChannelValueU8 R() const noexcept
    {
      return ColorChannelValueU8(m_r);
    }

    //! @brief get the green component
    constexpr ColorChannelValueU8 G() const noexcept
    {
      return ColorChannelValueU8(m_g);
    }

    //! @brief get the blue component
    constexpr ColorChannelValueU8 B() const noexcept
    {
      return ColorChannelValueU8(m_b);
    }

    //! @brief get the alpha component
    constexpr ColorChannelValueU8 A() const noexcept
    {
      return ColorChannelValueU8(m_a);
    }

    //! @brief get the red component
    constexpr uint8_t RawR() const noexcept
    {
      return m_r;
    }

    //! @brief get the green component
    constexpr uint8_t RawG() const noexcept
    {
      return m_g;
    }

    //! @brief get the blue component
    constexpr uint8_t RawB() const noexcept
    {
      return m_b;
    }

    //! @brief get the alpha component
    constexpr uint8_t RawA() const noexcept
    {
      return m_a;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Multiply the RGB value with the given float, the A value will be unmodified
    //! @param color the source color
    //! @param value the value to multiply RGB with. In the range of 0.0 to 1.0
    static constexpr Color MultiplyRGB(const Color color, const float value) noexcept
    {
      return UncheckedMultiplyRGB(color, MathHelper::Clamp(value, 0.0f, 1.0f));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Multiply the RGB value with the given float, the A value will be unmodified
    //! @param color the source color
    //! @param value the value to multiply RGB with. In the range of 0.0 to 1.0
    static constexpr Color UncheckedMultiplyRGB(const Color color, const float value) noexcept
    {
      assert(value >= 0.0f && value <= 1.0f);
      const constexpr uint32_t Shift = 32u;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const auto fixedValue = static_cast<uint64_t>(value * static_cast<double>(static_cast<uint64_t>(1) << Shift));
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const uint64_t r = ((color.RawR() * fixedValue) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedValue) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedValue) + Half) >> Shift;

      return {UncheckedNumericCast<uint8_t>(r), UncheckedNumericCast<uint8_t>(g), UncheckedNumericCast<uint8_t>(b), color.RawA()};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr Color MultiplyRGBA(const Color color, const float value) noexcept
    {
      return UncheckedMultiplyRGBA(color, MathHelper::Clamp(value, 0.0f, 1.0f));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr Color UncheckedMultiplyRGBA(const Color color, const float value) noexcept
    {
      assert(value >= 0.0f && value <= 1.0f);
      const constexpr uint32_t Shift = 32u;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const auto fixedValue = static_cast<uint64_t>(value * static_cast<double>(static_cast<uint64_t>(1) << Shift));
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const uint64_t r = ((color.RawR() * fixedValue) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedValue) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedValue) + Half) >> Shift;
      const uint64_t a = ((color.RawA() * fixedValue) + Half) >> Shift;

      return {UncheckedNumericCast<uint8_t>(r), UncheckedNumericCast<uint8_t>(g), UncheckedNumericCast<uint8_t>(b), UncheckedNumericCast<uint8_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr Color MultiplyA(const Color color, const float value) noexcept
    {
      return UncheckedMultiplyA(color, MathHelper::Clamp(value, 0.0f, 1.0f));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr Color UncheckedMultiplyA(const Color color, const float value) noexcept
    {
      assert(value >= 0.0f && value <= 1.0f);
      const constexpr uint32_t Shift = 32u;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const auto fixedValue = static_cast<uint64_t>(value * static_cast<double>(static_cast<uint64_t>(1) << Shift));
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const uint64_t a = ((color.RawA() * fixedValue) + Half) >> Shift;

      return {color.RawR(), color.RawG(), color.RawB(), UncheckedNumericCast<uint8_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color MultiplyA(const Color color, const ColorChannelValueU8 alpha) noexcept
    {
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const constexpr uint32_t Shift = 40u;
      const constexpr uint32_t MaxValue = 0xFF;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const uint64_t fixedA = (static_cast<uint64_t>(alpha.RawValue) << Shift) / MaxValue;
      const uint64_t a = ((color.RawA() * fixedA) + Half) >> Shift;
      return {color.RawR(), color.RawG(), color.RawB(), UncheckedNumericCast<uint8_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color Premultiply(const Color color) noexcept
    {
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const constexpr uint32_t Shift = 40u;
      const constexpr uint32_t MaxValue = 0xFF;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const uint64_t fixedA = (static_cast<uint64_t>(color.RawA()) << Shift) / MaxValue;
      const uint64_t r = ((color.RawR() * fixedA) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedA) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedA) + Half) >> Shift;
      return {UncheckedNumericCast<uint8_t>(r), UncheckedNumericCast<uint8_t>(g), UncheckedNumericCast<uint8_t>(b), color.RawA()};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color Premultiply(const Color color, const float alphaMul) noexcept
    {
      const constexpr uint32_t Shift = 32u;
      const constexpr uint32_t MaxValue = 0xFF;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const double constant = ((static_cast<double>(color.RawA()) / static_cast<float>(MaxValue)) * MathHelper::Clamp(alphaMul, 0.0f, 1.0f)) *
                              static_cast<double>(static_cast<uint64_t>(1) << Shift);
      assert(constant >= 0.0f);
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const auto fixedA = static_cast<uint64_t>(constant);
      const uint64_t r = ((color.RawR() * fixedA) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedA) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedA) + Half) >> Shift;
      const uint64_t a = ((MaxValue * fixedA) + Half) >> Shift;
      return {UncheckedNumericCast<uint8_t>(r), UncheckedNumericCast<uint8_t>(g), UncheckedNumericCast<uint8_t>(b), UncheckedNumericCast<uint8_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color PremultiplyRGB(const Color color, const float alphaMul, const ColorChannelValueU8 newA) noexcept
    {
      const constexpr uint32_t Shift = 32u;
      const constexpr uint32_t MaxValue = 0xFF;
      const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const double constant = ((static_cast<double>(color.RawA()) / static_cast<float>(MaxValue)) * MathHelper::Clamp(alphaMul, 0.0f, 1.0f)) *
                              static_cast<double>(static_cast<uint64_t>(1) << Shift);
      assert(constant >= 0.0f);
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const auto fixedA = static_cast<uint64_t>(constant);
      const uint64_t r = ((color.RawR() * fixedA) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedA) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedA) + Half) >> Shift;
      return {UncheckedNumericCast<uint8_t>(r), UncheckedNumericCast<uint8_t>(g), UncheckedNumericCast<uint8_t>(b), newA.RawValue};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color PremultiplyRGB(const Color color, const float alphaMul, const uint8_t newA) noexcept
    {
      return PremultiplyRGB(color, alphaMul, ColorChannelValueU8(newA));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------
    static constexpr inline Color ClearA(const Color color) noexcept
    {
      return {color.m_r, color.m_g, color.m_b, static_cast<uint8_t>(0u)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color SetR(const Color color, ColorChannelValueU8 value) noexcept
    {
      return {value.RawValue, color.m_g, color.m_b, color.m_a};
    }

    static constexpr inline Color SetG(const Color color, ColorChannelValueU8 value) noexcept
    {
      return {color.m_r, value.RawValue, color.m_b, color.m_a};
    }

    static constexpr inline Color SetB(const Color color, ColorChannelValueU8 value) noexcept
    {
      return {color.m_r, color.m_g, value.RawValue, color.m_a};
    }

    static constexpr inline Color SetA(const Color color, ColorChannelValueU8 value) noexcept
    {
      return {color.m_r, color.m_g, color.m_b, value.RawValue};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------


    static constexpr inline Color SetR(const Color color, uint8_t value) noexcept
    {
      return {value, color.m_g, color.m_b, color.m_a};
    }

    static constexpr inline Color SetG(const Color color, uint8_t value) noexcept
    {
      return {color.m_r, value, color.m_b, color.m_a};
    }

    static constexpr inline Color SetB(const Color color, uint8_t value) noexcept
    {
      return {color.m_r, color.m_g, value, color.m_a};
    }

    static constexpr inline Color SetA(const Color color, uint8_t value) noexcept
    {
      return {color.m_r, color.m_g, color.m_b, value};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline Color SetR(const Color color, float value) noexcept
    {
      return SetR(color, ColorChannelConverter::RawF32ToRawU8(value));
    }

    static constexpr inline Color SetG(const Color color, float value) noexcept
    {
      return SetG(color, ColorChannelConverter::RawF32ToRawU8(value));
    }

    static constexpr inline Color SetB(const Color color, float value) noexcept
    {
      return SetB(color, ColorChannelConverter::RawF32ToRawU8(value));
    }

    static constexpr inline Color SetA(const Color color, float value) noexcept
    {
      return SetA(color, ColorChannelConverter::RawF32ToRawU8(value));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Perform bilinear filtering
    static inline constexpr Color Bilinear(const Color val00, const Color val10, const Color val01, const Color val11, const float weightX,
                                           const float weightY)
    {
      return {
        MathHelper::Bilinear(static_cast<float>(val00.m_r) / 255.0f, static_cast<float>(val10.m_r) / 255.0f, static_cast<float>(val01.m_r) / 255.0f,
                             static_cast<float>(val11.m_r) / 255.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.m_g) / 255.0f, static_cast<float>(val10.m_g) / 255.0f, static_cast<float>(val01.m_g) / 255.0f,
                             static_cast<float>(val11.m_g) / 255.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.m_b) / 255.0f, static_cast<float>(val10.m_b) / 255.0f, static_cast<float>(val01.m_b) / 255.0f,
                             static_cast<float>(val11.m_b) / 255.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.m_a) / 255.0f, static_cast<float>(val10.m_a) / 255.0f, static_cast<float>(val01.m_a) / 255.0f,
                             static_cast<float>(val11.m_a) / 255.0f, weightX, weightY),
      };
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr Color CreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {ColorChannelValueU8(r), ColorChannelValueU8(g), ColorChannelValueU8(b), ColorChannelValueU8(a)};
    }

    static constexpr Color CreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    {
      return {ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a)};
    }

    static constexpr Color CreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    {
      return {ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a)};
    }

    static constexpr Color CreateR8G8B8A8UNorm(const int64_t r, const int64_t g, const int64_t b, const int64_t a)
    {
      return {ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a)};
    }

    static constexpr Color CreateR8G8B8A8UNorm(const uint64_t r, const uint64_t g, const uint64_t b, const uint64_t a)
    {
      return {ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr Color UncheckedCreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr Color UncheckedCreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      return {ColorChannelValueU8::UncheckedCreate(r), ColorChannelValueU8::UncheckedCreate(g), ColorChannelValueU8::UncheckedCreate(b),
              ColorChannelValueU8::UncheckedCreate(a)};
    }

    static constexpr Color UncheckedCreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      return {ColorChannelValueU8::UncheckedCreate(r), ColorChannelValueU8::UncheckedCreate(g), ColorChannelValueU8::UncheckedCreate(b),
              ColorChannelValueU8::UncheckedCreate(a)};
    }

    static constexpr Color UncheckedCreateR8G8B8A8UNorm(const int64_t r, const int64_t g, const int64_t b, const int64_t a) noexcept
    {
      return {ColorChannelValueU8::UncheckedCreate(r), ColorChannelValueU8::UncheckedCreate(g), ColorChannelValueU8::UncheckedCreate(b),
              ColorChannelValueU8::UncheckedCreate(a)};
    }

    static constexpr Color UncheckedCreateR8G8B8A8UNorm(const uint64_t r, const uint64_t g, const uint64_t b, const uint64_t a) noexcept
    {
      return {ColorChannelValueU8::UncheckedCreate(r), ColorChannelValueU8::UncheckedCreate(g), ColorChannelValueU8::UncheckedCreate(b),
              ColorChannelValueU8::UncheckedCreate(a)};
    }
  };

  //------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr bool operator==(const Color lhs, const Color rhs) noexcept
  {
    return lhs.AsPackedColor32() == rhs.AsPackedColor32();
  }

  inline constexpr bool operator!=(const Color lhs, const Color rhs) noexcept
  {
    return lhs.AsPackedColor32() != rhs.AsPackedColor32();
  }

  //------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr Color operator*(const Color lhs, const Color rhs) noexcept
  {
    const constexpr uint32_t MaxValue = 0xFF;
    const constexpr uint32_t Shift = 40;
    const constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

    const uint64_t fixedR = (static_cast<uint64_t>(rhs.RawR()) << Shift) / MaxValue;
    const uint64_t fixedG = (static_cast<uint64_t>(rhs.RawG()) << Shift) / MaxValue;
    const uint64_t fixedB = (static_cast<uint64_t>(rhs.RawB()) << Shift) / MaxValue;
    const uint64_t fixedA = (static_cast<uint64_t>(rhs.RawA()) << Shift) / MaxValue;

    // basically ((lhs.r * rhs.r) + 0.5f) / 2.0f
    const uint64_t r = ((lhs.RawR() * fixedR) + Half) >> Shift;
    const uint64_t g = ((lhs.RawG() * fixedG) + Half) >> Shift;
    const uint64_t b = ((lhs.RawB() * fixedB) + Half) >> Shift;
    const uint64_t a = ((lhs.RawA() * fixedA) + Half) >> Shift;
    return Color::UncheckedCreateR8G8B8A8UNorm(r, g, b, a);
  }
}

#endif
