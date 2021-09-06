#ifndef VULKAN_SDFFONTS_DYNAMICMESH_HPP
#define VULKAN_SDFFONTS_DYNAMICMESH_HPP
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

#include <FslBase/Math/SpanRange.hpp>
#include <FslBase/Span/Span.hpp>
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMBufferManager.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMVertexBuffer.hpp>
#include <FslUtil/Vulkan1_0/Managed/VMIndexBuffer.hpp>
#include <memory>
#include <vector>
#include <utility>

namespace Fsl
{
  struct MeshDescription
  {
    ReadOnlySpan<VkVertexInputBindingDescription> VertexInputBindingDescription;
    ReadOnlySpan<VkVertexInputAttributeDescription> VertexAttributeDescription;

    constexpr inline MeshDescription(ReadOnlySpan<VkVertexInputBindingDescription> vertexInputBindingDescription,
                                     ReadOnlySpan<VkVertexInputAttributeDescription> vertexAttributeDescription)
      : VertexInputBindingDescription(vertexInputBindingDescription)
      , VertexAttributeDescription(vertexAttributeDescription)
    {
    }
  };

  class DynamicMesh
  {
    std::vector<VertexPositionTexture> m_vertices;
    Vulkan::VMVertexBuffer m_vb;
    Vulkan::VMIndexBuffer m_ib;
    VkVertexInputBindingDescription m_vertexInputBindingDescription{};
    std::array<VkVertexInputAttributeDescription, 2> m_vertexAttributeDescription{};

    SpanRange<uint32_t> m_activeSpanRange;
    mutable uint32_t m_spanUsageFlags{};
    uint32_t m_maxFramesInFlight{};

  public:
    // move assignment operator
    DynamicMesh& operator=(DynamicMesh&& other) noexcept
    {
      if (this != &other)
      {
        // Free existing resources then transfer the content of other to this one and fill other with default values
        Reset();

        // Claim ownership here
        m_vertices = std::move(other.m_vertices);
        m_vb = std::move(other.m_vb);
        m_ib = std::move(other.m_ib);
        m_vertexInputBindingDescription = other.m_vertexInputBindingDescription;
        m_vertexAttributeDescription = other.m_vertexAttributeDescription;
        m_activeSpanRange = other.m_activeSpanRange;
        m_spanUsageFlags = other.m_spanUsageFlags;
        m_maxFramesInFlight = other.m_maxFramesInFlight;

        // Remove the data from other
        other.m_vertexInputBindingDescription = {};
        other.m_vertexAttributeDescription = {};
        other.m_activeSpanRange = {};
        other.m_spanUsageFlags = {};
        other.m_maxFramesInFlight = {};
      }
      return *this;
    }

    // move constructor
    DynamicMesh(DynamicMesh&& other) noexcept
      : m_vertices(std::move(other.m_vertices))
      , m_vb(std::move(other.m_vb))
      , m_ib(std::move(other.m_ib))
      , m_vertexInputBindingDescription(other.m_vertexInputBindingDescription)
      , m_vertexAttributeDescription(other.m_vertexAttributeDescription)
      , m_activeSpanRange(other.m_activeSpanRange)
      , m_spanUsageFlags(other.m_spanUsageFlags)
      , m_maxFramesInFlight(other.m_maxFramesInFlight)
    {
      // Remove the data from other
      other.m_vertexInputBindingDescription = {};
      other.m_vertexAttributeDescription = {};
      other.m_activeSpanRange = {};
      other.m_spanUsageFlags = {};
      other.m_maxFramesInFlight = {};
    }

    DynamicMesh() = default;
    DynamicMesh(const std::shared_ptr<Vulkan::VMBufferManager>& bufferManager, std::vector<VertexPositionTexture> vertices,
                const ReadOnlySpan<uint16_t>& indices, const uint32_t maxFramesInFlight);


    Span<VertexPositionTexture> BeginWrite(const uint32_t currentFrameIndex);
    void EndWrite();

    void Touch(const uint32_t currentFrameIndex) const;


    // const Vulkan::VMVertexBuffer& GetBufferObject() const
    //{
    //  return m_vb;
    //}

    uint32_t GetElementStride() const
    {
      return m_vb.GetElementStride();
    }

    const VkBuffer* GetVBBufferPointer() const
    {
      return m_vb.GetBufferPointer();
    }

    VkBuffer GetIBBuffer() const
    {
      return m_ib.GetBuffer();
    }

    VkDeviceSize GetVertexStartOffset() const;
    SpanRange<uint32_t> GetIndexSpanRange() const;

    MeshDescription GetMeshDescription() const;

    void Reset() noexcept;
  };
}

#endif
