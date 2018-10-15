#ifndef FSLGRAPHICS_BITMAP_RAWCUBEBITMAPEX_HPP
#define FSLGRAPHICS_BITMAP_RAWCUBEBITMAPEX_HPP
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

#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <FslGraphics/Bitmap/RawCubeBitmap.hpp>

namespace Fsl
{
  class RawCubeBitmapEx
  {
    // FIX: optimize this (there is no need to store the internal data as RawBitmaps as most of the data is identical)
    RawBitmapEx m_bitmapPosX;
    RawBitmapEx m_bitmapNegX;
    RawBitmapEx m_bitmapPosY;
    RawBitmapEx m_bitmapNegY;
    RawBitmapEx m_bitmapPosZ;
    RawBitmapEx m_bitmapNegZ;

  public:
    RawCubeBitmapEx();
    RawCubeBitmapEx(const RawBitmapEx& bitmapPosX, const RawBitmapEx& bitmapNegX, const RawBitmapEx& bitmapPosY, const RawBitmapEx& bitmapNegY,
                    const RawBitmapEx& bitmapPosZ, const RawBitmapEx& bitmapNegZ);

    bool IsValid() const
    {
      return m_bitmapPosX.IsValid();
    }

    //! The width of the bitmap in pixels
    uint32_t Width() const
    {
      return m_bitmapPosX.Width();
    }

    //! The height of the bitmap in pixels
    uint32_t Height() const
    {
      return m_bitmapPosX.Height();
    }

    //! @brief Get the extent
    Extent2D GetExtent() const
    {
      return m_bitmapPosX.GetExtent();
    }

    //! Get the pixel format of the raw bitmap
    PixelFormat GetPixelFormat() const
    {
      return m_bitmapPosX.GetPixelFormat();
    }

    RawBitmapEx GetPosX() const
    {
      return m_bitmapPosX;
    }
    RawBitmapEx GetNegX() const
    {
      return m_bitmapNegX;
    }
    RawBitmapEx GetPosY() const
    {
      return m_bitmapPosY;
    }
    RawBitmapEx GetNegY() const
    {
      return m_bitmapNegY;
    }
    RawBitmapEx GetPosZ() const
    {
      return m_bitmapPosZ;
    }
    RawBitmapEx GetNegZ() const
    {
      return m_bitmapNegZ;
    }

    //! Conversion operator for easy conversion to a RawBitmap
    operator RawCubeBitmap() const
    {
      return RawCubeBitmap(m_bitmapPosX, m_bitmapNegX, m_bitmapPosY, m_bitmapNegY, m_bitmapPosZ, m_bitmapNegZ);
    }
  };
}

#endif
