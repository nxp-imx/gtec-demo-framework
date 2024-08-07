/****************************************************************************************************************************************************
 * Copyright 2021, 2024 NXP
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

#include <FslBase/Span/ReadOnlyFlexSpanUtil.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestReadOnlyFlexSpanUtil = TestFixtureFslBase;
}

TEST(TestReadOnlyFlexSpanUtil, AsSpan_PointerLength)
{
  constexpr static std::array<char, 11> Data = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  ReadOnlyFlexSpan span = ReadOnlyFlexSpanUtil::AsSpan(Data.data(), Data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), Data.data());
  EXPECT_EQ(span.size(), Data.size());
  EXPECT_EQ(span.length(), Data.size());
  EXPECT_EQ(span.stride(), sizeof(char));
}


TEST(TestReadOnlyFlexSpanUtil, AsSpan_PointerLength_NullPtrZeroLength)
{
  ReadOnlyFlexSpan span = ReadOnlyFlexSpanUtil::AsSpan<uint16_t>(nullptr, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), sizeof(uint16_t));
}


TEST(TestReadOnlyFlexSpanUtil, AsSpan_PointerLength_NullPtrInvalidLength)
{
  constexpr static std::array<char, 11> Data = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  EXPECT_THROW(ReadOnlyFlexSpanUtil::AsSpan<char>(nullptr, Data.size()), std::invalid_argument);
}


TEST(TestReadOnlyFlexSpanUtil, AsSpan_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  ReadOnlyFlexSpan span = ReadOnlyFlexSpanUtil::AsSpan(psz, lenPsz);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), psz);
  EXPECT_EQ(span.size(), lenPsz);
  EXPECT_EQ(span.length(), lenPsz);
  EXPECT_EQ(span.stride(), sizeof(char));
}


TEST(TestReadOnlyFlexSpanUtil, AsSpan_FromZeroTerminated_constexpr)
{
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  constexpr static char Data[11] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  constexpr static ReadOnlyFlexSpan Span = ReadOnlyFlexSpanUtil::AsSpan(Data, 11);

  EXPECT_FALSE(Span.empty());
  EXPECT_EQ(Span.data(), Data);
  EXPECT_EQ(Span.size(), 11u);
  EXPECT_EQ(Span.length(), 11u);
  EXPECT_EQ(Span.stride(), sizeof(char));
}
