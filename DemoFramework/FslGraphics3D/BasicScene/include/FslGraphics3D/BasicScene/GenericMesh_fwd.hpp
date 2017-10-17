#ifndef FSLGRAPHICS3D_BASICSCENE_GENERICMESH_FWD_HPP
#define FSLGRAPHICS3D_BASICSCENE_GENERICMESH_FWD_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics3D/BasicScene/Mesh.hpp>
#include <vector>

namespace Fsl
{
  namespace Graphics3D
  {
    template<typename TVertex, typename TIndex>
    class GenericMesh : public Mesh
    {
      std::vector<TVertex> m_vertices;
      std::vector<TIndex> m_indices;
    public:
      typedef TVertex vertex_type;
      typedef TIndex index_type;

      //! @brief create a empty mesh
      GenericMesh();

      //! @brief Create a mesh capable of holding a mesh of the specified size.
      GenericMesh(const std::size_t vertexCapacity, const std::size_t indexCapacity, const PrimitiveType primitiveType);

      //! @brief Create a mesh from the supplied vertices and indices.
      GenericMesh(const TVertex*const pVertices, const std::size_t vertexCount, const TIndex*const pIndices, const std::size_t indexCount, const PrimitiveType primitiveType);

      //! @brief Create a mesh from the supplied vertices and indices.
      GenericMesh(const std::vector<TVertex>& vertices, const std::vector<TIndex>& indices, const PrimitiveType primitiveType);

      //! @brief Create a mesh from the supplied vertices and indices.
      GenericMesh(const std::vector<TVertex>& vertices, const std::size_t vertexStartOffset, const std::size_t vertexCount,
                  const std::vector<TIndex>& indices, const std::size_t indexStartOffset, const std::size_t indexCount,
                  const PrimitiveType primitiveType);


      //! @brief Reset this mesh to be empty
      virtual void Reset();

      //! @brief Recreate a mesh capable of holding a mesh of the specified size.
      virtual void Reset(const int32_t vertexCapacity, const int32_t indexCapacity, const PrimitiveType primitiveType);

      //! @brief Recreate a mesh capable of holding a mesh of the specified size.
      void Reset(const std::size_t vertexCapacity, const std::size_t indexCapacity, const PrimitiveType primitiveType);

      //! @brief Recreate this mesh based on the supplied supplied vertices and indices.
      void Reset(const std::vector<TVertex>& vertices, const std::vector<TIndex>& indices, const PrimitiveType primitiveType);

      //! @brief Recreate this mesh based on the supplied vertices and indices.
      void Reset(const std::vector<TVertex>& vertices, const std::size_t vertexStartOffset, const std::size_t vertexCount,
                 const std::vector<TIndex>& indices, const std::size_t indexStartOffset, const std::size_t indexCount,
                 const PrimitiveType primitiveType);


      //! @brief Get a pointer to the vertices
      //! @note The pointer is only valid until this object is modified.
      const TVertex* GetVertices() const;

      //! @brief Get a pointer to the indices
      //! @note The pointer is only valid until this object is modified.
      const TIndex* GetIndices() const;

      //! @brief Get a reference to the vertex array
      //! @note The reference is only valid until this object is modified.
      const std::vector<TVertex>& GetVertexArray() const;

      //! @brief Get a reference to the index array
      //! @note The reference is only valid until this object is modified.
      const std::vector<TIndex>& GetIndexArray() const;

      //! @brief Get a pointer to the vertices
      //! @note The pointer is only valid until this object is modified.
      TVertex* DirectAccessVertices();

      //! @brief Get a pointer to the indices
      //! @note The pointer is only valid until this object is modified.
      TIndex* DirectAccessIndices();

      virtual VertexDeclaration GetVertexDeclaration() const;
      virtual const RawMeshContent GenericDirectAccess() const;
      virtual RawMeshContentEx GenericDirectAccess();
    };
  }
}

#endif
