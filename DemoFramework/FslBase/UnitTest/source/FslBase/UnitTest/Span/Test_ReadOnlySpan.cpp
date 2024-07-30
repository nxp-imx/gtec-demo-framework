/****************************************************************************************************************************************************
 * Copyright 2019, 2022, 2024 NXP
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

#include <FslBase/Span/ReadOnlySpan.hpp>
#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestReadOnlySpan = TestFixtureFslBase;

  ReadOnlySpan<char> Convert(const std::string& str)
  {
    return ReadOnlySpan<char>(str.data(), str.size());
  }

  template <typename T, std::size_t TSize>
  ReadOnlySpan<T> Convert(const std::array<T, TSize>& array)
  {
    return ReadOnlySpan<T>(array.data(), array.size());
  }
}


TEST(TestReadOnlySpan, Construct)
{
  ReadOnlySpan<char> span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}


TEST(TestReadOnlySpan, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  ReadOnlySpan<char> span(psz, lenPsz);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), lenPsz);
}


TEST(TestReadOnlySpan, Construct_FromStr)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), str.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, SubSpan)
{
  const char* const psz = "0123456789";
  ReadOnlySpan<char> span(psz, strlen(psz));

  EXPECT_TRUE(SpanUtil::ValueEquals(span, span.subspan(0)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("123456789", 9), span.subspan(1u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("9", 1), span.subspan(9u)));

  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("12", 2), span.subspan(1u, 2)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("9", 1), span.subspan(9u, 1u)));

  // its ok to read the last char
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>(), span.subspan(10u)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, SubSpan_Empty)
{
  ReadOnlySpan<char> span;
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>(), span.subspan(0)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, SubSpan_LastChar1)
{
  ReadOnlySpan<char> span("0123456789", 10);
  const auto subspan = span.subspan(10u, 0u);
  EXPECT_TRUE(subspan.empty());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, SubSpan_LastChar2)
{
  ReadOnlySpan<char> span("0123456789", 10);
  const auto subspan = span.subspan(10u);
  EXPECT_TRUE(subspan.empty());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, SubSpan_InvalidPos)
{
  ReadOnlySpan<char> span("0123456789", 10);

  EXPECT_THROW(span.subspan(10u, 1u), std::out_of_range);
  EXPECT_THROW(span.subspan(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// unchecked_subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, UncheckedSubSpan)
{
  const char* const psz = "0123456789";
  ReadOnlySpan<char> span(psz, strlen(psz));

  EXPECT_TRUE(SpanUtil::ValueEquals(span, span.unchecked_subspan(0, span.size())));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("123456789", 9), span.unchecked_subspan(1u, span.size() - 1u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("9", 1), span.unchecked_subspan(9u, span.size() - 9u)));

  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("12", 2), span.unchecked_subspan(1u, 2u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("9", 1), span.unchecked_subspan(9u, 1u)));

  // its ok to access the span at size() if the length is zero
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>(), span.unchecked_subspan(9u, span.size() - 10u)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, UncheckedSubSpan_Empty)
{
  ReadOnlySpan<char> span;
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>(), span.unchecked_subspan(0u, 0u)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// clamped_subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, ClampedSubSpan)
{
  const char* const psz = "0123456789";
  ReadOnlySpan<char> span(psz, strlen(psz));

  EXPECT_TRUE(SpanUtil::ValueEquals(span, span.clamped_subspan(0)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("123456789", 9), span.clamped_subspan(1u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("9", 1), span.clamped_subspan(9u)));

  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("12", 2), span.clamped_subspan(1u, 2)));
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>("9", 1), span.clamped_subspan(9u, 1u)));

  // its ok to read the last char
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>(), span.clamped_subspan(10u)));
}

TEST(TestReadOnlySpan, ClampedSubSpan_Empty)
{
  ReadOnlySpan<char> span;
  EXPECT_TRUE(SpanUtil::ValueEquals(ReadOnlySpan<char>(), span.clamped_subspan(0)));
}

TEST(TestReadOnlySpan, ClampedSubSpan_Last1)
{
  ReadOnlySpan<char> span("0123456789", 10);
  const auto subspan = span.clamped_subspan(10u, 0u);
  EXPECT_TRUE(subspan.empty());
}

TEST(TestReadOnlySpan, ClampedSubSpan_Last2)
{
  ReadOnlySpan<char> span("0123456789", 10);
  const auto subspan = span.clamped_subspan(10u);
  EXPECT_TRUE(subspan.empty());
}

TEST(TestReadOnlySpan, ClampedSubSpan_InvalidPos1)
{
  ReadOnlySpan<char> span("0123456789", 10);
  ReadOnlySpan<char> subspan = span.clamped_subspan(11u);
  EXPECT_TRUE(subspan.empty());
}

TEST(TestReadOnlySpan, ClampedSubSpan_InvalidPos2)
{
  ReadOnlySpan<char> span("0123456789", 10);

  EXPECT_TRUE(span.clamped_subspan(10u, 0u).empty());
  EXPECT_TRUE(span.clamped_subspan(10u, 1u).empty());
  EXPECT_TRUE(span.clamped_subspan(11u).empty());
  EXPECT_EQ(span.clamped_subspan(0u, 11u).size(), span.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// unchecked_first
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, UncheckedFirst)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.unchecked_first(0u).size(), 0u);
  EXPECT_EQ(span.unchecked_first(8u).size(), 8u);
  EXPECT_EQ(span.unchecked_first(9u).size(), 9u);
  EXPECT_EQ(span.unchecked_first(10u).size(), 10u);

  EXPECT_EQ(span.unchecked_first(1u)[0], span[0]);

  EXPECT_EQ(span.unchecked_first(2u)[0], span[0]);
  EXPECT_EQ(span.unchecked_first(2u)[1], span[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// first
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, First)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.first(0u).size(), 0u);
  EXPECT_EQ(span.first(8u).size(), 8u);
  EXPECT_EQ(span.first(9u).size(), 9u);
  EXPECT_EQ(span.first(10u).size(), 10u);

  EXPECT_EQ(span.first(1u)[0], span[0]);

  EXPECT_EQ(span.first(2u)[0], span[0]);
  EXPECT_EQ(span.first(2u)[1], span[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, First_InvalidPos)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_THROW(span.first(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// clamped_first
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, ClampedFirst)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.clamped_first(0u).size(), 0u);
  EXPECT_EQ(span.clamped_first(8u).size(), 8u);
  EXPECT_EQ(span.clamped_first(9u).size(), 9u);
  EXPECT_EQ(span.clamped_first(10u).size(), 10u);

  EXPECT_EQ(span.clamped_first(1u)[0], span[0]);

  EXPECT_EQ(span.clamped_first(2u)[0], span[0]);
  EXPECT_EQ(span.clamped_first(2u)[1], span[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, ClampedFirst_InvalidPos)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.clamped_first(11u).size(), 10u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// unchecked_last
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, UncheckedLast)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.unchecked_last(0u).size(), 0u);
  EXPECT_EQ(span.unchecked_last(8u).size(), 8u);
  EXPECT_EQ(span.unchecked_last(9u).size(), 9u);
  EXPECT_EQ(span.unchecked_last(10u).size(), 10u);

  EXPECT_EQ(span.unchecked_last(1u)[0], span[9]);

  EXPECT_EQ(span.unchecked_last(2u)[0], span[8]);
  EXPECT_EQ(span.unchecked_last(2u)[1], span[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// last
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, Last)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.last(0u).size(), 0u);
  EXPECT_EQ(span.last(8u).size(), 8u);
  EXPECT_EQ(span.last(9u).size(), 9u);
  EXPECT_EQ(span.last(10u).size(), 10u);

  EXPECT_EQ(span.last(1u)[0], span[9]);

  EXPECT_EQ(span.last(2u)[0], span[8]);
  EXPECT_EQ(span.last(2u)[1], span[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, CheckedLast_InvalidPos)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_THROW(span.last(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// clamped_last
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, ClampedLast)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.clamped_last(0u).size(), 0u);
  EXPECT_EQ(span.clamped_last(8u).size(), 8u);
  EXPECT_EQ(span.clamped_last(9u).size(), 9u);
  EXPECT_EQ(span.clamped_last(10u).size(), 10u);

  EXPECT_EQ(span.clamped_last(1u)[0], span[9]);

  EXPECT_EQ(span.clamped_last(2u)[0], span[8]);
  EXPECT_EQ(span.clamped_last(2u)[1], span[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, ClampedLast_InvalidPos)
{
  ReadOnlySpan<char> span("0123456789", 10);
  EXPECT_EQ(span.clamped_last(11u).size(), 10u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// front
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, front)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  EXPECT_EQ(span.front(), str.front());
}


TEST(TestReadOnlySpan, back)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  EXPECT_EQ(span.back(), str.back());
}


TEST(TestReadOnlySpan, opIndex)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(span[i], str[i]);
  }
}


TEST(TestReadOnlySpan, At)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(span.at(i), str.at(i));
  }
}


TEST(TestReadOnlySpan, At_OutOfBounds)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  EXPECT_THROW(span.at(str.size()), std::out_of_range);
}


TEST(TestReadOnlySpan, At_Empty_OutOfBounds)
{
  std::string str;
  ReadOnlySpan<char> span = Convert(str);

  EXPECT_THROW(span.at(str.size()), std::out_of_range);
}


TEST(TestReadOnlySpan, opEqual_Empty)
{
  std::string str1;
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  std::span<const char> q1 = strView1;
  std::span<const char> q2 = strView2;

  // EXPECT_TRUE(SpanUtil::ValueCompare(strView1, strView2) == 0);
  EXPECT_TRUE(SpanUtil::ValueCompare(q1, q2) == 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// iterators
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestReadOnlySpan, begin_empty)
{
  ReadOnlySpan<char> span;

  ASSERT_EQ(span.end(), span.begin());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, begin_iterator_to_end)
{
  const std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  ReadOnlySpan<uint8_t> span = Convert(content);

  // NOLINTNEXTLINE(readability-qualified-auto)
  auto itrSource = content.begin();
  // NOLINTNEXTLINE(readability-qualified-auto)
  const auto itrSourceEnd = content.end();

  auto itr = span.begin();
  auto itrEnd = span.end();
  while (itr != itrEnd && itrSource != itrSourceEnd)
  {
    EXPECT_EQ(*itrSource, *itr);
    ++itrSource;
    ++itr;
  }
  ASSERT_EQ(itrEnd, itr);
  ASSERT_EQ(itrSourceEnd, itrSource);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, begin_foreach_with_compare)
{
  const std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  ReadOnlySpan<uint8_t> span = Convert(content);


  {    // compare span with content using a 'foreach'
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrSource = content.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrSourceEnd = content.end();
    for (auto entry : span)
    {
      ASSERT_TRUE(itrSourceEnd != itrSource);
      EXPECT_EQ(*itrSource, entry);
      ++itrSource;
    }
    ASSERT_TRUE(itrSourceEnd == itrSource);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

// TEST(TestReadOnlySpan, cbegin_empty)
//{
//   ReadOnlySpan<char> span;
//
//   ASSERT_EQ(span.cend(), span.cbegin());
// }
//
////
///---------------------------------------------------------------------------------------------------------------------------------------------------
//
// TEST(TestReadOnlySpan, cbegin_iterator_to_cend)
//{
//  const std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
//  ReadOnlySpan<uint8_t> span = Convert(content);
//
//  // NOLINTNEXTLINE(readability-qualified-auto)
//  auto itrSource = content.begin();
//  // NOLINTNEXTLINE(readability-qualified-auto)
//  const auto itrSourceEnd = content.end();
//
//  auto itr = span.cbegin();
//  auto itrEnd = span.cend();
//  while (itr != itrEnd && itrSource != itrSourceEnd)
//  {
//    EXPECT_EQ(*itrSource, *itr);
//    ++itrSource;
//    ++itr;
//  }
//  ASSERT_EQ(itrEnd, itr);
//  ASSERT_EQ(itrSourceEnd, itrSource);
//}
