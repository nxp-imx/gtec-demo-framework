/****************************************************************************************************************************************************
 * Copyright 2018, 2024 NXP
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

#include <FslDemoApp/Util/Graphics/Service/ImageConverter/ImageConverterLibraryBasicService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureUtil.hpp>

namespace Fsl
{
  ImageConverterLibraryBasicService::ImageConverterLibraryBasicService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  ImageConverterLibraryBasicService::~ImageConverterLibraryBasicService() = default;


  ReadOnlySpan<SupportedConversion> ImageConverterLibraryBasicService::GetSupportedConversions(const ConversionType conversionType) const noexcept
  {
    switch (conversionType)
    {
    case ConversionType::Bitmap:
      return BitmapUtil::GetSupportedConversions();
    case ConversionType::Texture:
      return TextureUtil::GetSupportedConversions();
    }
    return {};
  }


  ImageConvertResult ImageConverterLibraryBasicService::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (desiredPixelFormat == PixelFormat::Undefined || rBitmap.GetPixelFormat() == desiredPixelFormat)
    {
      // Nothing to do -> quick exit
      return ImageConvertResult::Completed;
    }

    // Use the basic converter to convert
    if (!BitmapUtil::TryConvert(rBitmap, desiredPixelFormat))
    {
      return ImageConvertResult::PixelFormatConversionNotSupported;
    }
    assert(rBitmap.GetPixelFormat() == desiredPixelFormat);
    return ImageConvertResult::Completed;
  }


  ImageConvertResult ImageConverterLibraryBasicService::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat)
  {
    if (desiredPixelFormat == PixelFormat::Undefined || rTexture.GetPixelFormat() == desiredPixelFormat)
    {
      // Nothing to do -> quick exit
      return ImageConvertResult::Completed;
    }

    if (!TextureUtil::TryConvert(rTexture, desiredPixelFormat, rTexture.GetBitmapOrigin()))
    {
      return ImageConvertResult::PixelFormatConversionNotSupported;
    }
    assert(rTexture.GetPixelFormat() == desiredPixelFormat);
    return ImageConvertResult::Completed;
  }


  ImageConvertResult ImageConverterLibraryBasicService::TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap)
  {
    if (&rDstBitmap == &srcBitmap)
    {
      return ImageConvertResult::InplaceConversionNotSupported;
    }
    if (rDstBitmap.GetOrigin() == srcBitmap.GetOrigin())
    {
      return ImageConvertResult::OriginConversionNotSupported;
    }

    return ImageConvertResult::NotSupported;
  }


  ImageConvertResult ImageConverterLibraryBasicService::TryConvert(Texture& rDstTexture, const Texture& srcTexture)
  {
    if (&rDstTexture == &srcTexture)
    {
      return ImageConvertResult::InplaceConversionNotSupported;
    }
    if (rDstTexture.GetBitmapOrigin() == srcTexture.GetBitmapOrigin())
    {
      return ImageConvertResult::OriginConversionNotSupported;
    }
    return ImageConvertResult::NotSupported;
  }
}
