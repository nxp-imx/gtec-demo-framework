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

// The interaction with FB was copied from the existing G2D overlay example

#include "G2DSurface.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
#include <cstring>

namespace Fsl
{
  namespace
  {
    g2d_format Convert(const PixelFormat pf, const bool isOpaque)
    {
      switch (PixelFormatUtil::GetPixelFormatLayout(pf))
      {
      case PixelFormatLayout::R8G8B8A8:
        return (!isOpaque ? G2D_RGBA8888 : G2D_RGBX8888);
      default:
        throw NotSupportedException("Pixel format is unsupported");
      }
    }

    const int32_t CalcG2DMinStride(const int32_t width, g2d_format g2dFormat)
    {
      switch (g2dFormat)
      {
      case G2D_RGB565:
      case G2D_BGR565:
        return width * 2;
      case G2D_RGBA8888:
      case G2D_RGBX8888:
      case G2D_BGRA8888:
      case G2D_BGRX8888:
      case G2D_ARGB8888:
      case G2D_ABGR8888:
      case G2D_XRGB8888:
      case G2D_XBGR8888:
        return width * 4;
      default:
        throw NotSupportedException("Unsupported pixel format");
      }
    }
  }


  G2DSurface::G2DSurface(const Bitmap& bitmap, const bool isOpaque, const std::string name, const bool allowCache)
    : m_pBuf(nullptr)
    , Name(name)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess access(bitmap, rawBitmap);

    const int32_t minStride = PixelFormatUtil::CalcMinimumStride(rawBitmap.Width(), rawBitmap.GetPixelFormat(), StrideRequirement::Minimum);
    const int32_t bufferSize = minStride * rawBitmap.Height();

    std::memset(&m_surface, 0, sizeof(g2d_surface));
    m_surface.left = 0;
    m_surface.top = 0;
    m_surface.right = rawBitmap.Width();
    m_surface.bottom = rawBitmap.Height();
    m_surface.stride = rawBitmap.Width(); // minStride;
    m_surface.width = rawBitmap.Width();
    m_surface.height = rawBitmap.Height();
    m_surface.rot = G2D_ROTATION_0;
    m_surface.format = Convert(rawBitmap.GetPixelFormat(), isOpaque);
    // FIX: blend func
    m_surface.blendfunc = G2D_SRC_ALPHA;
    m_surface.global_alpha = 0x80;

    m_pBuf = g2d_alloc(bufferSize, allowCache ? 1 : 0);
    if (m_pBuf == nullptr)
      throw GraphicsException("Failed to allocate surface buffer");

    m_surface.planes[0] = m_pBuf->buf_paddr;

    std::memcpy(m_pBuf->buf_vaddr, rawBitmap.Content(), bufferSize);

    FSLLOG("Surface plane[0]=" << std::hex << m_surface.planes[0] << std::dec << " for '" << name << "'");
  }


  G2DSurface::G2DSurface(const int32_t width, const int32_t height, const g2d_format g2dFormat, const std::string name, const bool allowCache)
  {
    std::memset(&m_surface, 0, sizeof(g2d_surface));
    m_surface.left = 0;
    m_surface.top = 0;
    m_surface.right = width;
    m_surface.bottom = height;
    m_surface.stride = width;
    m_surface.width = width;
    m_surface.height = height;
    m_surface.rot = G2D_ROTATION_0;
    m_surface.format = g2dFormat;
    // FIX: blend func
    m_surface.blendfunc = G2D_SRC_ALPHA;
    m_surface.global_alpha = 0x80;

    const int32_t bufferSize = CalcG2DMinStride(width, g2dFormat) * height;

    m_pBuf = g2d_alloc(bufferSize, allowCache ? 1 : 0);
    if (m_pBuf == nullptr)
      throw GraphicsException("Failed to allocate surface buffer");

    m_surface.planes[0] = m_pBuf->buf_paddr;

    std::memset(m_pBuf->buf_vaddr, 0, bufferSize);
    FSLLOG("Surface plane[0]=" << std::hex << m_surface.planes[0] << std::dec << " for '" << name << "'");
  }


  G2DSurface::~G2DSurface()
  {
    FSLLOG("Freeing: " << Name);
    if (m_pBuf != nullptr)
      g2d_free(m_pBuf);
    FSLLOG("Freed: " << Name);
  }

}
