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

#include <FslUtil/OpenGLES3/Pool/DynamicVertexBufferPool.hpp>
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <algorithm>
#include <cassert>
#include <limits>
#include <type_traits>
#include <utility>

namespace Fsl
{
  namespace GLES3
  {
    namespace
    {
      // We do this since we want to be confident that we can cast our UInt32_t's and std::size's to something we can pass to glDrawArrays
      // static_assert(BUCKET_SIZE <= static_cast<std::make_unsigned<GLsizei>::type>(std::numeric_limits<GLsizei>::max()),
      //              "We assume the bucketSize will be less than the max positive value of GLsizei");

      inline GLBuffer CreateBuffer(const VertexDeclaration& vertexDeclaration, const uint32_t vertexCount)
      {
        return GLBuffer(GL_ARRAY_BUFFER, nullptr, vertexCount, vertexDeclaration.VertexStride(), GL_DYNAMIC_DRAW);
      }
    }


    DynamicVertexBufferPool::DynamicVertexBufferPool(VertexDeclaration vertexDeclaration, const uint32_t bufferVertexCapacity,
                                                     const uint32_t minimumPrimitiveVertexCount)
      : DynamicVertexBufferPool()
    {
      Reset(std::move(vertexDeclaration), bufferVertexCapacity, minimumPrimitiveVertexCount);
    }


    DynamicVertexBufferPool::~DynamicVertexBufferPool()
    {
      Reset();
    }


    void DynamicVertexBufferPool::Reset() noexcept
    {
      if (!IsValid())
      {
        return;
      }

      m_bufferVertexCapacity = 0;
      m_minimumPrimitiveVertexCount = 0;
      m_vertexDeclaration = {};
      m_buckets.clear();
      m_activeCount = 0;
      m_isValid = false;
    }


    void DynamicVertexBufferPool::Reset(VertexDeclaration vertexDeclaration, const uint32_t bufferVertexCapacity,
                                        const uint32_t minimumPrimitiveVertexCount)
    {
      if ((bufferVertexCapacity % minimumPrimitiveVertexCount) != 0u)
      {
        throw std::invalid_argument("bufferVertexCapacity must be divisible by minimumPrimitiveVertexCount");
      }
      Reset();

      try
      {
        m_vertexDeclaration = std::move(vertexDeclaration);
        m_bufferVertexCapacity = bufferVertexCapacity;
        m_minimumPrimitiveVertexCount = minimumPrimitiveVertexCount;

        // Allocate the initial bucket
        m_buckets.emplace_back(CreateBuffer(m_vertexDeclaration, bufferVertexCapacity));
        m_isValid = true;
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }


    void DynamicVertexBufferPool::Clear()
    {
      m_activeCount = 0;
    }


    DynamicVertexBufferPool::VertexBufferEntry DynamicVertexBufferPool::NextFree(const uint32_t vertexCapacityHint)
    {
      if (m_activeCount >= std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("Maximum number of supported buffers reached");
      }

      assert((vertexCapacityHint % m_minimumPrimitiveVertexCount) == 0);

      uint32_t currentBucketIndex = m_activeCount / m_bufferVertexCapacity;
      uint32_t indexInsideBucket = m_activeCount % m_bufferVertexCapacity;
      uint32_t currentBucketCapacity = m_bufferVertexCapacity - indexInsideBucket;
      if (currentBucketCapacity < m_minimumPrimitiveVertexCount)
      {
        ++currentBucketIndex;
        indexInsideBucket = 0;
        currentBucketCapacity = m_bufferVertexCapacity;
        m_activeCount = currentBucketIndex * m_bufferVertexCapacity;
      }
      assert((currentBucketCapacity % m_minimumPrimitiveVertexCount) == 0);

      // Check if we need to allocate another bucket
      assert(currentBucketIndex <= m_buckets.size());
      if (currentBucketIndex >= m_buckets.size())
      {
        m_buckets.push_back(CreateBuffer(m_vertexDeclaration, m_bufferVertexCapacity));
      }

      // take the next available command buffer
      const auto vertexBuffer = m_buckets[currentBucketIndex].Get();
      auto capacity = std::min(vertexCapacityHint, currentBucketCapacity);

      m_activeCount += capacity;

      return VertexBufferEntry(vertexBuffer, capacity, indexInsideBucket);
    }
  }
}
