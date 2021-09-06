/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslGraphics/Vertices/VertexPositionColor.hpp>
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
  using TestVertices_VertexPositionColor = TestFixtureFslGraphics;
}


TEST(TestVertices_VertexPositionColor, Construct_Default)
{
  const VertexPositionColor vertex;

  EXPECT_EQ(Vector3(), vertex.Position);
  EXPECT_EQ(Color(), vertex.Color);
}


TEST(TestVertices_VertexPositionColor, Construct)
{
  const Vector3 pos(1.0f, 2.0f, 3.0f);
  const Color color(4u, 5u, 6u, 7u);
  const VertexPositionColor vertex(pos, color);

  EXPECT_EQ(pos, vertex.Position);
  EXPECT_EQ(color, vertex.Color);
}


TEST(TestVertices_VertexPositionColor, GetVertexDeclaration)
{
  const VertexElementEx expected0(offsetof(VertexPositionColor, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionColor, Color), VertexElementFormat::X8Y8Z8W8_UNORM, VertexElementUsage::Color, 0u);
  const auto vertexDecl = VertexDeclaration(VertexPositionColor::AsVertexDeclarationSpan());

  EXPECT_EQ(sizeof(VertexPositionColor), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColor, GetVertexDeclarationArray)
{
  const VertexElementEx expected0(offsetof(VertexPositionColor, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionColor, Color), VertexElementFormat::X8Y8Z8W8_UNORM, VertexElementUsage::Color, 0u);
  const auto vertexDecl = VertexPositionColor::GetVertexDeclarationArray();

  EXPECT_EQ(sizeof(VertexPositionColor), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColor, AsVertexDeclarationSpan)
{
  const VertexElementEx expected0(offsetof(VertexPositionColor, Position), VertexElementFormat::Vector3, VertexElementUsage::Position, 0u);
  const VertexElementEx expected1(offsetof(VertexPositionColor, Color), VertexElementFormat::X8Y8Z8W8_UNORM, VertexElementUsage::Color, 0u);
  const auto vertexDecl = VertexPositionColor::AsVertexDeclarationSpan();

  EXPECT_EQ(sizeof(VertexPositionColor), vertexDecl.VertexStride());
  ASSERT_EQ(2u, vertexDecl.Count());
  EXPECT_NE(nullptr, vertexDecl.DirectAccess());

  // Get by element usage
  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Position, 0u), 0);
  EXPECT_EQ(expected0, vertexDecl.VertexElementGet(VertexElementUsage::Position, 0u));

  EXPECT_EQ(vertexDecl.VertexElementGetIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(vertexDecl.VertexElementIndexOf(VertexElementUsage::Color, 0u), 1);
  EXPECT_EQ(expected1, vertexDecl.VertexElementGet(VertexElementUsage::Color, 0u));

  // By index
  EXPECT_EQ(expected0, vertexDecl.At(0u));
  EXPECT_EQ(expected1, vertexDecl.At(1u));

  // Direct access should produce the same as At
  for (uint32_t i = 0; i < vertexDecl.Count(); ++i)
  {
    EXPECT_EQ(vertexDecl.At(i), vertexDecl.DirectAccess()[i]);
  }
}


TEST(TestVertices_VertexPositionColor, Equal)
{
  const VertexPositionColor vertex1;
  const VertexPositionColor vertex2(Vector3(1.0f, 2.0f, 3.0f), Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  const VertexPositionColor vertex3(Vector3(1.0f, 2.0f, 3.0f), Vector4(0.4f, 0.5f, 0.6f, 0.7f));

  EXPECT_EQ(vertex1, vertex1);
  EXPECT_EQ(vertex2, vertex2);
  EXPECT_EQ(vertex3, vertex3);
}


TEST(TestVertices_VertexPositionColor, NotEqual)
{
  const VertexPositionColor vertex1;
  const VertexPositionColor vertex2(Vector3(1.0f, 2.0f, 3.0f), Vector4(0.1f, 0.2f, 0.3f, 0.4f));
  const VertexPositionColor vertex3(Vector3(1.0f, 2.0f, 3.0f), Vector4(0.4f, 0.5f, 0.6f, 0.7f));

  EXPECT_NE(vertex1, vertex2);
  EXPECT_NE(vertex1, vertex3);

  EXPECT_NE(vertex2, vertex1);
  EXPECT_NE(vertex2, vertex3);

  EXPECT_NE(vertex3, vertex1);
  EXPECT_NE(vertex3, vertex2);
}
