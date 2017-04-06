#ifndef FSLGRAPHICS_DRAFT_PROCEDURAL_MESHBUILDER_HPP
#define FSLGRAPHICS_DRAFT_PROCEDURAL_MESHBUILDER_HPP
/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Vector3.hpp>
#include <FslGraphics_Draft/Procedural/MeshBuilder_fwd.hpp>
#include <FslGraphics_Draft/Procedural/IndexUtil.hpp>
#include <FslGraphics_Draft/Procedural/VertexUtil.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <algorithm>
#include <limits>

namespace Fsl {
namespace Procedural
{
  // The class is defined in MeshBuilder_fwd.hpp and this file contains the implementation of the 'templated' functions of MeshBuilder.
  // This allows us to use templates without 'spreading' dependencies to files that don't need it by allowing them to only include the _fwd file


  template<typename TVertex, typename TIndex>
  MeshBuilder<TVertex, TIndex>::MeshBuilder()
    : m_vertices()
    , m_indices()
    , m_primitiveType(PrimitiveType::TriangleList)
    , m_primitiveCount(0)
    , m_vertexCount(0)
    , m_indexCount(0)
  {
  }


  template<typename TVertex, typename TIndex>
  MeshBuilder<TVertex, TIndex>::MeshBuilder(const PrimitiveType primitiveType)
    : m_vertices()
    , m_indices()
    , m_primitiveType(primitiveType)
    , m_primitiveCount(0)
    , m_vertexCount(0)
    , m_indexCount(0)
  {
  }


  template<typename TVertex, typename TIndex>
  MeshBuilder<TVertex, TIndex>::MeshBuilder(const PrimitiveType primitiveType, const std::size_t vertexCapacity, const std::size_t indexCapacity)
    : m_vertices(vertexCapacity)
    , m_indices(indexCapacity)
    , m_primitiveType(primitiveType)
    , m_primitiveCount(0)
    , m_vertexCount(0)
    , m_indexCount(0)
  {
  }


  template<typename TVertex, typename TIndex>
  std::size_t MeshBuilder<TVertex, TIndex>::GetVertexCount() const
  {
    return m_vertexCount;
  }


  template<typename TVertex, typename TIndex>
  std::size_t MeshBuilder<TVertex, TIndex>::GetIndexCount() const
  {
    return m_indexCount;
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::Clear()
  {
    m_indexCount = 0;
    m_vertexCount = 0;
    m_primitiveCount = 0;
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::Append(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh)
  {
    if (mesh.GetPrimitiveType() != m_primitiveType)
      throw std::invalid_argument("The primitive types doesn't match");

    EnsureAppendCapacity(mesh);

    assert(m_vertexCount <= std::numeric_limits<TIndex>::max());
    m_indexCount += IndexUtil::Merge(m_indices, m_indexCount, mesh.GetIndexArray(), static_cast<TIndex>(m_vertexCount), m_primitiveType);
    m_vertexCount += VertexUtil::Copy(m_vertices, m_vertexCount, mesh.GetVertexArray());
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::Append(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const Vector3& positionMod)
  {
    if (mesh.GetPrimitiveType() != m_primitiveType)
      throw std::invalid_argument("The primitive types doesn't match");

    EnsureAppendCapacity(mesh);

    m_indexCount += IndexUtil::Merge(m_indices, m_indexCount, mesh.GetIndexArray(), m_vertexCount, m_primitiveType);
    m_vertexCount += VertexUtil::Copy(m_vertices, m_vertexCount, mesh.GetVertexArray(), positionMod);
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::Append(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const float positionModX, const float positionModY, const float positionModZ)
  {
    Append(mesh, Vector3(positionModX, positionModY, positionModZ));
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::AppendIndices(const std::vector<TIndex>& indices)
  {
    AppendIndices(indices, 0);
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::AppendIndices(const std::vector<TIndex>& indices, const TIndex indexMod)
  {
    const auto minIndexCapacity = IndexUtil::CalcAppendMinimumCapacity(m_indexCount, indices.size(), m_primitiveType);
    EnsureIndexCapacity(minIndexCapacity);
    m_indexCount += IndexUtil::Merge(m_indices, m_indexCount, indices, indexMod, m_primitiveType);
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::AppendInstances(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const std::size_t instanceCount, const bool shareInstanceVertices)
  {
    const auto maxInstances = CalcAppendMaxInstancesThatFit(m_vertexCount, m_indexCount, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), mesh, shareInstanceVertices);

    if (instanceCount > maxInstances)
      throw std::invalid_argument("Can't fit the requested number of instances");

    if (TryAppendInstances(mesh, instanceCount, shareInstanceVertices) != instanceCount)
      throw std::runtime_error("Failed to add the requested amount of instances");
  }


  template<typename TVertex, typename TIndex>
  std::size_t MeshBuilder<TVertex, TIndex>::TryAppendInstances(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const std::size_t instanceCount, const bool shareInstanceVertices)
  {
    const auto maxInstances = CalcAppendMaxInstancesThatFit(m_vertexCount, m_indexCount, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), mesh, shareInstanceVertices);
    const std::size_t numInstances = std::min(instanceCount, maxInstances);

    // Ensure capacity for all instances
    EnsureVertexCapacity(m_vertexCount + numInstances * mesh.GetVertexCount());
    const auto indexCapacity = IndexUtil::CalcAppendInstancesRequiredCapacity(m_indexCount, mesh.GetIndexCount(), numInstances, m_primitiveType);
    EnsureIndexCapacity(indexCapacity);

    if (!shareInstanceVertices)
    {
      for (std::size_t i = 0; i < numInstances; ++i)
        Append(mesh);
    }
    else
    {
      // Add the mesh
      Append(mesh);
      // Add additional instances via indices only
      const std::size_t instancesLeft = numInstances - 1;
      for (std::size_t i = 0; i < instancesLeft; ++i)
      {
        assert(m_vertexCount <= std::numeric_limits<TIndex>::max());
        AppendIndices(mesh.GetIndexArray(), static_cast<TIndex>(m_vertexCount));
      }
    }
    return numInstances;
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::EnsureCapacity(const std::size_t vertexCount, const std::size_t indexCount)
  {
    EnsureVertexCapacity(vertexCount);
    EnsureIndexCapacity(indexCount);
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::EnsureVertexCapacity(const std::size_t count)
  {
    if (count >= 0 && std::size_t(count) >= m_vertices.size())
    {
      m_vertices.resize(count);
    }
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::EnsureIndexCapacity(const std::size_t count)
  {
    if (count > std::numeric_limits<TIndex>::max())
      throw std::invalid_argument("indexCount is out of bounds");

    if (count >= 0 && std::size_t(count) >= m_indices.size())
    {
      m_indices.resize(count);
    }
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::Reset(const PrimitiveType primitiveType)
  {
    Clear();
    m_primitiveType = primitiveType;
  }

  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::ExtractTo(Graphics3D::GenericMesh<TVertex, TIndex>& rMesh)
  {
    rMesh.Reset(m_vertices, 0, m_vertexCount, m_indices, 0, m_indexCount, m_primitiveType);
  }


  template<typename TVertex, typename TIndex>
  std::size_t MeshBuilder<TVertex, TIndex>::CalcAppendMaxInstancesThatFit(const std::size_t currentVertexCount, const std::size_t currentIndexCount,
                                                                          const std::size_t vertexCapacity, const std::size_t indexCapacity,
                                                                          const Graphics3D::GenericMesh<TVertex, TIndex>& mesh,
                                                                          const bool shareVertices)
  {
    if (currentVertexCount >= vertexCapacity)
      throw std::invalid_argument("invalid arguments");

    const std::size_t maxVertexCapacity = vertexCapacity - currentVertexCount;
    const std::size_t maxInstancesBasedOnVertices = VertexUtil::CalcMaxInstancesThatFit(mesh.GetVertexCount(), maxVertexCapacity, shareVertices);
    const std::size_t maxInstancesBasedOnIndices = IndexUtil::CalcAppendMaxInstancesThatFit(currentIndexCount, mesh.GetIndexCount(), indexCapacity, mesh.GetPrimitiveType());
    return (maxInstancesBasedOnVertices <= maxInstancesBasedOnIndices ? maxInstancesBasedOnVertices : maxInstancesBasedOnIndices);
  }


  template<typename TVertex, typename TIndex>
  void MeshBuilder<TVertex, TIndex>::EnsureAppendCapacity(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh)
  {
    const std::size_t minIndexCapacity = IndexUtil::CalcAppendMinimumCapacity(m_indexCount, mesh.GetIndexCount(), m_primitiveType);
    const std::size_t minVertexCapacity = m_vertexCount + mesh.GetVertexCount();

    assert(minIndexCapacity >= m_indexCount);
    assert(minVertexCapacity >= m_vertexCount);

    if (std::size_t(minIndexCapacity) > m_indices.size())
      m_indices.resize(minIndexCapacity);

    if (std::size_t(minVertexCapacity) > m_vertices.size())
      m_vertices.resize(minVertexCapacity);
  }
}}

#endif
