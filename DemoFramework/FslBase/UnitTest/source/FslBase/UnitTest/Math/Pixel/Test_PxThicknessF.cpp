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

#include <FslBase/Log/Math/Pixel/LogPxThicknessF.hpp>
#include <FslBase/Math/Pixel/PxThicknessF.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxThicknessF = TestFixtureFslBase;
}


TEST(TestMathPixel_PxThicknessF, Default)
{
  PxThicknessF value;
  constexpr auto size0Pxf = PxSize1DF::Create(0);

  EXPECT_EQ(size0Pxf, value.Left());
  EXPECT_EQ(size0Pxf, value.Top());
  EXPECT_EQ(size0Pxf, value.Right());
  EXPECT_EQ(size0Pxf, value.Bottom());
  EXPECT_EQ(size0Pxf, value.SumX());
  EXPECT_EQ(size0Pxf, value.SumY());
  EXPECT_EQ(PxSize2DF(), value.Sum());
}


TEST(TestMathPixel_PxThicknessF, Values)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_EQ(left, value.Left());
  EXPECT_EQ(top, value.Top());
  EXPECT_EQ(right, value.Right());
  EXPECT_EQ(bottom, value.Bottom());
  EXPECT_FLOAT_EQ((left + right).RawValue(), value.SumX().RawValue());
  EXPECT_FLOAT_EQ((top + bottom).RawValue(), value.SumY().RawValue());
  EXPECT_FLOAT_EQ((left + right).RawValue(), value.Sum().RawWidth());
  EXPECT_FLOAT_EQ((top + bottom).RawValue(), value.Sum().RawHeight());
}

TEST(TestMathPixel_PxThicknessF, TopLeft)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_EQ(PxVector2(left, top), value.TopLeft());
}

TEST(TestMathPixel_PxThicknessF, TopRight)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_EQ(PxVector2(right, top), value.TopRight());
}

TEST(TestMathPixel_PxThicknessF, BottomLeft)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_EQ(PxVector2(left, bottom), value.BottomLeft());
}

TEST(TestMathPixel_PxThicknessF, BottomRight)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_EQ(PxVector2(right, bottom), value.BottomRight());
}

TEST(TestMathPixel_PxThicknessF, Sum)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_FLOAT_EQ((left + right).RawValue(), value.Sum().RawWidth());
  EXPECT_FLOAT_EQ((top + bottom).RawValue(), value.Sum().RawHeight());
}


TEST(TestMathPixel_PxThicknessF, SumX)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_FLOAT_EQ((left + right).RawValue(), value.SumX().RawValue());
}

TEST(TestMathPixel_PxThicknessF, SumY)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value(left, top, right, bottom);

  EXPECT_FLOAT_EQ((top + bottom).RawValue(), value.SumY().RawValue());
}


TEST(TestMathPixel_PxThicknessF, OperatorEqual)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value1(left, top, right, bottom);
  PxThicknessF value2(left, top, right, bottom);

  EXPECT_EQ(value1, value2);
}


TEST(TestMathPixel_PxThicknessF, OperatorNotEqual)
{
  constexpr const auto left = PxSize1DF::Create(1);
  constexpr const auto top = PxSize1DF::Create(2);
  constexpr const auto right = PxSize1DF::Create(3);
  constexpr const auto bottom = PxSize1DF::Create(4);
  PxThicknessF value1(left, top, right, bottom);
  PxThicknessF value2(left, top, right, PxSize1DF::Create(5));

  EXPECT_NE(value1, value2);
}
