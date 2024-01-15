/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/DeviceMemoryUtil.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceMemory.hpp>
#include <RapidVulkan/Check.hpp>
#include <algorithm>
#include <cassert>
#include <cstring>

namespace Fsl::Vulkan
{
  namespace
  {
    inline void ValidateInsideMappedRange(const SpanRange<VkDeviceSize>& mappedSpan, const VkDeviceSize offset, const VkDeviceSize size)
    {
      const auto mappedEnd = mappedSpan.End();
      if (offset < mappedSpan.Start)
      {
        throw std::invalid_argument("dstOffset < mapped start");
      }
      if (offset > mappedEnd)
      {
        throw std::invalid_argument("dstOffset > mapped length");
      }
      if (size > (mappedEnd - offset))
      {
        throw std::invalid_argument("dstOffset + dataSize > mapped length");
      }
    }


    inline void ValidateNonCoherentSpan(const VkDeviceSize nonCoherentAtomSize, VkDeviceSize offset, const VkDeviceSize size)
    {
      if ((offset % nonCoherentAtomSize) != 0)
      {
        throw std::invalid_argument("offset must obey non-coherent memory alignment requirements");
      }
      if ((size % nonCoherentAtomSize) != 0)
      {
        throw std::invalid_argument("size must obey non-coherent memory alignment requirements");
      }
    }

    inline void ValidateInsideCapacity(const VkDeviceSize capacity, const VkDeviceSize offset, const VkDeviceSize size)
    {
      if (offset > capacity)
      {
        throw std::invalid_argument("offset > capacity");
      }
      if (size > (capacity - offset))
      {
        throw std::invalid_argument("offset + size > capacity");
      }
    }
  }

  void VUDeviceMemory::Reset() noexcept
  {
    if (!m_deviceMemory.IsValid())
    {
      return;
    }

    if (m_isMapped)
    {
      // FSLLOG3_DEBUG_WARNING("Resetting DeviceMemory that is mapped, forcing unmap.");
      DoUnmapMemory();
    }

    // Use destruction order
    m_mappedSpan = {};
    m_isMapped = false;
    m_pData = nullptr;
    m_physicalDeviceLimitNonCoherentAtomSize = 0;
    m_capacity = 0;
    m_memoryPropertyFlags = 0;
    m_deviceMemory.Reset();
  }


  void VUDeviceMemory::Reset(const VkDevice device, const VkMemoryRequirements& memoryRequirements,
                             const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties, const VkMemoryPropertyFlags desiredPropertyFlags,
                             const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize)
  {
    const uint32_t memoryTypeIndex =
      MemoryTypeUtil::GetMemoryTypeIndex(physicalDeviceMemoryProperties, memoryRequirements.memoryTypeBits, desiredPropertyFlags);

    assert(memoryTypeIndex <= physicalDeviceMemoryProperties.memoryTypeCount);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;

    auto actualPropertyFlags = physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;

    Reset(device, allocateInfo, actualPropertyFlags, physicalDeviceLimitNonCoherentAtomSize);
  }


  void VUDeviceMemory::Reset(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags,
                             const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize)
  {
    // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.
    if (device == VK_NULL_HANDLE)
    {
      throw std::invalid_argument("device can not be VK_NULL_HANDLE");
    }
    assert(memoryAllocationInfo.sType == VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
    assert(physicalDeviceLimitNonCoherentAtomSize > 0);

    if (IsValid())
    {
      Reset();
    }

    m_deviceMemory.Reset(device, memoryAllocationInfo);
    m_memoryPropertyFlags = memoryPropertyFlags;
    m_capacity = memoryAllocationInfo.allocationSize;
    m_physicalDeviceLimitNonCoherentAtomSize = physicalDeviceLimitNonCoherentAtomSize;
    assert(!m_isMapped);
    assert(m_pData == nullptr);
  }


  VkDeviceMemory VUDeviceMemory::Release() noexcept
  {
    m_memoryPropertyFlags = 0;
    m_capacity = 0;
    m_physicalDeviceLimitNonCoherentAtomSize = 0;
    m_pData = nullptr;
    m_isMapped = false;
    m_mappedSpan = {};
    return m_deviceMemory.Release();
  }


  void* VUDeviceMemory::MapMemory(const VkDeviceSize offset, const VkDeviceSize size, const VkMemoryMapFlags flags)
  {
    if (!IsValid())
    {
      throw UsageErrorException("Device memory is invalid");
    }
    if (offset > m_capacity)
    {
      throw std::invalid_argument("offset > capacity");
    }
    // Handle the 'magic' VK_WHOLE_SIZE behavior
    const VkDeviceSize actualSize = (size != VK_WHOLE_SIZE ? size : m_capacity - offset);
    if (actualSize > (m_capacity - offset))
    {
      throw std::invalid_argument("offset+size > capacity");
    }
    if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0u)
    {
      throw NotSupportedException("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT not set");
    }
    if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
    {
      ValidateNonCoherentSpan(m_physicalDeviceLimitNonCoherentAtomSize, offset, actualSize);
    }

    if (IsMapped())
    {
      FSLLOG3_DEBUG_WARNING("Mapping already mapped memory");
      DoUnmapMemory();
    }

    // Since we want to ensure that the m_pData is left untouched on error we use a local variable as a intermediary
    void* pData = nullptr;
    RAPIDVULKAN_CHECK(vkMapMemory(m_deviceMemory.GetDevice(), m_deviceMemory.Get(), offset, size, flags, &pData));
    m_pData = pData;
    m_isMapped = VK_TRUE;
    m_mappedSpan = SpanRange<VkDeviceSize>(offset, actualSize);
    return m_pData;
  }


  void VUDeviceMemory::InvalidateMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size)
  {
    if (!IsValid())
    {
      throw UsageErrorException("Device memory is invalid");
    }
    if (!IsMapped())
    {
      throw UsageErrorException("The memory is not mapped");
    }
    ValidateInsideMappedRange(m_mappedSpan, offset, size);

    // Handle the 'magic' VK_WHOLE_SIZE behavior
    if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
    {
      const VkDeviceSize actualSize = (size != VK_WHOLE_SIZE ? size : m_capacity - offset);
      ValidateNonCoherentSpan(m_physicalDeviceLimitNonCoherentAtomSize, offset, actualSize);
    }

    VkMappedMemoryRange mappedMemoryRange{};
    mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedMemoryRange.memory = m_deviceMemory.Get();
    mappedMemoryRange.offset = offset;
    mappedMemoryRange.size = size;
    RAPIDVULKAN_CHECK(vkInvalidateMappedMemoryRanges(m_deviceMemory.GetDevice(), 1, &mappedMemoryRange));
  }


  void VUDeviceMemory::FlushMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size)
  {
    if (!IsValid())
    {
      throw UsageErrorException("Device memory is invalid");
    }
    if (!IsMapped())
    {
      throw UsageErrorException("The memory is not mapped");
    }
    ValidateInsideMappedRange(m_mappedSpan, offset, size);

    // Handle the 'magic' VK_WHOLE_SIZE behavior
    if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
    {
      const VkDeviceSize actualSize = (size != VK_WHOLE_SIZE ? size : m_capacity - offset);
      ValidateNonCoherentSpan(m_physicalDeviceLimitNonCoherentAtomSize, offset, actualSize);
    }

    VkMappedMemoryRange mappedMemoryRange{};
    mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedMemoryRange.memory = m_deviceMemory.Get();
    mappedMemoryRange.offset = offset;
    mappedMemoryRange.size = size;

    RAPIDVULKAN_CHECK(vkFlushMappedMemoryRanges(m_deviceMemory.GetDevice(), 1, &mappedMemoryRange));
  }


  void VUDeviceMemory::UnmapMemory() noexcept
  {
    if (!IsValid())
    {
      FSLLOG3_DEBUG_WARNING("Device memory is not valid, call ignored.");
      return;
    }
    if (!m_isMapped)
    {
      FSLLOG3_DEBUG_WARNING("Device memory is not mapped, call ignored.");
      return;
    }
    if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0u)
    {
      FSLLOG3_DEBUG_WARNING("Can't unmap memory that is not host visible, call ignored");
      return;
    }

    DoUnmapMemory();
  }


  void VUDeviceMemory::Upload(const VkDeviceSize dstOffset, const void* const pSrcData, const VkDeviceSize srcDataSize,
                              const VkMemoryMapFlags memoryMapFlags)
  {
    if (!IsValid())
    {
      throw UsageErrorException("Device memory is invalid");
    }
    // Quick exit
    if (srcDataSize <= 0)
    {
      return;
    }
    if (pSrcData == nullptr)
    {
      throw std::invalid_argument("pData can not be null");
    }
    if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0u)
    {
      throw NotSupportedException("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT not set");
    }

    if (IsMapped())
    {
      // Validate upload operation
      ValidateInsideMappedRange(m_mappedSpan, dstOffset, srcDataSize);

      auto relativeDstOffset = dstOffset - m_mappedSpan.Start;

      if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0u)
      {
        std::memcpy(static_cast<uint8_t*>(m_pData) + relativeDstOffset, pSrcData, srcDataSize);
      }
      else
      {
        auto dstSpan = DeviceMemoryUtil::MemorySpan(m_mappedSpan, relativeDstOffset);
        auto result =
          DeviceMemoryUtil::TryFastCopyDataNonCoherent(m_deviceMemory.GetDevice(), m_physicalDeviceLimitNonCoherentAtomSize, m_deviceMemory.Get(),
                                                       m_capacity, m_memoryPropertyFlags, dstSpan, m_pData, pSrcData, srcDataSize);
        RapidVulkan::CheckError(result, "vkFlushMappedMemoryRanges", __FILE__, __LINE__);
      }
    }
    else
    {
      ValidateInsideCapacity(m_capacity, dstOffset, srcDataSize);

      DeviceMemoryUtil::FastUpload(m_deviceMemory.GetDevice(), m_physicalDeviceLimitNonCoherentAtomSize, m_deviceMemory.Get(), m_capacity,
                                   m_memoryPropertyFlags, dstOffset, memoryMapFlags, pSrcData, srcDataSize);
    }
  }

  void VUDeviceMemory::DoUnmapMemory() noexcept
  {
    assert(m_isMapped);
    assert(m_deviceMemory.IsValid());

    vkUnmapMemory(m_deviceMemory.GetDevice(), m_deviceMemory.Get());
    m_pData = nullptr;
    m_isMapped = false;
    m_mappedSpan = {};
  }
}
