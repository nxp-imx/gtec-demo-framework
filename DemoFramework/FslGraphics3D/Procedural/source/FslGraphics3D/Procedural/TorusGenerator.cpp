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

#include <FslGraphics3D/Procedural/TorusGenerator.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <cassert>
#include <cstdlib>
#include <vector>

namespace Fsl
{
  namespace Procedural
  {
    // Based on the algorithm specified here -> http://paulbourke.net/geometry/torus/
    namespace
    {
      void GenerateRingVertices(std::vector<BasicMesh::vertex_type>& rVertices, int& rVertexIndex, const int minorSegments, const float radius,
                                const float ringRadius, const float mod, const float horizontalAngularStride, const float sinTheta,
                                const float cosTheta, const float uCurrent, const float v1, const float v2, const float vAdd)
      {
        float phi = 0.0f;
        for (int horizontalIndex = 0; horizontalIndex < minorSegments; ++horizontalIndex)
        {
          phi = horizontalAngularStride * float(horizontalIndex);

          // position
          float z = ringRadius * std::sin(phi);
          const float cosPhi = std::cos(phi);
          {
            float x = cosTheta * (radius + ringRadius * cosPhi);
            float y = sinTheta * (radius + ringRadius * cosPhi);

            rVertices[rVertexIndex].Position = Vector3(x, z, y);
          }
          // normal
          {
            float x = cosTheta * (ringRadius * cosPhi);
            float y = sinTheta * (ringRadius * cosPhi);

            Vector3 pos = Vector3(x * mod, z * mod, y * mod);
            pos.Normalize();
            rVertices[rVertexIndex].Normal = pos;
          }
          rVertices[rVertexIndex].TextureCoordinate = Vector2(uCurrent, v1 + (float(horizontalIndex) * vAdd));
          ++rVertexIndex;
        }

        // The last coordinate is equal to the first except for the texture coordinate
        rVertices[rVertexIndex] = rVertices[rVertexIndex - minorSegments];
        rVertices[rVertexIndex].TextureCoordinate = Vector2(uCurrent, v2);
        ++rVertexIndex;
      }

      void GenerateVertices(std::vector<BasicMesh::vertex_type>& rVertices, const int majorSegments, const int minorSegments, const float radius,
                            const float ringRadius, const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
      {
        const float verticalAngularStride = (MathHelper::PI * 2.0f) / float(majorSegments);
        const float horizontalAngularStride = (MathHelper::PI * 2.0f) / float(minorSegments);

        float mod = 0.0f;
        if (windingOrder == WindingOrder::CCW)
        {
          mod = 1.0f;
        }
        else
        {
          mod = -1.0f;
        }

        const float u1 = textureArea.X0;
        const float v1 = textureArea.Y1;
        const float u2 = textureArea.X1;
        const float v2 = textureArea.Y0;
        const float uAdd = (u2 - u1) / float(majorSegments);
        const float vAdd = (v2 - v1) / float(minorSegments);


        float theta = 0.0f;
        int vertexIndex = 0;
        for (int verticalIndex = 0; verticalIndex < majorSegments; ++verticalIndex)
        {
          theta = verticalAngularStride * float(verticalIndex);
          const float sinTheta = std::sin(theta);
          const float cosTheta = std::cos(theta);
          const auto uCurrent = static_cast<float>(u1 + (float(verticalIndex) * uAdd));

          GenerateRingVertices(rVertices, vertexIndex, minorSegments, radius, ringRadius, mod, horizontalAngularStride, sinTheta, cosTheta, uCurrent,
                               v1, v2, vAdd);
        }
        {
          // The last ring is equal to the first except for the texture coordinate
          const float sinTheta = std::sin(0.0f);
          const float cosTheta = std::cos(0.0f);
          GenerateRingVertices(rVertices, vertexIndex, minorSegments, radius, ringRadius, mod, horizontalAngularStride, sinTheta, cosTheta, u2, v1,
                               v2, vAdd);
        }
      }


      void GenerateTriangleListIndices(std::vector<BasicMesh::index_type>& rIndices, const int majorSegments, const int minorSegments,
                                       const WindingOrder::Enum windingOrder)
      {
        int numVerticesPerRow = minorSegments + 1;

        const int offset0 = (windingOrder == WindingOrder::CCW ? 0 : 1);
        const int offset1 = (windingOrder == WindingOrder::CCW ? 1 : 0);

        int index = 0;
        for (int verticalIndex = 0; verticalIndex < majorSegments; ++verticalIndex)
        {
          for (int horizontalIndex = 0; horizontalIndex < minorSegments; ++horizontalIndex)
          {
            int lt = ((horizontalIndex + 0) + (verticalIndex + 1) * (numVerticesPerRow));
            int rt = ((horizontalIndex + 1) + (verticalIndex + 1) * (numVerticesPerRow));

            int lb = ((horizontalIndex + 0) + (verticalIndex + 0) * (numVerticesPerRow));
            int rb = ((horizontalIndex + 1) + (verticalIndex + 0) * (numVerticesPerRow));

            rIndices[index + offset0] = rt;
            rIndices[index + offset1] = lt;
            rIndices[index + 2] = lb;
            index += 3;

            rIndices[index + offset0] = rb;
            rIndices[index + offset1] = rt;
            rIndices[index + 2] = lb;
            index += 3;
          }
        }
      }

      void GenerateTriangleStripIndices(std::vector<BasicMesh::index_type>& rIndices, const int majorSegments, const int minorSegments,
                                        const WindingOrder::Enum windingOrder)
      {
        // BEWARE of the flip  in the code below

        // Generate a counter clockwise quad strip mesh
        //         | a   | b   | c   | d   | *e  | *f  | *g  | *h  | i   | j   | k   | l   |
        //         |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
        // 0-1-2   | 0-1 |   1 | 1-2 |   2 |   2 |     |     |     |     |     |     |     |
        // |/|/|   | |/  |  /| | |/  |  /| |   | |     |     |     |     |     |     |     |
        // 3-4-5   | 3   | 3-4 | 4   | 4-5 |   5 | 3-5 | 3-5 | 3   | 3-4 |   4 | 4-5 |   5 |
        // |/|/|   |     |     |     |     |     |     |     | |   | |/  |  /| | |/  |  /| |
        // 6-7-8   |     |     |     |     |     |     |     | 6   | 6   | 6-7 | 7   | 7-8 |
        //
        // * = a degenerated triangle
        //
        // Triangle indices (CCW)
        // | Top         | a: 0,3,1 ccw | b: 3,1,4 cw | c: 1,4,2 ccw | d: 4,2,5 cw |                | 0,3,1,4,2,5
        // | Degenerated | e: 2,5,5 ccw | f: 5,5,3 cw | g: 5,3,3 ccw | h: 3,3,6 cw |  (always four) | 5,3,3,6
        // | Middle      | i: 3,6,4 ccw | j: 6,4,7 cw | k: 4,7,5 ccw | l: 7,5,8 cw |                | 4,7,5,8

        // Generate a clockwise quad strip mesh
        //         | a   | b   | c   | d   | *e  | *f    | *h    | *h  | i   | j   | k   | l   |
        //         |-----|-----|-----|-----|-----|-------|-------|-----|-----|-----|-----|-----|
        // 0-1-2   | 0   | 0-1 | 1   | 1-2 |   2 |     2 |     2 |     |     |     |     |     |
        // |\|\|   | |\  |  \| | |\  |  \| |   | |    /  |    /  |     |     |     |     |     |
        // 3-4-5   | 3-4 |   4 | 4-5 |   5 |   5 |   /   |   /   | 3   | 3   | 3-4 | 7   | 4-5 |
        // |\|\|   |     |     |     |     |     |  /    |  /    | |   | |\  |  \| | |\  |  \| |
        // 6-7-8   |     |     |     |     |     | 6     | 6     | 6   | 6-7 |   7 | 4-8 |   8 |
        //
        // * = a degenerated triangle
        //
        // Triangle indices (CW)
        // | Top         | a: 3,0,4 cw | b: 0,4,1 ccw | c: 4,1,5 cw | d: 1,5,2 ccw |                | 3,0,4,1,5,2
        // | Degenerated | e: 5,2,2 cw | f: 2,2,6 ccw | g: 2,6,6 cw | h: 6,6,3 ccw | (always four)  | 2,6,6,3
        // | Middle      | i: 6,3,7 cw | j: 3,7,4 ccw | k: 7,4,8 cw | l: 4,8,5 ccw |                | 7,4,8,5

        // Generate the indices for a long triangle strips relying on two degenerated triangles for y-line switching.
        const int yOffset = minorSegments + 1;

        int dstIndex = 0;
        int offset = 0;

        const int dstOffset0 = (windingOrder == WindingOrder::CCW ? 0 : 1);
        const int dstOffset1 = (windingOrder == WindingOrder::CCW ? 1 : 0);

        // Write the start indices
        BasicMesh::index_type* pIndices = rIndices.data();
        pIndices[dstIndex + dstOffset0] = yOffset;
        pIndices[dstIndex + dstOffset1] = 0;
        dstIndex += 2;
        for (int y = 0; y < majorSegments; ++y)
        {
          for (int x = 0; x < minorSegments; ++x)
          {
            pIndices[dstIndex + dstOffset0] = (offset + yOffset + x + 1);
            pIndices[dstIndex + dstOffset1] = (offset + x + 1);
            dstIndex += 2;
          }
          // Add two degenerated triangles for y-line switching if there are more rows
          if ((y + 1) < majorSegments)
          {
            if (windingOrder == WindingOrder::CCW)
            {
              pIndices[dstIndex + 0] = pIndices[dstIndex - 1];
              pIndices[dstIndex + 1] = offset + yOffset * 2;
              pIndices[dstIndex + 2] = offset + yOffset * 2;
              pIndices[dstIndex + 3] = offset + yOffset;
            }
            else
            {
              pIndices[dstIndex + 0] = pIndices[dstIndex - 1];
              pIndices[dstIndex + 1] = offset + yOffset;
              pIndices[dstIndex + 2] = offset + yOffset;
              pIndices[dstIndex + 3] = offset + yOffset * 2;
            }
            dstIndex += 4;
          }
          offset += yOffset;
        }
        assert(std::size_t(dstIndex) == rIndices.size());
      }
    }


    BasicMesh TorusGenerator::GenerateList(const int majorSegments, const int minorSegments, const float radius, const float ringRadius,
                                           const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
    {
      const int numVerticesPerRow = minorSegments + 1;
      const int numVerticesPerColumn = majorSegments + 1;
      const int numVertices = numVerticesPerRow * numVerticesPerColumn;

      const int numIndices = minorSegments * majorSegments * 6;

      std::vector<BasicMesh::vertex_type> vertices(numVertices);
      std::vector<BasicMesh::index_type> indices(numIndices);

      GenerateVertices(vertices, majorSegments, minorSegments, radius, ringRadius, textureArea, windingOrder);
      GenerateTriangleListIndices(indices, majorSegments, minorSegments, windingOrder);
      return BasicMesh(vertices, indices, PrimitiveType::TriangleList);
    }


    BasicMesh TorusGenerator::GenerateStrip(const int majorSegments, const int minorSegments, const float radius, const float ringRadius,
                                            const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
    {
      // The torus generated here is not the most efficient strip possible, but the generated quads are ordered
      const int numVerticesPerRow = minorSegments + 1;
      const int numVerticesPerColumn = majorSegments + 1;
      const int numVertices = numVerticesPerRow * numVerticesPerColumn;

      const int verticesY = majorSegments + 1;
      const int numIndices = 2 + (minorSegments * majorSegments * 2) + ((verticesY - 2) * 4);

      std::vector<BasicMesh::vertex_type> vertices(numVertices);
      std::vector<BasicMesh::index_type> indices(numIndices);

      GenerateVertices(vertices, majorSegments, minorSegments, radius, ringRadius, textureArea, windingOrder);
      GenerateTriangleStripIndices(indices, majorSegments, minorSegments, windingOrder);
      return BasicMesh(vertices, indices, PrimitiveType::TriangleStrip);
    }
  }
}
