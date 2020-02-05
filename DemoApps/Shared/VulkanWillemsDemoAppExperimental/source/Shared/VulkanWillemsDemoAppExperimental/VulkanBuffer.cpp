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

#include <Shared/VulkanWillemsDemoAppExperimental/VulkanBuffer.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

using namespace RapidVulkan;

namespace Fsl
{
  namespace Willems
  {
    VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
        {
          Reset();
        }

        // Claim ownership here
        m_buffer = std::move(other.m_buffer);
        m_deviceMemory = std::move(other.m_deviceMemory);
        m_descriptor = other.m_descriptor;
        m_size = other.m_size;
        m_alignment = other.m_alignment;
        m_usageFlags = other.m_usageFlags;
        m_memoryPropertyFlags = other.m_memoryPropertyFlags;
        m_pMapped = other.m_pMapped;

        // Remove the data from other
        other.m_descriptor = VkDescriptorBufferInfo{};
        other.m_size = 0;
        other.m_alignment = 0;
        other.m_usageFlags = 0;
        other.m_memoryPropertyFlags = 0;
        other.m_pMapped = nullptr;
      }
      return *this;
    }


    VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
      : m_buffer(std::move(other.m_buffer))
      , m_deviceMemory(std::move(other.m_deviceMemory))
      , m_descriptor(other.m_descriptor)
      , m_size(other.m_size)
      , m_alignment(other.m_alignment)
      , m_usageFlags(other.m_usageFlags)
      , m_memoryPropertyFlags(other.m_memoryPropertyFlags)
      , m_pMapped(other.m_pMapped)
    {
      // Remove the data from other
      other.m_descriptor = VkDescriptorBufferInfo{};
      other.m_size = 0;
      other.m_alignment = 0;
      other.m_usageFlags = 0;
      other.m_memoryPropertyFlags = 0;
      other.m_pMapped = nullptr;
    }


    VulkanBuffer::VulkanBuffer(RapidVulkan::Buffer&& buffer, RapidVulkan::Memory&& deviceMemory, const VkDescriptorBufferInfo& descriptor,
                               const VkDeviceSize size, const VkDeviceSize alignment, const VkBufferUsageFlags usageFlags,
                               const VkMemoryPropertyFlags memoryPropertyFlags)
      : m_buffer(std::move(buffer))
      , m_deviceMemory(std::move(deviceMemory))
      , m_descriptor(descriptor)
      , m_size(size)
      , m_alignment(alignment)
      , m_usageFlags(usageFlags)
      , m_memoryPropertyFlags(memoryPropertyFlags)
      , m_pMapped(nullptr)
    {
      const bool hasOneValid = m_buffer.IsValid() || m_deviceMemory.IsValid();
      if (m_buffer.IsValid() != hasOneValid || m_deviceMemory.IsValid() != hasOneValid)
      {
        throw std::invalid_argument("Either all objects has to be valid or none must be");
      }

      if (m_buffer.GetDevice() != m_deviceMemory.GetDevice())
      {
        throw std::invalid_argument("All objects must belong to the same device");
      }
    }


    VulkanBuffer::VulkanBuffer()
      : m_descriptor{}
      , m_size(0)
      , m_alignment(0)
      , m_usageFlags(0)
      , m_memoryPropertyFlags(0)
      , m_pMapped(nullptr)
    {
    }


    void VulkanBuffer::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      assert(m_buffer.IsValid());
      assert(m_deviceMemory.IsValid());
      //! If this goes of a mapped buffer is being reset, unmap it!
      if (m_pMapped != nullptr)
      {
        Unmap();
      }

      m_buffer.Reset();
      m_deviceMemory.Reset();
      m_descriptor = VkDescriptorBufferInfo{};
      m_size = 0;
      m_alignment = 0;
      m_usageFlags = 0;
      m_memoryPropertyFlags = 0;
      m_pMapped = nullptr;
    }


    void VulkanBuffer::Map(const VkDeviceSize size, const VkDeviceSize offset)
    {
      MapEx(offset, size);
    }

    void VulkanBuffer::MapEx(const VkDeviceSize offset, const VkDeviceSize size)
    {
      assert(IsValid());
      RAPIDVULKAN_CHECK(vkMapMemory(m_buffer.GetDevice(), m_deviceMemory.Get(), offset, size, 0, &m_pMapped));
    }


    void VulkanBuffer::Unmap()
    {
      FSLLOG3_DEBUG_WARNING_IF(!IsValid(), "Unmap called on a invalid object, request ignored.");
      FSLLOG3_DEBUG_WARNING_IF(m_pMapped == nullptr, "Unmap called on a unmapped object, request ignored.");
      if (m_pMapped != nullptr)
      {
        vkUnmapMemory(m_buffer.GetDevice(), m_deviceMemory.Get());
        m_pMapped = nullptr;
      }
    }


    void VulkanBuffer::Bind(const VkDeviceSize offset)
    {
      assert(IsValid());
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_buffer.GetDevice(), m_buffer.Get(), m_deviceMemory.Get(), offset));
    }
  }
}
