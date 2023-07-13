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

#include <FslBase/Log/Math/Pixel/LogPxTrimmedNineSlice.hpp>
#include <FslBase/Math/Pixel/PxTrimmedNineSlice.hpp>
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
  constexpr auto sizePx = PxSize2D::Create(1, 2);
  constexpr auto trimMarginPxf = PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr auto trimmedNineSlicePxf = PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr auto contentMarginPx = PxThickness::Create(11, 12, 13, 14);

  PxTrimmedNineSlice value(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);

  EXPECT_EQ(sizePx, value.SizePx);
  EXPECT_EQ(trimMarginPxf, value.TrimMarginPxf);
  EXPECT_EQ(trimmedNineSlicePxf, value.TrimmedNineSlicePxf);
  EXPECT_EQ(contentMarginPx, value.ContentMarginPx);
}

TEST(TestMathPixel_PxTrimmedNineSlice, OpEqual)
{
  constexpr auto sizePx = PxSize2D::Create(1, 2);
  constexpr auto trimMarginPxf = PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr auto trimmedNineSlicePxf = PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr auto contentMarginPx = PxThickness::Create(11, 12, 13, 14);

  PxTrimmedNineSlice value0(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);
  PxTrimmedNineSlice value1(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);

  EXPECT_EQ(value0, value1);
}

TEST(TestMathPixel_PxTrimmedNineSlice, NotOpEqual)
{
  constexpr auto sizePx = PxSize2D::Create(1, 2);
  constexpr auto trimMarginPxf = PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr auto trimmedNineSlicePxf = PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr auto contentMarginPx = PxThickness::Create(11, 12, 13, 14);

  PxTrimmedNineSlice value(sizePx, trimMarginPxf, trimmedNineSlicePxf, contentMarginPx);

  constexpr auto size42Px = PxSize1D::Create(42);
  constexpr auto size42Pxf = PxSize1DF::Create(42);

  EXPECT_NE(PxTrimmedNineSlice(PxSize2D(size42Px, sizePx.Height()), trimMarginPxf, trimmedNineSlicePxf, contentMarginPx), value);
  EXPECT_NE(PxTrimmedNineSlice(PxSize2D(sizePx.Width(), size42Px), trimMarginPxf, trimmedNineSlicePxf, contentMarginPx), value);

  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(size42Pxf, trimMarginPxf.Top(), trimMarginPxf.Right(), trimMarginPxf.Bottom()),
                               trimmedNineSlicePxf, contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(trimMarginPxf.Left(), size42Pxf, trimMarginPxf.Right(), trimMarginPxf.Bottom()),
                               trimmedNineSlicePxf, contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(trimMarginPxf.Left(), trimMarginPxf.Top(), size42Pxf, trimMarginPxf.Bottom()),
                               trimmedNineSlicePxf, contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, PxThicknessF(trimMarginPxf.Left(), trimMarginPxf.Top(), trimMarginPxf.Right(), size42Pxf), trimmedNineSlicePxf,
                               contentMarginPx),
            value);

  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(size42Pxf, trimmedNineSlicePxf.Top(), trimmedNineSlicePxf.Right(), trimmedNineSlicePxf.Bottom()),
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(trimmedNineSlicePxf.Left(), size42Pxf, trimmedNineSlicePxf.Right(), trimmedNineSlicePxf.Bottom()),
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(trimmedNineSlicePxf.Left(), trimmedNineSlicePxf.Top(), size42Pxf, trimmedNineSlicePxf.Bottom()),
                               contentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf,
                               PxThicknessF(trimmedNineSlicePxf.Left(), trimmedNineSlicePxf.Top(), trimmedNineSlicePxf.Right(), size42Pxf),
                               contentMarginPx),
            value);

  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(size42Px, contentMarginPx.Top(), contentMarginPx.Right(), contentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(contentMarginPx.Left(), size42Px, contentMarginPx.Right(), contentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(contentMarginPx.Left(), contentMarginPx.Top(), size42Px, contentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(sizePx, trimMarginPxf, trimmedNineSlicePxf,
                               PxThickness(contentMarginPx.Left(), contentMarginPx.Top(), contentMarginPx.Right(), size42Px)),
            value);
}
