#ifndef FSLGRAPHICS_FONT_TEXTUREATLASGLYPHINFO_HPP
#define FSLGRAPHICS_FONT_TEXTUREATLASGLYPHINFO_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslGraphics/Font/FontGlyphBasicKerning.hpp>

namespace Fsl
{
  struct TextureAtlasGlyphInfo
  {
    Rectangle SrcRect;
    FontGlyphBasicKerning Kerning;
    uint8_t TheFlags{0};

    struct Flags
    {
      enum Enum
      {
        Unavailable = 0,

        Available = 0x01,
      };
    };

    //! @brief Check if the available flag is set
    bool IsAvailable() const
    {
      return (TheFlags & Flags::Available) != 0;
    }

    TextureAtlasGlyphInfo() = default;

    TextureAtlasGlyphInfo(const Flags::Enum flags)
      : TheFlags(static_cast<uint8_t>(flags))
    {
    }

    TextureAtlasGlyphInfo(const uint8_t flags)
      : TheFlags(flags)
    {
    }

    TextureAtlasGlyphInfo(const Rectangle& srcRect, const FontGlyphBasicKerning& kerning)
      : SrcRect(srcRect)
      , Kerning(kerning)
      , TheFlags(Flags::Available)
    {
    }

    TextureAtlasGlyphInfo(const Rectangle& srcRect, const FontGlyphBasicKerning& kerning, const Flags::Enum flags)
      : SrcRect(srcRect)
      , Kerning(kerning)
      , TheFlags(static_cast<uint8_t>(flags))
    {
    }

    TextureAtlasGlyphInfo(const Rectangle& srcRect, const FontGlyphBasicKerning& kerning, const uint8_t flags)
      : SrcRect(srcRect)
      , Kerning(kerning)
      , TheFlags(flags)
    {
    }
  };
}

#endif
