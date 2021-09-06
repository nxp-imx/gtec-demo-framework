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

#include <FslGraphics/TextureAtlas/AtlasTextureInfoUtil.hpp>
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxRectangleU32.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  constexpr const uint32_t TEST_DP = 320;
  using Test_AtlasTextureInfoUtil = TestFixtureFslGraphics;
}


TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_LeftOfTrim)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(0, 0, 8, 15);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_FALSE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(0u, srcRect.X);
  EXPECT_EQ(0u, srcRect.Y);
  EXPECT_EQ(0u, srcRect.Width);
  EXPECT_EQ(0u, srcRect.Height);
  EXPECT_EQ(Vector2(0, 0), srcOrigin);
}


TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_RightOfTrim)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(11, 0, 7, 15);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_FALSE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(0u, srcRect.X);
  EXPECT_EQ(0u, srcRect.Y);
  EXPECT_EQ(0u, srcRect.Width);
  EXPECT_EQ(0u, srcRect.Height);
  EXPECT_EQ(Vector2(0, 0), srcOrigin);
}


TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_TopOfTrim)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(0, 0, 18, 6);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_FALSE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(0u, srcRect.X);
  EXPECT_EQ(0u, srcRect.Y);
  EXPECT_EQ(0u, srcRect.Width);
  EXPECT_EQ(0u, srcRect.Height);
  EXPECT_EQ(Vector2(0, 0), srcOrigin);
}


TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_BottomOfTrim)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(0, 10, 18, 5);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_FALSE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(0u, srcRect.X);
  EXPECT_EQ(0u, srcRect.Y);
  EXPECT_EQ(0u, srcRect.Width);
  EXPECT_EQ(0u, srcRect.Height);
  EXPECT_EQ(Vector2(0, 0), srcOrigin);
}


TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_FullSrc)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(0, 0, 18, 15);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_TRUE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(1u, srcRect.X);
  EXPECT_EQ(2u, srcRect.Y);
  EXPECT_EQ(3u, srcRect.Width);
  EXPECT_EQ(4u, srcRect.Height);
  EXPECT_EQ(Vector2(-8, -6), srcOrigin);
}


TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_FullyInsideTrim)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(8, 6, 2, 3);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_TRUE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(1u, srcRect.X);
  EXPECT_EQ(2u, srcRect.Y);
  EXPECT_EQ(2u, srcRect.Width);
  EXPECT_EQ(3u, srcRect.Height);
  EXPECT_EQ(Vector2(0, 0), srcOrigin);
}

TEST(Test_AtlasTextureInfoUtil, AdjustSourceRect_Trim_FullTrim)
{
  // src rect(-7, -4, 18, 15) on texture
  AtlasTextureInfo atlasTextureInfo(PxRectangleU32(1, 2, 3, 4), PxThicknessU(8, 6, 7, 5), TEST_DP);

  PxRectangleU32 srcRectOrg(8, 6, 3, 4);

  Vector2 srcOrigin;
  auto srcRect = srcRectOrg;
  EXPECT_TRUE(AtlasTextureInfoUtil::AdjustSourceRect(srcRect, atlasTextureInfo, srcOrigin));

  EXPECT_EQ(1u, srcRect.X);
  EXPECT_EQ(2u, srcRect.Y);
  EXPECT_EQ(3u, srcRect.Width);
  EXPECT_EQ(4u, srcRect.Height);
  EXPECT_EQ(Vector2(0, 0), srcOrigin);
}
