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

#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslGraphics/Bitmap/UncheckedRawBitmapTransformer.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverterFunctions.hpp>
#include <half.hpp>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl::FslGraphics2D::RawBitmapConverterFunctions
{
  namespace
  {
    // Conversion based on http://entropymine.com/imageworsener/srgbformula/

    //! This returns a value in the range of 0f to 1f.
    float ConvertSRGBToLinearFloat(const float valueSRGB) noexcept
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
      auto res = std::pow((valueSRGB + 0.055f) / 1.055f, 2.4f);
      assert(res >= 0.0f && res <= 1.0f);
      return res;
    }

    float ConvertUInt8SRGBToLinearFloat(const uint8_t valueSRGB) noexcept
    {
      return ConvertSRGBToLinearFloat(static_cast<float>(valueSRGB) / 255.0f);
    }

    //! @brief Convert a UInt8 SRGB value to a linear UInt16 value
    //! @return Values will be in the range 0 to 0xFFFF
    uint16_t ConvertSRGBToLinearUInt16(const uint8_t valueSRGB)
    {
      const float colorLinear = ConvertSRGBToLinearFloat(static_cast<float>(valueSRGB) / 255.0f);

      const float expandedLinearValue = std::round(colorLinear * std::numeric_limits<uint16_t>::max());
      assert(expandedLinearValue >= 0.0f && expandedLinearValue <= static_cast<float>(std::numeric_limits<uint32_t>::max()));
      const auto convertedLinearValue = static_cast<uint32_t>(expandedLinearValue);

      return convertedLinearValue <= std::numeric_limits<uint16_t>::max() ? static_cast<uint16_t>(convertedLinearValue)
                                                                          : std::numeric_limits<uint16_t>::max();
    }

    uint16_t ConvertSRGBToLinearFP16AsUInt16(const uint8_t srgbValue)
    {
      const float linearValue = ConvertSRGBToLinearFloat(static_cast<float>(srgbValue) / 255.0f);
      return half_float::detail::float2half<std::float_round_style::round_to_nearest>(linearValue);
    }

    //! This returns a value in the range of 0f to 1f.
    float ConvertLinearToSRGBFloat(const float valueLinear) noexcept
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
      auto res = std::pow(valueLinear, 1.0f / 2.4f) * 1.055f - 0.055f;
      assert(res >= 0.0f && res <= 1.0f);
      return res;
    }

    uint16_t ConvertLinearFloatToLinearFP16(const float valueLinear)
    {
      return half_float::detail::float2half<std::float_round_style::round_to_nearest>(valueLinear);
    }


    float ConvertLinearFP16ToLinearFloat(const uint16_t valueLinear)
    {
      return half_float::detail::half2float<float>(valueLinear);
    }


    uint8_t ConvertLinearFloatToSRGBUInt8(const float valueLinear) noexcept
    {
      const float colorSRGB = ConvertLinearToSRGBFloat(valueLinear);
      const float expandedSRGBValue = std::round(colorSRGB * std::numeric_limits<uint8_t>::max());
      assert(expandedSRGBValue >= 0.0f && expandedSRGBValue <= static_cast<float>(std::numeric_limits<uint32_t>::max()));
      const auto convertedSRGBValue = static_cast<uint32_t>(expandedSRGBValue);
      return convertedSRGBValue <= std::numeric_limits<uint8_t>::max() ? static_cast<uint8_t>(convertedSRGBValue)
                                                                       : std::numeric_limits<uint8_t>::max();
    }


    uint8_t ConvertLinearFP16ToSRGBUInt8(const uint16_t valueLinear)
    {
      return ConvertLinearFloatToSRGBUInt8(half_float::detail::half2float<float>(valueLinear));
    }

    uint8_t ConvertLinearUInt16ToSRGBUInt8(const uint16_t valueLinear) noexcept
    {
      return ConvertLinearFloatToSRGBUInt8(static_cast<float>(valueLinear) / static_cast<float>(std::numeric_limits<uint16_t>::max()));
    }

    uint16_t ConvertLinearUInt8ToLinearUInt16(const uint8_t valueLinear) noexcept
    {
      // 65535 / 255 = 257
      return valueLinear * 257;
    }

    uint16_t ConvertLinearUInt8ToLinearFP16(const uint8_t valueLinear)
    {
      return ConvertLinearFloatToLinearFP16(static_cast<float>(valueLinear) / 255.0f);
    }

    constexpr float ConvertLinearUInt8ToLinearFloat(const uint8_t valueLinear) noexcept
    {
      return static_cast<float>(valueLinear) / 255.0f;
    }

    constexpr uint8_t ConvertLinearUInt16ToLinearUInt8(const uint16_t valueLinear) noexcept
    {
      return valueLinear / 257;
    }

    uint8_t ConvertLinearFloatToLinearUInt8(const float valueLinear) noexcept
    {
      auto converted = static_cast<int32_t>(std::round(MathHelper::Clamp(valueLinear, 0.0f, 1.0f) * std::numeric_limits<uint8_t>::max()));
      return static_cast<uint8_t>(MathHelper::Clamp(converted, 0, 255));
    }

    uint8_t ConvertLinearFP16ToLinearUInt8(const uint16_t valueLinear) noexcept
    {
      return ConvertLinearFloatToLinearUInt8(ConvertLinearFP16ToLinearFloat(valueLinear));
    }
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  bool DoesMemoryRegionOverlap(const RawBitmapEx& dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    return UncheckedRawBitmapTransformer::DoesMemoryRegionOverlap(dstBitmap, srcBitmap);
  }


  bool IsSafeInplaceModificationOrNoMemoryOverlap(const RawBitmapEx& dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    return UncheckedRawBitmapTransformer::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap);
  }


  void UncheckedR8G8B8SrgbToR16G16B16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint16_t, PixelFormat::R16G16B16_UNORM, uint8_t, PixelFormat::R8G8B8_SRGB>(
      dstBitmap, srcBitmap, ConvertSRGBToLinearUInt16);
  }


  void UncheckedR8G8B8A8SrgbToR16G16B16A16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<uint16_t, PixelFormat::R16G16B16A16_UNORM, uint8_t,
                                                                         PixelFormat::R8G8B8A8_SRGB>(dstBitmap, srcBitmap, ConvertSRGBToLinearUInt16,
                                                                                                     ConvertLinearUInt8ToLinearUInt16);
  }


  void UncheckedR8G8B8SrgbToR16G16B16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint16_t, PixelFormat::R16G16B16_SFLOAT, uint8_t, PixelFormat::R8G8B8_SRGB>(
      dstBitmap, srcBitmap, ConvertSRGBToLinearFP16AsUInt16);
  }

  void UncheckedR8G8B8A8SrgbToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<uint16_t, PixelFormat::R16G16B16A16_SFLOAT, uint8_t,
                                                                         PixelFormat::R8G8B8A8_SRGB>(
      dstBitmap, srcBitmap, ConvertSRGBToLinearFP16AsUInt16, ConvertLinearUInt8ToLinearFP16);
  }

  void UncheckedR8G8B8SrgbToR32G32B32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<float, PixelFormat::R32G32B32_SFLOAT, uint8_t, PixelFormat::R8G8B8_SRGB>(
      dstBitmap, srcBitmap, ConvertUInt8SRGBToLinearFloat);
  }

  void UncheckedR8G8B8A8SrgbToR32G32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<float, PixelFormat::R32G32B32A32_SFLOAT, uint8_t,
                                                                         PixelFormat::R8G8B8A8_SRGB>(
      dstBitmap, srcBitmap, ConvertUInt8SRGBToLinearFloat, ConvertLinearUInt8ToLinearFloat);
  }

  // Linear to SRGB (clamp)

  void UncheckedR16G16B16UNormToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint8_t, PixelFormat::R8G8B8_SRGB, uint16_t, PixelFormat::R16G16B16_UNORM>(
      dstBitmap, srcBitmap, ConvertLinearUInt16ToSRGBUInt8);
  }

  void UncheckedR16G16B16A16UNormToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<uint8_t, PixelFormat::R8G8B8A8_SRGB, uint16_t,
                                                                         PixelFormat::R16G16B16A16_UNORM>(
      dstBitmap, srcBitmap, ConvertLinearUInt16ToSRGBUInt8, ConvertLinearUInt16ToLinearUInt8);
  }

  void UncheckedR16G16B16FloatToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint8_t, PixelFormat::R8G8B8_SRGB, uint16_t, PixelFormat::R16G16B16_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFP16ToSRGBUInt8);
  }


  void UncheckedR16G16B16A16FloatToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<uint8_t, PixelFormat::R8G8B8A8_SRGB, uint16_t,
                                                                         PixelFormat::R16G16B16A16_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFP16ToSRGBUInt8, ConvertLinearFP16ToLinearUInt8);
  }


  void UncheckedR32G32B32FloatToR8G8B8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint8_t, PixelFormat::R8G8B8_SRGB, float, PixelFormat::R32G32B32_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFloatToSRGBUInt8);
  }


  void UncheckedR32G32B32A32FloatToR8G8B8A8Srgb(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap) noexcept
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<uint8_t, PixelFormat::R8G8B8A8_SRGB, float,
                                                                         PixelFormat::R32G32B32A32_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFloatToSRGBUInt8, ConvertLinearFloatToLinearUInt8);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR16G16B16FloatToR32G32B32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<float, PixelFormat::R32G32B32_SFLOAT, uint16_t, PixelFormat::R16G16B16_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFP16ToLinearFloat);
  }


  void UncheckedR16G16B16A16FloatToR32G32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformFourChannels<float, PixelFormat::R32G32B32A32_SFLOAT, uint16_t, PixelFormat::R16G16B16A16_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFP16ToLinearFloat);
  }


  void UncheckedR32G32B32FloatToR16G16B16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint16_t, PixelFormat::R16G16B16_SFLOAT, float, PixelFormat::R32G32B32_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFloatToLinearFP16);
  }


  void UncheckedR32G32B32A32FloatToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformFourChannels<uint16_t, PixelFormat::R16G16B16A16_SFLOAT, float, PixelFormat::R32G32B32A32_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFloatToLinearFP16);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR16G16B16UNormToR32G32B32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<float, PixelFormat::R32G32B32_SFLOAT, uint16_t, PixelFormat::R16G16B16_UNORM>(
      dstBitmap, srcBitmap, ColorChannelConverter::RawU16ToRawF32);
  }


  void UncheckedR16G16B16A16UNormToR32G32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformFourChannels<float, PixelFormat::R32G32B32A32_SFLOAT, uint16_t, PixelFormat::R16G16B16A16_UNORM>(
      dstBitmap, srcBitmap, ColorChannelConverter::RawU16ToRawF32);
  }

  void UncheckedR32G32B32FloatToR16G16B16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannels<uint16_t, PixelFormat::R16G16B16_UNORM, float, PixelFormat::R32G32B32_SFLOAT>(
      dstBitmap, srcBitmap, ColorChannelConverter::RawF32ToRawU16);
  }


  void UncheckedR32G32B32A32FloatToR16G16B16A16UNorm(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformFourChannels<uint16_t, PixelFormat::R16G16B16A16_UNORM, float, PixelFormat::R32G32B32A32_SFLOAT>(
      dstBitmap, srcBitmap, ColorChannelConverter::RawF32ToRawU16);
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // Special
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  void UncheckedR32G32B32FloatToR16G16B16A16Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap)
  {
    UncheckedRawBitmapTransformer::TransformThreeChannelsAndAddChannel<uint16_t, PixelFormat::R16G16B16A16_SFLOAT, float,
                                                                       PixelFormat::R32G32B32_SFLOAT>(
      dstBitmap, srcBitmap, ConvertLinearFloatToLinearFP16, ConvertLinearFloatToLinearFP16(1.0f));
  }
}
