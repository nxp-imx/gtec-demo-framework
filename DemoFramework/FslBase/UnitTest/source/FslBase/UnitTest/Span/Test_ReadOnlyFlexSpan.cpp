/****************************************************************************************************************************************************
 * Copyright 2021 NXP
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

#include <FslBase/Span/ReadOnlyFlexSpan.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestReadOnlyFlexSpan = TestFixtureFslBase;

  ReadOnlyFlexSpan Convert(const std::string& str)
  {
    return ReadOnlyFlexSpan(str.data(), str.size(), sizeof(char));
  }
}


TEST(TestReadOnlyFlexSpan, Construct)
{
  ReadOnlyFlexSpan span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), 0u);
}

TEST(TestReadOnlyFlexSpan, Construct_constexpr)
{
  constexpr ReadOnlyFlexSpan span;

  EXPECT_TRUE(span.empty());
  EXPECT_EQ(span.data(), nullptr);
  EXPECT_EQ(span.size(), 0u);
  EXPECT_EQ(span.length(), 0u);
  EXPECT_EQ(span.stride(), 0u);
}


TEST(TestReadOnlyFlexSpan, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  ReadOnlyFlexSpan span(psz, lenPsz, sizeof(char));

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), lenPsz);
  EXPECT_EQ(span.length(), lenPsz);
  EXPECT_EQ(span.stride(), sizeof(char));
}

TEST(TestReadOnlyFlexSpan, Construct_FromZeroTerminated_constexpr)
{
  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  constexpr static char data[11] = {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  constexpr static ReadOnlyFlexSpan span(data, 11, sizeof(char));

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.data(), data);
  EXPECT_EQ(span.size(), 11u);
  EXPECT_EQ(span.length(), 11u);
  EXPECT_EQ(span.stride(), sizeof(char));
}


TEST(TestReadOnlyFlexSpan, Construct_FromStr)
{
  std::string str("Hello world");
  ReadOnlyFlexSpan span = Convert(str);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), str.size());
  EXPECT_EQ(span.length(), str.size());
  EXPECT_EQ(span.stride(), sizeof(char));
}


TEST(TestReadOnlyFlexSpan, SubSpan)
{
  std::array<uint16_t, 10> testArray = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ReadOnlyFlexSpan span(testArray.data(), testArray.size(), sizeof(uint16_t));

  {
    ReadOnlyFlexSpan res = span.subspan();
    EXPECT_EQ(testArray.size(), res.size());
    EXPECT_EQ(&testArray[0], res.data());
    EXPECT_EQ(sizeof(uint16_t), res.stride());
    EXPECT_EQ(res.size(), res.length());
  }

  {
    ReadOnlyFlexSpan res = span.subspan(1u);
    EXPECT_EQ(9u, res.size());
    EXPECT_EQ(&testArray[1], res.data());
    EXPECT_EQ(sizeof(uint16_t), res.stride());
    EXPECT_EQ(res.size(), res.length());
  }
  {
    ReadOnlyFlexSpan res = span.subspan(9u);
    EXPECT_EQ(1u, res.size());
    EXPECT_EQ(&testArray[9], res.data());
    EXPECT_EQ(sizeof(uint16_t), res.stride());
    EXPECT_EQ(res.size(), res.length());
  }
  {
    ReadOnlyFlexSpan res = span.subspan(1u, 2u);
    EXPECT_EQ(2u, res.size());
    EXPECT_EQ(&testArray[1], res.data());
    EXPECT_EQ(sizeof(uint16_t), res.stride());
    EXPECT_EQ(res.size(), res.length());
  }
  {
    ReadOnlyFlexSpan res = span.subspan(9u, 2u);
    EXPECT_EQ(1u, res.size());
    EXPECT_EQ(&testArray[9], res.data());
    EXPECT_EQ(sizeof(uint16_t), res.stride());
    EXPECT_EQ(res.size(), res.length());
  }
  // its ok to read the last entry
  {
    ReadOnlyFlexSpan res = span.subspan(testArray.size());
    EXPECT_EQ(0u, res.size());
    EXPECT_EQ(sizeof(uint16_t), res.stride());
    EXPECT_EQ(res.size(), res.length());
  }
}

TEST(TestReadOnlyFlexSpan, SubSpan_Empty)
{
  ReadOnlyFlexSpan span;
  {
    ReadOnlyFlexSpan res = span.subspan();
    EXPECT_EQ(0u, res.size());
    EXPECT_EQ(0u, res.stride());
    EXPECT_EQ(0u, res.length());
  }
}


TEST(TestReadOnlyFlexSpan, SubSpan_InvalidPos)
{
  ReadOnlyFlexSpan span("0123456789", 10, sizeof(char));

  EXPECT_THROW(span.subspan(11u), std::out_of_range);
}
