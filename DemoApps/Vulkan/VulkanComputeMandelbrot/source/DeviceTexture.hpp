#ifndef VULKAN_VULKANCOMPUTEMANDELBROT_DEVICETEXTURE_HPP
#define VULKAN_VULKANCOMPUTEMANDELBROT_DEVICETEXTURE_HPP
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
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceMemory.hpp>
#include <vulkan/vulkan.h>
#include "ImageEx.hpp"

namespace Fsl
{
  class DeviceTexture
  {
    ImageEx m_image;
    Vulkan::VUDeviceMemory m_memory;
    VkFormat m_imageFormat;

  public:
    DeviceTexture(const DeviceTexture&) = delete;
    DeviceTexture& operator=(const DeviceTexture&) = delete;

    // move assignment operator
    DeviceTexture& operator=(DeviceTexture&& other) noexcept;
    // move constructor
    DeviceTexture(DeviceTexture&& other) noexcept;

    DeviceTexture();
    DeviceTexture(ImageEx&& image, Vulkan::VUDeviceMemory&& deviceMemory, const VkFormat imageFormat);
    ~DeviceTexture();

    void Reset() noexcept;
    void Reset(ImageEx&& image, Vulkan::VUDeviceMemory&& deviceMemory, const VkFormat imageFormat);


    const ImageEx& GetImage() const
    {
      return m_image;
    }

    // FIX: need to decide how we handle this
    ImageEx& GetImage2()
    {
      return m_image;
    }

    const Vulkan::VUDeviceMemory& GetDeviceMemory() const
    {
      return m_memory;
    }


    VkFormat GetImageFormat() const
    {
      return m_imageFormat;
    }


    VkDeviceSize GetAllocationSize() const
    {
      return m_memory.GetAllocationSize();
    }

    VkMemoryPropertyFlags GetMemoryPropertyFlags() const
    {
      return m_memory.GetMemoryPropertyFlags();
    }

    void MapMemory(const VkDeviceSize offset, const VkDeviceSize size, const VkMemoryMapFlags flags)
    {
      m_memory.MapMemory(offset, size, flags);
    }

    const void* GetMappedMemoryPointer() const
    {
      return m_memory.GetMappedMemoryPointer();
    }

    void* GetMappedMemoryPointer()
    {
      return m_memory.GetMappedMemoryPointer();
    }

    void InvalidateMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size)
    {
      return m_memory.InvalidateMappedMemoryRanges(offset, size);
    }


    void UnmapMemory()
    {
      m_memory.UnmapMemory();
    }


    void ImageCmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask, const VkImageLayout newLayout,
                                 const VkImageSubresourceRange& subresourceRange)
    {
      m_image.CmdPipelineBarrier(cmdBuffer, dstAccessMask, newLayout, subresourceRange);
    }

    void CopyImage(const VkCommandBuffer cmdBuffer, ImageEx& rTargetImage, const VkImageCopy& imageCopy)
    {
      m_image.CopyImage(cmdBuffer, rTargetImage, imageCopy);
    }

    void CopyImageToBuffer(const VkCommandBuffer cmdBuffer, Vulkan::VUBuffer& rTargetBuffer, const VkBufferImageCopy& bufferImageCopy);
  };
}

#endif
