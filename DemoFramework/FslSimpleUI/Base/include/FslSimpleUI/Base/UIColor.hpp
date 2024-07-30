#ifndef FSLSIMPLEUI_BASE_UICOLOR_HPP
#define FSLSIMPLEUI_BASE_UICOLOR_HPP
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

#include <FslGraphics/Color.hpp>
#include <FslGraphics/ColorU16.hpp>
#include <FslSimpleUI/Base/UIColorSpace.hpp>

namespace Fsl::UI
{
  struct UIColor
  {
  private:
    ColorU16 m_value;
    UIColorSpace m_colorSpace{UIColorSpace::SRGBNonLinear};

  public:
    constexpr UIColor() noexcept = default;

    constexpr explicit UIColor(const PackedColor32 value) noexcept
      : m_value(value)
    {
    }

    constexpr explicit UIColor(const PackedColor64 value) noexcept
      : m_value(value)
    {
    }

    constexpr explicit UIColor(const Color value) noexcept
      : m_value(value.AsPackedColor32())
    {
    }

    constexpr explicit UIColor(const ColorU16 value) noexcept
      : m_value(value)

    {
    }

    constexpr UIColor(const PackedColor32 value, const UIColorSpace colorSpace) noexcept
      : m_value(value)
      , m_colorSpace(colorSpace)
    {
    }

    constexpr UIColor(const PackedColor64 value, const UIColorSpace colorSpace) noexcept
      : m_value(value)
      , m_colorSpace(colorSpace)
    {
    }

    constexpr UIColor(const Color value, const UIColorSpace colorSpace) noexcept
      : m_value(value.AsPackedColor32())
      , m_colorSpace(colorSpace)
    {
    }

    constexpr UIColor(const ColorU16 value, const UIColorSpace colorSpace) noexcept
      : m_value(value)
      , m_colorSpace(colorSpace)
    {
    }

    constexpr UIColor(const ColorChannelValueU8 r, const ColorChannelValueU8 g, const ColorChannelValueU8 b, const ColorChannelValueU8 a,
                      const UIColorSpace colorSpace) noexcept
      : m_value(r, g, b, a)
      , m_colorSpace(colorSpace)
    {
    }

    constexpr UIColor(const ColorChannelValueU16 r, const ColorChannelValueU16 g, const ColorChannelValueU16 b, const ColorChannelValueU16 a,
                      const UIColorSpace colorSpace) noexcept
      : m_value(r, g, b, a)
      , m_colorSpace(colorSpace)
    {
    }

    constexpr UIColor(const float r, const float g, const float b, const float a,
                      const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
      : m_value(ColorU16::Create(r, g, b, a))
      , m_colorSpace(colorSpace)
    {
    }

    constexpr ColorU16 GetValue() const noexcept
    {
      return m_value;
    }

    constexpr UIColorSpace GetColorSpace() const noexcept
    {
      return m_colorSpace;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr inline ColorChannelValueU16 R() const noexcept
    {
      return m_value.R();
    }

    constexpr inline ColorChannelValueU16 G() const noexcept
    {
      return m_value.G();
    }

    constexpr inline ColorChannelValueU16 B() const noexcept
    {
      return m_value.B();
    }

    constexpr inline ColorChannelValueU16 A() const noexcept
    {
      return m_value.A();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr inline uint16_t RawR() const noexcept
    {
      return m_value.RawR();
    }

    constexpr inline uint16_t RawG() const noexcept
    {
      return m_value.RawG();
    }

    constexpr inline uint16_t RawB() const noexcept
    {
      return m_value.RawB();
    }

    constexpr inline uint16_t RawA() const noexcept
    {
      return m_value.RawA();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr inline PackedColor32 AsPackedColor32() const noexcept
    {
      return m_value.AsPackedColor32();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    constexpr inline PackedColor64 AsPackedColor64() const noexcept
    {
      return m_value.AsPackedColor64();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIColor ClearA(const UIColor color) noexcept
    {
      return {ColorU16::ClearA(color.m_value), color.m_colorSpace};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIColor SetR(const UIColor color, const ColorChannelValueU16 value) noexcept
    {
      return {ColorU16::SetR(color.m_value, value), color.m_colorSpace};
    }

    static constexpr inline UIColor SetR(const UIColor color, const float value) noexcept
    {
      return {ColorU16::SetR(color.m_value, ColorChannelValueU16::Create(value)), color.m_colorSpace};
    }

    static constexpr inline UIColor SetG(const UIColor color, const ColorChannelValueU16 value) noexcept
    {
      return {ColorU16::SetG(color.m_value, value), color.m_colorSpace};
    }

    static constexpr inline UIColor SetG(const UIColor color, const float value) noexcept
    {
      return {ColorU16::SetG(color.m_value, ColorChannelValueU16::Create(value)), color.m_colorSpace};
    }

    static constexpr inline UIColor SetB(const UIColor color, const ColorChannelValueU16 value) noexcept
    {
      return {ColorU16::SetB(color.m_value, value), color.m_colorSpace};
    }

    static constexpr inline UIColor SetB(const UIColor color, const float value) noexcept
    {
      return {ColorU16::SetB(color.m_value, ColorChannelValueU16::Create(value)), color.m_colorSpace};
    }

    static constexpr inline UIColor SetA(const UIColor color, const ColorChannelValueU16 value) noexcept
    {
      return {ColorU16::SetA(color.m_value, value), color.m_colorSpace};
    }

    static constexpr inline UIColor SetA(const UIColor color, const float value) noexcept
    {
      return {ColorU16::SetA(color.m_value, ColorChannelValueU16::Create(value)), color.m_colorSpace};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIColor MultiplyRGB(const UIColor color, const float value) noexcept
    {
      return {ColorU16::MultiplyRGB(color.m_value, value), color.m_colorSpace};
    }

    static constexpr inline UIColor MultiplyA(const UIColor color, const float value) noexcept
    {
      return {ColorU16::MultiplyA(color.m_value, value), color.m_colorSpace};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIColor Create(const float r, const float g, const float b, const float a,
                                           const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
    {
      return {ColorU16::Create(r, g, b, a), colorSpace};
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr inline UIColor CreateR8G8B8A8(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a,
                                                   const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
    {
      return {ColorU16::CreateR8G8B8A8UNorm(r, g, b, a), colorSpace};
    }

    static constexpr inline UIColor CreateR8G8B8A8(const int16_t r, const int16_t g, const int16_t b, const int16_t a,
                                                   const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
    {
      return {ColorU16::CreateR8G8B8A8UNorm(r, g, b, a), colorSpace};
    }

    static constexpr inline UIColor CreateR8G8B8A8(const uint16_t r, const uint16_t g, const uint16_t b, const uint16_t a,
                                                   const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
    {
      return {ColorU16::CreateR8G8B8A8UNorm(r, g, b, a), colorSpace};
    }

    static constexpr inline UIColor CreateR8G8B8A8(const int32_t r, const int32_t g, const int32_t b, const int32_t a,
                                                   const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
    {
      return {ColorU16::CreateR8G8B8A8UNorm(r, g, b, a), colorSpace};
    }

    static constexpr inline UIColor CreateR8G8B8A8(const uint32_t r, const uint32_t g, const uint32_t b, const uint32_t a,
                                                   const UIColorSpace colorSpace = UIColorSpace::SRGBNonLinear) noexcept
    {
      return {ColorU16::CreateR8G8B8A8UNorm(r, g, b, a), colorSpace};
    }
  };

  // op==

  constexpr bool operator==(const UIColor lhs, const UIColor rhs) noexcept
  {
    return lhs.GetValue() == rhs.GetValue() && lhs.GetColorSpace() == rhs.GetColorSpace();
  }

  // op!=

  constexpr bool operator!=(const UIColor lhs, const UIColor rhs) noexcept
  {
    return !(lhs == rhs);
  }

}

#endif
