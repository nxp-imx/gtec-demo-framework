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
  using TestSpanUtilValueCompare = TestFixtureFslBase;

  template <typename T, std::size_t TSize>
  std::span<T> Convert(std::array<T, TSize>& array)
  {
    return std::span<T>(array);
  }

}


TEST(TestSpanUtilValueCompare, ValueEquals_Empty)
{
  std::span<uint8_t> span1;
  std::span<uint8_t> span2;

  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(span1, span2));
}

TEST(TestSpanUtilValueCompare, UncheckedValueEquals_SpanTypes)
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

TEST(TestSpanUtilValueCompare, ValueEquals_SpanTypes)
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

TEST(TestSpanUtilValueCompare, opEqual1)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueEquals(span1, span2));
}

TEST(TestSpanUtilValueCompare, opEqual2)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 11> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'D'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompare, opEqual3)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 10> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompare, opEqual4)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 12> content2 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '2'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompare, opEqual5)
{
  std::array<uint8_t, 11> content1 = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueEquals(span1, span2));
}


TEST(TestSpanUtilValueCompare, opLess_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompare, opLess1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};

  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompare, opLess2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};

  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompare, opLess3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompare, opLess4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}


TEST(TestSpanUtilValueCompare, opLess5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) < 0);
}

TEST(TestSpanUtilValueCompare, opLessOrEqual_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompare, opLessOrEqual1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompare, opLessOrEqual2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompare, opLessOrEqual3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompare, opLessOrEqual4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}


TEST(TestSpanUtilValueCompare, opLessOrEqual5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) <= 0);
}

TEST(TestSpanUtilValueCompare, opGreater_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompare, opGreater1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompare, opGreater2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompare, opGreater3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompare, opGreater4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}


TEST(TestSpanUtilValueCompare, opGreater5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) > 0);
}

TEST(TestSpanUtilValueCompare, opGreaterOrEqual_Empty)
{
  std::array<uint8_t, 0> content1{};
  std::array<uint8_t, 0> content2{};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompare, opGreaterOrEqual1)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'c'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompare, opGreaterOrEqual2)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompare, opGreaterOrEqual3)
{
  std::array<uint8_t, 2> content1 = {'a', 'b'};
  std::array<uint8_t, 2> content2 = {'a', 'b'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompare, opGreaterOrEqual4)
{
  std::array<uint8_t, 1> content1 = {'a'};
  std::array<uint8_t, 2> content2 = {'a', 'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_FALSE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}


TEST(TestSpanUtilValueCompare, opGreaterOrEqual5)
{
  std::array<uint8_t, 2> content1 = {'a', 'a'};
  std::array<uint8_t, 1> content2 = {'a'};
  auto span1 = Convert(content1);
  auto span2 = Convert(content2);

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(span1, span2) >= 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ValueCompare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestSpanUtilValueCompare, ValueCompare)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

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

TEST(TestSpanUtilValueCompare, ValueCompare_Null)
{
  std::array<uint8_t, 0> contentA = {};

  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<char>(), std::span<char>()) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<uint8_t>(contentA.data(), contentA.size()), std::span<uint8_t>()) == 0);
  EXPECT_TRUE(Fsl::SpanUtil::ValueCompare(std::span<uint8_t>(), std::span<uint8_t>(contentA.data(), contentA.size())) == 0);
}


TEST(TestSpanUtilValueCompare, ValueCompare_SpanTypes)
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

TEST(TestSpanUtilValueCompare, UncheckedValueCompare_SpanTypes)
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

TEST(TestSpanUtilValueCompare, ValueCompareLessThan)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

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

TEST(TestSpanUtilValueCompare, ValueCompareLessThanOrEqual)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

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

TEST(TestSpanUtilValueCompare, OperatorGreaterThan)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

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

TEST(TestSpanUtilValueCompare, OperatorGreaterThanOrEqual)
{
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentAB = {'A', 'B'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};

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

TEST(TestSpanUtilValueCompare, OperatorEqual)
{
  std::array<uint8_t, 1> contentLowerA = {'a'};
  std::array<uint8_t, 1> contentA = {'A'};
  std::array<uint8_t, 1> contentB = {'B'};
  std::array<uint8_t, 1> contentC = {'C'};
  std::array<uint8_t, 2> contentAA = {'A', 'A'};
  std::array<uint8_t, 2> contentBA = {'B', 'A'};
  std::array<uint8_t, 2> contentCA = {'C', 'A'};


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

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// StartsWith
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, StartsWith)
{
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>(), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>(), std::span<const char>("", 0)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("", 0), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("", 0), std::span<const char>("", 0)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 1), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 1), std::span<const char>("", 0)));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 1), std::span<const char>("A", 1)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>("", 0)));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>("A", 1)));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>("AB", 2)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("", 0)));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("A", 1)));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("AB", 2)));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("ABC", 3)));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>(), std::span<const char>("A", 1)));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("", 0), std::span<const char>("A", 1)));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 2), std::span<const char>("B", 1)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 2), std::span<const char>("AB", 2)));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>("B", 1)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>("AC", 2)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), std::span<const char>("ABC", 3)));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("B", 1)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("AC", 2)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("ABD", 3)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("ABC", 3), std::span<const char>("ABDA", 4)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, StartsWith_SpanTypes)
{
  std::array<char, 3> arrayABC = {'A', 'B', 'C'};
  std::array<char, 2> arrayAB = {'A', 'B'};
  std::array<char, 2> arrayAC = {'A', 'C'};

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>(arrayABC), std::span<const char>(arrayAB)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>(arrayABC), std::span<const char>(arrayAC)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayABC), std::span<char>(arrayAB)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayABC), std::span<char>(arrayAC)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>(arrayABC), std::span<char>(arrayAB)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>(arrayABC), std::span<char>(arrayAC)));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayABC), std::span<const char>(arrayAB)));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayABC), std::span<const char>(arrayAC)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// StartsWith value
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestReadOnlySpan, StartsWith_value1)
{
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 1), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("B", 1), 'B'));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<const char>("BC", 2), 'B'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>(), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>(), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("", 0), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("", 0), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 1), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("A", 1), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<const char>("AB", 2), 'a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, StartsWith_value2)
{
  std::array<char, 1> arrayA = {'A'};
  std::array<char, 1> arrayB = {'B'};
  std::array<char, 2> arrayAB = {'A', 'B'};
  std::array<char, 2> arrayBC = {'B', 'C'};
  std::array<char, 0> arrayEmpty = {};

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayA), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayB), 'B'));

  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayAB), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayBC), 'B'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayEmpty), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayEmpty), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayA), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayA), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayAB), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::StartsWith(std::span<char>(arrayAB), 'a'));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// EndsWith
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, EndsWith)
{
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>(), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>(), std::span<const char>("", 0)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("", 0), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("", 0), std::span<const char>("", 0)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), std::span<const char>("", 0)));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), std::span<const char>("A", 1)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>("", 0)));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>("A", 1)));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>("BA", 2)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>()));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("", 0)));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("A", 1)));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("BA", 2)));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("CBA", 3)));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>(), std::span<const char>("A", 1)));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("", 0), std::span<const char>("A", 1)));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), std::span<const char>("B", 1)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), std::span<const char>("BA", 2)));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>("B", 1)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>("CA", 2)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), std::span<const char>("CBA", 3)));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("B", 1)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("CA", 2)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("DBA", 3)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("CBA", 3), std::span<const char>("ADBA", 4)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, EndsWith_SpanTypes)
{
  std::array<char, 3> arrayCBA = {'C', 'B', 'A'};
  std::array<char, 2> arrayBA = {'B', 'A'};
  std::array<char, 2> arrayCA = {'C', 'A'};

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>(arrayCBA), std::span<const char>(arrayBA)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>(arrayCBA), std::span<const char>(arrayCA)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayCBA), std::span<char>(arrayBA)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayCBA), std::span<char>(arrayCA)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>(arrayCBA), std::span<char>(arrayBA)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>(arrayCBA), std::span<char>(arrayCA)));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayCBA), std::span<const char>(arrayBA)));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayCBA), std::span<const char>(arrayCA)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, EndsWith_value1)
{
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("B", 1), 'B'));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<const char>("CB", 2), 'B'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>(), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>(), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("", 0), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("", 0), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("A", 1), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<const char>("BA", 2), 'a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, EndsWith_value2)
{
  std::array<char, 1> arrayA = {'A'};
  std::array<char, 1> arrayB = {'B'};
  std::array<char, 2> arrayBA = {'B', 'A'};
  std::array<char, 2> arrayCB = {'C', 'B'};
  std::array<char, 0> arrayEmpty = {};

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayA), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayB), 'B'));

  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayBA), 'A'));
  EXPECT_TRUE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayCB), 'B'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayEmpty), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayEmpty), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayA), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayA), 'a'));

  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayBA), ' '));
  EXPECT_FALSE(Fsl::SpanUtil::EndsWith(std::span<char>(arrayBA), 'a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Find
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, Find_value)
{
  EXPECT_EQ(0u, Fsl::SpanUtil::Find(std::span<const char>("a", 1), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("a", 1), 'a', 1u));

  EXPECT_EQ(0u, Fsl::SpanUtil::Find(std::span<const char>("aa", 2), 'a'));
  EXPECT_EQ(1u, Fsl::SpanUtil::Find(std::span<const char>("aa", 2), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("aa", 2), 'a', 2u));

  EXPECT_EQ(0u, Fsl::SpanUtil::Find(std::span<const char>("aba", 3), 'a'));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>("aba", 3), 'a', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>("aba", 3), 'a', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("aba", 3), 'a', 3u));

  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>("cba", 3), 'a'));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>("cba", 3), 'a', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>("cba", 3), 'a', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("cba", 3), 'a', 3u));


  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(), 'a', 2u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("", 0), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("", 0), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("", 0), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("", 0), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("", 0), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("", 0), 'a', 2u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("a", 1), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("b", 1), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("a", 1), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("b", 1), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("a", 1), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("b", 1), 'a', 2u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("ab", 2), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("bc", 2), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("ab", 2), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("bc", 2), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("ab", 2), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>("bc", 2), 'a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, Find_value_spantypes)
{
  std::array<char, 3> arrayCBA = {'C', 'B', 'A'};

  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>(arrayCBA), 'A'));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>(arrayCBA), 'A', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<const char>(arrayCBA), 'A', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<const char>(arrayCBA), 'A', 3u));


  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<char>(arrayCBA), 'A'));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<char>(arrayCBA), 'A', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::Find(std::span<char>(arrayCBA), 'A', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::Find(std::span<char>(arrayCBA), 'A', 3u));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// rfind
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, rfind_char)
{
  EXPECT_EQ(0u, Fsl::SpanUtil::RFind(std::span<const char>("a", 1), 'a'));
  EXPECT_EQ(0u, Fsl::SpanUtil::RFind(std::span<const char>("a", 1), 'a', 0u));
  EXPECT_EQ(0u, Fsl::SpanUtil::RFind(std::span<const char>("a", 1), 'a', 1u));

  EXPECT_EQ(1u, Fsl::SpanUtil::RFind(std::span<const char>("aa", 2), 'a'));
  EXPECT_EQ(0u, Fsl::SpanUtil::RFind(std::span<const char>("aa", 2), 'a', 0u));
  EXPECT_EQ(1u, Fsl::SpanUtil::RFind(std::span<const char>("aa", 2), 'a', 1u));
  EXPECT_EQ(1u, Fsl::SpanUtil::RFind(std::span<const char>("aa", 2), 'a', 2u));

  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>("aba", 3), 'a'));
  EXPECT_EQ(0u, Fsl::SpanUtil::RFind(std::span<const char>("aba", 3), 'a', 0u));
  EXPECT_EQ(0u, Fsl::SpanUtil::RFind(std::span<const char>("aba", 3), 'a', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>("aba", 3), 'a', 2u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>("aba", 3), 'a', 3u));

  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>("cba", 3), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("cba", 3), 'a', 0u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("cba", 3), 'a', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>("cba", 3), 'a', 2u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>("cba", 3), 'a', 3u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(), 'a', 2u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("", 0), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("", 0), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("", 0), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("", 0), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("", 0), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("", 0), 'a', 2u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("a", 1), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("b", 1), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("a", 1), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("b", 1), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("a", 1), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("b", 1), 'a', 2u));

  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("ab", 2), ' '));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("bc", 2), 'a'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("ab", 2), ' ', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("bc", 2), 'a', 1u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("ab", 2), ' ', 2u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>("bc", 2), 'a', 2u));
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, rfind_char_SpanTypes)
{
  std::array<char, 3> arrayCBA = {'C', 'B', 'A'};

  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>(arrayCBA), 'A'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(arrayCBA), 'A', 0u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<const char>(arrayCBA), 'A', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>(arrayCBA), 'A', 2u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<const char>(arrayCBA), 'A', 3u));

  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<char>(arrayCBA), 'A'));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<char>(arrayCBA), 'A', 0u));
  EXPECT_EQ(std::span<const char>::extent, Fsl::SpanUtil::RFind(std::span<char>(arrayCBA), 'A', 1u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<char>(arrayCBA), 'A', 2u));
  EXPECT_EQ(2u, Fsl::SpanUtil::RFind(std::span<char>(arrayCBA), 'A', 3u));
}
