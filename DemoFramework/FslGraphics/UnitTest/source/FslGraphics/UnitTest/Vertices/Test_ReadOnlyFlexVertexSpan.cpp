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

#include <FslBase/Span/ReadOnlyFlexSpanUtil.hpp>
#include <FslBase/Span/ReadOnlyFlexSpanUtil_Array.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpan.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <FslGraphics/Vertices/VertexPositionColorF.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestReadOnlyFlexVertexSpan = TestFixtureFslBase;
}


TEST(TestReadOnlyFlexVertexSpan, Construct_Default)
{
  ReadOnlyFlexVertexSpan span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), 0u);
  EXPECT_TRUE(span.AsVertexDeclarationSpan().Empty());
}


TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(vertices.data(), vertices.size(), vertexDeclaration);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), vertices.data());
  EXPECT_EQ(span.size(), vertices.size());
  EXPECT_EQ(span.length(), vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(nullptr, vertices.size(), vertexDeclaration);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_LengthZero)
{
  // constexpr std::array<VertexPosition, 4> vertices = {
  //  VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
  //  VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
  //  VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
  //  VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  //};

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(nullptr, 0u, vertexDeclaration);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_NullVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const VertexDeclarationSpan vertexDeclaration;
  EXPECT_THROW(ReadOnlyFlexVertexSpan(nullptr, vertices.size(), vertexDeclaration), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_LengthZero_NullVertexDeclaration)
{
  // constexpr std::array<VertexPosition, 4> vertices = {
  //   VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
  //   VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
  //   VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
  //   VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  // };

  const VertexDeclarationSpan vertexDeclaration;
  EXPECT_THROW(ReadOnlyFlexVertexSpan(nullptr, 0u, vertexDeclaration), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  EXPECT_THROW(ReadOnlyFlexVertexSpan(vertices.data(), vertices.size(), VertexDeclarationSpan()), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpan, Construct_ReadOnlyFlexSpan)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), vertices.data());
  EXPECT_EQ(span.size(), vertices.size());
  EXPECT_EQ(span.length(), vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan)
{
  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexSpan flexSpan;
  EXPECT_THROW(ReadOnlyFlexVertexSpan(flexSpan, vertexDeclaration), std::invalid_argument);
}

TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan2)
{
  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexSpan flexSpan(nullptr, 0, vertexDeclaration.VertexStride());
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), flexSpan.data());
  EXPECT_EQ(span.size(), flexSpan.size());
  EXPECT_EQ(span.length(), flexSpan.size());
  EXPECT_EQ(span.stride(), flexSpan.stride());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}


TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan_EmptyVertexDecl)
{
  VertexDeclarationSpan vertexDeclaration;
  ReadOnlyFlexSpan flexSpan;
  EXPECT_THROW(ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan_EmptyVertexDecl2)
{
  VertexDeclarationSpan vertexDeclaration;
  ReadOnlyFlexSpan flexSpan(nullptr, 0, 1);
  EXPECT_THROW(ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpan, Construct_ReadOnlyFlexSpan_EmptyVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);
  VertexDeclarationSpan vertexDeclaration;

  EXPECT_THROW(ReadOnlyFlexVertexSpan(flexSpan, vertexDeclaration), std::invalid_argument);
}

TEST(TestReadOnlyFlexVertexSpan, Construct_ReadOnlyFlexSpan_IncompatibleVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);
  const VertexDeclarationSpan vertexDeclaration = VertexPositionColorF::AsVertexDeclarationSpan();

  EXPECT_THROW(ReadOnlyFlexVertexSpan(flexSpan, vertexDeclaration), std::invalid_argument);
}

// -----------------------------


TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NoCheck)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(vertices.data(), vertices.size(), vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), vertices.data());
  EXPECT_EQ(span.size(), vertices.size());
  EXPECT_EQ(span.length(), vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// Not valid with NoCheck
// TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_NoCheck)
//{
//  constexpr std::array<VertexPosition, 4> vertices = {
//    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
//    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
//    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
//    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
//  };
//
//  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
//  ReadOnlyFlexVertexSpan span(nullptr, vertices.size(), vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.size(), 0u);
//  EXPECT_EQ(span.length(), 0u);
//  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//}

TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_LengthZero_NoCheck)
{
  // constexpr std::array<VertexPosition, 4> vertices = {
  //   VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
  //   VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
  //   VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
  //   VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  // };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(nullptr, 0u, vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// Not valid with no check
// TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_NullVertexDeclaration_NoCheck)
//{
//  constexpr std::array<VertexPosition, 4> vertices = {
//    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
//    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
//    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
//    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
//  };
//
//  const VertexDeclarationSpan vertexDeclaration;
//  ReadOnlyFlexVertexSpan span(nullptr, vertices.size(), vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.size(), 0u);
//  EXPECT_EQ(span.length(), 0u);
//  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//}


// Not valid with no check
// TEST(TestReadOnlyFlexVertexSpan, Construct_PointerLength_NullPointer_LengthZero_NullVertexDeclaration_NoCheck)
//{
//  constexpr std::array<VertexPosition, 4> vertices = {
//    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
//    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
//    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
//    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
//  };
//
//  const VertexDeclarationSpan vertexDeclaration;
//  ReadOnlyFlexVertexSpan span(nullptr, 0u, vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.size(), 0u);
//  EXPECT_EQ(span.length(), 0u);
//  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
//}


TEST(TestReadOnlyFlexVertexSpan, Construct_ReadOnlyFlexSpan_NoCheck)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), vertices.data());
  EXPECT_EQ(span.size(), vertices.size());
  EXPECT_EQ(span.length(), vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// Not valid with no check
// TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan_NoCheck)
//{
//  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
//  ReadOnlyFlexSpan flexSpan;
//  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), flexSpan.data());
//  EXPECT_EQ(span.size(), flexSpan.size());
//  EXPECT_EQ(span.length(), flexSpan.size());
//  // In this special case where we have a empty flex-span and a valid vertex declaration we expect to get the stride from the valid vertex
//  declaration EXPECT_NE(span.stride(), flexSpan.stride()); EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
//}

TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan_NoCheck)
{
  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexSpan flexSpan(nullptr, 0, vertexDeclaration.VertexStride());
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), flexSpan.data());
  EXPECT_EQ(span.size(), flexSpan.size());
  EXPECT_EQ(span.length(), flexSpan.size());
  EXPECT_EQ(span.stride(), flexSpan.stride());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// Not valid with NO CHECK
// TEST(TestReadOnlyFlexVertexSpan, Construct_EmptyReadOnlyFlexSpan_EmptyVertexDecl_NoCheck)
//{
//  VertexDeclarationSpan vertexDeclaration;
//  ReadOnlyFlexSpan flexSpan;
//  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), flexSpan.data());
//  EXPECT_EQ(span.size(), flexSpan.size());
//  EXPECT_EQ(span.length(), flexSpan.size());
//  EXPECT_EQ(span.stride(), 0u);
//  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
//}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  ReadOnlyFlexVertexSpan res = span.subspan();

  EXPECT_FALSE(res.empty());
  EXPECT_NE(res.data(), nullptr);
  EXPECT_EQ(res.data(), vertices.data());
  EXPECT_EQ(res.size(), vertices.size());
  EXPECT_EQ(res.length(), vertices.size());
  EXPECT_EQ(res.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(res.AsVertexDeclarationSpan(), vertexDeclaration);
}


// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan_Offset1)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  ReadOnlyFlexVertexSpan res = span.subspan(1);

  EXPECT_FALSE(res.empty());
  EXPECT_EQ(res.data(), &vertices[1]);
  EXPECT_EQ(res.size(), vertices.size() - 1u);
  EXPECT_EQ(res.length(), vertices.size() - 1u);
  EXPECT_EQ(res.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(res.AsVertexDeclarationSpan(), vertexDeclaration);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan_Offset3)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  ReadOnlyFlexVertexSpan res = span.subspan(3);

  EXPECT_FALSE(res.empty());
  EXPECT_EQ(res.data(), &vertices[3]);
  EXPECT_EQ(res.size(), vertices.size() - 3u);
  EXPECT_EQ(res.length(), vertices.size() - 3u);
  EXPECT_EQ(res.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(res.AsVertexDeclarationSpan(), vertexDeclaration);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan_Offset4)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  ReadOnlyFlexVertexSpan res = span.subspan(4);

  EXPECT_TRUE(res.empty());
  EXPECT_EQ(res.size(), 0u);
  EXPECT_EQ(res.length(), 0u);
  EXPECT_EQ(res.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(res.AsVertexDeclarationSpan(), vertexDeclaration);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan_Offset5_Invalid)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  EXPECT_THROW(span.subspan(5), std::out_of_range);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan_Offset1_Len0)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  ReadOnlyFlexVertexSpan res = span.subspan(1, 0);

  EXPECT_TRUE(res.empty());
  EXPECT_EQ(res.data(), &vertices[1]);
  EXPECT_EQ(res.size(), 0u);
  EXPECT_EQ(res.length(), 0u);
  EXPECT_EQ(res.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(res.AsVertexDeclarationSpan(), vertexDeclaration);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpan, Subspan_Offset1_Len2)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };
  auto flexSpan = ReadOnlyFlexSpanUtil::AsSpan(vertices);

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span(flexSpan, vertexDeclaration);

  ReadOnlyFlexVertexSpan res = span.subspan(1, 2);

  EXPECT_FALSE(res.empty());
  EXPECT_EQ(res.data(), &vertices[1]);
  EXPECT_EQ(res.size(), 2u);
  EXPECT_EQ(res.length(), 2u);
  EXPECT_EQ(res.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(res.AsVertexDeclarationSpan(), vertexDeclaration);
}

//
//
// TEST(TestReadOnlyFlexVertexSpan, SubSpan)
//{
//  std::array<uint16_t, 10> testArray = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//  ReadOnlyFlexibleSpan span(testArray.data(), testArray.size(), sizeof(uint16_t));
//
//  {
//    ReadOnlyFlexibleSpan res = span.subspan();
//    EXPECT_EQ(testArray.size(), res.size());
//    EXPECT_EQ(&testArray[0], res.data());
//    EXPECT_EQ(sizeof(uint16_t), res.elementSize());
//    EXPECT_EQ(res.size(), res.length());
//  }
//
//  {
//    ReadOnlyFlexibleSpan res = span.subspan(1u);
//    EXPECT_EQ(9u, res.size());
//    EXPECT_EQ(&testArray[1], res.data());
//    EXPECT_EQ(sizeof(uint16_t), res.elementSize());
//    EXPECT_EQ(res.size(), res.length());
//  }
//  {
//    ReadOnlyFlexibleSpan res = span.subspan(9u);
//    EXPECT_EQ(1u, res.size());
//    EXPECT_EQ(&testArray[9], res.data());
//    EXPECT_EQ(sizeof(uint16_t), res.elementSize());
//    EXPECT_EQ(res.size(), res.length());
//  }
//  {
//    ReadOnlyFlexibleSpan res = span.subspan(1u, 2u);
//    EXPECT_EQ(2u, res.size());
//    EXPECT_EQ(&testArray[1], res.data());
//    EXPECT_EQ(sizeof(uint16_t), res.elementSize());
//    EXPECT_EQ(res.size(), res.length());
//  }
//  {
//    ReadOnlyFlexibleSpan res = span.subspan(9u, 2u);
//    EXPECT_EQ(1u, res.size());
//    EXPECT_EQ(&testArray[9], res.data());
//    EXPECT_EQ(sizeof(uint16_t), res.elementSize());
//    EXPECT_EQ(res.size(), res.length());
//  }
//  // its ok to read the last entry
//  {
//    ReadOnlyFlexibleSpan res = span.subspan(testArray.size());
//    EXPECT_EQ(0u, res.size());
//    EXPECT_EQ(sizeof(uint16_t), res.elementSize());
//    EXPECT_EQ(res.size(), res.length());
//  }
//}
//
// TEST(TestReadOnlyFlexVertexSpan, SubSpan_Empty)
//{
//  ReadOnlyFlexibleSpan span;
//  {
//    ReadOnlyFlexibleSpan res = span.subspan();
//    EXPECT_EQ(0u, res.size());
//    EXPECT_EQ(0u, res.elementSize());
//    EXPECT_EQ(0u, res.length());
//  }
//}
//
//
// TEST(TestReadOnlyFlexVertexSpan, SubSpan_InvalidPos)
//{
//  ReadOnlyFlexibleSpan span("0123456789", 10, sizeof(char));
//
//  EXPECT_THROW(span.subspan(11u), std::out_of_range);
//}
