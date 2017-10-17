#if 1
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

#include "MeshRenderNormals.hpp"
#include "Shader/ShaderBase.hpp"
#include <FslGraphics/Vertices/VertexPositionColor.hpp>
#include <FslGraphics3D/Procedural/VertexUtil.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  using namespace GLES3;

  MeshRenderNormals::MeshRenderNormals(const Procedural::BasicMesh& mesh)
    : m_vertices(mesh.GetVertexCount() * 2)
  {
    Procedural::VertexUtil::ExtractNormalsAsLineList(m_vertices, 0, mesh.GetVertexArray(), 5);
  }


  MeshRenderNormals::~MeshRenderNormals()
  {
  }


  void MeshRenderNormals::Bind(const ShaderBase& shader)
  {
    const ShaderVertexConfig shaderConfig = shader.GetShaderConfig();
    assert(shaderConfig.Position != GLValues::INVALID_LOCATION);

    glVertexAttribPointer(shaderConfig.Position, 3, GL_FLOAT, GL_FALSE, 0, m_vertices.data());
    glEnableVertexAttribArray(shaderConfig.Position);
  }

  void MeshRenderNormals::Draw()
  {
    assert(m_vertices.size() <= static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()));
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_vertices.size()));
  }

  void MeshRenderNormals::Unbind()
  {
  }

}
#endif
