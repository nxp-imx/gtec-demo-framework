#ifndef FSLGRAPHICS_BITMAP_RAWBITMAPEX_HPP
#define FSLGRAPHICS_BITMAP_RAWBITMAPEX_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslGraphics/Bitmap/RawBitmap.hpp>

namespace Fsl
{
  class RawBitmapEx : public RawBitmap
  {
    void* m_pContent{nullptr};

  public:
    constexpr RawBitmapEx() = default;

    RawBitmapEx(void* const pContent, const PxExtent2D extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : RawBitmap(pContent, extent, pixelFormat, origin)
      , m_pContent(pContent)
    {
    }

    RawBitmapEx(void* const pContent, const PxExtent2D extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin origin)
      : RawBitmap(pContent, extent, pixelFormat, stride, origin)
      , m_pContent(pContent)
    {
    }

    RawBitmapEx(void* const pContent, const int32_t width, const int32_t height, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : RawBitmap(pContent, width, height, pixelFormat, origin)
      , m_pContent(pContent)
    {
    }

    RawBitmapEx(void* const pContent, const uint32_t width, const uint32_t height, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : RawBitmap(pContent, width, height, pixelFormat, origin)
      , m_pContent(pContent)
    {
    }

    RawBitmapEx(void* const pContent, const int32_t width, const int32_t height, const PixelFormat pixelFormat, const uint32_t stride,
                const BitmapOrigin origin)
      : RawBitmap(pContent, width, height, pixelFormat, stride, origin)
      , m_pContent(pContent)
    {
    }

    RawBitmapEx(void* const pContent, const uint32_t width, const uint32_t height, const PixelFormat pixelFormat, const uint32_t stride,
                const BitmapOrigin origin)
      : RawBitmap(pContent, width, height, pixelFormat, stride, origin)
      , m_pContent(pContent)
    {
    }

    using RawBitmap::Content;

    //! @brief Get a pointer to the content
    constexpr void* Content()
    {
      return m_pContent;
    }

    void SetPixelFormat(const PixelFormat value)
    {
      DoSetPixelFormat(value);
    }

    //! Set the origin of the bitmap
    void SetBitmapOrigin(const BitmapOrigin value)
    {
      DoSetBitmapOrigin(value);
    }

    //! Set the stride
    void SetStride(const uint32_t value)
    {
      DoSetStride(value);
    }
  };
}

#endif
