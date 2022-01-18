#ifndef FSLGRAPHICS3D_BASICSCENE_GENERICMESH_HPP
#define FSLGRAPHICS3D_BASICSCENE_GENERICMESH_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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

#include <FslGraphics3D/BasicScene/GenericMesh_fwd.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil_Vector.hpp>
#include <FslGraphics/PrimitiveTypeUtil.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    // The class is defined in GenericMesh_fwd.hpp and this file contains the implementation of the 'templated' functions of GenericMesh.
    // This allows us to use templates without 'spreading' dependencies to files that don't need it by allowing them to only include the _fwd file

    template <typename TVertex, typename TIndex>
    GenericMesh<TVertex, TIndex>::GenericMesh()
      : Mesh()
      , m_vertices()
      , m_indices()
    {
    }


    template <typename TVertex, typename TIndex>
    GenericMesh<TVertex, TIndex>::GenericMesh(const std::size_t vertexCapacity, const std::size_t indexCapacity, const PrimitiveType primitiveType)
      : Mesh(vertexCapacity, indexCapacity, primitiveType)
      , m_vertices(vertexCapacity)
      , m_indices(indexCapacity)
    {
    }


    template <typename TVertex, typename TIndex>
    GenericMesh<TVertex, TIndex>::GenericMesh(const TVertex* const pVertices, const std::size_t vertexCount, const TIndex* const pIndices,
                                              const std::size_t indexCount, const PrimitiveType primitiveType)
      : Mesh(vertexCount, indexCount, primitiveType)
      , m_vertices(pVertices, pVertices + vertexCount)
      , m_indices(pIndices, pIndices + indexCount)
    {
    }


    template <typename TVertex, typename TIndex>
    GenericMesh<TVertex, TIndex>::GenericMesh(const std::vector<TVertex>& vertices, const std::vector<TIndex>& indices,
                                              const PrimitiveType primitiveType)
      : Mesh(vertices.size(), indices.size(), primitiveType)
      , m_vertices(vertices)
      , m_indices(indices)
    {
    }


    template <typename TVertex, typename TIndex>
    GenericMesh<TVertex, TIndex>::GenericMesh(const std::vector<TVertex>& vertices, const std::size_t vertexStartOffset,
                                              const std::size_t vertexCount, const std::vector<TIndex>& indices, const std::size_t indexStartOffset,
                                              const std::size_t indexCount, const PrimitiveType primitiveType)
      : Mesh(vertexCount, indexCount, primitiveType)
      , m_vertices(vertexCount)
      , m_indices(indexCount)
    {
      if ((vertexStartOffset + vertexCount) > vertices.size() || (indexStartOffset + indexCount) > indices.size())
      {
        throw std::invalid_argument("One of the arguments is invalid");
      }

      std::copy(vertices.begin() + vertexStartOffset, vertices.begin() + (vertexStartOffset + vertexCount), m_vertices.begin());
      std::copy(indices.begin() + indexStartOffset, indices.begin() + (indexStartOffset + indexCount), m_indices.begin());
    }


    template <typename TVertex, typename TIndex>
    void GenericMesh<TVertex, TIndex>::Reset()
    {
      Mesh::Reset();
      m_vertices.clear();
      m_indices.clear();
    }


    template <typename TVertex, typename TIndex>
    void GenericMesh<TVertex, TIndex>::Reset(const int32_t vertexCapacity, const int32_t indexCapacity, const PrimitiveType primitiveType)
    {
      Mesh::Reset(vertexCapacity, indexCapacity, primitiveType);
      m_vertices.resize(vertexCapacity);
      m_indices.resize(indexCapacity);
    }


    template <typename TVertex, typename TIndex>
    void GenericMesh<TVertex, TIndex>::Reset(const std::size_t vertexCapacity, const std::size_t indexCapacity, const PrimitiveType primitiveType)
    {
      Mesh::Reset(vertexCapacity, indexCapacity, primitiveType);
      m_vertices.resize(vertexCapacity);
      m_indices.resize(indexCapacity);
    }


    template <typename TVertex, typename TIndex>
    void GenericMesh<TVertex, TIndex>::Reset(const std::vector<TVertex>& vertices, const std::vector<TIndex>& indices,
                                             const PrimitiveType primitiveType)
    {
      Mesh::Reset(vertices.size(), indices.size(), primitiveType);

      m_vertices.resize(vertices.size());
      m_indices.resize(indices.size());

      // Copy the input
      std::copy(vertices.begin(), vertices.end(), m_vertices.begin());
      std::copy(indices.begin(), indices.end(), indices.begin());
    }


    template <typename TVertex, typename TIndex>
    void GenericMesh<TVertex, TIndex>::Reset(const std::vector<TVertex>& vertices, const std::size_t vertexStartOffset, const std::size_t vertexCount,
                                             const std::vector<TIndex>& indices, const std::size_t indexStartOffset, const std::size_t indexCount,
                                             const PrimitiveType primitiveType)
    {
      if ((vertexStartOffset + vertexCount) > vertices.size() || (indexStartOffset + indexCount) > indices.size())
      {
        throw std::invalid_argument("One of the arguments is invalid");
      }

      Mesh::Reset(vertices.size(), indices.size(), primitiveType);

      m_vertices.resize(vertexCount);
      m_indices.resize(indexCount);

      std::copy(vertices.begin() + vertexStartOffset, vertices.begin() + (vertexStartOffset + vertexCount), m_vertices.begin());
      std::copy(indices.begin() + indexStartOffset, indices.begin() + (indexStartOffset + indexCount), m_indices.begin());
    }


    template <typename TVertex, typename TIndex>
    const TVertex* GenericMesh<TVertex, TIndex>::GetVertices() const
    {
      return m_vertices.data();
    }


    template <typename TVertex, typename TIndex>
    const TIndex* GenericMesh<TVertex, TIndex>::GetIndices() const
    {
      return m_indices.data();
    }


    template <typename TVertex, typename TIndex>
    const std::vector<TVertex>& GenericMesh<TVertex, TIndex>::GetVertexArray() const
    {
      return m_vertices;
    }


    template <typename TVertex, typename TIndex>
    const std::vector<TIndex>& GenericMesh<TVertex, TIndex>::GetIndexArray() const
    {
      return m_indices;
    }


    template <typename TVertex, typename TIndex>
    ReadOnlyFlexVertexSpan GenericMesh<TVertex, TIndex>::AsReadOnlyFlexVertexSpan() const
    {
      return ReadOnlyFlexVertexSpanUtil::AsSpan(m_vertices);
    }


    template <typename TVertex, typename TIndex>
    ReadOnlySpan<TIndex> GenericMesh<TVertex, TIndex>::AsReadOnlyIndexSpan() const
    {
      return ReadOnlySpanUtil::AsSpan(m_indices);
    }


    template <typename TVertex, typename TIndex>
    TVertex* GenericMesh<TVertex, TIndex>::DirectAccessVertices()
    {
      return m_vertices.data();
    }


    template <typename TVertex, typename TIndex>
    TIndex* GenericMesh<TVertex, TIndex>::DirectAccessIndices()
    {
      return m_indices.data();
    }


    template <typename TVertex, typename TIndex>
    RawMeshContent GenericMesh<TVertex, TIndex>::GenericDirectAccess() const
    {
      return RawMeshContent(m_vertices.data(), static_cast<uint32_t>(m_vertices.size()), sizeof(TVertex), m_indices.data(),
                            static_cast<uint32_t>(m_indices.size()), sizeof(TIndex), GetPrimitiveType());
    }


    template <typename TVertex, typename TIndex>
    RawMeshContentEx GenericMesh<TVertex, TIndex>::GenericDirectAccess()
    {
      return RawMeshContentEx(m_vertices.data(), static_cast<uint32_t>(m_vertices.size()), sizeof(TVertex), m_indices.data(),
                              static_cast<uint32_t>(m_indices.size()), sizeof(TIndex), GetPrimitiveType());
    }

    template <typename TVertex, typename TIndex>
    VertexDeclarationSpan GenericMesh<TVertex, TIndex>::AsVertexDeclarationSpan() const
    {
      return TVertex::AsVertexDeclarationSpan();
    }
  }
}

#endif
