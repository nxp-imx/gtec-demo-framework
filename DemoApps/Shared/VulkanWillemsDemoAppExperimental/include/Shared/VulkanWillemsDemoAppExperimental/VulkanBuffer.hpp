#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANBUFFER_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANBUFFER_HPP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslBase/Attributes.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vulkan/vulkan.h>
#include <cassert>

namespace Fsl
{
  namespace Willems
  {
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VulkanBuffer
    {
      RapidVulkan::Buffer m_buffer;
      RapidVulkan::Memory m_deviceMemory;
      VkDescriptorBufferInfo m_descriptor;
      VkDeviceSize m_size;
      VkDeviceSize m_alignment;
      //! sage flags to be filled by external source at buffer creation (to query at some later point)
      VkBufferUsageFlags m_usageFlags;
      //! Memory properties flags to be filled by external source at buffer creation (to query at some later point)
      VkMemoryPropertyFlags m_memoryPropertyFlags;
      void* m_pMapped;

    public:
      VulkanBuffer(const VulkanBuffer&) = delete;
      VulkanBuffer& operator=(const VulkanBuffer&) = delete;

      //! @brief Move assignment operator
      VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VulkanBuffer(VulkanBuffer&& other) noexcept;

      //! @brief Move objects into this object
      VulkanBuffer(RapidVulkan::Buffer&& buffer, RapidVulkan::Memory&& deviceMemory, const VkDescriptorBufferInfo& descriptor,
                   const VkDeviceSize size, const VkDeviceSize alignment, const VkBufferUsageFlags usageFlags,
                   const VkMemoryPropertyFlags memoryPropertyFlags);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VulkanBuffer();

      ~VulkanBuffer()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset() noexcept;


      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return m_buffer.GetDevice();
      }

      //! @brief Get the associated 'buffer'
      VkBuffer GetBuffer() const
      {
        return m_buffer.Get();
      }

      //! @brief Get the associated 'buffer'
      const VkBuffer* GetBufferPointer() const
      {
        return m_buffer.GetPointer();
      }

      //! @brief Get the associated 'memory'
      VkDeviceMemory GetMemory() const
      {
        return m_deviceMemory.Get();
      }

      VkDescriptorBufferInfo GetDesciptor() const
      {
        return m_descriptor;
      }

      const VkDescriptorBufferInfo* GetDescriptorPointer() const
      {
        return &m_descriptor;
      }

      VkDeviceSize GetSize() const
      {
        return m_size;
      }

      VkDeviceSize GetAlignment() const
      {
        return m_alignment;
      }

      VkBufferUsageFlags GetUsageFlags() const
      {
        return m_usageFlags;
      }

      VkMemoryPropertyFlags GetMemoryPropertyFlags() const
      {
        return m_memoryPropertyFlags;
      }


      const void* GetMappedPointer() const
      {
        return m_pMapped;
      }


      void* GetMappedPointer()
      {
        return m_pMapped;
      }


      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_buffer.IsValid();
      }

      //! @brief Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
      //! @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
      //! @param offset (Optional) Byte offset from beginning
      //! @note The ordering of these params are reversed compared to vulkan so this is deprecated, use MapEx instead
      FSL_ATTR_DEPRECATED void Map(const VkDeviceSize size = VK_WHOLE_SIZE, const VkDeviceSize offset = 0);

      //! @brief Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
      //! @param offset (Optional) Byte offset from beginning
      //! @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
      void MapEx(const VkDeviceSize offset = 0, const VkDeviceSize size = VK_WHOLE_SIZE);

      //! @brief Unmap a mapped memory range
      //! @note Does not return a result as vkUnmapMemory can't fail
      void Unmap();

      // @brief Attach the allocated memory block to the buffer
      // @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
      // @return VkResult of the bindBufferMemory call
      void Bind(const VkDeviceSize offset = 0);
    };
  }
}

#endif
