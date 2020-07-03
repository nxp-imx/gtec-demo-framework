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

#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Util/VulkanConvert.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace Vulkan
  {
    namespace
    {
      void Convert(std::vector<VMVertexElement>& rVertexElements, const VertexDeclaration& vertexDeclaration)
      {
        rVertexElements.resize(vertexDeclaration.Count());
        for (std::size_t i = 0; i < vertexDeclaration.Count(); ++i)
        {
          auto element = vertexDeclaration.At(i);
          rVertexElements[i] = VMVertexElement(element, VulkanConvert::ToVkFormat(element.Format));
        }
      }
    }

    void VMVertexBuffer::Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const void* const pVertices, const std::size_t elementCount,
                               const std::size_t elementCapacity, const VertexDeclaration& vertexDeclaration, const VMBufferUsage usage)
    {
      if (!bufferManager)
      {
        throw std::invalid_argument("bufferManager can not be null");
      }
      if (pVertices == nullptr)
      {
        throw std::invalid_argument("pVertices can not be null");
      }
      if (elementCount > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("element counts larger than a uint32_t is not supported");
      }
      if (elementCount > elementCapacity)
      {
        throw NotSupportedException("element count should be <= elementCapacity");
      }

      Reset();


      try
      {
        const auto stride = vertexDeclaration.VertexStride();
        m_vertexBuffer = bufferManager->CreateBuffer(pVertices, elementCount, elementCapacity, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, usage);
        Convert(m_vertexElements, vertexDeclaration);
        m_bufferManager = bufferManager;
        m_vertexCount = UncheckedNumericCast<uint32_t>(elementCapacity);
        m_elementStride = stride;
        m_usage = usage;
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }

    void VMVertexBuffer::Reset(const std::shared_ptr<VMBufferManager>& bufferManager, const std::size_t elementCapacity,
                               const VertexDeclaration& vertexDeclaration)
    {
      if (!bufferManager)
      {
        throw std::invalid_argument("bufferManager can not be null");
      }
      if (elementCapacity > std::numeric_limits<uint32_t>::max())
      {
        throw NotSupportedException("element counts larger than a uint32_t is not supported");
      }

      Reset();


      try
      {
        const auto stride = vertexDeclaration.VertexStride();
        m_vertexBuffer = bufferManager->CreateDynamicBuffer(elementCapacity, stride, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        Convert(m_vertexElements, vertexDeclaration);
        m_bufferManager = bufferManager;
        m_vertexCount = static_cast<uint32_t>(elementCapacity);
        m_elementStride = stride;
        m_usage = VMBufferUsage::DYNAMIC;
      }
      catch (const std::exception&)
      {
        Reset();
        throw;
      }
    }

    void VMVertexBuffer::SetData(const uint32_t dstElementOffset, const void* pVertices, const std::size_t elementCount, const uint32_t elementStride)
    {
      if (dstElementOffset > m_vertexCount)
      {
        throw std::invalid_argument("dstElementOffset out of bounds");
      }
      if (elementCount > (m_vertexCount - dstElementOffset))
      {
        throw std::invalid_argument(
          fmt::format("out of bounds (dstElementOffset {} elementCount {} vertexCount {})", dstElementOffset, elementCount, m_vertexCount));
      }
      if (elementStride != m_elementStride)
      {
        throw std::invalid_argument("elementStride must match the VMVertexBuffer element stride");
      }

      auto dstOffset = dstElementOffset * elementStride;

      m_vertexBuffer.Upload(dstOffset, pVertices, elementCount * elementStride);
    }

    int32_t VMVertexBuffer::GetVertexElementIndex(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      auto index = VertexElementIndexOf(usage, usageIndex);
      if (index < 0)
      {
        throw NotFoundException("The supplied usage and usageIndex combo was not found");
      }
      return index;
    }


    int32_t VMVertexBuffer::VertexElementIndexOf(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      auto itrFind = std::find_if(m_vertexElements.begin(), m_vertexElements.end(), [usage, usageIndex](const VMVertexElement& entry) {
        return (entry.Usage == usage && entry.UsageIndex == usageIndex);
      });
      if (itrFind != m_vertexElements.end())
      {
        return static_cast<int32_t>(std::distance(m_vertexElements.begin(), itrFind));
      }
      return -1;
    }

    const VMVertexElement& VMVertexBuffer::GetVertexElementAt(const uint32_t vertexElementIndex) const
    {
      if (vertexElementIndex >= m_vertexElements.size())
      {
        throw IndexOutOfRangeException("VMVertexBuffer index out of bounds");
      }
      return m_vertexElements[vertexElementIndex];
    }


    const VMVertexElement& VMVertexBuffer::GetVertexElement(const VertexElementUsage usage, const uint32_t usageIndex) const
    {
      auto elementIndex = GetVertexElementIndex(usage, usageIndex);
      return m_vertexElements[elementIndex];
    }
  }
}
