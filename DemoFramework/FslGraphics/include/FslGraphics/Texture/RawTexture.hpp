#ifndef FSLGRAPHICS_TEXTURE_RAWTEXTURE_HPP
#define FSLGRAPHICS_TEXTURE_RAWTEXTURE_HPP
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
#include <FslBase/BlobRecord.hpp>
#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Texture/TextureInfo.hpp>
#include <FslGraphics/Texture/TextureType.hpp>
#include <cstddef>

namespace Fsl
{
  //! EARLY ACCESS: Early design of a raw texture access container
  class RawTexture
  {
    TextureType m_textureType;
    const void* m_pContent;
    std::size_t m_contentByteSize;
    //! The pointer to the raw blobs, we expect this to be a multidimensional array
    const BlobRecord* m_pBlobs;
    std::size_t m_blobCount;
    Extent3D m_extent;
    PixelFormat m_pixelFormat;
    TextureInfo m_textureInfo;
    BitmapOrigin m_bitmapOrigin;
  public:
    RawTexture();

    //! @param pTextureBlobs in the content. The pointer to the raw texture blobs. See RawTextureBlobHelper::GetBlockIndex for how its accessed.
    //! @param textureBlobCount the number of textureBlobs in pTextureBlobs (we expect this to be equal to faces*leves*layers)
    RawTexture(const TextureType textureType, const void*const pContent, const std::size_t contentByteSize,
               const BlobRecord*const pTextureBlobs, const std::size_t textureBlobCount,
               const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo,
               const BitmapOrigin& bitmapOrigin);

    bool IsValid() const
    {
      return m_pBlobs != nullptr;
    }

    //! @brief Get a raw texture of the given blob of the texture
    BlobRecord GetTextureBlob(const uint32_t level = 0, const uint32_t face = 0, const uint32_t layer = 0) const;


    //! @brief Get the bitmap origin of the texture data
    BitmapOrigin GetBitmapOrigin() const
    {
      return m_bitmapOrigin;
    }


    const void* GetContent() const
    {
      return m_pContent;
    }


    std::size_t GetContentByteSize() const
    {
      return m_contentByteSize;
    }

    //! The stride at the given level (not valid for compressed textures)
    std::size_t GetStride(const std::size_t level = 0) const;

    //! The widtha and height of the texture in pixels
    Extent3D GetExtent(const std::size_t level = 0) const;

    //! @brief Get the number of faces (one for normal textures, six for cube maps)
    uint32_t GetFaces() const
    {
      return m_textureInfo.Faces;
    }

    //! @brief Get the number of array layers (for texture arrays, will be zero for normal textures)
    uint32_t GetLayers() const
    {
      return m_textureInfo.Layers;
    }

    //! @brief The number of mip map levels
    uint32_t GetLevels() const
    {
      return m_textureInfo.Levels;
    }

    TextureInfo GetTextureInfo() const
    {
      return m_textureInfo;
    }

    //! Get the pixel format of the raw bitmap
    PixelFormat GetPixelFormat() const
    {
      return m_pixelFormat;
    }

    //! Get the pixel format of the raw bitmap
    TextureType GetTextureType() const
    {
      return m_textureType;
    }

    BlobRecord GetBlob(const std::size_t index) const;

    std::size_t GetBlobCount() const
    {
      return m_blobCount;
    }
  protected:
    void DoSetBitmapOrigin(const BitmapOrigin& origin)
    {
      m_bitmapOrigin = origin;
    }
  };
}

#endif
