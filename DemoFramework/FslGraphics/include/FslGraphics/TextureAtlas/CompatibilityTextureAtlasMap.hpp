#ifndef FSLGRAPHICS_TEXTUREATLAS_COMPATIBILITYTEXTUREATLASMAP_HPP
#define FSLGRAPHICS_TEXTUREATLAS_COMPATIBILITYTEXTUREATLASMAP_HPP
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

#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/ITextureAtlas.hpp>
#include <FslGraphics/TextureAtlas/TextureAtlasMap.hpp>
#include <utility>

namespace Fsl
{
  //! Exists to provide compatibility in places where we need both a TextureAtlasMap and a ITextureAtlas
  class CompatibilityTextureAtlasMap final
    : public TextureAtlasMap
    , public ITextureAtlas
  {
    BasicTextureAtlas m_textureAtlas;

  public:
    CompatibilityTextureAtlasMap(const CompatibilityTextureAtlasMap&) = default;
    CompatibilityTextureAtlasMap& operator=(const CompatibilityTextureAtlasMap&) = default;

    // move assignment operator
    CompatibilityTextureAtlasMap& operator=(CompatibilityTextureAtlasMap&& other) noexcept = default;
    // move constructor
    CompatibilityTextureAtlasMap(CompatibilityTextureAtlasMap&& other) noexcept = default;

    CompatibilityTextureAtlasMap() = default;

    explicit CompatibilityTextureAtlasMap(BasicTextureAtlas textureAtlas)
      : TextureAtlasMap(textureAtlas)
      , m_textureAtlas(std::move(textureAtlas))
    {
    }

    uint32_t Count() const final
    {
      return m_textureAtlas.Count();
    }

    const NamedAtlasTexture& GetEntry(const uint32_t index) const final
    {
      return m_textureAtlas.GetEntry(index);
    }

    uint32_t NineSliceCount() const final
    {
      return m_textureAtlas.NineSliceCount();
    }

    const TextureAtlasNineSlicePatch& GetNineSlicePatch(const uint32_t index) const final
    {
      return m_textureAtlas.GetNineSlicePatch(index);
    }
  };
}
#endif
