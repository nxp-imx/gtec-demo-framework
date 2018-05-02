/****************************************************************************************************************************************************
* Copyright 2018 NXP
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
*    * Neither the name of the NXP. nor the names of
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

#include <Shared/OpenGLES3/CubeMapping/SkyboxMesh.hpp>
#include <Shared/OpenGLES3/CubeMapping/SkyboxVertices.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>

namespace Fsl
{
  using namespace GLES3;


  SkyboxMesh::SkyboxMesh()
    : VertexBuffer()
    , VertexArray()
  {
  }


  SkyboxMesh::SkyboxMesh(const GLES3::GLProgram& program)
    : SkyboxMesh()
  {
    Reset(program);
  }


  void SkyboxMesh::Reset()
  {
    VertexBuffer.Reset();
    VertexArray.Reset();
  }


  void SkyboxMesh::Reset(const GLES3::GLProgram& program)
  {
    if (IsValid())
      Reset();

    try
    {
      const std::array<VertexPositionTexture3, 6 * 6> vertices = SkyboxVertices::GetVertices(1.0f);

      auto vertexDecl = VertexPositionTexture3::GetVertexDeclaration();
      std::vector<GLES3::GLVertexAttribLink> attribLink =
      {
        GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0)),
        GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0))
      };

      VertexBuffer.Reset(vertices.data(), vertices.size(), GL_STATIC_DRAW);

      // Prepare the vertex arrays
      VertexArray.Reset(true);
      VertexArray.Bind();
      {
        //Set up VBO Vertex Attribute information
        GL_CHECK(glBindBuffer(VertexBuffer.GetTarget(), VertexBuffer.Get()));
        VertexBuffer.EnableAttribArrays(attribLink);
      }
      VertexArray.Unbind();
    }
    catch (const std::exception&)
    {
      Reset();
      throw;
    }
  }

}
