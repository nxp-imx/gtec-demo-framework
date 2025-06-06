#ifndef FSLUTIL_VULKAN1_0_MANAGED_VMVERTEXBUFFER_HPP
#define FSLUTIL_VULKAN1_0_MANAGED_VMVERTEXBUFFER_HPP
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
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpan.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil.hpp>
#include <FslGraphics/Vertices/VertexDeclarationSpan.hpp>
#include <FslUtil/Vulkan1_0/Common.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferUsage.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexElement.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <vulkan/vulkan.h>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl::Vulkan
{
  //! A managed vertex buffer is a high level 'vertex buffer' implementation giving easy access to common use cases.
  class VMVertexBuffer
  {
    std::shared_ptr<VMBufferManager> m_bufferManager;
    Vulkan::VUBufferMemory m_vertexBuffer;
    std::vector<VMVertexElement> m_vertexElements;
    uint32_t m_vertexCount = 0;
    uint32_t m_elementStride = 0;
    VMBufferUsage m_usage = VMBufferUsage::STATIC;

  public:
    VMVertexBuffer(const VMVertexBuffer&) = delete;
    VMVertexBuffer& operator=(const VMVertexBuffer&) = delete;

    // move assignment operator
    VMVertexBuffer& operator=(VMVertexBuffer&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_bufferManager = std::move(other.m_bufferManager);
        m_vertexBuffer = std::move(other.m_vertexBuffer);
        m_vertexElements = std::move(other.m_vertexElements);
        m_vertexCount = other.m_vertexCount;
        m_elementStride = other.m_elementStride;
        m_usage = other.m_usage;

        // Remove the data from other
        other.m_vertexCount = 0;
        other.m_elementStride = 0;
        other.m_usage = VMBufferUsage::STATIC;
      }
      return *this;
    }

    // move constructor
    VMVertexBuffer(VMVertexBuffer&& other) noexcept
      : m_bufferManager(std::move(other.m_bufferManager))
      , m_vertexBuffer(std::move(other.m_vertexBuffer))
      , m_vertexElements(std::move(other.m_vertexElements))
      , m_vertexCount(other.m_vertexCount)
      , m_elementStride(other.m_elementStride)
      , m_usage(other.m_usage)
    {
      // Remove the data from other
      other.m_vertexCount = 0;
      other.m_elementStride = 0;
      other.m_usage = VMBufferUsage::STATIC;
    }

    //! @brief Create a uninitialized vertex buffer
    VMVertexBuffer() = default;

    //! @brief Create a initialized vertex buffer

    VMVertexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlyFlexVertexSpan& vertexSpan, const VMBufferUsage usage)
    {
      Reset(bufferManager, vertexSpan, usage);
    }

    VMVertexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlyFlexVertexSpan& vertexSpan, const std::size_t elementCapacity,
                   const VMBufferUsage usage)
    {
      Reset(bufferManager, vertexSpan, elementCapacity, usage);
    }

    //! @brief Create a initialized dynamic vertex buffer to a given capacity
    VMVertexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, VertexDeclarationSpan vertexDeclaration)
      : VMVertexBuffer()
    {
      Reset(bufferManager, elementCapacity, vertexDeclaration);
    }


    ~VMVertexBuffer() noexcept = default;

    inline void Reset() noexcept
    {
      if (!m_vertexBuffer.IsValid())
      {
        return;
      }

      // Use destruction order
      m_usage = VMBufferUsage::STATIC;
      m_elementStride = 0;
      m_vertexCount = 0;
      m_vertexElements.clear();
      m_vertexBuffer.Reset();
      m_bufferManager.reset();
    }

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlyFlexVertexSpan& vertexSpan, const VMBufferUsage usage)
    {
      // Since no capacity is specified we just use the size
      Reset(bufferManager, vertexSpan, vertexSpan.size(), usage);
    }

    //! @brief Reset the buffer to contain the supplied elements
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlyFlexVertexSpan& vertexSpan, const std::size_t elementCapacity,
               const VMBufferUsage usage);

    //! @brief Reset the buffer to a dynamic buffer of the given capacity
    //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
    //! internally)
    void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, VertexDeclarationSpan vertexDeclaration);


    void SetData(ReadOnlyFlexSpan vertexSpan)
    {
      SetData(0u, vertexSpan);
    }

    void SetData(const uint32_t dstElementOffset, ReadOnlyFlexSpan vertexSpan);

    bool IsValid() const noexcept
    {
      return m_vertexBuffer.IsValid();
    }

    uint32_t GetVertexCount() const noexcept
    {
      return m_vertexCount;
    }

    //! @brief Get the element stride (size of one element in bytes)
    uint32_t GetElementStride() const noexcept
    {
      return m_elementStride;
    }

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const noexcept
    {
      return m_vertexBuffer.GetDevice();
    }

    //! @brief Get the associated 'buffer'
    VkBuffer GetBuffer() const noexcept
    {
      return m_vertexBuffer.GetBuffer();
    }

    const VkBuffer* GetBufferPointer() const noexcept
    {
      return m_vertexBuffer.GetBufferPointer();
    }

    //! @brief Get the vertex element index, throws a NotSupportedException if the usage and usageIndex combo wasn't found
    int32_t GetVertexElementIndex(const VertexElementUsage usage, const uint32_t usageIndex) const;

    //! @brief Find the element index of for the given usage and usageIndex (if not found <0 is returned)
    int32_t VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const;

    const VMVertexElement& GetVertexElementAt(const uint32_t vertexElementIndex) const;

    const VMVertexElement& GetVertexElement(const VertexElementUsage usage, const uint32_t usageIndex) const;
  };
}

#endif
