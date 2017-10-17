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

#include <FslUtil/Vulkan1_0/Extend/BufferEx.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    // move assignment operator
    BufferEx& BufferEx::operator=(BufferEx&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_buffer = std::move(other.m_buffer);
        //m_createInfo = std::move(other.m_createInfo);
        m_size = other.m_size;
        m_usage = other.m_usage;
        m_accessMask = other.m_accessMask;

        // Remove the data from other
        other.m_accessMask = 0;
        other.m_size = 0;
        other.m_usage = 0;
      }
      return *this;
    }


    // Transfer ownership from other to this
    BufferEx::BufferEx(BufferEx&& other)
      : m_buffer(std::move(other.m_buffer))
      //, m_createInfo(std::move(other.m_createInfo))
      , m_size(other.m_size)
      , m_usage(other.m_usage)
      , m_accessMask(other.m_accessMask)
    {
      // Remove the data from other
      other.m_accessMask = 0;
      other.m_size = 0;
      other.m_usage = 0;
    }


    // For now we implement the code here, if this turns out to be a performance problem we can move the code
    // to the header file to ensure its inlined.
    BufferEx::BufferEx()
      : m_buffer()
      //, m_createInfo()
      , m_size(0)
      , m_usage(0)
      , m_accessMask(0)
    {
    }


    BufferEx::BufferEx(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t*const pQueueFamilyIndices)
      : BufferEx()
    {
      Reset(device, flags, size, usage, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
    }


    BufferEx::BufferEx(const VkDevice device, const VkBufferCreateInfo& createInfo)
      : BufferEx()
    {
      Reset(device, createInfo);
    }


    BufferEx::~BufferEx()
    {
      Reset();
    }


    void BufferEx::Reset()
    {
      if (! m_buffer.IsValid())
        return;

      m_buffer.Reset();
      //m_createInfo.Reset();
      m_size = 0;
      m_usage = 0;
      m_accessMask = 0;
    }


    void BufferEx::Reset(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage, const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t*const pQueueFamilyIndices)
    {
      VkBufferCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      createInfo.flags = flags;
      createInfo.size = size;
      createInfo.usage = usage;
      createInfo.sharingMode = sharingMode;
      createInfo.queueFamilyIndexCount = queueFamilyIndexCount;
      createInfo.pQueueFamilyIndices = pQueueFamilyIndices;

      Reset(device, createInfo);
    }


    void BufferEx::Reset(const VkDevice device, const VkBufferCreateInfo& createInfo)
    {
      m_buffer.Reset(device, createInfo);
      //m_createInfo.Reset(createInfo);
      m_size = createInfo.size;
      m_usage = createInfo.usage;
      m_accessMask = 0;
    }


    void BufferEx::CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask)
    {
      if (m_accessMask == dstAccessMask)
        return;

      VkBufferMemoryBarrier bufferMemoryBarrier{};
      bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
      bufferMemoryBarrier.srcAccessMask = m_accessMask;
      bufferMemoryBarrier.dstAccessMask = dstAccessMask;
      bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      bufferMemoryBarrier.buffer = m_buffer.Get();
      bufferMemoryBarrier.offset = 0;
      bufferMemoryBarrier.size = GetSize();

      vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
      m_accessMask = dstAccessMask;
    }
  }
}
