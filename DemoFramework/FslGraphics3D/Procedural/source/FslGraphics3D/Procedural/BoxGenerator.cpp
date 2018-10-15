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

#include <FslGraphics3D/Procedural/BoxGenerator.hpp>
#include <FslGraphics3D/Procedural/IndexUtil.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <cassert>
#include <cstdlib>
#include <limits>

namespace Fsl
{
  namespace Procedural
  {
    namespace
    {
      void GenerateVertices(std::vector<BasicMesh::vertex_type>& rVertices, const Vector3& dstCenter, const float dstWidth, const float dstHeight,
                            const float dstDepth, const NativeTextureArea* const pTextureAreas, const int32_t numTextureAreas,
                            const WindingOrder::Enum windingOrder)
      {
        if (dstWidth <= 0.0f || dstHeight <= 0.0f || dstDepth <= 0.0f || pTextureAreas == nullptr || numTextureAreas < 6)
        {
          throw std::invalid_argument("GenerateVertices");
        }

        assert(rVertices.size() >= (4 * 6));

        const float dx2 = dstWidth * 0.5f;
        const float dy2 = dstWidth * 0.5f;
        const float dz2 = dstWidth * 0.5f;
        const float x1 = dstCenter.X - dx2;
        const float x2 = x1 + dstWidth;
        const float y1 = dstCenter.Y + dy2;
        const float y2 = y1 - dstHeight;
        const float z1 = dstCenter.Z + dz2;
        const float z2 = z1 - dstDepth;

        //   F---E
        //  /|  /|
        // A---B |
        // | H-|-G
        // |/  |/
        // C---D


        // Front (C,A,D,B)
        const NativeTextureArea* pTextureArea = pTextureAreas;
        int index = 0;
        rVertices[index + 0].Position = Vector3(x1, y2, z1);
        rVertices[index + 1].Position = Vector3(x1, y1, z1);
        rVertices[index + 2].Position = Vector3(x2, y2, z1);
        rVertices[index + 3].Position = Vector3(x2, y1, z1);
        rVertices[index + 0].Normal = Vector3(0.0f, 0.0f, 1.0f);
        rVertices[index + 1].Normal = Vector3(0.0f, 0.0f, 1.0f);
        rVertices[index + 2].Normal = Vector3(0.0f, 0.0f, 1.0f);
        rVertices[index + 3].Normal = Vector3(0.0f, 0.0f, 1.0f);
        rVertices[index + 0].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y2);
        rVertices[index + 1].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y1);
        rVertices[index + 2].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y2);
        rVertices[index + 3].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y1);

        // Right (D,B,G,E)
        ++pTextureArea;
        index += 4;
        rVertices[index + 0].Position = Vector3(x2, y2, z1);
        rVertices[index + 1].Position = Vector3(x2, y1, z1);
        rVertices[index + 2].Position = Vector3(x2, y2, z2);
        rVertices[index + 3].Position = Vector3(x2, y1, z2);
        rVertices[index + 0].Normal = Vector3(1.0f, 0.0f, 0.0f);
        rVertices[index + 1].Normal = Vector3(1.0f, 0.0f, 0.0f);
        rVertices[index + 2].Normal = Vector3(1.0f, 0.0f, 0.0f);
        rVertices[index + 3].Normal = Vector3(1.0f, 0.0f, 0.0f);
        rVertices[index + 0].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y2);
        rVertices[index + 1].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y1);
        rVertices[index + 2].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y2);
        rVertices[index + 3].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y1);

        // Back (G,E,H,F)
        ++pTextureArea;
        index += 4;
        rVertices[index + 0].Position = Vector3(x2, y2, z2);
        rVertices[index + 1].Position = Vector3(x2, y1, z2);
        rVertices[index + 2].Position = Vector3(x1, y2, z2);
        rVertices[index + 3].Position = Vector3(x1, y1, z2);
        rVertices[index + 0].Normal = Vector3(0.0f, 0.0f, -1.0f);
        rVertices[index + 1].Normal = Vector3(0.0f, 0.0f, -1.0f);
        rVertices[index + 2].Normal = Vector3(0.0f, 0.0f, -1.0f);
        rVertices[index + 3].Normal = Vector3(0.0f, 0.0f, -1.0f);
        rVertices[index + 0].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y2);
        rVertices[index + 1].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y1);
        rVertices[index + 2].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y2);
        rVertices[index + 3].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y1);

        // Left (H,F,C,A)
        ++pTextureArea;
        index += 4;
        rVertices[index + 0].Position = Vector3(x1, y2, z2);
        rVertices[index + 1].Position = Vector3(x1, y1, z2);
        rVertices[index + 2].Position = Vector3(x1, y2, z1);
        rVertices[index + 3].Position = Vector3(x1, y1, z1);
        rVertices[index + 0].Normal = Vector3(-1.0f, 0.0f, 0.0f);
        rVertices[index + 1].Normal = Vector3(-1.0f, 0.0f, 0.0f);
        rVertices[index + 2].Normal = Vector3(-1.0f, 0.0f, 0.0f);
        rVertices[index + 3].Normal = Vector3(-1.0f, 0.0f, 0.0f);
        rVertices[index + 0].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y2);
        rVertices[index + 1].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y1);
        rVertices[index + 2].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y2);
        rVertices[index + 3].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y1);

        // Top (A,F,B,E)
        ++pTextureArea;
        index += 4;
        rVertices[index + 0].Position = Vector3(x1, y1, z1);
        rVertices[index + 1].Position = Vector3(x1, y1, z2);
        rVertices[index + 2].Position = Vector3(x2, y1, z1);
        rVertices[index + 3].Position = Vector3(x2, y1, z2);
        rVertices[index + 0].Normal = Vector3(0.0f, 1.0f, 0.0f);
        rVertices[index + 1].Normal = Vector3(0.0f, 1.0f, 0.0f);
        rVertices[index + 2].Normal = Vector3(0.0f, 1.0f, 0.0f);
        rVertices[index + 3].Normal = Vector3(0.0f, 1.0f, 0.0f);
        rVertices[index + 0].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y2);
        rVertices[index + 1].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y1);
        rVertices[index + 2].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y2);
        rVertices[index + 3].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y1);

        // Bottom (H,C,G,D)
        ++pTextureArea;
        index += 4;
        rVertices[index + 0].Position = Vector3(x1, y2, z2);
        rVertices[index + 1].Position = Vector3(x1, y2, z1);
        rVertices[index + 2].Position = Vector3(x2, y2, z2);
        rVertices[index + 3].Position = Vector3(x2, y2, z1);
        rVertices[index + 0].Normal = Vector3(0.0f, -1.0f, 0.0f);
        rVertices[index + 1].Normal = Vector3(0.0f, -1.0f, 0.0f);
        rVertices[index + 2].Normal = Vector3(0.0f, -1.0f, 0.0f);
        rVertices[index + 3].Normal = Vector3(0.0f, -1.0f, 0.0f);
        rVertices[index + 0].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y2);
        rVertices[index + 1].TextureCoordinate = Vector2(pTextureArea->X1, pTextureArea->Y1);
        rVertices[index + 2].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y2);
        rVertices[index + 3].TextureCoordinate = Vector2(pTextureArea->X2, pTextureArea->Y1);
      }


      void GenerateTriangleListIndices(std::vector<BasicMesh::index_type>& rIndices, const WindingOrder::Enum windingOrder)
      {
        const int offset0 = (windingOrder == WindingOrder::CCW ? 1 : 0);
        const int offset1 = (windingOrder == WindingOrder::CCW ? 0 : 1);

        int index = 0;
        BasicMesh::index_type srcIndex = 0;
        for (int i = 0; i < 6; ++i)
        {
          rIndices[index + offset0] = srcIndex + 0;
          rIndices[index + offset1] = srcIndex + 1;
          rIndices[index + 2] = srcIndex + 2;
          index += 3;

          rIndices[index + offset0] = srcIndex + 2;
          rIndices[index + offset1] = srcIndex + 1;
          rIndices[index + 2] = srcIndex + 3;
          index += 3;
          srcIndex += 4;
        }
      }


      void GenerateTriangleStripIndices(std::vector<BasicMesh::index_type>& rIndices, const WindingOrder::Enum windingOrder)
      {
        if (windingOrder != WindingOrder::CCW)
        {
          rIndices[0] = 0;
          rIndices[1] = 1;
          rIndices[2] = 2;
          rIndices[3] = 3;
        }
        else
        {
          rIndices[0] = 1;
          rIndices[1] = 0;
          rIndices[2] = 3;
          rIndices[3] = 2;
        }

        std::size_t dstIndex = 4;
        for (BasicMesh::index_type i = 0; i < 5; ++i)
        {
          assert((4 + i * 4) <= std::numeric_limits<BasicMesh::index_type>::max());
          dstIndex += IndexUtil::MergeTriangleStripIndices(rIndices, dstIndex, rIndices, 0, 4, static_cast<BasicMesh::index_type>(4 + i * 4));
        }
      }
    }


    BasicMesh BoxGenerator::GenerateList(const Vector3& dstCenter, const float dstWidth, const float dstHeight, const float dstDepth,
                                         const NativeTextureArea* const pTextureAreas, const int32_t numTextureAreas,
                                         const WindingOrder::Enum windingOrder)
    {
      const int numVertices = 4 * 6;    // four for each side, six sides
      const int numIndices = 6 * 6;     // six for each side, six sides

      std::vector<BasicMesh::vertex_type> vertices(numVertices);
      std::vector<BasicMesh::index_type> indices(numIndices);

      GenerateVertices(vertices, dstCenter, dstWidth, dstHeight, dstDepth, pTextureAreas, numTextureAreas, windingOrder);
      GenerateTriangleListIndices(indices, windingOrder);
      return BasicMesh(vertices, indices, PrimitiveType::TriangleList);
    }


    BasicMesh BoxGenerator::GenerateStrip(const Vector3& dstCenter, const float dstWidth, const float dstHeight, const float dstDepth,
                                          const NativeTextureArea* const pTextureAreas, const int32_t numTextureAreas,
                                          const WindingOrder::Enum windingOrder)
    {
      const int numVertices = 4 * 6;                 // four for each side, six sides
      const int numIndices = 4 * 6 + (6 - 1) * 4;    // four for each side, six sides, and 4 degenerated per side beside the first


      std::vector<BasicMesh::vertex_type> vertices(numVertices);
      std::vector<BasicMesh::index_type> indices(numIndices);

      GenerateVertices(vertices, dstCenter, dstWidth, dstHeight, dstDepth, pTextureAreas, numTextureAreas, windingOrder);
      GenerateTriangleStripIndices(indices, windingOrder);
      return BasicMesh(vertices, indices, PrimitiveType::TriangleStrip);
    }
  }
}
