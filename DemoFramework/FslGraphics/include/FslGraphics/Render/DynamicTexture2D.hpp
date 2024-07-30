#ifndef FSLGRAPHICS_RENDER_DYNAMICTEXTURE2D_HPP
#define FSLGRAPHICS_RENDER_DYNAMICTEXTURE2D_HPP
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

#include <FslGraphics/Bitmap/Bitmap.hpp>
#include <FslGraphics/Bitmap/ReadOnlyRawBitmap.hpp>
#include <FslGraphics/Render/BaseTexture2D.hpp>
#include <FslGraphics/Render/Texture2DFilterHint.hpp>
#include <FslGraphics/Texture/ReadOnlyRawTexture.hpp>
#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/TextureFlags.hpp>

namespace Fsl
{
  class IDynamicNativeTexture2D;
  class INativeGraphics;

  class DynamicTexture2D : public BaseTexture2D
  {
  public:
    //! @brief Create a uninitialized texture (use SetData to add texture data to it)
    DynamicTexture2D() = default;

    //! @brief Create a initialized texture
    DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
                     const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const ReadOnlyRawBitmap& bitmap, const Texture2DFilterHint filterHint,
                     const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
                     const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Create a initialized texture
    DynamicTexture2D(const std::shared_ptr<INativeGraphics>& nativeGraphics, const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint,
                     const TextureFlags textureFlags = TextureFlags::NotDefined);

    ~DynamicTexture2D() = default;

    using BaseTexture2D::Reset;

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Bitmap& bitmap, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const ReadOnlyRawBitmap& bitmap, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const Texture& texture, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Reset the texture to contain the given bitmap (this is a very slow operation)
    void Reset(const std::shared_ptr<INativeGraphics>& nativeGraphics, const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint,
               const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Set the data of the texture
    void SetData(const Bitmap& bitmap, const Texture2DFilterHint filterHint, const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Set the data of the texture
    void SetData(const ReadOnlyRawBitmap& bitmap, const Texture2DFilterHint filterHint, const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Set the data of the texture
    void SetData(const Texture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Set the data of the texture
    void SetData(const ReadOnlyRawTexture& texture, const Texture2DFilterHint filterHint, const TextureFlags textureFlags = TextureFlags::NotDefined);

    //! @brief Acquire the native texture (returns null if none exist)
    std::shared_ptr<IDynamicNativeTexture2D> TryGetDynamicNativeTexture() const;

    //! @brief Acquire the native texture (throws if none exist)
    std::shared_ptr<IDynamicNativeTexture2D> GetDynamicNativeTexture() const;

    using BaseTexture2D::operator==;
    using BaseTexture2D::operator!=;
  };
}

#endif
