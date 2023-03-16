/****************************************************************************************************************************************************
 * Copyright 2019, 2022 NXP
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
#include <FslBase/UnitTest/Helper/Common.hpp>
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
  EXPECT_EQ(span.length(), 0u);
}


TEST(TestReadOnlySpan, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  ReadOnlySpan<char> span(psz, lenPsz);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), lenPsz);
  EXPECT_EQ(span.length(), lenPsz);
}


TEST(TestReadOnlySpan, Construct_FromStr)
{
  std::string str("Hello world");
  ReadOnlySpan<char> span = Convert(str);

  EXPECT_FALSE(span.empty());
  EXPECT_NE(span.data(), nullptr);
  EXPECT_EQ(span.size(), str.size());
  EXPECT_EQ(span.length(), str.size());
}


TEST(TestReadOnlySpan, SubSpan)
{
  const char* const psz = "0123456789";
  ReadOnlySpan<char> span(psz, strlen(psz));

  EXPECT_EQ(span, span.subspan());
  EXPECT_EQ(ReadOnlySpan<char>("123456789", 9), span.subspan(1u));
  EXPECT_EQ(ReadOnlySpan<char>("9", 1), span.subspan(9u));

  EXPECT_EQ(ReadOnlySpan<char>("12", 2), span.subspan(1u, 2));
  EXPECT_EQ(ReadOnlySpan<char>("9", 1), span.subspan(9u, 2));

  // its ok to read the last char
  EXPECT_EQ(ReadOnlySpan<char>(), span.subspan(10u));
}

TEST(TestReadOnlySpan, SubSpan_Empty)
{
  ReadOnlySpan<char> span;
  EXPECT_EQ(ReadOnlySpan<char>(), span.subspan());
}


TEST(TestReadOnlySpan, SubSpan_InvalidPos)
{
  ReadOnlySpan<char> span("0123456789", 10);

  EXPECT_THROW(span.subspan(11u), std::out_of_range);
}


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

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestReadOnlySpan, opEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestReadOnlySpan, opEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestReadOnlySpan, opEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestReadOnlySpan, opEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestReadOnlySpan, opEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestReadOnlySpan, opNotEqual_Empty)
{
  std::string str1;
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestReadOnlySpan, opNotEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestReadOnlySpan, opNotEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestReadOnlySpan, opNotEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestReadOnlySpan, opNotEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestReadOnlySpan, opNotEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestReadOnlySpan, opLess_Empty)
{
  std::string str1;
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestReadOnlySpan, opLess1)
{
  std::string str1("ab");
  std::string str2("ac");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestReadOnlySpan, opLess2)
{
  std::string str1("ab");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestReadOnlySpan, opLess3)
{
  std::string str1("ab");
  std::string str2("ab");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestReadOnlySpan, opLess4)
{
  std::string str1("a");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestReadOnlySpan, opLess5)
{
  std::string str1("aa");
  std::string str2("a");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}

TEST(TestReadOnlySpan, opLessOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestReadOnlySpan, opLessOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestReadOnlySpan, opLessOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}


TEST(TestReadOnlySpan, opLessOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestReadOnlySpan, opLessOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestReadOnlySpan, opLessOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}

TEST(TestReadOnlySpan, opGreater_Empty)
{
  std::string str1;
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestReadOnlySpan, opGreater1)
{
  std::string str1("ab");
  std::string str2("ac");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestReadOnlySpan, opGreater2)
{
  std::string str1("ab");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}


TEST(TestReadOnlySpan, opGreater3)
{
  std::string str1("ab");
  std::string str2("ab");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestReadOnlySpan, opGreater4)
{
  std::string str1("a");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestReadOnlySpan, opGreater5)
{
  std::string str1("aa");
  std::string str2("a");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}

TEST(TestReadOnlySpan, opGreaterOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestReadOnlySpan, opGreaterOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestReadOnlySpan, opGreaterOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestReadOnlySpan, opGreaterOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestReadOnlySpan, opGreaterOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestReadOnlySpan, opGreaterOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  ReadOnlySpan<char> strView1 = Convert(str1);
  ReadOnlySpan<char> strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// compare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, compare)
{
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).compare(ReadOnlySpan<char>("B", 1)) < 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("B", 1)) == 0);
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1).compare(ReadOnlySpan<char>("B", 1)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("A", 1)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("C", 1)) < 0);

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).compare(ReadOnlySpan<char>("BA", 2)) < 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("BA", 2)) < 0);
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1).compare(ReadOnlySpan<char>("BA", 2)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).compare(ReadOnlySpan<char>("A", 1)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).compare(ReadOnlySpan<char>("B", 1)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).compare(ReadOnlySpan<char>("C", 1)) < 0);

  EXPECT_TRUE(ReadOnlySpan<char>("AA", 2).compare(ReadOnlySpan<char>("B", 1)) < 0);
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).compare(ReadOnlySpan<char>("B", 1)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("CA", 2).compare(ReadOnlySpan<char>("B", 1)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("AA", 2)) > 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("BA", 2)) < 0);
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).compare(ReadOnlySpan<char>("CA", 2)) < 0);
}

TEST(TestReadOnlySpan, compare_Null)
{
  EXPECT_TRUE(ReadOnlySpan<char>().compare(ReadOnlySpan<char>()) == 0);
  EXPECT_TRUE(ReadOnlySpan<char>("", 0).compare(ReadOnlySpan<char>()) == 0);
  EXPECT_TRUE(ReadOnlySpan<char>().compare(ReadOnlySpan<char>("", 0)) == 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// starts_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, starts_with)
{
  EXPECT_TRUE(ReadOnlySpan<char>().starts_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>().starts_with(ReadOnlySpan<char>("", 0)));

  EXPECT_TRUE(ReadOnlySpan<char>("", 0).starts_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("", 0).starts_with(ReadOnlySpan<char>("", 0)));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).starts_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).starts_with(ReadOnlySpan<char>("", 0)));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).starts_with(ReadOnlySpan<char>("A", 1)));

  EXPECT_TRUE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>("", 0)));
  EXPECT_TRUE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>("A", 1)));
  EXPECT_TRUE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>("AB", 2)));

  EXPECT_TRUE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("", 0)));
  EXPECT_TRUE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("A", 1)));
  EXPECT_TRUE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("AB", 2)));
  EXPECT_TRUE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("ABC", 3)));

  EXPECT_FALSE(ReadOnlySpan<char>().starts_with(ReadOnlySpan<char>("A", 1)));

  EXPECT_FALSE(ReadOnlySpan<char>("", 0).starts_with(ReadOnlySpan<char>("A", 1)));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 2).starts_with(ReadOnlySpan<char>("B", 1)));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 2).starts_with(ReadOnlySpan<char>("AB", 2)));

  EXPECT_FALSE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>("B", 1)));
  EXPECT_FALSE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>("AC", 2)));
  EXPECT_FALSE(ReadOnlySpan<char>("AB", 2).starts_with(ReadOnlySpan<char>("ABC", 3)));

  EXPECT_FALSE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("B", 1)));
  EXPECT_FALSE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("AC", 2)));
  EXPECT_FALSE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("ABD", 3)));
  EXPECT_FALSE(ReadOnlySpan<char>("ABC", 3).starts_with(ReadOnlySpan<char>("ABDA", 4)));
}

TEST(TestReadOnlySpan, starts_with_value)
{
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).starts_with('A'));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).starts_with('B'));

  EXPECT_TRUE(ReadOnlySpan<char>("AB", 2).starts_with('A'));
  EXPECT_TRUE(ReadOnlySpan<char>("BC", 2).starts_with('B'));

  EXPECT_FALSE(ReadOnlySpan<char>().starts_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>().starts_with('a'));

  EXPECT_FALSE(ReadOnlySpan<char>("", 0).starts_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>("", 0).starts_with('a'));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1).starts_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1).starts_with('a'));

  EXPECT_FALSE(ReadOnlySpan<char>("AB", 2).starts_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>("AB", 2).starts_with('a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ends_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, ends_with)
{
  EXPECT_TRUE(ReadOnlySpan<char>().ends_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>().ends_with(ReadOnlySpan<char>("", 0)));

  EXPECT_TRUE(ReadOnlySpan<char>("", 0).ends_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("", 0).ends_with(ReadOnlySpan<char>("", 0)));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).ends_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).ends_with(ReadOnlySpan<char>("", 0)));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).ends_with(ReadOnlySpan<char>("A", 1)));

  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>("", 0)));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>("A", 1)));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>("BA", 2)));

  EXPECT_TRUE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>()));
  EXPECT_TRUE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("", 0)));
  EXPECT_TRUE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("A", 1)));
  EXPECT_TRUE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("BA", 2)));
  EXPECT_TRUE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("CBA", 3)));

  EXPECT_FALSE(ReadOnlySpan<char>().ends_with(ReadOnlySpan<char>("A", 1)));

  EXPECT_FALSE(ReadOnlySpan<char>("", 0).ends_with(ReadOnlySpan<char>("A", 1)));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1).ends_with(ReadOnlySpan<char>("B", 1)));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1).ends_with(ReadOnlySpan<char>("BA", 2)));

  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>("B", 1)));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>("CA", 2)));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2).ends_with(ReadOnlySpan<char>("CBA", 3)));

  EXPECT_FALSE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("B", 1)));
  EXPECT_FALSE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("CA", 2)));
  EXPECT_FALSE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("DBA", 3)));
  EXPECT_FALSE(ReadOnlySpan<char>("CBA", 3).ends_with(ReadOnlySpan<char>("ADBA", 4)));
}

TEST(TestReadOnlySpan, ends_with_value)
{
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1).ends_with('A'));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1).ends_with('B'));

  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2).ends_with('A'));
  EXPECT_TRUE(ReadOnlySpan<char>("CB", 2).ends_with('B'));

  EXPECT_FALSE(ReadOnlySpan<char>().ends_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>().ends_with('a'));

  EXPECT_FALSE(ReadOnlySpan<char>("", 0).ends_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>("", 0).ends_with('a'));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1).ends_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1).ends_with('a'));

  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2).ends_with(' '));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2).ends_with('a'));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// find
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, find_char)
{
  EXPECT_EQ(0u, ReadOnlySpan<char>("a", 1).find('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).find('a', 1u));

  EXPECT_EQ(0u, ReadOnlySpan<char>("aa", 2).find('a'));
  EXPECT_EQ(1u, ReadOnlySpan<char>("aa", 2).find('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("aa", 2).find('a', 2u));

  EXPECT_EQ(0u, ReadOnlySpan<char>("aba", 3).find('a'));
  EXPECT_EQ(2u, ReadOnlySpan<char>("aba", 3).find('a', 1u));
  EXPECT_EQ(2u, ReadOnlySpan<char>("aba", 3).find('a', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("aba", 3).find('a', 3u));

  EXPECT_EQ(2u, ReadOnlySpan<char>("cba", 3).find('a'));
  EXPECT_EQ(2u, ReadOnlySpan<char>("cba", 3).find('a', 1u));
  EXPECT_EQ(2u, ReadOnlySpan<char>("cba", 3).find('a', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("cba", 3).find('a', 3u));


  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().find(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().find('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().find(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().find('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().find(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().find('a', 2u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).find(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).find('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).find(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).find('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).find(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).find('a', 2u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).find(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("b", 1).find('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).find(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("b", 1).find('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).find(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("b", 1).find('a', 2u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("ab", 2).find(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("bc", 2).find('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("ab", 2).find(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("bc", 2).find('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("ab", 2).find(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("bc", 2).find('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// rfind
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, rfind_char)
{
  EXPECT_EQ(0u, ReadOnlySpan<char>("a", 1).rfind('a'));
  EXPECT_EQ(0u, ReadOnlySpan<char>("a", 1).rfind('a', 0u));
  EXPECT_EQ(0u, ReadOnlySpan<char>("a", 1).rfind('a', 1u));

  EXPECT_EQ(1u, ReadOnlySpan<char>("aa", 2).rfind('a'));
  EXPECT_EQ(0u, ReadOnlySpan<char>("aa", 2).rfind('a', 0u));
  EXPECT_EQ(1u, ReadOnlySpan<char>("aa", 2).rfind('a', 1u));
  EXPECT_EQ(1u, ReadOnlySpan<char>("aa", 2).rfind('a', 2u));

  EXPECT_EQ(2u, ReadOnlySpan<char>("aba", 3).rfind('a'));
  EXPECT_EQ(0u, ReadOnlySpan<char>("aba", 3).rfind('a', 0u));
  EXPECT_EQ(0u, ReadOnlySpan<char>("aba", 3).rfind('a', 1u));
  EXPECT_EQ(2u, ReadOnlySpan<char>("aba", 3).rfind('a', 2u));
  EXPECT_EQ(2u, ReadOnlySpan<char>("aba", 3).rfind('a', 3u));

  EXPECT_EQ(2u, ReadOnlySpan<char>("cba", 3).rfind('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("cba", 3).rfind('a', 0u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("cba", 3).rfind('a', 1u));
  EXPECT_EQ(2u, ReadOnlySpan<char>("cba", 3).rfind('a', 2u));
  EXPECT_EQ(2u, ReadOnlySpan<char>("cba", 3).rfind('a', 3u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().rfind(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().rfind('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().rfind(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().rfind('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().rfind(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>().rfind('a', 2u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).rfind(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).rfind('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).rfind(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).rfind('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).rfind(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("", 0).rfind('a', 2u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).rfind(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("b", 1).rfind('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).rfind(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("b", 1).rfind('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("a", 1).rfind(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("b", 1).rfind('a', 2u));

  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("ab", 2).rfind(' '));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("bc", 2).rfind('a'));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("ab", 2).rfind(' ', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("bc", 2).rfind('a', 1u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("ab", 2).rfind(' ', 2u));
  EXPECT_EQ(ReadOnlySpan<char>::extent, ReadOnlySpan<char>("bc", 2).rfind('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, OperatorLessThan)
{
  EXPECT_LT(ReadOnlySpan<char>("A", 1), ReadOnlySpan<char>("B", 1));
  EXPECT_LT(ReadOnlySpan<char>("A", 1), ReadOnlySpan<char>("AB", 2));
  EXPECT_LT(ReadOnlySpan<char>("", 0), ReadOnlySpan<char>("AB", 2));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) < ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("C", 1) < ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("C", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) < ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("C", 1) < ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) < ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) < ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) < ReadOnlySpan<char>("C", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("AA", 2) < ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) < ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("CA", 2) < ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("AA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) < ReadOnlySpan<char>("CA", 2));
}
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, OperatorLessThanOrEqual)
{
  EXPECT_LE(ReadOnlySpan<char>("A", 1), ReadOnlySpan<char>("B", 1));
  EXPECT_LE(ReadOnlySpan<char>("A", 1), ReadOnlySpan<char>("AB", 2));
  EXPECT_LE(ReadOnlySpan<char>("", 0), ReadOnlySpan<char>("AB", 2));
  EXPECT_LE(ReadOnlySpan<char>("A", 1), ReadOnlySpan<char>("A", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) <= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("C", 1) <= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("C", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) <= ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("C", 1) <= ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) <= ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) <= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) <= ReadOnlySpan<char>("C", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("AA", 2) <= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) <= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("CA", 2) <= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("AA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) <= ReadOnlySpan<char>("CA", 2));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, OperatorGreaterThan)
{
  EXPECT_GT(ReadOnlySpan<char>("B", 1), ReadOnlySpan<char>("A", 1));
  EXPECT_GT(ReadOnlySpan<char>("AB", 2), ReadOnlySpan<char>("A", 1));
  EXPECT_GT(ReadOnlySpan<char>("AB", 2), ReadOnlySpan<char>("", 0));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) > ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1) > ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("C", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) > ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1) > ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) > ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) > ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) > ReadOnlySpan<char>("C", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("AA", 2) > ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) > ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("CA", 2) > ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("AA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) > ReadOnlySpan<char>("CA", 2));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(ReadOnlySpan<char>("B", 1), ReadOnlySpan<char>("A", 1));
  EXPECT_GE(ReadOnlySpan<char>("AB", 2), ReadOnlySpan<char>("A", 1));
  EXPECT_GE(ReadOnlySpan<char>("AB", 2), ReadOnlySpan<char>("", 0));
  EXPECT_GE(ReadOnlySpan<char>("A", 1), ReadOnlySpan<char>("A", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) >= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1) >= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("C", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) >= ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1) >= ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) >= ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) >= ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) >= ReadOnlySpan<char>("C", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("AA", 2) >= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) >= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("CA", 2) >= ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("AA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) >= ReadOnlySpan<char>("CA", 2));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, OperatorEqual)
{
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) == ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("", 0) == ReadOnlySpan<char>("", 0));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) == ReadOnlySpan<char>("", 0));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) == ReadOnlySpan<char>("a", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) == ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("C", 1) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("C", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) == ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("C", 1) == ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) == ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) == ReadOnlySpan<char>("C", 1));

  EXPECT_FALSE(ReadOnlySpan<char>("AA", 2) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("BA", 2) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("CA", 2) == ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("AA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("BA", 2));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) == ReadOnlySpan<char>("CA", 2));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, OperatorNotEqual)
{
  EXPECT_FALSE(ReadOnlySpan<char>("A", 1) != ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("", 0) != ReadOnlySpan<char>("", 0));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) != ReadOnlySpan<char>("", 0));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) != ReadOnlySpan<char>("a", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) != ReadOnlySpan<char>("B", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("A", 1));
  EXPECT_FALSE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("C", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("A", 1) != ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("C", 1) != ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) != ReadOnlySpan<char>("A", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) != ReadOnlySpan<char>("C", 1));

  EXPECT_TRUE(ReadOnlySpan<char>("AA", 2) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("BA", 2) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("CA", 2) != ReadOnlySpan<char>("B", 1));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("AA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("BA", 2));
  EXPECT_TRUE(ReadOnlySpan<char>("B", 1) != ReadOnlySpan<char>("CA", 2));
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

TEST(TestReadOnlySpan, cbegin_empty)
{
  ReadOnlySpan<char> span;

  ASSERT_EQ(span.cend(), span.cbegin());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestReadOnlySpan, cbegin_iterator_to_cend)
{
  const std::array<uint8_t, 11> content = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  ReadOnlySpan<uint8_t> span = Convert(content);

  // NOLINTNEXTLINE(readability-qualified-auto)
  auto itrSource = content.begin();
  // NOLINTNEXTLINE(readability-qualified-auto)
  const auto itrSourceEnd = content.end();

  auto itr = span.cbegin();
  auto itrEnd = span.cend();
  while (itr != itrEnd && itrSource != itrSourceEnd)
  {
    EXPECT_EQ(*itrSource, *itr);
    ++itrSource;
    ++itr;
  }
  ASSERT_EQ(itrEnd, itr);
  ASSERT_EQ(itrSourceEnd, itrSource);
}
