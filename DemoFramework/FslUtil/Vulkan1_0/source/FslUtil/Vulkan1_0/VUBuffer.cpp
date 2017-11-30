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

#include <FslUtil/Vulkan1_0/VUBuffer.hpp>
#include <FslUtil/Vulkan1_0/VUDevice.hpp>
#include <FslBase/Bits/AlignmentUtil.hpp>
#include <FslBase/Log/Log.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector3.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslBase/Math/Matrix.hpp>
#include <RapidVulkan/Check.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>
#include <glm/gtc/type_ptr.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    VUBuffer& VUBuffer::operator = (VUBuffer&& other)
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        if (IsValid())
          Reset();

        // Claim ownership here
        m_buffer = std::move(other.m_buffer);
        m_memory = std::move(other.m_memory);
        m_descriptor = other.m_descriptor;
        m_pMapped = other.m_pMapped;

        // Remove the data from other
        other.m_descriptor = VkDescriptorBufferInfo{};
        other.m_pMapped = nullptr;
      }
      return *this;
    }


    VUBuffer::VUBuffer(VUBuffer&& other)
      : m_buffer(std::move(other.m_buffer))
      , m_memory(std::move(other.m_memory))
      , m_descriptor(other.m_descriptor)
      , m_pMapped(other.m_pMapped)
    {
      // Remove the data from other
      other.m_descriptor = VkDescriptorBufferInfo{};
      other.m_pMapped = nullptr;
    }


    VUBuffer::VUBuffer(BufferEx&& buffer, DeviceMemoryEx&& deviceMemory, const VkDescriptorBufferInfo& descriptor)
      : m_buffer(std::move(buffer))
      , m_memory(std::move(deviceMemory))
      , m_descriptor(descriptor)
      , m_pMapped(nullptr)
    {
      const bool hasOneValid = m_buffer.IsValid() || m_memory.IsValid();
      if (m_buffer.IsValid() != hasOneValid || m_memory.IsValid() != hasOneValid)
        throw std::invalid_argument("Either all objects has to be valid or none must be");

      if (m_buffer.GetDevice() != m_memory.GetDevice())
        throw std::invalid_argument("All objects must belong to the same device");
    }


    VUBuffer::VUBuffer()
      : m_buffer()
      , m_memory()
      , m_descriptor{}
      , m_pMapped(nullptr)
    {
    }


    VUBuffer::VUBuffer(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize subBufferCount)
      : VUBuffer()
    {
      Reset(device, bufferCreateInfo, memoryPropertyFlags, subBufferCount);
    }


    VUBuffer::VUBuffer(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize subBufferCount)
      : VUBuffer()
    {
      Reset(physicalDevice, device, bufferCreateInfo, memoryPropertyFlags, subBufferCount);
    }


    VUBuffer::VUBuffer(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
      : VUBuffer()
    {
      Reset(device, bufferCreateInfo, memoryAllocationInfo, memoryPropertyFlags);
    }


    void VUBuffer::Reset()
    {
      if (!IsValid())
        return;

      assert(m_buffer.IsValid());
      assert(m_memory.IsValid());
      //! If this goes of a mapped buffer is being reset, unmap it!
      if (m_pMapped != nullptr)
      {
        FSLLOG_DEBUG_WARNING("Resetting a mapped buffer, please Unmap before calling Reset");
        Unmap();
      }

      m_buffer.Reset();
      m_memory.Reset();
      m_descriptor = VkDescriptorBufferInfo{};
      m_pMapped = nullptr;
    }


    void VUBuffer::Reset(const VUDevice& device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize bufferSegments)
    {
      Reset(device.GetPhysicalDevice(), device.Get(), bufferCreateInfo, memoryPropertyFlags, bufferSegments);
    }


    void VUBuffer::Reset(const VUPhysicalDeviceRecord& physicalDevice, const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryPropertyFlags memoryPropertyFlags, const VkDeviceSize bufferSegments)
    {
      if (IsValid())
        Reset();

      try
      {
        VkBufferCreateInfo bufferCreateInfoEx = bufferCreateInfo;
        if (bufferSegments > 1)
          bufferCreateInfoEx.size = AlignmentUtil::GetByteSize(bufferCreateInfoEx.size, physicalDevice.Properties.limits.minUniformBufferOffsetAlignment) * bufferSegments;

        m_buffer.Reset(device, bufferCreateInfoEx);
        const VkMemoryRequirements memoryRequirements = m_buffer.GetBufferMemoryRequirements();

        VkMemoryAllocateInfo memoryAllocationInfo{};
        memoryAllocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocationInfo.allocationSize = memoryRequirements.size;
        // Find a memoryTypeIndex that matches the requested memoryPropertyFlags
        memoryAllocationInfo.memoryTypeIndex = physicalDevice.GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryPropertyFlags);

        m_memory.Reset(device, memoryAllocationInfo, memoryPropertyFlags);

        RAPIDVULKAN_CHECK(vkBindBufferMemory(device, m_buffer.Get(), m_memory.Get(), 0));

        SetMemberVarsOnNewReset(bufferSegments);
      }
      catch (const std::exception&)
      {
        // Cleanup this object so its back to its default invalid object state
        if (m_buffer.IsValid())
          m_buffer.Reset();
        if (m_memory.IsValid())
          m_memory.Reset();
        throw;
      }
    }


    void VUBuffer::Reset(const VkDevice device, const VkBufferCreateInfo& bufferCreateInfo, const VkMemoryAllocateInfo& memoryAllocationInfo, const VkMemoryPropertyFlags memoryPropertyFlags)
    {
      if (IsValid())
        Reset();

      try
      {
        m_buffer.Reset(device, bufferCreateInfo);
        m_memory.Reset(device, memoryAllocationInfo, memoryPropertyFlags);

        RAPIDVULKAN_CHECK(vkBindBufferMemory(device, m_buffer.Get(), m_memory.Get(), 0));

        SetMemberVarsOnNewReset(1);
      }
      catch (const std::exception&)
      {
        // Cleanup this object so its back to its default invalid object state
        if (m_buffer.IsValid())
          m_buffer.Reset();
        if (m_memory.IsValid())
          m_memory.Reset();
        throw;
      }
    }


    void* VUBuffer::Map(const VkDeviceSize offset, const VkDeviceSize size)
    {
      assert(IsValid());
      void* pMapped;
      RAPIDVULKAN_CHECK(vkMapMemory(m_buffer.GetDevice(), m_memory.Get(), offset, size, 0, &pMapped));
      m_pMapped = pMapped;
      return m_pMapped;
    }


    void VUBuffer::Unmap()
    {
      FSLLOG_DEBUG_WARNING_IF(!IsValid(), "Unmap called on a invalid object, request ignored.");
      FSLLOG_DEBUG_WARNING_IF(m_pMapped == nullptr, "Unmap called on a unmapped object, request ignored.");
      if (m_pMapped)
      {
        vkUnmapMemory(m_buffer.GetDevice(), m_memory.Get());
        m_pMapped = nullptr;
      }
    }


    void VUBuffer::Bind(const VkDeviceSize offset)
    {
      assert(IsValid());
      RAPIDVULKAN_CHECK(vkBindBufferMemory(m_buffer.GetDevice(), m_buffer.Get(), m_memory.Get(), offset));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const int32_t scalar)
    {
      m_memory.Upload(offset, flags, &scalar, sizeof(int32_t));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const float scalar)
    {
      m_memory.Upload(offset, flags, &scalar, sizeof(float));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::vec2& vec)
    {
      m_memory.Upload(offset, flags, glm::value_ptr(vec), sizeof(float) * 2);
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Vector2& vec)
    {
      m_memory.Upload(offset, flags, vec.DirectAccess(), sizeof(Vector2));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::vec3& vec)
    {
      m_memory.Upload(offset, flags, glm::value_ptr(vec), sizeof(float) * 3);
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Vector3& vec)
    {
      m_memory.Upload(offset, flags, vec.DirectAccess(), sizeof(Vector3));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::vec4& vec)
    {
      m_memory.Upload(offset, flags, glm::value_ptr(vec), sizeof(float) * 4);
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Vector4& vec)
    {
      m_memory.Upload(offset, flags, vec.DirectAccess(), sizeof(Vector4));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::mat2& mat)
    {
      // Only upload relevant columns and rows.
      m_memory.Upload(offset, flags, glm::value_ptr(glm::mat4(mat)), sizeof(float) * 6);
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::mat3& mat)
    {
      // Only upload relevant columns and rows.
      m_memory.Upload(offset, flags, glm::value_ptr(glm::mat4(mat)), sizeof(float) * 11);
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const glm::mat4& mat)
    {
      m_memory.Upload(offset, flags, glm::value_ptr(mat), sizeof(float) * 16);
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const Matrix& mat)
    {
      m_memory.Upload(offset, flags, mat.DirectAccess(), sizeof(Matrix));
    }


    void VUBuffer::Upload(const uint32_t offset, const VkMemoryMapFlags flags, const void*const pData, const uint32_t dataSize)
    {
      m_memory.Upload(offset, flags, pData, dataSize);
    }


    void VUBuffer::SetMemberVarsOnNewReset(const VkDeviceSize bufferSegments)
    {
      // Default to the full size of this buffer
      m_descriptor = VkDescriptorBufferInfo{};
      m_descriptor.buffer = m_buffer.Get();
      m_descriptor.range = m_buffer.GetSize();
      m_bufferSegments = bufferSegments;
      m_pMapped = nullptr;
    }
  }
}
