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

#include <FslDemoService/ImageConverter/HDR/ImageConverterLibraryHDRService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapConverter.hpp>
#include <FslGraphics2D/PixelFormatConverter/Bitmap/RawBitmapToneMapper.hpp>
#include <half.hpp>

namespace Fsl
{
  ImageConverterLibraryHDRService::ImageConverterLibraryHDRService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  ImageConverterLibraryHDRService::~ImageConverterLibraryHDRService() = default;


  ReadOnlySpan<SupportedConversion> ImageConverterLibraryHDRService::GetSupportedConversions(const ConversionType conversionType) const noexcept
  {
    switch (conversionType)
    {
    case ConversionType::Bitmap:
      return FslGraphics2D::RawBitmapConverter::GetSupportedConversions();
    case ConversionType::Texture:
      return {};
    }
    return {};
  }


  ImageConvertResult ImageConverterLibraryHDRService::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (PixelFormatUtil::IsCompressed(desiredPixelFormat))
    {
      return ImageConvertResult::PixelFormatConversionNotSupported;
    }

    // Calc the preferred dst stride for the new format
    const uint32_t dstPreferredStride = rBitmap.GetPreferredStride(desiredPixelFormat);

    bool result = false;
    {
      Bitmap::ScopedDirectReadWriteAccess bitmapAccess(rBitmap);

      RawBitmapEx srcRawBitmap = bitmapAccess.AsRawBitmap();
      RawBitmapEx dstRawBitmap = RawBitmapEx::TryCreate(Span<uint8_t>(static_cast<uint8_t*>(srcRawBitmap.Content()), srcRawBitmap.GetByteSize()),
                                                        srcRawBitmap.GetSize(), desiredPixelFormat, dstPreferredStride, rBitmap.GetOrigin());
      if (!dstRawBitmap.IsValid())
      {
        // Inplace conversion is not possible as the RawBitmapEx using the given data is not valid
        return ImageConvertResult::PixelFormatConversionNotSupported;
      }

      result = FslGraphics2D::RawBitmapConverter::TryTransform(dstRawBitmap, srcRawBitmap);
    }
    if (!result)
    {
      return ImageConvertResult::PixelFormatConversionNotSupported;
    }

    // Reconfigure the bitmap so it matches the modified data
    rBitmap.Reset(rBitmap.GetSize(), desiredPixelFormat, dstPreferredStride, rBitmap.GetOrigin(), BitmapClearMethod::DontModify);
    return ImageConvertResult::Completed;
  }


  ImageConvertResult ImageConverterLibraryHDRService::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat)
  {
    return ImageConvertResult::NotSupported;
  }


  ImageConvertResult ImageConverterLibraryHDRService::TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap)
  {
    if (&rDstBitmap == &srcBitmap)
    {
      return ImageConvertResult::InplaceConversionNotSupported;
    }
    if (srcBitmap.GetOrigin() != rDstBitmap.GetOrigin() && rDstBitmap.GetOrigin() != BitmapOrigin::Undefined)
    {
      return ImageConvertResult::OriginConversionNotSupported;
    }

    bool result = false;
    {
      Bitmap::ScopedDirectReadAccess srcBitmapAccess(srcBitmap);
      Bitmap::ScopedDirectReadWriteAccess dstBitmapAccess(rDstBitmap);

      result = FslGraphics2D::RawBitmapConverter::TryTransform(dstBitmapAccess.AsRawBitmap(), srcBitmapAccess.AsRawBitmap());
    }
    return result ? ImageConvertResult::Completed : ImageConvertResult::PixelFormatConversionNotSupported;
  }


  ImageConvertResult ImageConverterLibraryHDRService::TryConvert(Texture& rDstTexture, const Texture& srcTexture)
  {
    if (&rDstTexture == &srcTexture)
    {
      return ImageConvertResult::InplaceConversionNotSupported;
    }
    if (srcTexture.GetBitmapOrigin() != rDstTexture.GetBitmapOrigin() && rDstTexture.GetBitmapOrigin() != BitmapOrigin::Undefined)
    {
      return ImageConvertResult::OriginConversionNotSupported;
    }
    return ImageConvertResult::NotSupported;
  }


  ReadOnlySpan<SupportedToneMapping> ImageConverterLibraryHDRService::GetSupportedToneMappings(const ConversionType conversionType) const noexcept
  {
    switch (conversionType)
    {
    case ConversionType::Bitmap:
      return FslGraphics2D::RawBitmapToneMapper::GetSupportedToneMappings();
    case ConversionType::Texture:
      return {};
    }
    return {};
  }


  ToneMappingResult ImageConverterLibraryHDRService::TryToneMap(Bitmap& rBitmap, const BasicToneMapper toneMapping, const float exposure)
  {
    Bitmap::ScopedDirectReadWriteAccess bitmapAccess(rBitmap);
    if (FslGraphics2D::RawBitmapToneMapper::TryToneMap(bitmapAccess.AsRawBitmap(), bitmapAccess.AsRawBitmap(), toneMapping, exposure))
    {
      return ToneMappingResult::Completed;
    }
    return ToneMappingResult::NotSupported;
  }


  ToneMappingResult ImageConverterLibraryHDRService::TryToneMap(Texture& rTexture, const BasicToneMapper toneMapping, const float exposure)
  {
    return ToneMappingResult::NotSupported;
  }
}
