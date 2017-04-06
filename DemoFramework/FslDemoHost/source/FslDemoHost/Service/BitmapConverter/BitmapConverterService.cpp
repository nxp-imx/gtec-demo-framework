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

#include <FslDemoHost/Service/BitmapConverter/BitmapConverterService.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/BitmapUtil.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureUtil.hpp>

namespace Fsl
{
  BitmapConverterService::BitmapConverterService(const ServiceProvider& serviceProvider)
    : ThreadLocalService(serviceProvider)
  {
  }


  BitmapConverterService::~BitmapConverterService()
  {
  }


  bool BitmapConverterService::TryConvert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvertPixelFormat(rBitmap, desiredPixelFormat))
      return false;

    if (desiredOrigin != BitmapOrigin::Undefined && rBitmap.GetOrigin() != desiredOrigin)
    {
      BitmapUtil::FlipHorizontal(rBitmap);
    }
    return true;
  }


  void BitmapConverterService::Convert(Bitmap& rBitmap, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    if (!TryConvert(rBitmap, desiredPixelFormat, desiredOrigin))
      throw GraphicsException("Unsupported pixel format");
  }


  bool BitmapConverterService::TryConvert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    return TextureUtil::TryConvert(rTexture, desiredPixelFormat, desiredOrigin);
  }


  void BitmapConverterService::Convert(Texture& rTexture, const PixelFormat desiredPixelFormat, const BitmapOrigin desiredOrigin)
  {
    return TextureUtil::Convert(rTexture, desiredPixelFormat, desiredOrigin);
  }



  //bool BitmapConverterService::TryConvert(Bitmap& rDstBitmap, const Bitmap& srcBitmap, const PixelFormat desiredPixelFormat)
  //{
  //  // Detect in-place conversions requests and handle them separately
  //  if (&rDstBitmap == &srcBitmap)
  //    return TryConvert(rDstBitmap, desiredPixelFormat);

  //  for (auto itr = m_converters.begin(); itr != m_converters.end(); ++itr)
  //  {
  //    if ((*itr)->TryConvert(rDstBitmap, srcBitmap, desiredPixelFormat))
  //      return true;
  //  }
  //  return false;
  //}


  bool BitmapConverterService::TryConvertPixelFormat(Bitmap& rBitmap, const PixelFormat desiredPixelFormat)
  {
    if (desiredPixelFormat == PixelFormat::Undefined || rBitmap.GetPixelFormat() == desiredPixelFormat)
      return true;

    // For now use the limited converter from FslGraphics
    return BitmapUtil::TryConvert(rBitmap, desiredPixelFormat);


    //{ // Lets see if we can convert using a temporary object
    //  Bitmap tmp(rBitmap.Width(), rBitmap.Height(), desiredPixelFormat, rBitmap.GetOrigin());
    //  if (TryConvert(tmp, rBitmap, desiredPixelFormat))
    //  {
    //    // Overwrite the existing bitmap with the temporary one
    //    rBitmap = tmp;
    //    return true;
    //  }
    //}
    //return false;
  }
}
