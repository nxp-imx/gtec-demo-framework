#ifndef FSLGRAPHICS_RENDER_ATLASTEXTURE2D_HPP
#define FSLGRAPHICS_RENDER_ATLASTEXTURE2D_HPP
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

#include <FslGraphics/Render/BaseTexture2D.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>

namespace Fsl
{
  class AtlasTexture2D
  {
    BaseTexture2D m_atlas;
    AtlasTextureInfo m_info;

  public:
    // move assignment operator
    AtlasTexture2D& operator=(AtlasTexture2D&& other) noexcept;
    // move constructor
    AtlasTexture2D(AtlasTexture2D&& other) noexcept;

    // Request that the compiler generates a copy constructor and assignment operator
    AtlasTexture2D(const AtlasTexture2D&) = default;
    AtlasTexture2D& operator=(const AtlasTexture2D&) = default;


    //! @brief Create a uninitialized texture (use SetData to add texture data to it)
    AtlasTexture2D() = default;

    //! @brief Create a initialized texture
    AtlasTexture2D(const BaseTexture2D& texAtlas, const AtlasTextureInfo& info);

    ~AtlasTexture2D();

    //! @brief Check if this contains a valid texture.
    bool IsValid() const noexcept;

    //! @brief Get the texture size of the atlas.
    PxSize2D GetAtlasSize() const noexcept
    {
      return m_atlas.GetSize();
    }

    //! @brief Get the virtual texture size.
    //! @note Not valid entries will be zero size
    PxSize2D GetSize() const noexcept;

    PxExtent2D GetExtent() const noexcept
    {
      return m_info.ExtentPx;
    }

    //! @brief Get the virtual texture size.
    const AtlasTextureInfo& GetInfo() const noexcept
    {
      return m_info;
    }

    //! @brief Acquire the native texture (returns null if none exist)
    std::shared_ptr<INativeTexture2D> TryGetNative() const;

    const INativeTexture2D* TryGetNativePointer() const
    {
      return m_atlas.TryGetNativePointer();
    }

    const BaseTexture2D& GetAtlasTexture() const
    {
      return m_atlas;
    }

    void Reset() noexcept;
    void Reset(const BaseTexture2D& texAtlas, const AtlasTextureInfo& info);

    bool operator==(const AtlasTexture2D& rhs) const
    {
      return m_atlas == rhs.m_atlas && m_info == rhs.m_info;
    }
    bool operator!=(const AtlasTexture2D& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
