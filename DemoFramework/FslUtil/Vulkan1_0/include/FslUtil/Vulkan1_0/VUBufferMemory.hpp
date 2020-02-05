#ifndef FSLUTIL_VULKAN1_0_VUBUFFERMEMORY_HPP
#define FSLUTIL_VULKAN1_0_VUBUFFERMEMORY_HPP
/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDeviceMemory.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Core.hpp>
#include <array>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    class VUDevice;
    struct VUPhysicalDeviceRecord;

    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VUBufferMemory
    {
      VUBuffer m_buffer;
      VUDeviceMemory m_memory;
      VkDescriptorBufferInfo m_descriptorBufferInfo{};

    public:
      VUBufferMemory(const VUBufferMemory&) = delete;
      VUBufferMemory& operator=(const VUBufferMemory&) = delete;

      //! @brief Move assignment operator
      VUBufferMemory& operator=(VUBufferMemory&& other) noexcept;

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VUBufferMemory(VUBufferMemory&& other) noexcept;

      //! @brief Move objects into this object
      VUBufferMemory(VUBuffer&& buffer, VUDeviceMemory&& deviceMemory, const VkDescriptorBufferInfo& descriptor);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUBufferMemory() = default;

      VUBufferMemory(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
        : VUBufferMemory()
      {
        Reset(device, bufferCreateInfo, memoryPropertyFlags);
      }


      VUBufferMemory(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo,
                     const VkMemoryPropertyFlags memoryPropertyFlags)
        : VUBufferMemory()
      {
        Reset(physicalDevice, device, bufferCreateInfo, memoryPropertyFlags);
      }

      // VUBuffer(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo,
      //         const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize)
      //  : VUBuffer()
      //{
      //  Reset(device, bufferCreateInfo, memoryAllocationInfo, memoryPropertyFlags, physicalDeviceLimitNonCoherentAtomSize);
      //}


      ~VUBufferMemory() noexcept
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset() noexcept;

      void Reset(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags);
      void Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo,
                 const VkMemoryPropertyFlags memoryPropertyFlags);

      //! @brief Reset the buffer object
      //! @param memoryPropertyFlags Be sure to specify the property flags from
      //! physicalDevice.MemoryProperties.memoryTypes[memoryAllocationInfo.memoryTypeIndex].propertyFlags so that all the memory property flags are
      //! used.
      // void Reset(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo,
      //           const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize physicalDeviceLimitNonCoherentAtomSize);

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
        return m_memory.Get();
      }

      VkDescriptorBufferInfo GetDescriptorBufferInfo() const
      {
        return m_descriptorBufferInfo;
      }

      const VkDescriptorBufferInfo* GetDescriptorBufferInfoPointer() const
      {
        return &m_descriptorBufferInfo;
      }


      //! @brief Get the originally requested buffer size
      VkDeviceSize GetBufferSize() const
      {
        return m_buffer.GetSize();
      }

      //! @brief Beware this might be larger than the initial requested buffer size due to alignment and other requirements.
      VkDeviceSize GetAllocationSize() const
      {
        return m_memory.GetAllocationSize();
      }

      VkBufferUsageFlags GetUsageFlags() const
      {
        return m_buffer.GetUsage();
      }

      VkMemoryPropertyFlags GetMemoryPropertyFlags() const
      {
        return m_memory.GetMemoryPropertyFlags();
      }


      const void* GetMappedPointer() const
      {
        return m_memory.GetMappedMemoryPointer();
      }


      void* GetMappedPointer()
      {
        return m_memory.GetMappedMemoryPointer();
      }


      //! @brief Check if this object contains a valid resource
      inline bool IsValid() const
      {
        return m_buffer.IsValid();
      }

      bool IsMapped() const
      {
        return m_memory.IsMapped();
      }

      //! @brief Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
      //! @param offset (Optional) Byte offset from beginning
      //! @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
      //! @return the mapped pointer
      void* Map(const VkDeviceSize offset = 0, const VkDeviceSize size = VK_WHOLE_SIZE, const VkMemoryMapFlags flags = 0)
      {
        return m_memory.MapMemory(offset, size, flags);
      }

      //! @brief Unmap a mapped memory range
      //! @note Does not return a result as vkUnmapMemory can't fail
      void Unmap()
      {
        m_memory.UnmapMemory();
      }

      // @brief Attach the allocated memory block to the buffer
      // @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
      // @return VkResult of the bindBufferMemory call
      void Bind(const VkDeviceSize offset = 0);


      void Upload(const uint32_t offset, const void* const pData, const VkDeviceSize dataSize, const VkMemoryMapFlags flags = 0)
      {
        m_memory.Upload(offset, pData, dataSize, flags);
      }

      //! @brief A 'exception safe' way of mapping a object
      class ScopedMap
      {
        VUBufferMemory& m_rBuffer;

      public:
        void* pMappedMemory;

        ScopedMap(VUBufferMemory& rBuffer, const VkDeviceSize offset = 0, const VkDeviceSize size = VK_WHOLE_SIZE)
          : m_rBuffer(rBuffer)
        {
          pMappedMemory = m_rBuffer.Map(offset, size);
        }

        ~ScopedMap()
        {
          pMappedMemory = nullptr;
          m_rBuffer.Unmap();
        }
      };

    private:
      void SetMemberVarsOnNewReset();
    };
  }
}

#endif
