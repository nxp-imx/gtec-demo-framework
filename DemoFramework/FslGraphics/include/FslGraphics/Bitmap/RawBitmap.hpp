#ifndef FSLGRAPHICS_BITMAP_RAWBITMAP_HPP
#define FSLGRAPHICS_BITMAP_RAWBITMAP_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Extent2D.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>

namespace Fsl
{
  struct RawBitmapArea;

  class RawBitmap
  {
    //! The pointer to the raw image data
    const void* m_pContent{nullptr};
    Extent2D m_extent;
    uint32_t m_stride{0};
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    BitmapOrigin m_origin{BitmapOrigin::UpperLeft};

  public:
    RawBitmap();
    RawBitmap(const void* const pContent, const Extent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);
    RawBitmap(const void* const pContent, const Extent2D& extent, const PixelFormat pixelFormat, const uint32_t stride, const BitmapOrigin origin);

    RawBitmap(const void* const pContent, const int32_t width, const int32_t height, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : RawBitmap(pContent, Extent2D(width, height), pixelFormat, origin)
    {
    }

    RawBitmap(const void* const pContent, const uint32_t width, const uint32_t height, const PixelFormat pixelFormat, const BitmapOrigin origin)
      : RawBitmap(pContent, Extent2D(width, height), pixelFormat, origin)
    {
    }

    RawBitmap(const void* const pContent, const int32_t width, const int32_t height, const PixelFormat pixelFormat, const uint32_t stride,
              const BitmapOrigin origin)
      : RawBitmap(pContent, Extent2D(width, height), pixelFormat, stride, origin)
    {
    }

    RawBitmap(const void* const pContent, const uint32_t width, const uint32_t height, const PixelFormat pixelFormat, const uint32_t stride,
              const BitmapOrigin origin)
      : RawBitmap(pContent, Extent2D(width, height), pixelFormat, stride, origin)
    {
    }


    RawBitmap(const RawBitmapArea& rawBitmapArea, const PixelFormat pixelFormat, const BitmapOrigin origin);


    bool IsValid() const
    {
      return m_pContent != nullptr;
    }

    //! @brief Get a pointer to the content
    const void* Content() const
    {
      return m_pContent;
    }

    //! The width of the bitmap in pixels
    Extent2D::element_type Width() const
    {
      return m_extent.Width;
    }

    //! The height of the bitmap in pixels
    Extent2D::element_type Height() const
    {
      return m_extent.Height;
    }

    //! The number of bytes that represent one scan line of the bitmap.
    uint32_t Stride() const
    {
      return m_stride;
    }

    //! The number of bytes that can be stored in m_pContent
    uint32_t GetBufferLength() const
    {
      return m_extent.Height * m_stride;
    }

    Extent2D GetExtent() const
    {
      return m_extent;
    }

    //! Get the origin of the bitmap
    BitmapOrigin GetOrigin() const
    {
      return m_origin;
    }

    //! Get the pixel format of the raw bitmap
    PixelFormat GetPixelFormat() const
    {
      return m_pixelFormat;
    }

    PixelFormatLayout GetPixelFormatLayout() const;

  protected:
    void DoSetPixelFormat(const PixelFormat value)
    {
      m_pixelFormat = value;
    }

    //! Set the origin of the bitmap
    void DoSetBitmapOrigin(const BitmapOrigin value)
    {
      m_origin = value;
    }

    //! Set the stride
    void DoSetStride(const uint32_t value)
    {
      m_stride = value;
    }
  };
}

#endif
