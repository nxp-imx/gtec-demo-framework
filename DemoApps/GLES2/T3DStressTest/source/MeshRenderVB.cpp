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

#include "MeshRenderVB.hpp"
#include "Shader/ShaderBase.hpp"
#include <FslUtil/OpenGLES2/GLUtil.hpp>

namespace Fsl
{
  MeshRenderVB::MeshRenderVB(const Procedural::BasicMesh& mesh)
    : m_vb(mesh.GetVertices(), mesh.GetVertexCount(), Procedural::BasicMesh::vertex_type::GetVertexDeclaration(), GL_STATIC_DRAW)
    , m_ib(mesh.GetIndices(), mesh.GetIndexCount(), GL_STATIC_DRAW)
    , m_primitiveType(GLES2::GLUtil::Convert(mesh.GetPrimitiveType()))
  {
    m_link[0] = GLES2::GLVertexAttribLink(0, m_vb.GetVertexElementIndex(VertexElementUsage::Position, 0));
    m_link[1] = GLES2::GLVertexAttribLink(0, m_vb.GetVertexElementIndex(VertexElementUsage::Normal, 0));
    m_link[2] = GLES2::GLVertexAttribLink(0, m_vb.GetVertexElementIndex(VertexElementUsage::TextureCoordinate, 0));
  }


  void MeshRenderVB::Bind(const ShaderBase& shader)
  {
    const ShaderVertexConfig& shaderConfig = shader.GetShaderConfig();
    glBindBuffer(m_vb.GetTarget(), m_vb.Get());
    glBindBuffer(m_ib.GetTarget(), m_ib.Get());

    m_link[0].AttribIndex = shaderConfig.Position;
    m_link[1].AttribIndex = shaderConfig.Normal;
    m_link[2].AttribIndex = shaderConfig.TexCoord;
    m_vb.EnableAttribArrays(m_link);
  }

  void MeshRenderVB::Draw()
  {
    glDrawElements(m_primitiveType, m_ib.GetCapacity(), m_ib.GetType(), nullptr);
  }

  void MeshRenderVB::Unbind()
  {
    m_vb.DisableAttribArrays(m_link);
    glBindBuffer(m_vb.GetTarget(), 0);
    glBindBuffer(m_ib.GetTarget(), 0);
  }
}
