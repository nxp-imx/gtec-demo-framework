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

#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/Log/Math/Pixel/LogPxPoint2.hpp>
#include <FslBase/Math/Pixel/TypeConverter.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_TypeConverter = TestFixtureFslBase;
}

// --- ToPxInt32


// --- ToPxClipRectangle

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangle)
{
  const PxRectangle value(1, 2, 3, 4);

  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Bottom()), res.Bottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangle2D)
{
  const PxRectangle2D value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Bottom()), res.Bottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangleU16)
{
  const PxRectangleU16 value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Bottom()), res.Bottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangleU32)
{
  const PxRectangleU32 value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxClipRectangle::value_type>(value.Bottom()), res.Bottom());
}

// --- ToPxAreaRectangleF

TEST(TestMathPixel_TypeConverter, UncheckedToPxAreaRectangleF_PxRectangle2D)
{
  const PxRectangle2D value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Bottom()), res.Bottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxAreaRectangleF_PxRectangleU16)
{
  const PxRectangleU16 value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Bottom()), res.Bottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxAreaRectangleF_PxRectangleU32)
{
  const PxRectangleU32 value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Left()), res.Left());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Top()), res.Top());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Right()), res.Right());
  EXPECT_EQ(static_cast<PxAreaRectangleF::value_type>(value.Bottom()), res.Bottom());
}

// --- ToPxExtent2D

TEST(TestMathPixel_TypeConverter, UncheckedToPxExtent2D_PxPoint2)
{
  const PxPoint2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(static_cast<PxExtent2D::value_type>(value.X), res.Width);
  EXPECT_EQ(static_cast<PxExtent2D::value_type>(value.Y), res.Height);
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxExtent2D_PxSize2D)
{
  const PxSize2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(static_cast<PxExtent2D::value_type>(value.Width()), res.Width);
  EXPECT_EQ(static_cast<PxExtent2D::value_type>(value.Height()), res.Height);
}

// --- ToPxPoint2

TEST(TestMathPixel_TypeConverter, UncheckedToPxPoint2_PxExtent2D)
{
  const PxExtent2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxPoint2>(value);

  EXPECT_EQ(static_cast<PxPoint2::value_type>(value.Width), res.X);
  EXPECT_EQ(static_cast<PxPoint2::value_type>(value.Height), res.Y);
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxPoint2_PxSize2D)
{
  const PxSize2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxPoint2>(value);

  EXPECT_EQ(static_cast<PxPoint2::value_type>(value.Width()), res.X);
  EXPECT_EQ(static_cast<PxPoint2::value_type>(value.Height()), res.Y);
}

// --- ToPxSize2D

TEST(TestMathPixel_TypeConverter, UncheckedToPxSize2D_PxPoint2)
{
  const PxPoint2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxSize2D>(value);

  EXPECT_EQ(static_cast<PxSize2D::value_type>(value.X), res.Width());
  EXPECT_EQ(static_cast<PxSize2D::value_type>(value.Y), res.Height());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxSize2D_PxExtent2D)
{
  const PxExtent2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxSize2D>(value);

  EXPECT_EQ(static_cast<PxSize2D::value_type>(value.Width), res.Width());
  EXPECT_EQ(static_cast<PxSize2D::value_type>(value.Height), res.Height());
}
