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
#include <FslGraphics/Vertices/VertexPositionTexture.hpp>
#include <cstddef>

using namespace Fsl;

namespace
{
  using TestVertices_VertexPositionTexture = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexPositionTexture, Construct_Default)
{
  VertexPositionTexture vertex;

  EXPECT_EQ(Vector3(), vertex.Position);
  EXPECT_EQ(Vector2(), vertex.TextureCoordinate);
}


TEST(TestVertices_VertexPositionTexture, GetVertexDeclaration)
{
  const VertexElement expected0(offsetof(VertexPositionTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElement expected1(offsetof(VertexPositionTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexDeclaration(VertexPositionTexture::AsVertexDeclarationSpan());

  EXPECT_EQ(sizeof(VertexPositionTexture), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}

TEST(TestVertices_VertexPositionTexture, GetVertexDeclarationArray)
{
  const VertexElement expected0(offsetof(VertexPositionTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElement expected1(offsetof(VertexPositionTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionTexture::GetVertexDeclarationArray();

  EXPECT_EQ(sizeof(VertexPositionTexture), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionTexture, AsVertexDeclarationSpan)
{
  const VertexElement expected0(offsetof(VertexPositionTexture, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElement expected1(offsetof(VertexPositionTexture, TextureCoordinate), VertexElementFormat::Vector2,
                                VertexElementUsage::TextureCoordinate, 0u);
  const auto vertexDecl = VertexPositionTexture::AsVertexDeclarationSpan();

  EXPECT_EQ(sizeof(VertexPositionTexture), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::TextureCoordinate, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::TextureCoordinate, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}
