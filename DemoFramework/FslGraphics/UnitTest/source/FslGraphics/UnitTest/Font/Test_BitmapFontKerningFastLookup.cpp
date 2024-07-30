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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslGraphics/Font/BitmapFontKerningFastLookup.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <fmt/format.h>

using namespace Fsl;

namespace
{
  using TestFont_BitmapFontKerningFastLookup = TestFixtureFslGraphics;

  const std::array<BitmapFontKerning, 11> g_kernings = {
    BitmapFontKerning(0, 1, PxValue(0)),        BitmapFontKerning(0, 2, PxValue(1)),         BitmapFontKerning(0, 3, PxValue(2)),
    BitmapFontKerning(0x40, 0, PxValue(3)),     BitmapFontKerning(0x41, 1, PxValue(4)),      BitmapFontKerning(0x140, 0, PxValue(5)),
    BitmapFontKerning(0x140, 0xFF, PxValue(6)), BitmapFontKerning(0x141, 0, PxValue(7)),     BitmapFontKerning(0x143, 0, PxValue(8)),
    BitmapFontKerning(0x240, 10, PxValue(9)),   BitmapFontKerning(0x243, 0x40, PxValue(10)),
  };

}


TEST(TestFont_BitmapFontKerningFastLookup, Construct)
{
  BitmapFontKerningFastLookup fastlookup(SpanUtil::AsReadOnlySpan(g_kernings));

  for (std::size_t i = 0; i < g_kernings.size(); ++i)
  {
    const auto* pFind = fastlookup.TryGet(g_kernings[i].GetId());
    if (pFind == nullptr)
    {
      throw NotFoundException(
        fmt::format("Could not find kerning for first: {} second: {} (index: {})", g_kernings[i].First, g_kernings[i].Second, i));
    }
    EXPECT_EQ(*pFind, g_kernings[i]);
  }
}


TEST(TestFont_BitmapFontKerningFastLookup, TryGet_NotFound)
{
  BitmapFontKerningFastLookup fastlookup(SpanUtil::AsReadOnlySpan(g_kernings));

  EXPECT_TRUE(nullptr == fastlookup.TryGet(0x39, 0));
  EXPECT_TRUE(nullptr == fastlookup.TryGet(0x139, 0));
  EXPECT_TRUE(nullptr == fastlookup.TryGet(0xFFFFFF, 0));

  EXPECT_TRUE(nullptr == fastlookup.TryGet(0, 0));
  EXPECT_TRUE(nullptr == fastlookup.TryGet(0, 4));
  EXPECT_TRUE(nullptr == fastlookup.TryGet(0x143, 1));

  EXPECT_TRUE(nullptr == fastlookup.TryGet(0x243, 1));
}
