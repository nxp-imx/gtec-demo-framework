/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslUtil/Vulkan1_0/UnitTest/Helper/Common.hpp>
#include <FslUtil/Vulkan1_0/UnitTest/Helper/TestFixtureFslUtil_Vulkan1_0.hpp>
#include <FslUtil/Vulkan1_0/VUTextureUtil.hpp>

using namespace Fsl;

namespace
{
  using TestFixtureFslUtil_Vulkan1_0_VUTextureUtil = TestFixtureFslUtil_Vulkan1_0;
}


TEST(TestFixtureFslUtil_Vulkan1_0_VUTextureUtil, CalcTextureArea_One)
{
  TextureRectangle rect(PxRectangle::Create(0, 0, 128, 256), PxSize2D::Create(128, 256));

  auto result = Vulkan::VUTextureUtil::CalcTextureArea(rect, 1, 1);
  EXPECT_EQ(0.0f, result.X0);
  EXPECT_EQ(0.0f, result.Y0);
  EXPECT_EQ(1.0f, result.X1);
  EXPECT_EQ(1.0f, result.Y1);
}

TEST(TestFixtureFslUtil_Vulkan1_0_VUTextureUtil, CalcTextureArea_SameXY)
{
  TextureRectangle rect(PxRectangle::Create(0, 0, 128, 256), PxSize2D::Create(128, 256));

  auto result = Vulkan::VUTextureUtil::CalcTextureArea(rect, 5, 5);
  EXPECT_EQ(0.0f, result.X0);
  EXPECT_EQ(0.0f, result.Y0);
  EXPECT_EQ(5.0f, result.X1);
  EXPECT_EQ(5.0f, result.Y1);
}


TEST(TestFixtureFslUtil_Vulkan1_0_VUTextureUtil, CalcTextureArea_Diff_X_Y)
{
  TextureRectangle rect(PxRectangle::Create(0, 0, 128, 256), PxSize2D::Create(128, 256));

  auto result = Vulkan::VUTextureUtil::CalcTextureArea(rect, 4, 5);
  EXPECT_EQ(0.0f, result.X0);
  EXPECT_EQ(0.0f, result.Y0);
  EXPECT_EQ(4.0f, result.X1);
  EXPECT_EQ(5.0f, result.Y1);
}


TEST(TestFixtureFslUtil_Vulkan1_0_VUTextureUtil, CalcTextureArea_NegativeX_Y)
{
  TextureRectangle rect(PxRectangle::Create(0, 0, 128, 256), PxSize2D::Create(128, 256));

  auto result = Vulkan::VUTextureUtil::CalcTextureArea(rect, -4, 5);
  EXPECT_EQ(0.0f, result.X0);
  EXPECT_EQ(0.0f, result.Y0);
  EXPECT_EQ(-4.0f, result.X1);
  EXPECT_EQ(5.0f, result.Y1);
}


TEST(TestFixtureFslUtil_Vulkan1_0_VUTextureUtil, CalcTextureArea_NegativeX_NegativeY)
{
  TextureRectangle rect(PxRectangle::Create(0, 0, 128, 256), PxSize2D::Create(128, 256));

  auto result = Vulkan::VUTextureUtil::CalcTextureArea(rect, 4, -5);
  EXPECT_EQ(0.0f, result.X0);
  EXPECT_EQ(0.0f, result.Y0);
  EXPECT_EQ(4.0f, result.X1);
  EXPECT_EQ(-5.0f, result.Y1);
}


TEST(TestFixtureFslUtil_Vulkan1_0_VUTextureUtil, CalcTextureArea_NonRepeatable)
{
  TextureRectangle rect(PxRectangle::Create(50, 100, 200, 400), PxSize2D::Create(400, 800));

  auto result = Vulkan::VUTextureUtil::CalcTextureArea(rect, 1, 1);
  EXPECT_FLOAT_EQ((50.0f / 400.0f), result.X0);
  EXPECT_FLOAT_EQ((100.0f / 800.0f), result.Y0);
  EXPECT_FLOAT_EQ((250.0f / 400.0f), result.X1);
  EXPECT_FLOAT_EQ((500.0f / 800.0f), result.Y1);
}
