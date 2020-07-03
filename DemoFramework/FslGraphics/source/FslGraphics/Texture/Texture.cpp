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

#include <FslGraphics/Texture/Texture.hpp>
#include <FslGraphics/Texture/TextureBlobBuilder.hpp>
#include <FslGraphics/Texture/TextureTypeUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/PixelFormatUtil.hpp>
//#include <FslGraphics/RawTextureUtil.hpp>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <utility>

namespace Fsl
{
  namespace
  {
    constexpr BitmapOrigin CheckBitmapOrigin(const BitmapOrigin origin)
    {
      return origin != BitmapOrigin::Undefined ? origin : BitmapOrigin::UpperLeft;
    }

    inline PxExtent3D CalcExtent(const PxExtent3D& startExtent, const std::size_t level)
    {
      return level == 0 ? startExtent
                        : PxExtent3D(std::max(startExtent.Width >> level, static_cast<uint32_t>(1u)),
                                     std::max(startExtent.Height >> level, static_cast<uint32_t>(1u)),
                                     std::max(startExtent.Depth >> level, static_cast<uint32_t>(1u)));
    }

    //! @brief Returns the total texel count
    uint32_t ValidateBlobs(const std::vector<uint8_t>& srcContent, const std::vector<BlobRecord>& srcBlobs, const PxExtent3D& extent,
                           const PixelFormat pixelFormat, const TextureInfo& textureInfo)
    {
      const bool isCompressed = PixelFormatUtil::IsCompressed(pixelFormat);

      const std::size_t contentByteSize = srcContent.size();

      uint32_t totalTexelCount = 0;
      PxExtent3D currentExtent = extent;
      for (uint32_t level = 0; level < textureInfo.Levels; ++level)
      {
        if (level > 0 && (extent.Width == 0 || extent.Height == 0 || extent.Depth == 0))
        {
          throw std::invalid_argument("The number of levels and the start extend causes zero sized extents at some levels");
        }

        const uint32_t currentTexelCount = (currentExtent.Width * currentExtent.Height * currentExtent.Depth);
        for (uint32_t layer = 0; layer < textureInfo.Layers; ++layer)
        {
          for (uint32_t face = 0; face < textureInfo.Faces; ++face)
          {
            const std::size_t index = textureInfo.GetBlockIndex(level, face, layer);
            auto blob = srcBlobs[index];
            // two checks bypasses any overflow issues
            if (blob.Size > contentByteSize)
            {
              throw std::invalid_argument("Content blob is outside the content buffer");
            }
            if (blob.Offset > (contentByteSize - blob.Size))
            {
              throw std::invalid_argument("Content blob is outside the content buffer");
            }

            if (!isCompressed)
            {
              const auto stride = static_cast<std::size_t>(PixelFormatUtil::CalcMinimumStride(currentExtent.Width, pixelFormat));
              const auto expectedSize = currentExtent.Height * stride;
              if (expectedSize != blob.Size)
              {
                throw std::invalid_argument("Content of the given pixelFormat and extent does not fit the blob size, invalid blob");
              }
            }
            totalTexelCount += currentTexelCount;
          }
        }
        currentExtent.Width = std::max(currentExtent.Width >> 1, static_cast<uint32_t>(1u));
        currentExtent.Height = std::max(currentExtent.Height >> 1, static_cast<uint32_t>(1u));
        currentExtent.Depth = std::max(currentExtent.Depth >> 1, static_cast<uint32_t>(1u));
      }
      return totalTexelCount;
    }

    // Dummy area we use to get a content pointer for zero sized bitmaps size the vector data methods returns a nullptr
    uint32_t g_dummyAreaForZeroSizedBitmaps = 0;
  }

  // move assignment operator
  Texture& Texture::operator=(Texture&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      ResetNoThrow();

      // Claim ownership here
      m_content = std::move(other.m_content);
      m_blobs = std::move(other.m_blobs);
      m_extent = other.m_extent;
      m_pixelFormat = other.m_pixelFormat;
      m_textureType = other.m_textureType;
      m_textureInfo = other.m_textureInfo;
      m_totalTexels = other.m_totalTexels;
      m_bitmapOrigin = other.m_bitmapOrigin;
      m_isLocked = other.m_isLocked;

      // Remove the data from other
      other.m_extent = PxExtent3D();
      other.m_pixelFormat = PixelFormat::Undefined;
      other.m_textureType = TextureType::Undefined;
      other.m_textureInfo = TextureInfo();
      other.m_totalTexels = 0;
      other.m_bitmapOrigin = BitmapOrigin::Undefined;
      other.m_isLocked = false;
    }
    return *this;
  }


  // Transfer ownership from other to this
  Texture::Texture(Texture&& other) noexcept
    : m_content(std::move(other.m_content))
    , m_blobs(std::move(other.m_blobs))
    , m_extent(other.m_extent)
    , m_pixelFormat(other.m_pixelFormat)
    , m_textureType(other.m_textureType)
    , m_textureInfo(other.m_textureInfo)
    , m_totalTexels(other.m_totalTexels)
    , m_bitmapOrigin(other.m_bitmapOrigin)
    , m_isLocked(other.m_isLocked)
  {
    // Remove the data from other
    other.m_extent = PxExtent3D();
    other.m_pixelFormat = PixelFormat::Undefined;
    other.m_textureType = TextureType::Undefined;
    other.m_textureInfo = TextureInfo();
    other.m_totalTexels = 0;
    other.m_bitmapOrigin = BitmapOrigin::Undefined;
    other.m_isLocked = false;
  }


  Texture::Texture() = default;


  Texture::Texture(const TextureBlobBuilder& builder)
    : Texture()
  {
    Reset(builder);
  }


  Texture::Texture(const void* const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder)
    : Texture()
  {
    Reset(pContent, contentByteSize, builder);
  }


  Texture::Texture(const void* const pContent, const std::size_t contentByteSize, TextureBlobBuilder&& builder)
    : Texture()
  {
    Reset(pContent, contentByteSize, std::move(builder));
  }


  Texture::Texture(std::vector<uint8_t>&& content, TextureBlobBuilder&& builder)
    : Texture()
  {
    Reset(std::move(content), std::move(builder));
  }


  Texture::Texture(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : Texture()
  {
    DoReset(extent, pixelFormat, origin);
  }


  Texture::Texture(std::vector<uint8_t>&& content, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
    : Texture()
  {
    DoReset(std::move(content), extent, pixelFormat, origin);
  }


  Texture::~Texture()
  {
    FSLLOG3_WARNING_IF(m_isLocked, "Destroying a locked texture, the content being accessed will no longer be available");
  }


  void Texture::ReleaseInto(std::vector<uint8_t>& rContentTarget)
  {
    // Reset() should not throw, but this warrants a program stop since its a critical error
    if (m_isLocked)
    {
      throw UsageErrorException("Can not release a locked texture, that would invalidate the content being accessed");
    }

    // Get the current content array, then reset this object
    rContentTarget = std::move(m_content);

    ResetNoThrow();
  }


  void Texture::Reset()
  {
    // Reset() should not throw, but this warrants a program stop since its a critical error
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    ResetNoThrow();
  }


  void Texture::Reset(const TextureBlobBuilder& builder)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    if (!builder.IsValid())
    {
      throw std::invalid_argument("build can not be invalid");
    }

    DoReset(nullptr, 0, builder);
  }


  void Texture::Reset(const void* const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    if (pContent == nullptr)
    {
      throw NotSupportedException("The pContent buffer can not be null");
    }
    if (!builder.IsValid())
    {
      throw std::invalid_argument("build can not be invalid");
    }
    if (contentByteSize != builder.GetContentSize())
    {
      throw NotSupportedException("the builder content size did not match the buffer size");
    }

    DoReset(pContent, contentByteSize, builder);
  }


  void Texture::Reset(const void* const pContent, const std::size_t contentByteSize, TextureBlobBuilder&& builder)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    // TODO: claim the vector from the builder to prevent unnecessary allocations
    Reset(pContent, contentByteSize, builder);
  }


  void Texture::Reset(std::vector<uint8_t>&& content, TextureBlobBuilder&& builder)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    if (!builder.IsValid())
    {
      throw std::invalid_argument("build can not be invalid");
    }
    if (content.size() != builder.GetContentSize())
    {
      throw NotSupportedException("the builder content size did not match the buffer size");
    }

    DoReset(std::move(content), std::move(builder));
  }


  void Texture::Reset(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    DoReset(extent, pixelFormat, origin);
  }


  void Texture::Reset(std::vector<uint8_t>&& content, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not reset a locked texture, that would invalidate the content being accessed");
    }

    DoReset(std::move(content), extent, pixelFormat, origin);
  }


  BitmapOrigin Texture::GetBitmapOrigin() const
  {
    return m_bitmapOrigin;
  }


  uint32_t Texture::GetFaces() const
  {
    return m_textureInfo.Faces;
  }


  PxExtent3D Texture::GetExtent(const std::size_t level) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetExtent called on invalid object");
    }
    if (level >= m_textureInfo.Levels)
    {
      throw std::invalid_argument("argument out of bounds");
    }

    return CalcExtent(m_extent, level);
  }

  PxExtent2D Texture::GetExtent2D(const std::size_t level) const
  {
    auto res = GetExtent(level);
    if (res.Depth != 1u)
    {
      throw UsageErrorException("GetExtent2D called on a non 2d texture");
    }
    return {res.Width, res.Height};
  }


  std::size_t Texture::GetStride(const std::size_t level) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetExtent called on invalid object");
    }
    if (level >= m_textureInfo.Levels)
    {
      throw std::invalid_argument("argument out of bounds");
    }

    const PxExtent3D levelExtent = CalcExtent(m_extent, level);
    return PixelFormatUtil::CalcMinimumStride(levelExtent.Width, m_pixelFormat);
  }


  uint32_t Texture::GetLayers() const
  {
    return m_textureInfo.Layers;
  }


  uint32_t Texture::GetLevels() const
  {
    return m_textureInfo.Levels;
  }


  PixelFormat Texture::GetPixelFormat() const
  {
    return m_pixelFormat;
  }


  PixelFormatLayout Texture::GetPixelFormatLayout() const
  {
    return PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat);
  }


  TextureType Texture::GetTextureType() const
  {
    return m_textureType;
  }


  uint32_t Texture::GetTotalTexelCount() const
  {
    return m_totalTexels;
  }


  std::size_t Texture::GetBlobCount() const
  {
    return m_blobs.size();
  }


  BlobRecord Texture::GetBlob(const std::size_t index) const
  {
    if (!IsValid())
    {
      throw UsageErrorException("GetBlobs() called on invalid Texture");
    }
    if (index >= m_blobs.size())
    {
      throw UsageErrorException("index out of bounds");
    }
    return m_blobs[index];
  }


  BlobRecord Texture::GetTextureBlob(const uint32_t level, const uint32_t face, const uint32_t layer) const
  {
    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers)
    {
      throw std::invalid_argument("out of bounds");
    }

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);
    return m_blobs[index];
  }


  uint32_t Texture::GetByteSize() const
  {
    assert(m_content.size() <= std::numeric_limits<uint32_t>::max());
    return static_cast<uint32_t>(m_content.size());
  }


  void Texture::SetUInt8(const uint32_t level, const uint32_t face, const uint32_t layer, const uint32_t x, const uint32_t y, const uint32_t z,
                         const uint8_t value, const bool ignoreOrigin)
  {
    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers || PixelFormatUtil::IsCompressed(m_pixelFormat))
    {
      FSLLOG3_DEBUG_WARNING_IF(level >= m_textureInfo.Levels, "level is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(face >= m_textureInfo.Faces, "face is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(layer >= m_textureInfo.Layers, "layer is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(PixelFormatUtil::IsCompressed(m_pixelFormat), "Cant modify a compressed pixel format");
      return;
    }
    const auto bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(m_pixelFormat);
    const PxExtent3D levelExtent = CalcExtent(m_extent, level);
    const auto levelWidthInBytes = (levelExtent.Width * bytesPerPixel);
    if (x >= levelWidthInBytes || y >= levelExtent.Height || z >= levelExtent.Depth)
    {
      FSLLOG3_DEBUG_WARNING_IF(x >= levelWidthInBytes, "x is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(y >= levelExtent.Height, "y is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(z >= levelExtent.Depth, "z is out of bounds");
      return;
    }

    const uint32_t actualY = ignoreOrigin ? y : (m_bitmapOrigin == BitmapOrigin::UpperLeft ? y : levelExtent.Height - 1 - y);

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);
    const auto& rBlobRecord = m_blobs[index];

    const std::size_t stride = PixelFormatUtil::CalcMinimumStride(levelExtent.Width, bytesPerPixel);

    // slizeSize is the size of one 2d image.
    const auto sliceSize = levelExtent.Height * stride;

    m_content[rBlobRecord.Offset + (sliceSize * z) + (stride * actualY) + x] = value;
  }


  uint8_t Texture::GetUInt8(const uint32_t level, const uint32_t face, const uint32_t layer, const uint32_t x, const uint32_t y, const uint32_t z,
                            const bool ignoreOrigin) const
  {
    if (level >= m_textureInfo.Levels || face >= m_textureInfo.Faces || layer >= m_textureInfo.Layers || PixelFormatUtil::IsCompressed(m_pixelFormat))
    {
      FSLLOG3_DEBUG_WARNING_IF(level >= m_textureInfo.Levels, "level is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(face >= m_textureInfo.Faces, "face is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(layer >= m_textureInfo.Layers, "layer is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(PixelFormatUtil::IsCompressed(m_pixelFormat), "Cant read from a compressed pixel format");
      return 0;
    }
    const auto bytesPerPixel = PixelFormatUtil::GetBytesPerPixel(m_pixelFormat);
    const PxExtent3D levelExtent = CalcExtent(m_extent, level);
    const auto levelWidthInBytes = (levelExtent.Width * bytesPerPixel);
    if (x >= levelWidthInBytes || y >= levelExtent.Height || z >= levelExtent.Depth)
    {
      FSLLOG3_DEBUG_WARNING_IF(x >= levelWidthInBytes, "x is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(y >= levelExtent.Height, "y is out of bounds");
      FSLLOG3_DEBUG_WARNING_IF(z >= levelExtent.Depth, "z is out of bounds");
      return 0;
    }

    const uint32_t actualY = ignoreOrigin ? y : (m_bitmapOrigin == BitmapOrigin::UpperLeft ? y : levelExtent.Height - 1 - y);

    const std::size_t index = m_textureInfo.GetBlockIndex(level, face, layer);
    const auto& rBlobRecord = m_blobs[index];

    const std::size_t stride = PixelFormatUtil::CalcMinimumStride(levelExtent.Width, bytesPerPixel);

    // slizeSize is the size of one 2d image.
    const auto sliceSize = levelExtent.Height * stride;

    return m_content[rBlobRecord.Offset + (sliceSize * z) + (stride * actualY) + x];
  }


  void Texture::SwapCompatibleBlobs(const uint32_t level, const uint32_t face0, const uint32_t layer0, const uint32_t face1, const uint32_t layer1)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("Can not SwapCompatibleBlobs a locked texture, that would invalidate the content being accessed");
    }

    if (level >= m_textureInfo.Levels || face0 >= m_textureInfo.Faces || layer0 >= m_textureInfo.Layers || face1 >= m_textureInfo.Faces ||
        layer1 >= m_textureInfo.Layers)
    {
      throw std::invalid_argument("out of bounds");
    }

    const std::size_t index0 = m_textureInfo.GetBlockIndex(level, face0, layer0);
    const std::size_t index1 = m_textureInfo.GetBlockIndex(level, face1, layer1);
    if (index0 == index1)
    {
      return;
    }

    std::swap(m_blobs[index0], m_blobs[index1]);
  }


  void Texture::SetCompatiblePixelFormat(const PixelFormat compatiblePixelFormat, const bool allowBytePerPixelCompatible)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("The texture is already locked");
    }

    if (allowBytePerPixelCompatible && PixelFormatUtil::GetBytesPerPixel(m_pixelFormat) != PixelFormatUtil::GetBytesPerPixel(compatiblePixelFormat))
    {
      throw UsageErrorException("The supplied pixel format was not byte per pixel compatible");
    }

    if (!allowBytePerPixelCompatible &&
        PixelFormatUtil::GetPixelFormatLayout(m_pixelFormat) != PixelFormatUtil::GetPixelFormatLayout(compatiblePixelFormat))
    {
      throw UsageErrorException("The supplied pixel format was not compatible");
    }

    m_pixelFormat = compatiblePixelFormat;
  }

  void Texture::ChangeCompatiblePixelFormatFlags(const PixelFormatFlags::Enum flag)
  {
    auto newFormat = PixelFormatUtil::TrySetCompatiblePixelFormatFlag(m_pixelFormat, flag);
    if (newFormat == PixelFormat::Undefined)
    {
      throw NotSupportedException("Could not change pixel format flags");
    }
    m_pixelFormat = newFormat;
  }


  bool Texture::TrySetCompatiblePixelFormatFlag(const PixelFormatFlags::Enum flag)
  {
    if (m_isLocked)
    {
      FSLLOG3_DEBUG_WARNING("The texture is already locked");
      return false;
    }

    const auto newPixelFormat = PixelFormatUtil::TrySetCompatiblePixelFormatFlag(m_pixelFormat, flag);
    if (newPixelFormat == PixelFormat::Undefined)
    {
      return false;
    }
    m_pixelFormat = newPixelFormat;
    return true;
  }

  void Texture::OverrideOrigin(const BitmapOrigin bitmapOrigin)
  {
    if (m_isLocked)
    {
      throw UsageErrorException("The texture is already locked");
    }
    if (bitmapOrigin == BitmapOrigin::Undefined)
    {
      throw std::invalid_argument("bitmapOrigin can not be undefined");
    }

    m_bitmapOrigin = bitmapOrigin;
  }


  void Texture::DoReset(const void* const pContent, const std::size_t contentByteSize, const TextureBlobBuilder& builder)
  {
    // If any of these fire the builder did not keep its contract or
    // we forgot to validate a input parameter somewhere
    assert(!m_isLocked);
    assert(builder.IsValid());
    assert(builder.GetTextureType() != TextureType::Undefined);
    assert(builder.GetPixelFormat() != PixelFormat::Undefined);
    assert(builder.GetBitmapOrigin() != BitmapOrigin::Undefined);
    assert(builder.GetLevels() >= 1);
    assert(builder.GetFaces() == TextureTypeUtil::GetFaceCount(builder.GetTextureType()));
    assert(builder.GetLayers() >= 1);
    assert(builder.GetBlobCount() == (builder.GetLevels() * builder.GetFaces() * builder.GetLayers()));
    assert(pContent == nullptr || contentByteSize == builder.GetContentSize());

    if (IsValid())
    {
      Reset();
    }

    try
    {
      const auto blobCount = builder.GetBlobCount();
      m_content.resize(builder.GetContentSize());
      m_blobs.resize(blobCount);

      for (std::size_t i = 0; i < blobCount; ++i)
      {
        m_blobs[i] = builder.GetBlobByIndex(i);
      }

      const uint32_t totalTexels = ValidateBlobs(m_content, m_blobs, builder.GetExtent(), builder.GetPixelFormat(), builder.GetTextureInfo());
      if (totalTexels != builder.GetTotalTexelCount())
      {
        throw std::invalid_argument("The builder does not contain the expected amount of texels in the blobs");
      }
    }
    catch (const std::exception&)
    {
      m_content.clear();
      m_blobs.clear();
      throw;
    }

    // Just copy the data old school style
    if (pContent != nullptr && contentByteSize > 0)
    {
      std::memcpy(m_content.data(), pContent, contentByteSize);
    }
    m_extent = builder.GetExtent();
    m_pixelFormat = builder.GetPixelFormat();
    m_textureType = builder.GetTextureType();
    m_textureInfo = builder.GetTextureInfo();
    m_totalTexels = builder.GetTotalTexelCount();
    m_bitmapOrigin = CheckBitmapOrigin(builder.GetBitmapOrigin());
  }


  void Texture::DoReset(std::vector<uint8_t>&& content, TextureBlobBuilder&& builder)
  {
    // If any of these fire the builder did not keep its contract or
    // we forgot to validate a input parameter somewhere
    assert(!m_isLocked);
    assert(builder.IsValid());
    assert(builder.GetTextureType() != TextureType::Undefined);
    assert(builder.GetPixelFormat() != PixelFormat::Undefined);
    assert(builder.GetBitmapOrigin() != BitmapOrigin::Undefined);
    assert(builder.GetLevels() >= 1);
    assert(builder.GetFaces() == TextureTypeUtil::GetFaceCount(builder.GetTextureType()));
    assert(builder.GetLayers() >= 1);
    assert(builder.GetBlobCount() == (builder.GetLevels() * builder.GetFaces() * builder.GetLayers()));
    assert(content.size() == builder.GetContentSize());

    if (IsValid())
    {
      Reset();
    }

    try
    {
      const auto blobCount = builder.GetBlobCount();
      m_content = std::move(content);
      m_blobs.resize(blobCount);

      for (std::size_t i = 0; i < blobCount; ++i)
      {
        m_blobs[i] = builder.GetBlobByIndex(i);
      }

      const uint32_t totalTexels = ValidateBlobs(m_content, m_blobs, builder.GetExtent(), builder.GetPixelFormat(), builder.GetTextureInfo());
      if (totalTexels != builder.GetTotalTexelCount())
      {
        throw std::invalid_argument("The builder does not contain the expected amount of texels in the blobs");
      }
    }
    catch (const std::exception&)
    {
      m_content.clear();
      m_blobs.clear();
      throw;
    }

    // We don't copy the content here size we 'moved' the source into this object
    m_extent = builder.GetExtent();
    m_pixelFormat = builder.GetPixelFormat();
    m_textureType = builder.GetTextureType();
    m_textureInfo = builder.GetTextureInfo();
    m_totalTexels = builder.GetTotalTexelCount();
    m_bitmapOrigin = CheckBitmapOrigin(builder.GetBitmapOrigin());
  }


  void Texture::DoReset(const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    // If any of these fire the caller did not keep its contract.
    assert(!m_isLocked);
    if (IsValid())
    {
      Reset();
    }

    try
    {
      const std::size_t minStride = PixelFormatUtil::CalcMinimumStride(extent.Width, pixelFormat);
      const std::size_t totalByteSize = (extent.Height * minStride);

      m_content.resize(totalByteSize);
      m_blobs.resize(1);
      m_blobs[0].Offset = 0;
      m_blobs[0].Size = totalByteSize;

      std::fill(m_content.begin(), m_content.end(), static_cast<uint8_t>(0));
    }
    catch (const std::exception&)
    {
      m_content.clear();
      m_blobs.clear();
      throw;
    }

    // We don't copy the content here size we 'moved' the source into this object
    m_extent = PxExtent3D(extent.Width, extent.Height, 1u);
    m_pixelFormat = pixelFormat;
    m_textureType = TextureType::Tex2D;
    m_textureInfo = TextureInfo(1, 1, 1);
    m_totalTexels = extent.Width * extent.Height;
    m_bitmapOrigin = CheckBitmapOrigin(origin);
  }


  void Texture::DoReset(std::vector<uint8_t>&& content, const PxExtent2D& extent, const PixelFormat pixelFormat, const BitmapOrigin origin)
  {
    // If any of these fire the caller did not keep its contract.
    assert(!m_isLocked);
    if (IsValid())
    {
      Reset();
    }

    const std::size_t minStride = PixelFormatUtil::CalcMinimumStride(extent.Width, pixelFormat);
    const std::size_t totalByteSize = (extent.Height * minStride);
    if (content.size() != totalByteSize)
    {
      throw std::invalid_argument("the content buffer size is does not match the described content");
    }

    try
    {
      m_content = std::move(content);
      m_blobs.resize(1);
      m_blobs[0].Offset = 0;
      m_blobs[0].Size = totalByteSize;
    }
    catch (const std::exception&)
    {
      m_content.clear();
      m_blobs.clear();
      throw;
    }

    // We don't copy the content here size we 'moved' the source into this object
    m_extent = PxExtent3D(extent.Width, extent.Height, 1u);
    m_pixelFormat = pixelFormat;
    m_textureType = TextureType::Tex2D;
    m_textureInfo = TextureInfo(1, 1, 1);
    m_totalTexels = extent.Width * extent.Height;
    m_bitmapOrigin = CheckBitmapOrigin(origin);
  }


  RawTexture Texture::Lock() const
  {
    if (m_isLocked)
    {
      throw UsageErrorException("The texture is already locked");
    }
    m_isLocked = true;

    const auto* const pData = m_content.data();
    if (pData != nullptr)
    {
      return {m_textureType, m_content.data(), m_content.size(), m_blobs.data(), m_blobs.size(),
              m_extent,      m_pixelFormat,    m_textureInfo,    m_bitmapOrigin};
    }
    assert((m_extent.Width * m_extent.Height * m_extent.Depth) == 0u);
    return {m_textureType, &g_dummyAreaForZeroSizedBitmaps, 0u, m_blobs.data(), m_blobs.size(), m_extent, m_pixelFormat, m_textureInfo,
            m_bitmapOrigin};
  }


  RawTextureEx Texture::LockEx()
  {
    if (m_isLocked)
    {
      throw UsageErrorException("The texture is already locked");
    }

    m_isLocked = true;
    auto* pData = m_content.data();
    if (pData != nullptr)
    {
      return {m_textureType, m_content.data(), m_content.size(), m_blobs.data(), m_blobs.size(),
              m_extent,      m_pixelFormat,    m_textureInfo,    m_bitmapOrigin};
    }
    assert((m_extent.Width * m_extent.Height * m_extent.Depth) == 0u);
    return {m_textureType, &g_dummyAreaForZeroSizedBitmaps, m_content.size(), m_blobs.data(), m_blobs.size(), m_extent, m_pixelFormat, m_textureInfo,
            m_bitmapOrigin};
  }


  void Texture::UnlockEx(const RawTextureEx& texture)
  {
    if (!m_isLocked)
    {
      throw UsageErrorException("The texture is not locked");
    }

    BitmapOrigin currentOrigin = texture.GetBitmapOrigin();
    if (currentOrigin != m_bitmapOrigin)
    {
      m_bitmapOrigin = CheckBitmapOrigin(currentOrigin);
    }

    m_isLocked = false;
  }


  void Texture::Unlock() const
  {
    if (!m_isLocked)
    {
      throw UsageErrorException("The texture is not locked");
    }
    m_isLocked = false;
  }


  void Texture::ResetNoThrow()
  {
    FSLLOG3_WARNING_IF(m_isLocked, "Destroying a locked texture, the content being accessed will no longer be available");
    m_content.clear();
    m_blobs.clear();
    m_extent = PxExtent3D();
    m_pixelFormat = PixelFormat::Undefined;
    m_textureType = TextureType::Undefined;
    m_textureInfo = TextureInfo();
    m_totalTexels = 0;
    m_bitmapOrigin = BitmapOrigin::Undefined;
    m_isLocked = false;
  }
}
