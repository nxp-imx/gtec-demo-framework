#ifndef FSLGRAPHICS_RENDER_TEXTURE2D_HPP
#define FSLGRAPHICS_RENDER_TEXTURE2D_HPP
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
#include <FslGraphics/Bitmap/RawBitmap.hpp>
#include <FslGraphics/Render/BaseTexture2D.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/TextureFlags.hpp>

namespace Fsl
{
  class INativeGraphics;

  //! @brief A read only native texture
  class Texture2D : public BaseTexture2D
  {
  public:
    //! @brief Create a uninitialized texture (use SetData to add texture data to it)
    Texture2D() = default;

    //! @brief Create a initialized texture
    Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
              const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
              const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
              const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    Texture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawTexture& texture, const Texture2DFilterHint filterHint,
              const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    Texture2D(std::shared_ptr<INativeTexture2D> native, const PxExtent2D extent, const PixelFormat pixelFormat);

    ~Texture2D() = default;

    using BaseTexture2D::Reset;

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawBitmap& bitmap, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const RawTexture& texture, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);
    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(std::shared_ptr<INativeTexture2D> native, const PxExtent2D extent, const PixelFormat pixelFormat);

    using BaseTexture2D::operator==;
    using BaseTexture2D::operator!=;
  };
}

#endif
