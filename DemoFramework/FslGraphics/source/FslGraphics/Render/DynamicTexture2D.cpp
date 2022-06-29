/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslGraphics/Render/Adapter/IDynamicNativeTexture2D.hpp>
#include <FslGraphics/Render/Adapter/INativeGraphics.hpp>
#include <FslGraphics/Render/DynamicTexture2D.hpp>
#include <FslGraphics/Texture/RawTextureHelper.hpp>
#include <utility>

namespace Fsl
{
  DynamicTexture2D::DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap,
                                     const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    : DynamicTexture2D()
  {
    Reset(nativeGraphics, bitmap, filterHint, textureFlags);
  }


  DynamicTexture2D::DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawBitmap& bitmap,
                                     const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    : DynamicTexture2D()
  {
    Reset(nativeGraphics, bitmap, filterHint, textureFlags);
  }


  DynamicTexture2D::DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture,
                                     const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    : DynamicTexture2D()
  {
    Reset(nativeGraphics, texture, filterHint, textureFlags);
  }


  DynamicTexture2D::DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawTexture& texture,
                                     const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
    : DynamicTexture2D()
  {
    Reset(nativeGraphics, texture, filterHint, textureFlags);
  }


  void DynamicTexture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                               const TextureFlags textureFlags)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);
    Reset(nativeGraphics, rawBitmap, filterHint, textureFlags);
  }


  void DynamicTexture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
                               const TextureFlags textureFlags)
  {
    if (!bitmap.IsValid())
    {
      throw std::invalid_argument("bitmap is invalid");
    }
    if (bitmap.Width() < 1 || bitmap.Height() < 1)
    {
      throw std::invalid_argument("bitmap size is invalid");
    }

    Reset(nativeGraphics, RawTextureHelper::ToRawTexture(bitmap), filterHint, textureFlags);
  }


  void DynamicTexture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
                               const TextureFlags textureFlags)
  {
    RawTexture rawTexture;
    Texture::ScopedDirectAccess directAccess(texture, rawTexture);
    Reset(nativeGraphics, rawTexture, filterHint, textureFlags);
  }


  void DynamicTexture2D::Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawTexture& texture,
                               const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    if (!nativeGraphics)
    {
      throw std::invalid_argument("nativeGraphics can not be null");
    }
    if (!texture.IsValid())
    {
      throw std::invalid_argument("bitmap is invalid");
    }
    if (texture.GetExtent().Width < 1 || texture.GetExtent().Height < 1 || texture.GetExtent().Depth != 1)
    {
      throw std::invalid_argument("bitmap size is invalid");
    }

    auto native = nativeGraphics->CreateDynamicTexture2D(texture, filterHint, textureFlags);

    DoReset(std::move(native), PxExtent2D(texture.GetExtent().Width, texture.GetExtent().Height), texture.GetPixelFormat());
  }

  void DynamicTexture2D::SetData(const Bitmap& bitmap, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    RawBitmap rawBitmap;
    Bitmap::ScopedDirectAccess directAccess(bitmap, rawBitmap);
    SetData(rawBitmap, filterHint, textureFlags);
  }


  void DynamicTexture2D::SetData(const RawBitmap& bitmap, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    if (!bitmap.IsValid())
    {
      throw std::invalid_argument("bitmap is invalid");
    }
    SetData(RawTextureHelper::ToRawTexture(bitmap), filterHint, textureFlags);
  }


  void DynamicTexture2D::SetData(const Texture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    RawTexture rawTexture;
    Texture::ScopedDirectAccess scopedAccess(texture, rawTexture);
    SetData(rawTexture, filterHint, textureFlags);
  }


  void DynamicTexture2D::SetData(const RawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags)
  {
    auto native = TryGetDynamicNativeTexture();
    if (!native)
    {
      throw UsageErrorException("SetData called on a invalid object");
    }
    if (!texture.IsValid())
    {
      throw std::invalid_argument("texture is invalid");
    }
    const auto currentExtent = GetExtent();
    if (texture.GetExtent().Width != currentExtent.Width || texture.GetExtent().Height != currentExtent.Height || texture.GetExtent().Depth != 1)
    {
      throw UsageErrorException("texture is not of the expected size");
    }
    if (texture.GetPixelFormat() != GetPixelFormat())
    {
      throw UsageErrorException("texture is not of the right pixel format");
    }

    native->SetData(texture, filterHint, textureFlags);
  }

  std::shared_ptr<IDynamicNativeTexture2D> DynamicTexture2D::TryGetDynamicNativeTexture() const
  {
    return std::dynamic_pointer_cast<IDynamicNativeTexture2D>(TryGetNative());
  }


  std::shared_ptr<IDynamicNativeTexture2D> DynamicTexture2D::GetDynamicNativeTexture() const
  {
    auto native = TryGetDynamicNativeTexture();
    if (!native)
    {
      throw GraphicsException("Not connected to native texture");
    }
    return native;
  }
}
