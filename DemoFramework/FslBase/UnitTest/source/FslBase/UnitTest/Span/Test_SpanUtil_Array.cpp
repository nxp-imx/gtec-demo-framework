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

#include <FslBase/Span/SpanUtil_Array.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestSpanUtil_Array = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSpanUtil_Array, AsReadOnlySpan)
{
  constexpr std::array<uint16_t, 10> Data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(Data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), Data.data());
  EXPECT_EQ(span.size(), Data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan_Empty)
{
  constexpr std::array<uint16_t, 0> Data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(Data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), Data.data());
  EXPECT_EQ(span.size(), Data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan_Empty)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::AsSpan(data);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedAsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsReadOnlySpan1)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedAsReadOnlySpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsReadOnlySpan2)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedAsReadOnlySpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsReadOnlySpan_Empty)
{
  const std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedAsReadOnlySpan(data, 0u, data.size());

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedAsSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsSpan1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsSpan2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsSpan_Half1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 0u, data.size() / 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size() / 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsSpan_Half2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 2u, data.size() / 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 2u);
  EXPECT_EQ(span.size(), data.size() / 2u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedAsSpan_Empty)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::UncheckedAsSpan(data, 0u, data.size());

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsSpan subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan1_subspan)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan2_subspan)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan3_subspan)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan4_subspan)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan_subspan_Empty1)
{
  const std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan_subspan_Empty2)
{
  const std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan_subspan_PositionOverflowHandled)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, data.size() + 2, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan_subspan_SizeOverflowHandled1)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 0u, data.size() + 2);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan_subspan_SizeOverflowHandled2)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, 9, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 9u);
  EXPECT_EQ(span.size(), 1u);
  EXPECT_EQ(span[0], data[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsReadOnlySpan_subspan_PositionAndSizeOverflowHandled)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::ClampedAsReadOnlySpan(data, data.size() + 2, 2u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedAsSubSpan subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan1_subspan)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan2_subspan)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan3_subspan)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan4_subspan)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, offset, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan_subspan_Empty1)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan_subspan_Empty2)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan_subspan_PositionOverflowHandled)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, data.size() + 2, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan_subspan_SizeOverflowHandled1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 0u, data.size() + 2);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan_subspan_SizeOverflowHandled2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, 9, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + 9u);
  EXPECT_EQ(span.size(), 1u);
  EXPECT_EQ(span[0], data[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedAsSpan_subspan_PositionAndSizeOverflowHandled)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedAsSpan(data, data.size() + 2, 2u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.size(), 0u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan1)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan2)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = 1u;
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 1u, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan3)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan_Empty1)
{
  const std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan_Empty2)
{
  const std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::AsReadOnlySpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsReadOnlySpan_Throws)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::AsReadOnlySpan(data, data.size() + 1u), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// AsReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::AsSpan(data, 0u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = 1u;
  Span<uint16_t> span = SpanUtil::AsSpan(data, 1u, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan3)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const auto offset = data.size() - 2u;
  Span<uint16_t> span = SpanUtil::AsSpan(data, offset);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data() + offset);
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[offset]);
  EXPECT_EQ(span[1], data[offset + 1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan_Empty1)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::AsSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan_Empty2)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::AsSpan(data, 0u, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, AsSpan_Throws)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::AsSpan(data, data.size() + 1u), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedFirstReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedFirstReadOnlySpan1)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedFirstReadOnlySpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedFirstReadOnlySpan2)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedFirstReadOnlySpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedFirstReadOnlySpan_Empty)
{
  std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::UncheckedFirstReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// UncheckedFirstSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedFirstSpan1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedFirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedFirstSpan2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::UncheckedFirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, UncheckedFirstSpan_Empty)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::UncheckedFirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// FirstReadOnlySpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstReadOnlySpan1)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::FirstReadOnlySpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstReadOnlySpan2)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlySpan<uint16_t> span = SpanUtil::FirstReadOnlySpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstReadOnlySpan_Empty)
{
  const std::array<uint16_t, 0> data{};
  ReadOnlySpan<uint16_t> span = SpanUtil::FirstReadOnlySpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstReadOnlySpan_Throws)
{
  const std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::FirstReadOnlySpan(data, data.size() + 1u), std::invalid_argument);
  EXPECT_THROW(SpanUtil::FirstReadOnlySpan(data, std::dynamic_extent), std::invalid_argument);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// FirstSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstSpan1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::FirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstSpan2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::FirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstSpan_Empty)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::FirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, FirstSpan_Throws)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_THROW(SpanUtil::FirstSpan(data, data.size() + 1u), std::invalid_argument);
  EXPECT_THROW(SpanUtil::FirstSpan(data, std::dynamic_extent), std::invalid_argument);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedFirstSpan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedFirstSpan1)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedFirstSpan(data, data.size());

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedFirstSpan2)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  Span<uint16_t> span = SpanUtil::ClampedFirstSpan(data, 2u);

  EXPECT_FALSE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), 2u);
  EXPECT_EQ(span[0], data[0]);
  EXPECT_EQ(span[1], data[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedFirstSpan_Empty)
{
  std::array<uint16_t, 0> data{};
  Span<uint16_t> span = SpanUtil::ClampedFirstSpan(data, 0u);

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), data.data());
  EXPECT_EQ(span.size(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtil_Array, ClampedFirstSpan_Throws)
{
  std::array<uint16_t, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  EXPECT_EQ(SpanUtil::ClampedFirstSpan(data, data.size() + 1u).size(), data.size());
}
