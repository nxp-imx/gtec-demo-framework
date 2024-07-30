/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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

#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestReadOnlyFlexVertexSpanUtilReadOnlySpan = TestFixtureFslBase;
}


TEST(TestReadOnlyFlexVertexSpanUtilReadOnlySpan, AsSpan)
{
  constexpr std::array<VertexPosition, 4> Vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  const auto srcVertices = SpanUtil::AsReadOnlySpan(Vertices);
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(srcVertices, vertexDeclaration);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), Vertices.data());
  EXPECT_EQ(span.size(), Vertices.size());
  EXPECT_EQ(span.length(), Vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}


TEST(TestReadOnlyFlexVertexSpanUtilReadOnlySpan, AsSpan_AutoVertexDecl)
{
  constexpr std::array<VertexPosition, 4> Vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  const auto srcVertices = SpanUtil::AsReadOnlySpan(Vertices);
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(srcVertices);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), Vertices.data());
  EXPECT_EQ(span.size(), Vertices.size());
  EXPECT_EQ(span.length(), Vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}


TEST(TestReadOnlyFlexVertexSpanUtilReadOnlySpan, AsSpan_NullVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> Vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  const auto srcVertices = SpanUtil::AsReadOnlySpan(Vertices);

  EXPECT_THROW(ReadOnlyFlexVertexSpanUtil::AsSpan(srcVertices, VertexDeclarationSpan()), std::invalid_argument);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpanUtilReadOnlySpan, AsSpan_NoCheck)
{
  constexpr std::array<VertexPosition, 4> Vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  const auto srcVertices = SpanUtil::AsReadOnlySpan(Vertices);
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(srcVertices, vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), Vertices.data());
  EXPECT_EQ(span.size(), Vertices.size());
  EXPECT_EQ(span.length(), Vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpanUtilReadOnlySpan, AsSpan_AutoVertexDecl_NoCheck)
{
  constexpr std::array<VertexPosition, 4> Vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  const auto srcVertices = SpanUtil::AsReadOnlySpan(Vertices);
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(srcVertices, OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), Vertices.data());
  EXPECT_EQ(span.size(), Vertices.size());
  EXPECT_EQ(span.length(), Vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}
