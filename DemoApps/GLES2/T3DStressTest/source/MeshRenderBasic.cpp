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
#include <FslUtil/OpenGLES2/GLUtil.hpp>
#include <FslUtil/OpenGLES2/GLValues.hpp>
#include <cassert>
#include "Shader/ShaderBase.hpp"

namespace Fsl
{
  using namespace GLES2;

  MeshRenderBasic::MeshRenderBasic(const Procedural::BasicMesh& mesh)
    : m_vertexCount(mesh.GetVertexCount())
    , m_indexCount(mesh.GetIndexCount())
    , m_pVertices(nullptr)
    , m_pNormals(nullptr)
    , m_pTextureCoords(nullptr)
    , m_pIndices(nullptr)
    , m_primitiveType(GLUtil::Convert(mesh.GetPrimitiveType()))
  {
    const uint32_t indexCount = mesh.GetIndexCount();
    const uint16_t* pSrcIndices = mesh.GetIndices();
    const VertexPositionNormalTexture* const src = mesh.GetVertices();
    const uint32_t vertexCount = mesh.GetVertexCount();
    m_pVertices = new float[3 * vertexCount];
    m_pNormals = new float[3 * vertexCount];
    m_pTextureCoords = new float[2 * vertexCount];
    m_pIndices = new unsigned short[indexCount];
    for (uint32_t i = 0; i < vertexCount; ++i)
    {
      m_pVertices[i * 3 + 0] = src[i].Position.X;
      m_pVertices[i * 3 + 1] = src[i].Position.Y;
      m_pVertices[i * 3 + 2] = src[i].Position.Z;

      m_pNormals[i * 3 + 0] = src[i].Normal.X;
      m_pNormals[i * 3 + 1] = src[i].Normal.Y;
      m_pNormals[i * 3 + 2] = src[i].Normal.Z;

      m_pTextureCoords[i * 2 + 0] = src[i].TextureCoordinate.X;
      m_pTextureCoords[i * 2 + 1] = src[i].TextureCoordinate.Y;
    }

    for (uint32_t i = 0; i < indexCount; ++i)
    {
      m_pIndices[i] = static_cast<unsigned short>(pSrcIndices[i]);
    }
  }

  MeshRenderBasic::~MeshRenderBasic()
  {
    delete[] m_pVertices;
    m_pVertices = nullptr;
    delete[] m_pNormals;
    m_pNormals = nullptr;
    delete[] m_pTextureCoords;
    m_pTextureCoords = nullptr;
    delete[] m_pIndices;
    m_pIndices = nullptr;
  }

  void MeshRenderBasic::Bind(const ShaderBase& shader)
  {
    ShaderVertexConfig shaderConfig = shader.GetShaderConfig();
    if (shaderConfig.Position != GLValues::INVALID_LOCATION)
    {
      glVertexAttribPointer(shaderConfig.Position, 3, GL_FLOAT, GL_FALSE, 0, m_pVertices);
      glEnableVertexAttribArray(shaderConfig.Position);
    }

    if (shaderConfig.Normal != GLValues::INVALID_LOCATION)
    {
      glVertexAttribPointer(shaderConfig.Normal, 3, GL_FLOAT, GL_FALSE, 0, m_pNormals);
      glEnableVertexAttribArray(shaderConfig.Normal);
    }

    if (shaderConfig.TexCoord != GLValues::INVALID_LOCATION)
    {
      glVertexAttribPointer(shaderConfig.TexCoord, 2, GL_FLOAT, GL_FALSE, 0, m_pTextureCoords);
      glEnableVertexAttribArray(shaderConfig.TexCoord);
    }
  }

  void MeshRenderBasic::Draw()
  {
    glDrawElements(m_primitiveType, UncheckedNumericCast<GLsizei>(m_indexCount), GL_UNSIGNED_SHORT, m_pIndices);
  }

  void MeshRenderBasic::Unbind()
  {
  }
}
