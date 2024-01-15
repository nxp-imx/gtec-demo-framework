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

#include <FslBase/Log/Math/LogPoint2.hpp>
#include <FslBase/Log/Math/LogRectangle.hpp>
#include <FslBase/Log/Math/LogThickness.hpp>
#include <FslBase/Log/String/LogUTF8String.hpp>
#include <FslGraphics/Exceptions.hpp>
#include <FslGraphics/TextureAtlas/BasicTextureAtlas.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  constexpr const uint32_t TEST_DP = 320;
  // constexpr const uint32_t DEFAULT_DP = 160;

  using TestTextureAtlas_BasicTextureAtlas = TestFixtureFslGraphics;

  bool CallGetEntry(const BasicTextureAtlas& atlas, const int32_t index)
  {
    const auto& test = atlas.GetEntry(index);
    return test.TextureInfo.Dpi > 0u;
  }
}


TEST(TestTextureAtlas_BasicTextureAtlas, Construct_Default)
{
  BasicTextureAtlas atlas;
  EXPECT_EQ(0u, atlas.Count());
  EXPECT_THROW(CallGetEntry(atlas, 0), std::invalid_argument);
  EXPECT_THROW(atlas.SetEntry(0, PxRectangleU32::Create(2, 2, 3, 3), PxThicknessU::Create(2, 2, 5, 5), 160, "hello"), std::invalid_argument);
}

TEST(TestTexture_BasicTextureAtlas, Reset)
{
  BasicTextureAtlas atlas;
  atlas.Reset(1);
  EXPECT_EQ(1u, atlas.Count());
  {
    const auto& entry0 = atlas.GetEntry(0);
    EXPECT_EQ(UTF8String(), entry0.Name);
    EXPECT_EQ(PxPoint2(), entry0.TextureInfo.OffsetPx);
    EXPECT_EQ(PxExtent2D(), entry0.TextureInfo.ExtentPx);
    EXPECT_EQ(PxThicknessU(), entry0.TextureInfo.TrimMarginPx);
    EXPECT_EQ(PxRectangleU32(), entry0.TextureInfo.TrimmedRectPx);
  }

  atlas.SetEntry(0, PxRectangleU32::Create(4, 6, 8, 12), PxThicknessU::Create(3, 4, 9, 14), TEST_DP, "hello");
  {
    const auto& entry0 = atlas.GetEntry(0);
    EXPECT_EQ(IO::Path("hello"), entry0.Name);
    EXPECT_EQ(PxPoint2::Create(1, 2), entry0.TextureInfo.OffsetPx);
    EXPECT_EQ(PxExtent2D::Create(20, 30), entry0.TextureInfo.ExtentPx);
    EXPECT_EQ(PxThicknessU::Create(3, 4, 9, 14), entry0.TextureInfo.TrimMarginPx);
    EXPECT_EQ(PxRectangleU32::Create(4, 6, 8, 12), entry0.TextureInfo.TrimmedRectPx);
    EXPECT_EQ(TEST_DP, entry0.TextureInfo.Dpi);
  }
}
