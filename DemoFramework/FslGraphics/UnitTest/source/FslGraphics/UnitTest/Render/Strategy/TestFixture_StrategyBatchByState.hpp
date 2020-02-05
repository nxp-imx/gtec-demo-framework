#ifndef FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTFIXTURE_STRATEGYBATCHBYSTATE_HPP
#define FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTFIXTURE_STRATEGYBATCHBYSTATE_HPP
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

#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <vector>
#include "TestQuad.hpp"
#include "TestQuadCompare.hpp"
#include "TestSegment.hpp"
#include "TestStrategyBatchByState.hpp"

namespace Fsl
{
  template <Fsl::BlendState TBlend0, Fsl::BlendState TBlend1>
  struct TestConfig
  {
    static const Fsl::BlendState BLEND0 = TBlend0;
    static const Fsl::BlendState BLEND1 = TBlend1;
  };

  class TestFixture_StrategyBatchByStateBase : public TestFixtureFslGraphics
  {
  public:
    using strategy_type = Fsl::TestStrategyBatchByState;
    strategy_type m_strategy;

    static void AddQuad(strategy_type& rStrategy, const Fsl::TestQuad& quad)
    {
      rStrategy.AddQuad(quad.Vec0, quad.Vec1, quad.Vec2, quad.Vec3, quad.TexCoords0, quad.TexCoords1, quad.Color);
    }

    uint32_t GetSegmentCount() const
    {
      return m_strategy.GetSegmentCount();
    }

    uint32_t GetVertexCount() const
    {
      return m_strategy.GetVertexCount();
    }

    uint32_t GetQuadCount() const
    {
      return m_strategy.GetQuadCount();
    }

    void CheckSpan(const std::vector<Fsl::TestQuad>& content) const
    {
      CheckSpan(m_strategy, content);
    }

    void CheckSegment(const std::vector<Fsl::TestSegment>& content) const
    {
      CheckSegment(m_strategy, content);
    }

    static uint32_t GetSegmentCount(const strategy_type& strategy)
    {
      return strategy.GetSegmentCount();
    }

    static uint32_t GetVertexCount(const strategy_type& strategy)
    {
      return strategy.GetVertexCount();
    }

    static uint32_t GetQuadCount(const strategy_type& strategy)
    {
      return strategy.GetQuadCount();
    }

    static void CheckSpan(const strategy_type& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      auto span = strategy.GetSpan();
      ASSERT_NE(span.pVertices, nullptr);
      EXPECT_EQ(span.VertexCount, static_cast<uint32_t>(content.size() * strategy_type::VERTICES_PER_QUAD));

      // Quads are stored in a back-to-front order (equal to insertion order)
      const auto quadCount = static_cast<uint32_t>(content.size());
      for (uint32_t i = 0; i < quadCount; ++i)
      {
        Expect_EQ(span, content[i], i);
      }
    }

    static void CheckSegment(const strategy_type& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      EXPECT_EQ(strategy.GetSegmentCount(), static_cast<uint32_t>(content.size()));
      const auto segmentCount = static_cast<uint32_t>(content.size());
      for (uint32_t i = 0; i < segmentCount; ++i)
      {
        auto segment = strategy.GetSegment(i);
        EXPECT_EQ(segment.VertexCount / strategy_type::VERTICES_PER_QUAD, content[i].QuadCount);
        EXPECT_EQ(segment.TextureInfo, content[i].TextureInfo);
        EXPECT_EQ(segment.ActiveBlendState, content[i].ActiveBlendState);
      }
    }
  };

  template <typename TConfig>
  class TestFixture_StrategyBatchByState : public TestFixture_StrategyBatchByStateBase
  {
  public:
    using config_type = TConfig;

    const Fsl::BlendState BLEND0 = config_type::BLEND0;
    const Fsl::BlendState BLEND1 = config_type::BLEND1;
  };
}

#endif
