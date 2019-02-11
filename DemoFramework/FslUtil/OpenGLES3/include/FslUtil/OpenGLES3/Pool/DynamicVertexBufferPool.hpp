#ifndef FSLUTIL_OPENGLES3_POOL_DYNAMICVERTEXBUFFERPOOL_HPP
#define FSLUTIL_OPENGLES3_POOL_DYNAMICVERTEXBUFFERPOOL_HPP
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

#include <FslUtil/OpenGLES3/GLBuffer.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <deque>
#include <vector>

namespace Fsl
{
  namespace GLES3
  {
    //! Simple vertex buffer pool, it uses a basic bucket allocation scheme.
    //! This is a experimental class.
    class DynamicVertexBufferPool
    {
      VertexDeclaration m_vertexDeclaration;
      uint32_t m_bufferVertexCapacity{0};
      uint32_t m_minimumPrimitiveVertexCount{0};
      std::deque<GLBuffer> m_buckets;
      uint32_t m_activeCount{0};
      bool m_isValid{false};

    public:
      struct VertexBufferEntry
      {
        GLuint VertexBuffer;
        uint32_t VertexCapacity;
        //! This is the start index inside the buffer that was applied to pMapped
        uint32_t UsedStartIndex;

        VertexBufferEntry(const GLuint vertexBuffer, const uint32_t vertexCapacity, const uint32_t usedStartIndex)
          : VertexBuffer(vertexBuffer)
          , VertexCapacity(vertexCapacity)
          , UsedStartIndex(usedStartIndex)
        {
        }
      };

      DynamicVertexBufferPool(const DynamicVertexBufferPool&) = delete;
      DynamicVertexBufferPool& operator=(const DynamicVertexBufferPool&) = delete;

      //! @brief Move assignment operator
      DynamicVertexBufferPool& operator=(DynamicVertexBufferPool&& other) noexcept
      {
        if (this != &other)
        {
          // Free existing resources then transfer the content of other to this one and fill other with default values
          if (IsValid())
          {
            Reset();
          }

          // Claim ownership here
          m_bufferVertexCapacity = other.m_bufferVertexCapacity;
          m_minimumPrimitiveVertexCount = other.m_minimumPrimitiveVertexCount;
          m_buckets = std::move(other.m_buckets);
          m_activeCount = other.m_activeCount;

          // Remove the data from other
          other.m_bufferVertexCapacity = 0;
          other.m_minimumPrimitiveVertexCount = 0;
          other.m_activeCount = 0;
        }
        return *this;
      }

      //! @brief Move constructor
      //! Transfer ownership from other to this
      DynamicVertexBufferPool(DynamicVertexBufferPool&& other) noexcept
        : m_bufferVertexCapacity(other.m_bufferVertexCapacity)
        , m_minimumPrimitiveVertexCount(other.m_minimumPrimitiveVertexCount)
        , m_buckets(std::move(other.m_buckets))
        , m_activeCount(other.m_activeCount)
      {
        // Remove the data from other
        other.m_bufferVertexCapacity = 0;
        other.m_minimumPrimitiveVertexCount = 0;
        other.m_activeCount = 0;
      }

      DynamicVertexBufferPool() = default;

      //! @param minimumPrimitiveVertexCount the minimum number of vertices needed to store a primitive of the type stored in the buffer.
      //                                     The capacity returned by NextFree will never be less than this!
      explicit DynamicVertexBufferPool(VertexDeclaration vertexDeclaration, const uint32_t bufferVertexCapacity,
                                       const uint32_t minimumPrimitiveVertexCount);
      ~DynamicVertexBufferPool();

      void Reset() noexcept;
      //! @param bufferVertexCapacity the number of vertices to allocate per buffer.
      //!                             We expect "(bufferVertexCapacity % minimumPrimitiveVertexCount) == 0"
      //! @param minimumPrimitiveVertexCount the minimum number of vertices needed to store a primitive of the type stored in the buffer.
      //                                     The capacity returned by NextFree will never be less than this!
      void Reset(VertexDeclaration vertexDeclaration, const uint32_t bufferVertexCapacity, const uint32_t minimumPrimitiveVertexCount);

      bool IsValid() const
      {
        return m_isValid;
      }

      void Clear();


      //! @brief Get the next free command buffer
      //! @param vertexCapacityHint is the desired capacity, but beware that the returned capacity can be less. However its
      //                            assumed that the entire returned capacity is claimed
      VertexBufferEntry NextFree(const uint32_t vertexCapacityHint);

      //! @brief get the vertex stride for the vertex buffers in this batch
      uint32_t GetVertexStride() const
      {
        return m_vertexDeclaration.VertexStride();
      }

    private:
    };
  }
}

#endif
