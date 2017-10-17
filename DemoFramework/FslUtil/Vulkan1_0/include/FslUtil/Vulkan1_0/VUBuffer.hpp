#ifndef FSLUTIL_VULKAN1_0_VUBUFFER_HPP
#define FSLUTIL_VULKAN1_0_VUBUFFER_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2017 Freescale Semiconductor, Inc.
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
#include <FslUtil/Vulkan1_0/Extend/BufferEx.hpp>
#include <FslUtil/Vulkan1_0/Extend/DeviceMemoryEx.hpp>
#include <glm/matrix.hpp>

namespace Fsl
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct Matrix;

  namespace Vulkan
  {
    class VUDevice;
    struct VUPhysicalDeviceRecord;

    //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
    class VUBuffer
    {
      BufferEx m_buffer;
      DeviceMemoryEx m_memory;
      VkDescriptorBufferInfo m_descriptor;
      VkDeviceSize m_bufferSegments;
      void* m_pMapped;
    public:
      VUBuffer(const VUBuffer&) = delete;
      VUBuffer& operator=(const VUBuffer&) = delete;

      //! @brief Move assignment operator
      VUBuffer& operator=(VUBuffer&& other);

      //! @brief Move constructor
      //! Transfer ownership from other to this
      VUBuffer(VUBuffer&& other);

      //! @brief Move objects into this object
      VUBuffer(BufferEx&& buffer, DeviceMemoryEx&& deviceMemory, const VkDescriptorBufferInfo& descriptor);

      //! @brief Create a 'invalid' instance (use Reset to populate it)
      VUBuffer();
      VUBuffer(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize subBufferCount = 1);
      VUBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize subBufferCount = 1);
      VUBuffer(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags);

      ~VUBuffer()
      {
        Reset();
      }

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset();

      void Reset(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize subBufferCount = 1);
      void Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize subBufferCount = 1);
      void Reset(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags);

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

      VkDescriptorBufferInfo GetDescriptor() const
      {
        return m_descriptor;
      }

      const VkDescriptorBufferInfo* GetDescriptorPointer() const
      {
        return &m_descriptor;
      }

      VkDeviceSize GetAllocationSize() const
      {
        return m_memory.GetAllocationSize();
      }

      VkDeviceSize GetSegmentCount() const
      {
        return m_bufferSegments;
      }

      VkDeviceSize GetSegmentSize() const
      {
        return GetAllocationSize() / m_bufferSegments;
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
      //! @param offset (Optional) Byte offset from beginning
      //! @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
      //! @return the mapped pointer
      void* Map(const VkDeviceSize offset = 0, const VkDeviceSize size = VK_WHOLE_SIZE);

      //! @brief Unmap a mapped memory range
      //! @note Does not return a result as vkUnmapMemory can't fail
      void Unmap();

      // @brief Attach the allocated memory block to the buffer
      // @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
      // @return VkResult of the bindBufferMemory call
      void Bind(const VkDeviceSize offset = 0);


      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const int32_t scalar);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const float scalar);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::vec2& vec);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Vector2& vec);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::vec3& vec);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Vector3& vec);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::vec4& vec);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Vector4& vec);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::mat3& mat);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::mat2& mat);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::mat4& mat);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Matrix& mat);
      void Upload(const uint32_t offset, const VkMemoryMapFlags flags, const void*const pData, const uint32_t dataSize);

      //! @brief A 'exception safe' way of mapping a object
      class ScopedMap
      {
        VUBuffer& m_rBuffer;
      public:
        void* pMappedMemory;

        ScopedMap(VUBuffer& rBuffer, const VkDeviceSize offset = 0, const VkDeviceSize size = VK_WHOLE_SIZE)
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
      void SetMemberVarsOnNewReset(const VkDeviceSize bufferSegments);
    };
  }
}

#endif
