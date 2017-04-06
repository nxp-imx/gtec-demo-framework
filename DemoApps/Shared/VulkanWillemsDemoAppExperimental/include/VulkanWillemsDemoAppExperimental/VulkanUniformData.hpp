#ifndef VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANUNIFORMDATA_HPP
#define VULKANWILLEMSDEMOAPPEXPERIMENTAL_VULKANUNIFORMDATA_HPP
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

#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/Buffer.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <vulkan/vulkan.h>
#include <cstdlib>

namespace Fsl
{
  namespace Willems
  {
    // Contains all vulkan objects required for a uniform data object
    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    struct VulkanUniformData
    {
      Vulkan::Buffer Buffer;
      Vulkan::Memory Memory;
      VkDescriptorBufferInfo Descriptor;
      uint32_t AllocSize;
      void* pMapped;

      VulkanUniformData(const VulkanUniformData&) = delete;
      VulkanUniformData& operator=(const VulkanUniformData&) = delete;

      //! @brief Move assignment operator
      VulkanUniformData& operator=(VulkanUniformData&& other);

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VulkanUniformData(VulkanUniformData&& other);

      //! @brief Move objects into this object
      VulkanUniformData(Vulkan::Buffer&& buffer, Vulkan::Memory&& deviceMemory, const VkDescriptorBufferInfo& descriptor, const uint32_t allocSize);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VulkanUniformData();

      ~VulkanUniformData()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();


      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return Buffer.GetDevice();
      }

      //! @brief Get the associated 'buffer'
      VkBuffer GetBuffer() const
      {
        return Buffer.Get();
      }

      //! @brief Get the associated 'buffer'
      const VkBuffer* GetBufferPointer() const
      {
        return Buffer.GetPointer();
      }

      //! @brief Get the associated 'memory'
      VkDeviceMemory GetMemory() const
      {
        return Memory.Get();
      }

      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return Buffer.IsValid();
      }

      //! @brief Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
      //! @param offset (Optional) Byte offset from beginning
      //! @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
      void Map(const VkDeviceSize offset = 0, const VkDeviceSize size = VK_WHOLE_SIZE);

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
