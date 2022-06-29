#ifndef SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_MESHLOADER_MESHBUFFER_HPP
#define SHARED_VULKANWILLEMSDEMOAPPEXPERIMENTAL_MESHLOADER_MESHBUFFER_HPP
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

#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshBufferInfo.hpp>
#include <Shared/VulkanWillemsDemoAppExperimental/MeshLoader/MeshDescriptor.hpp>
#include <glm/vec3.hpp>
#include <vulkan/vulkan.h>
#include <vector>

namespace Fsl::Willems::MeshLoader
{
  //! Mesh representation storing all data required to generate buffers
  //! This object is movable so it can be thought of as behaving in the same was as a unique_ptr and is compatible with std containers
  class MeshBuffer
  {
    std::vector<MeshDescriptor> m_meshDescriptors;
    MeshBufferInfo m_vertices;
    MeshBufferInfo m_indices;
    uint32_t m_indexCount;
    glm::vec3 m_dim{};

  public:
    MeshBuffer(const MeshBuffer&) = delete;
    MeshBuffer& operator=(const MeshBuffer&) = delete;

    //! @brief Move assignment operator
    MeshBuffer& operator=(MeshBuffer&& other) noexcept;

    //! @brief Move constructor
    //! Transfer ownership from other to this
    MeshBuffer(MeshBuffer&& other) noexcept;

    //! @brief Move objects into this object
    MeshBuffer(const MeshDescriptor& meshDescriptor, MeshBufferInfo&& vertices, MeshBufferInfo&& indices, const uint32_t indexCount,
               const glm::vec3& dim);

    //! @brief Move objects into this object
    MeshBuffer(std::vector<MeshDescriptor>&& meshDescriptors, MeshBufferInfo&& vertices, MeshBufferInfo&& indices, const uint32_t indexCount,
               const glm::vec3& dim);

    //! @brief Create a 'invalid' instance (use Reset to populate it)
    MeshBuffer();


    ~MeshBuffer()
    {
      Reset();
    }

    //! @brief Destroys any owned resources and resets the object to its default state.
    void Reset() noexcept;

    void Reset(const MeshDescriptor& meshDescriptor, MeshBufferInfo&& vertices, MeshBufferInfo&& indices, const uint32_t indexCount,
               const glm::vec3& dim);

    void Reset(std::vector<MeshDescriptor>&& meshDescriptors, MeshBufferInfo&& vertices, MeshBufferInfo&& indices, const uint32_t indexCount,
               const glm::vec3& dim);

    //! @brief Get the associated 'Device'
    VkDevice GetDevice() const
    {
      return m_vertices.Buffer.GetDevice();
    }

    const std::vector<MeshDescriptor>& GetMeshDescriptors() const
    {
      return m_meshDescriptors;
    }

    const MeshBufferInfo& GetVertices() const
    {
      return m_vertices;
    }

    const MeshBufferInfo& GetIndices() const
    {
      return m_indices;
    }

    uint32_t GetIndexCount() const
    {
      return m_indexCount;
    }

    glm::vec3 GetDim() const
    {
      return m_dim;
    }

    //! @brief Check if this object contains a valid resource
    inline bool IsValid() const
    {
      return m_vertices.IsValid();
    }
  };
}

#endif
