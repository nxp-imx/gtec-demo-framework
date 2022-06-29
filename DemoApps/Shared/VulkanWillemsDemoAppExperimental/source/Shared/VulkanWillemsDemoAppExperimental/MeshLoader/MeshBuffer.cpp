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

#include <FslBase/Log/Log3Fmt.hpp>
#include <RapidVulkan/Check.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshBuffer.hpp>
#include <vulkan/vulkan.h>
#include <cassert>
#include <utility>

namespace Fsl::Willems::MeshLoader
{
  MeshBuffer& MeshBuffer::operator=(MeshBuffer&& other) noexcept
  {
    if (this != &other)
    {
      // Free existing resources then transfer the content of other to this one and fill other with default values
      if (IsValid())
      {
        Reset();
      }

      // Claim ownership here
      m_meshDescriptors = std::move(other.m_meshDescriptors);
      m_vertices = std::move(other.m_vertices);
      m_indices = std::move(other.m_indices);
      m_indexCount = other.m_indexCount;
      m_dim = other.m_dim;

      // Remove the data from other
      other.m_indexCount = 0;
      other.m_dim = glm::vec3();
    }
    return *this;
  }


  MeshBuffer::MeshBuffer(MeshBuffer&& other) noexcept
    : m_meshDescriptors(std::move(other.m_meshDescriptors))
    , m_vertices(std::move(other.m_vertices))
    , m_indices(std::move(other.m_indices))
    , m_indexCount(other.m_indexCount)
    , m_dim(other.m_dim)
  {
    // Remove the data from other
    other.m_indexCount = 0;
    other.m_dim = glm::vec3();
  }


  MeshBuffer::MeshBuffer(const MeshDescriptor& meshDescriptor, MeshBufferInfo&& vertices, MeshBufferInfo&& indices, const uint32_t indexCount,
                         const glm::vec3& dim)
    : MeshBuffer()
  {
    Reset(meshDescriptor, std::move(vertices), std::move(indices), indexCount, dim);
  }


  MeshBuffer::MeshBuffer(std::vector<MeshDescriptor>&& meshDescriptors, MeshBufferInfo&& vertices, MeshBufferInfo&& indices,
                         const uint32_t indexCount, const glm::vec3& dim)
    : MeshBuffer()
  {
    Reset(std::move(meshDescriptors), std::move(vertices), std::move(indices), indexCount, dim);
  }


  MeshBuffer::MeshBuffer()
    : m_indexCount(0)
  {
  }


  void MeshBuffer::Reset() noexcept
  {
    if (!IsValid())
    {
      return;
    }

    assert(m_vertices.IsValid());
    assert(m_indices.IsValid());

    m_meshDescriptors.clear();
    m_vertices.Reset();
    m_indices.Reset();
    m_indexCount = 0;
    m_dim = glm::vec3();
  }


  void MeshBuffer::Reset(const MeshDescriptor& meshDescriptor, MeshBufferInfo&& vertices, MeshBufferInfo&& indices, const uint32_t indexCount,
                         const glm::vec3& dim)
  {
    std::vector<MeshDescriptor> meshDescriptors(1);
    meshDescriptors[0] = meshDescriptor;
    Reset(std::move(meshDescriptors), std::move(vertices), std::move(indices), indexCount, dim);
  }

  void MeshBuffer::Reset(std::vector<MeshDescriptor>&& meshDescriptors, MeshBufferInfo&& vertices, MeshBufferInfo&& indices,
                         const uint32_t indexCount, const glm::vec3& dim)
  {
    if (IsValid())
    {
      Reset();
    }

    const bool hasOneValid = vertices.IsValid() || indices.IsValid();
    if (vertices.IsValid() != hasOneValid || indices.IsValid() != hasOneValid)
    {
      throw std::invalid_argument("Either all objects has to be valid or none must be");
    }

    if (vertices.GetDevice() != indices.GetDevice())
    {
      throw std::invalid_argument("All objects must belong to the same device");
    }

    m_meshDescriptors = std::move(meshDescriptors);
    m_vertices = std::move(vertices);
    m_indices = std::move(indices);
    m_indexCount = indexCount;
    m_dim = dim;
  }
}
