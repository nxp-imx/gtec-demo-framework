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

#include <FslGraphics3D/Procedural/SegmentedQuadGenerator.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <cassert>
#include <cstdlib>

namespace Fsl
{
  namespace Procedural
  {
    namespace
    {
      void GenerateVertices(std::vector<VertexPositionNormalTexture>& rDst, const BoxF& dstBox, const float dstZ, const int verticesX,
                            const int verticesY, const NativeTextureArea& textureArea)
      {
        assert(verticesX >= 2);
        assert(verticesY >= 2);
        assert(rDst.size() >= std::size_t(verticesX * verticesY));

        const double x1 = dstBox.X1;
        const double y1 = dstBox.Y1;
        const double x2 = dstBox.X2;
        const double y2 = dstBox.Y2;
        const double u1 = textureArea.X1;
        const double v1 = textureArea.Y1;
        const double u2 = textureArea.X2;
        const double v2 = textureArea.Y2;

        const int xEnd = verticesX - 1;
        const int yEnd = verticesY - 1;
        const double xAdd = (x2 - x1) / xEnd;
        const double yAdd = (y2 - y1) / yEnd;
        const double uAdd = (u2 - u1) / xEnd;
        const double vAdd = (v2 - v1) / yEnd;

        // FIX: allow control of normal direction?
        const Vector3 normal(0.0f, 0.0f, 1.0f);

        double yPos = y1;
        double vPos = v1;
        int dstIndex = 0;
        for (int y = 0; y < verticesY; ++y)
        {
          double xPos = x1;
          double uPos = u1;
          for (int x = 0; x < verticesX; ++x)
          {
            rDst[dstIndex].Position = Vector3(static_cast<float>(xPos), static_cast<float>(yPos), dstZ);
            rDst[dstIndex].Normal = normal;
            rDst[dstIndex].TextureCoordinate = Vector2(static_cast<float>(uPos), static_cast<float>(vPos));
            xPos += xAdd;
            uPos += uAdd;
            ++dstIndex;
          }
          yPos += yAdd;
          vPos += vAdd;
        }

        // Clamp the x values to be a 100 match
        dstIndex = verticesX - 1;
        int dstIndexEnd = verticesX * verticesY;
        while (dstIndex < dstIndexEnd)
        {
          rDst[dstIndex].Position.X = dstBox.X2;
          rDst[dstIndex].TextureCoordinate.X = textureArea.X2;
          dstIndex += verticesX;
        }

        // Clamp the y values to be a 100 match
        dstIndex = verticesX * (verticesY - 1);
        dstIndexEnd = verticesX * verticesY;
        while (dstIndex < dstIndexEnd)
        {
          rDst[dstIndex].Position.Y = dstBox.Y2;
          rDst[dstIndex].TextureCoordinate.Y = textureArea.Y2;
          ++dstIndex;
        }

        // Verify that the precision is obeyed (we actually want 100% the same number not something that is considered the same number)
        // We might actually have to write the last entry manually using the input numbers to achieve that.
        assert(rDst.front().Position == Vector3(dstBox.X1, dstBox.Y1, dstZ));
        assert(rDst.back().Position == Vector3(dstBox.X2, dstBox.Y2, dstZ));
        assert(rDst.front().TextureCoordinate == Vector2(textureArea.X1, textureArea.Y1));
        assert(rDst.back().TextureCoordinate == Vector2(textureArea.X2, textureArea.Y2));
      }


      void GenerateTriangleListIndices(std::vector<BasicMesh::index_type>& rDst, const int segmentsX, const int segmentsY,
                                       const WindingOrder::Enum windingOrder)
      {
        assert(segmentsX >= 1);
        assert(segmentsY >= 1);

        assert(rDst.size() >= std::size_t(segmentsX * segmentsY * 6));

        // A-B-C-D
        // |\|\|\|
        // E-F-G-H
        // |\|\|\|
        // I-J-K-L

        const int offset0 = (windingOrder == WindingOrder::CW ? 0 : 1);
        const int offset1 = (windingOrder == WindingOrder::CW ? 1 : 0);
        const int verticesX = segmentsX + 1;

        int dstIdx = 0;
        int yOffset = 0;
        for (int y = 0; y < segmentsY; ++y)
        {
          for (int x = 0; x < segmentsX; ++x)
          {
            rDst[dstIdx + offset0] = yOffset + (x + verticesX);
            rDst[dstIdx + offset1] = yOffset + x;
            rDst[dstIdx + 2] = yOffset + (x + verticesX + 1);

            rDst[dstIdx + offset0 + 3] = yOffset + (x + verticesX + 1);
            rDst[dstIdx + offset1 + 3] = yOffset + x;
            rDst[dstIdx + 5] = yOffset + (x + 1);
            dstIdx += 6;
          }
          yOffset += verticesX;
        }
      }


      void GenerateTriangleStripIndices(std::vector<BasicMesh::index_type>& rDst, const int segmentsX, const int segmentsY,
                                        const WindingOrder::Enum windingOrder)
      {
        assert(segmentsX >= 1);
        assert(segmentsY >= 1);

        // Generate a counter clockwise quad strip mesh
        //         | a   | b   | c   | d   | *e  | *f  | g   | h   | i   | j   | *k  | *l  | m   | n   | o   | p   |
        //         |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
        // 0-1-2   | 0-1 |   1 | 1-2 |   2 |   2 |     |     |     |     |     |     |     |     |     |     |     |
        // |/|/|   | |/  |  /| | |/  |  /| |   | |     |     |     |     |     |     |     |     |     |     |     |
        // 3-4-5   | 3   | 3-4 | 4   | 4-5 |   5 |   5 |   5 | 4-5 |   4 | 3-4 | 3   | 3   |     |     |     |     |
        // |/|/|   |     |     |     |     |   | |   | |  /| | |/  |  /| | |/  | |   | |   |     |     |     |     |
        // 6-7-8   |     |     |     |     |   8 |   8 | 7-8 | 7   | 6-7 | 6   | 6   | 6   | 6-7 |   7 | 7-8 |   8 |
        // |/|/|   |     |     |     |     |     |     |     |     |     |     |     | |   | |/  |  /| | |/  |  /| |
        // 9-A-B   |     |     |     |     |     |     |     |     |     |     |     | 9   | 9   | 9-A | A   | A-B |
        //         |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
        //
        // * = a degenerated triangle
        //
        // Triangle indices (CCW)
        // | Top         | a: 0,3,1 ccw | b: 3,1,4 cw | c: 1,4,2 ccw | d: 4,2,5 cw |
        // | Degenerated | e: 2,5,8 ccw | f: 5,8,5 cw |              |             |  (always two)
        // | Middle      | g: 8,5,7 ccw | h: 5,7,4 cw | i: 7,4,6 ccw | j: 4,6,3 cw |
        // | Degenerated | k: 6,3,6 ccw | l: 3,6,9 cw |              |             |  (always two)
        // | Bottom      | m: 6,9,7 ccw | n: 9,7,A cw | o: 7,A,8 ccw | p: A,8,B cw |

        // Generate a clockwise quad strip mesh
        //         | a   | b   | c   | d   | *e  | *f  | g   | h   | i   | j   | *k  | *l  | m   | n   | o   | p   |
        //         |-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
        // 0-1-2   | 0   | 0-1 | 1   | 1-2 |   2 |   2 |     |     |     |     |     |     |     |     |     |     |
        // |/|/|   | |\  |  \| | |\  |  \| |   | |   | |     |     |     |     |     |     |     |     |     |     |
        // 3-4-5   | 3-4 |   4 | 4-5 |   5 |   5 |   5 | 4-5 | 4   | 3-4 | 3   | 3   |     |     |     |     |     |
        // |/|/|   |     |     |     |     |     |   | |  \| | |\  |  \| | |\  | |   |     |     |     |     |     |
        // 6-7-8   |     |     |     |     |     |   8 |   8 | 7-8 |   7 | 6-7 | 6   | 6   | 6   | 6-7 | 7   | 7-8 |
        // |/|/|   |     |     |     |     |     |     |     |     |     |     | |   | |   | |\  |  \| | |\  |  \| |
        // 9-A-B   |     |     |     |     |     |     |     |     |     |     | 9   | 9   | 9-A |   A | A-B |   B |
        //         |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
        //
        // * = a degenerated triangle
        //
        // Triangle indices (CW)
        // | Top         | a: 3,0,4 cw | b: 0,4,1 ccw | c: 4,1,5 cw | d: 1,5,2 ccw |
        // | Degenerated | e: 5,2,5 cw | f: 2,5,8 ccw |             |              |  (always two)
        // | Middle      | g: 5,8,4 cw | h: 8,4,7 ccw | i: 4,7,3 cw | j: 7,3,6 ccw |
        // | Degenerated | k: 3,6,9 cw | l: 6,9,6 ccw |             |              |  (always two)
        // | Bottom      | m: 9,6,A cw | n: 6,A,7 ccw | o: A,7,B cw | p: 7,B,( ccw |


        assert(rDst.size() == std::size_t(2 + (segmentsX * segmentsY * 2) + ((segmentsY)-1) * 2));

        // Generate the indices for a long triangle strips relying on two degenerated triangles for y-line switching.
        const int yOffset = segmentsX + 1;

        int dstIndex = 0;
        int offset = 0;

        const int dstOffset0 = (windingOrder == WindingOrder::CW ? 0 : 1);
        const int dstOffset1 = (windingOrder == WindingOrder::CW ? 1 : 0);

        // Write the start indices
        BasicMesh::index_type* pIndices = rDst.data();
        pIndices[dstIndex + dstOffset0] = yOffset;
        pIndices[dstIndex + dstOffset1] = 0;
        dstIndex += 2;

        for (int y = 0; y < segmentsY; ++y)
        {
          if ((y & 1) == 0)
          {
            for (int x = 0; x < segmentsX; ++x)
            {
              pIndices[dstIndex + dstOffset0] = (offset + yOffset + x + 1);
              pIndices[dstIndex + dstOffset1] = (offset + x + 1);
              dstIndex += 2;
            }
            // Add two degenerated triangles for y-line switching if there are more rows
            if ((y + 1) < segmentsY)
            {
              pIndices[dstIndex + dstOffset0] = (offset + yOffset + segmentsX);
              pIndices[dstIndex + dstOffset1] = (offset + yOffset + segmentsX) + yOffset;
              dstIndex += 2;
            }
          }
          else
          {
            for (int x = segmentsX - 1; x >= 0; --x)
            {
              pIndices[dstIndex + dstOffset0] = (offset) + x;
              pIndices[dstIndex + dstOffset1] = (offset + yOffset + x);
              dstIndex += 2;
            }
            // Add two degenerated triangles for y-line switching if there are more rows
            if ((y + 1) < segmentsY)
            {
              pIndices[dstIndex + dstOffset0] = (offset + 2 * yOffset);
              pIndices[dstIndex + dstOffset1] = (offset + yOffset);
              dstIndex += 2;
            }
          }
          offset += yOffset;
        }
      }
    }


    BasicMesh SegmentedQuadGenerator::GenerateList(const BoxF& dstBox, const float dstZ, const int segmentsX, const int segmentsY,
                                                   const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
    {
      if (segmentsX < 1 || segmentsY < 1)
      {
        throw std::invalid_argument("invalid arguments");
      }

      const int verticesX = segmentsX + 1;
      const int verticesY = segmentsY + 1;

      std::vector<BasicMesh::vertex_type> vertices(verticesX * verticesY);
      std::vector<BasicMesh::index_type> indices(segmentsX * segmentsY * 6);
      GenerateVertices(vertices, dstBox, dstZ, verticesX, verticesY, textureArea);
      GenerateTriangleListIndices(indices, segmentsX, segmentsY, windingOrder);
      return BasicMesh(vertices, indices, PrimitiveType::TriangleList);
    }


    BasicMesh SegmentedQuadGenerator::GenerateList(const Vector3& dstCenter, const float dstWidth, const float dstHeight, const int segmentsX,
                                                   const int segmentsY, const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
    {
      const float x = dstCenter.X - (dstWidth * 0.5f);
      const float y = dstCenter.Y - (dstHeight * 0.5f);
      const BoxF dstRect(x, -y, dstWidth, -dstHeight, true);
      return GenerateList(dstRect, dstCenter.Z, segmentsX, segmentsY, textureArea, windingOrder);
    }


    BasicMesh SegmentedQuadGenerator::GenerateStrip(const BoxF& dstBox, const float dstZ, const int segmentsX, const int segmentsY,
                                                    const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
    {
      if (segmentsX < 1 || segmentsY < 1)
      {
        throw std::invalid_argument("invalid arguments");
      }

      const int verticesX = segmentsX + 1;
      const int verticesY = segmentsY + 1;
      const int indexCount = 2 + (segmentsX * segmentsY * 2) + (verticesY - 2) * 2;

      std::vector<BasicMesh::vertex_type> vertices(verticesX * verticesY);
      std::vector<BasicMesh::index_type> indices(indexCount);

      GenerateVertices(vertices, dstBox, dstZ, verticesX, verticesY, textureArea);
      GenerateTriangleStripIndices(indices, segmentsX, segmentsY, windingOrder);

      return BasicMesh(vertices, indices, PrimitiveType::TriangleStrip);
    }


    BasicMesh SegmentedQuadGenerator::GenerateStrip(const Vector3& dstCenter, const float dstWidth, const float dstHeight, const int segmentsX,
                                                    const int segmentsY, const NativeTextureArea& textureArea, const WindingOrder::Enum windingOrder)
    {
      const float x = dstCenter.X - (dstWidth * 0.5f);
      const float y = dstCenter.Y - (dstHeight * 0.5f);
      const BoxF dstRect(x, -y, dstWidth, -dstHeight, true);
      return GenerateStrip(dstRect, dstCenter.Z, segmentsX, segmentsY, textureArea, windingOrder);
    }
  }
}
