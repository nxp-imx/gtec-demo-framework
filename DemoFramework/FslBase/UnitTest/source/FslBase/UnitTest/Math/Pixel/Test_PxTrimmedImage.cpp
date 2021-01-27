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

#include <FslBase/Math/Pixel/PxTrimmedImage.hpp>
#include <FslBase/Log/Math/Pixel/LogPxTrimmedImage.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxTrimmedImage = TestFixtureFslBase;
}


TEST(TestMathPixel_PxTrimmedImage, Construct_Default)
{
  PxTrimmedImage value;

  EXPECT_EQ(PxSize2D(), value.SizePx);
  EXPECT_EQ(PxThicknessF(), value.TrimMarginPxf);
  EXPECT_EQ(PxSize2DF(), value.TrimmedSizePxf);
}

TEST(TestMathPixel_PxTrimmedImage, Construct)
{
  constexpr PxSize2D sizePx(1, 2);
  constexpr PxThicknessF trimMarginPxf(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr PxSize2DF trimmedSizePxf(7.0f, 8.0f);

  PxTrimmedImage value(sizePx, trimMarginPxf, trimmedSizePxf);

  EXPECT_EQ(sizePx, value.SizePx);
  EXPECT_EQ(trimMarginPxf, value.TrimMarginPxf);
  EXPECT_EQ(trimmedSizePxf, value.TrimmedSizePxf);
}

TEST(TestMathPixel_PxTrimmedImage, OpEqual)
{
  constexpr PxSize2D sizePx(1, 2);
  constexpr PxThicknessF trimMarginPxf(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr PxSize2DF trimmedSizePxf(7.0f, 8.0f);

  PxTrimmedImage value0(sizePx, trimMarginPxf, trimmedSizePxf);
  PxTrimmedImage value1(sizePx, trimMarginPxf, trimmedSizePxf);

  EXPECT_EQ(value0, value1);
}

TEST(TestMathPixel_PxTrimmedImage, NotOpEqual)
{
  constexpr PxSize2D sizePx(1, 2);
  constexpr PxThicknessF trimMarginPxf(3.0f, 4.0f, 5.0f, 6.0f);
  constexpr PxSize2DF trimmedSizePxf(7.0f, 8.0f);

  PxTrimmedImage value(sizePx, trimMarginPxf, trimmedSizePxf);

  EXPECT_NE(PxTrimmedImage(PxSize2D(42, sizePx.Height()), trimMarginPxf, trimmedSizePxf), value);
  EXPECT_NE(PxTrimmedImage(PxSize2D(sizePx.Width(), 42), trimMarginPxf, trimmedSizePxf), value);
  EXPECT_NE(PxTrimmedImage(sizePx, PxThicknessF(42.0f, trimMarginPxf.Top(), trimMarginPxf.Right(), trimMarginPxf.Bottom()), trimmedSizePxf), value);
  EXPECT_NE(PxTrimmedImage(sizePx, PxThicknessF(trimMarginPxf.Left(), 42.0f, trimMarginPxf.Right(), trimMarginPxf.Bottom()), trimmedSizePxf), value);
  EXPECT_NE(PxTrimmedImage(sizePx, PxThicknessF(trimMarginPxf.Left(), trimMarginPxf.Top(), 42.0f, trimMarginPxf.Bottom()), trimmedSizePxf), value);
  EXPECT_NE(PxTrimmedImage(sizePx, PxThicknessF(trimMarginPxf.Left(), trimMarginPxf.Top(), trimMarginPxf.Right(), 42.0f), trimmedSizePxf), value);
  EXPECT_NE(PxTrimmedImage(sizePx, trimMarginPxf, PxSize2DF(42.0f, trimmedSizePxf.Height())), value);
  EXPECT_NE(PxTrimmedImage(sizePx, trimMarginPxf, PxSize2DF(trimmedSizePxf.Width(), 42.0f)), value);
}
