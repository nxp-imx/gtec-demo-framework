/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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

#include <FslBase/Span/SpanUtil.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestSpanUtil = TestFixtureFslBase;
}


TEST(TestSpanUtil, AsSpan_PointerLength)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data.data(), data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}

TEST(TestSpanUtil, AsSpan_PointerLength_NullPtr)
{
  Span<uint16_t> span = SpanUtil::AsSpan<uint16_t>(nullptr, 0);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
}


TEST(TestSpanUtil, AsSpan_PointerLength_NullPtrInvalidLength)
{
  constexpr std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::AsSpan<uint16_t>(nullptr, data.size()), std::invalid_argument);
}


TEST(TestSpanUtil, AsSpan_PointerLength_NoCheck)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data.data(), data.size(), OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}


TEST(TestSpanUtil, AsSpan_Array)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}

TEST(TestSpanUtil, AsSpan_ArrayEmpty)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}


TEST(TestSpanUtil, AsSpan_Vector)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}
