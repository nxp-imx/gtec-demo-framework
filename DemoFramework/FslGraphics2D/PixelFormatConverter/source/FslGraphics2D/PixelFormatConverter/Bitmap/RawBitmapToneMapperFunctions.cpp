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

#include <FslGraphics/Bitmap/UncheckedRawBitmapTransformer.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapToneMapperFunctions.hpp>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl::FslGraphics2D::RawBitmapToneMapperFunctions
{
  namespace
  {
    float KeepValue(const float value) noexcept
    {
      return value;
    }


    struct ToneMapperHable final
    {
      float Exposure;

      constexpr explicit ToneMapperHable(const float exposure) noexcept
        : Exposure(exposure)
      {
      }

      static inline constexpr float Uncharted2ToneMap(float x) noexcept
      {
        constexpr float A = 0.15f;
        constexpr float B = 0.50f;
        constexpr float C = 0.10f;
        constexpr float D = 0.20f;
        constexpr float E = 0.02f;
        constexpr float F = 0.30f;
        return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
      }

      constexpr float operator()(const float value) noexcept
      {
        // Exposure
        const float exposedValue = value * Exposure;

        const float exposureBias = 2.0f;
        const float curr = Uncharted2ToneMap(exposureBias * exposedValue);

        constexpr float W = 11.2f;
        constexpr float WhiteScale = 1.0f / Uncharted2ToneMap(W);
        return curr * WhiteScale;
      }
    };


    struct ToneMapperReinhard final
    {
      float Exposure;

      constexpr explicit ToneMapperReinhard(const float exposure) noexcept
        : Exposure(exposure)
      {
      }

      constexpr float operator()(const float value) noexcept
      {
        // Exposure
        const float exposedValue = value * Exposure;

        // Simple reinhard
        return exposedValue / (1.0f + exposedValue);
      }
    };

  }


  void UncheckedHableR32G32B32A32FloatToRG32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, const float exposure) noexcept
  {
    ToneMapperHable tonemapper(exposure);
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<float, PixelFormat::R32G32B32A32_SFLOAT, float,
                                                                         PixelFormat::R32G32B32A32_SFLOAT>(dstBitmap, srcBitmap, tonemapper,
                                                                                                           KeepValue);
  }

  void UncheckedReinhardR32G32B32A32FloatToRG32B32A32Float(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, const float exposure) noexcept
  {
    ToneMapperReinhard tonemapper(exposure);
    UncheckedRawBitmapTransformer::TransformThreeChannelsTransformFourth<float, PixelFormat::R32G32B32A32_SFLOAT, float,
                                                                         PixelFormat::R32G32B32A32_SFLOAT>(dstBitmap, srcBitmap, tonemapper,
                                                                                                           KeepValue);
  }
}
