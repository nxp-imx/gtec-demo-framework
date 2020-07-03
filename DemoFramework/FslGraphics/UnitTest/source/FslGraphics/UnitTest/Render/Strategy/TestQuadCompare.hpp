#ifndef FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTQUADCOMPARE_HPP
#define FSLGRAPHICS_UNITTEST_RENDER_STRATEGY_FSLGRAPHICS_UNITTEST_TESTQUADCOMPARE_HPP
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

#include <FslUnitTest/TestFixture.hpp>
#include <FslBase/BasicTypes.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/Math/Vector4.hpp>
#include <FslGraphics/Vertices/VertexPositionColorTexture.hpp>
#include <FslGraphics/Vertices/VertexSpan.hpp>
#include "TestQuad.hpp"

namespace Fsl
{
  inline void Expect_EQ(const Fsl::VertexPositionColorTexture& vertex, const Fsl::Vector2& pos, const Fsl::Vector2& texCoord,
                        const Fsl::Vector4& color)
  {
    EXPECT_EQ(vertex.Position.X, pos.X);
    EXPECT_EQ(vertex.Position.Y, pos.Y);
    EXPECT_EQ(vertex.TextureCoordinate.X, texCoord.X);
    EXPECT_EQ(vertex.TextureCoordinate.Y, texCoord.Y);
    EXPECT_EQ(vertex.Color, color);
  }

  inline void Expect_EQ(const Fsl::VertexSpan<VertexPositionColorTexture>& span, const Fsl::TestQuad& quad, const uint32_t quadIndex)
  {
    constexpr const uint32_t VERTEX_COUNT = 4;

    auto quadVertexIndex = quadIndex * VERTEX_COUNT;
    if (quadVertexIndex >= span.VertexCount)
    {
      throw std::invalid_argument("quad index out of bounds");
    }

    Fsl::Vector2 texCoord_10(quad.TexCoords1.X, quad.TexCoords0.Y);
    Fsl::Vector2 texCoord_01(quad.TexCoords0.X, quad.TexCoords1.Y);

    const auto* pSrcVertices = span.pVertices + quadVertexIndex;
    ASSERT_NE(pSrcVertices, nullptr);
    if (pSrcVertices == nullptr)
    {
      // Get clang to stop complaining about something thats not possible
      throw std::runtime_error("pSrcVertices nullptr");
    }
    Expect_EQ(pSrcVertices[0], quad.Vec0, quad.TexCoords0, quad.Color);
    Expect_EQ(pSrcVertices[1], quad.Vec1, texCoord_10, quad.Color);
    Expect_EQ(pSrcVertices[2], quad.Vec2, texCoord_01, quad.Color);
    Expect_EQ(pSrcVertices[3], quad.Vec3, quad.TexCoords1, quad.Color);
  }
}

#endif
