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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Bitmap/RawBitmapArea.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cmath>
#include <limits>


namespace Fsl
{
  RawBitmap::RawBitmap(const void* const pContent, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : m_pContent(pContent)
    , m_extent(extent)
    , m_stride(PixelFormatUtil::CalcMinimumStride(extent.Width, pixelFormat))
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
  {
    if (m_pContent == nullptr)
    {
      throw std::invalid_argument("pContent can not be null");
    }
  }


  RawBitmap::RawBitmap(const void* const pContent, const PxExtent2D& extent, const PixelFormat pixelFormat, const uint32_t stride,
                       const BitmapOrigin origin)
    : m_pContent(pContent)
    , m_extent(extent)
    , m_stride(stride)
    , m_pixelFormat(pixelFormat)
    , m_origin(origin)
  {
    if (m_pContent == nullptr)
    {
      throw std::invalid_argument("pContent can not be null");
    }
    if (stride < PixelFormatUtil::CalcMinimumStride(m_extent.Width, pixelFormat))
    {
      throw std::invalid_argument("stride is smaller than the width allows");
    }
  }


  RawBitmap::RawBitmap(const RawBitmapArea& rawBitmapArea, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : RawBitmap(rawBitmapArea.Content, rawBitmapArea.Extent.Width, rawBitmapArea.Extent.Height, pixelFormat,
                static_cast<int32_t>(rawBitmapArea.Stride), origin)
  {
    if (rawBitmapArea.Stride > static_cast<uint32_t>(std::numeric_limits<int32_t>::max()))
    {
      throw NotSupportedException("The stride is larger than a RawBitmap allows");
    }
  }


  PixelFormatLayout RawBitmap::GetPixelFormatLayout() const
  {
    return PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat);
  }
}
