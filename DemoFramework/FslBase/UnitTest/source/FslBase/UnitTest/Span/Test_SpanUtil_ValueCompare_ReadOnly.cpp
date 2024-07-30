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
  using TestSpanUtilValueCompareReadOnly = TestFixtureFslBase;

  template <typename T, std::size_t TSize>
  std::span<const T> Convert(const std::array<T, TSize>& array)
  {
    return std::span<const T>(array);
  }

}


TEST(TestSpanUtilValueCompareReadOnly, ValueEquals_Empty)
{
  std::span<const uint8_t> span1;
  std::span<const uint8_t> span2;

  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(span1, span2));
}

TEST(TestSpanUtilValueCompareReadOnly, UncheckedValueEquals_SpanTypes)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};

  std::span<const uint8_t> readOnlySpanA(contentA);
  std::span<const uint8_t> readOnlySpanB(contentB);
  std::span<const uint8_t> readOnlySpanC(contentC);

  std::span<uint8_t> spanA(contentA);
  std::span<uint8_t> spanB(contentB);
  std::span<uint8_t> spanC(contentC);

  constexpr std::size_t CharCount = 1;

  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(Convert(contentA), Convert(contentB), CharCount));
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueEquals(Convert(contentB), Convert(contentB), CharCount));
  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(Convert(contentC), Convert(contentB), CharCount));

  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(readOnlySpanA, readOnlySpanB, CharCount));
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueEquals(readOnlySpanB, readOnlySpanB, CharCount));
  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(readOnlySpanC, readOnlySpanB, CharCount));

  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(spanA, spanB, CharCount));
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueEquals(spanB, spanB, CharCount));
  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(spanC, spanB, CharCount));

  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(readOnlySpanA, spanB, CharCount));
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueEquals(readOnlySpanB, spanB, CharCount));
  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(readOnlySpanC, spanB, CharCount));

  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(spanA, readOnlySpanB, CharCount));
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueEquals(spanB, readOnlySpanB, CharCount));
  EXPECT_FALSE(Fsl::SpanUtil::UncheckedValueEquals(spanC, readOnlySpanB, CharCount));
}

TEST(TestSpanUtilValueCompareReadOnly, ValueEquals_SpanTypes)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};

  std::span<const uint8_t> readOnlySpanA(contentA);
  std::span<const uint8_t> readOnlySpanB(contentB);
  std::span<const uint8_t> readOnlySpanC(contentC);

  std::span<uint8_t> spanA(contentA);
  std::span<uint8_t> spanB(contentB);
  std::span<uint8_t> spanC(contentC);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(Convert(contentA), Convert(contentB)));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(Convert(contentB), Convert(contentB)));
  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(Convert(contentC), Convert(contentB)));

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(readOnlySpanA, readOnlySpanB));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(readOnlySpanB, readOnlySpanB));
  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(readOnlySpanC, readOnlySpanB));

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(spanA, spanB));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(spanB, spanB));
  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(spanC, spanB));

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(readOnlySpanA, spanB));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(readOnlySpanB, spanB));
  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(readOnlySpanC, spanB));

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(spanA, readOnlySpanB));
  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(spanB, readOnlySpanB));
  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(spanC, readOnlySpanB));
}

TEST(TestSpanUtilValueCompareReadOnly, opEqual1)
{
  const std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(span1, span2));
}

TEST(TestSpanUtilValueCompareReadOnly, opEqual2)
{
  const std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'D'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompareReadOnly, opEqual3)
{
  const std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 10> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompareReadOnly, opEqual4)
{
  const std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 12> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '2'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompareReadOnly, opEqual5)
{
  const std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  const std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompareReadOnly, opLess_Empty)
{
  const std::array<uint8_t, 0> content1{};
  const std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLess1)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'c'};

  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLess2)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};

  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLess3)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLess4)
{
  const std::array<uint8_t, 1> content1 = {'a'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLess5)
{
  const std::array<uint8_t, 2> content1 = {'a', 'a'};
  const std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}

TEST(TestSpanUtilValueCompareReadOnly, opLessOrEqual_Empty)
{
  const std::array<uint8_t, 0> content1{};
  const std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLessOrEqual1)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLessOrEqual2)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLessOrEqual3)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLessOrEqual4)
{
  const std::array<uint8_t, 1> content1 = {'a'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opLessOrEqual5)
{
  const std::array<uint8_t, 2> content1 = {'a', 'a'};
  const std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}

TEST(TestSpanUtilValueCompareReadOnly, opGreater_Empty)
{
  const std::array<uint8_t, 0> content1{};
  const std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreater1)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreater2)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreater3)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreater4)
{
  const std::array<uint8_t, 1> content1 = {'a'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreater5)
{
  const std::array<uint8_t, 2> content1 = {'a', 'a'};
  const std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}

TEST(TestSpanUtilValueCompareReadOnly, opGreaterOrEqual_Empty)
{
  const std::array<uint8_t, 0> content1{};
  const std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreaterOrEqual1)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreaterOrEqual2)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreaterOrEqual3)
{
  const std::array<uint8_t, 2> content1 = {'a', 'b'};
  const std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreaterOrEqual4)
{
  const std::array<uint8_t, 1> content1 = {'a'};
  const std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompareReadOnly, opGreaterOrEqual5)
{
  const std::array<uint8_t, 2> content1 = {'a', 'a'};
  const std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ValueCompare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompareReadOnly, ValueCompare)
{
  const std::array<uint8_t, 1> contentA = {'A'};
  const std::array<uint8_t, 1> contentB = {'B'};
  const std::array<uint8_t, 1> contentC = {'C'};
  const std::array<uint8_t, 2> contentAA = {'A', 'A'};
  const std::array<uint8_t, 2> contentBA = {'B', 'A'};
  const std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentC)) < 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentBA)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentBA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentC)) < 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAA), Convert(contentB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentCA), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentAA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) < 0);
}

TEST(TestSpanUtilValueCompareReadOnly, ValueCompare_Null)
{
  std::array<uint8_t, 0> contentA = {};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<char>(), std::span<char>()) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<uint8_t>(contentA.data(), contentA.size()), std::span<uint8_t>()) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<uint8_t>(), std::span<uint8_t>(contentA.data(), contentA.size())) == 0);
}


TEST(TestSpanUtilValueCompareReadOnly, ValueCompare_SpanTypes)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};

  std::span<const uint8_t> readOnlySpanA(contentA);
  std::span<const uint8_t> readOnlySpanB(contentB);
  std::span<const uint8_t> readOnlySpanC(contentC);

  std::span<uint8_t> spanA(contentA);
  std::span<uint8_t> spanB(contentB);
  std::span<uint8_t> spanC(contentC);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(readOnlySpanA, readOnlySpanB) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(readOnlySpanB, readOnlySpanB) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(readOnlySpanC, readOnlySpanB) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(spanA, spanB) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(spanB, spanB) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(spanC, spanB) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(readOnlySpanA, spanB) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(readOnlySpanB, spanB) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(readOnlySpanC, spanB) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(spanA, readOnlySpanB) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(spanB, readOnlySpanB) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(spanC, readOnlySpanB) > 0);
}

TEST(TestSpanUtilValueCompareReadOnly, UncheckedValueCompare_SpanTypes)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};

  std::span<const uint8_t> readOnlySpanA(contentA);
  std::span<const uint8_t> readOnlySpanB(contentB);
  std::span<const uint8_t> readOnlySpanC(contentC);

  std::span<uint8_t> spanA(contentA);
  std::span<uint8_t> spanB(contentB);
  std::span<uint8_t> spanC(contentC);

  constexpr std::size_t CharCount = 1;

  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(Convert(contentA), Convert(contentB), CharCount) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(Convert(contentB), Convert(contentB), CharCount) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(Convert(contentC), Convert(contentB), CharCount) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(readOnlySpanA, readOnlySpanB, CharCount) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(readOnlySpanB, readOnlySpanB, CharCount) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(readOnlySpanC, readOnlySpanB, CharCount) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(spanA, spanB, CharCount) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(spanB, spanB, CharCount) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(spanC, spanB, CharCount) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(readOnlySpanA, spanB, CharCount) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(readOnlySpanB, spanB, CharCount) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(readOnlySpanC, spanB, CharCount) > 0);

  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(spanA, readOnlySpanB, CharCount) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(spanB, readOnlySpanB, CharCount) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::UncheckedValueCompare(spanC, readOnlySpanB, CharCount) > 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Compare less than
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompareReadOnly, ValueCompareLessThan)
{
  const std::array<uint8_t, 1> contentA = {'A'};
  const std::array<uint8_t, 1> contentB = {'B'};
  const std::array<uint8_t, 1> contentC = {'C'};
  const std::array<uint8_t, 2> contentAA = {'A', 'A'};
  const std::array<uint8_t, 2> contentAB = {'A', 'B'};
  const std::array<uint8_t, 2> contentBA = {'B', 'A'};
  const std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentAB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<uint8_t>(), Convert(contentAB)) < 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentC)) < 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentBA)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentBA)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentA)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentC)) < 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAA), Convert(contentB)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentCA), Convert(contentB)) < 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentAA)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) < 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) < 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ValueCompare <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompareReadOnly, ValueCompareLessThanOrEqual)
{
  const std::array<uint8_t, 1> contentA = {'A'};
  const std::array<uint8_t, 1> contentB = {'B'};
  const std::array<uint8_t, 1> contentC = {'C'};
  const std::array<uint8_t, 2> contentAA = {'A', 'A'};
  const std::array<uint8_t, 2> contentAB = {'A', 'B'};
  const std::array<uint8_t, 2> contentBA = {'B', 'A'};
  const std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentAB)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<uint8_t>(), Convert(contentAB)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentA)) <= 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentC)) <= 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentBA)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentBA)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentA)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentC)) <= 0);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAA), Convert(contentB)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentCA), Convert(contentB)) <= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentAA)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) <= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) <= 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ValueCompare >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompareReadOnly, OperatorGreaterThan)
{
  const std::array<uint8_t, 1> contentA = {'A'};
  const std::array<uint8_t, 1> contentB = {'B'};
  const std::array<uint8_t, 1> contentC = {'C'};
  const std::array<uint8_t, 2> contentAA = {'A', 'A'};
  const std::array<uint8_t, 2> contentAB = {'A', 'B'};
  const std::array<uint8_t, 2> contentBA = {'B', 'A'};
  const std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAB), Convert(contentA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAB), std::span<uint8_t>()) > 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentC)) > 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentBA)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentBA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentA)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentC)) > 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentAA), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentCA), Convert(contentB)) > 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentAA)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) > 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) > 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ValueCompare >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompareReadOnly, OperatorGreaterThanOrEqual)
{
  const std::array<uint8_t, 1> contentA = {'A'};
  const std::array<uint8_t, 1> contentB = {'B'};
  const std::array<uint8_t, 1> contentC = {'C'};
  const std::array<uint8_t, 2> contentAA = {'A', 'A'};
  const std::array<uint8_t, 2> contentAB = {'A', 'B'};
  const std::array<uint8_t, 2> contentBA = {'B', 'A'};
  const std::array<uint8_t, 2> contentCA = {'C', 'A'};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAB), Convert(contentA)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentAB), std::span<uint8_t>()) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentA)) >= 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) >= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentC)) >= 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentBA)) >= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentBA)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentA)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) >= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentC)) >= 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentAA), Convert(contentB)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentCA), Convert(contentB)) >= 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentAA)) >= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) >= 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) >= 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ValueCompare !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompareReadOnly, OperatorEqual)
{
  const std::array<uint8_t, 1> contentLowerA = {'a'};
  const std::array<uint8_t, 1> contentA = {'A'};
  const std::array<uint8_t, 1> contentB = {'B'};
  const std::array<uint8_t, 1> contentC = {'C'};
  const std::array<uint8_t, 2> contentAA = {'A', 'A'};
  const std::array<uint8_t, 2> contentBA = {'B', 'A'};
  const std::array<uint8_t, 2> contentCA = {'C', 'A'};


  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentA)) == 0);
  EXPECT_TRUE(std::span<uint8_t>().empty());
  EXPECT_FALSE(Convert(contentA).empty());
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentLowerA)) == 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentB)) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentA)) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentC)) == 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentA), Convert(contentBA)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentC), Convert(contentBA)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentA)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentC)) == 0);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentAA), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentBA), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentCA), Convert(contentB)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentAA)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentBA)) == 0);
  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(Convert(contentB), Convert(contentCA)) == 0);
}
