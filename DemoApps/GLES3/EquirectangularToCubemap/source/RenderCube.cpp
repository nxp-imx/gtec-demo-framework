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

#include "RenderCube.hpp"
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslUtil/OpenGLES3/Exceptions.hpp>
#include <FslUtil/OpenGLES3/GLCheck.hpp>
#include <GLES3/gl3.h>
#include <algorithm>
#include <cmath>

namespace Fsl
{
  using namespace GLES3;

  namespace
  {
    // B D
    // |\|
    // A C
    // A = 1.0
    const float CUBE_DIMENSIONS = 1.0f;

    const float CUBE_FLOOR = -CUBE_DIMENSIONS;
    const float CUBE_CEILING = CUBE_DIMENSIONS;
    const float CUBE_LEFT = -CUBE_DIMENSIONS;
    const float CUBE_RIGHT = CUBE_DIMENSIONS;
    const float CUBE_BACK = -CUBE_DIMENSIONS;    // zBack
    const float CUBE_FRONT = CUBE_DIMENSIONS;    // zFront

    const std::array<VertexPosition, 6 * 6> g_vertices = {

      // Right
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK)),

      // Left
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT)),

      // Top
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT)),

      // Bottom
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT)),

      // Front
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_FRONT)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_FRONT)),

      // Back
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_FLOOR, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_RIGHT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_CEILING, CUBE_BACK)),
      VertexPosition(Vector3(CUBE_LEFT, CUBE_FLOOR, CUBE_BACK)),
    };
  }

  RenderCube::RenderCube(const GLProgram& program)
  {
    // Prepare the vertex buffer
    m_vertexBufferInfo.VertexBuffer.Reset(g_vertices.data(), g_vertices.size(), GL_STATIC_DRAW);

    constexpr auto vertexDecl = VertexPosition::GetVertexDeclarationArray();
    m_vertexBufferInfo.AttribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
  }


  RenderCube::~RenderCube() = default;


  void RenderCube::BeginDraw()
  {
    const auto& vertexBuffer = m_vertexBufferInfo.VertexBuffer;
    glBindBuffer(vertexBuffer.GetTarget(), vertexBuffer.Get());

    // Bind the vertex attributes
    vertexBuffer.EnableAttribArrays(m_vertexBufferInfo.AttribLink.data(), m_vertexBufferInfo.AttribLink.size());
  }


  void RenderCube::DrawCube()
  {
    const auto& vertexBuffer = m_vertexBufferInfo.VertexBuffer;
    glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.GetGLCapacity());
  }

  void RenderCube::EndDraw()
  {
    m_vertexBufferInfo.VertexBuffer.DisableAttribArrays();
  }
}
