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

#include <FslGraphics3D/BasicScene/Mesh.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslGraphics/PrimitiveTypeUtil.hpp>
#include <limits>

namespace Fsl
{
  namespace Graphics3D
  {
    Mesh::Mesh()
      : m_name()
      , m_vertexCount(0)
      , m_indexCount(0)
      , m_primitiveType(PrimitiveType::TriangleList)
      , m_primitiveCount(0)
      , m_materialIndex(0)
      , m_isValid(false)
    {
    }


    Mesh::Mesh(const int32_t vertexCount, const int32_t indexCount, const PrimitiveType primitiveType)
      : m_name()
      , m_vertexCount(static_cast<uint32_t>(vertexCount))
      , m_indexCount(static_cast<uint32_t>(indexCount))
      , m_primitiveType(primitiveType)
      , m_primitiveCount(0)
      , m_materialIndex(0)
      , m_isValid(true)
    {
      if (vertexCount < 0 || indexCount < 0)
        throw std::invalid_argument("vertexCount and indexCount can not be less than zero");

      m_primitiveCount = PrimitiveTypeUtil::CalcPrimitiveCount(m_indexCount, primitiveType);
    }


    Mesh::Mesh(const std::size_t vertexCount, const std::size_t indexCount, const PrimitiveType primitiveType)
      : m_name()
      , m_vertexCount(static_cast<uint32_t>(vertexCount))
      , m_indexCount(static_cast<uint32_t>(indexCount))
      , m_primitiveType(primitiveType)
      , m_primitiveCount(0)
      , m_materialIndex(0)
      , m_isValid(true)
    {
      if (vertexCount > std::numeric_limits<uint32_t>::max() || indexCount > std::numeric_limits<uint32_t>::max())
        throw NotSupportedException("We only support 'count 32bit' elements");
      m_primitiveCount = PrimitiveTypeUtil::CalcPrimitiveCount(m_indexCount, primitiveType);
    }


    void Mesh::Reset()
    {
      m_vertexCount = 0;
      m_indexCount = 0;
      m_primitiveType = PrimitiveType::TriangleList;
      m_primitiveCount = 0;
      m_name.Clear();
      m_materialIndex = 0;
      m_isValid = false;
    }


    void Mesh::Reset(const int32_t vertexCount, const int32_t indexCount, const PrimitiveType primitiveType)
    {
      if (vertexCount < 0 || indexCount < 0)
        throw std::invalid_argument("vertexCount and indexCount can not be less than zero");

      m_vertexCount = vertexCount;
      m_indexCount = indexCount;
      m_primitiveType = primitiveType;
      m_primitiveCount = PrimitiveTypeUtil::CalcPrimitiveCount(m_indexCount, primitiveType);
      m_name.Clear();
      m_materialIndex = 0;
      m_isValid = true;
    }


    void Mesh::Reset(const std::size_t vertexCount, const std::size_t indexCount, const PrimitiveType primitiveType)
    {
      if (vertexCount > std::numeric_limits<uint32_t>::max() || indexCount > std::numeric_limits<uint32_t>::max())
        throw NotSupportedException("We only support 'count 32bit' elements");

      m_vertexCount = static_cast<uint32_t>(vertexCount);
      m_indexCount = static_cast<uint32_t>(indexCount);
      m_primitiveType = primitiveType;
      m_primitiveCount = PrimitiveTypeUtil::CalcPrimitiveCount(m_indexCount, primitiveType);
      m_name.Clear();
      m_materialIndex = 0;
      m_isValid = true;
    }


    void Mesh::SetMaterialIndex(const uint32_t materialIndex)
    {
      m_materialIndex = materialIndex;
    }


    const UTF8String& Mesh::GetName() const
    {
      return m_name;
    }


    void Mesh::SetName(const UTF8String& name)
    {
      m_name = name;
    }


    bool Mesh::IsValid() const
    {
      return m_isValid;
    }


  }
}
