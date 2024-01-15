#ifndef FSLUTIL_VULKAN1_0_VUBUFFER_HPP
#define FSLUTIL_VULKAN1_0_VUBUFFER_HPP
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

// Make sure Common.hpp is the first include file (to make the error message as helpful as possible when disabled)
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/Buffer.hpp>
#include <vulkan/vulkan.h>

namespace Fsl::Vulkan
{
  // Extends the Buffer class with 'VkBufferCreateInfo' and 'VkAccessFlags' so its available for query at a later time.
  // This object is movable so it can be thought of as behaving int he same was as a unique_ptr and is compatible with std containers
  class VUBuffer
  {
    RapidVulkan::Buffer m_buffer;
    VkDeviceSize m_size{0};
    VkBufferUsageFlags m_usage{0};
    VkAccessFlags m_accessMask{0};

  public:
    VUBuffer(const VUBuffer&) = delete;
    VUBuffer& operator=(const VUBuffer&) = delete;

    // move assignment operator
    VUBuffer& operator=(VUBuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_buffer = std::move(other.m_buffer);
        // m_createInfo = std::move(other.m_createInfo);
        m_size = other.m_size;
        m_usage = other.m_usage;
        m_accessMask = other.m_accessMask;

        // Remove the data from other
        other.m_accessMask = 0;
        other.m_size = 0;
        other.m_usage = 0;
      }
      return *this;
    }

    // move constructor
    VUBuffer(VUBuffer&& other) noexcept
      : m_buffer(std::move(other.m_buffer))
      , m_size(other.m_size)
      , m_usage(other.m_usage)
      , m_accessMask(other.m_accessMask)
    {
      // Remove the data from other
      other.m_accessMask = 0;
      other.m_size = 0;
      other.m_usage = 0;
    }


    VUBuffer() = default;


    //! @brief Create a new buffer.
    VUBuffer(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage,
             const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* const pQueueFamilyIndices);

    //! @brief Create a new buffer.
    VUBuffer(const VkDevice device, const VkBufferCreateInfo& createInfo);

    ~VUBuffer() noexcept
    {
      Reset();
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage,
               const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* const pQueueFamilyIndices);

    //! @brief Replaces the managed object with a new one (releasing the old)
    void Reset(const VkDevice device, const VkBufferCreateInfo& createInfo);

    //! @brief Get the device associated with this object
    VkDevice GetDevice() const noexcept
    {
      return m_buffer.GetDevice();
    }

    //! @brief Get the buffer handle associated with this object
    VkBuffer Get() const noexcept
    {
      return m_buffer.Get();
    }

    //! @brief Get a pointer to the associated resource handle
    const VkBuffer* GetPointer() const noexcept
    {
      return m_buffer.GetPointer();
    }

    //! @brief Check if this buffer object is valid
    bool IsValid() const noexcept
    {
      return m_buffer.IsValid();
    }


    VkMemoryRequirements GetBufferMemoryRequirements() const noexcept
    {
      return m_buffer.GetBufferMemoryRequirements();
    }


    VkAccessFlags GetAccessMask() const noexcept
    {
      return m_accessMask;
    }


    VkDeviceSize GetSize() const noexcept
    {
      return m_size;
    }


    VkBufferUsageFlags GetUsage() const noexcept
    {
      return m_usage;
    }
  };
}

#endif
