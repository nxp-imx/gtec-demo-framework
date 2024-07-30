/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include "TestFixture_StrategySortByTransparency.hpp"

using namespace Fsl;

namespace
{
  // IMPROVEMENT: create a 'texture release' test by using shared_ptr and weak ptrs.

  using TextureInfo = TestStrategySortByTransparency::texture_info_type;

  // using TestTypes = ::testing::Types<TestConfigOpaqueTransp>;
  // using TestTypes = ::testing::Types<TestConfigTranspOpaque>;
  using TestTypes = ::testing::Types<TestConfigOpaqueTransp, TestConfigTranspOpaque>;

  TYPED_TEST_SUITE(TestFixture_SortByTransparency, TestTypes);

  const constexpr Color Color0(0.1f, 0.2f, 0.3f, 0.4f);
  const constexpr Color Color1(0.5f, 0.6f, 0.7f, 0.8f);
  const constexpr Color Color2(0.9f, 1.0f, 1.1f, 1.2f);

  const constexpr TestQuad Quad0(Vector2(100, 101), Vector2(102, 103), Vector2(104, 105), Vector2(106, 107), Vector2(0, 1), Vector2(2, 3), Color0);
  const constexpr TestQuad Quad1(Vector2(200, 201), Vector2(202, 203), Vector2(204, 205), Vector2(206, 207), Vector2(4, 5), Vector2(6, 7), Color1);
  const constexpr TestQuad Quad2(Vector2(300, 301), Vector2(302, 203), Vector2(304, 305), Vector2(306, 307), Vector2(8, 9), Vector2(10, 11), Color2);

  const constexpr TextureInfo TexInfo0(1337u);
  const constexpr TextureInfo TexInfo1(1338u);
}

TYPED_TEST(TestFixture_SortByTransparency, InitialState)
{
  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, CapacityOfZero)
{
  typename TestFixture::strategy_type strategy(0);
  // Capacity of zero will always give at least a capacity of 1 quad
  EXPECT_GE(strategy.GetCapacity(), 1u);

  EXPECT_EQ(strategy.GetOpaqueQuadCount(), 0u);
  EXPECT_EQ(strategy.GetTransparentQuadCount(), 0u);
  EXPECT_EQ(strategy.GetOpaqueSegmentCount(), 0u);
  EXPECT_EQ(strategy.GetTransparentSegmentCount(), 0u);
}


TYPED_TEST(TestFixture_SortByTransparency, State_Blend0Tex0)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);

  EXPECT_EQ(this->m_strategy.GetActiveTexture(), TexInfo0);
  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, State_Tex0Blend0)
{
  this->m_strategy.SetTexture(TexInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);

  ASSERT_EQ(this->m_strategy.GetActiveTexture(), TexInfo0);
  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Limits_Tex0BlendAdd)
{
  typename TestFixture::strategy_type strategy(1);

  strategy.SetTexture(TexInfo0);
  strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(strategy, Quad0);

  ASSERT_EQ(strategy.GetActiveTexture(), TexInfo0);
  EXPECT_EQ(this->GetBlend0QuadCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1QuadCount(strategy), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1SegmentCount(strategy), 0u);

  this->CheckBlend0Span(strategy, {Quad0});
  this->CheckBlend1Span(strategy, {});
  this->CheckBlend0Segment(strategy, {TestSegment(1, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment(strategy, {});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto opaqueSegementCount = strategy.GetOpaqueSegmentCount();
    const auto transpSegementCount = strategy.GetTransparentSegmentCount();
    strategy.GetOpaqueVertexCount();
    strategy.GetTransparentVertexCount();
    strategy.GetOpaqueQuadCount();
    strategy.GetTransparentQuadCount();
    strategy.GetOpaqueSpan();
    strategy.GetTransparentSpan();
    for (uint32_t i = 0; i < opaqueSegementCount; ++i)
    {
      strategy.GetOpaqueSegment(i);
    }
    for (uint32_t i = 0; i < transpSegementCount; ++i)
    {
      strategy.GetTransparentSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(TexInfo0);
    strategy.SetTexture(TexInfo1);
  }

  ASSERT_EQ(strategy.GetActiveTexture(), TexInfo1);
  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);
  ASSERT_EQ(strategy.GetActiveTexture(), TexInfo1);

  EXPECT_EQ(this->GetBlend0QuadCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1QuadCount(strategy), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1SegmentCount(strategy), 0u);

  this->CheckBlend0Span(strategy, {Quad0});
  this->CheckBlend1Span(strategy, {});
  this->CheckBlend0Segment(strategy, {TestSegment(1, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment(strategy, {});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto opaqueSegementCount = strategy.GetOpaqueSegmentCount();
    const auto transpSegementCount = strategy.GetTransparentSegmentCount();
    strategy.GetOpaqueVertexCount();
    strategy.GetTransparentVertexCount();
    strategy.GetOpaqueQuadCount();
    strategy.GetTransparentQuadCount();
    strategy.GetOpaqueSpan();
    strategy.GetTransparentSpan();
    for (uint32_t i = 0; i < opaqueSegementCount; ++i)
    {
      strategy.GetOpaqueSegment(i);
    }
    for (uint32_t i = 0; i < transpSegementCount; ++i)
    {
      strategy.GetTransparentSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(TexInfo0);
    strategy.SetTexture(TexInfo1);
  }
}


TYPED_TEST(TestFixture_SortByTransparency, Limits_Tex0Blend0Add_Blend1Add)
{
  typename TestFixture::strategy_type strategy(1);

  strategy.SetTexture(TexInfo0);
  strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(strategy, Quad0);
  strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(strategy, Quad1);

  ASSERT_EQ(strategy.GetActiveTexture(), TexInfo0);
  EXPECT_EQ(this->GetBlend0QuadCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1QuadCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend0SegmentCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1SegmentCount(strategy), 1u);

  this->CheckBlend0Span(strategy, {Quad0});
  this->CheckBlend1Span(strategy, {Quad1});
  this->CheckBlend0Segment(strategy, {TestSegment(1, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment(strategy, {TestSegment(1, TexInfo0, TestFixture::BLEND1)});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto opaqueSegementCount = strategy.GetOpaqueSegmentCount();
    const auto transpSegementCount = strategy.GetTransparentSegmentCount();
    strategy.GetOpaqueVertexCount();
    strategy.GetTransparentVertexCount();
    strategy.GetOpaqueQuadCount();
    strategy.GetTransparentQuadCount();
    strategy.GetOpaqueSpan();
    strategy.GetTransparentSpan();
    for (uint32_t i = 0; i < opaqueSegementCount; ++i)
    {
      strategy.GetOpaqueSegment(i);
    }
    for (uint32_t i = 0; i < transpSegementCount; ++i)
    {
      strategy.GetTransparentSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(TexInfo0);
    strategy.SetTexture(TexInfo1);
  }

  ASSERT_EQ(strategy.GetActiveTexture(), TexInfo1);
  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);
  ASSERT_EQ(strategy.GetActiveTexture(), TexInfo1);

  EXPECT_EQ(this->GetBlend0QuadCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1QuadCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend0SegmentCount(strategy), 1u);
  EXPECT_EQ(this->GetBlend1SegmentCount(strategy), 1u);

  this->CheckBlend0Span(strategy, {Quad0});
  this->CheckBlend1Span(strategy, {Quad1});
  this->CheckBlend0Segment(strategy, {TestSegment(1, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment(strategy, {TestSegment(1, TexInfo0, TestFixture::BLEND1)});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto opaqueSegementCount = strategy.GetOpaqueSegmentCount();
    const auto transpSegementCount = strategy.GetTransparentSegmentCount();
    strategy.GetOpaqueVertexCount();
    strategy.GetTransparentVertexCount();
    strategy.GetOpaqueQuadCount();
    strategy.GetTransparentQuadCount();
    strategy.GetOpaqueSpan();
    strategy.GetTransparentSpan();
    for (uint32_t i = 0; i < opaqueSegementCount; ++i)
    {
      strategy.GetOpaqueSegment(i);
    }
    for (uint32_t i = 0; i < transpSegementCount; ++i)
    {
      strategy.GetTransparentSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(TexInfo0);
    strategy.SetTexture(TexInfo1);
  }
}


TYPED_TEST(TestFixture_SortByTransparency, State_Blend0Tex0Blend1)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);

  ASSERT_EQ(this->m_strategy.GetActiveTexture(), TexInfo0);
  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Clear)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});

  this->m_strategy.Clear();

  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0AddAdd_Clear)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0, Quad1});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(2, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});

  this->m_strategy.Clear();

  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0AddTex1Add_Clear)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 2u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0, Quad1});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1, TexInfo0, TestFixture::BLEND0), TestSegment(1, TexInfo1, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});

  this->m_strategy.Clear();

  EXPECT_EQ(this->GetBlend0QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend1QuadCount(), 0u);
  EXPECT_EQ(this->GetBlend0SegmentCount(), 0u);
  EXPECT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({});
  this->CheckBlend1Span({});
  this->CheckBlend0Segment({});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0AddAdd)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0, Quad1});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});
}

//! This tests the merge capability in SetTexture to ensure that we dont create unnecessary segments
TYPED_TEST(TestFixture_SortByTransparency, SetTextureMerge_Blend0Tex0Add_Tex1Tex0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetTexture(TexInfo1);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0, Quad1});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Add_Blend0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, Quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, Quad2);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  // We expect one opaque segment as there has been no texture switch so it can resume adding to the existing segment
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, Quad2});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Tex1Add_Blend0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, Quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, Quad2);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  // We expect two opaque segment as there has been a texture switch
  ASSERT_EQ(this->GetBlend0SegmentCount(), 2u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, Quad2});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0), TestSegment(1u, TexInfo1, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo1, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Add_Blend0Tex1Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, Quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, Quad2);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  // We expect two opaque segment as there has been a texture switch
  ASSERT_EQ(this->GetBlend0SegmentCount(), 2u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, Quad2});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0), TestSegment(1u, TexInfo1, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Tex1Add_Blend0Tex0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, Quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad2);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  // We expect one opaque segment as there has been no texture switch for opaque so it can resume adding to the existing segment
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, Quad2});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo1, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Tex1Add_Tex0Blend0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, Quad1);
  this->m_strategy.SetTexture(TexInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, Quad2);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  // We expect one opaque segment as there has been no texture switch for opaque so it can resume adding to the existing segment
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, Quad2});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo1, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_SortByTransparency, Empty_EnsureCapacity)
{
  typename TestFixture::strategy_type strategy(4);
  auto newCapacity = 8u;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  EXPECT_EQ(strategy.GetOpaqueQuadCount(), 0u);
  EXPECT_EQ(strategy.GetTransparentQuadCount(), 0u);
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Tex1Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 2u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0, Quad1});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0), TestSegment(1u, TexInfo1, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 0u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 2u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 0u);

  this->CheckBlend0Span({Quad0, Quad1});
  this->CheckBlend1Span({});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0), TestSegment(1u, TexInfo1, TestFixture::BLEND0)});
  this->CheckBlend1Segment({});
}


TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND1)});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetBlend0QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0});
  this->CheckBlend1Span({Quad1});

  this->CheckBlend0Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo0, TestFixture::BLEND1)});
}

TYPED_TEST(TestFixture_SortByTransparency, Blend0Tex0Add_Blend1Tex1Add_Tex0Blend0Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(TexInfo0);
  this->AddQuad(this->m_strategy, Quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(TexInfo1);
  this->AddQuad(this->m_strategy, ::Quad1);
  this->m_strategy.SetTexture(TexInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, ::Quad2);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, ::Quad2});
  this->CheckBlend1Span({::Quad1});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo1, TestFixture::BLEND1)});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetBlend0QuadCount(), 2u);
  ASSERT_EQ(this->GetBlend1QuadCount(), 1u);
  ASSERT_EQ(this->GetBlend0SegmentCount(), 1u);
  ASSERT_EQ(this->GetBlend1SegmentCount(), 1u);

  this->CheckBlend0Span({Quad0, ::Quad2});
  this->CheckBlend1Span({::Quad1});

  this->CheckBlend0Segment({TestSegment(2u, TexInfo0, TestFixture::BLEND0)});
  this->CheckBlend1Segment({TestSegment(1u, TexInfo1, TestFixture::BLEND1)});
}


TEST_F(TestFixture_SortByTransparencyBase, SetBlendState)
{
  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  this->m_strategy.SetBlendState(BlendState::AlphaBlend);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::AlphaBlend);

  this->m_strategy.SetBlendState(BlendState::NonPremultiplied);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::NonPremultiplied);

  this->m_strategy.SetBlendState(BlendState::Opaque);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Opaque);
}

TEST_F(TestFixture_SortByTransparencyBase, SetBlendState_BlendOpaqueTex0Add)
{
  this->m_strategy.SetBlendState(BlendState::Opaque);
  this->m_strategy.SetTexture(TexInfo0);
  TestFixture_SortByTransparencyBase_SetBlendState_BlendOpaqueTex0Add_Test::AddQuad(this->m_strategy, Quad0);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Opaque);

  ASSERT_EQ(GetOpaqueQuadCount(), 1u);
  ASSERT_EQ(GetTranspQuadCount(), 0u);
  ASSERT_EQ(GetOpaqueSegmentCount(), 1u);
  ASSERT_EQ(GetTranspSegmentCount(), 0u);

  CheckOpaqueSpan({Quad0});
  CheckTranspSpan({});

  CheckOpaqueSegment({TestSegment(1u, TexInfo0, BlendState::Opaque)});
  CheckTranspSegment({});
}

TEST_F(TestFixture_SortByTransparencyBase, SetBlendState_BlendOpaqueTex0Add2_BlendAdditiveAdd)
{
  this->m_strategy.SetBlendState(BlendState::Opaque);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Opaque);

  this->m_strategy.SetTexture(TexInfo0);
  TestFixture_SortByTransparencyBase_SetBlendState_BlendOpaqueTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, Quad0);

  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  TestFixture_SortByTransparencyBase_SetBlendState_BlendOpaqueTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(GetOpaqueQuadCount(), 1u);
  ASSERT_EQ(GetTranspQuadCount(), 1u);
  ASSERT_EQ(GetOpaqueSegmentCount(), 1u);
  ASSERT_EQ(GetTranspSegmentCount(), 1u);

  CheckOpaqueSpan({Quad0});
  CheckTranspSpan({Quad1});

  CheckOpaqueSegment({TestSegment(1u, TexInfo0, BlendState::Opaque)});
  CheckTranspSegment({TestSegment(1u, TexInfo0, BlendState::Additive)});
}


TEST_F(TestFixture_SortByTransparencyBase, SetBlendState_BlendAdditiveTex0Add2_BlendAdditiveAdd)
{
  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  this->m_strategy.SetTexture(TexInfo0);
  TestFixture_SortByTransparencyBase_SetBlendState_BlendAdditiveTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, Quad0);

  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  TestFixture_SortByTransparencyBase_SetBlendState_BlendAdditiveTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(GetOpaqueQuadCount(), 0u);
  ASSERT_EQ(GetTranspQuadCount(), 2u);
  ASSERT_EQ(GetOpaqueSegmentCount(), 0u);
  ASSERT_EQ(GetTranspSegmentCount(), 1u);

  CheckOpaqueSpan({});
  CheckTranspSpan({Quad0, Quad1});

  CheckOpaqueSegment({});
  CheckTranspSegment({TestSegment(2u, TexInfo0, BlendState::Additive)});
}


TEST_F(TestFixture_SortByTransparencyBase, SetBlendState_BlendAdditiveTex0Add2_BlendAlphaAdd)
{
  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  this->m_strategy.SetTexture(TexInfo0);
  TestFixture_SortByTransparencyBase_SetBlendState_BlendAdditiveTex0Add2_BlendAlphaAdd_Test::AddQuad(this->m_strategy, Quad0);

  this->m_strategy.SetBlendState(BlendState::AlphaBlend);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::AlphaBlend);

  TestFixture_SortByTransparencyBase_SetBlendState_BlendAdditiveTex0Add2_BlendAlphaAdd_Test::AddQuad(this->m_strategy, Quad1);

  ASSERT_EQ(GetOpaqueQuadCount(), 0u);
  ASSERT_EQ(GetTranspQuadCount(), 2u);
  ASSERT_EQ(GetOpaqueSegmentCount(), 0u);
  ASSERT_EQ(GetTranspSegmentCount(), 2u);

  CheckOpaqueSpan({});
  CheckTranspSpan({Quad0, Quad1});

  CheckOpaqueSegment({});
  CheckTranspSegment({TestSegment(1u, TexInfo0, BlendState::Additive), TestSegment(1u, TexInfo0, BlendState::AlphaBlend)});
}
