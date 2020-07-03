#ifndef FSLGRAPHICS3D_BASICSCENE_MESH_HPP
#define FSLGRAPHICS3D_BASICSCENE_MESH_HPP
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
#include <FslBase/String/UTF8String.hpp>
#include <FslGraphics/PrimitiveType.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslGraphics3D/BasicScene/RawMeshContent.hpp>
#include <FslGraphics3D/BasicScene/RawMeshContentEx.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    class Mesh
    {
      UTF8String m_name;
      uint32_t m_vertexCount;
      uint32_t m_indexCount;
      PrimitiveType m_primitiveType;
      uint32_t m_primitiveCount;
      uint32_t m_materialIndex;
      bool m_isValid;

    public:
      Mesh();
      Mesh(const int32_t vertexCount, const int32_t indexCount, const PrimitiveType primitiveType);
      Mesh(const std::size_t vertexCount, const std::size_t indexCount, const PrimitiveType primitiveType);
      virtual ~Mesh() = default;

      //! @brief Get the primitive type
      PrimitiveType GetPrimitiveType() const
      {
        return m_primitiveType;
      }

      //! @brief Get the number of vertices.
      uint32_t GetVertexCount() const
      {
        return m_vertexCount;
      }

      //! @brief Get the number of indices.
      uint32_t GetIndexCount() const
      {
        return m_indexCount;
      }

      //! @brief Get the number of primitives
      uint32_t GetPrimitiveCount() const
      {
        return m_primitiveCount;
      }

      //! @brief Get the material index
      uint32_t GetMaterialIndex() const
      {
        return m_materialIndex;
      }

      //! @brief Set the material index
      void SetMaterialIndex(const uint32_t materialIndex);

      //! @brief Get the name of this mesh
      const UTF8String& GetName() const;

      //! @brief Get the name of this mesh
      void SetName(const UTF8String& name);

      virtual void Reset();
      virtual void Reset(const int32_t vertexCount, const int32_t indexCount, const PrimitiveType primitiveType);
      virtual void Reset(const std::size_t vertexCount, const std::size_t indexCount, const PrimitiveType primitiveType);

      //! @brief Get the vertex declaration
      virtual VertexDeclaration GetVertexDeclaration() const = 0;
      //! @brief Get readonly direct access to the vertex data
      virtual RawMeshContent GenericDirectAccess() const = 0;
      //! @brief Get direct access to the vertex data
      virtual RawMeshContentEx GenericDirectAccess() = 0;

      //! @brief Check if the mesh contains valid data
      bool IsValid() const;

    protected:
    };
  }
}

#endif
