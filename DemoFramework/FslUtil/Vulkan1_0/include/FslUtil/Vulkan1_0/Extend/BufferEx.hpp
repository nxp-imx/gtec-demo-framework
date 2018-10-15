#ifndef FSLUTIL_VULKAN1_0_EXTEND_BUFFEREX_HPP
#define FSLUTIL_VULKAN1_0_EXTEND_BUFFEREX_HPP
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
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <RapidVulkan/Buffer.hpp>
//#include <FslUtil/Vulkan1_0/SafeType/BufferCreateInfoCopy.hpp>
#include <vulkan/vulkan.h>
#include <FslBase/Noncopyable.hpp>

namespace Fsl
{
  namespace Vulkan
  {
    // Extends the Buffer class with 'VkBufferCreateInfo' and 'VkAccessFlags' so its available for query at a later time.
    // This object is movable so it can be thought of as behaving int he same was as a unique_ptr and is compatible with std containers
    class BufferEx : private Noncopyable
    {
      RapidVulkan::Buffer m_buffer;
      // BufferCreateInfoCopy m_createInfo;
      VkDeviceSize m_size;
      VkBufferUsageFlags m_usage;
      VkAccessFlags m_accessMask;

    public:
      // move assignment operator
      BufferEx& operator=(BufferEx&& other) noexcept;
      // move constructor
      BufferEx(BufferEx&& other) noexcept;

      BufferEx();

      //! @brief Create a new buffer.
      BufferEx(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage,
               const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* const pQueueFamilyIndices);

      //! @brief Create a new buffer.
      BufferEx(const VkDevice device, const VkBufferCreateInfo& createInfo);

      ~BufferEx();

      //! @brief Destroys any owned resources and resets the object to its default state.
      void Reset() noexcept;

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkBufferCreateFlags flags, const VkDeviceSize size, const VkBufferUsageFlags usage,
                 const VkSharingMode sharingMode, const uint32_t queueFamilyIndexCount, const uint32_t* const pQueueFamilyIndices);

      //! @brief Replaces the managed object with a new one (releasing the old)
      void Reset(const VkDevice device, const VkBufferCreateInfo& createInfo);

      //! @brief Get the device associated with this object
      VkDevice GetDevice() const
      {
        return m_buffer.GetDevice();
      }

      //! @brief Get the buffer handle associated with this object
      VkBuffer Get() const
      {
        return m_buffer.Get();
      }

      //! @brief Get a pointer to the associated resource handle
      const VkBuffer* GetPointer() const
      {
        return m_buffer.GetPointer();
      }

      //! @brief Check if this buffer object is valid
      bool IsValid() const
      {
        return m_buffer.IsValid();
      }


      VkMemoryRequirements GetBufferMemoryRequirements() const
      {
        return m_buffer.GetBufferMemoryRequirements();
      }


      VkAccessFlags GetAccessMask() const
      {
        return m_accessMask;
      }


      VkDeviceSize GetSize() const
      {
        // return m_createInfo.Get().size;
        return m_size;
      }


      VkBufferUsageFlags GetUsage() const
      {
        // return m_createInfo.Get().usage;
        return m_usage;
      }


      void CmdPipelineBarrier(const VkCommandBuffer cmdBuffer, const VkAccessFlags dstAccessMask);
    };
  }
}

#endif
