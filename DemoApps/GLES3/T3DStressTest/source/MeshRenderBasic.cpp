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

#include "MeshRenderBasic.hpp"
#include <FslBase/UncheckedNumericCast.hpp>
#include <FslUtil/OpenGLES3/GLUtil.hpp>
#include <FslUtil/OpenGLES3/GLValues.hpp>
#include <cassert>
#include "Shader/ShaderBase.hpp"

namespace Fsl
{
  using namespace GLES3;

  MeshRenderBasic::MeshRenderBasic(const Procedural::BasicMesh& mesh)
    : m_vertexCount(mesh.GetVertexCount())
    , m_indexCount(mesh.GetIndexCount())
    , m_vertices(mesh.GetVertexCount())
    , m_normals(mesh.GetVertexCount())
    , m_textureCoords(mesh.GetVertexCount())
    , m_indices(mesh.GetIndexCount())
    , m_primitiveType(GLUtil::Convert(mesh.GetPrimitiveType()))
  {
    const uint32_t indexCount = mesh.GetIndexCount();
    const uint16_t* pSrcIndices = mesh.GetIndices();
    const VertexPositionNormalTexture* const src = mesh.GetVertices();
    const uint32_t vertexCount = mesh.GetVertexCount();
    for (uint32_t i = 0; i < vertexCount; ++i)
    {
      m_vertices[i] = src[i].Position;
      m_normals[i] = src[i].Normal;
      m_textureCoords[i] = src[i].TextureCoordinate;
    }

    for (uint32_t i = 0; i < indexCount; ++i)
    {
      m_indices[i] = static_cast<unsigned short>(pSrcIndices[i]);
    }
  }

  MeshRenderBasic::~MeshRenderBasic() = default;

  void MeshRenderBasic::Bind(const ShaderBase& shader)
  {
    ShaderVertexConfig shaderConfig = shader.GetShaderConfig();
    if (shaderConfig.Position != GLValues::InvalidLocation)
    {
      static_assert(sizeof(Vector3) == (sizeof(GLfloat) * 3), "Vector3 not of the expected size");
      glVertexAttribPointer(shaderConfig.Position, 3, GL_FLOAT, GL_FALSE, 0, m_vertices.data());
      glEnableVertexAttribArray(shaderConfig.Position);
    }

    if (shaderConfig.Normal != GLValues::InvalidLocation)
    {
      static_assert(sizeof(Vector3) == (sizeof(GLfloat) * 3), "Vector3 not of the expected size");
      glVertexAttribPointer(shaderConfig.Normal, 3, GL_FLOAT, GL_FALSE, 0, m_normals.data());
      glEnableVertexAttribArray(shaderConfig.Normal);
    }

    if (shaderConfig.TexCoord != GLValues::InvalidLocation)
    {
      static_assert(sizeof(Vector2) == (sizeof(GLfloat) * 2), "Vector2 not of the expected size");
      glVertexAttribPointer(shaderConfig.TexCoord, 2, GL_FLOAT, GL_FALSE, 0, m_textureCoords.data());
      glEnableVertexAttribArray(shaderConfig.TexCoord);
    }
  }

  void MeshRenderBasic::Draw()
  {
    glDrawElements(m_primitiveType, UncheckedNumericCast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, m_indices.data());
  }

  void MeshRenderBasic::DrawInstanced(const int layerCount)
  {
    glDrawElementsInstanced(m_primitiveType, UncheckedNumericCast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, m_indices.data(), layerCount);
  }

  void MeshRenderBasic::Unbind()
  {
  }
}
