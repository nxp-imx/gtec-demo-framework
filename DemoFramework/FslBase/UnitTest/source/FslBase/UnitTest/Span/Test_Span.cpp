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

#include <FslBase/Span/Span.hpp>
#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>

using namespace Fsl;

namespace
{
  using TestSpan = TestFixtureFslBase;

  template <typename T, std::size_t TSize>
  Span<T> Convert(std::array<T, TSize>& array)
  {
    return Span<T>(array.data(), array.size());
  }
}


TEST(TestSpan, Construct)
{
  Span<uint8_t> span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
}

TEST(TestSpan, Construct_FromArray)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), content.size());
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, SubSpan)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 9> content123456789 = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 1> content9 = {'9'};
  std::array<char, 2> content12 = {'1', '2'};
  Span<char> span(content);

  EXPECT_TRUE(SpanUtil::ValueEquals(span, span.subspan(0)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content123456789), span.subspan(1u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content9), span.subspan(9u)));

  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content12), span.subspan(1u, 2)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content9), span.subspan(9u, 1u)));

  // its ok to read the last char
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(), span.subspan(10u)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, SubSpan_Empty)
{
  Span<char> span;
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(), span.subspan(0)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, SubSpan_LastChar1)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  const auto subspan = span.subspan(10u, 0u);
  EXPECT_TRUE(subspan.empty());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, SubSpan_LastChar2)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  const auto subspan = span.subspan(10u);
  EXPECT_TRUE(subspan.empty());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, SubSpan_InvalidPos)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);

  EXPECT_THROW(span.subspan(10u, 1u), std::out_of_range);
  EXPECT_THROW(span.subspan(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// unchecked_subspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, UncheckedSubSpan)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 9> content123456789 = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 1> content9 = {'9'};
  std::array<char, 2> content12 = {'1', '2'};
  Span<char> span(content);

  EXPECT_TRUE(SpanUtil::ValueEquals(span, span.unchecked_subspan(0, span.size())));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content123456789), span.unchecked_subspan(1u, span.size() - 1u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content9), span.unchecked_subspan(9u, span.size() - 9u)));

  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content12), span.unchecked_subspan(1u, 2u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content9), span.unchecked_subspan(9u, 1u)));

  // its ok to access the span at size() if the length is zero
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(), span.unchecked_subspan(9u, span.size() - 10u)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, UncheckedSubSpan_Empty)
{
  Span<char> span;
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(), span.unchecked_subspan(0u, 0u)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ClampedSubspan
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, ClampedSubSpan)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 9> content123456789 = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  std::array<char, 1> content9 = {'9'};
  std::array<char, 2> content12 = {'1', '2'};
  Span<char> span(content);

  EXPECT_TRUE(SpanUtil::ValueEquals(span, span.clamped_subspan(0)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content123456789), span.clamped_subspan(1u)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content9), span.clamped_subspan(9u)));

  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content12), span.clamped_subspan(1u, 2)));
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(content9), span.clamped_subspan(9u, 1u)));

  // its ok to read the last char
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(), span.clamped_subspan(10u)));
}

TEST(TestSpan, ClampedSubSpan_Empty)
{
  Span<char> span;
  EXPECT_TRUE(SpanUtil::ValueEquals(Span<char>(), span.clamped_subspan(0)));
}

TEST(TestSpan, ClampedSubSpan_Last1)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  const auto subspan = span.clamped_subspan(10u, 0u);
  EXPECT_TRUE(subspan.empty());
}

TEST(TestSpan, ClampedSubSpan_Last2)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  const auto subspan = span.clamped_subspan(10u);
  EXPECT_TRUE(subspan.empty());
}

TEST(TestSpan, ClampedSubSpan_InvalidPos1)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  Span<char> subspan = span.clamped_subspan(11u);
  EXPECT_TRUE(subspan.empty());
}

TEST(TestSpan, ClampedSubSpan_InvalidPos2)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);

  EXPECT_TRUE(span.clamped_subspan(10u, 0u).empty());
  EXPECT_TRUE(span.clamped_subspan(10u, 1u).empty());
  EXPECT_TRUE(span.clamped_subspan(11u).empty());
  EXPECT_EQ(span.clamped_subspan(0u, 11u).size(), span.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// unchecked_first
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, UncheckedFirst)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
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

TEST(TestSpan, First)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_EQ(span.first(0u).size(), 0u);
  EXPECT_EQ(span.first(8u).size(), 8u);
  EXPECT_EQ(span.first(9u).size(), 9u);
  EXPECT_EQ(span.first(10u).size(), 10u);

  EXPECT_EQ(span.first(1u)[0], span[0]);

  EXPECT_EQ(span.first(2u)[0], span[0]);
  EXPECT_EQ(span.first(2u)[1], span[1]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, First_InvalidPos)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_THROW(span.first(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// clamped_first
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, ClampedFirst)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_EQ(span.clamped_first(0u).size(), 0u);
  EXPECT_EQ(span.clamped_first(8u).size(), 8u);
  EXPECT_EQ(span.clamped_first(9u).size(), 9u);
  EXPECT_EQ(span.clamped_first(10u).size(), 10u);

  EXPECT_EQ(span.clamped_first(1u)[0], span[0]);

  EXPECT_EQ(span.clamped_first(2u)[0], span[0]);
  EXPECT_EQ(span.clamped_first(2u)[1], span[1]);
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, ClampedFirst_InvalidPos)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_EQ(span.clamped_first(11u).size(), 10u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// unchecked_last
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, UncheckedLast)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
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

TEST(TestSpan, Last)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_EQ(span.last(0u).size(), 0u);
  EXPECT_EQ(span.last(8u).size(), 8u);
  EXPECT_EQ(span.last(9u).size(), 9u);
  EXPECT_EQ(span.last(10u).size(), 10u);

  EXPECT_EQ(span.last(1u)[0], span[9]);

  EXPECT_EQ(span.last(2u)[0], span[8]);
  EXPECT_EQ(span.last(2u)[1], span[9]);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, Last_InvalidPos)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_THROW(span.last(11u), std::out_of_range);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// clamped_last
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, ClampedLast)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_EQ(span.clamped_last(0u).size(), 0u);
  EXPECT_EQ(span.clamped_last(8u).size(), 8u);
  EXPECT_EQ(span.clamped_last(9u).size(), 9u);
  EXPECT_EQ(span.clamped_last(10u).size(), 10u);

  EXPECT_EQ(span.clamped_last(1u)[0], span[9]);

  EXPECT_EQ(span.clamped_last(2u)[0], span[8]);
  EXPECT_EQ(span.clamped_last(2u)[1], span[9]);
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, ClampedLast_InvalidPos)
{
  std::array<char, 10> content = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  Span<char> span(content);
  EXPECT_EQ(span.clamped_last(11u).size(), 10u);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// front
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, front)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_EQ(span.front(), content[0]);
}


TEST(TestSpan, back)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_EQ(span.back(), content[content.size() - 1]);
}


TEST(TestSpan, opIndex)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(span[i], content[i]);
  }
}

TEST(TestSpan, opIndex_write)
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


TEST(TestSpan, At)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(span.at(i), content.at(i));
  }
}


TEST(TestSpan, At_OutOfBounds)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span = Convert(content);

  EXPECT_THROW(span.at(content.size()), std::out_of_range);
}


TEST(TestSpan, At_Empty_OutOfBounds)
{
  Span<uint8_t> span;

  EXPECT_THROW(span.at(span.size()), std::out_of_range);
}

#if 0
TEST(TestSpan, opEqual_Empty)
{
  Span<uint8_t> span1;
  Span<uint8_t> span2;

  EXPECT_TRUE(span1 == span2);
}

TEST(TestSpan, opEqual1)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 == span2);
}

TEST(TestSpan, opEqual2)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'D'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 == span2);
}


TEST(TestSpan, opEqual3)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 10> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 == span2);
}


TEST(TestSpan, opEqual4)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 12> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '2'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 == span2);
}


TEST(TestSpan, opEqual5)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 == span2);
}


TEST(TestSpan, opNotEqual_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 != span2);
}

TEST(TestSpan, opNotEqual1)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 != span2);
}

TEST(TestSpan, opNotEqual2)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'D'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 != span2);
}


TEST(TestSpan, opNotEqual3)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 10> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 != span2);
}


TEST(TestSpan, opNotEqual4)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 12> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '2'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 != span2);
}


TEST(TestSpan, opNotEqual5)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 != span2);
}


TEST(TestSpan, opLess_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 < span2);
}


TEST(TestSpan, opLess1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};

  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 < span2);
}


TEST(TestSpan, opLess2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};

  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 < span2);
}


TEST(TestSpan, opLess3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 < span2);
}


TEST(TestSpan, opLess4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 < span2);
}


TEST(TestSpan, opLess5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 < span2);
}

TEST(TestSpan, opLessOrEqual_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 <= span2);
}


TEST(TestSpan, opLessOrEqual1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 <= span2);
}


TEST(TestSpan, opLessOrEqual2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 <= span2);
}


TEST(TestSpan, opLessOrEqual3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 <= span2);
}


TEST(TestSpan, opLessOrEqual4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 <= span2);
}


TEST(TestSpan, opLessOrEqual5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 <= span2);
}

TEST(TestSpan, opGreater_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 > span2);
}


TEST(TestSpan, opGreater1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 > span2);
}


TEST(TestSpan, opGreater2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 > span2);
}


TEST(TestSpan, opGreater3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 > span2);
}


TEST(TestSpan, opGreater4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 > span2);
}


TEST(TestSpan, opGreater5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 > span2);
}

TEST(TestSpan, opGreaterOrEqual_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 >= span2);
}


TEST(TestSpan, opGreaterOrEqual1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 >= span2);
}


TEST(TestSpan, opGreaterOrEqual2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 >= span2);
}


TEST(TestSpan, opGreaterOrEqual3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 >= span2);
}


TEST(TestSpan, opGreaterOrEqual4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(span1 >= span2);
}


TEST(TestSpan, opGreaterOrEqual5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(span1 >= span2);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// compare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, compare)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Convert(contentA).compare(Convert(contentB)) < 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentB)) == 0);
  EXPECT_TRUE(Convert(contentC).compare(Convert(contentB)) > 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentA)) > 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentC)) < 0);

  EXPECT_TRUE(Convert(contentA).compare(Convert(contentBA)) < 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentBA)) < 0);
  EXPECT_TRUE(Convert(contentC).compare(Convert(contentBA)) > 0);
  EXPECT_TRUE(Convert(contentBA).compare(Convert(contentA)) > 0);
  EXPECT_TRUE(Convert(contentBA).compare(Convert(contentB)) > 0);
  EXPECT_TRUE(Convert(contentBA).compare(Convert(contentC)) < 0);

  EXPECT_TRUE(Convert(contentAA).compare(Convert(contentB)) < 0);
  EXPECT_TRUE(Convert(contentBA).compare(Convert(contentB)) > 0);
  EXPECT_TRUE(Convert(contentCA).compare(Convert(contentB)) > 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentAA)) > 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentBA)) < 0);
  EXPECT_TRUE(Convert(contentB).compare(Convert(contentCA)) < 0);
}

TEST(TestSpan, compare_Null)
{
  std::array<uint8_t, 0> contentA = {};

  EXPECT_TRUE(Span<char>().compare(Span<char>()) == 0);
  EXPECT_TRUE(Span<uint8_t>(contentA.data(), contentA.size()).compare(Span<uint8_t>()) == 0);
  EXPECT_TRUE(Span<uint8_t>().compare(Span<uint8_t>(contentA.data(), contentA.size())) == 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, OperatorLessThan)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_LT(Convert(contentA), Convert(contentB));
  EXPECT_LT(Convert(contentA), Convert(contentAB));
  EXPECT_LT(Span<uint8_t>(), Convert(contentAB));

  EXPECT_TRUE(Convert(contentA) < Convert(contentB));
  EXPECT_FALSE(Convert(contentB) < Convert(contentB));
  EXPECT_FALSE(Convert(contentC) < Convert(contentB));
  EXPECT_FALSE(Convert(contentB) < Convert(contentA));
  EXPECT_FALSE(Convert(contentB) < Convert(contentB));
  EXPECT_TRUE(Convert(contentB) < Convert(contentC));

  EXPECT_TRUE(Convert(contentA) < Convert(contentBA));
  EXPECT_TRUE(Convert(contentB) < Convert(contentBA));
  EXPECT_FALSE(Convert(contentC) < Convert(contentBA));
  EXPECT_FALSE(Convert(contentBA) < Convert(contentA));
  EXPECT_FALSE(Convert(contentBA) < Convert(contentB));
  EXPECT_TRUE(Convert(contentBA) < Convert(contentC));

  EXPECT_TRUE(Convert(contentAA) < Convert(contentB));
  EXPECT_FALSE(Convert(contentBA) < Convert(contentB));
  EXPECT_FALSE(Convert(contentCA) < Convert(contentB));
  EXPECT_FALSE(Convert(contentB) < Convert(contentAA));
  EXPECT_TRUE(Convert(contentB) < Convert(contentBA));
  EXPECT_TRUE(Convert(contentB) < Convert(contentCA));
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, OperatorLessThanOrEqual)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_LE(Convert(contentA), Convert(contentB));
  EXPECT_LE(Convert(contentA), Convert(contentAB));
  EXPECT_LE(Span<uint8_t>(), Convert(contentAB));
  EXPECT_LE(Convert(contentA), Convert(contentA));

  EXPECT_TRUE(Convert(contentA) <= Convert(contentB));
  EXPECT_TRUE(Convert(contentB) <= Convert(contentB));
  EXPECT_FALSE(Convert(contentC) <= Convert(contentB));
  EXPECT_FALSE(Convert(contentB) <= Convert(contentA));
  EXPECT_TRUE(Convert(contentB) <= Convert(contentB));
  EXPECT_TRUE(Convert(contentB) <= Convert(contentC));

  EXPECT_TRUE(Convert(contentA) <= Convert(contentBA));
  EXPECT_TRUE(Convert(contentB) <= Convert(contentBA));
  EXPECT_FALSE(Convert(contentC) <= Convert(contentBA));
  EXPECT_FALSE(Convert(contentBA) <= Convert(contentA));
  EXPECT_FALSE(Convert(contentBA) <= Convert(contentB));
  EXPECT_TRUE(Convert(contentBA) <= Convert(contentC));

  EXPECT_TRUE(Convert(contentAA) <= Convert(contentB));
  EXPECT_FALSE(Convert(contentBA) <= Convert(contentB));
  EXPECT_FALSE(Convert(contentCA) <= Convert(contentB));
  EXPECT_FALSE(Convert(contentB) <= Convert(contentAA));
  EXPECT_TRUE(Convert(contentB) <= Convert(contentBA));
  EXPECT_TRUE(Convert(contentB) <= Convert(contentCA));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, OperatorGreaterThan)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_GT(Convert(contentB), Convert(contentA));
  EXPECT_GT(Convert(contentAB), Convert(contentA));
  EXPECT_GT(Convert(contentAB), Span<uint8_t>());

  EXPECT_FALSE(Convert(contentA) > Convert(contentB));
  EXPECT_FALSE(Convert(contentB) > Convert(contentB));
  EXPECT_TRUE(Convert(contentC) > Convert(contentB));
  EXPECT_TRUE(Convert(contentB) > Convert(contentA));
  EXPECT_FALSE(Convert(contentB) > Convert(contentB));
  EXPECT_FALSE(Convert(contentB) > Convert(contentC));

  EXPECT_FALSE(Convert(contentA) > Convert(contentBA));
  EXPECT_FALSE(Convert(contentB) > Convert(contentBA));
  EXPECT_TRUE(Convert(contentC) > Convert(contentBA));
  EXPECT_TRUE(Convert(contentBA) > Convert(contentA));
  EXPECT_TRUE(Convert(contentBA) > Convert(contentB));
  EXPECT_FALSE(Convert(contentBA) > Convert(contentC));

  EXPECT_FALSE(Convert(contentAA) > Convert(contentB));
  EXPECT_TRUE(Convert(contentBA) > Convert(contentB));
  EXPECT_TRUE(Convert(contentCA) > Convert(contentB));
  EXPECT_TRUE(Convert(contentB) > Convert(contentAA));
  EXPECT_FALSE(Convert(contentB) > Convert(contentBA));
  EXPECT_FALSE(Convert(contentB) > Convert(contentCA));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, OperatorGreaterThanOrEqual)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_GE(Convert(contentB), Convert(contentA));
  EXPECT_GE(Convert(contentAB), Convert(contentA));
  EXPECT_GE(Convert(contentAB), Span<uint8_t>());
  EXPECT_GE(Convert(contentA), Convert(contentA));

  EXPECT_FALSE(Convert(contentA) >= Convert(contentB));
  EXPECT_TRUE(Convert(contentB) >= Convert(contentB));
  EXPECT_TRUE(Convert(contentC) >= Convert(contentB));
  EXPECT_TRUE(Convert(contentB) >= Convert(contentA));
  EXPECT_TRUE(Convert(contentB) >= Convert(contentB));
  EXPECT_FALSE(Convert(contentB) >= Convert(contentC));

  EXPECT_FALSE(Convert(contentA) >= Convert(contentBA));
  EXPECT_FALSE(Convert(contentB) >= Convert(contentBA));
  EXPECT_TRUE(Convert(contentC) >= Convert(contentBA));
  EXPECT_TRUE(Convert(contentBA) >= Convert(contentA));
  EXPECT_TRUE(Convert(contentBA) >= Convert(contentB));
  EXPECT_FALSE(Convert(contentBA) >= Convert(contentC));

  EXPECT_FALSE(Convert(contentAA) >= Convert(contentB));
  EXPECT_TRUE(Convert(contentBA) >= Convert(contentB));
  EXPECT_TRUE(Convert(contentCA) >= Convert(contentB));
  EXPECT_TRUE(Convert(contentB) >= Convert(contentAA));
  EXPECT_FALSE(Convert(contentB) >= Convert(contentBA));
  EXPECT_FALSE(Convert(contentB) >= Convert(contentCA));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, OperatorEqual)
{
  std::array<uint8_t, 1> contentLowerA = {'a'};
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Convert(contentA) == Convert(contentA));
  EXPECT_TRUE(Span<uint8_t>().empty());
  EXPECT_FALSE(Convert(contentA).empty());
  EXPECT_FALSE(Convert(contentA) == Convert(contentB));
  EXPECT_FALSE(Convert(contentA) == Convert(contentLowerA));

  EXPECT_FALSE(Convert(contentA) == Convert(contentB));
  EXPECT_TRUE(Convert(contentB) == Convert(contentB));
  EXPECT_FALSE(Convert(contentC) == Convert(contentB));
  EXPECT_FALSE(Convert(contentB) == Convert(contentA));
  EXPECT_TRUE(Convert(contentB) == Convert(contentB));
  EXPECT_FALSE(Convert(contentB) == Convert(contentC));

  EXPECT_FALSE(Convert(contentA) == Convert(contentBA));
  EXPECT_FALSE(Convert(contentB) == Convert(contentBA));
  EXPECT_FALSE(Convert(contentC) == Convert(contentBA));
  EXPECT_FALSE(Convert(contentBA) == Convert(contentA));
  EXPECT_FALSE(Convert(contentBA) == Convert(contentB));
  EXPECT_FALSE(Convert(contentBA) == Convert(contentC));

  EXPECT_FALSE(Convert(contentAA) == Convert(contentB));
  EXPECT_FALSE(Convert(contentBA) == Convert(contentB));
  EXPECT_FALSE(Convert(contentCA) == Convert(contentB));
  EXPECT_FALSE(Convert(contentB) == Convert(contentAA));
  EXPECT_FALSE(Convert(contentB) == Convert(contentBA));
  EXPECT_FALSE(Convert(contentB) == Convert(contentCA));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, OperatorNotEqual)
{
  std::array<uint8_t, 1> contentLowerA = {'a'};
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_FALSE(Convert(contentA) != Convert(contentA));
  EXPECT_FALSE(!Span<uint8_t>().empty());
  EXPECT_TRUE(!Convert(contentA).empty());
  EXPECT_TRUE(Convert(contentA) != Convert(contentB));
  EXPECT_TRUE(Convert(contentA) != Convert(contentLowerA));

  EXPECT_TRUE(Convert(contentA) != Convert(contentB));
  EXPECT_FALSE(Convert(contentB) != Convert(contentB));
  EXPECT_TRUE(Convert(contentC) != Convert(contentB));
  EXPECT_TRUE(Convert(contentB) != Convert(contentA));
  EXPECT_FALSE(Convert(contentB) != Convert(contentB));
  EXPECT_TRUE(Convert(contentB) != Convert(contentC));

  EXPECT_TRUE(Convert(contentA) != Convert(contentBA));
  EXPECT_TRUE(Convert(contentB) != Convert(contentBA));
  EXPECT_TRUE(Convert(contentC) != Convert(contentBA));
  EXPECT_TRUE(Convert(contentBA) != Convert(contentA));
  EXPECT_TRUE(Convert(contentBA) != Convert(contentB));
  EXPECT_TRUE(Convert(contentBA) != Convert(contentC));

  EXPECT_TRUE(Convert(contentAA) != Convert(contentB));
  EXPECT_TRUE(Convert(contentBA) != Convert(contentB));
  EXPECT_TRUE(Convert(contentCA) != Convert(contentB));
  EXPECT_TRUE(Convert(contentB) != Convert(contentAA));
  EXPECT_TRUE(Convert(contentB) != Convert(contentBA));
  EXPECT_TRUE(Convert(contentB) != Convert(contentCA));
}
#endif
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// iterators
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestSpan, begin_empty)
{
  Span<char> span;

  ASSERT_EQ(span.end(), span.begin());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, begin_iterator_to_end)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  Span<uint8_t> span = Convert(content);

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

TEST(TestSpan, begin_iterator_to_end_with_mod)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 11> content2 = {'W', 'o', 'r', 'l', 'd', ' ', 'h', 'e', 'l', 'l', 'o'};
  Span<uint8_t> span = Convert(content);


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

TEST(TestSpan, begin_foreach_with_mod)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 11> content2 = {'W', 'o', 'r', 'l', 'd', ' ', 'h', 'e', 'l', 'l', 'o'};
  Span<uint8_t> span = Convert(content);


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


TEST(TestSpan, begin_empty_const)
{
  const Span<char> span;

  ASSERT_EQ(span.end(), span.begin());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpan, begin_iterator_to_end_const)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const Span<uint8_t> span = Convert(content);

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

TEST(TestSpan, begin_foreach_with_compare_const)
{
  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const Span<uint8_t> span = Convert(content);

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

// TEST(TestSpan, cbegin_empty)
//{
//   ReadOnlySpan<char> span;
//
//   ASSERT_EQ(span.cend(), span.cbegin());
// }
//
////---------------------------------------------------------------------------------------------------------------------------------------------------
//
// TEST(TestSpan, cbegin_iterator_to_cend_const)
//{
//  std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
//  const Span<uint8_t> span = Convert(content);
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
