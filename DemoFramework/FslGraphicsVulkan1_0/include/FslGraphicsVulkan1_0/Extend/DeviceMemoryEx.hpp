#ifndef FSLGRAPHICSVULKAN1_0_EXTEND_DEVICEMEMORYEX_HPP
#define FSLGRAPHICSVULKAN1_0_EXTEND_DEVICEMEMORYEX_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslGraphicsVulkan1_0/Common.hpp>
#include <FslGraphicsVulkan1_0/Memory.hpp>
#include <vulkan/vulkan.h>
#include <FslBase/Noncopyable.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    // This object is movable so it can be thought of as behaving int he same was as a unique_ptr and is compatible with std containers
    class DeviceMemoryEx : private Noncopyable
    {
      Memory m_deviceMemory;
      VkMemoryPropertyFlags m_memoryPropertyFlags;
      VkMemoryAllocateInfo m_allocateInfo;
      void* m_pData;
      bool m_isMapped;
    public:
      // move assignment operator
      DeviceMemoryEx& operator=(DeviceMemoryEx&& other);
      // move constructor
      DeviceMemoryEx(DeviceMemoryEx&& other);

      DeviceMemoryEx();

      //! @brief Create a new memory.
      DeviceMemoryEx(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags);

      //! @brief Create a new memory.
      DeviceMemoryEx(const VkDevice device, const VkMemoryRequirements& memoryRequirements, const uint32_t memoryTypeCount, const VkMemoryType* memoryTypes, const VkMemoryPropertyFlags propertyFlags);

      ~DeviceMemoryEx();

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkMemoryRequirements& memoryRequirements, const uint32_t memoryTypeCount, const VkMemoryType* memoryTypes, const VkMemoryPropertyFlags propertyFlags);

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags);

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
        return m_allocateInfo.allocationSize;
      }


      VkMemoryPropertyFlags GetMemoryPropertyFlags() const
      {
        return m_memoryPropertyFlags;
      }

      void MapMemory(const VkDeviceSize offset, const VkDeviceSize size, const VkMemoryMapFlags flags);

      void* GetMappedMemoryPointer() const;

      void InvalidateMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size);

      void FlushMappedMemoryRanges(const VkDeviceSize offset, const VkDeviceSize size);

      void UnmapMemory();

      void Upload(const VkDeviceSize offset, const VkMemoryMapFlags flags, const void* uploadData, const size_t uploadDataSize);
    };
  }
}

#endif
