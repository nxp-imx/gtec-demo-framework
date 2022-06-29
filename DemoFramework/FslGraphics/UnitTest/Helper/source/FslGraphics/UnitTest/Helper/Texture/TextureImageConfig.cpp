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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/Texture/TextureImageConfig.hpp>


namespace Fsl::TextureImageConfig
{
  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // R8G8B8
  // -----------------------------------------------------------------------------------------------------------------------------------------------


  R8G8B8::pixel_t R8G8B8::GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin)
  {
    if (!image.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (image.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    pixel_t pixel;
    pixel.R = image.GetUInt8(0, 0, 0, (x * 3) + 0, y, 0, ignoreOrigin);
    pixel.G = image.GetUInt8(0, 0, 0, (x * 3) + 1, y, 0, ignoreOrigin);
    pixel.B = image.GetUInt8(0, 0, 0, (x * 3) + 2, y, 0, ignoreOrigin);
    return pixel;
  }


  void R8G8B8::SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin)
  {
    if (!rImage.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (rImage.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    rImage.SetUInt8(0, 0, 0, (x * 3) + 0, y, 0, pixel.R, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 3) + 1, y, 0, pixel.G, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 3) + 2, y, 0, pixel.B, ignoreOrigin);
  }

  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // B8G8R8
  // -----------------------------------------------------------------------------------------------------------------------------------------------


  B8G8R8::pixel_t B8G8R8::GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin)
  {
    if (!image.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (image.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    pixel_t pixel;
    pixel.B = image.GetUInt8(0, 0, 0, (x * 3) + 0, y, 0, ignoreOrigin);
    pixel.G = image.GetUInt8(0, 0, 0, (x * 3) + 1, y, 0, ignoreOrigin);
    pixel.R = image.GetUInt8(0, 0, 0, (x * 3) + 2, y, 0, ignoreOrigin);
    return pixel;
  }


  void B8G8R8::SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin)
  {
    if (!rImage.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (rImage.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    rImage.SetUInt8(0, 0, 0, (x * 3) + 0, y, 0, pixel.B, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 3) + 1, y, 0, pixel.G, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 3) + 2, y, 0, pixel.R, ignoreOrigin);
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // R8G8B8A8
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  R8G8B8A8::pixel_t R8G8B8A8::GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin)
  {
    if (!image.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (image.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    pixel_t pixel;
    pixel.R = image.GetUInt8(0, 0, 0, (x * 4) + 0, y, 0, ignoreOrigin);
    pixel.G = image.GetUInt8(0, 0, 0, (x * 4) + 1, y, 0, ignoreOrigin);
    pixel.B = image.GetUInt8(0, 0, 0, (x * 4) + 2, y, 0, ignoreOrigin);
    pixel.A = image.GetUInt8(0, 0, 0, (x * 4) + 3, y, 0, ignoreOrigin);
    return pixel;
  }


  void R8G8B8A8::SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin)
  {
    if (!rImage.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (rImage.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    rImage.SetUInt8(0, 0, 0, (x * 4) + 0, y, 0, pixel.R, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 4) + 1, y, 0, pixel.G, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 4) + 2, y, 0, pixel.B, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 4) + 3, y, 0, pixel.A, ignoreOrigin);
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // B8G8R8A8
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  B8G8R8A8::pixel_t B8G8R8A8::GetPixel(const image_t& image, const uint32_t x, const uint32_t y, const bool ignoreOrigin)
  {
    if (!image.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (image.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    pixel_t pixel;
    pixel.B = image.GetUInt8(0, 0, 0, (x * 4) + 0, y, 0, ignoreOrigin);
    pixel.G = image.GetUInt8(0, 0, 0, (x * 4) + 1, y, 0, ignoreOrigin);
    pixel.R = image.GetUInt8(0, 0, 0, (x * 4) + 2, y, 0, ignoreOrigin);
    pixel.A = image.GetUInt8(0, 0, 0, (x * 4) + 3, y, 0, ignoreOrigin);
    return pixel;
  }


  void B8G8R8A8::SetPixel(image_t& rImage, const uint32_t x, const uint32_t y, const pixel_t& pixel, const bool ignoreOrigin)
  {
    if (!rImage.IsValid())
    {
      throw std::invalid_argument("texture is not valid");
    }
    if (rImage.GetPixelFormatLayout() != ActivePixelLayout)
    {
      throw std::invalid_argument("texture is not of the expected PixelFormat");
    }

    rImage.SetUInt8(0, 0, 0, (x * 4) + 0, y, 0, pixel.B, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 4) + 1, y, 0, pixel.G, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 4) + 2, y, 0, pixel.R, ignoreOrigin);
    rImage.SetUInt8(0, 0, 0, (x * 4) + 3, y, 0, pixel.A, ignoreOrigin);
  }
}
