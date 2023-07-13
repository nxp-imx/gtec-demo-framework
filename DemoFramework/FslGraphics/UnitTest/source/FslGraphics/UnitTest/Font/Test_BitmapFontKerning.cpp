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

#include <FslGraphics/Font/BitmapFontKerning.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using TestFont_BitmapFontKerning = TestFixtureFslGraphics;
}


TEST(TestFont_BitmapFontKerning, Construct_Default)
{
  BitmapFontKerning value;
  EXPECT_EQ(0u, value.First);
  EXPECT_EQ(0u, value.Second);
  EXPECT_EQ(PxValue(0), value.AmountPx);
}


TEST(TestFont_BitmapFontKerning, Construct)
{
  BitmapFontKerning value(1u, 2u, PxValue(-1));
  EXPECT_EQ(1u, value.First);
  EXPECT_EQ(2u, value.Second);
  EXPECT_EQ(PxValue::Create(-1), value.AmountPx);
}


TEST(TestFont_BitmapFontKerning, OpEqual)
{
  EXPECT_EQ(BitmapFontKerning(1u, 2u, PxValue::Create(-1)), BitmapFontKerning(1u, 2u, PxValue::Create(-1)));
}


TEST(TestFont_BitmapFontKerning, OpNotEqual)
{
  EXPECT_NE(BitmapFontKerning(2u, 2u, PxValue::Create(-1)), BitmapFontKerning(1u, 2u, PxValue::Create(-1)));
  EXPECT_NE(BitmapFontKerning(1u, 3u, PxValue::Create(-1)), BitmapFontKerning(1u, 2u, PxValue::Create(-1)));
  EXPECT_NE(BitmapFontKerning(1u, 2u, PxValue::Create(-2)), BitmapFontKerning(1u, 2u, PxValue::Create(-1)));
}
