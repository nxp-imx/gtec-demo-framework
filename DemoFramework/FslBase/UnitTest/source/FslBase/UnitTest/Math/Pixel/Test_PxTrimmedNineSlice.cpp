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
  constexpr auto SizePx = PxSize2D::Create(1, 2);
  constexpr auto TrimMarginPxf = PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr auto TrimmedNineSlicePxf = PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr auto ContentMarginPx = PxThickness::Create(11, 12, 13, 14);

  PxTrimmedNineSlice value(SizePx, TrimMarginPxf, TrimmedNineSlicePxf, ContentMarginPx);

  EXPECT_EQ(SizePx, value.SizePx);
  EXPECT_EQ(TrimMarginPxf, value.TrimMarginPxf);
  EXPECT_EQ(TrimmedNineSlicePxf, value.TrimmedNineSlicePxf);
  EXPECT_EQ(ContentMarginPx, value.ContentMarginPx);
}

TEST(TestMathPixel_PxTrimmedNineSlice, OpEqual)
{
  constexpr auto SizePx = PxSize2D::Create(1, 2);
  constexpr auto TrimMarginPxf = PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr auto TrimmedNineSlicePxf = PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr auto ContentMarginPx = PxThickness::Create(11, 12, 13, 14);

  PxTrimmedNineSlice value0(SizePx, TrimMarginPxf, TrimmedNineSlicePxf, ContentMarginPx);
  PxTrimmedNineSlice value1(SizePx, TrimMarginPxf, TrimmedNineSlicePxf, ContentMarginPx);

  EXPECT_EQ(value0, value1);
}

TEST(TestMathPixel_PxTrimmedNineSlice, NotOpEqual)
{
  constexpr auto SizePx = PxSize2D::Create(1, 2);
  constexpr auto TrimMarginPxf = PxThicknessF::Create(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr auto TrimmedNineSlicePxf = PxThicknessF::Create(7.0f, 8.0f, 9.0f, 10.0f);
  constexpr auto ContentMarginPx = PxThickness::Create(11, 12, 13, 14);

  PxTrimmedNineSlice value(SizePx, TrimMarginPxf, TrimmedNineSlicePxf, ContentMarginPx);

  constexpr auto Size42Px = PxSize1D::Create(42);
  constexpr auto Size42Pxf = PxSize1DF::Create(42);

  EXPECT_NE(PxTrimmedNineSlice(PxSize2D(Size42Px, SizePx.Height()), TrimMarginPxf, TrimmedNineSlicePxf, ContentMarginPx), value);
  EXPECT_NE(PxTrimmedNineSlice(PxSize2D(SizePx.Width(), Size42Px), TrimMarginPxf, TrimmedNineSlicePxf, ContentMarginPx), value);

  EXPECT_NE(PxTrimmedNineSlice(SizePx, PxThicknessF(Size42Pxf, TrimMarginPxf.Top(), TrimMarginPxf.Right(), TrimMarginPxf.Bottom()),
                               TrimmedNineSlicePxf, ContentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, PxThicknessF(TrimMarginPxf.Left(), Size42Pxf, TrimMarginPxf.Right(), TrimMarginPxf.Bottom()),
                               TrimmedNineSlicePxf, ContentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, PxThicknessF(TrimMarginPxf.Left(), TrimMarginPxf.Top(), Size42Pxf, TrimMarginPxf.Bottom()),
                               TrimmedNineSlicePxf, ContentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, PxThicknessF(TrimMarginPxf.Left(), TrimMarginPxf.Top(), TrimMarginPxf.Right(), Size42Pxf), TrimmedNineSlicePxf,
                               ContentMarginPx),
            value);

  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf,
                               PxThicknessF(Size42Pxf, TrimmedNineSlicePxf.Top(), TrimmedNineSlicePxf.Right(), TrimmedNineSlicePxf.Bottom()),
                               ContentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf,
                               PxThicknessF(TrimmedNineSlicePxf.Left(), Size42Pxf, TrimmedNineSlicePxf.Right(), TrimmedNineSlicePxf.Bottom()),
                               ContentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf,
                               PxThicknessF(TrimmedNineSlicePxf.Left(), TrimmedNineSlicePxf.Top(), Size42Pxf, TrimmedNineSlicePxf.Bottom()),
                               ContentMarginPx),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf,
                               PxThicknessF(TrimmedNineSlicePxf.Left(), TrimmedNineSlicePxf.Top(), TrimmedNineSlicePxf.Right(), Size42Pxf),
                               ContentMarginPx),
            value);

  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf, TrimmedNineSlicePxf,
                               PxThickness(Size42Px, ContentMarginPx.Top(), ContentMarginPx.Right(), ContentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf, TrimmedNineSlicePxf,
                               PxThickness(ContentMarginPx.Left(), Size42Px, ContentMarginPx.Right(), ContentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf, TrimmedNineSlicePxf,
                               PxThickness(ContentMarginPx.Left(), ContentMarginPx.Top(), Size42Px, ContentMarginPx.Bottom())),
            value);
  EXPECT_NE(PxTrimmedNineSlice(SizePx, TrimMarginPxf, TrimmedNineSlicePxf,
                               PxThickness(ContentMarginPx.Left(), ContentMarginPx.Top(), ContentMarginPx.Right(), Size42Px)),
            value);
}
