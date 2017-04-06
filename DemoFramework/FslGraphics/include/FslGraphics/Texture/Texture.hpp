#ifndef FSLGRAPHICS_TEXTURE_TEXTURE_HPP
#define FSLGRAPHICS_TEXTURE_TEXTURE_HPP
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
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Extent3D.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/Texture/RawTextureEx.hpp>
#include <FslGraphics/Texture/TextureInfo.hpp>
#include <FslGraphics/Texture/TextureType.hpp>

#include <vector>

namespace Fsl
{
  class TextureBlobBuilder;

  //! EARLY ACCESS: Early design of a general texture container.
  class Texture
  {
    //! The raw image data
    std::vector<uint8_t> m_content;
    std::vector<BlobRecord> m_blobs;
    Extent3D m_extent;
    PixelFormat m_pixelFormat;
    TextureType m_textureType;
    TextureInfo m_textureInfo;
    uint32_t m_totalTexels;
    BitmapOrigin m_bitmapOrigin;
    mutable bool m_isLocked;
  public:
    // move assignment operator
    Texture& operator=(Texture&& other);
    // move constructor
    Texture(Texture&& other);

    // Request that the compiler generates a copy constructor and assignment operator
    Texture(const Texture&) = default;
    Texture& operator= (const Texture&) = default;

    //! @brief Create a invalid texture
    Texture();

    //! @brief Create a empty texture based on the given builder
    Texture(const TextureBlobBuilder& builder);

    //! @brief Create a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    Texture(const void*const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder);

    //! @brief Create a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    Texture(const void*const pContent, const std::size_t contentByteSize, TextureBlobBuilder&& builder);

    ~Texture();

    //! @brief Destroys the texture and resets the object to its default state.
    void Reset();

    //! @brief Destroys the texture and then empty texture based on the given builder
    void Reset(const TextureBlobBuilder& builder);

    //! @brief Destroys the texture and creates a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    void Reset(const void*const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder);

    //! @brief Destroys the texture and creates a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    void Reset(const void*const pContent, const std::size_t contentByteSize, TextureBlobBuilder&& builder);

    //! @brief Check if this object contains a valid resource
    bool IsValid() const
    {
      return m_textureType != TextureType::Undefined;
    }

    //! @brief Get the bitmap origin
    BitmapOrigin GetBitmapOrigin() const;

    //! @brief The extent of the texture
    Extent3D GetExtent(const std::size_t level = 0) const;

    //! @brief The total number of faces (1 for normal textures, 6 for cube maps)
    uint32_t GetFaces() const;

    //! @brief The number of array layers (for texture arrays, will be zero for normal textures)
    uint32_t GetLayers() const;

    //! @brief The number of levels (commonly used for mip-maps)
    uint32_t GetLevels() const;

    //! @brief Get the pixel format
    PixelFormat GetPixelFormat() const;

    //! @brief Get the pixel format layout
    PixelFormatLayout GetPixelFormatLayout() const;

    TextureInfo GetTextureInfo() const
    {
      return m_textureInfo;
    }

    //! @brief Get the type of the texture
    TextureType GetTextureType() const;

    //! @brief Returns the total number of texels (for all faces, levels, layers, etc)
    uint32_t GetTotalTexelCount() const;

    std::size_t GetBlobCount() const;
    BlobRecord GetBlob(const std::size_t index) const;
    BlobRecord GetTextureBlob(const uint32_t level = 0, const uint32_t face = 0, const uint32_t layer = 0) const;

    uint32_t GetByteSize() const;


    //! @brief Swap compatible blobs
    //! @param level = the level to perform the swap at (since it has to be compatible blobs they must reside on the same level)
    //! @param face0 = the face of the first blob
    //! @param layer0 = the layer of the first blob
    //! @param face1 = the face of the second blob
    //! @param layer1 = the layer of the second blob
    void SwapCompatibleBlobs(const uint32_t level, const uint32_t face0, const uint32_t layer0, const uint32_t face1, const uint32_t layer1);

    //! @brief Allows the bitmap pixel format to be modified as long as the pixel formats memory layout is compatible
    //! @note This only changes the 'PixelFormat' type it does not modify any pixels. So if you call this beware of this!
    void SetCompatiblePixelFormat(const PixelFormat compatiblePixelFormat, const bool allowBytePerPixelCompatible=false);

    //! Provides direct access to the texture during its lifetime.
    class ScopedDirectAccess
    {
      const Texture* m_pTexture1;
      Texture* m_pTexture2;
      RawTextureEx* m_pRawTextureEx;
    public:
      ScopedDirectAccess(const ScopedDirectAccess&) = delete;
      ScopedDirectAccess& operator=(const ScopedDirectAccess&) = delete;

      // Read only lock
      ScopedDirectAccess(const Texture& texture, RawTexture& rRawTexture)
        : m_pTexture1(&texture)
        , m_pTexture2(nullptr)
        , m_pRawTextureEx(nullptr)
      {
        rRawTexture = texture.Lock();
      }

      // Read/write lock
      ScopedDirectAccess(Texture& texture, RawTextureEx& rRawTexture)
        : m_pTexture1(nullptr)
        , m_pTexture2(&texture)
        , m_pRawTextureEx(&rRawTexture)
      {
        rRawTexture = texture.LockEx();
      }

      ~ScopedDirectAccess()
      {
        if (m_pTexture1 != nullptr)
          m_pTexture1->Unlock();
        else if (m_pTexture2 != nullptr)
        {
          m_pTexture2->UnlockEx(*m_pRawTextureEx);
        }
      }
    };
  private:
    void DoReset(const void*const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder);

    RawTexture Lock() const;
    RawTextureEx LockEx();
    void UnlockEx(const RawTextureEx& texture);
    void Unlock() const;
    void ResetNoThrow();
  };
}

#endif
