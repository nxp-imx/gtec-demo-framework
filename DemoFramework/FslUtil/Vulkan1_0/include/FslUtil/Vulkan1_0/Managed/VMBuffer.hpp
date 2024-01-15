#ifndef FSLUTIL_VULKAN1_0_MANAGED_VMBUFFER_HPP
#define FSLUTIL_VULKAN1_0_MANAGED_VMBUFFER_HPP
/****************************************************************************************************************************************************
 * Copyright 2021-2022 NXP
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
#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferUsage.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <memory>
#include <utility>

namespace Fsl::Vulkan
{
  //! A managed buffer is a high level 'buffer' implementation giving easy access to common use cases.
  class VMBuffer
  {
    std::shared_ptr<VMBufferManager> m_bufferManager;
    Vulkan::VUBufferMemory m_buffer;
    uint32_t m_elementCapacity = 0;
    uint32_t m_elementStride = 0;
    VMBufferUsage m_usage = VMBufferUsage::STATIC;

  public:
    VMBuffer(const VMBuffer&) = delete;
    VMBuffer& operator=(const VMBuffer&) = delete;

    // move assignment operator
    VMBuffer& operator=(VMBuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_bufferManager = std::move(other.m_bufferManager);
        m_buffer = std::move(other.m_buffer);
        m_elementCapacity = other.m_elementCapacity;
        m_elementStride = other.m_elementStride;
        m_usage = other.m_usage;

        // Remove the data from other
        other.m_elementCapacity = 0;
        other.m_elementStride = 0;
        other.m_usage = VMBufferUsage::STATIC;
      }
      return *this;
    }

    // move constructor
    VMBuffer(VMBuffer&& other) noexcept
      : m_bufferManager(std::move(other.m_bufferManager))
      , m_buffer(std::move(other.m_buffer))
      , m_elementCapacity(other.m_elementCapacity)
      , m_elementStride(other.m_elementStride)
      , m_usage(other.m_usage)
    {
      // Remove the data from other
      other.m_elementCapacity = 0;
      other.m_elementStride = 0;
      other.m_usage = VMBufferUsage::STATIC;
    }

    //! @brief Create a uninitialized buffer
    VMBuffer() = default;

    //! @brief Create a initialized buffer
    VMBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, ReadOnlyFlexSpan span, const VkBufferUsageFlags bufferUsageFlags,
             const VMBufferUsage usage)
      : VMBuffer(bufferManager, span, span.size(), bufferUsageFlags, usage)
    {
      Reset(bufferManager, span, bufferUsageFlags, usage);
    }

    VMBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, ReadOnlyFlexSpan span, const std::size_t elementCapacity,
             const VkBufferUsageFlags bufferUsageFlags, const VMBufferUsage usage)
      : VMBuffer()
    {
      Reset(bufferManager, span, elementCapacity, bufferUsageFlags, usage);
    }


    //! @brief Create a initialized dynamic buffer to a given capacity
    VMBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, const uint32_t elementStride,
             const VkBufferUsageFlags bufferUsageFlags)
      : VMBuffer()
    {
      Reset(bufferManager, elementCapacity, elementStride, bufferUsageFlags);
    }


    ~VMBuffer() noexcept = default;

    inline void Reset() noexcept
    {
      if (!m_buffer.IsValid())
      {
        return;
      }

      // Use destruction order
      m_usage = VMBufferUsage::STATIC;
      m_elementStride = 0;
      m_elementCapacity = 0;
      m_buffer.Reset();
      m_bufferManager.reset();
    }

    //! @brief Reset the buffer to a dynamic buffer of the given capacity
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, const uint32_t elementStride,
               const VkBufferUsageFlags bufferUsageFlags);

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, ReadOnlyFlexSpan span, const std::size_t elementCapacity,
               const VkBufferUsageFlags bufferUsageFlags, const VMBufferUsage usage);

    void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, ReadOnlyFlexSpan span, const VkBufferUsageFlags bufferUsageFlags,
               const VMBufferUsage usage)
    {
      Reset(bufferManager, span, span.size(), bufferUsageFlags, usage);
    }


    void SetData(ReadOnlyFlexSpan span)
    {
      SetData(0u, span);
    }

    void SetData(const uint32_t dstIndex, ReadOnlyFlexSpan span);

    bool IsValid() const noexcept
    {
      return m_buffer.IsValid();
    }

    //! @brief Get the element stride (size of one element in bytes)
    uint32_t GetElementStride() const noexcept
    {
      return m_elementStride;
    }

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const noexcept
    {
      return m_buffer.GetDevice();
    }

    //! @brief Get the associated 'buffer'
    VkBuffer GetBuffer() const noexcept
    {
      return m_buffer.GetBuffer();
    }

    const VkBuffer* GetBufferPointer() const noexcept
    {
      return m_buffer.GetBufferPointer();
    }
  };
}

#endif
