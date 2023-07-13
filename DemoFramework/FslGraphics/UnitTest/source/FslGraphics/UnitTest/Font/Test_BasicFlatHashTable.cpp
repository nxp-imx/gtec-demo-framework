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
#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslGraphics/Font/BasicFlatHashTable.hpp>
#include <FslGraphics/Font/BitmapFontChar.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <fmt/format.h>

using namespace Fsl;

namespace
{
  using TestFont_BasicFlatHashTable = TestFixtureFslGraphics;

  const std::array<BitmapFontChar, 10> g_chars = {
    BitmapFontChar(0, PxRectangleU32(), PxPoint2(), PxValueU16(0)),     BitmapFontChar(0x32, PxRectangleU32(), PxPoint2(), PxValueU16(1)),
    BitmapFontChar(0x40, PxRectangleU32(), PxPoint2(), PxValueU16(2)),  BitmapFontChar(0x41, PxRectangleU32(), PxPoint2(), PxValueU16(3)),
    BitmapFontChar(0x52, PxRectangleU32(), PxPoint2(), PxValueU16(4)),  BitmapFontChar(0x140, PxRectangleU32(), PxPoint2(), PxValueU16(5)),
    BitmapFontChar(0x141, PxRectangleU32(), PxPoint2(), PxValueU16(6)), BitmapFontChar(0x143, PxRectangleU32(), PxPoint2(), PxValueU16(7)),
    BitmapFontChar(0x240, PxRectangleU32(), PxPoint2(), PxValueU16(8)), BitmapFontChar(0x243, PxRectangleU32(), PxPoint2(), PxValueU16(9)),
  };
}

TEST(TestFont_BasicFlatHashTable, Construct_TryGet)
{
  BasicFlatHashTable<BitmapFontChar, uint32_t, 256u> fastlookup(ReadOnlySpanUtil::AsSpan(g_chars));

  for (std::size_t i = 0; i < g_chars.size(); ++i)
  {
    const auto* pFind = fastlookup.TryGet(g_chars[i].Id);
    if (pFind == nullptr)
    {
      throw NotFoundException(fmt::format("Could not find  id: {} (index: {})", g_chars[i].Id, i));
    }
    EXPECT_EQ(*pFind, g_chars[i]);
  }
}


TEST(TestFont_BasicFlatHashTable, TryGet_NotFound)
{
  BasicFlatHashTable<BitmapFontChar, uint32_t, 256u> fastlookup(ReadOnlySpanUtil::AsSpan(g_chars));

  EXPECT_TRUE(nullptr == fastlookup.TryGet(0x39));
  EXPECT_TRUE(nullptr == fastlookup.TryGet(0x139));
  EXPECT_TRUE(nullptr == fastlookup.TryGet(0xFFFFFF));
}
