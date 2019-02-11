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

#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Math/BoundingFrustum.hpp>
#include <FslBase/Math/BoxF.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Math/Ray.hpp>
#include <FslBase/Math/Rect.hpp>
#include <FslBase/Math/Rectangle.hpp>
#include <FslBase/Math/Rectangle2D.hpp>
#include <FslBase/Math/Rectangle3D.hpp>
#include <FslBase/Math/Vector3.hpp>

namespace Fsl
{
  namespace Graphics3D
  {
    constexpr std::size_t LineBuilder::MIN_VERTEX_CAPACITY;
    constexpr float LineBuilder::RAY_LENGTH;

    namespace
    {
      inline void Fill(std::vector<VertexPositionColor>& rVertices, const uint32_t dstOffset, const std::array<Vector3, 8>& corners,
                       const Vector4& color)
      {
        assert(rVertices.size() >= 24u);
        assert(dstOffset <= (rVertices.size() - 24u));
        auto* pDst = rVertices.data() + dstOffset;

        // Completely unrolled with direct sets to prevent unnecessary temporary writes

        // near,left,top -> near,right,top
        pDst[0].Position = corners[0];
        pDst[0].Color = color;
        pDst[1].Position = corners[1];
        pDst[1].Color = color;

        // near,right,top -> near,right,bottom
        pDst[2].Position = corners[1];
        pDst[2].Color = color;
        pDst[3].Position = corners[2];
        pDst[3].Color = color;

        // near,right,bottom -> near,left,bottom
        pDst[4].Position = corners[2];
        pDst[4].Color = color;
        pDst[5].Position = corners[3];
        pDst[5].Color = color;

        // near,left,bottom -> near,left,top
        pDst[6].Position = corners[3];
        pDst[6].Color = color;
        pDst[7].Position = corners[0];
        pDst[7].Color = color;

        // far,left,top -> far,right,top
        pDst[8].Position = corners[4];
        pDst[8].Color = color;
        pDst[9].Position = corners[5];
        pDst[9].Color = color;

        // far,right,top -> far,right,bottom
        pDst[10].Position = corners[5];
        pDst[10].Color = color;
        pDst[11].Position = corners[6];
        pDst[11].Color = color;

        // far,right,bottom -> far,left,bottom
        pDst[12].Position = corners[6];
        pDst[12].Color = color;
        pDst[13].Position = corners[7];
        pDst[13].Color = color;

        // far,left,bottom -> far,left,top
        pDst[14].Position = corners[7];
        pDst[14].Color = color;
        pDst[15].Position = corners[4];
        pDst[15].Color = color;

        // near,left,top -> far,left,top
        pDst[16].Position = corners[0];
        pDst[16].Color = color;
        pDst[17].Position = corners[4];
        pDst[17].Color = color;

        // near,right,top -> far,right,top
        pDst[18].Position = corners[1];
        pDst[18].Color = color;
        pDst[19].Position = corners[5];
        pDst[19].Color = color;

        // near,right,bottom -> far,right,bottom
        pDst[20].Position = corners[2];
        pDst[20].Color = color;
        pDst[21].Position = corners[6];
        pDst[21].Color = color;

        // near,left,bottom -> far,left,bottom
        pDst[22].Position = corners[3];
        pDst[22].Color = color;
        pDst[23].Position = corners[7];
        pDst[23].Color = color;
      }
    }

    void LineBuilder::Add(const BoundingBox& boundingBox, const Vector4& color)
    {
      EnsureCapacityFor(24u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      pDst[0].Position.X = boundingBox.Min.X;
      pDst[0].Position.Y = boundingBox.Min.Y;
      pDst[0].Position.Z = boundingBox.Min.Z;
      pDst[0].Color = color;
      pDst[1].Position.X = boundingBox.Max.X;
      pDst[1].Position.Y = boundingBox.Min.Y;
      pDst[1].Position.Z = boundingBox.Min.Z;
      pDst[1].Color = color;

      pDst[2].Position.X = boundingBox.Min.X;
      pDst[2].Position.Y = boundingBox.Max.Y;
      pDst[2].Position.Z = boundingBox.Min.Z;
      pDst[2].Color = color;
      pDst[3].Position.X = boundingBox.Max.X;
      pDst[3].Position.Y = boundingBox.Max.Y;
      pDst[3].Position.Z = boundingBox.Min.Z;
      pDst[3].Color = color;

      pDst[4].Position.X = boundingBox.Min.X;
      pDst[4].Position.Y = boundingBox.Min.Y;
      pDst[4].Position.Z = boundingBox.Max.Z;
      pDst[4].Color = color;
      pDst[5].Position.X = boundingBox.Max.X;
      pDst[5].Position.Y = boundingBox.Min.Y;
      pDst[5].Position.Z = boundingBox.Max.Z;
      pDst[5].Color = color;

      pDst[6].Position.X = boundingBox.Min.X;
      pDst[6].Position.Y = boundingBox.Max.Y;
      pDst[6].Position.Z = boundingBox.Max.Z;
      pDst[6].Color = color;
      pDst[7].Position.X = boundingBox.Max.X;
      pDst[7].Position.Y = boundingBox.Max.Y;
      pDst[7].Position.Z = boundingBox.Max.Z;
      pDst[7].Color = color;

      pDst[8].Position.X = boundingBox.Min.X;
      pDst[8].Position.Y = boundingBox.Min.Y;
      pDst[8].Position.Z = boundingBox.Min.Z;
      pDst[8].Color = color;
      pDst[9].Position.X = boundingBox.Min.X;
      pDst[9].Position.Y = boundingBox.Min.Y;
      pDst[9].Position.Z = boundingBox.Max.Z;
      pDst[9].Color = color;

      pDst[10].Position.X = boundingBox.Max.X;
      pDst[10].Position.Y = boundingBox.Min.Y;
      pDst[10].Position.Z = boundingBox.Min.Z;
      pDst[10].Color = color;
      pDst[11].Position.X = boundingBox.Max.X;
      pDst[11].Position.Y = boundingBox.Min.Y;
      pDst[11].Position.Z = boundingBox.Max.Z;
      pDst[11].Color = color;

      pDst[12].Position.X = boundingBox.Min.X;
      pDst[12].Position.Y = boundingBox.Max.Y;
      pDst[12].Position.Z = boundingBox.Min.Z;
      pDst[12].Color = color;
      pDst[13].Position.X = boundingBox.Min.X;
      pDst[13].Position.Y = boundingBox.Max.Y;
      pDst[13].Position.Z = boundingBox.Max.Z;
      pDst[13].Color = color;

      pDst[14].Position.X = boundingBox.Max.X;
      pDst[14].Position.Y = boundingBox.Max.Y;
      pDst[14].Position.Z = boundingBox.Min.Z;
      pDst[14].Color = color;
      pDst[15].Position.X = boundingBox.Max.X;
      pDst[15].Position.Y = boundingBox.Max.Y;
      pDst[15].Position.Z = boundingBox.Max.Z;
      pDst[15].Color = color;

      pDst[16].Position.X = boundingBox.Min.X;
      pDst[16].Position.Y = boundingBox.Min.Y;
      pDst[16].Position.Z = boundingBox.Min.Z;
      pDst[16].Color = color;
      pDst[17].Position.X = boundingBox.Min.X;
      pDst[17].Position.Y = boundingBox.Max.Y;
      pDst[17].Position.Z = boundingBox.Min.Z;
      pDst[17].Color = color;

      pDst[18].Position.X = boundingBox.Min.X;
      pDst[18].Position.Y = boundingBox.Min.Y;
      pDst[18].Position.Z = boundingBox.Max.Z;
      pDst[18].Color = color;
      pDst[19].Position.X = boundingBox.Min.X;
      pDst[19].Position.Y = boundingBox.Max.Y;
      pDst[19].Position.Z = boundingBox.Max.Z;
      pDst[19].Color = color;

      pDst[20].Position.X = boundingBox.Max.X;
      pDst[20].Position.Y = boundingBox.Min.Y;
      pDst[20].Position.Z = boundingBox.Min.Z;
      pDst[20].Color = color;
      pDst[21].Position.X = boundingBox.Max.X;
      pDst[21].Position.Y = boundingBox.Max.Y;
      pDst[21].Position.Z = boundingBox.Min.Z;
      pDst[21].Color = color;

      pDst[22].Position.X = boundingBox.Max.X;
      pDst[22].Position.Y = boundingBox.Min.Y;
      pDst[22].Position.Z = boundingBox.Max.Z;
      pDst[22].Color = color;
      pDst[23].Position.X = boundingBox.Max.X;
      pDst[23].Position.Y = boundingBox.Max.Y;
      pDst[23].Position.Z = boundingBox.Max.Z;
      pDst[23].Color = color;

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const BoundingBox& boundingBox, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(24u);
      auto* pDst = m_vertices.data() + m_entries;

      // Unrolled as much as possible to ensure as few temporary writes as possible
      Vector3::Transform(Vector3(boundingBox.Min.X, boundingBox.Min.Y, boundingBox.Min.Z), matrix, pDst[0].Position);
      pDst[0].Color = color;
      Vector3::Transform(Vector3(boundingBox.Max.X, boundingBox.Min.Y, boundingBox.Min.Z), matrix, pDst[1].Position);
      pDst[1].Color = color;
      Vector3::Transform(Vector3(boundingBox.Min.X, boundingBox.Max.Y, boundingBox.Min.Z), matrix, pDst[2].Position);
      pDst[2].Color = color;
      Vector3::Transform(Vector3(boundingBox.Max.X, boundingBox.Max.Y, boundingBox.Min.Z), matrix, pDst[3].Position);
      pDst[3].Color = color;
      Vector3::Transform(Vector3(boundingBox.Min.X, boundingBox.Min.Y, boundingBox.Max.Z), matrix, pDst[4].Position);
      pDst[4].Color = color;
      Vector3::Transform(Vector3(boundingBox.Max.X, boundingBox.Min.Y, boundingBox.Max.Z), matrix, pDst[5].Position);
      pDst[5].Color = color;
      Vector3::Transform(Vector3(boundingBox.Min.X, boundingBox.Max.Y, boundingBox.Max.Z), matrix, pDst[6].Position);
      pDst[6].Color = color;
      Vector3::Transform(Vector3(boundingBox.Max.X, boundingBox.Max.Y, boundingBox.Max.Z), matrix, pDst[7].Position);
      pDst[7].Color = color;

      pDst[8].Position = pDst[0].Position;
      pDst[8].Color = color;
      pDst[9].Position = pDst[4].Position;
      pDst[9].Color = color;
      pDst[10].Position = pDst[1].Position;
      pDst[10].Color = color;
      pDst[11].Position = pDst[5].Position;
      pDst[11].Color = color;
      pDst[12].Position = pDst[2].Position;
      pDst[12].Color = color;
      pDst[13].Position = pDst[6].Position;
      pDst[13].Color = color;
      pDst[14].Position = pDst[3].Position;
      pDst[14].Color = color;
      pDst[15].Position = pDst[7].Position;
      pDst[15].Color = color;

      pDst[16].Position = pDst[0].Position;
      pDst[16].Color = color;
      pDst[17].Position = pDst[2].Position;
      pDst[17].Color = color;
      pDst[18].Position = pDst[4].Position;
      pDst[18].Color = color;
      pDst[19].Position = pDst[6].Position;
      pDst[19].Color = color;
      pDst[20].Position = pDst[1].Position;
      pDst[20].Color = color;
      pDst[21].Position = pDst[3].Position;
      pDst[21].Color = color;
      pDst[22].Position = pDst[5].Position;
      pDst[22].Color = color;
      pDst[23].Position = pDst[7].Position;
      pDst[23].Color = color;

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }


    void LineBuilder::Add(const BoundingFrustum& boundingFrustum, const Vector4& color)
    {
      EnsureCapacityFor(24u);

      boundingFrustum.GetCorners(m_cornersScratchpad);
      Fill(m_vertices, m_entries, m_cornersScratchpad, color);

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const BoundingFrustum& boundingFrustum, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(24u);

      boundingFrustum.GetCorners(m_cornersScratchpad);

      Vector3::Transform(m_cornersScratchpad[0], matrix, m_cornersScratchpad[0]);
      Vector3::Transform(m_cornersScratchpad[1], matrix, m_cornersScratchpad[1]);
      Vector3::Transform(m_cornersScratchpad[2], matrix, m_cornersScratchpad[2]);
      Vector3::Transform(m_cornersScratchpad[3], matrix, m_cornersScratchpad[3]);
      Vector3::Transform(m_cornersScratchpad[4], matrix, m_cornersScratchpad[4]);
      Vector3::Transform(m_cornersScratchpad[5], matrix, m_cornersScratchpad[5]);
      Vector3::Transform(m_cornersScratchpad[6], matrix, m_cornersScratchpad[6]);
      Vector3::Transform(m_cornersScratchpad[7], matrix, m_cornersScratchpad[7]);

      Fill(m_vertices, m_entries, m_cornersScratchpad, color);
      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }


    void LineBuilder::Add(const BoxF& value, const Vector4& color)
    {
      EnsureCapacityFor(24u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      pDst[0].Position.X = value.X1;
      pDst[0].Position.Y = value.Y1;
      pDst[0].Position.Z = 0.0f;
      pDst[0].Color = color;
      pDst[1].Position.X = value.X2;
      pDst[1].Position.Y = value.Y1;
      pDst[1].Position.Z = 0.0f;
      pDst[1].Color = color;

      pDst[2].Position.X = value.X1;
      pDst[2].Position.Y = value.Y2;
      pDst[2].Position.Z = 0.0f;
      pDst[2].Color = color;
      pDst[3].Position.X = value.X2;
      pDst[3].Position.Y = value.Y2;
      pDst[3].Position.Z = 0.0f;
      pDst[3].Color = color;

      pDst[4].Position.X = value.X1;
      pDst[4].Position.Y = value.Y1;
      pDst[4].Position.Z = 0.0f;
      pDst[4].Color = color;
      pDst[5].Position.X = value.X2;
      pDst[5].Position.Y = value.Y1;
      pDst[5].Position.Z = 0.0f;
      pDst[5].Color = color;

      pDst[6].Position.X = value.X1;
      pDst[6].Position.Y = value.Y2;
      pDst[6].Position.Z = 0.0f;
      pDst[6].Color = color;
      pDst[7].Position.X = value.X2;
      pDst[7].Position.Y = value.Y2;
      pDst[7].Position.Z = 0.0f;
      pDst[7].Color = color;

      pDst[8].Position.X = value.X1;
      pDst[8].Position.Y = value.Y1;
      pDst[8].Position.Z = 0.0f;
      pDst[8].Color = color;
      pDst[9].Position.X = value.X1;
      pDst[9].Position.Y = value.Y1;
      pDst[9].Position.Z = 0.0f;
      pDst[9].Color = color;

      pDst[10].Position.X = value.X2;
      pDst[10].Position.Y = value.Y1;
      pDst[10].Position.Z = 0.0f;
      pDst[10].Color = color;
      pDst[11].Position.X = value.X2;
      pDst[11].Position.Y = value.Y1;
      pDst[11].Position.Z = 0.0f;
      pDst[11].Color = color;

      pDst[12].Position.X = value.X1;
      pDst[12].Position.Y = value.Y2;
      pDst[12].Position.Z = 0.0f;
      pDst[12].Color = color;
      pDst[13].Position.X = value.X1;
      pDst[13].Position.Y = value.Y2;
      pDst[13].Position.Z = 0.0f;
      pDst[13].Color = color;

      pDst[14].Position.X = value.X2;
      pDst[14].Position.Y = value.Y2;
      pDst[14].Position.Z = 0.0f;
      pDst[14].Color = color;
      pDst[15].Position.X = value.X2;
      pDst[15].Position.Y = value.Y2;
      pDst[15].Position.Z = 0.0f;
      pDst[15].Color = color;

      pDst[16].Position.X = value.X1;
      pDst[16].Position.Y = value.Y1;
      pDst[16].Position.Z = 0.0f;
      pDst[16].Color = color;
      pDst[17].Position.X = value.X1;
      pDst[17].Position.Y = value.Y2;
      pDst[17].Position.Z = 0.0f;
      pDst[17].Color = color;

      pDst[18].Position.X = value.X1;
      pDst[18].Position.Y = value.Y1;
      pDst[18].Position.Z = 0.0f;
      pDst[18].Color = color;
      pDst[19].Position.X = value.X1;
      pDst[19].Position.Y = value.Y2;
      pDst[19].Position.Z = 0.0f;
      pDst[19].Color = color;

      pDst[20].Position.X = value.X2;
      pDst[20].Position.Y = value.Y1;
      pDst[20].Position.Z = 0.0f;
      pDst[20].Color = color;
      pDst[21].Position.X = value.X2;
      pDst[21].Position.Y = value.Y2;
      pDst[21].Position.Z = 0.0f;
      pDst[21].Color = color;

      pDst[22].Position.X = value.X2;
      pDst[22].Position.Y = value.Y1;
      pDst[22].Position.Z = 0.0f;
      pDst[22].Color = color;
      pDst[23].Position.X = value.X2;
      pDst[23].Position.Y = value.Y2;
      pDst[23].Position.Z = 0.0f;
      pDst[23].Color = color;

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const BoxF& value, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(24u);
      auto* pDst = m_vertices.data() + m_entries;

      // Unrolled as much as possible to ensure as few temporary writes as possible
      Vector3::Transform(Vector3(value.X1, value.Y1, 0.0f), matrix, pDst[0].Position);
      pDst[0].Color = color;
      Vector3::Transform(Vector3(value.X2, value.Y1, 0.0f), matrix, pDst[1].Position);
      pDst[1].Color = color;
      Vector3::Transform(Vector3(value.X1, value.Y2, 0.0f), matrix, pDst[2].Position);
      pDst[2].Color = color;
      Vector3::Transform(Vector3(value.X2, value.Y2, 0.0f), matrix, pDst[3].Position);
      pDst[3].Color = color;
      Vector3::Transform(Vector3(value.X1, value.Y1, 0.0f), matrix, pDst[4].Position);
      pDst[4].Color = color;
      Vector3::Transform(Vector3(value.X2, value.Y1, 0.0f), matrix, pDst[5].Position);
      pDst[5].Color = color;
      Vector3::Transform(Vector3(value.X1, value.Y2, 0.0f), matrix, pDst[6].Position);
      pDst[6].Color = color;
      Vector3::Transform(Vector3(value.X2, value.Y2, 0.0f), matrix, pDst[7].Position);
      pDst[7].Color = color;

      pDst[8].Position = pDst[0].Position;
      pDst[8].Color = color;
      pDst[9].Position = pDst[4].Position;
      pDst[9].Color = color;
      pDst[10].Position = pDst[1].Position;
      pDst[10].Color = color;
      pDst[11].Position = pDst[5].Position;
      pDst[11].Color = color;
      pDst[12].Position = pDst[2].Position;
      pDst[12].Color = color;
      pDst[13].Position = pDst[6].Position;
      pDst[13].Color = color;
      pDst[14].Position = pDst[3].Position;
      pDst[14].Color = color;
      pDst[15].Position = pDst[7].Position;
      pDst[15].Color = color;

      pDst[16].Position = pDst[0].Position;
      pDst[16].Color = color;
      pDst[17].Position = pDst[2].Position;
      pDst[17].Color = color;
      pDst[18].Position = pDst[4].Position;
      pDst[18].Color = color;
      pDst[19].Position = pDst[6].Position;
      pDst[19].Color = color;
      pDst[20].Position = pDst[1].Position;
      pDst[20].Color = color;
      pDst[21].Position = pDst[3].Position;
      pDst[21].Color = color;
      pDst[22].Position = pDst[5].Position;
      pDst[22].Color = color;
      pDst[23].Position = pDst[7].Position;
      pDst[23].Color = color;

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }


    void LineBuilder::Add(const Ray& value, const Vector4& color)
    {
      vertex_type vertices[2];
      vertices[0].Position = value.Position;
      vertices[0].Color = color;
      // Add directly into Position
      Vector3::Add(value.Position, Vector3::Normalize(value.Direction) * RAY_LENGTH, vertices[1].Position);
      vertices[1].Color = color;

      AddLines(vertices, 2);
    }


    void LineBuilder::Add(const Ray& value, const Vector4& color, const Matrix& matrix)
    {
      vertex_type vertices[2];
      Vector3::Transform(value.Position, matrix, vertices[0].Position);
      vertices[0].Color = color;
      Vector3::Transform((value.Position + (Vector3::Normalize(value.Direction) * RAY_LENGTH)), matrix, vertices[1].Position);
      vertices[1].Color = color;

      AddLines(vertices, 2);
    }

    void LineBuilder::Add(const Rect& value, const Vector4& color)
    {
      EnsureCapacityFor(8u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      // Line 0
      pDst[0].Position.X = value.Left();
      pDst[0].Position.Y = value.Top();
      pDst[0].Position.Z = 0.0f;
      pDst[0].Color = color;
      pDst[1].Position.X = value.Right();
      pDst[1].Position.Y = value.Top();
      pDst[1].Position.Z = 0.0f;
      pDst[1].Color = color;

      // Line 1
      pDst[2].Position.X = value.Right();
      pDst[2].Position.Y = value.Top();
      pDst[2].Position.Z = 0.0f;
      pDst[2].Color = color;
      pDst[3].Position.X = value.Right();
      pDst[3].Position.Y = value.Bottom();
      pDst[3].Position.Z = 0.0f;
      pDst[3].Color = color;

      // Line 2
      pDst[4].Position.X = value.Right();
      pDst[4].Position.Y = value.Bottom();
      pDst[4].Position.Z = 0.0f;
      pDst[4].Color = color;
      pDst[5].Position.X = value.Left();
      pDst[5].Position.Y = value.Bottom();
      pDst[5].Position.Z = 0.0f;
      pDst[5].Color = color;

      // Line 3
      pDst[6].Position.X = value.Left();
      pDst[6].Position.Y = value.Bottom();
      pDst[6].Position.Z = 0.0f;
      pDst[6].Color = color;
      pDst[7].Position.X = value.Left();
      pDst[7].Position.Y = value.Top();
      pDst[7].Position.Z = 0.0f;
      pDst[7].Color = color;

      m_entries += 8u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rect& value, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(8u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      // Line 0
      Vector3::Transform(Vector3(value.Left(), value.Top(), 0), matrix, pDst[0].Position);
      pDst[0].Color = color;

      Vector3::Transform(Vector3(value.Right(), value.Top(), 0), matrix, pDst[1].Position);
      pDst[1].Color = color;

      // Line 1
      pDst[2].Position = pDst[1].Position;
      pDst[2].Color = color;
      Vector3::Transform(Vector3(value.Right(), value.Bottom(), 0), matrix, pDst[3].Position);
      pDst[3].Color = color;

      // Line 2
      pDst[4].Position = pDst[3].Position;
      pDst[4].Color = color;
      Vector3::Transform(Vector3(value.Left(), value.Bottom(), 0), matrix, pDst[5].Position);
      pDst[5].Color = color;

      // Line 3
      pDst[6].Position = pDst[5].Position;
      pDst[6].Color = color;
      pDst[7].Position = pDst[0].Position;
      pDst[7].Color = color;

      m_entries += 8u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rectangle& value, const Vector4& color)
    {
      EnsureCapacityFor(8u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      // Line 0
      pDst[0].Position.X = static_cast<float>(value.Left());
      pDst[0].Position.Y = static_cast<float>(value.Top());
      pDst[0].Position.Z = 0.0f;
      pDst[0].Color = color;

      pDst[1].Position.X = static_cast<float>(value.Right());
      pDst[1].Position.Y = static_cast<float>(value.Top());
      pDst[1].Position.Z = 0.0f;
      pDst[1].Color = color;

      // Line 1
      pDst[2].Position.X = static_cast<float>(value.Right());
      pDst[2].Position.Y = static_cast<float>(value.Top());
      pDst[2].Position.Z = 0.0f;
      pDst[2].Color = color;

      pDst[3].Position.X = static_cast<float>(value.Right());
      pDst[3].Position.Y = static_cast<float>(value.Bottom());
      pDst[3].Position.Z = 0.0f;
      pDst[3].Color = color;

      // Line 2
      pDst[4].Position.X = static_cast<float>(value.Right());
      pDst[4].Position.Y = static_cast<float>(value.Bottom());
      pDst[4].Position.Z = 0.0f;
      pDst[4].Color = color;

      pDst[5].Position.X = static_cast<float>(value.Left());
      pDst[5].Position.Y = static_cast<float>(value.Bottom());
      pDst[5].Position.Z = 0.0f;
      pDst[5].Color = color;

      // Line 3
      pDst[6].Position.X = static_cast<float>(value.Left());
      pDst[6].Position.Y = static_cast<float>(value.Bottom());
      pDst[6].Position.Z = 0.0f;
      pDst[6].Color = color;

      pDst[7].Position.X = static_cast<float>(value.Left());
      pDst[7].Position.Y = static_cast<float>(value.Top());
      pDst[7].Position.Z = 0.0f;
      pDst[7].Color = color;

      m_entries += 8u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rectangle& value, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(8u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      // Line 0
      Vector3::Transform(Vector3(static_cast<float>(value.Left()), static_cast<float>(value.Top()), 0.0f), matrix, pDst[0].Position);
      pDst[0].Color = color;

      Vector3::Transform(Vector3(static_cast<float>(value.Right()), static_cast<float>(value.Top()), 0.0f), matrix, pDst[1].Position);
      pDst[1].Color = color;

      // Line 1
      pDst[2].Position = pDst[1].Position;
      pDst[2].Color = color;
      Vector3::Transform(Vector3(static_cast<float>(value.Right()), static_cast<float>(value.Bottom()), 0.0f), matrix, pDst[3].Position);
      pDst[3].Color = color;

      // Line 2
      pDst[4].Position = pDst[3].Position;
      pDst[4].Color = color;
      Vector3::Transform(Vector3(static_cast<float>(value.Left()), static_cast<float>(value.Bottom()), 0.0f), matrix, pDst[5].Position);
      pDst[5].Color = color;

      // Line 3
      pDst[6].Position = pDst[5].Position;
      pDst[6].Color = color;
      pDst[7].Position = pDst[0].Position;
      pDst[7].Color = color;

      m_entries += 8u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rectangle2D& value, const Vector4& color)
    {
      EnsureCapacityFor(8u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      // Line 0
      pDst[0].Position.X = static_cast<float>(value.Left());
      pDst[0].Position.Y = static_cast<float>(value.Top());
      pDst[0].Position.Z = 0.0f;
      pDst[0].Color = color;

      pDst[1].Position.X = static_cast<float>(value.Right());
      pDst[1].Position.Y = static_cast<float>(value.Top());
      pDst[1].Position.Z = 0.0f;
      pDst[1].Color = color;

      // Line 1
      pDst[2].Position.X = static_cast<float>(value.Right());
      pDst[2].Position.Y = static_cast<float>(value.Top());
      pDst[2].Position.Z = 0.0f;
      pDst[2].Color = color;

      pDst[3].Position.X = static_cast<float>(value.Right());
      pDst[3].Position.Y = static_cast<float>(value.Bottom());
      pDst[3].Position.Z = 0.0f;
      pDst[3].Color = color;

      // Line 2
      pDst[4].Position.X = static_cast<float>(value.Right());
      pDst[4].Position.Y = static_cast<float>(value.Bottom());
      pDst[4].Position.Z = 0.0f;
      pDst[4].Color = color;

      pDst[5].Position.X = static_cast<float>(value.Left());
      pDst[5].Position.Y = static_cast<float>(value.Bottom());
      pDst[5].Position.Z = 0.0f;
      pDst[5].Color = color;

      // Line 3
      pDst[6].Position.X = static_cast<float>(value.Left());
      pDst[6].Position.Y = static_cast<float>(value.Bottom());
      pDst[6].Position.Z = 0.0f;
      pDst[6].Color = color;

      pDst[7].Position.X = static_cast<float>(value.Left());
      pDst[7].Position.Y = static_cast<float>(value.Top());
      pDst[7].Position.Z = 0.0f;
      pDst[7].Color = color;

      m_entries += 8u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rectangle2D& value, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(8u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      // Line 0
      Vector3::Transform(Vector3(static_cast<float>(value.Left()), static_cast<float>(value.Top()), 0.0f), matrix, pDst[0].Position);
      pDst[0].Color = color;
      Vector3::Transform(Vector3(static_cast<float>(value.Right()), static_cast<float>(value.Top()), 0.0f), matrix, pDst[1].Position);
      pDst[1].Color = color;

      // Line 1
      pDst[2].Position = pDst[1].Position;
      pDst[2].Color = color;
      Vector3::Transform(Vector3(static_cast<float>(value.Right()), static_cast<float>(value.Bottom()), 0.0f), matrix, pDst[3].Position);
      pDst[3].Color = color;

      // Line 2
      pDst[4].Position = pDst[3].Position;
      pDst[4].Color = color;
      Vector3::Transform(Vector3(static_cast<float>(value.Left()), static_cast<float>(value.Bottom()), 0.0f), matrix, pDst[5].Position);
      pDst[5].Color = color;

      // Line 3
      pDst[6].Position = pDst[5].Position;
      pDst[6].Color = color;
      pDst[7].Position = pDst[0].Position;
      pDst[7].Color = color;

      m_entries += 8u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rectangle3D& value, const Vector4& color)
    {
      EnsureCapacityFor(24u);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes

      const auto left = static_cast<float>(value.Left());
      const auto right = static_cast<float>(value.Right());
      const auto top = static_cast<float>(value.Top());
      const auto bottom = static_cast<float>(value.Bottom());
      const auto front = static_cast<float>(value.Front());
      const auto back = static_cast<float>(value.Back());

      pDst[0].Position.X = left;
      pDst[0].Position.Y = bottom;
      pDst[0].Position.Z = back;
      pDst[0].Color = color;
      pDst[1].Position.X = right;
      pDst[1].Position.Y = bottom;
      pDst[1].Position.Z = back;
      pDst[1].Color = color;

      pDst[2].Position.X = left;
      pDst[2].Position.Y = top;
      pDst[2].Position.Z = back;
      pDst[2].Color = color;
      pDst[3].Position.X = right;
      pDst[3].Position.Y = top;
      pDst[3].Position.Z = back;
      pDst[3].Color = color;

      pDst[4].Position.X = left;
      pDst[4].Position.Y = bottom;
      pDst[4].Position.Z = front;
      pDst[4].Color = color;
      pDst[5].Position.X = right;
      pDst[5].Position.Y = bottom;
      pDst[5].Position.Z = front;
      pDst[5].Color = color;

      pDst[6].Position.X = left;
      pDst[6].Position.Y = top;
      pDst[6].Position.Z = front;
      pDst[6].Color = color;
      pDst[7].Position.X = right;
      pDst[7].Position.Y = top;
      pDst[7].Position.Z = front;
      pDst[7].Color = color;

      pDst[8].Position.X = left;
      pDst[8].Position.Y = bottom;
      pDst[8].Position.Z = back;
      pDst[8].Color = color;
      pDst[9].Position.X = left;
      pDst[9].Position.Y = bottom;
      pDst[9].Position.Z = front;
      pDst[9].Color = color;

      pDst[10].Position.X = right;
      pDst[10].Position.Y = bottom;
      pDst[10].Position.Z = back;
      pDst[10].Color = color;
      pDst[11].Position.X = right;
      pDst[11].Position.Y = bottom;
      pDst[11].Position.Z = front;
      pDst[11].Color = color;

      pDst[12].Position.X = left;
      pDst[12].Position.Y = top;
      pDst[12].Position.Z = back;
      pDst[12].Color = color;
      pDst[13].Position.X = left;
      pDst[13].Position.Y = top;
      pDst[13].Position.Z = front;
      pDst[13].Color = color;

      pDst[14].Position.X = right;
      pDst[14].Position.Y = top;
      pDst[14].Position.Z = back;
      pDst[14].Color = color;
      pDst[15].Position.X = right;
      pDst[15].Position.Y = top;
      pDst[15].Position.Z = front;
      pDst[15].Color = color;

      pDst[16].Position.X = left;
      pDst[16].Position.Y = bottom;
      pDst[16].Position.Z = back;
      pDst[16].Color = color;
      pDst[17].Position.X = left;
      pDst[17].Position.Y = top;
      pDst[17].Position.Z = back;
      pDst[17].Color = color;

      pDst[18].Position.X = left;
      pDst[18].Position.Y = bottom;
      pDst[18].Position.Z = front;
      pDst[18].Color = color;
      pDst[19].Position.X = left;
      pDst[19].Position.Y = top;
      pDst[19].Position.Z = front;
      pDst[19].Color = color;

      pDst[20].Position.X = right;
      pDst[20].Position.Y = bottom;
      pDst[20].Position.Z = back;
      pDst[20].Color = color;
      pDst[21].Position.X = right;
      pDst[21].Position.Y = top;
      pDst[21].Position.Z = back;
      pDst[21].Color = color;

      pDst[22].Position.X = right;
      pDst[22].Position.Y = bottom;
      pDst[22].Position.Z = front;
      pDst[22].Color = color;
      pDst[23].Position.X = right;
      pDst[23].Position.Y = top;
      pDst[23].Position.Z = front;
      pDst[23].Color = color;

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::Add(const Rectangle3D& value, const Vector4& color, const Matrix& matrix)
    {
      EnsureCapacityFor(24u);
      auto* pDst = m_vertices.data() + m_entries;

      const auto left = static_cast<float>(value.Left());
      const auto right = static_cast<float>(value.Right());
      const auto top = static_cast<float>(value.Top());
      const auto bottom = static_cast<float>(value.Bottom());
      const auto front = static_cast<float>(value.Front());
      const auto back = static_cast<float>(value.Back());

      // Unrolled as much as possible to ensure as few temporary writes as possible
      Vector3::Transform(Vector3(left, bottom, back), matrix, pDst[0].Position);
      pDst[0].Color = color;
      Vector3::Transform(Vector3(right, bottom, back), matrix, pDst[1].Position);
      pDst[1].Color = color;
      Vector3::Transform(Vector3(left, top, back), matrix, pDst[2].Position);
      pDst[2].Color = color;
      Vector3::Transform(Vector3(right, top, back), matrix, pDst[3].Position);
      pDst[3].Color = color;
      Vector3::Transform(Vector3(left, bottom, front), matrix, pDst[4].Position);
      pDst[4].Color = color;
      Vector3::Transform(Vector3(right, bottom, front), matrix, pDst[5].Position);
      pDst[5].Color = color;
      Vector3::Transform(Vector3(left, top, front), matrix, pDst[6].Position);
      pDst[6].Color = color;
      Vector3::Transform(Vector3(right, top, front), matrix, pDst[7].Position);
      pDst[7].Color = color;

      pDst[8].Position = pDst[0].Position;
      pDst[8].Color = color;
      pDst[9].Position = pDst[4].Position;
      pDst[9].Color = color;
      pDst[10].Position = pDst[1].Position;
      pDst[10].Color = color;
      pDst[11].Position = pDst[5].Position;
      pDst[11].Color = color;
      pDst[12].Position = pDst[2].Position;
      pDst[12].Color = color;
      pDst[13].Position = pDst[6].Position;
      pDst[13].Color = color;
      pDst[14].Position = pDst[3].Position;
      pDst[14].Color = color;
      pDst[15].Position = pDst[7].Position;
      pDst[15].Color = color;

      pDst[16].Position = pDst[0].Position;
      pDst[16].Color = color;
      pDst[17].Position = pDst[2].Position;
      pDst[17].Color = color;
      pDst[18].Position = pDst[4].Position;
      pDst[18].Color = color;
      pDst[19].Position = pDst[6].Position;
      pDst[19].Color = color;
      pDst[20].Position = pDst[1].Position;
      pDst[20].Color = color;
      pDst[21].Position = pDst[3].Position;
      pDst[21].Color = color;
      pDst[22].Position = pDst[5].Position;
      pDst[22].Color = color;
      pDst[23].Position = pDst[7].Position;
      pDst[23].Color = color;

      m_entries += 24u;
      assert(m_entries <= m_vertices.size());
    }


    void LineBuilder::AddAxis(const Vector3& position, const float axisLength)
    {
      EnsureCapacityFor(6u);
      auto* pDst = m_vertices.data() + m_entries;

      constexpr Vector4 colorRed = Color::Red().ToVector4();
      constexpr Vector4 colorGreen = Color::Green().ToVector4();
      constexpr Vector4 colorBlue = Color::Blue().ToVector4();

      // Line 0 - X-Axis
      pDst[0].Position = position;
      pDst[0].Color = colorRed;
      pDst[1].Position.X = position.X + axisLength;
      pDst[1].Position.Y = position.Y;
      pDst[1].Position.Z = position.Z;
      pDst[1].Color = colorRed;

      // Line 1 - Y-Axis
      pDst[2].Position = position;
      pDst[2].Color = colorGreen;
      pDst[3].Position.X = position.X;
      pDst[3].Position.Y = position.Y + axisLength;
      pDst[3].Position.Z = position.Z;
      pDst[3].Color = colorGreen;

      // Line 2 - Z-Axis
      pDst[4].Position = position;
      pDst[4].Color = colorBlue;
      pDst[5].Position.X = position.X;
      pDst[5].Position.Y = position.Y;
      pDst[5].Position.Z = position.Z + axisLength;
      pDst[5].Color = colorBlue;

      m_entries += 6u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::AddAxis(const Vector3& position, const float axisLength, const Matrix& matrix)
    {
      EnsureCapacityFor(6u);
      auto* pDst = m_vertices.data() + m_entries;

      constexpr Vector4 colorRed = Color::Red().ToVector4();
      constexpr Vector4 colorGreen = Color::Green().ToVector4();
      constexpr Vector4 colorBlue = Color::Blue().ToVector4();

      // Line 0 - X-Axis
      Vector3::Transform(position, matrix, pDst[0].Position);
      pDst[0].Color = colorRed;
      Vector3::Transform(Vector3(position.X + axisLength, position.Y, position.Z), matrix, pDst[1].Position);
      pDst[1].Color = colorRed;

      // Line 1 - Y-Axis
      pDst[2].Position = pDst[0].Position;
      pDst[2].Color = colorGreen;
      Vector3::Transform(Vector3(position.X, position.Y + axisLength, position.Z), matrix, pDst[3].Position);
      pDst[3].Color = colorGreen;

      // Line 2 - Z-Axis
      pDst[4].Position = pDst[0].Position;
      pDst[4].Color = colorBlue;
      Vector3::Transform(Vector3(position.X, position.Y, position.Z + axisLength), matrix, pDst[5].Position);
      pDst[5].Color = colorBlue;

      m_entries += 6u;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::AddGridXZ(const Rect& rect, const float posY, const uint32_t steps, const Vector4& color)
    {
      const auto numVertices = VERTICES_PER_LINE * 2u * steps;
      EnsureCapacityFor(numVertices);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      {
        const float xAdd = rect.Width() / (steps + 1);
        const float z1 = rect.Top();
        const float z2 = rect.Bottom();
        float x = rect.Left() + xAdd;

        for (uint32_t i = 0; i < steps; ++i)
        {
          pDst[0].Position.X = x;
          pDst[0].Position.Y = posY;
          pDst[0].Position.Z = z1;
          pDst[0].Color = color;
          pDst[1].Position.X = x;
          pDst[1].Position.Y = posY;
          pDst[1].Position.Z = z2;
          pDst[1].Color = color;
          pDst += 2;
          x += xAdd;
        }
      }
      {
        const float zAdd = rect.Height() / (steps + 1);
        const float x1 = rect.Left();
        const float x2 = rect.Right();
        float z = rect.Top() + zAdd;

        for (uint32_t i = 0; i < steps; ++i)
        {
          pDst[0].Position.X = x1;
          pDst[0].Position.Y = posY;
          pDst[0].Position.Z = z;
          pDst[0].Color = color;
          pDst[1].Position.X = x2;
          pDst[1].Position.Y = posY;
          pDst[1].Position.Z = z;
          pDst[1].Color = color;
          pDst += 2;
          z += zAdd;
        }
      }

      m_entries += numVertices;
      assert(m_entries <= m_vertices.size());
    }


    void LineBuilder::AddGridXZ(const Rect& rect, const float posY, const uint32_t steps, const Vector4& color, const Matrix& matrix)
    {
      const auto numVertices = VERTICES_PER_LINE * 2u * steps;
      EnsureCapacityFor(numVertices);
      auto* pDst = m_vertices.data() + m_entries;

      // Completely unrolled with direct sets to prevent unnecessary temporary writes
      {
        const float xAdd = rect.Width() / (steps + 1);
        const float z1 = rect.Top();
        const float z2 = rect.Bottom();
        float x = rect.Left() + xAdd;

        for (uint32_t i = 0; i < steps; ++i)
        {
          Vector3::Transform(Vector3(x, posY, z1), matrix, pDst[0].Position);
          pDst[0].Color = color;
          Vector3::Transform(Vector3(x, posY, z2), matrix, pDst[1].Position);
          pDst[1].Color = color;
          pDst += 2;
          x += xAdd;
        }
      }
      {
        const float zAdd = rect.Height() / (steps + 1);
        const float x1 = rect.Left();
        const float x2 = rect.Right();
        float z = rect.Top() + zAdd;

        for (uint32_t i = 0; i < steps; ++i)
        {
          Vector3::Transform(Vector3(x1, posY, z), matrix, pDst[0].Position);
          pDst[0].Color = color;
          Vector3::Transform(Vector3(x2, posY, z), matrix, pDst[1].Position);
          pDst[1].Color = color;
          pDst += 2;
          z += zAdd;
        }
      }

      m_entries += numVertices;
      assert(m_entries <= m_vertices.size());
    }


    void LineBuilder::AddLines(const VertexPositionColor* const pVertices, const std::size_t vertexCount)
    {
      if (pVertices == nullptr)
      {
        throw std::invalid_argument("pVertices can not be null");
      }
      if ((vertexCount % VERTICES_PER_LINE) != 0)
      {
        throw std::invalid_argument("the added lines did not contain the expected amount of vertices");
      }
      EnsureCapacityFor(vertexCount);

      {    // Add vertices
        const VertexPositionColor* pSrc = pVertices;
        const VertexPositionColor* const pSrcEnd = pVertices + vertexCount;

        assert(m_entries <= m_vertices.size());
        assert(vertexCount <= (m_vertices.size() - m_entries));

        auto* pDst = m_vertices.data() + m_entries;
        while (pSrc != pSrcEnd)
        {
          assert(pDst >= m_vertices.data());
          assert(pDst < (m_vertices.data() + m_vertices.size()));
          *pDst = *pSrc;
          ++pSrc;
          ++pDst;
        }
        m_entries += static_cast<uint32_t>(vertexCount);
        assert(m_entries <= m_vertices.size());
      }
    }

    void LineBuilder::AddLines(const VertexPositionColor* const pVertices, const std::size_t vertexCount, const Matrix& matrix)
    {
      if (pVertices == nullptr)
      {
        throw std::invalid_argument("pVertices can not be null");
      }
      if ((vertexCount % VERTICES_PER_LINE) != 0)
      {
        throw std::invalid_argument("the added lines did not contain the expected amount of vertices");
      }
      EnsureCapacityFor(vertexCount);

      {    // Add vertices
        const VertexPositionColor* pSrc = pVertices;
        const VertexPositionColor* const pSrcEnd = pVertices + vertexCount;

        assert(m_entries <= m_vertices.size());
        assert(vertexCount <= (m_vertices.size() - m_entries));

        auto* pDst = m_vertices.data() + m_entries;
        while (pSrc != pSrcEnd)
        {
          assert(pDst >= m_vertices.data());
          assert(pDst < (m_vertices.data() + m_vertices.size()));
          pDst->Position = Vector3::Transform(pSrc->Position, matrix);
          pDst->Color = pSrc->Color;
          ++pSrc;
          ++pDst;
        }
        m_entries += static_cast<uint32_t>(vertexCount);
        assert(m_entries <= m_vertices.size());
      }
    }
    void LineBuilder::AddSphere(const Vector3& center, const float radius, const Vector4& colYZ, const Vector4& colXZ, const Vector4& colXY,
                                const uint32_t steps)
    {
      const auto numVertices = VERTICES_PER_LINE * steps * 3;
      EnsureCapacityFor(numVertices);
      auto* pDst = m_vertices.data() + m_entries;

      float radiansAdd = MathHelper::RADS360 / steps;
      {
        float radians = 0.0f;
        float sinRadius = (radius * std::sin(radians));
        float cosRadius = (radius * std::cos(radians));
        for (uint32_t i = 0; i < steps; ++i)
        {
          pDst[(i * 2) + 0].Position.X = center.X + sinRadius;
          pDst[(i * 2) + 0].Position.Y = center.Y + cosRadius;
          pDst[(i * 2) + 0].Position.Z = center.Z;
          pDst[(i * 2) + 0].Color = colXY;
          radians += radiansAdd;
          sinRadius = (radius * std::sin(radians));
          cosRadius = (radius * std::cos(radians));
          pDst[(i * 2) + 1].Position.X = center.X + sinRadius;
          pDst[(i * 2) + 1].Position.Y = center.Y + cosRadius;
          pDst[(i * 2) + 1].Position.Z = center.Z;
          pDst[(i * 2) + 1].Color = colXY;
        }
      }
      const auto endStep = steps * 2;
      {
        float radians = 0.0f;
        float sinRadius = (radius * std::sin(radians));
        float cosRadius = (radius * std::cos(radians));
        for (uint32_t i = steps; i < endStep; ++i)
        {
          pDst[(i * 2) + 0].Position.X = center.X;
          pDst[(i * 2) + 0].Position.Y = center.Y + sinRadius;
          pDst[(i * 2) + 0].Position.Z = center.Z + cosRadius;
          pDst[(i * 2) + 0].Color = colYZ;
          radians += radiansAdd;
          sinRadius = (radius * std::sin(radians));
          cosRadius = (radius * std::cos(radians));
          pDst[(i * 2) + 1].Position.X = center.X;
          pDst[(i * 2) + 1].Position.Y = center.Y + sinRadius;
          pDst[(i * 2) + 1].Position.Z = center.Z + cosRadius;
          pDst[(i * 2) + 1].Color = colYZ;
        }
      }
      const auto endStep2 = steps * 3;
      {
        float radians = 0.0f;
        float sinRadius = (radius * std::sin(radians));
        float cosRadius = (radius * std::cos(radians));
        for (uint32_t i = endStep; i < endStep2; ++i)
        {
          pDst[(i * 2) + 0].Position.X = center.X + sinRadius;
          pDst[(i * 2) + 0].Position.Y = center.Y;
          pDst[(i * 2) + 0].Position.Z = center.Z + cosRadius;
          pDst[(i * 2) + 0].Color = colXZ;
          radians += radiansAdd;
          sinRadius = (radius * std::sin(radians));
          cosRadius = (radius * std::cos(radians));
          pDst[(i * 2) + 1].Position.X = center.X + sinRadius;
          pDst[(i * 2) + 1].Position.Y = center.Y;
          pDst[(i * 2) + 1].Position.Z = center.Z + cosRadius;
          pDst[(i * 2) + 1].Color = colXZ;
        }
      }
      m_entries += numVertices;
      assert(m_entries <= m_vertices.size());
    }

    void LineBuilder::AddSphere(const Vector3& center, const float radius, const Vector4& colYZ, const Vector4& colXZ, const Vector4& colXY,
                                const Matrix& matrix, const uint32_t steps)
    {
      const auto numVertices = VERTICES_PER_LINE * steps * 3;
      EnsureCapacityFor(numVertices);
      auto* pDst = m_vertices.data() + m_entries;

      float radiansAdd = MathHelper::RADS360 / steps;
      {
        float radians = 0.0f;
        float sinRadius = (radius * std::sin(radians));
        float cosRadius = (radius * std::cos(radians));
        for (uint32_t i = 0; i < steps; ++i)
        {
          Vector3::Transform(Vector3(center.X + sinRadius, center.Y + cosRadius, center.Z), matrix, pDst[(i * 2) + 0].Position);
          pDst[(i * 2) + 0].Color = colXY;
          radians += radiansAdd;
          sinRadius = (radius * std::sin(radians));
          cosRadius = (radius * std::cos(radians));
          Vector3::Transform(Vector3(center.X + sinRadius, center.Y + cosRadius, center.Z), matrix, pDst[(i * 2) + 1].Position);
          pDst[(i * 2) + 1].Color = colXY;
        }
      }
      const auto endStep = steps * 2;
      {
        float radians = 0.0f;
        float sinRadius = (radius * std::sin(radians));
        float cosRadius = (radius * std::cos(radians));
        for (uint32_t i = steps; i < endStep; ++i)
        {
          Vector3::Transform(Vector3(center.X, center.Y + sinRadius, center.Z + cosRadius), matrix, pDst[(i * 2) + 0].Position);
          pDst[(i * 2) + 0].Color = colYZ;
          radians += radiansAdd;
          sinRadius = (radius * std::sin(radians));
          cosRadius = (radius * std::cos(radians));
          Vector3::Transform(Vector3(center.X, center.Y + sinRadius, center.Z + cosRadius), matrix, pDst[(i * 2) + 1].Position);
          pDst[(i * 2) + 1].Color = colYZ;
        }
      }
      const auto endStep2 = steps * 3;
      {
        float radians = 0.0f;
        float sinRadius = (radius * std::sin(radians));
        float cosRadius = (radius * std::cos(radians));
        for (uint32_t i = endStep; i < endStep2; ++i)
        {
          Vector3::Transform(Vector3(center.X + sinRadius, center.Y, center.Z + cosRadius), matrix, pDst[(i * 2) + 0].Position);
          pDst[(i * 2) + 0].Color = colXZ;
          radians += radiansAdd;
          sinRadius = (radius * std::sin(radians));
          cosRadius = (radius * std::cos(radians));
          Vector3::Transform(Vector3(center.X + sinRadius, center.Y, center.Z + cosRadius), matrix, pDst[(i * 2) + 1].Position);
          pDst[(i * 2) + 1].Color = colXZ;
        }
      }
      m_entries += numVertices;
      assert(m_entries <= m_vertices.size());
    }
  }
}
