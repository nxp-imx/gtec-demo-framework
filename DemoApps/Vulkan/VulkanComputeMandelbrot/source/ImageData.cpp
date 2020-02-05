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

#include "ImageData.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslUtil/Vulkan1_0/Util/ConvertUtil.hpp>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  // move assignment operator
  ImageData& ImageData::operator=(ImageData&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      Reset();

      // Claim ownership here
      m_data = std::move(other.m_data);
      m_scratchpadAllOffsets = std::move(other.m_scratchpadAllOffsets);
      m_extent3D = other.m_extent3D;
      m_imageType = other.m_imageType;
      m_format = other.m_format;
      m_mipLevels = other.m_mipLevels;
      m_arrayLayers = other.m_arrayLayers;

      // Remove the data from other
      other.m_extent3D = VkExtent3D{};
      other.m_imageType = VK_IMAGE_TYPE_1D;
      other.m_format = VK_FORMAT_UNDEFINED;
      other.m_mipLevels = 0;
      other.m_arrayLayers = 0;
    }
    return *this;
  }


  // Transfer ownership from other to this
  ImageData::ImageData(ImageData&& other) noexcept
    : m_data(std::move(other.m_data))
    , m_scratchpadAllOffsets(std::move(other.m_scratchpadAllOffsets))
    , m_extent3D(other.m_extent3D)
    , m_imageType(other.m_imageType)
    , m_format(other.m_format)
    , m_mipLevels(other.m_mipLevels)
    , m_arrayLayers(other.m_arrayLayers)
  {
    // Remove the data from other
    other.m_extent3D = VkExtent3D{};
    other.m_imageType = VK_IMAGE_TYPE_1D;
    other.m_format = VK_FORMAT_UNDEFINED;
    other.m_mipLevels = 0;
    other.m_arrayLayers = 0;
  }


  ImageData::ImageData()
    : m_extent3D{}
    , m_imageType(VK_IMAGE_TYPE_1D)
    , m_format(VK_FORMAT_UNDEFINED)
    , m_mipLevels(0)
    , m_arrayLayers(0)
  {
  }


  ImageData::ImageData(const uint32_t width, const uint32_t height, const uint32_t depth, const VkImageType imageType, const VkFormat format)
    : m_extent3D{}
    , m_imageType(imageType)
    , m_format(format)
    , m_mipLevels(1)
    , m_arrayLayers(1)
  {
    m_extent3D.width = width;
    m_extent3D.height = height;
    m_extent3D.depth = depth;

    m_extent3D.depth = depth;

    // One format hack
    assert(format == VK_FORMAT_R8G8B8A8_UNORM);

    m_bytesPerPixel = 4;
    m_data.resize(width * height * depth * m_bytesPerPixel);
  }


  ImageData::~ImageData()
  {
    Reset();
  }


  void ImageData::Reset() noexcept
  {
    if (!IsValid())
    {
      return;
    }

    m_data.resize(0);
    m_scratchpadAllOffsets.resize(0);
    m_extent3D = VkExtent3D{};
    m_imageType = VK_IMAGE_TYPE_1D;
    m_format = VK_FORMAT_UNDEFINED;
    m_mipLevels = 0;
    m_arrayLayers = 0;
  }


  bool ImageData::IsValid() const
  {
    // For now we consider a image with zero bytes per pixel invalid
    return m_bytesPerPixel != 0;
  }


  uint32_t ImageData::GetSize() const
  {
    return static_cast<uint32_t>(m_data.size());
  }


  void ImageData::SetData(const void* const pData, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout)
  {
    if (!IsValid())
    {
      throw UsageErrorException("Can not SetData on a invalid object");
    }

    if ((pData == nullptr) || mipLevel >= m_mipLevels || arrayLayer >= m_arrayLayers)
    {
      throw std::invalid_argument("Invalid arguments");
    }

    // We just do a assert since this should be enough if the object was marked as valid
    assert(m_bytesPerPixel != 0);

    VkExtent3D currentExtent;
    std::size_t dstOffset;
    GetExtentAndOffset(currentExtent, dstOffset, mipLevel, arrayLayer);

    if (subresourceLayout.size > static_cast<VkDeviceSize>(m_data.size() - dstOffset))
    {
      throw std::invalid_argument("subresourceLayout.size does not fit at the resulting offset");
    }

    const uint8_t* const pCurrentSourceBuffer =
      static_cast<const uint8_t*>(pData) + (arrayLayer * subresourceLayout.arrayPitch) + subresourceLayout.offset;
    uint8_t* pDst = m_data.data() + dstOffset;

    const std::size_t bytesPerLine = m_bytesPerPixel * currentExtent.width;

    for (uint32_t z = 0; z < currentExtent.depth; ++z)
    {
      const VkDeviceSize depthOffset = z * subresourceLayout.depthPitch;
      for (uint32_t y = 0; y < currentExtent.height; ++y)
      {
        const uint8_t* pCurrentSourceChannel = &pCurrentSourceBuffer[(y * subresourceLayout.rowPitch) + depthOffset];

        assert(m_data.data() <= pDst);
        assert((m_data.data() + m_data.size()) >= (pDst + bytesPerLine));
        std::memcpy(pDst, pCurrentSourceChannel, bytesPerLine);
        pDst += bytesPerLine;
      }
    }
  }


  void ImageData::GetExtentAndOffset(VkExtent3D& rCurrentExtent, std::size_t& rCurrentOffset, const uint32_t mipLevel,
                                     const uint32_t arrayLayer) const
  {
    if (!TryGetExtentAndOffset(rCurrentExtent, rCurrentOffset, mipLevel, arrayLayer))
    {
      throw std::invalid_argument("GetExtentAndOffset");
    }
  }


  bool ImageData::TryGetExtentAndOffset(VkExtent3D& currentExtent, std::size_t& currentOffset, const uint32_t mipLevel,
                                        const uint32_t arrayLayer) const
  {
    if (mipLevel >= m_mipLevels || arrayLayer >= m_arrayLayers)
    {
      return false;
    }

    // FIX: the m_scratchpadAllOffsets should not be needed here
    if (m_scratchpadAllOffsets.empty())
    {
      std::size_t offset = 0;

      for (uint32_t currentArrayLayer = 0; currentArrayLayer < m_arrayLayers; ++currentArrayLayer)
      {
        currentExtent = m_extent3D;

        for (uint32_t currentMipLevel = 0; currentMipLevel < m_mipLevels; ++currentMipLevel)
        {
          m_scratchpadAllOffsets.push_back(offset);

          currentExtent.width = std::max(m_extent3D.width >> (currentMipLevel), 1u);
          currentExtent.height = std::max(m_extent3D.height >> (currentMipLevel), 1u);
          currentExtent.depth = std::max(m_extent3D.depth >> (currentMipLevel), 1u);

          offset += m_bytesPerPixel * currentExtent.width * currentExtent.height * currentExtent.depth;
        }
      }
    }

    currentExtent.width = std::max(m_extent3D.width >> (mipLevel), 1u);
    currentExtent.height = std::max(m_extent3D.height >> (mipLevel), 1u);
    currentExtent.depth = std::max(m_extent3D.depth >> (mipLevel), 1u);

    currentOffset = m_scratchpadAllOffsets[(arrayLayer * m_mipLevels) + mipLevel];
    return true;
  }

  RawBitmap ImageData::Lock() const
  {
    // FIX: we assume the stride is 'minimal' here.
    // FIX: we currently know this is 2d image data

    const auto pixelFormat = Vulkan::ConvertUtil::Convert(m_format);
    return RawBitmap(m_data.data(), m_extent3D.width, m_extent3D.height, pixelFormat, BitmapOrigin::UpperLeft);
  }

  // RawBitmapEx ImageData::LockEx()
  //{

  //}


  void ImageData::Unlock() const
  {
  }

  // void ImageData::UnlockEx(const RawBitmapEx& bitmap)
  //{
  //}
}
