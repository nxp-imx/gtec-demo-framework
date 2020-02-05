#ifndef VULKAN_VULKANCOMPUTEMANDELBROT_IMAGEDATA_HPP
#define VULKAN_VULKANCOMPUTEMANDELBROT_IMAGEDATA_HPP
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
#include <FslGraphics/Bitmap/RawBitmapEx.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl
{
  class ImageData
  {
    std::vector<uint8_t> m_data;
    mutable std::vector<std::size_t> m_scratchpadAllOffsets;
    VkExtent3D m_extent3D;
    VkImageType m_imageType;
    VkFormat m_format;
    uint32_t m_mipLevels;
    uint32_t m_arrayLayers;
    uint32_t m_tmpSize{};
    uint32_t m_bytesPerPixel{};

  public:
    ImageData(const ImageData&) = delete;
    ImageData& operator=(const ImageData&) = delete;

    // move assignment operator
    ImageData& operator=(ImageData&& other) noexcept;
    // move constructor
    ImageData(ImageData&& other) noexcept;


    ImageData();
    ImageData(const uint32_t width, const uint32_t height, const uint32_t depth, const VkImageType imageType, const VkFormat format);
    ~ImageData();


    void Reset() noexcept;

    bool IsValid() const;

    VkFormat GetPixelFormat() const
    {
      return m_format;
    }

    VkImageType GetImageType() const
    {
      return m_imageType;
    }

    VkExtent3D GetExtent3D() const
    {
      return m_extent3D;
    }

    uint32_t GetMipLevels() const
    {
      return m_mipLevels;
    }

    uint32_t GetSize() const;

    void SetData(const void* const pData, const uint32_t mipLevel, const uint32_t arrayLayer, const VkSubresourceLayout& subresourceLayout);


    void GetExtentAndOffset(VkExtent3D& rCurrentExtent, std::size_t& rCurrentOffset, const uint32_t mipLevel, const uint32_t arrayLayer) const;
    bool TryGetExtentAndOffset(VkExtent3D& currentExtent, std::size_t& currentOffset, const uint32_t mipLevel, const uint32_t arrayLayer) const;


    //! Provides direct access to the scoped bitmap during its lifetime.
    class ScopedRawBitmapAccess
    {
      const ImageData* m_pImageData1;
      // ImageData* m_pImageData2;
      RawBitmapEx* m_pRawBitmapEx;

    public:
      ScopedRawBitmapAccess(const ScopedRawBitmapAccess&) = delete;
      ScopedRawBitmapAccess& operator=(const ScopedRawBitmapAccess&) = delete;

      // Read only lock
      ScopedRawBitmapAccess(const ImageData& imageData, RawBitmap& rRawBitmap)
        : m_pImageData1(&imageData)
        //, m_pImageData2(nullptr)
        , m_pRawBitmapEx(nullptr)
      {
        rRawBitmap = imageData.Lock();
      }

      // Read/write lock
      // ScopedRawBitmapAccess(ImageData& imageData, RawBitmapEx& rRawBitmap)
      //  : m_pImageData1(nullptr)
      //  , m_pImageData2(&imageData)
      //  , m_pRawBitmapEx(&rRawBitmap)
      //{
      //  rRawBitmap = imageData.LockEx();
      //}

      ~ScopedRawBitmapAccess()
      {
        if (m_pImageData1 != nullptr)
        {
          m_pImageData1->Unlock();
        }
        // else if (m_pImageData2 != nullptr)
        //  m_pImageData2->UnlockEx(*m_pRawBitmapEx);
      }
    };

  private:
    RawBitmap Lock() const;
    // RawBitmapEx LockEx();
    void Unlock() const;
    // void UnlockEx(const RawBitmapEx& bitmap);
  };
}

#endif
