#ifndef FSLGRAPHICS_TEXTURE_TEXTUREINFO_HPP
#define FSLGRAPHICS_TEXTURE_TEXTUREINFO_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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
#include <cassert>

namespace Fsl
{
  enum class TextureType;

  struct TextureInfo
  {
    uint32_t Levels{0};
    uint32_t Faces{0};
    uint32_t Layers{0};

    TextureInfo() = default;

    TextureInfo(const uint32_t levels, const TextureType& textureType, const uint32_t layers);

    TextureInfo(const uint32_t levels, const uint32_t faces, const uint32_t layers)
      : Levels(levels)
      , Faces(faces)
      , Layers(layers)
    {
      assert(IsValid());
    }

    bool IsValid() const
    {
      return Levels > 0 && Faces > 0 && Layers > 0;
    }

    uint32_t GetBlobCount() const
    {
      return Levels * Faces * Layers;
    }

    //! @brief Helper function for acquiring the right index for a block given levels, faces and layers
    //! @note Prevents code duplication and makes it easier to modify
    inline uint32_t GetBlockIndex(const uint32_t level, const uint32_t face, const uint32_t layer) const
    {
      assert(level <= Levels);
      assert(face <= Faces);
      assert(layer <= Layers);
      const uint32_t strideFaces = Levels;
      const uint32_t strideLayer = (strideFaces * Faces);
      return level + (strideFaces * face) + (strideLayer * layer);
    }


    bool operator==(const TextureInfo& rhs) const
    {
      return Levels == rhs.Levels && Faces == rhs.Faces && Layers == rhs.Layers;
    }

    bool operator!=(const TextureInfo& rhs) const
    {
      return !(*this == rhs);
    }
  };
}

#endif
