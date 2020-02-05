/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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
#include "TestFixture_StrategyBatchByState.hpp"

using namespace Fsl;

namespace
{
  // IMPROVEMENT: create a 'texture release' test by using shared_ptr and weak ptrs.

  using TextureInfo = TestStrategyBatchByState::texture_info_type;

  // using TestTypes = ::testing::Types<TestConfig<Fsl::BlendState::Opaque, Fsl::BlendState::AlphaBlend>>;
  // using TestTypes = ::testing::Types<TestConfig<Fsl::BlendState::AlphaBlend, Fsl::BlendState::Opaque>>;
  using TestTypes = ::testing::Types<TestConfig<Fsl::BlendState::Opaque, Fsl::BlendState::AlphaBlend>,
                                     TestConfig<Fsl::BlendState::AlphaBlend, Fsl::BlendState::Opaque>>;

  TYPED_TEST_SUITE(TestFixture_StrategyBatchByState, TestTypes);

  const constexpr Vector4 g_color0(0.1f, 0.2f, 0.3f, 0.4f);
  const constexpr Vector4 g_color1(0.5f, 0.6f, 0.7f, 0.8f);
  const constexpr Vector4 g_color2(0.9f, 1.0f, 1.1f, 1.2f);

  const constexpr TestQuad g_quad0(Vector2(100, 101), Vector2(102, 103), Vector2(104, 105), Vector2(106, 107), Vector2(0, 1), Vector2(2, 3),
                                   g_color0);
  const constexpr TestQuad g_quad1(Vector2(200, 201), Vector2(202, 203), Vector2(204, 205), Vector2(206, 207), Vector2(4, 5), Vector2(6, 7),
                                   g_color1);
  const constexpr TestQuad g_quad2(Vector2(300, 301), Vector2(302, 203), Vector2(304, 305), Vector2(306, 307), Vector2(8, 9), Vector2(10, 11),
                                   g_color2);

  const constexpr TextureInfo g_texInfo0(1337u);
  const constexpr TextureInfo g_texInfo1(1338u);
}

TYPED_TEST(TestFixture_StrategyBatchByState, InitialState)
{
  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, CapacityOfZero)
{
  typename TestFixture::strategy_type strategy(0);
  // Capacity of zero will always give at least a capacity of 1 quad
  EXPECT_GE(strategy.GetCapacity(), 1u);

  EXPECT_EQ(strategy.GetQuadCount(), 0u);
  EXPECT_EQ(strategy.GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, State_Blend0Tex0)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);

  EXPECT_EQ(this->m_strategy.GetActiveTexture(), g_texInfo0);
  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, State_Tex0Blend0)
{
  this->m_strategy.SetTexture(g_texInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);

  EXPECT_EQ(this->m_strategy.GetActiveTexture(), g_texInfo0);
  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Limits_Tex0BlendAdd)
{
  typename TestFixture::strategy_type strategy(1);

  strategy.SetTexture(g_texInfo0);
  strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(strategy, g_quad0);

  ASSERT_EQ(strategy.GetActiveTexture(), g_texInfo0);
  EXPECT_EQ(this->GetQuadCount(strategy), 1u);
  EXPECT_EQ(this->GetSegmentCount(strategy), 1u);

  this->CheckSpan(strategy, {g_quad0});
  this->CheckSegment(strategy, {TestSegment(1, g_texInfo0, TestFixture::BLEND0)});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto segementCount = strategy.GetSegmentCount();
    strategy.GetVertexCount();
    strategy.GetQuadCount();
    strategy.GetSpan();
    for (uint32_t i = 0; i < segementCount; ++i)
    {
      strategy.GetSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(g_texInfo0);
    strategy.SetTexture(g_texInfo1);
  }

  ASSERT_EQ(strategy.GetActiveTexture(), g_texInfo1);
  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);
  ASSERT_EQ(strategy.GetActiveTexture(), g_texInfo1);

  EXPECT_EQ(this->GetQuadCount(strategy), 1u);
  EXPECT_EQ(this->GetSegmentCount(strategy), 1u);

  this->CheckSpan(strategy, {g_quad0});
  this->CheckSegment(strategy, {TestSegment(1, g_texInfo0, TestFixture::BLEND0)});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto segementCount = strategy.GetSegmentCount();
    strategy.GetVertexCount();
    strategy.GetQuadCount();
    strategy.GetSpan();
    for (uint32_t i = 0; i < segementCount; ++i)
    {
      strategy.GetSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(g_texInfo0);
    strategy.SetTexture(g_texInfo1);
  }
}


TYPED_TEST(TestFixture_StrategyBatchByState, Limits_Tex0Blend0Add_Blend1Add)
{
  typename TestFixture::strategy_type strategy(2);

  strategy.SetTexture(g_texInfo0);
  strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(strategy, g_quad0);
  strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(strategy, g_quad1);

  ASSERT_EQ(strategy.GetActiveTexture(), g_texInfo0);
  EXPECT_EQ(this->GetQuadCount(strategy), 2u);
  EXPECT_EQ(this->GetSegmentCount(strategy), 2u);

  this->CheckSpan(strategy, {g_quad0, g_quad1});
  this->CheckSegment(strategy, {TestSegment(1, g_texInfo0, TestFixture::BLEND0), TestSegment(1, g_texInfo0, TestFixture::BLEND1)});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto segementCount = strategy.GetSegmentCount();
    strategy.GetVertexCount();
    strategy.GetQuadCount();
    strategy.GetSpan();
    for (uint32_t i = 0; i < segementCount; ++i)
    {
      strategy.GetSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(g_texInfo0);
    strategy.SetTexture(g_texInfo1);
  }

  ASSERT_EQ(strategy.GetActiveTexture(), g_texInfo1);
  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);
  ASSERT_EQ(strategy.GetActiveTexture(), g_texInfo1);

  EXPECT_EQ(this->GetQuadCount(strategy), 2u);
  EXPECT_EQ(this->GetSegmentCount(strategy), 2u);

  this->CheckSpan(strategy, {g_quad0, g_quad1});
  this->CheckSegment(strategy, {TestSegment(1, g_texInfo0, TestFixture::BLEND0), TestSegment(1, g_texInfo0, TestFixture::BLEND1)});

  {
    strategy.GetActiveTexture();
    strategy.GetCapacity();
    const auto segementCount = strategy.GetSegmentCount();
    strategy.GetVertexCount();
    strategy.GetQuadCount();
    strategy.GetSpan();
    for (uint32_t i = 0; i < segementCount; ++i)
    {
      strategy.GetSegment(i);
    }
    strategy.SetBlendState(TestFixture::BLEND0);
    strategy.SetBlendState(TestFixture::BLEND1);
    strategy.SetTexture(g_texInfo0);
    strategy.SetTexture(g_texInfo1);
  }
}


TYPED_TEST(TestFixture_StrategyBatchByState, State_Blend0Tex0Blend1)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);

  ASSERT_EQ(this->m_strategy.GetActiveTexture(), g_texInfo0);
  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);

  ASSERT_EQ(this->GetQuadCount(), 1u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0});

  this->CheckSegment({TestSegment(1, g_texInfo0, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Clear)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);

  ASSERT_EQ(this->GetQuadCount(), 1u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0});

  this->CheckSegment({TestSegment(1, g_texInfo0, TestFixture::BLEND0)});

  this->m_strategy.Clear();

  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0AddAdd_Clear)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(2, g_texInfo0, TestFixture::BLEND0)});

  this->m_strategy.Clear();

  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0AddTex1Add_Clear)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(1, g_texInfo0, TestFixture::BLEND0), TestSegment(1, g_texInfo1, TestFixture::BLEND0)});

  this->m_strategy.Clear();

  EXPECT_EQ(this->GetQuadCount(), 0u);
  EXPECT_EQ(this->GetSegmentCount(), 0u);

  this->CheckSpan({});
  this->CheckSegment({});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0AddAdd)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(2u, g_texInfo0, TestFixture::BLEND0)});
}

//! This tests the merge capability in SetTexture to ensure that we dont create unnecessary segments
TYPED_TEST(TestFixture_StrategyBatchByState, SetTextureMerge_Blend0Tex0Add_Tex1Tex0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetTexture(g_texInfo1);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(2u, g_texInfo0, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo0, TestFixture::BLEND1)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Add_Blend0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, g_quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, g_quad2);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  // We expect one opaque segment as there has been no texture switch so it can resume adding to the existing segment
  ASSERT_EQ(this->GetSegmentCount(), 3u);

  this->CheckSpan({g_quad0, g_quad1, g_quad2});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo0, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Tex1Add_Blend0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, g_quad2);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  ASSERT_EQ(this->GetSegmentCount(), 3u);

  this->CheckSpan({g_quad0, g_quad1, g_quad2});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo1, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Add_Blend0Tex1Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, g_quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad2);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  ASSERT_EQ(this->GetSegmentCount(), 3u);

  this->CheckSpan({g_quad0, g_quad1, g_quad2});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo0, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo1, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Tex1Add_Blend0Tex0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad1);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad2);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  ASSERT_EQ(this->GetSegmentCount(), 3u);

  this->CheckSpan({g_quad0, g_quad1, g_quad2});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Tex1Add_Tex0Blend0Add)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad1);
  this->m_strategy.SetTexture(g_texInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, g_quad2);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  ASSERT_EQ(this->GetSegmentCount(), 3u);

  this->CheckSpan({g_quad0, g_quad1, g_quad2});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Empty_EnsureCapacity)
{
  typename TestFixture::strategy_type strategy(4);

  uint32_t newCapacity = 8;
  strategy.EnsureCapacity(newCapacity);

  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);
  EXPECT_EQ(strategy.GetQuadCount(), 0u);
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);

  ASSERT_EQ(this->GetQuadCount(), 1u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);

  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);
  ASSERT_EQ(this->GetQuadCount(), 1u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});

  this->CheckSpan({g_quad0});

  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Tex1Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND0)});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND0)});
}


TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo0, TestFixture::BLEND1)});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo0, TestFixture::BLEND1)});
}

TYPED_TEST(TestFixture_StrategyBatchByState, Blend0Tex0Add_Blend1Tex1Add_Tex0Blend0Add_EnsureCapacity_GrowOne)
{
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->m_strategy.SetTexture(g_texInfo0);
  this->AddQuad(this->m_strategy, g_quad0);
  this->m_strategy.SetBlendState(TestFixture::BLEND1);
  this->m_strategy.SetTexture(g_texInfo1);
  this->AddQuad(this->m_strategy, g_quad1);
  this->m_strategy.SetTexture(g_texInfo0);
  this->m_strategy.SetBlendState(TestFixture::BLEND0);
  this->AddQuad(this->m_strategy, g_quad2);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  ASSERT_EQ(this->GetSegmentCount(), 3u);
  this->CheckSpan({g_quad0, g_quad1, g_quad2});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});

  auto newCapacity = this->m_strategy.GetCapacity() + 1;
  this->m_strategy.EnsureCapacity(newCapacity);
  ASSERT_GE(this->m_strategy.GetCapacity(), newCapacity);

  ASSERT_EQ(this->GetQuadCount(), 3u);
  ASSERT_EQ(this->GetSegmentCount(), 3u);
  this->CheckSpan({g_quad0, g_quad1, g_quad2});
  this->CheckSegment({TestSegment(1u, g_texInfo0, TestFixture::BLEND0), TestSegment(1u, g_texInfo1, TestFixture::BLEND1),
                      TestSegment(1u, g_texInfo0, TestFixture::BLEND0)});
}


TEST_F(TestFixture_StrategyBatchByStateBase, SetBlendState)
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

TEST_F(TestFixture_StrategyBatchByStateBase, SetBlendState_BlendOpaqueTex0Add)
{
  this->m_strategy.SetBlendState(BlendState::Opaque);
  this->m_strategy.SetTexture(g_texInfo0);
  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendOpaqueTex0Add_Test::AddQuad(this->m_strategy, g_quad0);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Opaque);

  ASSERT_EQ(this->GetQuadCount(), 1u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0});

  this->CheckSegment({TestSegment(1u, g_texInfo0, BlendState::Opaque)});
}


TEST_F(TestFixture_StrategyBatchByStateBase, SetBlendState_BlendOpaqueTex0Add2_BlendAdditiveAdd)
{
  this->m_strategy.SetBlendState(BlendState::Opaque);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Opaque);

  this->m_strategy.SetTexture(g_texInfo0);
  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendOpaqueTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, g_quad0);

  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendOpaqueTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});
  this->CheckSegment({TestSegment(1u, g_texInfo0, BlendState::Opaque), TestSegment(1u, g_texInfo0, BlendState::Additive)});
}


TEST_F(TestFixture_StrategyBatchByStateBase, SetBlendState_BlendAdditiveTex0Add2_BlendAdditiveAdd)
{
  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  this->m_strategy.SetTexture(g_texInfo0);
  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendAdditiveTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, g_quad0);

  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendAdditiveTex0Add2_BlendAdditiveAdd_Test::AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 1u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(2u, g_texInfo0, BlendState::Additive)});
}


TEST_F(TestFixture_StrategyBatchByStateBase, SetBlendState_BlendAdditiveTex0Add2_BlendAlphaAdd)
{
  this->m_strategy.SetBlendState(BlendState::Additive);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::Additive);

  this->m_strategy.SetTexture(g_texInfo0);
  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendAdditiveTex0Add2_BlendAlphaAdd_Test::AddQuad(this->m_strategy, g_quad0);

  this->m_strategy.SetBlendState(BlendState::AlphaBlend);
  EXPECT_EQ(this->m_strategy.GetActiveBlendState(), BlendState::AlphaBlend);

  TestFixture_StrategyBatchByStateBase_SetBlendState_BlendAdditiveTex0Add2_BlendAlphaAdd_Test::AddQuad(this->m_strategy, g_quad1);

  ASSERT_EQ(this->GetQuadCount(), 2u);
  ASSERT_EQ(this->GetSegmentCount(), 2u);

  this->CheckSpan({g_quad0, g_quad1});

  this->CheckSegment({TestSegment(1u, g_texInfo0, BlendState::Additive), TestSegment(1u, g_texInfo0, BlendState::AlphaBlend)});
}
