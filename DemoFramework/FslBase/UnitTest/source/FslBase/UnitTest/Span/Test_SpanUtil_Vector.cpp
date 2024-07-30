/****************************************************************************************************************************************************
 * Copyright 2019, 2024 NXP
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

#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslBase/Span/SpanUtil_Vector.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestSpanUtil_Vector = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan_Empty)
{
  const std::vector<uint16_t> data;
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan_Empty)
{
  std::vector<uint16_t> data = {};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedAsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsReadOnlySpan1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedAsReadOnlySpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsReadOnlySpan2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedAsReadOnlySpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsReadOnlySpan_Empty)
{
  std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedAsReadOnlySpan(data, 0u, data.size());

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedAsSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsSpan1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsSpan2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsSpan_Half1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 0u, data.size() / 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size() / 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsSpan_Half2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 2u, data.size() / 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 2u);
  EXPECT_EQ(span.size(), data.size() / 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedAsSpan_Empty)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 0u, data.size());

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedAsSpan subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan1_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan2_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan3_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan4_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan_subspan_Empty1)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan_subspan_Empty2)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan_subspan_PositionOverflowHandled)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, data.size() + 2, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan_subspan_SizeOverflowHandled1)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u, data.size() + 2);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan_subspan_SizeOverflowHandled2)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 9, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 9u);
  EXPECT_EQ(span.size(), 1u);
  EXPECT_EQ(span[0], data[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsReadOnlySpan_subspan_PositionAndSizeOverflowHandled)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, data.size() + 2, 2u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedAsSpan subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const1_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const2_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const3_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const4_subspan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const_subspan_Empty1)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const_subspan_Empty2)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const_subspan_PositionOverflowHandled)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, data.size() + 2, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const_subspan_SizeOverflowHandled1)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, data.size() + 2);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const_subspan_SizeOverflowHandled2)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, 9, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 9u);
  EXPECT_EQ(span.size(), 1u);
  EXPECT_EQ(span[0], data[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_const_subspan_PositionAndSizeOverflowHandled)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsSpan(data, data.size() + 2, 2u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedAsSubSpan subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan1_subspan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan2_subspan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan3_subspan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan4_subspan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_subspan_Empty1)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_subspan_Empty2)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_subspan_PositionOverflowHandled)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, data.size() + 2, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_subspan_SizeOverflowHandled1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, data.size() + 2);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_subspan_SizeOverflowHandled2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 9, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 9u);
  EXPECT_EQ(span.size(), 1u);
  EXPECT_EQ(span[0], data[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedAsSpan_subspan_PositionAndSizeOverflowHandled)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, data.size() + 2, 2u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan1)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan2)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = 1u;
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 1u, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan3)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan_Empty1)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan_Empty2)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsReadOnlySpan_Throws)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::AsReadOnlySpan(data, data.size() + 1u), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = 1u;
  Span<uint16_t> span = SpanUtil::AsSpan(data, 1u, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan3)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::AsSpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan_Empty1)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::AsSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan_Empty2)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::AsSpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, AsSpan_Throws)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::AsSpan(data, data.size() + 1u), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedFirstReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedFirstReadOnlySpan1)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedFirstReadOnlySpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedFirstReadOnlySpan2)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedFirstReadOnlySpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedFirstReadOnlySpan_Empty)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedFirstReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedFirstSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedFirstSpan1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedFirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedFirstSpan2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedFirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, UncheckedFirstSpan_Empty)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::UncheckedFirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// FirstReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstReadOnlySpan1)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::FirstReadOnlySpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstReadOnlySpan2)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::FirstReadOnlySpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstReadOnlySpan_Empty)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::FirstReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstReadOnlySpan_Throws)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::FirstReadOnlySpan(data, data.size() + 1u), std::invalid_argument);
  EXPECT_THROW(SpanUtil::FirstReadOnlySpan(data, std::dynamic_extent), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// FirstSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstSpan1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::FirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstSpan2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::FirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstSpan_Empty)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::FirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, FirstSpan_Throws)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::FirstSpan(data, data.size() + 1u), std::invalid_argument);
  EXPECT_THROW(SpanUtil::FirstSpan(data, std::dynamic_extent), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedFirstReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstReadOnlySpan1)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedFirstReadOnlySpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstReadOnlySpan2)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedFirstReadOnlySpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstReadOnlySpan_Empty)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedFirstReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstReadOnlySpan_Throws)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(SpanUtil::ClampedFirstReadOnlySpan(data, data.size() + 1u).size(), data.size());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedFirstReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan1_const)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedFirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan2_const)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedFirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan_const_Empty)
{
  const std::vector<uint16_t> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedFirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan_const_Throws)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(SpanUtil::ClampedFirstSpan(data, data.size() + 1u).size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedFirstSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan1)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedFirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan2)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedFirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan_Empty)
{
  std::vector<uint16_t> data{};
  Span<uint16_t> span = SpanUtil::ClampedFirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ClampedFirstSpan_Throws)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(SpanUtil::ClampedFirstSpan(data, data.size() + 1u).size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ToVector
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromReadOnlySpan_Empty)
{
  ReadOnlySpan<uint16_t> span;
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(data2.data(), span.data());
  EXPECT_EQ(data2.size(), span.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromReadOnlyStdSpan_Empty)
{
  std::span<const uint16_t> span;
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(data2.data(), span.data());
  EXPECT_EQ(data2.size(), span.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromSpan_Empty)
{
  Span<uint16_t> span;
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(data2.data(), span.data());
  EXPECT_EQ(data2.size(), span.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromStdSpan_Empty)
{
  std::span<uint16_t> span;
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(data2.data(), span.data());
  EXPECT_EQ(data2.size(), span.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromReadOnlySpan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data);
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_EQ(data2.size(), data.size());
  if (data2.size() == data.size())
  {
    for (std::size_t i = 0; i < data2.size(); ++i)
    {
      EXPECT_EQ(data2[i], data[i]);
    }
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromReadOnlyStdSpan)
{
  const std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::span<const uint16_t> span = SpanUtil::AsReadOnlySpan(data);
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_EQ(data2.size(), data.size());
  if (data2.size() == data.size())
  {
    for (std::size_t i = 0; i < data2.size(); ++i)
    {
      EXPECT_EQ(data2[i], data[i]);
    }
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromSpan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data);
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_EQ(data2.size(), data.size());
  if (data2.size() == data.size())
  {
    for (std::size_t i = 0; i < data2.size(); ++i)
    {
      EXPECT_EQ(data2[i], data[i]);
    }
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Vector, ToVector_FromStdSpan)
{
  std::vector<uint16_t> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::span<uint16_t> span = SpanUtil::AsSpan(data);
  const std::vector<uint16_t> data2 = SpanUtil::ToVector(span);

  EXPECT_EQ(data2.size(), data.size());
  if (data2.size() == data.size())
  {
    for (std::size_t i = 0; i < data2.size(); ++i)
    {
      EXPECT_EQ(data2[i], data[i]);
    }
  }
}
