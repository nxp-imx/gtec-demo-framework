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

#include <FslGraphics/Render/Texture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslGraphics/Render/Adapter/INativeTexture2D.hpp>
#include <FslBase/Exceptions.hpp>

namespace Fsl
{
  Texture2D::Texture2D() = default;


  Texture2D::Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                       const TextureFlags& textureFlags)
    : Texture2D()
  {
    Reset(nativeGraphics, bitmap, filterHint, textureFlags);
  }


  Texture2D::Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                       const TextureFlags& textureFlags)
    : Texture2D()
  {
    Reset(nativeGraphics, bitmap, filterHint, textureFlags);
  }


  Texture2D::Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
                       const TextureFlags& textureFlags)
    : Texture2D()
  {
    Reset(nativeGraphics, texture, filterHint, textureFlags);
  }


  Texture2D::Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawTexture& texture, const Texture2DFilterHint filterHint,
                       const TextureFlags& textureFlags)
    : Texture2D()
  {
    Reset(nativeGraphics, texture, filterHint, textureFlags);
  }

  Texture2D::~Texture2D() = default;


  bool Texture2D::IsValid() const
  {
    return m_native != nullptr;
  }


  void Texture2D::Reset()
  {
    m_native.reset();
    m_size = Point2();
    m_pixelFormat = PixelFormat::Undefined;
  }


  void Texture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                        const TextureFlags& textureFlags)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);
    Reset(nativeGraphics, rawBitmap, filterHint, textureFlags);
  }


  void Texture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                        const TextureFlags& textureFlags)
  {
    if (!nativeGraphics)
    {
      throw std::invalid_argument("nativeGraphics can not be null");
    }
    if (!bitmap.IsValid())
    {
      throw std::invalid_argument("bitmap is invalid");
    }
    if (bitmap.Width() < 1 || bitmap.Height() < 1)
    {
      throw std::invalid_argument("bitmap size is invalid");
    }

    Reset();

    m_native = nativeGraphics->CreateTexture2D(bitmap, filterHint, textureFlags);
    m_size = Point2(bitmap.Width(), bitmap.Height());
    m_pixelFormat = bitmap.GetPixelFormat();
  }


  void Texture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
                        const TextureFlags& textureFlags)
  {
    RawTexture rawTexture;
    Texture::ScopedDirectAccess directAccess(texture, rawTexture);
    Reset(nativeGraphics, rawTexture, filterHint, textureFlags);
  }


  void Texture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawTexture& texture, const Texture2DFilterHint filterHint,
                        const TextureFlags& textureFlags)
  {
    if (!nativeGraphics)
    {
      throw std::invalid_argument("nativeGraphics can not be null");
    }
    if (!texture.IsValid())
    {
      throw std::invalid_argument("bitmap is invalid");
    }
    if (texture.GetExtent().Width < 1 || texture.GetExtent().Height < 1)
    {
      throw std::invalid_argument("bitmap size is invalid");
    }

    Reset();

    m_native = nativeGraphics->CreateTexture2D(texture, filterHint, textureFlags);
    m_size = Point2(texture.GetExtent().Width, texture.GetExtent().Height);
    m_pixelFormat = texture.GetPixelFormat();
  }


  void Texture2D::SetData(const Bitmap& bitmap, const Texture2DFilterHint filterHint, const TextureFlags& textureFlags)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);
    SetData(rawBitmap, filterHint, textureFlags);
  }


  void Texture2D::SetData(const RawBitmap& bitmap, const Texture2DFilterHint filterHint, const TextureFlags& textureFlags)
  {
    if (!bitmap.IsValid())
    {
      throw std::invalid_argument("bitmap is invalid");
    }
    if (bitmap.Width() < 1 || bitmap.Height() < 1)
    {
      throw std::invalid_argument("bitmap size is invalid");
    }

    m_native->SetData(bitmap, filterHint, textureFlags);
  }


  void Texture2D::SetData(const Texture& texture, const Texture2DFilterHint filterHint, const TextureFlags& textureFlags)
  {
    RawTexture rawTexture;
    Texture::ScopedDirectAccess scopedAccess(texture, rawTexture);
    SetData(rawTexture, filterHint, textureFlags);
  }


  void Texture2D::SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags& textureFlags)
  {
    if (!texture.IsValid())
    {
      throw std::invalid_argument("texture is invalid");
    }
    if (texture.GetExtent().Width < 1 || texture.GetExtent().Height < 1)
    {
      throw std::invalid_argument("texture size is invalid");
    }

    m_native->SetData(texture, filterHint, textureFlags);
  }

  Extent2D Texture2D::GetExtent() const
  {
    return {m_size.X, m_size.Y};
  }


  Point2 Texture2D::GetSize() const
  {
    return m_size;
  }

  PixelFormat Texture2D::GetPixelFormat() const
  {
    return m_pixelFormat;
  }


  std::shared_ptr<INativeTexture2D> Texture2D::TryGetNative() const
  {
    return m_native;
  }

  std::shared_ptr<INativeTexture2D> Texture2D::GetNative() const
  {
    if (!m_native)
    {
      throw GraphicsException("Not connected to native texture");
    }
    return m_native;
  }
}
