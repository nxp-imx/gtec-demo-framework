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

#include <FslUtil/Vulkan1_0/Extend/DeviceMemoryEx.hpp>
#include <FslUtil/Vulkan1_0/Exceptions.hpp>
#include <FslUtil/Vulkan1_0/Util/MemoryTypeUtil.hpp>
#include <FslBase/Log/BasicLog.hpp>
#include <RapidVulkan/Check.hpp>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    // move assignment operator
    DeviceMemoryEx& DeviceMemoryEx::operator=(DeviceMemoryEx&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_deviceMemory = std::move(other.m_deviceMemory);
        m_memoryPropertyFlags = other.m_memoryPropertyFlags;
        m_allocateInfo = other.m_allocateInfo;
        m_pData = other.m_pData;
        m_isMapped = other.m_isMapped;

        // Remove the data from other
        other.m_memoryPropertyFlags = 0;
        other.m_allocateInfo = VkMemoryAllocateInfo{};
        other.m_pData = nullptr;
        other.m_isMapped = false;
      }
      return *this;
    }


    // Transfer ownership from other to this
    DeviceMemoryEx::DeviceMemoryEx(DeviceMemoryEx&& other) noexcept
      : m_deviceMemory(std::move(other.m_deviceMemory))
      , m_memoryPropertyFlags(other.m_memoryPropertyFlags)
      , m_allocateInfo(other.m_allocateInfo)
      , m_pData(other.m_pData)
      , m_isMapped(other.m_isMapped)
    {
      // Remove the data from other
      other.m_memoryPropertyFlags = 0;
      other.m_allocateInfo = VkMemoryAllocateInfo{};
      other.m_pData = nullptr;
      other.m_isMapped = false;
    }


    // For now we implement the code here, if this turns out to be a performance problem we can move the code
    // to the header file to ensure its inlined.
    DeviceMemoryEx::DeviceMemoryEx()
      : m_memoryPropertyFlags(0)
      , m_allocateInfo{}
      , m_pData(nullptr)
      , m_isMapped(false)
    {
    }


    DeviceMemoryEx::DeviceMemoryEx(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo,
                                   const VkMemoryPropertyFlags memoryPropertyFlags)
      : DeviceMemoryEx()
    {
      Reset(device, memoryAllocationInfo, memoryPropertyFlags);
    }


    DeviceMemoryEx::DeviceMemoryEx(const VkDevice device, const VkMemoryRequirements& memoryRequirements, const uint32_t memoryTypeCount,
                                   const VkMemoryType* memoryTypes, const VkMemoryPropertyFlags propertyFlags)
      : DeviceMemoryEx()
    {
      Reset(device, memoryRequirements, memoryTypeCount, memoryTypes, propertyFlags);
    }


    DeviceMemoryEx::~DeviceMemoryEx()
    {
      Reset();
    }


    void DeviceMemoryEx::Reset() noexcept
    {
      if (!m_deviceMemory.IsValid())
      {
        return;
      }

      if (m_isMapped)
      {
        FSLBASICLOG_DEBUG_WARNING("Resetting DeviceMemory that is mapped, forcing unmap.");
        UnmapMemory();
      }

      m_deviceMemory.Reset();
      m_memoryPropertyFlags = 0;
      m_allocateInfo = VkMemoryAllocateInfo{};
      m_isMapped = false;
    }


    void DeviceMemoryEx::Reset(const VkDevice device, const VkMemoryRequirements& memoryRequirements, const uint32_t memoryTypeCount,
                               const VkMemoryType* memoryTypes, const VkMemoryPropertyFlags propertyFlags)
    {
      const uint32_t memoryTypeIndex =
        MemoryTypeUtil::GetMemoryTypeIndex(memoryTypeCount, memoryTypes, memoryRequirements.memoryTypeBits, propertyFlags);

      VkMemoryAllocateInfo allocateInfo{};
      allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocateInfo.allocationSize = memoryRequirements.size;
      allocateInfo.memoryTypeIndex = memoryTypeIndex;

      Reset(device, allocateInfo, propertyFlags);
    }


    void DeviceMemoryEx::Reset(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo,
                               const VkMemoryPropertyFlags memoryPropertyFlags)
    {
      // We do the check here to be user friendly, if it becomes a performance issue switch it to a assert.
      if (device == VK_NULL_HANDLE)
      {
        throw std::invalid_argument("device can not be VK_NULL_HANDLE");
      }

      if (m_deviceMemory.IsValid())
      {
        Reset();
      }

      m_deviceMemory.Reset(device, memoryAllocationInfo);
      m_memoryPropertyFlags = memoryPropertyFlags;
      m_allocateInfo = memoryAllocationInfo;
      assert(!m_isMapped);
      assert(m_pData == nullptr);
    }


    VkDeviceMemory DeviceMemoryEx::Release()
    {
      return m_deviceMemory.Release();
    }


    void DeviceMemoryEx::MapMemory(const VkDeviceSize offset, const VkDeviceSize size, const VkMemoryMapFlags flags)
    {
      if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0u)
      {
        throw NotSupportedException("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT not set");
      }

      if (m_isMapped)
      {
        FSLBASICLOG_DEBUG_WARNING("Mapping already mapped memory");
        UnmapMemory();
      }

      // Since we want to ensure that the m_pData is left untouched on error we use a local variable as a intermediary
      void* pData;
      RAPIDVULKAN_CHECK(vkMapMemory(m_deviceMemory.GetDevice(), m_deviceMemory.Get(), offset, size, flags, &pData));
      m_pData = pData;
      m_isMapped = VK_TRUE;
    }


    void* DeviceMemoryEx::GetMappedMemoryPointer() const
    {
      FSLBASICLOG_DEBUG_WARNING_IF(!m_isMapped, "Requested a memory pointer for unmapped device memory, this will be a nullptr.")
      return m_pData;
    }


    void DeviceMemoryEx::InvalidateMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size)
    {
      VkMappedMemoryRange mappedMemoryRange{};
      mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      mappedMemoryRange.memory = m_deviceMemory.Get();
      mappedMemoryRange.offset = offset;
      mappedMemoryRange.size = size;
      RAPIDVULKAN_CHECK(vkInvalidateMappedMemoryRanges(m_deviceMemory.GetDevice(), 1, &mappedMemoryRange));
    }


    void DeviceMemoryEx::FlushMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size)
    {
      VkMappedMemoryRange mappedMemoryRange{};
      mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      mappedMemoryRange.memory = m_deviceMemory.Get();
      mappedMemoryRange.offset = offset;
      mappedMemoryRange.size = size;

      RAPIDVULKAN_CHECK(vkFlushMappedMemoryRanges(m_deviceMemory.GetDevice(), 1, &mappedMemoryRange));
    }


    void DeviceMemoryEx::UnmapMemory()
    {
      if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0u)
      {
        FSLBASICLOG_DEBUG_WARNING("Can't unmap memory that is not host visible, call ignored");
        return;
      }
      if (!m_isMapped)
      {
        FSLBASICLOG_DEBUG_WARNING("Device memory is not mapped, call ignored.");
        return;
      }

      vkUnmapMemory(m_deviceMemory.GetDevice(), m_deviceMemory.Get());
      m_pData = nullptr;
      m_isMapped = false;
    }


    void DeviceMemoryEx::Upload(const VkDeviceSize offset, const VkMemoryMapFlags flags, const void* const pData, const size_t dataSize)
    {
      if (pData == nullptr)
      {
        throw std::invalid_argument("pData can not be null");
      }

      if (dataSize <= 0)
      {
        return;
      }

      MapMemory(offset, dataSize, flags);
      try
      {
        std::memcpy(m_pData, pData, dataSize);
        if ((m_memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0u)
        {
          FlushMappedMemoryRanges(offset, dataSize);
        }
        UnmapMemory();
      }
      catch (const std::exception&)
      {
        UnmapMemory();
        throw;
      }
    }
  }
}
