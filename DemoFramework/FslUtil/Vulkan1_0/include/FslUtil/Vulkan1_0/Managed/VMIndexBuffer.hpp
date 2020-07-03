#ifndef FSLUTIL_VULKAN1_0_MANAGED_VMINDEXBUFFER_HPP
#define FSLUTIL_VULKAN1_0_MANAGED_VMINDEXBUFFER_HPP
/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include <FslBase/ReadOnlySpan.hpp>
#include <FslBase/ReadOnlySpanUtil.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferUsage.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/VUBufferMemory.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <cassert>
#include <memory>
#include <utility>
#include <vector>

namespace Fsl
{
  namespace Vulkan
  {
    //! A managed index buffer is a high level 'index buffer' implementation giving easy access to common use cases.
    class VMIndexBuffer
    {
      std::shared_ptr<VMBufferManager> m_bufferManager;
      Vulkan::VUBufferMemory m_indexBuffer;
      uint32_t m_indexCount = 0;
      uint32_t m_elementStride = 0;
      VMBufferUsage m_usage = VMBufferUsage::STATIC;

    public:
      VMIndexBuffer(const VMIndexBuffer&) = delete;
      VMIndexBuffer& operator=(const VMIndexBuffer&) = delete;

      // move assignment operator
      VMIndexBuffer& operator=(VMIndexBuffer&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          Reset();

          // Claim ownership here
          m_bufferManager = std::move(other.m_bufferManager);
          m_indexBuffer = std::move(other.m_indexBuffer);
          m_indexCount = other.m_indexCount;
          m_elementStride = other.m_elementStride;
          m_usage = other.m_usage;

          // Remove the data from other
          other.m_indexCount = 0;
          other.m_elementStride = 0;
          other.m_usage = VMBufferUsage::STATIC;
        }
        return *this;
      }

      // move constructor
      VMIndexBuffer(VMIndexBuffer&& other) noexcept
        : m_bufferManager(std::move(other.m_bufferManager))
        , m_indexBuffer(std::move(other.m_indexBuffer))
        , m_indexCount(other.m_indexCount)
        , m_elementStride(other.m_elementStride)
        , m_usage(other.m_usage)
      {
        // Remove the data from other
        other.m_indexCount = 0;
        other.m_elementStride = 0;
        other.m_usage = VMBufferUsage::STATIC;
      }

      //! @brief Create a uninitialized index buffer
      VMIndexBuffer() = default;

      //! @brief Create a initialized index buffer
      [[deprecated("use the ReadOnlySpan variant")]] VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager,
                                                                   const uint16_t* const pIndices, const std::size_t elementCount,
                                                                   const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(pIndices, elementCount), usage);
      }

      //! @brief Create a initialized index buffer
      [[deprecated("use the ReadOnlySpan variant")]] VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager,
                                                                   const uint32_t* const pIndices, const std::size_t elementCount,
                                                                   const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(pIndices, elementCount), usage);
      }

      //! @brief Create a initialized index buffer
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlySpan<uint16_t> indices, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, indices, usage);
      }

      //! @brief Create a initialized index buffer
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlySpan<uint32_t> indices, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, indices, usage);
      }

      //! @brief Create a initialized dynamic index buffer to a given capacity
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, const uint32_t elementStride)
        : VMIndexBuffer()
      {
        Reset(bufferManager, elementCapacity, elementStride);
      }

      //! @brief Create a initialized index buffer
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::vector<uint16_t>& indices, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      //! @brief Create a initialized index buffer
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::vector<uint32_t>& indices, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      //! @brief Create a initialized index buffer
      template <std::size_t TSize>
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::array<uint16_t, TSize>& indices, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      //! @brief Create a initialized index buffer
      template <std::size_t TSize>
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const std::array<uint32_t, TSize>& indices, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      ~VMIndexBuffer() noexcept = default;

      inline void Reset() noexcept
      {
        if (!m_indexBuffer.IsValid())
        {
          return;
        }

        m_usage = VMBufferUsage::STATIC;
        m_elementStride = 0;
        m_indexCount = 0;
        m_indexBuffer.Reset();
        m_bufferManager.reset();
      }

      //! @brief Reset the buffer to a dynamic buffer of the given capacity
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity, const uint32_t elementStride);

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      [[deprecated("use the ReadOnlySpan variant")]] void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const uint16_t* const pIndices,
                                                                const std::size_t elementCount, const VMBufferUsage usage)
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(pIndices, elementCount), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      [[deprecated("use the ReadOnlySpan variant")]] void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const uint32_t* const pIndices,
                                                                const std::size_t elementCount, const VMBufferUsage usage)
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(pIndices, elementCount), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlySpan<uint16_t> indices, const VMBufferUsage usage)
      {
        DoReset(bufferManager, indices.data(), indices.size(), sizeof(uint16_t), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const ReadOnlySpan<uint32_t> indices, const VMBufferUsage usage)
      {
        DoReset(bufferManager, indices.data(), indices.size(), sizeof(uint32_t), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::vector<uint16_t>& indices, const VMBufferUsage usage)
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::vector<uint32_t>& indices, const VMBufferUsage usage)
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      template <std::size_t TSize>
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::array<uint16_t, TSize>& indices, const VMBufferUsage usage)
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      template <std::size_t TSize>
      void Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::array<uint32_t, TSize>& indices, const VMBufferUsage usage)
      {
        Reset(bufferManager, ReadOnlySpanUtil::AsSpan(indices), usage);
      }


      [[deprecated("use the ReadOnlySpan variant")]] void SetData(const uint16_t* pIndices, const std::size_t elementCount)
      {
        SetData(ReadOnlySpanUtil::AsSpan(pIndices, elementCount));
      }

      [[deprecated("use the ReadOnlySpan variant")]] void SetData(const uint32_t* pIndices, const std::size_t elementCount)
      {
        SetData(ReadOnlySpanUtil::AsSpan(pIndices, elementCount));
      }

      void SetData(const ReadOnlySpan<uint16_t> indices)
      {
        DoSetData(indices.data(), indices.size(), sizeof(uint16_t));
      }

      void SetData(const ReadOnlySpan<uint32_t> indices)
      {
        DoSetData(indices.data(), indices.size(), sizeof(uint32_t));
      }


      void SetData(const std::vector<uint16_t>& indices)
      {
        SetData(ReadOnlySpanUtil::AsSpan(indices));
      }


      void SetData(const std::vector<uint32_t>& indices)
      {
        SetData(ReadOnlySpanUtil::AsSpan(indices));
      }


      template <std::size_t TSize>
      void SetData(const std::array<uint16_t, TSize>& indices)
      {
        SetData(ReadOnlySpanUtil::AsSpan(indices));
      }


      template <std::size_t TSize>
      void SetData(const std::array<uint32_t, TSize>& indices)
      {
        SetData(ReadOnlySpanUtil::AsSpan(indices));
      }


      bool IsValid() const
      {
        return m_indexBuffer.IsValid();
      }

      uint32_t GetIndexCount() const
      {
        return m_indexCount;
      }

      //! @brief Get the element stride (size of one element in bytes)
      uint32_t GetElementStride() const
      {
        return m_elementStride;
      }

      //! @brief Get the associated 'Device'
      VkDevice GetDevice() const
      {
        return m_indexBuffer.GetDevice();
      }

      //! @brief Get the associated 'buffer'
      VkBuffer GetBuffer() const
      {
        return m_indexBuffer.GetBuffer();
      }

      const VkBuffer* GetBufferPointer() const
      {
        return m_indexBuffer.GetBufferPointer();
      }

      VkIndexType GetIndexType() const
      {
        assert(m_elementStride == 2 || m_elementStride == 4);
        return m_elementStride == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
      }

    private:
      //! @brief Create a initialized index buffer
      VMIndexBuffer(const std::shared_ptr<VMBufferManager>& bufferManager, const void* const pIndices, const std::size_t elementCount,
                    const uint32_t elementStride, const VMBufferUsage usage)
        : VMIndexBuffer()
      {
        DoReset(bufferManager, pIndices, elementCount, elementStride, usage);
      }

      //! @brief Reset the buffer to contain the supplied elements
      //! @note  This is a very slow operation and its not recommended for updating the content of the buffer (since it creates a new buffer
      //! internally)
      void DoReset(const std::shared_ptr<VMBufferManager>& bufferManager, const void* pIndices, const std::size_t elementCount,
                   const uint32_t elementStride, const VMBufferUsage usage);

      void DoSetData(const void* pIndices, const std::size_t elementCount, const uint32_t elementStride);
    };
  }
}

#endif
