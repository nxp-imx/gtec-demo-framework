/****************************************************************************************************************************************************
 * Copyright 2017, 2022 NXP
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

#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <cassert>
#include <utility>

namespace Fsl::Vulkan
{
  VUBufferMemory& VUBufferMemory::operator=(VUBufferMemory&& other) noexcept
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
      m_memory = std::move(other.m_memory);
      m_descriptorBufferInfo = other.m_descriptorBufferInfo;

      // Remove the data from other
      other.m_descriptorBufferInfo = {};
    }
    return *this;
  }


  VUBufferMemory::VUBufferMemory(VUBufferMemory&& other) noexcept
    : m_buffer(std::move(other.m_buffer))
    , m_memory(std::move(other.m_memory))
    , m_descriptorBufferInfo(other.m_descriptorBufferInfo)
  {
    // Remove the data from other
    other.m_descriptorBufferInfo = {};
  }


  VUBufferMemory::VUBufferMemory(VUBuffer&& buffer, VUDeviceMemory&& deviceMemory, const VkDescriptorBufferInfo& descriptor)
    : m_buffer(std::move(buffer))
    , m_memory(std::move(deviceMemory))
    , m_descriptorBufferInfo(descriptor)
  {
    const bool hasOneValid = m_buffer.IsValid() || m_memory.IsValid();
    if (m_buffer.IsValid() != hasOneValid || m_memory.IsValid() != hasOneValid)
    {
      throw std::invalid_argument("Either all objects has to be valid or none must be");
    }

    if (m_buffer.GetDevice() != m_memory.GetDevice())
    {
      throw std::invalid_argument("All objects must belong to the same device");
    }
  }


  void VUBufferMemory::Reset() noexcept
  {
    if (!IsValid())
    {
      return;
    }

    assert(m_buffer.IsValid());
    assert(m_memory.IsValid());

    // Use destruction order
    m_descriptorBufferInfo = {};
    m_memory.Reset();
    m_buffer.Reset();
  }


  void VUBufferMemory::Reset(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
  {
    Reset(device.GetPhysicalDevice(), device.Get(), bufferCreateInfo, memoryPropertyFlags);
  }


  void VUBufferMemory::Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo,
                             const VkMemoryPropertyFlags memoryPropertyFlags)
  {
    if (IsValid())
    {
      Reset();
    }

    try
    {
      VkBufferCreateInfo bufferCreateInfoEx = bufferCreateInfo;

      m_buffer.Reset(device, bufferCreateInfoEx);
      const VkMemoryRequirements memoryRequirements = m_buffer.GetBufferMemoryRequirements();

      VkMemoryAllocateInfo memoryAllocationInfo{};
      memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      memoryAllocationInfo.allocationSize = memoryRequirements.size;
      // Find a memoryTypeIndex that matches the requested memoryPropertyFlags
      memoryAllocationInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryPropertyFlags);

      assert(memoryAllocationInfo.memoryTypeIndex < physicalDevice.MemoryProperties.memoryTypeCount);
      const auto finalMemoryPropertyFlags = physicalDevice.MemoryProperties.memoryTypes[memoryAllocationInfo.memoryTypeIndex].propertyFlags;
      m_memory.Reset(device, memoryAllocationInfo, finalMemoryPropertyFlags, physicalDevice.Properties.limits.nonCoherentAtomSize);

      RAPIDVULKAN_CHECK(vkBindBufferMemory(device, m_buffer.Get(), m_memory.Get(), 0));

      SetMemberVarsOnNewReset();
    }
    catch (const std::exception&)
    {
      // Cleanup this object so its back to its default invalid object state
      if (m_memory.IsValid())
      {
        m_memory.Reset();
      }
      if (m_buffer.IsValid())
      {
        m_buffer.Reset();
      }
      throw;
    }
  }


  // void VUBuffer::Reset(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo,
  //                     const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize)
  //{
  //  if (IsValid())
  //  {
  //    Reset();
  //  }

  //  try
  //  {
  //    m_buffer.Reset(device, bufferCreateInfo);
  //    m_memory.Reset(device, memoryAllocationInfo, memoryPropertyFlags, physicalDeviceLimitNonCoherentAtomSize);

  //    RAPIDVULKAN_CHECK(vkBindBufferMemory(device, m_buffer.Get(), m_memory.Get(), 0));

  //    SetMemberVarsOnNewReset(1);
  //  }
  //  catch (const std::exception&)
  //  {
  //    // Cleanup this object so its back to its default invalid object state
  //    if (m_memory.IsValid())
  //    {
  //      m_memory.Reset();
  //    }
  //    if (m_buffer.IsValid())
  //    {
  //      m_buffer.Reset();
  //    }
  //    throw;
  //  }
  //}


  void VUBufferMemory::Bind(const VkDeviceSize offset)
  {
    assert(IsValid());
    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_buffer.GetDevice(), m_buffer.Get(), m_memory.Get(), offset));
  }

  void VUBufferMemory::SetMemberVarsOnNewReset()
  {
    // Default to the full size of this buffer
    m_descriptorBufferInfo = {};
    m_descriptorBufferInfo.buffer = m_buffer.Get();
    m_descriptorBufferInfo.range = m_buffer.GetSize();
  }
}
