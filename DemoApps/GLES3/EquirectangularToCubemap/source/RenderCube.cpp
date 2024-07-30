/****************************************************************************************************************************************************
 * Copyright 2018, 2024 NXP
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
    constexpr float CubeDimensions = 1.0f;

    constexpr float CubeFloor = -CubeDimensions;
    constexpr float CubeCeiling = CubeDimensions;
    constexpr float CubeLeft = -CubeDimensions;
    constexpr float CubeRight = CubeDimensions;
    constexpr float CubeBack = -CubeDimensions;    // zBack
    constexpr float CubeFront = CubeDimensions;    // zFront

    const std::array<VertexPosition, 6 * 6> g_vertices = {

      // Right
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeBack)),

      // Left
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeFront)),

      // Top
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeFront)),

      // Bottom
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeFront)),

      // Front
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeFront)),
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeFront)),

      // Back
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeFloor, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeRight, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeLeft, CubeCeiling, CubeBack)),
      VertexPosition(Vector3(CubeLeft, CubeFloor, CubeBack)),
    };
  }

  RenderCube::RenderCube(const GLProgram& program)
  {
    // Prepare the vertex buffer
    m_vertexBufferInfo.VertexBuffer.Reset(g_vertices.data(), g_vertices.size(), GL_STATIC_DRAW);

    constexpr auto VertexDecl = VertexPosition::GetVertexDeclarationArray();
    m_vertexBufferInfo.AttribLink[0] =
      GLVertexAttribLink(program.GetAttribLocation("VertexPosition"), VertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0));
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
