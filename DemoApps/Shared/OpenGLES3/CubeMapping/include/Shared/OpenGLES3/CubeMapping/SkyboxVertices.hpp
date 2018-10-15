#ifndef SHARED_OPENGLES3_CUBEMAPPING_SKYBOXVERTICES_HPP
#define SHARED_OPENGLES3_CUBEMAPPING_SKYBOXVERTICES_HPP
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

#include <FslGraphics/Vertices/VertexPositionTexture3.hpp>
#include <array>

namespace Fsl
{
  class SkyboxVertices
  {
  public:
    //! @brief Generate skybox vertives
    static std::array<VertexPositionTexture3, 6 * 6> GetVertices(const float cubeDimensions)
    {
      const float cubeFloor = -cubeDimensions;
      const float cubeCeiling = cubeDimensions;
      const float cubeLeft = -cubeDimensions;
      const float cubeRight = cubeDimensions;
      const float cubeBack = -cubeDimensions;
      const float cubeFront = cubeDimensions;

      const float u0 = -1.0f;
      const float v0 = -1.0f;
      const float w0 = -1.0f;
      const float u1 = 1.0f;
      const float v1 = 1.0f;
      const float w1 = 1.0f;

      const std::array<VertexPositionTexture3, 6 * 6> vertices = {
        // A-C   C
        // |/   /|
        // B   B-D

        // Right
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeFront), Vector3(u1, v0, w0)),      // A
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeFront), Vector3(u1, v1, w0)),    // B
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeBack), Vector3(u1, v0, w1)),       // C
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeBack), Vector3(u1, v0, w1)),       // C
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeFront), Vector3(u1, v1, w0)),    // B
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeBack), Vector3(u1, v1, w1)),     // D

        // Left
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeFront), Vector3(u0, v1, w0)),    // A
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeFront), Vector3(u0, v0, w0)),      // B
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeBack), Vector3(u0, v1, w1)),     // C
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeBack), Vector3(u0, v1, w1)),     // C
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeFront), Vector3(u0, v0, w0)),      // B
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeBack), Vector3(u0, v0, w1)),       // D

        // Top
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeBack), Vector3(u1, v1, w1)),     // A
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeFront), Vector3(u1, v1, w0)),    // B
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeBack), Vector3(u0, v1, w1)),      // C
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeBack), Vector3(u0, v1, w1)),      // C
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeFront), Vector3(u1, v1, w0)),    // B
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeFront), Vector3(u0, v1, w0)),     // D

        // Bottom
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeBack), Vector3(u0, v0, w1)),      // A
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeFront), Vector3(u0, v0, w0)),     // B
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeBack), Vector3(u1, v0, w1)),     // C
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeBack), Vector3(u1, v0, w1)),     // C
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeFront), Vector3(u0, v0, w0)),     // B
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeFront), Vector3(u1, v0, w0)),    // D

        // Front
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeFront), Vector3(u1, v1, w0)),    // A
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeFront), Vector3(u1, v0, w0)),      // B
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeFront), Vector3(u0, v1, w0)),     // C
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeFront), Vector3(u0, v1, w0)),     // C
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeFront), Vector3(u1, v0, w0)),      // B
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeFront), Vector3(u0, v0, w0)),       // D

        // Back
        VertexPositionTexture3(Vector3(cubeLeft, cubeCeiling, cubeBack), Vector3(u0, v1, w1)),     // A
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeBack), Vector3(u0, v0, w1)),       // B
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeBack), Vector3(u1, v1, w1)),    // C
        VertexPositionTexture3(Vector3(cubeRight, cubeCeiling, cubeBack), Vector3(u1, v1, w1)),    // C
        VertexPositionTexture3(Vector3(cubeLeft, cubeFloor, cubeBack), Vector3(u0, v0, w1)),       // B
        VertexPositionTexture3(Vector3(cubeRight, cubeFloor, cubeBack), Vector3(u1, v0, w1)),      // D
      };
      return vertices;
    }
  };
}

#endif
