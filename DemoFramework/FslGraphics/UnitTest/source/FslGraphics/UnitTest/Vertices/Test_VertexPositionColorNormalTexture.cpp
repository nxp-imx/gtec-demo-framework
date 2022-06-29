/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/Log/Math/LogVector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/VertexDeclaration.hpp>
#include <FslGraphics/Vertices/VertexPositionColorNormalTexture.hpp>
#include <cstddef>

using namespace Fsl;

namespace
{
  using TestVertices_VertexPositionColorNormalTexture = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexPositionColorNormalTexture, Construct_Default)
{
  VertexPositionColorNormalTexture vertex;

  EXPECT_EQ(Vector3(), vertex.Position);
  EXPECT_EQ(Vector4(), vertex.Color);
  EXPECT_EQ(Vector3(), vertex.Normal);
  EXPECT_EQ(Vector2(), vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionColorNormalTexture, Construct)
{
  const Vector3 pos(1.0f, 2.0f, 3.0f);
  const Color color(4u, 5u, 6u, 7u);
  const Vector3 normal(8.0f, 9.0f, 10.0f);
  const Vector2 texCoord(11.0f, 12.0f);
  const VertexPositionColorNormalTexture vertex(pos, color, normal, texCoord);

  EXPECT_EQ(pos, vertex.Position);
  EXPECT_EQ(color.ToVector4(), vertex.Color);
  EXPECT_EQ(normal, vertex.Normal);
  EXPECT_EQ(texCoord, vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionColorNormalTexture, GetVertexDeclaration)
{
  const VertexElement expected0(offsetof(VertexPositionColorNormalTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElement expected1(offsetof(VertexPositionColorNormalTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u);
  const VertexElement expected2(offsetof(VertexPositionColorNormalTexture, Normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0u);
  const VertexElement expected3(offsetof(VertexPositionColorNormalTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexDeclaration(VertexPositionColorNormalTexture::AsVertexDeclarationSpan());

  EXPECT_EQ(sizeof(VertexPositionColorNormalTexture), vertexDecl.VertexStride());
  ASSERT_EQ(4u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0u), 2);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Normal, 0u), 2);
  EXPECT_EQ(expected2, vertexDecl.VertexElementGet(VertexElementUsage::Normal, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 3);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 3);
  EXPECT_EQ(expected3, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));
  EXPECT_EQ(expected2, vertexDecl.At(2u));
  EXPECT_EQ(expected3, vertexDecl.At(3u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColorNormalTexture, GetVertexDeclarationArray)
{
  const VertexElement expected0(offsetof(VertexPositionColorNormalTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElement expected1(offsetof(VertexPositionColorNormalTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u);
  const VertexElement expected2(offsetof(VertexPositionColorNormalTexture, Normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0u);
  const VertexElement expected3(offsetof(VertexPositionColorNormalTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionColorNormalTexture::GetVertexDeclarationArray();

  EXPECT_EQ(sizeof(VertexPositionColorNormalTexture), vertexDecl.VertexStride());
  ASSERT_EQ(4u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0u), 2);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Normal, 0u), 2);
  EXPECT_EQ(expected2, vertexDecl.VertexElementGet(VertexElementUsage::Normal, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 3);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 3);
  EXPECT_EQ(expected3, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));
  EXPECT_EQ(expected2, vertexDecl.At(2u));
  EXPECT_EQ(expected3, vertexDecl.At(3u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColorNormalTexture, AsVertexDeclarationSpan)
{
  const VertexElement expected0(offsetof(VertexPositionColorNormalTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElement expected1(offsetof(VertexPositionColorNormalTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u);
  const VertexElement expected2(offsetof(VertexPositionColorNormalTexture, Normal), VertexElementFormat::Vector3, VertexElementUsage::Normal, 0u);
  const VertexElement expected3(offsetof(VertexPositionColorNormalTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionColorNormalTexture::AsVertexDeclarationSpan();

  EXPECT_EQ(sizeof(VertexPositionColorNormalTexture), vertexDecl.VertexStride());
  ASSERT_EQ(4u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Normal, 0u), 2);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Normal, 0u), 2);
  EXPECT_EQ(expected2, vertexDecl.VertexElementGet(VertexElementUsage::Normal, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 3);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 3);
  EXPECT_EQ(expected3, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));
  EXPECT_EQ(expected2, vertexDecl.At(2u));
  EXPECT_EQ(expected3, vertexDecl.At(3u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}
