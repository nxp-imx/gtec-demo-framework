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

#include <FslGraphics/Vertices/VertexPositionColorNormalTangentTexture.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <cstddef>

using namespace Fsl;

namespace
{
  using TestVertices_VertexPositionColorNormalTangentTexture = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexPositionColorNormalTangentTexture, Construct_Default)
{
  VertexPositionColorNormalTangentTexture vertex;

  EXPECT_EQ(Vector3(), vertex.Position);
  EXPECT_EQ(Vector4(), vertex.Color);
  EXPECT_EQ(Vector3(), vertex.Normal);
  EXPECT_EQ(Vector3(), vertex.Tangent);
  EXPECT_EQ(Vector2(), vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionColorNormalTangentTexture, Construct)
{
  const Vector3 pos(1.0f, 2.0f, 3.0f);
  const Color color(4u, 5u, 6u, 7u);
  const Vector3 normal(8.0f, 9.0f, 10.0f);
  const Vector3 tangent(11.0f, 12.0f, 13.0f);
  const Vector2 texCoord(14.0f, 15.0f);
  const VertexPositionColorNormalTangentTexture vertex(pos, color, normal, tangent, texCoord);

  EXPECT_EQ(pos, vertex.Position);
  EXPECT_EQ(color.ToVector4(), vertex.Color);
  EXPECT_EQ(normal, vertex.Normal);
  EXPECT_EQ(tangent, vertex.Tangent);
  EXPECT_EQ(texCoord, vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionColorNormalTangentTexture, GetVertexDeclaration)
{
  std::array<VertexElementEx, 5> expected = {
    VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u),
    VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u),
    VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0u),
    VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, Tangent), VertexElementFormat::Vector3, VertexElementUsage::Tangent, 0u),
    VertexElementEx(offsetof(VertexPositionColorNormalTangentTexture, TextureCoordinate), VertexElementFormat::Vector2,
                    VertexElementUsage::TextureCoordinate, 0u)};

  const auto vertexDecl = VertexPositionColorNormalTangentTexture::GetVertexDeclaration();

  EXPECT_EQ(sizeof(VertexPositionColorNormalTangentTexture), vertexDecl.VertexStride());
  ASSERT_EQ(expected.size(), vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  for (uint32_t i = 0; i < expected.size(); ++i)
  {
    EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(expected[i].Usage, expected[i].UsageIndex), static_cast<int32_t>(i)) << "index: " << i;
    EXPECT_EQ(vertexDecl.VertexElementIndexOf(expected[i].Usage, expected[i].UsageIndex), static_cast<int32_t>(i)) << "index: " << i;
    EXPECT_EQ(expected[i], vertexDecl.VertexElementGet(expected[i].Usage, expected[i].UsageIndex)) << "index: " << i;
    // by index
    EXPECT_EQ(expected[i], vertexDecl.At(i)) << "index: " << i;
    // Direct access should produce the same as At
    EXPECT_EQ(expected[i], vertexDecl.DirectAccess()[i]) << "index: " << i;
  }
}
