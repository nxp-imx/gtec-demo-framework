#ifndef FSLGRAPHICS_TEXTURE_TEXTUREBLOBBUILDER_HPP
#define FSLGRAPHICS_TEXTURE_TEXTUREBLOBBUILDER_HPP
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

#include <FslBase/BlobRecord.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Texture/TextureInfo.hpp>
#include <FslGraphics/Texture/TextureType.hpp>
#include <vector>

namespace Fsl
{

  //! EARLY ACCESS: Early design of a builder for the general texture container
  class TextureBlobBuilder
  {
    TextureType m_textureType;
    Extent3D m_extent;
    PixelFormat m_pixelFormat;
    TextureInfo m_textureInfo;
    BitmapOrigin m_bitmapOrigin;
    std::vector<BlobRecord> m_blobs;
    uint32_t m_totalTexels;
    std::size_t m_contentSize;
  public:
    // move assignment operator
    TextureBlobBuilder& operator=(TextureBlobBuilder&& other);
    // move constructor
    TextureBlobBuilder(TextureBlobBuilder&& other);

    // Request that the compiler generates a copy constructor and assignment operator
    TextureBlobBuilder(const TextureBlobBuilder&) = default;
    TextureBlobBuilder& operator= (const TextureBlobBuilder&) = default;

    TextureBlobBuilder();
    TextureBlobBuilder(const TextureType textureType, const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin);
    TextureBlobBuilder(const TextureType textureType, const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const std::size_t contentSize);

    void Reset();
    void Reset(const TextureType textureType, const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin);
    void Reset(const TextureType textureType, const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const std::size_t contentSize);

    BlobRecord GetBlob(const uint32_t level, const uint32_t face, const uint32_t layer) const;
    void SetBlob(const BlobRecord& record, const uint32_t level, const uint32_t face, const uint32_t layer);

    //! @brief Set the default blob layout
    //! @note this only works on uncompressed formats
    void SetDefaultBlobLayout();

    //! @brief Get the bitmap origin
    BitmapOrigin GetBitmapOrigin() const
    {
      return m_bitmapOrigin;
    }

    std::size_t GetContentSize() const
    {
      return m_contentSize;
    }

    //! @brief The extent of the texture
    Extent3D GetExtent(const uint32_t level = 0) const;

    //! @brief The total number of faces (1 for normal textures, 6 for cube maps)
    uint32_t GetFaces() const
    {
      return m_textureInfo.Faces;
    }

    //! @brief The number of array layers (for texture arrays, will be zero for normal textures)
    uint32_t GetLayers() const
    {
      return m_textureInfo.Layers;
    }

    //! @brief The number of levels (commonly used for mip-maps)
    uint32_t GetLevels() const
    {
      return m_textureInfo.Levels;
    }

    //! @brief Get the pixel format
    PixelFormat GetPixelFormat() const
    {
      return m_pixelFormat;
    }

    //! @brief Get the pixel format layout
    PixelFormatLayout GetPixelFormatLayout() const;


    TextureInfo GetTextureInfo() const
    {
      return m_textureInfo;
    }

    //! @brief Get the type of the texture
    TextureType GetTextureType() const
    {
      return m_textureType;
    }

    //! @brief Returns the total number of texels (for all faces, levels, layers, etc)
    uint32_t GetTotalTexelCount() const
    {
      return m_totalTexels;
    }

    std::size_t GetBlobCount() const
    {
      return m_blobs.size();
    }

    BlobRecord GetBlobByIndex(const std::size_t index) const;

    bool IsValid() const
    {
      return m_textureType != TextureType::Undefined;
    }

  private:
    void DoReset(const TextureType textureType, const Extent3D& extent, const PixelFormat pixelFormat, const TextureInfo& textureInfo, const BitmapOrigin bitmapOrigin, const bool hasContentSize, const std::size_t contentSize);
    void ResetNoThrow();
  };
}

#endif
