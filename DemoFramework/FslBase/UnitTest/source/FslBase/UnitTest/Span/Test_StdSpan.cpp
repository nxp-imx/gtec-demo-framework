/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <span>

namespace
{
  using TestStdSpan = TestFixtureFslBase;

  template <typename T, std::size_t TSize>
  std::span<T> Convert(std::array<T, TSize>& array)
  {
    return std::span<T>(array);
  }
}


TEST(TestStdSpan, Construct)
{
  std::span<uint8_t> span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}

TEST(TestStdSpan, Construct_FromArray)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), content.size());
}


TEST(TestStdSpan, SubSpan)
{
  std::array<uint8_t, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  auto span = Convert(content);

  std::array<uint8_t, 9> res123456789 = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<uint8_t, 2> res12 = {'1', '2'};
  std::array<uint8_t, 1> res9 = {'9'};

  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(span, span.subspan(0u)));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(Convert(res123456789), span.subspan(1u)));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(Convert(res9), span.subspan(9u)));

  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(Convert(res12), span.subspan(1u, 2)));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(Convert(res9), span.subspan(9u, 1)));

  // its ok to read the last char
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(std::span<uint8_t>(), span.subspan(10u)));
}

TEST(TestStdSpan, SubSpan_Empty)
{
  std::span<uint8_t> span;
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(std::span<uint8_t>(), span.subspan(0u)));
}


// TEST(TestStdSpan, SubSpan_InvalidPos)
//{
//   std::array<uint8_t, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
//   auto span = Convert(content);
//
//   EXPECT_THROW(auto qw = span.subspan(11u), std::out_of_range);
// }


TEST(TestStdSpan, front)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_EQ(span.front(), content[0]);
}


TEST(TestStdSpan, back)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_EQ(span.back(), content[content.size() - 1]);
}


TEST(TestStdSpan, opIndex)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(span[i], content[i]);
  }
}

TEST(TestStdSpan, opIndex_write)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_EQ('H', content[0]);
  EXPECT_EQ('H', span[0]);
  span[0] = 'h';
  EXPECT_EQ('h', content[0]);
  EXPECT_EQ('h', span[0]);

  EXPECT_EQ('d', content[10]);
  EXPECT_EQ('d', span[10]);
  span[10] = 'D';
  EXPECT_EQ('D', content[10]);
  EXPECT_EQ('D', span[10]);
}


// TEST(TestStdSpan, At)
//{
//   std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
//   auto span = Convert(content);
//
//   for (std::size_t i = 0; i < content.size(); ++i)
//   {
//     EXPECT_EQ(span.at(i), content.at(i));
//   }
// }


// TEST(TestStdSpan, At_OutOfBounds)
//{
//   std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
//   auto span = Convert(content);
//
//   EXPECT_THROW(span.at(content.size()), std::out_of_range);
// }
//
//
// TEST(TestStdSpan, At_Empty_OutOfBounds)
//{
//   std::span<uint8_t> span;
//
//   EXPECT_THROW(span.at(span.size()), std::out_of_range);
// }


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// iterators
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestStdSpan, begin_empty)
{
  std::span<char> span;

  ASSERT_EQ(span.end(), span.begin());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestStdSpan, begin_iterator_to_end)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::span<uint8_t> span = Convert(content);

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

TEST(TestStdSpan, begin_iterator_to_end_with_mod)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 11> content2 = {'W', 'o', 'r', 'l', 'd', ' ', 'h', 'e', 'l', 'l', 'o'};
  std::span<uint8_t> span = Convert(content);


  {    // overwrite content with content2 using the span
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrSource = content2.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrSourceEnd = content2.end();
    auto itr = span.begin();
    auto itrEnd = span.end();
    while (itr != itrEnd && itrSource != itrSourceEnd)
    {
      *itr = *itrSource;
      ++itrSource;
      ++itr;
    }
    ASSERT_EQ(itrEnd, itr);
    ASSERT_EQ(itrSourceEnd, itrSource);
  }
  {    // verify that both the span and content is equal to content2
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrContent = content.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrContentEnd = content.end();
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrContent2 = content2.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrContent2End = content2.end();

    auto itr = span.begin();
    auto itrEnd = span.end();
    while (itr != itrEnd && itrContent != itrContentEnd && itrContent2 != itrContent2End)
    {
      EXPECT_EQ(*itrContent2, *itrContent);
      EXPECT_EQ(*itrContent2, *itr);
      ++itrContent;
      ++itrContent2;
      ++itr;
    }
    ASSERT_EQ(itrEnd, itr);
    ASSERT_EQ(itrContentEnd, itrContent);
    ASSERT_EQ(itrContent2End, itrContent2);

    ASSERT_TRUE(itrEnd == itr);
    ASSERT_TRUE(itrContentEnd == itrContent);
    ASSERT_TRUE(itrContent2End == itrContent2);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestStdSpan, begin_foreach_with_mod)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 11> content2 = {'W', 'o', 'r', 'l', 'd', ' ', 'h', 'e', 'l', 'l', 'o'};
  std::span<uint8_t> span = Convert(content);


  {    // overwrite content with content2 using the span
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrSource = content2.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrSourceEnd = content2.end();
    for (auto& rEntry : span)
    {
      ASSERT_TRUE(itrSourceEnd != itrSource);
      rEntry = *itrSource;
      ++itrSource;
    }
    ASSERT_TRUE(itrSourceEnd == itrSource);
  }
  {    // verify that both the span and content is equal to content2
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrContent = content.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrContentEnd = content.end();
    // NOLINTNEXTLINE(readability-qualified-auto)
    auto itrContent2 = content2.begin();
    // NOLINTNEXTLINE(readability-qualified-auto)
    const auto itrContent2End = content2.end();

    auto itr = span.begin();
    auto itrEnd = span.end();
    while (itr != itrEnd && itrContent != itrContentEnd && itrContent2 != itrContent2End)
    {
      EXPECT_EQ(*itrContent2, *itrContent);
      EXPECT_EQ(*itrContent2, *itr);
      ++itrContent;
      ++itrContent2;
      ++itr;
    }
    ASSERT_EQ(itrEnd, itr);
    ASSERT_EQ(itrContentEnd, itrContent);
    ASSERT_EQ(itrContent2End, itrContent2);

    ASSERT_TRUE(itrEnd == itr);
    ASSERT_TRUE(itrContentEnd == itrContent);
    ASSERT_TRUE(itrContent2End == itrContent2);
  }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// iterators const
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestStdSpan, begin_empty_const)
{
  const std::span<char> span;

  ASSERT_EQ(span.end(), span.begin());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestStdSpan, begin_iterator_to_end_const)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::span<uint8_t> span = Convert(content);

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

//---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestStdSpan, begin_foreach_with_compare_const)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::span<uint8_t> span = Convert(content);

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

//---------------------------------------------------------------------------------------------------------------------------------------------------

// TEST(TestStdSpan, cbegin_empty)
//{
//   std::span<char> span;
//
//   ASSERT_EQ(span.cend(), span.cbegin());
// }

//---------------------------------------------------------------------------------------------------------------------------------------------------

// TEST(TestStdSpan, cbegin_iterator_to_cend_const)
//{
//   std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
//   const std::span<uint8_t> span = Convert(content);
//
//   // NOLINTNEXTLINE(readability-qualified-auto)
//   auto itrSource = content.begin();
//   // NOLINTNEXTLINE(readability-qualified-auto)
//   const auto itrSourceEnd = content.end();
//
//   auto itr = span.cbegin();
//   auto itrEnd = span.cend();
//   while (itr != itrEnd && itrSource != itrSourceEnd)
//   {
//     EXPECT_EQ(*itrSource, *itr);
//     ++itrSource;
//     ++itr;
//   }
//   ASSERT_EQ(itrEnd, itr);
//   ASSERT_EQ(itrSourceEnd, itrSource);
// }
