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
  const auto value = PxRectangle::Create(1, 2, 3, 4);

  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawBottom()), res.RawBottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangle2D)
{
  const auto value = PxRectangle2D::Create(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawBottom()), res.RawBottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangleU16)
{
  const auto value = PxRectangleU16::Create(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawBottom()), res.RawBottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxClipRectangle_PxRectangleU32)
{
  const auto value = PxRectangleU32::Create(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxClipRectangle>(value);

  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxClipRectangle::raw_value_type>(value.RawBottom()), res.RawBottom());
}

// --- ToPxAreaRectangleF

TEST(TestMathPixel_TypeConverter, UncheckedToPxAreaRectangleF_PxRectangle2D)
{
  const auto value = PxRectangle2D::Create(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawBottom()), res.RawBottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxAreaRectangleF_PxRectangleU16)
{
  const auto value = PxRectangleU16::Create(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawBottom()), res.RawBottom());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxAreaRectangleF_PxRectangleU32)
{
  const auto value = PxRectangleU32::Create(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawLeft()), res.RawLeft());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawTop()), res.RawTop());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawRight()), res.RawRight());
  EXPECT_EQ(static_cast<PxAreaRectangleF::raw_value_type>(value.RawBottom()), res.RawBottom());
}

// --- ToPxExtent2D

TEST(TestMathPixel_TypeConverter, UncheckedToPxExtent2D_PxPoint2)
{
  const auto value = PxPoint2::Create(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(static_cast<PxExtent2D::raw_value_type>(value.X.Value), res.Width.Value);
  EXPECT_EQ(static_cast<PxExtent2D::raw_value_type>(value.Y.Value), res.Height.Value);
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxExtent2D_PxSize2D)
{
  const auto value = PxSize2D::Create(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(static_cast<PxExtent2D::raw_value_type>(value.RawWidth()), res.Width.Value);
  EXPECT_EQ(static_cast<PxExtent2D::raw_value_type>(value.RawHeight()), res.Height.Value);
}

// --- ToPxPoint2

TEST(TestMathPixel_TypeConverter, UncheckedToPxPoint2_PxExtent2D)
{
  const auto value = PxExtent2D::Create(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxPoint2>(value);

  EXPECT_EQ(static_cast<PxPoint2::raw_value_type>(value.Width.Value), res.X.Value);
  EXPECT_EQ(static_cast<PxPoint2::raw_value_type>(value.Height.Value), res.Y.Value);
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxPoint2_PxSize2D)
{
  const auto value = PxSize2D::Create(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxPoint2>(value);

  EXPECT_EQ(static_cast<PxPoint2::raw_value_type>(value.RawWidth()), res.X.Value);
  EXPECT_EQ(static_cast<PxPoint2::raw_value_type>(value.RawHeight()), res.Y.Value);
}

// --- ToPxSize2D

TEST(TestMathPixel_TypeConverter, UncheckedToPxSize2D_PxPoint2)
{
  const auto value = PxPoint2::Create(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxSize2D>(value);

  EXPECT_EQ(static_cast<PxSize2D::raw_value_type>(value.X.Value), res.RawWidth());
  EXPECT_EQ(static_cast<PxSize2D::raw_value_type>(value.Y.Value), res.RawHeight());
}

TEST(TestMathPixel_TypeConverter, UncheckedToPxSize2D_PxExtent2D)
{
  const auto value = PxExtent2D::Create(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxSize2D>(value);

  EXPECT_EQ(static_cast<PxSize2D::raw_value_type>(value.Width.Value), res.RawWidth());
  EXPECT_EQ(static_cast<PxSize2D::raw_value_type>(value.Height.Value), res.RawHeight());
}
