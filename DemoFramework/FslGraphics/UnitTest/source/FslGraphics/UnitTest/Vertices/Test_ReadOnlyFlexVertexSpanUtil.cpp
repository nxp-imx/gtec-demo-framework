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
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <FslGraphics/Vertices/ReadOnlyFlexVertexSpanUtil.hpp>
#include <FslGraphics/Vertices/VertexPosition.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestReadOnlyFlexVertexSpanUtil = TestFixtureFslBase;
}


TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(vertices.data(), vertices.size(), vertexDeclaration);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), vertices.data());
  EXPECT_EQ(span.size(), vertices.size());
  EXPECT_EQ(span.length(), vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  EXPECT_THROW(ReadOnlyFlexVertexSpanUtil::AsSpan<VertexPosition>(nullptr, vertices.size(), vertexDeclaration), std::invalid_argument);
}

TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_LengthZero)
{
  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan<VertexPosition>(nullptr, 0u, vertexDeclaration);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_NullVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const VertexDeclarationSpan vertexDeclaration;
  EXPECT_THROW(ReadOnlyFlexVertexSpanUtil::AsSpan<VertexPosition>(nullptr, vertices.size(), vertexDeclaration), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_LengthZero_NullVertexDeclaration)
{
  const VertexDeclarationSpan vertexDeclaration;
  EXPECT_THROW(ReadOnlyFlexVertexSpanUtil::AsSpan<VertexPosition>(nullptr, 0u, vertexDeclaration), std::invalid_argument);
}


TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullVertexDeclaration)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  EXPECT_THROW(ReadOnlyFlexVertexSpanUtil::AsSpan(vertices.data(), vertices.size(), VertexDeclarationSpan()), std::invalid_argument);
}

// -----------------------------

TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NoCheck)
{
  constexpr std::array<VertexPosition, 4> vertices = {
    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
  };

  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span =
    ReadOnlyFlexVertexSpanUtil::AsSpan(vertices.data(), vertices.size(), vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), vertices.data());
  EXPECT_EQ(span.size(), vertices.size());
  EXPECT_EQ(span.length(), vertices.size());
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// Not valid with NoCheck
// TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_NoCheck)
//{
//  constexpr std::array<VertexPosition, 4> vertices = {
//    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
//    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
//    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
//    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
//  };
//
//  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
//  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(nullptr, vertices.size(), vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.size(), 0u);
//  EXPECT_EQ(span.length(), 0u);
//  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//}

TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_LengthZero_NoCheck)
{
  const auto vertexDeclaration = VertexPosition::AsVertexDeclarationSpan();
  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan<VertexPosition>(nullptr, 0u, vertexDeclaration, OptimizationCheckFlag::NoCheck);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
}

// Not valid with no check
// TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_NullVertexDeclaration_NoCheck)
//{
//  constexpr std::array<VertexPosition, 4> vertices = {
//    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
//    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
//    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
//    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
//  };
//
//  const VertexDeclarationSpan vertexDeclaration;
//  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(nullptr, vertices.size(), vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.size(), 0u);
//  EXPECT_EQ(span.length(), 0u);
//  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//}


// Not valid with no check
// TEST(TestReadOnlyFlexVertexSpanUtil, Construct_PointerLength_NullPointer_LengthZero_NullVertexDeclaration_NoCheck)
//{
//  constexpr std::array<VertexPosition, 4> vertices = {
//    VertexPosition(Vector3(1.0f, 2.0f, 3.0f)),
//    VertexPosition(Vector3(4.0f, 5.0f, 6.0f)),
//    VertexPosition(Vector3(7.0f, 8.0f, 9.0f)),
//    VertexPosition(Vector3(9.1f, 9.2f, 9.3f)),
//  };
//
//  const VertexDeclarationSpan vertexDeclaration;
//  ReadOnlyFlexVertexSpan span = ReadOnlyFlexVertexSpanUtil::AsSpan(nullptr, 0u, vertexDeclaration, OptimizationCheckFlag::NoCheck);
//
//  EXPECT_TRUE(span.empty());
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.data(), nullptr);
//  EXPECT_EQ(span.size(), 0u);
//  EXPECT_EQ(span.length(), 0u);
//  EXPECT_EQ(span.stride(), vertexDeclaration.VertexStride());
//  EXPECT_EQ(span.AsVertexDeclarationSpan(), vertexDeclaration);
//}
