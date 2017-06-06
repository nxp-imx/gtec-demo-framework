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

#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <FslGraphicsVG/VGCheck.hpp>
#include <FslGraphicsVG/VGImageBuffer.hpp>

#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace OpenVG
  {

    namespace
    {
      VGImageFormat Convert(const PixelFormat pixelFormat)
      {
        switch (PixelFormatUtil::GetPixelFormatLayout(pixelFormat))
        {
        case PixelFormatLayout::B8G8R8A8:
          return VG_sARGB_8888;
        case PixelFormatLayout::R8G8B8A8:
          return VG_sABGR_8888;
        default:
          throw UnsupportedPixelFormatException("Expected R8G8B8A8 or B8G8R8A8", pixelFormat);
        }
      }
    }

    VGImageBuffer::VGImageBuffer()
      : m_handle(VG_INVALID_HANDLE)
      , m_size()
    {
    }


    VGImageBuffer::VGImageBuffer(const VGImage handle, const Point2& size)
      : m_handle(handle)
      , m_size(size)
    {
    }


    VGImageBuffer::VGImageBuffer(const Bitmap& bitmap, const VGbitfield quality)
      : m_handle(VG_INVALID_HANDLE)
      , m_size()
    {
      Reset(bitmap, quality);
    }


    VGImageBuffer::VGImageBuffer(const RawBitmap& bitmap, const VGbitfield quality)
      : m_handle(VG_INVALID_HANDLE)
      , m_size()
    {
      Reset(bitmap, quality);
    }


    VGImageBuffer::~VGImageBuffer()
    {
      Reset();
    }


    void VGImageBuffer::Reset()
    {
      if (m_handle != VG_INVALID_HANDLE)
      {
        vgDestroyImage(m_handle);
        m_handle = VG_INVALID_HANDLE;
        m_size = Point2();
      }
    }


    void VGImageBuffer::Reset(const VGImage handle, const Point2& size)
    {
      Reset();
      m_handle = handle;
      m_size = size;
    }


    void VGImageBuffer::Reset(const Bitmap& bitmap, const VGbitfield quality)
    {
      RawBitmap rawBitmap;
      Bitmap::ScopedDirectAccess scopedAccess(bitmap, rawBitmap);
      Reset(rawBitmap, quality);
    }


    void VGImageBuffer::Reset(const RawBitmap& bitmap, const VGbitfield quality)
    {
      if (!bitmap.IsValid())
        throw std::invalid_argument("The bitmap must be valid");

      Reset();

      const VGImageFormat pixelFormat = Convert(bitmap.GetPixelFormat());
      m_handle = vgCreateImage(pixelFormat, bitmap.Width(), bitmap.Height(), quality);
      FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();

      const uint8_t* pSrc = static_cast<const uint8_t*>(bitmap.Content()) + (bitmap.Height()-1) * bitmap.Stride();
      assert(bitmap.Stride() < static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
      int32_t stride = static_cast<int32_t>(bitmap.Stride());
      vgImageSubData(m_handle, pSrc, -stride, pixelFormat, 0, 0, bitmap.Width(), bitmap.Height());
      FSLGRAPHICSOPENVG_CHECK_FOR_ERROR();
    }



    VGImage VGImageBuffer::GetHandle() const
    {
      return m_handle;
    }


    const Point2 VGImageBuffer::GetSize() const
    {
      return m_size;
    }

  }

}
