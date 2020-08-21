#ifndef FSLGRAPHICS_BITMAP_RAWCUBEBITMAP_HPP
#define FSLGRAPHICS_BITMAP_RAWCUBEBITMAP_HPP
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
#include <FslGraphics/Texture/TextureInfo.hpp>

namespace Fsl
{
  class RawCubeBitmap
  {
    RawBitmap m_bitmapPosX;
    RawBitmap m_bitmapNegX;
    RawBitmap m_bitmapPosY;
    RawBitmap m_bitmapNegY;
    RawBitmap m_bitmapPosZ;
    RawBitmap m_bitmapNegZ;

  public:
    constexpr RawCubeBitmap() = default;
    RawCubeBitmap(const RawBitmap& bitmapPosX, const RawBitmap& bitmapNegX, const RawBitmap& bitmapPosY, const RawBitmap& bitmapNegY,
                  const RawBitmap& bitmapPosZ, const RawBitmap& bitmapNegZ);

    constexpr bool IsValid() const
    {
      return m_bitmapPosX.IsValid();
    }

    //! The width of the bitmap in pixels
    constexpr uint32_t Width() const
    {
      return m_bitmapPosX.Width();
    }

    //! The height of the bitmap in pixels
    constexpr uint32_t Height() const
    {
      return m_bitmapPosX.Height();
    }

    //! @brief Get the extent
    constexpr PxExtent2D GetExtent() const
    {
      return m_bitmapPosX.GetExtent();
    }

    //! Get the pixel format of the raw bitmap
    constexpr PixelFormat GetPixelFormat() const
    {
      return m_bitmapPosX.GetPixelFormat();
    }

    //! Get the origin all the raw bitmaps
    constexpr BitmapOrigin GetOrigin() const
    {
      return m_bitmapPosX.GetOrigin();
    }

    constexpr RawBitmap GetPosX() const
    {
      return m_bitmapPosX;
    }
    constexpr RawBitmap GetNegX() const
    {
      return m_bitmapNegX;
    }
    constexpr RawBitmap GetPosY() const
    {
      return m_bitmapPosY;
    }
    constexpr RawBitmap GetNegY() const
    {
      return m_bitmapNegY;
    }
    constexpr RawBitmap GetPosZ() const
    {
      return m_bitmapPosZ;
    }
    constexpr RawBitmap GetNegZ() const
    {
      return m_bitmapNegZ;
    }

    constexpr uint32_t GetByteSize() const
    {
      return m_bitmapPosX.GetByteSize() + m_bitmapNegX.GetByteSize() + m_bitmapPosY.GetByteSize() + m_bitmapNegY.GetByteSize() +
             m_bitmapPosZ.GetByteSize() + m_bitmapNegZ.GetByteSize();
    }

    constexpr TextureInfo GetTextureInfo() const
    {
      return {1u, 6u, 1u};
    }
  };
}

#endif
