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

#include <Shared/HDR/BasicScene/ModelVertices.hpp>

namespace Fsl
{
  std::array<VertexPositionNormalTexture, 6 * 5> ModelVertices::CreateTunnelVertexArray()
  {
    // B D
    // |\|
    // A C
    // A = 1.0
    const float size = 2.0f;
    const float length = (size * 2.0f) * 8.0f;

    const float yCeiling = size;
    const float yFloor = -size;

    const float xLeft = -size;
    const float xRight = size;
    const float zBack = size - length;    // zBack
    const float zFront = size;            // zFront
    const float u0Back = 0.0f;
    const float u1Back = 1.0f;
    const float v0Back = 1.0f;
    const float v1Back = 0.0f;

    const float u0Side = 0.0f;
    const float u1Side = length / (2 * size);
    const float v0Side = 1.0f;
    const float v1Side = 0.0f;

    const float u0Floor = 0.0f;
    const float u1Floor = length / (2 * size);
    const float v0Floor = 1.0f;
    const float v1Floor = 0.0f;

    const Vector3 normalFloor(0.0f, 1.0f, 0.0f);
    const Vector3 normalCeiling(0.0f, -1.0f, 0.0f);
    const Vector3 normalBack(0.0f, 0.0f, -1.0f);
    const Vector3 normalRight(-1.0f, 0.0f, 0.0f);
    const Vector3 normalLeft(1.0f, 0.0f, 0.0f);

    std::array<VertexPositionNormalTexture, 6 * 5> vertices = {
      // Floor
      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zBack), normalFloor, Vector2(u1Floor, v0Floor)),      // LB
      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zFront), normalFloor, Vector2(u0Floor, v0Floor)),     // LF
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zFront), normalFloor, Vector2(u0Floor, v1Floor)),    // RF

      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zBack), normalFloor, Vector2(u1Floor, v0Floor)),      // LB
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zFront), normalFloor, Vector2(u0Floor, v1Floor)),    // RF
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zBack), normalFloor, Vector2(u1Floor, v1Floor)),     // RB

      // Ceiling
      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zFront), normalCeiling, Vector2(u0Floor, v1Floor)),     // LF
      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zBack), normalCeiling, Vector2(u1Floor, v1Floor)),      // LB
      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zFront), normalCeiling, Vector2(u0Floor, v0Floor)),    // RF

      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zFront), normalCeiling, Vector2(u0Floor, v0Floor)),    // RF
      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zBack), normalCeiling, Vector2(u1Floor, v1Floor)),      // LB
      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zBack), normalCeiling, Vector2(u1Floor, v0Floor)),     // RB


      // Back wall
      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zBack), normalBack, Vector2(u0Back, v0Back)),
      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zBack), normalBack, Vector2(u0Back, v1Back)),
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zBack), normalBack, Vector2(u1Back, v1Back)),

      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zBack), normalBack, Vector2(u0Back, v0Back)),
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zBack), normalBack, Vector2(u1Back, v1Back)),
      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zBack), normalBack, Vector2(u1Back, v0Back)),

      // Right wall
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zBack), normalRight, Vector2(u1Side, v0Side)),       // FB
      VertexPositionNormalTexture(Vector3(xRight, yFloor, zFront), normalRight, Vector2(u0Side, v0Side)),      // FF
      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zFront), normalRight, Vector2(u0Side, v1Side)),    // CF

      VertexPositionNormalTexture(Vector3(xRight, yFloor, zBack), normalRight, Vector2(u1Side, v0Side)),       // FB
      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zFront), normalRight, Vector2(u0Side, v1Side)),    // CF
      VertexPositionNormalTexture(Vector3(xRight, yCeiling, zBack), normalRight, Vector2(u1Side, v1Side)),     // CB

      // Left wall
      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zFront), normalLeft, Vector2(u0Side, v1Side)),      // FF
      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zBack), normalLeft, Vector2(u1Side, v1Side)),       // FB
      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zFront), normalLeft, Vector2(u0Side, v0Side)),    // CF

      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zFront), normalLeft, Vector2(u0Side, v0Side)),    // CF
      VertexPositionNormalTexture(Vector3(xLeft, yFloor, zBack), normalLeft, Vector2(u1Side, v1Side)),       // FB
      VertexPositionNormalTexture(Vector3(xLeft, yCeiling, zBack), normalLeft, Vector2(u1Side, v0Side)),     // CB
    };

    return vertices;
  }


  std::array<VertexPositionTexture, 6> ModelVertices::CreateQuadVertexArray()
  {
    // B D
    // |\|
    // A C
    // A = 1.0

    const float size = 1.0f;
    const float x0 = -size;
    const float x1 = size;
    const float y0 = -size;
    const float y1 = size;
    const float zPos = 0.0f;

    const float u0 = 0.0f;
    const float u1 = 1.0f;
    const float v0 = 0.0f;
    const float v1 = 1.0f;

    std::array<VertexPositionTexture, 6> vertices = {
      // Floor
      VertexPositionTexture(Vector3(x0, y1, zPos), Vector2(u0, v1)), VertexPositionTexture(Vector3(x0, y0, zPos), Vector2(u0, v0)),
      VertexPositionTexture(Vector3(x1, y0, zPos), Vector2(u1, v0)),

      VertexPositionTexture(Vector3(x0, y1, zPos), Vector2(u0, v1)), VertexPositionTexture(Vector3(x1, y0, zPos), Vector2(u1, v0)),
      VertexPositionTexture(Vector3(x1, y1, zPos), Vector2(y1, v1)),
    };
    return vertices;
  }
}
