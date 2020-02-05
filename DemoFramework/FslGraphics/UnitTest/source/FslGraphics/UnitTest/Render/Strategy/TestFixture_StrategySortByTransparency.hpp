#ifndef FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTFIXTURE_STRATEGYSORTBYTRANSPARENCY_HPP
#define FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTFIXTURE_STRATEGYSORTBYTRANSPARENCY_HPP
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
#include "TestStrategySortByTransparency.hpp"

namespace Fsl
{
  template <Fsl::BlendState TBlend0, Fsl::BlendState TBlend1>
  struct TestConfig
  {
    static const Fsl::BlendState BLEND0 = TBlend0;
    static const Fsl::BlendState BLEND1 = TBlend1;
  };

  namespace TestConfigHelper
  {
    static constexpr auto VERTEX_COUNT = TestStrategySortByTransparency::VERTICES_PER_QUAD;


    inline void CheckOpaqueSpan(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      auto span = strategy.GetOpaqueSpan();
      ASSERT_NE(span.pVertices, nullptr);
      EXPECT_EQ(span.VertexCount, static_cast<uint32_t>(content.size() * VERTEX_COUNT));

      // Opaque quads are stored in a front-to-back order (reverse of insertion)
      const auto quadCount = static_cast<uint32_t>(content.size());
      for (uint32_t i = 0; i < quadCount; ++i)
      {
        Expect_EQ(span, content[quadCount - i - 1], i);
      }
    }

    inline void CheckTranspSpan(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      auto span = strategy.GetTransparentSpan();
      ASSERT_NE(span.pVertices, nullptr);
      EXPECT_EQ(span.VertexCount, static_cast<uint32_t>(content.size() * VERTEX_COUNT));

      // Transparent quads are stored in a back-to-front order (equal to insertion order)
      const auto quadCount = static_cast<uint32_t>(content.size());
      for (uint32_t i = 0; i < quadCount; ++i)
      {
        Expect_EQ(span, content[i], i);
      }
    }

    inline void CheckOpaqueSegment(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      EXPECT_EQ(strategy.GetOpaqueSegmentCount(), static_cast<uint32_t>(content.size()));
      const auto segmentCount = static_cast<uint32_t>(content.size());
      for (uint32_t i = 0; i < segmentCount; ++i)
      {
        auto segment = strategy.GetOpaqueSegment(i);
        EXPECT_EQ(segment.VertexCount / VERTEX_COUNT, content[i].QuadCount);
        EXPECT_EQ(segment.TextureInfo, content[i].TextureInfo);
        EXPECT_EQ(segment.ActiveBlendState, content[i].ActiveBlendState);
      }
    }

    inline void CheckTranspSegment(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      EXPECT_EQ(strategy.GetTransparentSegmentCount(), static_cast<uint32_t>(content.size()));
      const auto segmentCount = static_cast<uint32_t>(content.size());
      for (uint32_t i = 0; i < segmentCount; ++i)
      {
        auto segment = strategy.GetTransparentSegment(i);
        EXPECT_EQ(segment.VertexCount / VERTEX_COUNT, content[i].QuadCount);
        EXPECT_EQ(segment.TextureInfo, content[i].TextureInfo);
        EXPECT_EQ(segment.ActiveBlendState, content[i].ActiveBlendState);
      }
    }
  };

  struct TestConfigOpaqueTransp : public TestConfig<Fsl::BlendState::Opaque, Fsl::BlendState::AlphaBlend>
  {
    static uint32_t GetBlend0SegmentCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetOpaqueSegmentCount();
    }
    static uint32_t GetBlend1SegmentCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetTransparentSegmentCount();
    }
    static uint32_t GetBlend0VertexCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetOpaqueVertexCount();
    }
    static uint32_t GetBlend1VertexCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetTransparentVertexCount();
    }
    static uint32_t GetBlend0QuadCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetOpaqueQuadCount();
    }
    static uint32_t GetBlend1QuadCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetTransparentQuadCount();
    }
    static void CheckBlend0Span(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return TestConfigHelper::CheckOpaqueSpan(strategy, content);
    }
    static void CheckBlend1Span(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return TestConfigHelper::CheckTranspSpan(strategy, content);
    }
    static void CheckBlend0Segment(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      return TestConfigHelper::CheckOpaqueSegment(strategy, content);
    }
    static void CheckBlend1Segment(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      return TestConfigHelper::CheckTranspSegment(strategy, content);
    }
  };

  struct TestConfigTranspOpaque : public TestConfig<Fsl::BlendState::AlphaBlend, Fsl::BlendState::Opaque>
  {
    static uint32_t GetBlend0SegmentCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetTransparentSegmentCount();
    }
    static uint32_t GetBlend1SegmentCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetOpaqueSegmentCount();
    }
    static uint32_t GetBlend0VertexCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetTransparentVertexCount();
    }
    static uint32_t GetBlend1VertexCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetOpaqueVertexCount();
    }
    static uint32_t GetBlend0QuadCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetTransparentQuadCount();
    }
    static uint32_t GetBlend1QuadCount(const Fsl::TestStrategySortByTransparency& strategy)
    {
      return strategy.GetOpaqueQuadCount();
    }
    static void CheckBlend0Span(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return TestConfigHelper::CheckTranspSpan(strategy, content);
    }
    static void CheckBlend1Span(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return TestConfigHelper::CheckOpaqueSpan(strategy, content);
    }
    static void CheckBlend0Segment(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      return TestConfigHelper::CheckTranspSegment(strategy, content);
    }
    static void CheckBlend1Segment(const Fsl::TestStrategySortByTransparency& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      return TestConfigHelper::CheckOpaqueSegment(strategy, content);
    }
  };

  class TestFixture_SortByTransparencyBase : public TestFixtureFslGraphics
  {
  public:
    using strategy_type = Fsl::TestStrategySortByTransparency;
    strategy_type m_strategy;

    static void AddQuad(strategy_type& rStrategy, const Fsl::TestQuad& quad)
    {
      rStrategy.AddQuad(quad.Vec0, quad.Vec1, quad.Vec2, quad.Vec3, quad.TexCoords0, quad.TexCoords1, quad.Color);
    }

    uint32_t GetOpaqueSegmentCount() const
    {
      return m_strategy.GetOpaqueSegmentCount();
    }

    uint32_t GetTranspSegmentCount() const
    {
      return m_strategy.GetTransparentSegmentCount();
    }

    uint32_t GetOpaqueVertexCount() const
    {
      return m_strategy.GetOpaqueVertexCount();
    }

    uint32_t GetTranspVertexCount() const
    {
      return m_strategy.GetTransparentVertexCount();
    }

    uint32_t GetOpaqueQuadCount() const
    {
      return m_strategy.GetOpaqueQuadCount();
    }

    uint32_t GetTranspQuadCount() const
    {
      return m_strategy.GetTransparentQuadCount();
    }

    void CheckOpaqueSpan(const std::vector<Fsl::TestQuad>& content) const
    {
      return CheckOpaqueSpan(m_strategy, content);
    }

    void CheckTranspSpan(const std::vector<Fsl::TestQuad>& content) const
    {
      return CheckTranspSpan(m_strategy, content);
    }

    void CheckOpaqueSegment(const std::vector<Fsl::TestSegment>& content) const
    {
      return CheckOpaqueSegment(m_strategy, content);
    }

    void CheckTranspSegment(const std::vector<Fsl::TestSegment>& content) const
    {
      return CheckTranspSegment(m_strategy, content);
    }

    static void CheckOpaqueSpan(const strategy_type& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return TestConfigHelper::CheckOpaqueSpan(strategy, content);
    }

    static void CheckTranspSpan(const strategy_type& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return TestConfigHelper::CheckTranspSpan(strategy, content);
    }

    static void CheckOpaqueSegment(const strategy_type& rStrategy, const std::vector<Fsl::TestSegment>& content)
    {
      TestConfigHelper::CheckOpaqueSegment(rStrategy, content);
    }

    static void CheckTranspSegment(const strategy_type& rStrategy, const std::vector<Fsl::TestSegment>& content)
    {
      TestConfigHelper::CheckTranspSegment(rStrategy, content);
    }
  };

  template <typename TConfig>
  class TestFixture_SortByTransparency : public TestFixture_SortByTransparencyBase
  {
  public:
    using config_type = TConfig;

    const Fsl::BlendState BLEND0 = config_type::BLEND0;
    const Fsl::BlendState BLEND1 = config_type::BLEND1;


    uint32_t GetBlend0SegmentCount() const
    {
      return GetBlend0SegmentCount(m_strategy);
    }

    uint32_t GetBlend1SegmentCount() const
    {
      return GetBlend1SegmentCount(m_strategy);
    }

    uint32_t GetBlend0VertexCount() const
    {
      return GetBlend0VertexCount(m_strategy);
    }

    uint32_t GetBlend1VertexCount() const
    {
      return GetBlend1VertexCount(m_strategy);
    }

    uint32_t GetBlend0QuadCount() const
    {
      return GetBlend0QuadCount(m_strategy);
    }

    uint32_t GetBlend1QuadCount() const
    {
      return GetBlend1QuadCount(m_strategy);
    }

    void CheckBlend0Span(const std::vector<Fsl::TestQuad>& content) const
    {
      return CheckBlend0Span(m_strategy, content);
    }

    void CheckBlend1Span(const std::vector<Fsl::TestQuad>& content) const
    {
      return CheckBlend1Span(m_strategy, content);
    }

    void CheckBlend0Segment(const std::vector<Fsl::TestSegment>& content) const
    {
      return CheckBlend0Segment(m_strategy, content);
    }

    void CheckBlend1Segment(const std::vector<Fsl::TestSegment>& content) const
    {
      return CheckBlend1Segment(m_strategy, content);
    }

    static uint32_t GetBlend0SegmentCount(const strategy_type& strategy)
    {
      return config_type::GetBlend0SegmentCount(strategy);
    }

    static uint32_t GetBlend1SegmentCount(const strategy_type& strategy)
    {
      return config_type::GetBlend1SegmentCount(strategy);
    }

    static uint32_t GetBlend0VertexCount(const strategy_type& strategy)
    {
      return config_type::GetBlend0VertexCount(strategy);
    }

    static uint32_t GetBlend1VertexCount(const strategy_type& strategy)
    {
      return config_type::GetBlend1VertexCount(strategy);
    }

    static uint32_t GetBlend0QuadCount(const strategy_type& strategy)
    {
      return config_type::GetBlend0QuadCount(strategy);
    }

    static uint32_t GetBlend1QuadCount(const strategy_type& strategy)
    {
      return config_type::GetBlend1QuadCount(strategy);
    }

    static void CheckBlend0Span(const strategy_type& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return config_type::CheckBlend0Span(strategy, content);
    }

    static void CheckBlend1Span(const strategy_type& strategy, const std::vector<Fsl::TestQuad>& content)
    {
      return config_type::CheckBlend1Span(strategy, content);
    }

    static void CheckBlend0Segment(const strategy_type& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      return config_type::CheckBlend0Segment(strategy, content);
    }

    static void CheckBlend1Segment(const strategy_type& strategy, const std::vector<Fsl::TestSegment>& content)
    {
      return config_type::CheckBlend1Segment(strategy, content);
    }
  };
}

#endif
