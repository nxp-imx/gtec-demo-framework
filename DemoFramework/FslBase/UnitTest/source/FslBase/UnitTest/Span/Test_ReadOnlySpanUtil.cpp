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

#include <FslBase/Span/ReadOnlySpanUtil.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestReadOnlySpanUtil = TestFixtureFslBase;
}

TEST(TestReadOnlySpanUtil, AsSpan_PointerLength)
{
  constexpr std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data.data(), data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}

TEST(TestReadOnlySpanUtil, AsSpan_PointerLength_NullPtr)
{
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan<uint16_t>(nullptr, 0);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
}


TEST(TestReadOnlySpanUtil, AsSpan_ointerLength_NullPtrInvalidLength)
{
  constexpr std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(ReadOnlySpanUtil::AsSpan<uint16_t>(nullptr, data.size()), std::invalid_argument);
}


TEST(TestReadOnlySpanUtil, AsSpan_FromPointerLength_NoCheck)
{
  constexpr std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data.data(), data.size(), OptimizationCheckFlag::NoCheck);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}


TEST(TestReadOnlySpanUtil, AsSpan_Array)
{
  constexpr std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}

TEST(TestReadOnlySpanUtil, AsSpan_ArrayEmpty)
{
  constexpr std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}


TEST(TestReadOnlySpanUtil, AsSpan_Vector)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}


TEST(TestReadOnlySpanUtil, AsSpan_Vector_Empty)
{
  const std::vector<uint16_t> data;
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
  EXPECT_EQ(span.length(), data.size());
}

TEST(TestReadOnlySpanUtil, ToVector)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = ReadOnlySpanUtil::AsSpan(data);
  const std::vector<uint16_t> data2 = ReadOnlySpanUtil::ToVector(span);

  EXPECT_EQ(data2.size(), data.size());
  if (data2.size() == data.size())
  {
    for (std::size_t i = 0; i < data2.size(); ++i)
    {
      EXPECT_EQ(data2[i], data[i]);
    }
  }
}

TEST(TestReadOnlySpanUtil, ToVector_Empty)
{
  ReadOnlySpan<uint16_t> span;
  const std::vector<uint16_t> data2 = ReadOnlySpanUtil::ToVector(span);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(data2.data(), span.data());
  EXPECT_EQ(data2.size(), span.size());
}
