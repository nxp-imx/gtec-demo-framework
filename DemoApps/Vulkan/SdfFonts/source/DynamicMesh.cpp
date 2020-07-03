/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include "DynamicMesh.hpp"
#include <FslBase/Exceptions.hpp>
#include <FslBase/ReadOnlySpanUtil.hpp>
#include <FslBase/SpanUtil.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/Vulkan1_0/Util/VMVertexBufferUtil.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace
  {
    uint32_t GarbageCollectFrame(const uint32_t spanUsageFlags, const uint32_t currentFrameIndex, const uint32_t maxFramesInFlight)
    {
      // GC the current frame from all spans
      auto spanUsageFlagsTmp = spanUsageFlags;
      const auto srcBitFlag = 1 << currentFrameIndex;
      for (uint32_t i = 0; i < maxFramesInFlight; ++i)
      {
        spanUsageFlagsTmp &= ~(srcBitFlag << (i * maxFramesInFlight));
      }
      return spanUsageFlagsTmp;
    }


    uint32_t FindFreeSpan(const uint32_t spanUsageFlags, const uint32_t currentFrameIndex, const uint32_t maxFramesInFlight)
    {
      const auto srcSpanMask = (~(1 << currentFrameIndex)) & ((1 << (maxFramesInFlight)) - 1);
      for (uint32_t i = 0; i < maxFramesInFlight; ++i)
      {
        const auto bitFlag = srcSpanMask << (i * maxFramesInFlight);
        if ((spanUsageFlags & bitFlag) == 0u)
        {
          return i;
        }
      }
      throw NotFoundException("No free spans found");
    }

  }

  DynamicMesh::DynamicMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, std::vector<VertexPositionTexture> vertices,
                           const ReadOnlySpan<uint16_t>& indices, const uint32_t maxFramesInFlight)
    : m_vertices(std::move(vertices))
    , m_vb(bufferManager, ReadOnlySpanUtil::AsSpan(m_vertices), m_vertices.size() * maxFramesInFlight, Vulkan::VMBufferUsage::DYNAMIC)
    , m_ib(bufferManager, indices, Vulkan::VMBufferUsage::STATIC)
    , m_activeSpanRange(0, UncheckedNumericCast<uint32_t>(m_vertices.size()))
    , m_maxFramesInFlight(maxFramesInFlight)
  {
    // We basically use a bit per 'frame-span' to indicate which frameIndex the span has been touched by
    if ((maxFramesInFlight * maxFramesInFlight) >= 32)
    {
      throw NotSupportedException("maxFramesInFlight exceeded supported capacity");
    }

    m_vertexInputBindingDescription.binding = 0;
    m_vertexInputBindingDescription.stride = m_vb.GetElementStride();
    m_vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    constexpr std::array<VertexElementUsage, 2> shaderBindOrder = {VertexElementUsage::Position, VertexElementUsage::TextureCoordinate};
    Vulkan::VMVertexBufferUtil::FillVertexInputAttributeDescription(m_vertexAttributeDescription, shaderBindOrder, m_vb);
  }


  Span<VertexPositionTexture> DynamicMesh::BeginWrite(const uint32_t currentFrameIndex)
  {
    m_spanUsageFlags = GarbageCollectFrame(m_spanUsageFlags, currentFrameIndex, m_maxFramesInFlight);
    uint32_t index = FindFreeSpan(m_spanUsageFlags, currentFrameIndex, m_maxFramesInFlight);

    m_activeSpanRange.Start = UncheckedNumericCast<uint32_t>(index * m_vertices.size());
    return SpanUtil::AsSpan(m_vertices);
  }


  void DynamicMesh::EndWrite()
  {
    m_vb.SetData(m_activeSpanRange.Start, ReadOnlySpanUtil::AsSpan(m_vertices));
  }


  void DynamicMesh::Touch(const uint32_t currentFrameIndex) const
  {
    m_spanUsageFlags = GarbageCollectFrame(m_spanUsageFlags, currentFrameIndex, m_maxFramesInFlight);

    auto spanIndex = m_activeSpanRange.Start / m_activeSpanRange.Length;
    const auto srcBitFlag = 1 << currentFrameIndex;
    m_spanUsageFlags |= (srcBitFlag << (spanIndex * m_maxFramesInFlight));
  }


  MeshDescription DynamicMesh::GetMeshDescription() const
  {
    return {ReadOnlySpan<VkVertexInputBindingDescription>(&m_vertexInputBindingDescription, 1),
            ReadOnlySpanUtil::AsSpan(m_vertexAttributeDescription)};
  }


  VkDeviceSize DynamicMesh::GetVertexStartOffset() const
  {
    return m_activeSpanRange.Start * sizeof(VertexPositionTexture);
  }

  SpanRange<uint32_t> DynamicMesh::GetIndexSpanRange() const
  {
    return {0, m_ib.GetIndexCount()};
  }


  void DynamicMesh::Reset() noexcept
  {
    m_vertices.clear();
    m_vb.Reset();
    m_ib.Reset();
    m_vertexInputBindingDescription = {};
    m_vertexAttributeDescription = {};
    m_activeSpanRange = {};
    m_spanUsageFlags = {};
    m_maxFramesInFlight = {};
  }
}
