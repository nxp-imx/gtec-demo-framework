#ifndef FSLGRAPHICS_UNITTEST_HELPER_TEXTURE_TEXTUREIMAGECONFIG_HPP
#define FSLGRAPHICS_UNITTEST_HELPER_TEXTURE_TEXTUREIMAGECONFIG_HPP
/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslGraphics/PixelFormatLayout.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/UnitTest/Helper/PixelTestUtil.hpp>
#include <gtest/gtest.h>

namespace Fsl::TextureImageConfig
{
  template <typename TPixel, typename TColorTemplate, PixelFormat TPixelFormat>
  struct Base
  {
    using image_t = Texture;
    using pixel_t = TPixel;
    using color_template_t = TColorTemplate;

    static constexpr const PixelFormat ActivePixelFormat = TPixelFormat;
    static const PixelFormatLayout ActivePixelLayout = PixelFormatUtil::GetPixelFormatLayout(TPixelFormat);

    static image_t MakeImage(const PxExtent2D extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
    {
      return Texture(extent, pixelFormat, origin);
    }

    static PxExtent2D GetExtent(const image_t& image)
    {
      return {image.GetExtent().Width, image.GetExtent().Height};
    }
  };


  struct R8G8B8 : public Base<PixelTestUtil::R8G8B8::PixelUInt8, PixelTestUtil::R8G8B8::DefaultColorTemplate, PixelFormat::R8G8B8_UINT>
  {
    static pixel_t GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin);
    static void SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin);
  };

  struct B8G8R8 : public Base<PixelTestUtil::B8G8R8::PixelUInt8, PixelTestUtil::B8G8R8::DefaultColorTemplate, PixelFormat::B8G8R8_UINT>
  {
    static pixel_t GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin);
    static void SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin);
  };

  struct R8G8B8A8 : public Base<PixelTestUtil::R8G8B8A8::PixelUInt8, PixelTestUtil::R8G8B8A8::DefaultColorTemplate, PixelFormat::R8G8B8A8_UINT>
  {
    static pixel_t GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin);
    static void SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin);
  };


  struct B8G8R8A8 : public Base<PixelTestUtil::B8G8R8A8::PixelUInt8, PixelTestUtil::B8G8R8A8::DefaultColorTemplate, PixelFormat::B8G8R8A8_UINT>
  {
    static pixel_t GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin);
    static void SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin);
  };
}

#endif
