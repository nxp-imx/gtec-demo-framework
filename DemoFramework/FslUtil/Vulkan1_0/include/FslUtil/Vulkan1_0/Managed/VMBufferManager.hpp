#ifndef FSLUTIL_VULKAN1_0_MANAGED_VMBUFFERMANAGER_HPP
#define FSLUTIL_VULKAN1_0_MANAGED_VMBUFFERMANAGER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/CommandPool.hpp>
#include <RapidVulkan/CommandBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUPhysicalDeviceRecord.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferUsage.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    class VMBufferManager
    {
      VUPhysicalDeviceRecord m_physicalDevice;
      VkDevice m_device;
      VkQueue m_queue;
      // uint32_t m_queueFamilyIndex;
      RapidVulkan::CommandPool m_commandPool;
      RapidVulkan::CommandBuffer m_commandBuffer;

    public:
      VMBufferManager(const VMBufferManager&) = delete;
      VMBufferManager& operator=(const VMBufferManager&) = delete;
      VMBufferManager(VMBufferManager&& other) noexcept = delete;
      VMBufferManager& operator=(VMBufferManager&& other) noexcept = delete;

      VMBufferManager(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkQueue queue, const uint32_t queueFamilyIndex);

      //! @param elementStride the size of one element in bytes
      VUBufferMemory CreateBuffer(const void* const pVertices, const std::size_t elementCount, const std::size_t elementStride,
                                  const VkBufferUsageFlags bufferUsageFlags, const VMBufferUsage usage)
      {
        return CreateBuffer(pVertices, elementCount, elementCount, elementStride, bufferUsageFlags, usage);
      }

      VUBufferMemory CreateBuffer(const void* const pVertices, const std::size_t elementCount, const std::size_t elementCapacity,
                                  const std::size_t elementStride, const VkBufferUsageFlags bufferUsageFlags, const VMBufferUsage usage);

      VUBufferMemory CreateDynamicBuffer(const void* const pVertices, const std::size_t elementCount, const std::size_t elementStride,
                                         const VkBufferUsageFlags bufferUsageFlags)
      {
        return CreateDynamicBuffer(pVertices, elementCount, elementCount, elementStride, bufferUsageFlags);
      }

      VUBufferMemory CreateDynamicBuffer(const void* const pVertices, const std::size_t elementCount, const std::size_t elementCapacity,
                                         const std::size_t elementStride, const VkBufferUsageFlags bufferUsageFlags);


      //! @brief Create a dynamic buffer with the given element capacity
      VUBufferMemory CreateDynamicBuffer(const std::size_t elementCapacity, const std::size_t elementStride,
                                         const VkBufferUsageFlags bufferUsageFlags);

      VUBufferMemory CreateStaticBuffer(const void* const pVertices, const std::size_t elementCount, const std::size_t elementStride,
                                        const VkBufferUsageFlags bufferUsageFlags)
      {
        return CreateStaticBuffer(pVertices, elementCount, elementCount, elementStride, bufferUsageFlags);
      }

      VUBufferMemory CreateStaticBuffer(const void* const pVertices, const std::size_t elementCount, const std::size_t elementCapacity,
                                        const std::size_t elementStride, const VkBufferUsageFlags bufferUsageFlags);

    private:
      void CopyBuffer(VUBufferMemory& rDst, const VUBufferMemory& src);
    };
  }
}

#endif
