#ifndef FSLSIMPLEUI_RENDER_BASE_UIRENDERCOLOR_HPP
#define FSLSIMPLEUI_RENDER_BASE_UIRENDERCOLOR_HPP
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
#include <FslBase/Math/MathHelper_Bilinear.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Color.hpp>
#include <FslGraphics/ColorU16.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorChannelValueU16.hpp>
#include <FslGraphics/ColorChannelValueU8.hpp>
#include <FslGraphics/PackedColor32.hpp>
#include <FslGraphics/PackedColor64.hpp>
#include <cmath>

namespace Fsl::UI
{
  struct UIRenderColor
  {
  private:
    uint16_t m_r{0};
    uint16_t m_g{0};
    uint16_t m_b{0};
    uint16_t m_a{0};

    constexpr UIRenderColor(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
      : m_r(r)
      , m_g(g)
      , m_b(b)
      , m_a(a)
    {
    }

  public:
    constexpr UIRenderColor() noexcept = default;

    constexpr explicit UIRenderColor(const Color value) noexcept
      : UIRenderColor(value.R(), value.G(), value.B(), value.A())
    {
    }

    constexpr explicit UIRenderColor(const ColorU16 value) noexcept
      : m_r(value.RawR())
      , m_g(value.RawG())
      , m_b(value.RawB())
      , m_a(value.RawA())
    {
    }


    constexpr UIRenderColor(const ColorChannelValueU8 r, const ColorChannelValueU8 g, const ColorChannelValueU8 b, const ColorChannelValueU8 a) noexcept
      : UIRenderColor(ColorChannelConverter::ToU16(r), ColorChannelConverter::ToU16(g), ColorChannelConverter::ToU16(b), ColorChannelConverter::ToU16(a))
    {
    }

    constexpr UIRenderColor(const ColorChannelValueU16 r, const ColorChannelValueU16 g, const ColorChannelValueU16 b,
                       const ColorChannelValueU16 a) noexcept
      : m_r(r.RawValue)
      , m_g(g.RawValue)
      , m_b(b.RawValue)
      , m_a(a.RawValue)
    {
    }

    constexpr explicit UIRenderColor(const PackedColor32 value) noexcept
      : m_r(ColorChannelConverter::ToRawU16(value.R()))
      , m_g(ColorChannelConverter::ToRawU16(value.G()))
      , m_b(ColorChannelConverter::ToRawU16(value.B()))
      , m_a(ColorChannelConverter::ToRawU16(value.A()))
    {
    }

    constexpr explicit UIRenderColor(const PackedColor64 value) noexcept
      : m_r(value.RawR())
      , m_g(value.RawG())
      , m_b(value.RawB())
      , m_a(value.RawA())
    {
    }

    constexpr bool IsOpaque() const noexcept
    {
      return m_a == 0xFFFF;
    }

    constexpr Vector4 ToVector4() const noexcept
    {
      return {static_cast<float>(RawR()) / 65535.0f, static_cast<float>(RawG()) / 65535.0f, static_cast<float>(RawB()) / 65535.0f,
              static_cast<float>(RawA()) / 65535.0f};
    }


    //! get the packed value
    constexpr PackedColor32 AsPackedColor32() const noexcept
    {
      return {ColorChannelConverter::ToU8(R()), ColorChannelConverter::ToU8(G()), ColorChannelConverter::ToU8(B()), ColorChannelConverter::ToU8(A())};
    }

    //! get the packed value
    constexpr PackedColor64 AsPackedColor64() const noexcept
    {
      return {R(), G(), B(), A()};
    }

    //! @brief get the red component
    constexpr ColorChannelValueU16 R() const noexcept
    {
      return ColorChannelValueU16(m_r);
    }

    //! @brief get the green component
    constexpr ColorChannelValueU16 G() const noexcept
    {
      return ColorChannelValueU16(m_g);
    }

    //! @brief get the blue component
    constexpr ColorChannelValueU16 B() const noexcept
    {
      return ColorChannelValueU16(m_b);
    }

    //! @brief get the alpha component
    constexpr ColorChannelValueU16 A() const noexcept
    {
      return ColorChannelValueU16(m_a);
    }

    //! @brief get the red component
    constexpr uint16_t RawR() const noexcept
    {
      return m_r;
    }

    //! @brief get the green component
    constexpr uint16_t RawG() const noexcept
    {
      return m_g;
    }

    //! @brief get the blue component
    constexpr uint16_t RawB() const noexcept
    {
      return m_b;
    }

    //! @brief get the alpha component
    constexpr uint16_t RawA() const noexcept
    {
      return m_a;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Multiply the RGB value with the given float, the A value will be unmodified
    //! @param color the source color
    //! @param value the value to multiply RGB with. In the range of 0.0 to 1.0
    static constexpr UIRenderColor MultiplyRGB(UIRenderColor color, const float value) noexcept
    {
      return UncheckedMultiplyRGB(color, MathHelper::Clamp(value, 0.0f, 1.0f));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    //! @brief Multiply the RGB value with the given float, the A value will be unmodified
    //! @param color the source color
    //! @param value the value to multiply RGB with. In the range of 0.0 to 1.0
    static constexpr UIRenderColor UncheckedMultiplyRGB(UIRenderColor color, const float value) noexcept
    {
      assert(value >= 0.0f && value <= 1.0f);
      constexpr uint32_t Shift = 32u;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const auto fixedValue = static_cast<uint64_t>(value * static_cast<double>(static_cast<uint64_t>(1) << Shift));
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const uint64_t r = ((color.RawR() * fixedValue) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedValue) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedValue) + Half) >> Shift;

      return {UncheckedNumericCast<uint16_t>(r), UncheckedNumericCast<uint16_t>(g), UncheckedNumericCast<uint16_t>(b), color.RawA()};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor MultiplyRGBA(UIRenderColor color, float value) noexcept
    {
      return UncheckedMultiplyRGBA(color, MathHelper::Clamp(value, 0.0f, 1.0f));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor UncheckedMultiplyRGBA(UIRenderColor color, float value) noexcept
    {
      assert(value >= 0.0f && value <= 1.0f);
      constexpr uint32_t Shift = 32u;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const auto fixedValue = static_cast<uint64_t>(value * static_cast<double>(static_cast<uint64_t>(1) << Shift));
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const uint64_t r = ((color.RawR() * fixedValue) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedValue) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedValue) + Half) >> Shift;
      const uint64_t a = ((color.RawA() * fixedValue) + Half) >> Shift;

      return {UncheckedNumericCast<uint16_t>(r), UncheckedNumericCast<uint16_t>(g), UncheckedNumericCast<uint16_t>(b),
              UncheckedNumericCast<uint16_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor MultiplyA(const UIRenderColor color, const float value) noexcept
    {
      return UncheckedMultiplyA(color, MathHelper::Clamp(value, 0.0f, 1.0f));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor UncheckedMultiplyA(const UIRenderColor color, const float value) noexcept
    {
      assert(value >= 0.0f && value <= 1.0f);
      constexpr uint32_t Shift = 32u;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const auto fixedValue = static_cast<uint64_t>(value * static_cast<double>(static_cast<uint64_t>(1) << Shift));
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const uint64_t a = ((color.RawA() * fixedValue) + Half) >> Shift;

      return {color.RawR(), color.RawG(), color.RawB(), UncheckedNumericCast<uint16_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIRenderColor MultiplyA(const UIRenderColor color, const ColorChannelValueU16 alpha) noexcept
    {
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      constexpr uint32_t Shift = 40u;
      constexpr uint32_t MaxValue = 0xFFFF;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const uint64_t fixedA = (static_cast<uint64_t>(alpha.RawValue) << Shift) / MaxValue;
      const uint64_t a = ((color.RawA() * fixedA) + Half) >> Shift;
      return {color.RawR(), color.RawG(), color.RawB(), UncheckedNumericCast<uint16_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIRenderColor Premultiply(const UIRenderColor color) noexcept
    {
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      constexpr uint32_t Shift = 40u;
      constexpr uint32_t MaxValue = 0xFFFF;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const uint64_t fixedA = (static_cast<uint64_t>(color.RawA()) << Shift) / MaxValue;
      const uint64_t r = ((color.RawR() * fixedA) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedA) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedA) + Half) >> Shift;
      return {UncheckedNumericCast<uint16_t>(r), UncheckedNumericCast<uint16_t>(g), UncheckedNumericCast<uint16_t>(b), color.RawA()};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIRenderColor Premultiply(const UIRenderColor color, const float alphaMul) noexcept
    {
      constexpr uint32_t Shift = 32u;
      constexpr uint32_t MaxValue = 0xFFFF;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

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
      return {UncheckedNumericCast<uint16_t>(r), UncheckedNumericCast<uint16_t>(g), UncheckedNumericCast<uint16_t>(b),
              UncheckedNumericCast<uint16_t>(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIRenderColor PremultiplyRGB(const UIRenderColor color, const float alphaMul, const ColorChannelValueU16 newA) noexcept
    {
      constexpr uint32_t Shift = 32u;
      constexpr uint32_t MaxValue = 0xFFFF;
      constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

      const double constant = ((static_cast<double>(color.RawA()) / static_cast<float>(MaxValue)) * MathHelper::Clamp(alphaMul, 0.0f, 1.0f)) *
                              static_cast<double>(static_cast<uint64_t>(1) << Shift);
      assert(constant >= 0.0f);
      // This is a high precision fixed point calculation (higher than float precision in most cases)
      // It is basically ((r * a) + 0.5) / 2.0
      const auto fixedA = static_cast<uint64_t>(constant);
      const uint64_t r = ((color.RawR() * fixedA) + Half) >> Shift;
      const uint64_t g = ((color.RawG() * fixedA) + Half) >> Shift;
      const uint64_t b = ((color.RawB() * fixedA) + Half) >> Shift;
      return {UncheckedNumericCast<uint16_t>(r), UncheckedNumericCast<uint16_t>(g), UncheckedNumericCast<uint16_t>(b), newA.RawValue};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIRenderColor ClearA(const UIRenderColor color) noexcept
    {
      return {color.m_r, color.m_g, color.m_b, static_cast<uint16_t>(0u)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIRenderColor SetR(const UIRenderColor color, const ColorChannelValueU16 value) noexcept
    {
      return {value.RawValue, color.m_g, color.m_b, color.m_a};
    }

    static constexpr inline UIRenderColor SetG(const UIRenderColor color, const ColorChannelValueU16 value) noexcept
    {
      return {color.m_r, value.RawValue, color.m_b, color.m_a};
    }

    static constexpr inline UIRenderColor SetB(const UIRenderColor color, const ColorChannelValueU16 value) noexcept
    {
      return {color.m_r, color.m_g, value.RawValue, color.m_a};
    }

    static constexpr inline UIRenderColor SetA(const UIRenderColor color, const ColorChannelValueU16 value) noexcept
    {
      return {color.m_r, color.m_g, color.m_b, value.RawValue};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static inline UIRenderColor Bilinear(const UIRenderColor val00, const UIRenderColor val10, const UIRenderColor val01, const UIRenderColor val11, const float weightX,
                                    const float weightY) noexcept
    {
      return UncheckedCreate(
        MathHelper::Bilinear(static_cast<float>(val00.RawR()) / 65535.0f, static_cast<float>(val10.RawR()) / 65535.0f,
                             static_cast<float>(val01.RawR()) / 65535.0f, static_cast<float>(val11.RawR()) / 65535.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.RawG()) / 65535.0f, static_cast<float>(val10.RawG()) / 65535.0f,
                             static_cast<float>(val01.RawG()) / 65535.0f, static_cast<float>(val11.RawG()) / 65535.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.RawB()) / 65535.0f, static_cast<float>(val10.RawB()) / 65535.0f,
                             static_cast<float>(val01.RawB()) / 65535.0f, static_cast<float>(val11.RawB()) / 65535.0f, weightX, weightY),
        MathHelper::Bilinear(static_cast<float>(val00.RawA()) / 65535.0f, static_cast<float>(val10.RawA()) / 65535.0f,
                             static_cast<float>(val01.RawA()) / 65535.0f, static_cast<float>(val11.RawA()) / 65535.0f, weightX, weightY));
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor Create(const Vector4& color)
    {
      return {ColorChannelConverter::RawF32ToRawU16(color.X), ColorChannelConverter::RawF32ToRawU16(color.Y),
              ColorChannelConverter::RawF32ToRawU16(color.Z), ColorChannelConverter::RawF32ToRawU16(color.W)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor Create(const float r, const float g, const float b, const float a)
    {
      return {ColorChannelConverter::RawF32ToRawU16(r), ColorChannelConverter::RawF32ToRawU16(g), ColorChannelConverter::RawF32ToRawU16(b),
              ColorChannelConverter::RawF32ToRawU16(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor CreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {ColorChannelValueU8(r), ColorChannelValueU8(g), ColorChannelValueU8(b), ColorChannelValueU8(a)};
    }

    static constexpr UIRenderColor CreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a)
    {
      return {ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a)};
    }

    static constexpr UIRenderColor CreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a)
    {
      return {ColorChannelValueU8::Create(r), ColorChannelValueU8::Create(g), ColorChannelValueU8::Create(b), ColorChannelValueU8::Create(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor CreateR16G16B16A16UNorm(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr UIRenderColor CreateR16G16B16A16UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      return {ColorChannelValueU16::Create(r), ColorChannelValueU16::Create(g), ColorChannelValueU16::Create(b), ColorChannelValueU16::Create(a)};
    }

    static constexpr UIRenderColor CreateR16G16B16A16UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      return {ColorChannelValueU16::Create(r), ColorChannelValueU16::Create(g), ColorChannelValueU16::Create(b), ColorChannelValueU16::Create(a)};
    }

    static constexpr UIRenderColor CreateR16G16B16A16UNorm(const int64_t r, const int64_t g, const int64_t b, const int64_t a) noexcept
    {
      return {ColorChannelValueU16::Create(r), ColorChannelValueU16::Create(g), ColorChannelValueU16::Create(b), ColorChannelValueU16::Create(a)};
    }

    static constexpr UIRenderColor CreateR16G16B16A16UNorm(const uint64_t r, const uint64_t g, const uint64_t b, const uint64_t a) noexcept
    {
      return {ColorChannelValueU16::Create(r), ColorChannelValueU16::Create(g), ColorChannelValueU16::Create(b), ColorChannelValueU16::Create(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor UncheckedCreate(const Vector4& color) noexcept
    {
      return {ColorChannelConverter::RawF32ToRawU16(color.X), ColorChannelConverter::RawF32ToRawU16(color.Y),
              ColorChannelConverter::RawF32ToRawU16(color.Z), ColorChannelConverter::RawF32ToRawU16(color.W)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor UncheckedCreate(const float r, const float g, const float b, const float a) noexcept
    {
      return {ColorChannelConverter::RawF32ToRawU16(r), ColorChannelConverter::RawF32ToRawU16(g), ColorChannelConverter::RawF32ToRawU16(b),
              ColorChannelConverter::RawF32ToRawU16(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor UncheckedCreateR8G8B8A8UNorm(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) noexcept
    {
      return {ColorChannelValueU8(r), ColorChannelValueU8(g), ColorChannelValueU8(b), ColorChannelValueU8(a)};
    }

    static constexpr UIRenderColor UncheckedCreateR8G8B8A8UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      return {ColorChannelValueU8::UncheckedCreate(r), ColorChannelValueU8::UncheckedCreate(g), ColorChannelValueU8::UncheckedCreate(b),
              ColorChannelValueU8::UncheckedCreate(a)};
    }

    static constexpr UIRenderColor UncheckedCreateR8G8B8A8UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      return {ColorChannelValueU8::UncheckedCreate(r), ColorChannelValueU8::UncheckedCreate(g), ColorChannelValueU8::UncheckedCreate(b),
              ColorChannelValueU8::UncheckedCreate(a)};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr UIRenderColor UncheckedCreateR16G16B16A16UNorm(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a) noexcept
    {
      return {r, g, b, a};
    }

    static constexpr UIRenderColor UncheckedCreateR16G16B16A16UNorm(const int32_t r, const int32_t g, const int32_t b, const int32_t a) noexcept
    {
      return {ColorChannelValueU16::UncheckedCreate(r), ColorChannelValueU16::UncheckedCreate(g), ColorChannelValueU16::UncheckedCreate(b),
              ColorChannelValueU16::UncheckedCreate(a)};
    }

    static constexpr UIRenderColor UncheckedCreateR16G16B16A16UNorm(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a) noexcept
    {
      return {ColorChannelValueU16::UncheckedCreate(r), ColorChannelValueU16::UncheckedCreate(g), ColorChannelValueU16::UncheckedCreate(b),
              ColorChannelValueU16::UncheckedCreate(a)};
    }

    static constexpr UIRenderColor UncheckedCreateR16G16B16A16UNorm(const int64_t r, const int64_t g, const int64_t b, const int64_t a) noexcept
    {
      return {ColorChannelValueU16::UncheckedCreate(r), ColorChannelValueU16::UncheckedCreate(g), ColorChannelValueU16::UncheckedCreate(b),
              ColorChannelValueU16::UncheckedCreate(a)};
    }

    static constexpr UIRenderColor UncheckedCreateR16G16B16A16UNorm(const uint64_t r, const uint64_t g, const uint64_t b, const uint64_t a) noexcept
    {
      return {ColorChannelValueU16::UncheckedCreate(r), ColorChannelValueU16::UncheckedCreate(g), ColorChannelValueU16::UncheckedCreate(b),
              ColorChannelValueU16::UncheckedCreate(a)};
    }
  };

  //------------------------------------------------------------------------------------------------------------------------------------------------

  constexpr bool operator==(const UIRenderColor lhs, const UIRenderColor rhs) noexcept
  {
    return lhs.AsPackedColor64() == rhs.AsPackedColor64();
  }

  constexpr bool operator!=(const UIRenderColor lhs, const UIRenderColor rhs) noexcept
  {
    return lhs.AsPackedColor64() != rhs.AsPackedColor64();
  }

  //------------------------------------------------------------------------------------------------------------------------------------------------

  inline constexpr UIRenderColor operator*(const UIRenderColor lhs, const UIRenderColor rhs) noexcept
  {
    constexpr uint32_t MaxValue = 0xFFFF;
    constexpr uint32_t Shift = 40;
    constexpr uint64_t Half = static_cast<uint64_t>(1) << (Shift - 1);

    const uint64_t fixedR = (static_cast<uint64_t>(rhs.RawR()) << Shift) / MaxValue;
    const uint64_t fixedG = (static_cast<uint64_t>(rhs.RawG()) << Shift) / MaxValue;
    const uint64_t fixedB = (static_cast<uint64_t>(rhs.RawB()) << Shift) / MaxValue;
    const uint64_t fixedA = (static_cast<uint64_t>(rhs.RawA()) << Shift) / MaxValue;

    // basically ((lhs.r * rhs.r) + 0.5f) / 2.0f
    const uint64_t r = ((lhs.RawR() * fixedR) + Half) >> Shift;
    const uint64_t g = ((lhs.RawG() * fixedG) + Half) >> Shift;
    const uint64_t b = ((lhs.RawB() * fixedB) + Half) >> Shift;
    const uint64_t a = ((lhs.RawA() * fixedA) + Half) >> Shift;
    return UIRenderColor::UncheckedCreateR16G16B16A16UNorm(r, g, b, a);
  }
}

#endif
