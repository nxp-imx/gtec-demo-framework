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

#include <FslBase/Math/Pixel/TypeConverter_Math.hpp>
#include <FslBase/Log/Math/Pixel/LogPxPoint2.hpp>
#include <FslBase/Log/Math/Pixel/LogPxExtent2D.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMathPixel_TypeConverter_Math = TestFixtureFslBase;
}

// --- ToExtent2D

TEST(TestMathPixel_TypeConverter_Math, UncheckedToExtent2D_PxExtent2)
{
  const Extent2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(value.Width, res.Width);
  EXPECT_EQ(value.Height, res.Height);
}


// --- ToPoint2

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPoint2_PxPoint2)
{
  const PxPoint2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<Point2>(value);

  EXPECT_EQ(value.X, res.X);
  EXPECT_EQ(value.Y, res.Y);
}

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPoint2_PxSize2D)
{
  const PxSize2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<Point2>(value);

  EXPECT_EQ(value.Width(), res.X);
  EXPECT_EQ(value.Height(), res.Y);
}

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPoint2_PxExtent2D)
{
  const PxExtent2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<Point2>(value);

  EXPECT_EQ(int32_t(value.Width), res.X);
  EXPECT_EQ(int32_t(value.Height), res.Y);
}

// --- ToPxClipRectangle

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxClipRectangle_Rect)
{
  const Rect value(1.0f, 2.0f, 3.0f, 4.0f);
  const auto res = TypeConverter::UncheckedChangeTo<PxClipRectangle>(value);

  EXPECT_EQ(1, res.Left());
  EXPECT_EQ(2, res.Top());
  EXPECT_EQ(1 + 3, res.Right());
  EXPECT_EQ(2 + 4, res.Bottom());
}

// --- ToPxAreaRectangleF

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxAreaRectangleF_Rect)
{
  const Rect value(1.0f, 2.0f, 3.0f, 4.0f);
  const auto res = TypeConverter::UncheckedTo<PxAreaRectangleF>(value);

  EXPECT_EQ(value.Left(), res.Left());
  EXPECT_EQ(value.Top(), res.Top());
  EXPECT_EQ(value.Right(), res.Right());
  EXPECT_EQ(value.Bottom(), res.Bottom());
}

// --- ToPxExtent2D

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxExtent2D_Extent2D)
{
  const Extent2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(value.Width, res.Width);
  EXPECT_EQ(value.Height, res.Height);
}

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxExtent2D_Point2)
{
  const Point2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxExtent2D>(value);

  EXPECT_EQ(static_cast<PxExtent2D::value_type>(value.X), res.Width);
  EXPECT_EQ(static_cast<PxExtent2D::value_type>(value.Y), res.Height);
}


// --- ToPxPoint2

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxPoint2_Point2)
{
  const Point2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxPoint2>(value);

  EXPECT_EQ(value.X, res.X);
  EXPECT_EQ(value.Y, res.Y);
}

TEST(TestMathPixel_TypeConverter_Math, UncheckedChangeToPxPoint2_Vector2)
{
  {
    const Vector2 value(1, 2);
    const auto res = TypeConverter::UncheckedChangeTo<PxPoint2>(value);

    EXPECT_EQ(1, res.X);
    EXPECT_EQ(2, res.Y);
  }
  {
    const Vector2 value(1.1f, 2.2f);
    const auto res = TypeConverter::UncheckedChangeTo<PxPoint2>(value);

    EXPECT_EQ(1, res.X);
    EXPECT_EQ(2, res.Y);
  }
  {
    const Vector2 value(1.3f, 2.4f);
    const auto res = TypeConverter::UncheckedChangeTo<PxPoint2>(value);

    EXPECT_EQ(1, res.X);
    EXPECT_EQ(2, res.Y);
  }
  {
    const Vector2 value(1.5f, 2.6f);
    const auto res = TypeConverter::UncheckedChangeTo<PxPoint2>(value);

    EXPECT_EQ(2, res.X);
    EXPECT_EQ(3, res.Y);
  }
  {
    const Vector2 value(1.7f, 2.8f);
    const auto res = TypeConverter::UncheckedChangeTo<PxPoint2>(value);

    EXPECT_EQ(2, res.X);
    EXPECT_EQ(3, res.Y);
  }
  {
    const Vector2 value(1.9f, 3.0f);
    const auto res = TypeConverter::UncheckedChangeTo<PxPoint2>(value);

    EXPECT_EQ(2, res.X);
    EXPECT_EQ(3, res.Y);
  }
}

// --- ToPxRectangle

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxRectangle_Rect)
{
  const Rect value(1.0f, 2.0f, 3.0f, 4.0f);
  const auto res = TypeConverter::UncheckedChangeTo<PxRectangle>(value);

  EXPECT_EQ(1, res.Left());
  EXPECT_EQ(2, res.Top());
  EXPECT_EQ(1 + 3, res.Right());
  EXPECT_EQ(2 + 4, res.Bottom());
}

// --- ToPxRectangleU

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxRectangleU_Rectangle)
{
  const Rectangle value(1, 2, 3, 4);
  const auto res = TypeConverter::UncheckedTo<PxRectangleU>(value);

  EXPECT_EQ(uint32_t(value.Left()), res.Left());
  EXPECT_EQ(uint32_t(value.Top()), res.Top());
  EXPECT_EQ(uint32_t(value.Right()), res.Right());
  EXPECT_EQ(uint32_t(value.Bottom()), res.Bottom());
  EXPECT_EQ(uint32_t(value.Width()), res.Width);
  EXPECT_EQ(uint32_t(value.Height()), res.Height);
}

// --- ToPxSize2D

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxSize2D_Point2)
{
  const Point2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<PxSize2D>(value);

  EXPECT_EQ(value.X, res.Width());
  EXPECT_EQ(value.Y, res.Height());
}

TEST(TestMathPixel_TypeConverter_Math, UncheckedToPxSize2D_Vector2)
{
  const Vector2 value(1.4f, 2.5f);
  const auto res = TypeConverter::UncheckedChangeTo<PxSize2D>(value);

  EXPECT_EQ(1, res.Width());
  EXPECT_EQ(3, res.Height());
}


// --- ToVector2

TEST(TestMathPixel_TypeConverter_Math, UncheckedToVector2_PxPoint2)
{
  const PxPoint2 value(1, 2);
  const auto res = TypeConverter::UncheckedTo<Vector2>(value);

  EXPECT_EQ(static_cast<float>(value.X), res.X);
  EXPECT_EQ(static_cast<float>(value.Y), res.Y);
}


TEST(TestMathPixel_TypeConverter_Math, UncheckedToVector2_PxSize2D)
{
  const PxSize2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<Vector2>(value);

  EXPECT_EQ(static_cast<float>(value.Width()), res.X);
  EXPECT_EQ(static_cast<float>(value.Height()), res.Y);
}

TEST(TestMathPixel_TypeConverter_Math, UncheckedToVector2_PxExtent2)
{
  const PxExtent2D value(1, 2);
  const auto res = TypeConverter::UncheckedTo<Vector2>(value);

  EXPECT_EQ(static_cast<float>(value.Width), res.X);
  EXPECT_EQ(static_cast<float>(value.Height), res.Y);
}
