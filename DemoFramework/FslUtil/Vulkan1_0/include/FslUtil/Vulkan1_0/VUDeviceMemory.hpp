#ifndef FSLUTIL_VULKAN1_0_VUDEVICEMEMORY_HPP
#define FSLUTIL_VULKAN1_0_VUDEVICEMEMORY_HPP
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
#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/SpanRange.hpp>
#include <RapidVulkan/Memory.hpp>
#include <vulkan/vulkan.h>
#include <utility>

namespace Fsl
{
  namespace Vulkan
  {
    // This object is movable so it can be thought of as behaving int he same was as a unique_ptr and is compatible with std containers
    class VUDeviceMemory
    {
      RapidVulkan::Memory m_deviceMemory;
      VkMemoryPropertyFlags m_memoryPropertyFlags = 0;
      //! The device memory capacity
      VkDeviceSize m_capacity = 0;
      VkDeviceSize m_physicalDeviceLimitNonCoherentAtomSize = 0;
      void* m_pData = nullptr;
      bool m_isMapped = false;
      SpanRange<VkDeviceSize> m_mappedSpan;

    public:
      VUDeviceMemory(const VUDeviceMemory&) = delete;
      VUDeviceMemory& operator=(const VUDeviceMemory&) = delete;

      // move assignment operator
      VUDeviceMemory& operator=(VUDeviceMemory&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          Reset();

          // Claim ownership here
          m_deviceMemory = std::move(other.m_deviceMemory);
          m_memoryPropertyFlags = other.m_memoryPropertyFlags;
          m_capacity = other.m_capacity;
          m_physicalDeviceLimitNonCoherentAtomSize = other.m_physicalDeviceLimitNonCoherentAtomSize;
          m_pData = other.m_pData;
          m_isMapped = other.m_isMapped;
          m_mappedSpan = other.m_mappedSpan;

          // Remove the data from other
          other.m_memoryPropertyFlags = 0;
          other.m_capacity = 0;
          other.m_physicalDeviceLimitNonCoherentAtomSize = 0;
          other.m_pData = nullptr;
          other.m_isMapped = false;
          other.m_mappedSpan = {};
        }
        return *this;
      }

      // move constructor
      VUDeviceMemory(VUDeviceMemory&& other) noexcept
        : m_deviceMemory(std::move(other.m_deviceMemory))
        , m_memoryPropertyFlags(other.m_memoryPropertyFlags)
        , m_capacity(other.m_capacity)
        , m_physicalDeviceLimitNonCoherentAtomSize(other.m_physicalDeviceLimitNonCoherentAtomSize)
        , m_pData(other.m_pData)
        , m_isMapped(other.m_isMapped)
        , m_mappedSpan(other.m_mappedSpan)
      {
        // Remove the data from other
        other.m_memoryPropertyFlags = 0;
        other.m_capacity = 0;
        other.m_physicalDeviceLimitNonCoherentAtomSize = 0;
        other.m_pData = nullptr;
        other.m_isMapped = false;
        other.m_mappedSpan = {};
      }

      VUDeviceMemory() = default;

      //! @brief Create a new memory.
      VUDeviceMemory(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags,
                     const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize)
        : VUDeviceMemory()
      {
        Reset(device, memoryAllocationInfo, memoryPropertyFlags, physicalDeviceLimitNonCoherentAtomSize);
      }

      //! @brief Create a new memory.
      VUDeviceMemory(const VkDevice device, const VkMemoryRequirements& memoryRequirements,
                     const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties, const VkMemoryPropertyFlags propertyFlags,
                     const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize)
        : VUDeviceMemory()
      {
        Reset(device, memoryRequirements, physicalDeviceMemoryProperties, propertyFlags, physicalDeviceLimitNonCoherentAtomSize);
      }

      ~VUDeviceMemory() noexcept
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset() noexcept;

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkMemoryRequirements& memoryRequirements,
                 const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties, const VkMemoryPropertyFlags desiredPropertyFlags,
                 const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize);

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags,
                 const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize);

      //! @brief returns the managed handle and releases the ownership.
      VkDeviceMemory Release();

      //! @brief Get the device associated with this object
      VkDevice GetDevice() const
      {
        return m_deviceMemory.GetDevice();
      }

      //! @brief Get the memory handle associated with this object
      VkDeviceMemory Get() const
      {
        return m_deviceMemory.Get();
      }

      //! @brief Check if this memory object is valid
      bool IsValid() const
      {
        return m_deviceMemory.IsValid();
      }


      VkDeviceSize GetAllocationSize() const
      {
        return m_capacity;
      }


      VkMemoryPropertyFlags GetMemoryPropertyFlags() const
      {
        return m_memoryPropertyFlags;
      }

      void* MapMemory(const VkDeviceSize offset, const VkDeviceSize size, const VkMemoryMapFlags flags);

      const void* GetMappedMemoryPointer() const
      {
        FSLLOG3_DEBUG_WARNING_IF(!m_isMapped, "Requested a memory pointer for unmapped device memory, this will be a nullptr.")
        return m_pData;
      }

      void* GetMappedMemoryPointer()
      {
        FSLLOG3_DEBUG_WARNING_IF(!m_isMapped, "Requested a memory pointer for unmapped device memory, this will be a nullptr.")
        return m_pData;
      }

      bool IsMapped() const
      {
        return m_isMapped;
      }

      void InvalidateMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size);

      void FlushMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size);

      void UnmapMemory() noexcept;

      //! @brief Upload the given source data to device memory.
      //! @details If the memory is unmapped this method will map, upload (+flush if needed) and unmap it.
      //!          If the memory is mapped it will just upload (+flush if needed). However be sure that you are uploading inside the mapped area.
      //! @note    For frequently updated data its recommended to keep the memory mapped while the frequent update is necessary.
      void Upload(const VkDeviceSize dstOffset, const void* pSrcData, const VkDeviceSize srcDataSize, const VkMemoryMapFlags memoryMapFlags);

    private:
      void DoUnmapMemory() noexcept;
    };
  }
}

#endif
