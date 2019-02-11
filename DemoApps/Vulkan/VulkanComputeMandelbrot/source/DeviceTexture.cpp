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

#include "DeviceTexture.hpp"
#include <FslBase/Exceptions.hpp>
#include <utility>

namespace Fsl
{
  namespace
  {
    void CmdPipelineBarrier(const VkBuffer srcCmdBuffer, const VkDeviceSize srcSize, const VkAccessFlags srcAccessMask,
                            const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask)
    {
      if (srcAccessMask == dstAccessMask)
      {
        return;
      }

      VkBufferMemoryBarrier bufferMemoryBarrier{};
      bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
      bufferMemoryBarrier.srcAccessMask = srcAccessMask;
      bufferMemoryBarrier.dstAccessMask = dstAccessMask;
      bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      bufferMemoryBarrier.buffer = srcCmdBuffer;
      bufferMemoryBarrier.offset = 0;
      bufferMemoryBarrier.size = srcSize;

      vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0,
                           nullptr);
    }
  }


  // move assignment operator
  DeviceTexture& DeviceTexture::operator=(DeviceTexture&& other) noexcept
  {
    if (this != &other)
    {
      m_image = std::move(other.m_image);
      m_memory = std::move(other.m_memory);
      m_imageFormat = other.m_imageFormat;

      // Remove the data from other
      other.m_imageFormat = VK_FORMAT_UNDEFINED;
    }
    return *this;
  }


  // Transfer ownership from other to this
  DeviceTexture::DeviceTexture(DeviceTexture&& other) noexcept
    : m_image(std::move(other.m_image))
    , m_memory(std::move(other.m_memory))
    , m_imageFormat(other.m_imageFormat)
  {
    // Remove the data from other
    other.m_imageFormat = VK_FORMAT_UNDEFINED;
  }


  DeviceTexture::DeviceTexture()
    : m_imageFormat(VK_FORMAT_UNDEFINED)
  {
  }


  DeviceTexture::DeviceTexture(ImageEx&& image, Vulkan::VUDeviceMemory&& deviceMemory, const VkFormat imageFormat)
    : m_image(std::move(image))
    , m_memory(std::move(deviceMemory))
    , m_imageFormat(imageFormat)
  {
  }


  DeviceTexture::~DeviceTexture() = default;

  void DeviceTexture::Reset() noexcept
  {
    m_image.Reset();
    m_memory.Reset();
    m_imageFormat = VK_FORMAT_UNDEFINED;
  }


  void DeviceTexture::Reset(ImageEx&& image, Vulkan::VUDeviceMemory&& deviceMemory, const VkFormat imageFormat)
  {
    m_image = std::move(image);
    m_memory = std::move(deviceMemory);
    m_imageFormat = imageFormat;
  }


  void DeviceTexture::CopyImageToBuffer(const VkCommandBuffer cmdBuffer, Vulkan::VUBuffer& rTargetBuffer, const VkBufferImageCopy& bufferImageCopy)
  {
    if (!rTargetBuffer.IsValid())
    {
      throw std::invalid_argument("rTargetBuffer must be valid");
    }

    const VkImageLayout sourceImageLayout =
      m_image.GetImageLayout(bufferImageCopy.imageSubresource.mipLevel, bufferImageCopy.imageSubresource.baseArrayLayer);
    const VkAccessFlags sourceAccessMask =
      m_image.GetAccessMask(bufferImageCopy.imageSubresource.mipLevel, bufferImageCopy.imageSubresource.baseArrayLayer);

    // Prepare source image for copy.
    const VkImageSubresourceRange imageSubresourceRange = {bufferImageCopy.imageSubresource.aspectMask, bufferImageCopy.imageSubresource.mipLevel, 1,
                                                           bufferImageCopy.imageSubresource.baseArrayLayer,
                                                           bufferImageCopy.imageSubresource.layerCount};

    m_image.CmdPipelineBarrier(cmdBuffer, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageSubresourceRange);

    // Prepare target buffer for copy.

    const auto tempAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
    CmdPipelineBarrier(rTargetBuffer.Get(), rTargetBuffer.GetSize(), rTargetBuffer.GetAccessMask(), cmdBuffer, tempAccess);

    // Copy image by command.

    vkCmdCopyImageToBuffer(cmdBuffer, m_image.Get(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, rTargetBuffer.Get(), 1, &bufferImageCopy);

    // Revert back.
    CmdPipelineBarrier(rTargetBuffer.Get(), rTargetBuffer.GetSize(), tempAccess, cmdBuffer, rTargetBuffer.GetAccessMask());

    // Revert back.
    m_image.CmdPipelineBarrier(cmdBuffer, sourceAccessMask, sourceImageLayout, imageSubresourceRange);
  }
}
