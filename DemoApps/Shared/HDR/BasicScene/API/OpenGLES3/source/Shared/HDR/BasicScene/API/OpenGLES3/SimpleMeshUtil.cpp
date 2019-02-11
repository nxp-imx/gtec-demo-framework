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

#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <FslGraphics/Vertices/VertexPositionNormalTexture.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <Shared/HDR/BasicScene/ModelVertices.hpp>
#include <Shared/HDR/BasicScene/API/OpenGLES3/SimpleMeshUtil.hpp>
#include <array>

namespace Fsl
{
  using namespace GLES3;

  SimpleMesh SimpleMeshUtil::CreateTunnelVertexArray(const GLES3::GLProgram& program)
  {
    GLVertexBuffer vertexBuffer(ModelVertices::CreateTunnelVertexArray(), GL_STATIC_DRAW);

    std::array<GLES3::GLVertexAttribLink, 3> attribLink = {
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexBuffer.GetVertexElementIndex(VertexElementUsage::Position, 0)),
      GLVertexAttribLink(program.GetAttribLocation("VertexNormal"), vertexBuffer.GetVertexElementIndex(VertexElementUsage::Normal, 0)),
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexBuffer.GetVertexElementIndex(VertexElementUsage::TextureCoordinate, 0))};

    // Prepare the vertex arrays
    GLVertexArray vertexArray(true);
    vertexArray.Bind();
    {
      // Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(vertexBuffer.GetTarget(), vertexBuffer.Get()));
      vertexBuffer.EnableAttribArrays(attribLink);
    }
    vertexArray.Unbind();

    return SimpleMesh(std::move(vertexBuffer), std::move(vertexArray));
  }


  SimpleMesh SimpleMeshUtil::CreateQuadVertexArray(const GLES3::GLProgram& program)
  {
    GLVertexBuffer vertexBuffer(ModelVertices::CreateQuadVertexArray(), GL_STATIC_DRAW);

    std::array<GLES3::GLVertexAttribLink, 2> attribLink = {
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexBuffer.GetVertexElementIndex(VertexElementUsage::Position, 0)),
      GLVertexAttribLink(program.GetAttribLocation("VertexTexCoord"), vertexBuffer.GetVertexElementIndex(VertexElementUsage::TextureCoordinate, 0))};

    // Prepare the vertex arrays
    GLVertexArray vertexArray(true);
    vertexArray.Bind();
    {
      // Set up VBO Vertex Attribute information
      GL_CHECK(glBindBuffer(vertexBuffer.GetTarget(), vertexBuffer.Get()));
      vertexBuffer.EnableAttribArrays(attribLink);
    }
    vertexArray.Unbind();
    return SimpleMesh(std::move(vertexBuffer), std::move(vertexArray));
  }
}
