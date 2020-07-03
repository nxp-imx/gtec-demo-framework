#ifndef FSLGRAPHICS_RENDER_NINESLICEATLASTEXTURE2D_HPP
#define FSLGRAPHICS_RENDER_NINESLICEATLASTEXTURE2D_HPP
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

#include <FslBase/Math/NineSlice.hpp>
#include <FslGraphics/Render/AtlasTexture2D.hpp>

namespace Fsl
{
  struct NineSliceAtlasTexture2D
  {
    AtlasTexture2D Texture;
    NineSlice NSlice;

    NineSliceAtlasTexture2D() = default;

    explicit NineSliceAtlasTexture2D(const AtlasTexture2D& texture)
      : NineSliceAtlasTexture2D(texture, NineSlice())
    {
    }

    explicit NineSliceAtlasTexture2D(const AtlasTexture2D& texture, const NineSlice& nineSlice)
      : Texture(texture)
      , NSlice(nineSlice)
    {
    }

    bool IsValid() const
    {
      return Texture.IsValid();
    }

    bool operator==(const struct NineSliceAtlasTexture2D& rhs) const
    {
      return Texture == rhs.Texture && NSlice == rhs.NSlice;
    }
    bool operator!=(const NineSliceAtlasTexture2D& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
