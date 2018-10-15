#ifndef FSLGRAPHICS_TEXTURE_RAWTEXTUREEX_HPP
#define FSLGRAPHICS_TEXTURE_RAWTEXTUREEX_HPP
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

#include <FslGraphics/Texture/RawTexture.hpp>

namespace Fsl
{
  //! EARLY ACCESS: Early design of a raw texture access container
  class RawTextureEx : public RawTexture
  {
    void* m_pContent{nullptr};

  public:
    RawTextureEx() = default;

    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    RawTextureEx(const TextureType textureType, void* const pContent, const std::size_t contentByteSize, const BlobRecord* const pTextureBlobs,
                 const std::size_t textureBlobCount, const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
                 const BitmapOrigin& bitmapOrigin)
      : RawTexture(textureType, pContent, contentByteSize, pTextureBlobs, textureBlobCount, extent, pixelFormat, textureInfo, bitmapOrigin)
      , m_pContent(pContent)
    {
    }

    void* GetContentWriteAccess() const
    {
      return m_pContent;
    }

    void SetBitmapOrigin(const BitmapOrigin& origin)
    {
      DoSetBitmapOrigin(origin);
    }
  };
}

#endif
