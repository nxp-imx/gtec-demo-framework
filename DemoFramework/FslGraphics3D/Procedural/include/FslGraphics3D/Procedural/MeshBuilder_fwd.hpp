#ifndef FSLGRAPHICS3D_PROCEDURAL_MESHBUILDER_FWD_HPP
#define FSLGRAPHICS3D_PROCEDURAL_MESHBUILDER_FWD_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/PrimitiveType.hpp>
#include <FslGraphics3D/BasicScene/GenericMesh_fwd.hpp>
#include <vector>

namespace Fsl
{
  struct Vector3;

  namespace Procedural
  {
    //! @brief A simple mesh builder useful for combining meshes of the same primitive type.
    template <typename TVertex, typename TIndex>
    class MeshBuilder
    {
      std::vector<TVertex> m_vertices;
      std::vector<TIndex> m_indices;
      PrimitiveType m_primitiveType;
      std::size_t m_primitiveCount;
      std::size_t m_vertexCount;
      std::size_t m_indexCount;

    public:
      MeshBuilder();
      explicit MeshBuilder(const PrimitiveType primitiveType);
      MeshBuilder(const PrimitiveType primitiveType, const std::size_t vertexCapacity, const std::size_t indexCapacity);

      std::size_t GetVertexCount() const;
      std::size_t GetIndexCount() const;

      void Clear();
      void Append(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh);
      void Append(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const Vector3& positionMod);
      void Append(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const float positionModX, const float positionModY, const float positionModZ);

      //! @brief Append the indices
      //! @param indices the indices that should be added.
      void AppendIndices(const std::vector<TIndex>& indices);

      //! @brief Append the indices
      //! @param indices the indices that should be added.
      //! @param indexMod the value that should be added to each entry in indices
      void AppendIndices(const std::vector<TIndex>& indices, const TIndex indexMod);

      //! @brief Append the given mesh instanceCount times.
      //! @param mesh the mesh to append
      //! @param shareInstanceVertices if this is true only the indices will be duplicated
      void AppendInstances(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const std::size_t instanceCount, const bool shareInstanceVertices);

      //! @brief Try to Append the given mesh instanceCount times.
      //! @param mesh the mesh to append
      //! @param shareInstanceVertices if this is true only the indices will be duplicated
      //! @return the number of times the mesh was appended
      std::size_t TryAppendInstances(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const std::size_t instanceCount,
                                     const bool shareInstanceVertices);

      //! @brief Reset the primitive type of this builder (this also clears the builder)
      void Reset(const PrimitiveType primitiveType);

      //! @brief Copy the mesh currently stored in the builder into the supplied mesh.
      void ExtractTo(Graphics3D::GenericMesh<TVertex, TIndex>& rMesh);

      //! @brief Ensure that the builder has capacity for the given vertex and index count
      void EnsureCapacity(const std::size_t vertexCount, const std::size_t indexCount);

      //! @brief Ensure that the builder has capacity for the given vertex count
      void EnsureVertexCapacity(const std::size_t count);

      //! @brief Ensure that the builder has capacity for the given index count
      void EnsureIndexCapacity(const std::size_t count);

      //! @brief Calculate how many of instances of the mesh that can be appended inside the vertex and index limit.
      //! @param currentVertexCount the number of vertices stored in the buffer we are appending to
      //! @param currentIndexCount the number of indices stored in the buffer we are appending to
      //! @param vertexCapacity the maximum number of vertices that is allowed to be used for instancing.
      //! @param indexCapacity the maximum number of indices that is allowed to be used for instancing.
      //! @param mesh the mesh that should be instanced
      //! @param shareVertices if true all instances will share the same vertices.
      //! @return the number of instances of mesh that fit within the supplied constraints
      static std::size_t CalcAppendMaxInstancesThatFit(const std::size_t currentVertexCount, const std::size_t currentIndexCount,
                                                       const std::size_t vertexCapacity, const std::size_t indexCapacity,
                                                       const Graphics3D::GenericMesh<TVertex, TIndex>& mesh, const bool shareVertices);

    private:
      void EnsureAppendCapacity(const Graphics3D::GenericMesh<TVertex, TIndex>& mesh);
    };
  }
}

#endif
