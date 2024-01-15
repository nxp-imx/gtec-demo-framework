/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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

#include <FslBase/Bits/AlignmentUtil.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslUtil/Vulkan1_0/VUSegmentedBufferMemory.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>

namespace Fsl::Vulkan
{
  namespace
  {
    VkDeviceSize GetBufferMinimumAlignment(const VkPhysicalDeviceLimits& limits, const VkBufferUsageFlags usageFlags)
    {
      VkDeviceSize alignment = 1;
      if ((usageFlags & (VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)) != 0u)
      {
        alignment = std::max(alignment, limits.minTexelBufferOffsetAlignment);
      }
      else if ((usageFlags & (VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)) != 0u)
      {
        alignment = std::max(alignment, limits.minUniformBufferOffsetAlignment);
      }
      else if ((usageFlags & (VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)) != 0u)
      {
        alignment = std::max(alignment, limits.minStorageBufferOffsetAlignment);
      }
      return alignment;
    }
  }

  VUSegmentedBufferMemory& VUSegmentedBufferMemory::operator=(VUSegmentedBufferMemory&& other) noexcept
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
      m_segmentCount = other.m_segmentCount;
      m_segmentStride = other.m_segmentStride;
      m_elementSize = other.m_elementSize;

      // Remove the data from other
      other.m_descriptorBufferInfo = {};
      other.m_segmentCount = 0u;
      other.m_segmentStride = 0u;
      other.m_elementSize = 0u;
    }
    return *this;
  }


  VUSegmentedBufferMemory::VUSegmentedBufferMemory(VUSegmentedBufferMemory&& other) noexcept
    : m_buffer(std::move(other.m_buffer))
    , m_memory(std::move(other.m_memory))
    , m_descriptorBufferInfo(other.m_descriptorBufferInfo)
    , m_segmentCount(other.m_segmentCount)
    , m_segmentStride(other.m_segmentStride)
    , m_elementSize(other.m_elementSize)
  {
    // Remove the data from other
    other.m_descriptorBufferInfo = {};
    other.m_segmentCount = 0u;
    other.m_segmentStride = 0u;
    other.m_elementSize = 0u;
  }


  VUSegmentedBufferMemory::VUSegmentedBufferMemory(VUBuffer&& buffer, VUDeviceMemory&& deviceMemory, const VkDescriptorBufferInfo& descriptor)
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


  void VUSegmentedBufferMemory::Reset() noexcept
  {
    if (!IsValid())
    {
      return;
    }

    assert(m_buffer.IsValid());
    assert(m_memory.IsValid());

    // Use destruction order
    m_elementSize = 0u;
    m_segmentStride = 0u;
    m_segmentCount = 0u;
    m_descriptorBufferInfo = {};
    m_memory.Reset();
    m_buffer.Reset();
  }


  void VUSegmentedBufferMemory::Reset(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo,
                                      const VkMemoryPropertyFlags memoryPropertyFlags, const uint32_t segmentCount)
  {
    Reset(device.GetPhysicalDevice(), device.Get(), bufferCreateInfo, memoryPropertyFlags, segmentCount);
  }


  void VUSegmentedBufferMemory::Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo,
                                      const VkMemoryPropertyFlags memoryPropertyFlags, const uint32_t segmentCount)
  {
    if (IsValid())
    {
      Reset();
    }

    try
    {
      VkBufferCreateInfo bufferCreateInfoEx = bufferCreateInfo;
      VkDeviceSize segmentStride = 0u;
      if (segmentCount > 1)
      {
        const VkDeviceSize segmentAlignment = GetBufferMinimumAlignment(physicalDevice.Properties.limits, bufferCreateInfo.usage);
        segmentStride = AlignmentUtil::GetByteSize(bufferCreateInfoEx.size, segmentAlignment);
        bufferCreateInfoEx.size = segmentStride * segmentCount;
      }

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

      SetMemberVarsOnNewReset(segmentCount, segmentStride, bufferCreateInfo.size);
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


  void VUSegmentedBufferMemory::Bind(const VkDeviceSize offset)
  {
    assert(IsValid());
    RAPIDVULKAN_CHECK(vkBindBufferMemory(m_buffer.GetDevice(), m_buffer.Get(), m_memory.Get(), offset));
  }


  void VUSegmentedBufferMemory::SetMemberVarsOnNewReset(const uint32_t segmentCount, const VkDeviceSize segmentStride, const VkDeviceSize elementSize)
  {
    if (segmentStride > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("stride is unsupported");
    }

    // Default to the full size of this buffer
    m_descriptorBufferInfo = {};
    m_descriptorBufferInfo.buffer = m_buffer.Get();
    m_descriptorBufferInfo.range = elementSize;
    m_segmentCount = segmentCount;
    m_segmentStride = static_cast<uint32_t>(segmentStride);
    m_elementSize = elementSize;
  }


  void VUSegmentedBufferMemory::UploadArray(const uint32_t offset, const void* const pData, const std::size_t dataElementCount,
                                            const std::size_t elementSize, const VkMemoryMapFlags flags)
  {
    if (pData == nullptr)
    {
      throw std::invalid_argument("pData can not be null");
    }
    if (elementSize > std::numeric_limits<uint32_t>::max())
    {
      throw NotSupportedException("elementSize can not be larger than a uint32_t");
    }

    const auto* pSrcData = static_cast<const uint8_t*>(pData);
    uint32_t dstOffset = offset;
    const auto srcStride = static_cast<uint32_t>(elementSize);
    for (std::size_t i = 0; i < dataElementCount; ++i)
    {
      Upload(dstOffset, pSrcData, srcStride, flags);
      pSrcData += srcStride;
      dstOffset += m_segmentStride;
    }
  }
}
