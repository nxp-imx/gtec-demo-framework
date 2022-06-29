/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslDemoHost/Base/Service/BitmapConverter/BitmapConverterService.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureUtil.hpp>
#include <FslService/Consumer/ServiceProvider.hpp>

namespace Fsl
{
  namespace
  {
    bool TryConvertLibraries(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                             const std::deque<std::shared_ptr<IImageConverterLibraryService>>& converterLibraries)
    {
      for (const auto& converter : converterLibraries)
      {
        if (converter->TryConvert(rBitmap, rBitmap, desiredPixelFormat, desiredOrigin))
        {
          return true;
        }
      }
      return false;
    }

    bool TryConvertLibraries(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin,
                             const std::deque<std::shared_ptr<IImageConverterLibraryService>>& converterLibraries)
    {
      for (const auto& converter : converterLibraries)
      {
        if (converter->TryConvert(rTexture, rTexture, desiredPixelFormat, desiredOrigin))
        {
          return true;
        }
      }
      return false;
    }
  }


  BitmapConverterService::BitmapConverterService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)

  {
    // Extract the converter libraries so we can use the available ones
    serviceProvider.Get<IImageConverterLibraryService>(m_converterLibraries);
  }


  BitmapConverterService::~BitmapConverterService() = default;


  bool BitmapConverterService::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvertLibraries(rBitmap, desiredPixelFormat, desiredOrigin, m_converterLibraries))
    {
      return false;
    }

    if (desiredOrigin != BitmapOrigin::Undefined && rBitmap.GetOrigin() != desiredOrigin)
    {
      BitmapUtil::FlipHorizontal(rBitmap);
    }
    return true;
  }


  void BitmapConverterService::Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvert(rBitmap, desiredPixelFormat, desiredOrigin))
    {
      throw GraphicsException("Unsupported pixel format or origin");
    }
  }


  bool BitmapConverterService::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvertLibraries(rTexture, desiredPixelFormat, desiredOrigin, m_converterLibraries))
    {
      return false;
    }
    return TextureUtil::TryChangeOrigin(rTexture, desiredOrigin);
  }


  void BitmapConverterService::Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvertLibraries(rTexture, desiredPixelFormat, desiredOrigin, m_converterLibraries))
    {
      throw GraphicsException("Unsupported pixel format");
    }

    if (!TextureUtil::TryChangeOrigin(rTexture, desiredOrigin))
    {
      throw GraphicsException("Unsupported origin");
    }
  }
}
