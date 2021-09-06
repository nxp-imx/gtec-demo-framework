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

#include <FslGraphics/Vertices/VertexPositionColorFTexture.hpp>
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
#include <cstddef>

using namespace Fsl;

namespace
{
  using TestVertices_VertexPositionColorFTexture = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexPositionColorFTexture, Construct_Default)
{
  const VertexPositionColorFTexture vertex;

  EXPECT_EQ(Vector3(), vertex.Position);
  EXPECT_EQ(Vector4(), vertex.Color);
  EXPECT_EQ(Vector2(), vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionColorFTexture, Construct)
{
  const Vector3 pos(1.0f, 2.0f, 3.0f);
  const Vector4 color(4u, 5u, 6u, 7u);
  const Vector2 texCoord(8.0f, 9.0f);
  const VertexPositionColorFTexture vertex(pos, color, texCoord);

  EXPECT_EQ(pos, vertex.Position);
  EXPECT_EQ(color, vertex.Color);
  EXPECT_EQ(texCoord, vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionColorFTexture, GetVertexDeclaration)
{
  const VertexElementEx expected0(offsetof(VertexPositionColorFTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionColorFTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u);
  const VertexElementEx expected2(offsetof(VertexPositionColorFTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                  VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexDeclaration(VertexPositionColorFTexture::AsVertexDeclarationSpan());

  EXPECT_EQ(sizeof(VertexPositionColorFTexture), vertexDecl.VertexStride());
  ASSERT_EQ(3u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 2);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 2);
  EXPECT_EQ(expected2, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));
  EXPECT_EQ(expected2, vertexDecl.At(2u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColorFTexture, GetVertexDeclarationArray)
{
  const VertexElementEx expected0(offsetof(VertexPositionColorFTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionColorFTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u);
  const VertexElementEx expected2(offsetof(VertexPositionColorFTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                  VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionColorFTexture::GetVertexDeclarationArray();

  EXPECT_EQ(sizeof(VertexPositionColorFTexture), vertexDecl.VertexStride());
  ASSERT_EQ(3u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 2);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 2);
  EXPECT_EQ(expected2, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));
  EXPECT_EQ(expected2, vertexDecl.At(2u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColorFTexture, AsVertexDeclarationSpan)
{
  const VertexElementEx expected0(offsetof(VertexPositionColorFTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionColorFTexture, Color), VertexElementFormat::Vector4, VertexElementUsage::Color, 0u);
  const VertexElementEx expected2(offsetof(VertexPositionColorFTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                  VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionColorFTexture::AsVertexDeclarationSpan();

  EXPECT_EQ(sizeof(VertexPositionColorFTexture), vertexDecl.VertexStride());
  ASSERT_EQ(3u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 2);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 2);
  EXPECT_EQ(expected2, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));
  EXPECT_EQ(expected2, vertexDecl.At(2u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}
