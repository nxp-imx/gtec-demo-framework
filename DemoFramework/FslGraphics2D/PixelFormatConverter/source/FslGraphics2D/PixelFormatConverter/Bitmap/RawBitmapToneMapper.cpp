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

#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Bitmap/RawBitmapUtil.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverterFunctions.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapToneMapper.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapToneMapperFunctions.hpp>
#include <array>

namespace Fsl::FslGraphics2D::RawBitmapToneMapper
{
  namespace
  {
    constexpr std::array<SupportedToneMapping, 2> Supported = {SupportedToneMapping(PixelFormat::R32G32B32A32_SFLOAT, BasicToneMapper::Hable),
                                                               SupportedToneMapping(PixelFormat::R32G32B32A32_SFLOAT, BasicToneMapper::Reinhard)};
  }


  ReadOnlySpan<SupportedToneMapping> GetSupportedToneMappings() noexcept
  {
    return SpanUtil::AsReadOnlySpan(Supported);
  }


  bool TryToneMap(RawBitmapEx dstBitmap, const ReadOnlyRawBitmap& srcBitmap, const BasicToneMapper toneMapper, const float exposure) noexcept
  {
    if (dstBitmap.GetOrigin() != srcBitmap.GetOrigin())
    {
      // We only support conversion between the same bitmap origins
      return false;
    }
    if (dstBitmap.GetSize() != srcBitmap.GetSize())
    {
      // We only support conversion between the same bitmap sizes
      return false;
    }
    if (!RawBitmapConverterFunctions::IsSafeInplaceModificationOrNoMemoryOverlap(dstBitmap, srcBitmap))
    {
      // We only support converting between bitmaps that obeys the above rules
      return false;
    }

    if (srcBitmap.GetPixelFormat() == PixelFormat::R32G32B32A32_SFLOAT && dstBitmap.GetPixelFormat() == PixelFormat::R32G32B32A32_SFLOAT)
    {
      switch (toneMapper)
      {
      case BasicToneMapper::Hable:
        RawBitmapToneMapperFunctions::UncheckedHableR32G32B32A32FloatToRG32B32A32Float(dstBitmap, srcBitmap, exposure);
        return true;
      case BasicToneMapper::Reinhard:
        RawBitmapToneMapperFunctions::UncheckedReinhardR32G32B32A32FloatToRG32B32A32Float(dstBitmap, srcBitmap, exposure);
        return true;
      default:
        break;
      }
    }
    return false;
  }
}
