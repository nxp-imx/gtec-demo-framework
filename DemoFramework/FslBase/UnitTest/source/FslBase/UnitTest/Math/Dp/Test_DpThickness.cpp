/****************************************************************************************************************************************************
 * Copyright 2020, 2023-2024 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxThickness.hpp>
#include <FslBase/Math/Pixel/PxThickness.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathDp_PxThickness = TestFixtureFslBase;
}


TEST(TestMathDp_PxThickness, Default)
{
  PxThickness value;

  constexpr auto Size0Px = PxSize1D::Create(0);

  EXPECT_EQ(Size0Px, value.Left());
  EXPECT_EQ(Size0Px, value.Top());
  EXPECT_EQ(Size0Px, value.Right());
  EXPECT_EQ(Size0Px, value.Bottom());
  EXPECT_EQ(Size0Px, value.SumX());
  EXPECT_EQ(Size0Px, value.SumY());
  EXPECT_EQ(PxSize2D(), value.Sum());
}


TEST(TestMathDp_PxThickness, Values)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(Left, value.Left());
  EXPECT_EQ(Top, value.Top());
  EXPECT_EQ(Right, value.Right());
  EXPECT_EQ(Bottom, value.Bottom());
  EXPECT_EQ(Left + Right, value.SumX());
  EXPECT_EQ(Top + Bottom, value.SumY());
  EXPECT_EQ(PxSize2D(Left + Right, Top + Bottom), value.Sum());
}

TEST(TestMathDp_PxThickness, TopLeft)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(PxPoint2(Left, Top), value.TopLeft());
}

TEST(TestMathDp_PxThickness, TopRight)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(PxPoint2(Right, Top), value.TopRight());
}

TEST(TestMathDp_PxThickness, BottomLeft)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(PxPoint2(Left, Bottom), value.BottomLeft());
}

TEST(TestMathDp_PxThickness, BottomRight)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(PxPoint2(Right, Bottom), value.BottomRight());
}

TEST(TestMathDp_PxThickness, Sum)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(PxSize2D(Left + Right, Top + Bottom), value.Sum());
}


TEST(TestMathDp_PxThickness, SumX)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(Left + Right, value.SumX());
}

TEST(TestMathDp_PxThickness, SumY)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value(Left, Top, Right, Bottom);

  EXPECT_EQ(Top + Bottom, value.SumY());
}


TEST(TestMathDp_PxThickness, OperatorEqual)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value1(Left, Top, Right, Bottom);
  PxThickness value2(Left, Top, Right, Bottom);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathDp_PxThickness, OperatorNotEqual)
{
  constexpr const auto Left = PxSize1D::Create(1);
  constexpr const auto Top = PxSize1D::Create(2);
  constexpr const auto Right = PxSize1D::Create(3);
  constexpr const auto Bottom = PxSize1D::Create(4);
  PxThickness value1(Left, Top, Right, Bottom);
  PxThickness value2(Left, Top, Right, PxSize1D::Create(5));

  EXPECT_NE(value1, value2);
}
