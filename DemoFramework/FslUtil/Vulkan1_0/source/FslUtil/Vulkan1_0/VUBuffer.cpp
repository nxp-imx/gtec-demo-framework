/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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
 *    * Neither the name of the NXP. nor the names of
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

#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <RapidVulkan/Check.hpp>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    VUBuffer::VUBuffer(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage,
                       const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* const pQueueFamilyIndices)
      : VUBuffer()
    {
      Reset(device, flags, size, usage, sharingMode, queueFamilyIndexCount, pQueueFamilyIndices);
    }


    VUBuffer::VUBuffer(const VkDevice device, const VkBufferCreateInfo& createInfo)
      : VUBuffer()
    {
      Reset(device, createInfo);
    }


    void VUBuffer::Reset() noexcept
    {
      if (!m_buffer.IsValid())
      {
        return;
      }

      m_buffer.Reset();
      m_size = 0;
      m_usage = 0;
      m_accessMask = 0;
    }


    void VUBuffer::Reset(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage,
                         const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* const pQueueFamilyIndices)
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


    void VUBuffer::Reset(const VkDevice device, const VkBufferCreateInfo& createInfo)
    {
      m_buffer.Reset(device, createInfo);
      m_size = createInfo.size;
      m_usage = createInfo.usage;
      m_accessMask = 0;
    }
  }
}
