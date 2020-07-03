#ifndef FSLGRAPHICS_TEXTUREATLAS_TEXTUREATLASMAP_HPP
#define FSLGRAPHICS_TEXTUREATLAS_TEXTUREATLASMAP_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/IO/Path.hpp>
#include <FslGraphics/TextureAtlas/AtlasNineSlicePatchInfo.hpp>
#include <FslGraphics/TextureAtlas/AtlasTextureInfo.hpp>
#include <functional>
#include <map>

namespace Fsl
{
  class ITextureAtlas;

  class TextureAtlasMap
  {
    std::map<IO::Path, AtlasTextureInfo, std::less<>> m_map;
    std::map<IO::Path, AtlasNineSlicePatchInfo, std::less<>> m_ninesliceMap;

  public:
    TextureAtlasMap(const TextureAtlasMap&) = default;
    TextureAtlasMap& operator=(const TextureAtlasMap&) = default;

    // move assignment operator
    TextureAtlasMap& operator=(TextureAtlasMap&& other) noexcept = default;
    // move constructor
    TextureAtlasMap(TextureAtlasMap&& other) noexcept = default;

    TextureAtlasMap();
    explicit TextureAtlasMap(const ITextureAtlas& atlas);

    //! @brief Get the atlas texture info for the supplied texture
    AtlasTextureInfo GetAtlasTextureInfo(const IO::PathView& name) const;

    //! @brief Get the atlas texture info for the supplied texture
    AtlasTextureInfo GetAtlasTextureInfo(const IO::Path& name) const
    {
      return GetAtlasTextureInfo(name.AsPathView());
    }

    //! @brief Get the atlas texture info for the supplied texture
    AtlasNineSlicePatchInfo GetAtlasNineSlicePatchInfo(const IO::PathView& name) const;

    //! @brief Get the atlas texture info for the supplied texture
    AtlasNineSlicePatchInfo GetAtlasNineSlicePatchInfo(const IO::Path& name) const
    {
      return GetAtlasNineSlicePatchInfo(name.AsPathView());
    }
  };
}
#endif
