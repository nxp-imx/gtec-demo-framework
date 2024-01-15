/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslGraphics/ColorSpaceConversion.hpp>

// Conversion based on http://entropymine.com/imageworsener/srgbformula/

namespace Fsl::ColorSpaceConversion
{
  float ConvertSRGBToLinearFloat(const float valueSRGB)
  {
    if (valueSRGB <= 0.0f)
    {
      return 0.0f;
    }
    if (valueSRGB < 0.04045f)
    {
      return valueSRGB / 12.92f;
    }
    if (valueSRGB >= 1.0f)
    {
      return 1.0f;
    }
    return std::pow((valueSRGB + 0.055f) / 1.055f, 2.4f);
  }


  float ConvertLinearToSRGBFloat(const float valueLinear)
  {
    if (valueLinear <= 0.0f)
    {
      return 0.0f;
    }
    if (valueLinear < 0.0031308f)
    {
      return valueLinear * 12.92f;
    }
    if (valueLinear >= 1.0f)
    {
      return 1.0f;
    }
    return std::pow(valueLinear, 1.0f / 2.4f) * 1.055f - 0.055f;
  }


  float ConvertSRGBToLinearFloat(const uint8_t valueSRGB)
  {
    if (valueSRGB <= std::numeric_limits<uint8_t>::min())
    {
      return 0.0f;
    }
    if (valueSRGB >= std::numeric_limits<uint8_t>::max())
    {
      return 1.0f;
    }
    const float colorSRGB = static_cast<float>(valueSRGB) / 255.0f;
    return ConvertSRGBToLinearFloat(colorSRGB);
  }


  uint8_t ConvertLinearToSRGBUInt8(const float valueLinear)
  {
    if (valueLinear <= 0.0f)
    {
      return std::numeric_limits<uint8_t>::min();
    }

    if (valueLinear >= 1.0f)
    {
      return std::numeric_limits<uint8_t>::max();
    }

    const float colorSRGB = ConvertLinearToSRGBFloat(valueLinear);
    const float expandedSRGBValue = std::round(colorSRGB * std::numeric_limits<uint8_t>::max());
    assert(expandedSRGBValue >= 0.0f);
    const auto convertedSRGBValue = static_cast<uint32_t>(expandedSRGBValue);
    return convertedSRGBValue < std::numeric_limits<uint8_t>::max() ? static_cast<uint8_t>(convertedSRGBValue) : std::numeric_limits<uint8_t>::max();
  }


  //! @brief Convert a UInt8 SRGB value to a linear UInt16 value
  //! @return Values will be in the range 0 to 0xFFFF
  uint16_t ConvertSRGBToLinearUInt16(const uint8_t valueSRGB)
  {
    const float colorSRGB = static_cast<float>(valueSRGB) / 255.0f;
    const float colorLinear = ConvertSRGBToLinearFloat(colorSRGB);
    if (colorLinear <= 0.0f)
    {
      return std::numeric_limits<uint16_t>::min();
    }
    if (colorLinear >= 1.0f)
    {
      return std::numeric_limits<uint16_t>::max();
    }
    const float expandedLinearValue = std::round(colorLinear * std::numeric_limits<uint16_t>::max());
    assert(expandedLinearValue >= 0.0f);
    const auto convertedLinearValue = static_cast<uint32_t>(expandedLinearValue);
    return convertedLinearValue < std::numeric_limits<uint16_t>::max() ? static_cast<uint16_t>(convertedLinearValue)
                                                                       : std::numeric_limits<uint16_t>::max();
  }


  uint8_t ConvertLinearToSRGBUInt8(const uint16_t colorLinear)
  {
    if (colorLinear <= std::numeric_limits<uint16_t>::min())
    {
      return std::numeric_limits<uint8_t>::min();
    }
    if (colorLinear >= std::numeric_limits<uint16_t>::max())
    {
      return std::numeric_limits<uint8_t>::max();
    }
    const float colorLinearFloat = static_cast<float>(colorLinear) / std::numeric_limits<uint16_t>::max();
    return ConvertLinearToSRGBUInt8(colorLinearFloat);
  }


  //! @brief Very lossy convert a SRGB value to a linear uint8, this will result in massive precision loss for darker colors!!
  uint8_t LossyConvertSRGBToLinearUInt8(const uint8_t valueSRGB)
  {
    const float colorLinear = ConvertSRGBToLinearFloat(valueSRGB);
    const float expandedLinearValue = std::round(colorLinear * std::numeric_limits<uint8_t>::max());
    assert(expandedLinearValue >= 0.0f);
    const auto convertedLinearValue = static_cast<uint32_t>(expandedLinearValue);
    return convertedLinearValue < std::numeric_limits<uint8_t>::max() ? static_cast<uint8_t>(convertedLinearValue)
                                                                      : std::numeric_limits<uint8_t>::max();
  }


  //! @brief Very lossy convert a SRGB value to a linear uint8
  //! @warning this will result in massive precision loss for darker colors!!
  Color LossyConvertSRGBToLinearColor(const Color colorSRGB)
  {
    return {LossyConvertSRGBToLinearUInt8(colorSRGB.R()), LossyConvertSRGBToLinearUInt8(colorSRGB.G()), LossyConvertSRGBToLinearUInt8(colorSRGB.B()),
            LossyConvertSRGBToLinearUInt8(colorSRGB.A())};
  }
}
