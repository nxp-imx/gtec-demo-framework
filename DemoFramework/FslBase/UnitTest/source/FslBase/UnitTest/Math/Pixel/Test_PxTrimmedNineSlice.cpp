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

#include <FslBase/Math/Pixel/PxTrimmedNineSlice.hpp>
#include <FslBase/Log/Math/Pixel/LogPxTrimmedNineSlice.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxTrimmedNineSlice = TestFixtureFslBase;
}


TEST(TestMathPixel_PxTrimmedNineSlice, Construct_Default)
{
  PxTrimmedNineSlice value;

  EXPECT_EQ(PxSize2D(), value.SizePx);
  EXPECT_EQ(PxThicknessF(), value.TrimMarginPxf);
  EXPECT_EQ(PxThicknessF(), value.TrimmedNineSlicePxf);
  EXPECT_EQ(PxThickness(), value.ContentMarginPx);
}

TEST(TestMathPixel_PxTrimmedNineSlice, Construct)
{
  constexpr PxSize2D sizePx(1, 2);
  constexpr PxThicknessF trimMarginPxf(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr PxThicknessF trimmedNineSlicePxf(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr PxThickness contentMarginPx(11, 12, 13, 14);

  PxTrimmedNineSlice value(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);

  EXPECT_EQ(sizePx, value.SizePx);
  EXPECT_EQ(trimMarginPxf, value.TrimMarginPxf);
  EXPECT_EQ(trimmedNineSlicePxf, value.TrimmedNineSlicePxf);
  EXPECT_EQ(contentMarginPx, value.ContentMarginPx);
}

TEST(TestMathPixel_PxTrimmedNineSlice, OpEqual)
{
  constexpr PxSize2D sizePx(1, 2);
  constexpr PxThicknessF trimMarginPxf(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr PxThicknessF trimmedNineSlicePxf(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr PxThickness contentMarginPx(11, 12, 13, 14);

  PxTrimmedNineSlice value0(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);
  PxTrimmedNineSlice value1(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);

  EXPECT_EQ(value0, value1);
}

TEST(TestMathPixel_PxTrimmedNineSlice, NotOpEqual)
{
  constexpr PxSize2D sizePx(1, 2);
  constexpr PxThicknessF trimMarginPxf(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr PxThicknessF trimmedNineSlicePxf(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr PxThickness contentMarginPx(11, 12, 13, 14);

  PxTrimmedNineSlice value(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);

  EXPECT_NE(PxTrimmedNineSlice(PxSize2D(42, sizePx.Height()), trimMarginPxf, trimmedNineSlicePxf, contentMarginPx), value);
  EXPECT_NE(PxTrimmedNineSlice(PxSize2D(sizePx.Width(), 42), trimMarginPxf, trimmedNineSlicePxf, contentMarginPx), value);

  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(42, trimMarginPxf.Top(), trimMarginPxf.Right(), trimMarginPxf.Bottom()), trimmedNineSlicePxf,
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(trimMarginPxf.Left(), 42, trimMarginPxf.Right(), trimMarginPxf.Bottom()), trimmedNineSlicePxf,
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(trimMarginPxf.Left(), trimMarginPxf.Top(), 42, trimMarginPxf.Bottom()), trimmedNineSlicePxf,
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(trimMarginPxf.Left(), trimMarginPxf.Top(), trimMarginPxf.Right(), 42), trimmedNineSlicePxf,
                               contentMarginPx),
            value);

  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(42, trimmedNineSlicePxf.Top(), trimmedNineSlicePxf.Right(), trimmedNineSlicePxf.Bottom()),
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(trimmedNineSlicePxf.Left(), 42, trimmedNineSlicePxf.Right(), trimmedNineSlicePxf.Bottom()),
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(trimmedNineSlicePxf.Left(), trimmedNineSlicePxf.Top(), 42, trimmedNineSlicePxf.Bottom()),
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(trimmedNineSlicePxf.Left(), trimmedNineSlicePxf.Top(), trimmedNineSlicePxf.Right(), 42), contentMarginPx),
            value);

  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(42, contentMarginPx.Top(), contentMarginPx.Right(), contentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(contentMarginPx.Left(), 42, contentMarginPx.Right(), contentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(contentMarginPx.Left(), contentMarginPx.Top(), 42, contentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(contentMarginPx.Left(), contentMarginPx.Top(), contentMarginPx.Right(), 42)),
            value);
}
