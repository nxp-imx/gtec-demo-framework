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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/ColorSpaceConversion.hpp>
#include <FslSimpleUI/Base/UIColorConverter.hpp>

namespace Fsl::UI
{
  namespace
  {
    constexpr UIRenderColor ConvertToSRGBNonLinear(const UIColor color) noexcept
    {
      switch (color.GetColorSpace())
      {
      case UIColorSpace::SRGBNonLinear:
        return {color.R(), color.G(), color.B(), color.A()};
      case UIColorSpace::SRGBLinear:
        {
          const uint16_t r = ColorSpaceConversion::ConvertLinearToSRGBUInt16(color.RawR());
          const uint16_t g = ColorSpaceConversion::ConvertLinearToSRGBUInt16(color.RawG());
          const uint16_t b = ColorSpaceConversion::ConvertLinearToSRGBUInt16(color.RawB());
          const uint16_t a = color.RawA();    // alpha is linear
          return UIRenderColor(ColorU16::CreateR16G16B16A16UNorm(r, g, b, a));
        }
      }
      FSLLOG3_WARNING("Unsupported target color-space: {}", static_cast<uint32_t>(color.GetColorSpace()));
      return {color.R(), color.G(), color.B(), color.A()};
    }

    constexpr UIRenderColor ConvertToSRGBLinear(const UIColor color) noexcept
    {
      switch (color.GetColorSpace())
      {
      case UIColorSpace::SRGBNonLinear:
        {
          const uint16_t r = ColorSpaceConversion::ConvertSRGBToLinearUInt16(color.RawR());
          const uint16_t g = ColorSpaceConversion::ConvertSRGBToLinearUInt16(color.RawG());
          const uint16_t b = ColorSpaceConversion::ConvertSRGBToLinearUInt16(color.RawB());
          const uint16_t a = color.RawA();    // alpha is linear
          return UIRenderColor(ColorU16::CreateR16G16B16A16UNorm(r, g, b, a));
        }
      case UIColorSpace::SRGBLinear:
        return {color.R(), color.G(), color.B(), color.A()};
      }
      FSLLOG3_WARNING("Unsupported target color-space: {}", static_cast<uint32_t>(color.GetColorSpace()));
      return {color.R(), color.G(), color.B(), color.A()};
    }
  }

  UIColorConverter::UIColorConverter(const UIColorSpace colorSpace) noexcept
    : m_colorSpace(colorSpace)
  {
  }


  UIRenderColor UIColorConverter::Convert(const UIColor color) const noexcept
  {
    switch (m_colorSpace)
    {
    case UIColorSpace::SRGBNonLinear:
      return ConvertToSRGBNonLinear(color);
    case UIColorSpace::SRGBLinear:
      return ConvertToSRGBLinear(color);
    }
    FSLLOG3_WARNING("Unsupported target color-space: {}", static_cast<uint32_t>(m_colorSpace));
    return {color.R(), color.G(), color.B(), color.A()};
  }
}
