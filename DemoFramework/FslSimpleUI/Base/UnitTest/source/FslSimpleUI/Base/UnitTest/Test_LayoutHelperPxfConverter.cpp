/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslSimpleUI/Base/LayoutHelperPxfConverter.hpp>
#include <FslSimpleUI/Base/PxAvailableSizeUtil.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using Test_DPHelper = TestFixtureFslBase;
}


TEST(Test_LayoutHelperPxfConverter, PxfToPxInt32)
{
  EXPECT_EQ(0, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.0f));
  EXPECT_EQ(0, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.1f));
  EXPECT_EQ(0, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.2f));
  EXPECT_EQ(0, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.3f));
  EXPECT_EQ(0, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.4f));
  EXPECT_EQ(1, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.5f));
  EXPECT_EQ(1, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.6f));
  EXPECT_EQ(1, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.7f));
  EXPECT_EQ(1, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.8f));
  EXPECT_EQ(1, UI::LayoutHelperPxfConverter::PxfToPxInt32(0.9f));
  EXPECT_EQ(1, UI::LayoutHelperPxfConverter::PxfToPxInt32(1.0f));
}


TEST(Test_LayoutHelperPxfConverter, PxfToPxInt32_Infinity)
{
  EXPECT_EQ(UI::PxAvailableSizeUtil::InfiniteSpacePx, UI::LayoutHelperPxfConverter::PxfToPxInt32(std::numeric_limits<float>::infinity()));
}


// TEST(Test_LayoutHelperPxfConverter, PxfToPxInt32_NaN)
//{
//  EXPECT_EQ(UI::LayoutHelper::NaNPx, UI::LayoutHelperPxfConverter::PxfToPxInt32(std::numeric_limits<float>::quiet_NaN()));
//}


TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat)
{
  EXPECT_EQ(-10.0f, UI::LayoutHelperPxfConverter::PxToPxfFloat(-10));
  EXPECT_EQ(0.0f, UI::LayoutHelperPxfConverter::PxToPxfFloat(0));
  EXPECT_EQ(10.0f, UI::LayoutHelperPxfConverter::PxToPxfFloat(10));
}


TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat_Infinity)
{
  EXPECT_TRUE(std::isinf(UI::LayoutHelperPxfConverter::PxToPxfFloat(UI::PxAvailableSizeUtil::InfiniteSpacePx)));
}


TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat_InfinityBegin)
{
  EXPECT_TRUE(std::isinf(UI::LayoutHelperPxfConverter::PxToPxfFloat(UI::PxAvailableSizeUtil::InfiniteSpaceBeginPx)));
}


TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat_InfinityEnd)
{
  EXPECT_TRUE(std::isinf(UI::LayoutHelperPxfConverter::PxToPxfFloat(UI::PxAvailableSizeUtil::InfiniteSpaceEndPx)));
}


// TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat_NaN)
//{
//  EXPECT_TRUE(std::isnan(UI::LayoutHelperPxfConverter::PxToPxfFloat(UI::PxAvailableSizeUtil::NaNPx)));
//}
//
//
// TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat_NaN_Begin)
//{
//  EXPECT_TRUE(std::isnan(UI::LayoutHelperPxfConverter::PxToPxfFloat(UI::PxAvailableSizeUtil::NaNBeginPx)));
//}
//
//
// TEST(Test_LayoutHelperPxfConverter, PxToPxfFloat_NaN_End)
//{
//  EXPECT_TRUE(std::isnan(UI::LayoutHelperPxfConverter::PxToPxfFloat(UI::PxAvailableSizeUtil::NaNEndPx)));
//}


TEST(Test_LayoutHelperPxfConverter, PxfToPxPoint2_Vector2)
{
  EXPECT_EQ(PxPoint2::Create(-10, 10), UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(-10, 10)));
  EXPECT_EQ(PxPoint2::Create(0, 1), UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(0, 1)));
  EXPECT_EQ(PxPoint2::Create(10, -10), UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(10, -10)));
}

TEST(Test_LayoutHelperPxfConverter, PxfToPxPoint2_Vector2_Infinity)
{
  EXPECT_EQ(PxPoint2::Create(UI::PxAvailableSizeUtil::InfiniteSpacePx, 10),
            UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(std::numeric_limits<float>::infinity(), 10)));
  EXPECT_EQ(PxPoint2::Create(10, UI ::PxAvailableSizeUtil::InfiniteSpacePx),
            UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(10, std::numeric_limits<float>::infinity())));
}

// TEST(Test_LayoutHelperPxfConverter, PxfToPxPoint2_Vector2_NaN)
//{
//  EXPECT_EQ(PxPoint2(UI::PxAvailableSizeUtil::NaNPx, 10),
//  UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(std::numeric_limits<float>::quiet_NaN(), 10))); EXPECT_EQ(PxPoint2(10,
//  UI::PxAvailableSizeUtil::NaNPx), UI::LayoutHelperPxfConverter::PxfToPxPoint2(Vector2(10, std::numeric_limits<float>::quiet_NaN())));
//}


TEST(Test_LayoutHelperPxfConverter, PxToPxfVector2_PxPoint2)
{
  EXPECT_EQ(Vector2(-10, 10), UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2::Create(-10, 10)));
  EXPECT_EQ(Vector2(0, 1), UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2::Create(0, 1)));
  EXPECT_EQ(Vector2(10, -10), UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2::Create(10, -10)));
}

TEST(Test_LayoutHelperPxfConverter, PxToPxfVector2_PxPoint2_Infinity)
{
  auto val0 = UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2::Create(UI::PxAvailableSizeUtil::InfiniteSpacePx, 10));
  auto val1 = UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2::Create(10, UI ::PxAvailableSizeUtil::InfiniteSpacePx));

  EXPECT_TRUE(std::isinf(val0.X));
  EXPECT_EQ(10.0f, val0.Y);

  EXPECT_EQ(10.0f, val1.X);
  EXPECT_TRUE(std::isinf(val1.Y));
}

// TEST(Test_LayoutHelperPxfConverter, PxToPxfVector2_PxPoint2_NaN)
//{
//  auto val0 = UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2(UI::PxAvailableSizeUtil::NaNPx, 10));
//  auto val1 = UI::LayoutHelperPxfConverter::PxToPxfVector2(PxPoint2(10, UI ::PxAvailableSizeUtil::NaNPx));
//
//  EXPECT_TRUE(std::isnan(val0.X));
//  EXPECT_EQ(10.0f, val0.Y);
//
//  EXPECT_EQ(10.0f, val1.X);
//  EXPECT_TRUE(std::isnan(val1.Y));
//}

TEST(Test_LayoutHelperPxfConverter, PxfToPxRectangle_Rect)
{
  EXPECT_EQ(PxRectangle::Create(1, 2, 3, 4), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.0f, 2.0f, 3.0f, 4.0f)));
  EXPECT_EQ(PxRectangle::Create(1, 2, 3, 4), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.1f, 2.1f, 3.1f, 4.1f)));
  EXPECT_EQ(PxRectangle::Create(1, 2, 3, 4), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.2f, 2.2f, 3.2f, 4.2f)));
  EXPECT_EQ(PxRectangle::Create(1, 2, 3, 4), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.3f, 2.3f, 3.3f, 4.3f)));
  EXPECT_EQ(PxRectangle::Create(1, 2, 3, 4), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.4f, 2.4f, 3.4f, 4.4f)));
  EXPECT_EQ(PxRectangle::Create(2, 3, 4, 5), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.5f, 2.5f, 3.5f, 4.5f)));
  EXPECT_EQ(PxRectangle::Create(2, 3, 4, 5), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.6f, 2.6f, 3.6f, 4.6f)));
  EXPECT_EQ(PxRectangle::Create(2, 3, 4, 5), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.7f, 2.7f, 3.7f, 4.7f)));
  EXPECT_EQ(PxRectangle::Create(2, 3, 4, 5), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.8f, 2.8f, 3.8f, 4.8f)));
  EXPECT_EQ(PxRectangle::Create(2, 3, 4, 5), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(1.9f, 2.9f, 3.9f, 4.9f)));
  EXPECT_EQ(PxRectangle::Create(2, 3, 4, 5), UI::LayoutHelperPxfConverter::PxfToPxRectangle(Rect(2.0f, 3.0f, 4.0f, 5.0f)));
}
