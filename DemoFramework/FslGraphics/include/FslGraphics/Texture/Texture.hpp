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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/Pixel/PxExtent3D.hpp>
#include <FslGraphics/Bitmap/BitmapOrigin.hpp>
#include <FslGraphics/PixelFormat.hpp>
#include <FslGraphics/Texture/RawTexture.hpp>
#include <FslGraphics/Texture/RawTextureEx.hpp>
#include <FslGraphics/Texture/TextureInfo.hpp>
#include <FslGraphics/Texture/TextureType.hpp>
#include <cstdlib>
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
    PxExtent3D m_extent;
    PixelFormat m_pixelFormat{PixelFormat::Undefined};
    TextureType m_textureType{TextureType::Undefined};
    TextureInfo m_textureInfo;
    uint32_t m_totalTexels{0};
    BitmapOrigin m_bitmapOrigin{BitmapOrigin::Undefined};
    mutable bool m_isLocked{false};

  public:
    // move assignment operator
    Texture& operator=(Texture&& other) noexcept;
    // move constructor
    Texture(Texture&& other) noexcept;

    // Request that the compiler generates a copy constructor and assignment operator
    Texture(const Texture&) = default;
    Texture& operator=(const Texture&) = default;

    //! @brief Create a invalid texture
    Texture();

    //! @brief Create a empty texture based on the given builder
    explicit Texture(const TextureBlobBuilder& builder);

    //! @brief Create a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    Texture(const void* const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder);

    //! @brief Create a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    Texture(const void* const pContent, const std::size_t contentByteSize, TextureBlobBuilder&& builder);

    //! @brief Create a texture based on the given builder
    //! @param content a array containing the content
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    Texture(std::vector<uint8_t>&& content, TextureBlobBuilder&& builder);

    //! @brief Create a empty 2D texture of the given extent, pixelFormat and bitmap origin.
    //! @note  This is a easy way to populate this with a empty 2D texture
    Texture(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);

    //! @brief Create a empty 2D texture of the given extent, pixelFormat and bitmap origin.
    //! @note  This is a easy way to populate this with a 2D texture
    Texture(std::vector<uint8_t>&& content, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);


    ~Texture();

    //! @brief Release the internal content array into the supplied vector, then reset this class
    void ReleaseInto(std::vector<uint8_t>& rContentTarget);

    //! @brief Destroys the texture and resets the object to its default state.
    void Reset();

    //! @brief Destroys the texture and then empty texture based on the given builder
    void Reset(const TextureBlobBuilder& builder);

    //! @brief Destroys the texture and creates a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    void Reset(const void* const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder);

    //! @brief Destroys the texture and creates a texture based on the given builder
    //! @param pContent the content
    //! @param contentByteSize the size of the content in bytes (we expect this to match the builder.GetContentSize())
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    void Reset(const void* const pContent, const std::size_t contentByteSize, TextureBlobBuilder&& builder);

    //! @brief Destroys the texture and creates a texture based on the given builder
    //! @param content a array containing the content
    //! @param builder a valid builder with information about the layout of the content in the pContent buffer.
    void Reset(std::vector<uint8_t>&& content, TextureBlobBuilder&& builder);

    //! @brief Destroys the texture and creates a empty 2D texture of the given extent, pixelFormat and bitmap origin.
    //! @note  This is a easy way to populate this with a empty 2D texture
    void Reset(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);

    //! @brief Destroys the texture and creates a empty 2D texture of the given extent, pixelFormat and bitmap origin.
    //! @note  This is a easy way to populate this with a 2D texture
    void Reset(std::vector<uint8_t>&& content, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);

    //! @brief Check if this object contains a valid resource
    bool IsValid() const
    {
      return m_textureType != TextureType::Undefined;
    }

    //! @brief Get the bitmap origin
    BitmapOrigin GetBitmapOrigin() const;

    //! @brief The extent of the texture
    PxExtent3D GetExtent(const std::size_t level = 0) const;

    //! @brief The extent of the texture
    PxExtent2D GetExtent2D(const std::size_t level = 0) const;

    //! @brief The stride of the texture at level
    std::size_t GetStride(const std::size_t level = 0) const;

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

    //! @brief Set the uint8_t at the the given position.
    //! @param x = the x byte position to read (NOT PIXEL).
    //! @param y = the y position (this is equal to a pixel Y position).
    //! @param z = the z position (this is equal to a pixel Z position).
    //! @param value = the byte value to write.
    //! @note Writing outside the bitmap will do nothing (a warning will be logged in debug builds)
    void SetUInt8(const uint32_t level, const uint32_t face, const uint32_t layer, const uint32_t x, const uint32_t y, const uint32_t z,
                  const uint8_t value, const bool ignoreOrigin = false);

    //! @brief read the uint8_t at the the given position.
    //! @param x = the x byte position to read (NOT PIXEL).
    //! @param y = the y position (this is equal to a pixel Y position).
    //! @param z = the z position (this is equal to a pixel Z position).
    //! @note Reading outside the bitmap will do nothing (a warning will be logged in debug builds)
    uint8_t GetUInt8(const uint32_t level, const uint32_t face, const uint32_t layer, const uint32_t x, const uint32_t y, const uint32_t z,
                     const bool ignoreOrigin = false) const;

    //! @brief Swap compatible blobs
    //! @param level = the level to perform the swap at (since it has to be compatible blobs they must reside on the same level)
    //! @param face0 = the face of the first blob
    //! @param layer0 = the layer of the first blob
    //! @param face1 = the face of the second blob
    //! @param layer1 = the layer of the second blob
    void SwapCompatibleBlobs(const uint32_t level, const uint32_t face0, const uint32_t layer0, const uint32_t face1, const uint32_t layer1);

    //! @brief Allows the bitmap pixel format to be modified as long as the pixel formats memory layout is compatible
    //! @note This only changes the 'PixelFormat' type it does not modify any pixels. So if you call this beware of this!
    void SetCompatiblePixelFormat(const PixelFormat compatiblePixelFormat, const bool allowBytePerPixelCompatible = false);

    void ChangeCompatiblePixelFormatFlags(const PixelFormatFlags::Enum flag);

    //! @brief Try to modify the pixel format flag.
    //! @return true if the format was changed or false if it failed
    //! @note This only changes to format flag not the actual data
    bool TrySetCompatiblePixelFormatFlag(const PixelFormatFlags::Enum flag);

    //! @brief This does not change the texture content it just changes the origin reported by this object.
    void OverrideOrigin(const BitmapOrigin bitmapOrigin);

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
        try
        {
          if (m_pTexture1 != nullptr)
          {
            m_pTexture1->Unlock();
          }
          else if (m_pTexture2 != nullptr)
          {
            m_pTexture2->UnlockEx(*m_pRawTextureEx);
          }
        }
        catch (const std::exception&)
        {
          FSLLOG3_ERROR("ScopeDirectAccess unlock failed and destructor can not throw so aborting.");
          std::abort();
        }
      }
    };

  private:
    void DoReset(const void* const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder);
    void DoReset(std::vector<uint8_t>&& content, TextureBlobBuilder&& builder);
    void DoReset(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);
    void DoReset(std::vector<uint8_t>&& content, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin);

    RawTexture Lock() const;
    RawTextureEx LockEx();
    void UnlockEx(const RawTextureEx& texture);
    void Unlock() const;
    void ResetNoThrow();
  };
}

#endif
