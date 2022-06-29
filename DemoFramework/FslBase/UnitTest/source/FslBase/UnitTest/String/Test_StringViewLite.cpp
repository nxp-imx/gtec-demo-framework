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

#include <FslBase/Log/String/LogStringViewLite.hpp>
#include <FslBase/String/StringViewLite.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestString_StringViewLite = TestFixtureFslBase;

  StringViewLite Convert(const std::string& str)
  {
    return StringViewLite(str.data(), str.size());
  }
}


TEST(TestString_StringViewLite, Construct)
{
  StringViewLite strView;

  EXPECT_TRUE(strView.empty());
  EXPECT_EQ(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(TestString_StringViewLite, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  StringViewLite strView(psz, lenPsz);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), lenPsz);
  EXPECT_EQ(strView.length(), lenPsz);
}


TEST(TestString_StringViewLite, Construct_FromStr)
{
  std::string str("Hello world");
  StringViewLite strView = Convert(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}


TEST(TestString_StringViewLite, SubStr)
{
  StringViewLite strView("0123456789");

  EXPECT_EQ(strView, strView.substr());
  EXPECT_EQ("123456789", strView.substr(1u));
  EXPECT_EQ("9", strView.substr(9u));

  EXPECT_EQ("12", strView.substr(1u, 2));
  EXPECT_EQ("9", strView.substr(9u, 2));

  // its ok to read the last char
  EXPECT_EQ("", strView.substr(10u));
}

TEST(TestString_StringViewLite, SubStr_Empty)
{
  StringViewLite strView;

  EXPECT_EQ(StringViewLite(), strView.substr());
  EXPECT_EQ("", strView.substr());
}


TEST(TestString_StringViewLite, SubStr_InvalidPos)
{
  StringViewLite strView("0123456789");

  EXPECT_THROW(strView.substr(11u), std::out_of_range);
}


TEST(TestString_StringViewLite, front)
{
  std::string str("Hello world");
  StringViewLite strView = Convert(str);

  EXPECT_EQ(strView.front(), str.front());
}


TEST(TestString_StringViewLite, back)
{
  std::string str("Hello world");
  StringViewLite strView = Convert(str);

  EXPECT_EQ(strView.back(), str.back());
}


TEST(TestString_StringViewLite, opIndex)
{
  std::string str("Hello world");
  StringViewLite strView = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(strView[i], str[i]);
  }
}


TEST(TestString_StringViewLite, At)
{
  std::string str("Hello world");
  StringViewLite strView = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(strView.at(i), str.at(i));
  }
}


TEST(TestString_StringViewLite, At_OutOfBounds)
{
  std::string str("Hello world");
  StringViewLite strView = Convert(str);

  EXPECT_THROW(strView.at(str.size()), std::out_of_range);
}


TEST(TestString_StringViewLite, At_Empty_OutOfBounds)
{
  std::string str;
  StringViewLite strView = Convert(str);

  EXPECT_THROW(strView.at(str.size()), std::out_of_range);
}


TEST(TestString_StringViewLite, opEqual_Empty)
{
  std::string str1;
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_StringViewLite, opEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_StringViewLite, opEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringViewLite, opEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringViewLite, opEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringViewLite, opEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringViewLite, opNotEqual_Empty)
{
  std::string str1;
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_StringViewLite, opNotEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_StringViewLite, opNotEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringViewLite, opNotEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringViewLite, opNotEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringViewLite, opNotEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringViewLite, opLess_Empty)
{
  std::string str1;
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_StringViewLite, opLess1)
{
  std::string str1("ab");
  std::string str2("ac");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_StringViewLite, opLess2)
{
  std::string str1("ab");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_StringViewLite, opLess3)
{
  std::string str1("ab");
  std::string str2("ab");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_StringViewLite, opLess4)
{
  std::string str1("a");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_StringViewLite, opLess5)
{
  std::string str1("aa");
  std::string str2("a");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}

TEST(TestString_StringViewLite, opLessOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringViewLite, opLessOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringViewLite, opLessOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}


TEST(TestString_StringViewLite, opLessOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringViewLite, opLessOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringViewLite, opLessOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}

TEST(TestString_StringViewLite, opGreater_Empty)
{
  std::string str1;
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringViewLite, opGreater1)
{
  std::string str1("ab");
  std::string str2("ac");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringViewLite, opGreater2)
{
  std::string str1("ab");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}


TEST(TestString_StringViewLite, opGreater3)
{
  std::string str1("ab");
  std::string str2("ab");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringViewLite, opGreater4)
{
  std::string str1("a");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringViewLite, opGreater5)
{
  std::string str1("aa");
  std::string str2("a");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}

TEST(TestString_StringViewLite, opGreaterOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_StringViewLite, opGreaterOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_StringViewLite, opGreaterOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_StringViewLite, opGreaterOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_StringViewLite, opGreaterOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_StringViewLite, opGreaterOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  StringViewLite strView1 = Convert(str1);
  StringViewLite strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// compare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, compare)
{
  EXPECT_TRUE(StringViewLite("A").compare(StringViewLite("B")) < 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("B")) == 0);
  EXPECT_TRUE(StringViewLite("C").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("A")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("C")) < 0);

  EXPECT_TRUE(StringViewLite("A").compare(StringViewLite("BA")) < 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("BA")) < 0);
  EXPECT_TRUE(StringViewLite("C").compare(StringViewLite("BA")) > 0);
  EXPECT_TRUE(StringViewLite("BA").compare(StringViewLite("A")) > 0);
  EXPECT_TRUE(StringViewLite("BA").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(StringViewLite("BA").compare(StringViewLite("C")) < 0);

  EXPECT_TRUE(StringViewLite("AA").compare(StringViewLite("B")) < 0);
  EXPECT_TRUE(StringViewLite("BA").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(StringViewLite("CA").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("AA")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("BA")) < 0);
  EXPECT_TRUE(StringViewLite("B").compare(StringViewLite("CA")) < 0);
}

TEST(TestString_StringViewLite, compare_string_view)
{
  EXPECT_TRUE(StringViewLite("A").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("B")) == 0);
  EXPECT_TRUE(StringViewLite("C").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(StringViewLite("A").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(StringViewLite("C").compare(std::string_view("BA")) > 0);
  EXPECT_TRUE(StringViewLite("BA").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(StringViewLite("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(StringViewLite("BA").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(StringViewLite("AA").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(StringViewLite("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(StringViewLite("CA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("AA")) > 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(StringViewLite("B").compare(std::string_view("CA")) < 0);
}

TEST(TestString_StringViewLite, compare_CString)
{
  EXPECT_TRUE(StringViewLite("A").compare("B") < 0);
  EXPECT_TRUE(StringViewLite("B").compare("B") == 0);
  EXPECT_TRUE(StringViewLite("C").compare("B") > 0);
  EXPECT_TRUE(StringViewLite("B").compare("A") > 0);
  EXPECT_TRUE(StringViewLite("B").compare("C") < 0);

  EXPECT_TRUE(StringViewLite("A").compare("BA") < 0);
  EXPECT_TRUE(StringViewLite("B").compare("BA") < 0);
  EXPECT_TRUE(StringViewLite("C").compare("BA") > 0);
  EXPECT_TRUE(StringViewLite("BA").compare("A") > 0);
  EXPECT_TRUE(StringViewLite("BA").compare("B") > 0);
  EXPECT_TRUE(StringViewLite("BA").compare("C") < 0);

  EXPECT_TRUE(StringViewLite("AA").compare("B") < 0);
  EXPECT_TRUE(StringViewLite("BA").compare("B") > 0);
  EXPECT_TRUE(StringViewLite("CA").compare("B") > 0);
  EXPECT_TRUE(StringViewLite("B").compare("AA") > 0);
  EXPECT_TRUE(StringViewLite("B").compare("BA") < 0);
  EXPECT_TRUE(StringViewLite("B").compare("CA") < 0);
}


TEST(TestString_StringViewLite, compare_Null)
{
  EXPECT_TRUE(StringViewLite().compare(StringViewLite()) == 0);
  EXPECT_TRUE(StringViewLite("").compare(StringViewLite()) == 0);
  EXPECT_TRUE(StringViewLite().compare(StringViewLite("")) == 0);
}

TEST(TestString_StringViewLite, compare_Null_CString)
{
  EXPECT_TRUE(StringViewLite().compare(nullptr) == 0);
  EXPECT_TRUE(StringViewLite("").compare(nullptr) == 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// starts_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, starts_with)
{
  EXPECT_TRUE(StringViewLite().starts_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite().starts_with(StringViewLite("")));

  EXPECT_TRUE(StringViewLite("").starts_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("").starts_with(StringViewLite("")));

  EXPECT_TRUE(StringViewLite("A").starts_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("A").starts_with(StringViewLite("")));
  EXPECT_TRUE(StringViewLite("A").starts_with(StringViewLite("A")));

  EXPECT_TRUE(StringViewLite("AB").starts_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("AB").starts_with(StringViewLite("")));
  EXPECT_TRUE(StringViewLite("AB").starts_with(StringViewLite("A")));
  EXPECT_TRUE(StringViewLite("AB").starts_with(StringViewLite("AB")));

  EXPECT_TRUE(StringViewLite("ABC").starts_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("ABC").starts_with(StringViewLite("")));
  EXPECT_TRUE(StringViewLite("ABC").starts_with(StringViewLite("A")));
  EXPECT_TRUE(StringViewLite("ABC").starts_with(StringViewLite("AB")));
  EXPECT_TRUE(StringViewLite("ABC").starts_with(StringViewLite("ABC")));

  EXPECT_FALSE(StringViewLite().starts_with(StringViewLite("A")));

  EXPECT_FALSE(StringViewLite("").starts_with(StringViewLite("A")));

  EXPECT_FALSE(StringViewLite("A").starts_with(StringViewLite("B")));
  EXPECT_FALSE(StringViewLite("A").starts_with(StringViewLite("AB")));

  EXPECT_FALSE(StringViewLite("AB").starts_with(StringViewLite("B")));
  EXPECT_FALSE(StringViewLite("AB").starts_with(StringViewLite("AC")));
  EXPECT_FALSE(StringViewLite("AB").starts_with(StringViewLite("ABC")));

  EXPECT_FALSE(StringViewLite("ABC").starts_with(StringViewLite("B")));
  EXPECT_FALSE(StringViewLite("ABC").starts_with(StringViewLite("AC")));
  EXPECT_FALSE(StringViewLite("ABC").starts_with(StringViewLite("ABD")));
  EXPECT_FALSE(StringViewLite("ABC").starts_with(StringViewLite("ABDA")));
}


TEST(TestString_StringViewLite, starts_with_CString)
{
  EXPECT_TRUE(StringViewLite().starts_with(nullptr));
  EXPECT_TRUE(StringViewLite().starts_with(""));

  EXPECT_TRUE(StringViewLite("").starts_with(nullptr));
  EXPECT_TRUE(StringViewLite("").starts_with(""));

  EXPECT_TRUE(StringViewLite("A").starts_with(nullptr));
  EXPECT_TRUE(StringViewLite("A").starts_with(""));
  EXPECT_TRUE(StringViewLite("A").starts_with("A"));

  EXPECT_TRUE(StringViewLite("AB").starts_with(nullptr));
  EXPECT_TRUE(StringViewLite("AB").starts_with(""));
  EXPECT_TRUE(StringViewLite("AB").starts_with("A"));
  EXPECT_TRUE(StringViewLite("AB").starts_with("AB"));

  EXPECT_TRUE(StringViewLite("ABC").starts_with(nullptr));
  EXPECT_TRUE(StringViewLite("ABC").starts_with(""));
  EXPECT_TRUE(StringViewLite("ABC").starts_with("A"));
  EXPECT_TRUE(StringViewLite("ABC").starts_with("AB"));
  EXPECT_TRUE(StringViewLite("ABC").starts_with("ABC"));

  EXPECT_FALSE(StringViewLite().starts_with("A"));

  EXPECT_FALSE(StringViewLite("").starts_with("A"));

  EXPECT_FALSE(StringViewLite("A").starts_with("B"));
  EXPECT_FALSE(StringViewLite("A").starts_with("AB"));

  EXPECT_FALSE(StringViewLite("AB").starts_with("B"));
  EXPECT_FALSE(StringViewLite("AB").starts_with("AC"));
  EXPECT_FALSE(StringViewLite("AB").starts_with("ABC"));

  EXPECT_FALSE(StringViewLite("ABC").starts_with("B"));
  EXPECT_FALSE(StringViewLite("ABC").starts_with("AC"));
  EXPECT_FALSE(StringViewLite("ABC").starts_with("ABD"));
  EXPECT_FALSE(StringViewLite("ABC").starts_with("ABDA"));
}

TEST(TestString_StringViewLite, starts_with_char)
{
  EXPECT_TRUE(StringViewLite("A").starts_with('A'));
  EXPECT_TRUE(StringViewLite("B").starts_with('B'));

  EXPECT_TRUE(StringViewLite("AB").starts_with('A'));
  EXPECT_TRUE(StringViewLite("BC").starts_with('B'));

  EXPECT_FALSE(StringViewLite().starts_with(' '));
  EXPECT_FALSE(StringViewLite().starts_with('a'));

  EXPECT_FALSE(StringViewLite("").starts_with(' '));
  EXPECT_FALSE(StringViewLite("").starts_with('a'));

  EXPECT_FALSE(StringViewLite("A").starts_with(' '));
  EXPECT_FALSE(StringViewLite("A").starts_with('a'));

  EXPECT_FALSE(StringViewLite("AB").starts_with(' '));
  EXPECT_FALSE(StringViewLite("AB").starts_with('a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ends_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, ends_with)
{
  EXPECT_TRUE(StringViewLite().ends_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite().ends_with(StringViewLite("")));

  EXPECT_TRUE(StringViewLite("").ends_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("").ends_with(StringViewLite("")));

  EXPECT_TRUE(StringViewLite("A").ends_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("A").ends_with(StringViewLite("")));
  EXPECT_TRUE(StringViewLite("A").ends_with(StringViewLite("A")));

  EXPECT_TRUE(StringViewLite("BA").ends_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("BA").ends_with(StringViewLite("")));
  EXPECT_TRUE(StringViewLite("BA").ends_with(StringViewLite("A")));
  EXPECT_TRUE(StringViewLite("BA").ends_with(StringViewLite("BA")));

  EXPECT_TRUE(StringViewLite("CBA").ends_with(StringViewLite()));
  EXPECT_TRUE(StringViewLite("CBA").ends_with(StringViewLite("")));
  EXPECT_TRUE(StringViewLite("CBA").ends_with(StringViewLite("A")));
  EXPECT_TRUE(StringViewLite("CBA").ends_with(StringViewLite("BA")));
  EXPECT_TRUE(StringViewLite("CBA").ends_with(StringViewLite("CBA")));

  EXPECT_FALSE(StringViewLite().ends_with(StringViewLite("A")));

  EXPECT_FALSE(StringViewLite("").ends_with(StringViewLite("A")));

  EXPECT_FALSE(StringViewLite("A").ends_with(StringViewLite("B")));
  EXPECT_FALSE(StringViewLite("A").ends_with(StringViewLite("BA")));

  EXPECT_FALSE(StringViewLite("BA").ends_with(StringViewLite("B")));
  EXPECT_FALSE(StringViewLite("BA").ends_with(StringViewLite("CA")));
  EXPECT_FALSE(StringViewLite("BA").ends_with(StringViewLite("CBA")));

  EXPECT_FALSE(StringViewLite("CBA").ends_with(StringViewLite("B")));
  EXPECT_FALSE(StringViewLite("CBA").ends_with(StringViewLite("CA")));
  EXPECT_FALSE(StringViewLite("CBA").ends_with(StringViewLite("DBA")));
  EXPECT_FALSE(StringViewLite("CBA").ends_with(StringViewLite("ADBA")));
}


TEST(TestString_StringViewLite, ends_with_CString)
{
  EXPECT_TRUE(StringViewLite().ends_with(nullptr));
  EXPECT_TRUE(StringViewLite().ends_with(""));

  EXPECT_TRUE(StringViewLite("").ends_with(nullptr));
  EXPECT_TRUE(StringViewLite("").ends_with(""));

  EXPECT_TRUE(StringViewLite("A").ends_with(nullptr));
  EXPECT_TRUE(StringViewLite("A").ends_with(""));
  EXPECT_TRUE(StringViewLite("A").ends_with("A"));

  EXPECT_TRUE(StringViewLite("BA").ends_with(nullptr));
  EXPECT_TRUE(StringViewLite("BA").ends_with(""));
  EXPECT_TRUE(StringViewLite("BA").ends_with("A"));
  EXPECT_TRUE(StringViewLite("BA").ends_with("BA"));

  EXPECT_TRUE(StringViewLite("CBA").ends_with(nullptr));
  EXPECT_TRUE(StringViewLite("CBA").ends_with(""));
  EXPECT_TRUE(StringViewLite("CBA").ends_with("A"));
  EXPECT_TRUE(StringViewLite("CBA").ends_with("BA"));
  EXPECT_TRUE(StringViewLite("CBA").ends_with("CBA"));

  EXPECT_FALSE(StringViewLite().ends_with("A"));

  EXPECT_FALSE(StringViewLite("").ends_with("A"));

  EXPECT_FALSE(StringViewLite("A").ends_with("B"));
  EXPECT_FALSE(StringViewLite("A").ends_with("BA"));

  EXPECT_FALSE(StringViewLite("BA").ends_with("B"));
  EXPECT_FALSE(StringViewLite("BA").ends_with("CA"));
  EXPECT_FALSE(StringViewLite("BA").ends_with("CBA"));

  EXPECT_FALSE(StringViewLite("CBA").ends_with("B"));
  EXPECT_FALSE(StringViewLite("CBA").ends_with("CA"));
  EXPECT_FALSE(StringViewLite("CBA").ends_with("DBA"));
  EXPECT_FALSE(StringViewLite("CBA").ends_with("ADBA"));
}


TEST(TestString_StringViewLite, ends_with_char)
{
  EXPECT_TRUE(StringViewLite("A").ends_with('A'));
  EXPECT_TRUE(StringViewLite("B").ends_with('B'));

  EXPECT_TRUE(StringViewLite("BA").ends_with('A'));
  EXPECT_TRUE(StringViewLite("CB").ends_with('B'));

  EXPECT_FALSE(StringViewLite().ends_with(' '));
  EXPECT_FALSE(StringViewLite().ends_with('a'));

  EXPECT_FALSE(StringViewLite("").ends_with(' '));
  EXPECT_FALSE(StringViewLite("").ends_with('a'));

  EXPECT_FALSE(StringViewLite("A").ends_with(' '));
  EXPECT_FALSE(StringViewLite("A").ends_with('a'));

  EXPECT_FALSE(StringViewLite("BA").ends_with(' '));
  EXPECT_FALSE(StringViewLite("BA").ends_with('a'));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// find
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, find_char)
{
  EXPECT_EQ(0u, StringViewLite("a").find('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").find('a', 1u));

  EXPECT_EQ(0u, StringViewLite("aa").find('a'));
  EXPECT_EQ(1u, StringViewLite("aa").find('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("aa").find('a', 2u));

  EXPECT_EQ(0u, StringViewLite("aba").find('a'));
  EXPECT_EQ(2u, StringViewLite("aba").find('a', 1u));
  EXPECT_EQ(2u, StringViewLite("aba").find('a', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("aba").find('a', 3u));

  EXPECT_EQ(2u, StringViewLite("cba").find('a'));
  EXPECT_EQ(2u, StringViewLite("cba").find('a', 1u));
  EXPECT_EQ(2u, StringViewLite("cba").find('a', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("cba").find('a', 3u));


  EXPECT_EQ(StringViewLite::npos, StringViewLite().find(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().find('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().find(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().find('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().find(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().find('a', 2u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite("").find(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").find('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").find(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").find('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").find(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").find('a', 2u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").find(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("b").find('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").find(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("b").find('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").find(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("b").find('a', 2u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite("ab").find(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("bc").find('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("ab").find(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("bc").find('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("ab").find(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("bc").find('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// rfind
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, rfind_char)
{
  EXPECT_EQ(0u, StringViewLite("a").rfind('a'));
  EXPECT_EQ(0u, StringViewLite("a").rfind('a', 0u));
  EXPECT_EQ(0u, StringViewLite("a").rfind('a', 1u));

  EXPECT_EQ(1u, StringViewLite("aa").rfind('a'));
  EXPECT_EQ(0u, StringViewLite("aa").rfind('a', 0u));
  EXPECT_EQ(1u, StringViewLite("aa").rfind('a', 1u));
  EXPECT_EQ(1u, StringViewLite("aa").rfind('a', 2u));

  EXPECT_EQ(2u, StringViewLite("aba").rfind('a'));
  EXPECT_EQ(0u, StringViewLite("aba").rfind('a', 0u));
  EXPECT_EQ(0u, StringViewLite("aba").rfind('a', 1u));
  EXPECT_EQ(2u, StringViewLite("aba").rfind('a', 2u));
  EXPECT_EQ(2u, StringViewLite("aba").rfind('a', 3u));

  EXPECT_EQ(2u, StringViewLite("cba").rfind('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("cba").rfind('a', 0u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("cba").rfind('a', 1u));
  EXPECT_EQ(2u, StringViewLite("cba").rfind('a', 2u));
  EXPECT_EQ(2u, StringViewLite("cba").rfind('a', 3u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite().rfind(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().rfind('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().rfind(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().rfind('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().rfind(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite().rfind('a', 2u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite("").rfind(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").rfind('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").rfind(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").rfind('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").rfind(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("").rfind('a', 2u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").rfind(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("b").rfind('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").rfind(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("b").rfind('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("a").rfind(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("b").rfind('a', 2u));

  EXPECT_EQ(StringViewLite::npos, StringViewLite("ab").rfind(' '));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("bc").rfind('a'));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("ab").rfind(' ', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("bc").rfind('a', 1u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("ab").rfind(' ', 2u));
  EXPECT_EQ(StringViewLite::npos, StringViewLite("bc").rfind('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, OperatorLessThan)
{
  EXPECT_LT(StringViewLite("A"), StringViewLite("B"));
  EXPECT_LT(StringViewLite("A"), StringViewLite("AB"));
  EXPECT_LT(StringViewLite(""), StringViewLite("AB"));

  EXPECT_TRUE(StringViewLite("A") < StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") < StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("C") < StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") < StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("B") < StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") < StringViewLite("C"));

  EXPECT_TRUE(StringViewLite("A") < StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("B") < StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("C") < StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("BA") < StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("BA") < StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("BA") < StringViewLite("C"));

  EXPECT_TRUE(StringViewLite("AA") < StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("BA") < StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("CA") < StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") < StringViewLite("AA"));
  EXPECT_TRUE(StringViewLite("B") < StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("B") < StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThan_string_view_LHS)
{
  EXPECT_LT(std::string_view("A"), StringViewLite("B"));
  EXPECT_LT(std::string_view("A"), StringViewLite("AB"));
  EXPECT_LT(std::string_view(""), StringViewLite("AB"));

  EXPECT_TRUE(std::string_view("A") < StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") < StringViewLite("B"));
  EXPECT_FALSE(std::string_view("C") < StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") < StringViewLite("A"));
  EXPECT_FALSE(std::string_view("B") < StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") < StringViewLite("C"));

  EXPECT_TRUE(std::string_view("A") < StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("B") < StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("C") < StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("BA") < StringViewLite("A"));
  EXPECT_FALSE(std::string_view("BA") < StringViewLite("B"));
  EXPECT_TRUE(std::string_view("BA") < StringViewLite("C"));

  EXPECT_TRUE(std::string_view("AA") < StringViewLite("B"));
  EXPECT_FALSE(std::string_view("BA") < StringViewLite("B"));
  EXPECT_FALSE(std::string_view("CA") < StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") < StringViewLite("AA"));
  EXPECT_TRUE(std::string_view("B") < StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("B") < StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThan_string_view_RHS)
{
  EXPECT_LT(StringViewLite("A"), std::string_view("B"));
  EXPECT_LT(StringViewLite("A"), std::string_view("AB"));
  EXPECT_LT(StringViewLite(""), std::string_view("AB"));

  EXPECT_TRUE(StringViewLite("A") < std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") < std::string_view("B"));
  EXPECT_FALSE(StringViewLite("C") < std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") < std::string_view("A"));
  EXPECT_FALSE(StringViewLite("B") < std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") < std::string_view("C"));

  EXPECT_TRUE(StringViewLite("A") < std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("B") < std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("C") < std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("BA") < std::string_view("A"));
  EXPECT_FALSE(StringViewLite("BA") < std::string_view("B"));
  EXPECT_TRUE(StringViewLite("BA") < std::string_view("C"));

  EXPECT_TRUE(StringViewLite("AA") < std::string_view("B"));
  EXPECT_FALSE(StringViewLite("BA") < std::string_view("B"));
  EXPECT_FALSE(StringViewLite("CA") < std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") < std::string_view("AA"));
  EXPECT_TRUE(StringViewLite("B") < std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("B") < std::string_view("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThan_CString_LHS)
{
  EXPECT_LT("A", StringViewLite("B"));
  EXPECT_LT("A", StringViewLite("AB"));
  EXPECT_LT("", StringViewLite("AB"));

  EXPECT_TRUE("A" < StringViewLite("B"));
  EXPECT_FALSE("B" < StringViewLite("B"));
  EXPECT_FALSE("C" < StringViewLite("B"));
  EXPECT_FALSE("B" < StringViewLite("A"));
  EXPECT_FALSE("B" < StringViewLite("B"));
  EXPECT_TRUE("B" < StringViewLite("C"));

  EXPECT_TRUE("A" < StringViewLite("BA"));
  EXPECT_TRUE("B" < StringViewLite("BA"));
  EXPECT_FALSE("C" < StringViewLite("BA"));
  EXPECT_FALSE("BA" < StringViewLite("A"));
  EXPECT_FALSE("BA" < StringViewLite("B"));
  EXPECT_TRUE("BA" < StringViewLite("C"));

  EXPECT_TRUE("AA" < StringViewLite("B"));
  EXPECT_FALSE("BA" < StringViewLite("B"));
  EXPECT_FALSE("CA" < StringViewLite("B"));
  EXPECT_FALSE("B" < StringViewLite("AA"));
  EXPECT_TRUE("B" < StringViewLite("BA"));
  EXPECT_TRUE("B" < StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThan_CString_RHS)
{
  EXPECT_LT(StringViewLite("A"), "B");
  EXPECT_LT(StringViewLite("A"), "AB");
  EXPECT_LT(StringViewLite(""), "AB");

  EXPECT_TRUE(StringViewLite("A") < "B");
  EXPECT_FALSE(StringViewLite("B") < "B");
  EXPECT_FALSE(StringViewLite("C") < "B");
  EXPECT_FALSE(StringViewLite("B") < "A");
  EXPECT_FALSE(StringViewLite("B") < "B");
  EXPECT_TRUE(StringViewLite("B") < "C");

  EXPECT_TRUE(StringViewLite("A") < "BA");
  EXPECT_TRUE(StringViewLite("B") < "BA");
  EXPECT_FALSE(StringViewLite("C") < "BA");
  EXPECT_FALSE(StringViewLite("BA") < "A");
  EXPECT_FALSE(StringViewLite("BA") < "B");
  EXPECT_TRUE(StringViewLite("BA") < "C");

  EXPECT_TRUE(StringViewLite("AA") < "B");
  EXPECT_FALSE(StringViewLite("BA") < "B");
  EXPECT_FALSE(StringViewLite("CA") < "B");
  EXPECT_FALSE(StringViewLite("B") < "AA");
  EXPECT_TRUE(StringViewLite("B") < "BA");
  EXPECT_TRUE(StringViewLite("B") < "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, OperatorLessThanOrEqual)
{
  EXPECT_LE(StringViewLite("A"), StringViewLite("B"));
  EXPECT_LE(StringViewLite("A"), StringViewLite("AB"));
  EXPECT_LE(StringViewLite(""), StringViewLite("AB"));
  EXPECT_LE(StringViewLite("A"), StringViewLite("A"));

  EXPECT_TRUE(StringViewLite("A") <= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") <= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("C") <= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") <= StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("B") <= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") <= StringViewLite("C"));

  EXPECT_TRUE(StringViewLite("A") <= StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("B") <= StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("C") <= StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("BA") <= StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("BA") <= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("BA") <= StringViewLite("C"));

  EXPECT_TRUE(StringViewLite("AA") <= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("BA") <= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("CA") <= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") <= StringViewLite("AA"));
  EXPECT_TRUE(StringViewLite("B") <= StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("B") <= StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThanOrEqual_string_view_lhs)
{
  EXPECT_LE(std::string_view("A"), StringViewLite("B"));
  EXPECT_LE(std::string_view("A"), StringViewLite("AB"));
  EXPECT_LE(std::string_view(""), StringViewLite("AB"));
  EXPECT_LE(std::string_view("A"), StringViewLite("A"));

  EXPECT_TRUE(std::string_view("A") <= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") <= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("C") <= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") <= StringViewLite("A"));
  EXPECT_TRUE(std::string_view("B") <= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") <= StringViewLite("C"));

  EXPECT_TRUE(std::string_view("A") <= StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("B") <= StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("C") <= StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("BA") <= StringViewLite("A"));
  EXPECT_FALSE(std::string_view("BA") <= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("BA") <= StringViewLite("C"));

  EXPECT_TRUE(std::string_view("AA") <= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("BA") <= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("CA") <= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") <= StringViewLite("AA"));
  EXPECT_TRUE(std::string_view("B") <= StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("B") <= StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThanOrEqual_string_view_rhs)
{
  EXPECT_LE(StringViewLite("A"), std::string_view("B"));
  EXPECT_LE(StringViewLite("A"), std::string_view("AB"));
  EXPECT_LE(StringViewLite(""), std::string_view("AB"));
  EXPECT_LE(StringViewLite("A"), std::string_view("A"));

  EXPECT_TRUE(StringViewLite("A") <= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") <= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("C") <= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") <= std::string_view("A"));
  EXPECT_TRUE(StringViewLite("B") <= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") <= std::string_view("C"));

  EXPECT_TRUE(StringViewLite("A") <= std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("B") <= std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("C") <= std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("BA") <= std::string_view("A"));
  EXPECT_FALSE(StringViewLite("BA") <= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("BA") <= std::string_view("C"));

  EXPECT_TRUE(StringViewLite("AA") <= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("BA") <= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("CA") <= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") <= std::string_view("AA"));
  EXPECT_TRUE(StringViewLite("B") <= std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("B") <= std::string_view("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThanOrEqual_CString_LHS)
{
  EXPECT_LE("A", StringViewLite("B"));
  EXPECT_LE("A", StringViewLite("AB"));
  EXPECT_LE("", StringViewLite("AB"));
  EXPECT_LE("A", StringViewLite("A"));

  EXPECT_TRUE("A" <= StringViewLite("B"));
  EXPECT_TRUE("B" <= StringViewLite("B"));
  EXPECT_FALSE("C" <= StringViewLite("B"));
  EXPECT_FALSE("B" <= StringViewLite("A"));
  EXPECT_TRUE("B" <= StringViewLite("B"));
  EXPECT_TRUE("B" <= StringViewLite("C"));

  EXPECT_TRUE("A" <= StringViewLite("BA"));
  EXPECT_TRUE("B" <= StringViewLite("BA"));
  EXPECT_FALSE("C" <= StringViewLite("BA"));
  EXPECT_FALSE("BA" <= StringViewLite("A"));
  EXPECT_FALSE("BA" <= StringViewLite("B"));
  EXPECT_TRUE("BA" <= StringViewLite("C"));

  EXPECT_TRUE("AA" <= StringViewLite("B"));
  EXPECT_FALSE("BA" <= StringViewLite("B"));
  EXPECT_FALSE("CA" <= StringViewLite("B"));
  EXPECT_FALSE("B" <= StringViewLite("AA"));
  EXPECT_TRUE("B" <= StringViewLite("BA"));
  EXPECT_TRUE("B" <= StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorLessThanOrEqual_CString_RHS)
{
  EXPECT_LE(StringViewLite("A"), "B");
  EXPECT_LE(StringViewLite("A"), "AB");
  EXPECT_LE(StringViewLite(""), "AB");
  EXPECT_LE(StringViewLite("A"), "A");

  EXPECT_TRUE(StringViewLite("A") <= "B");
  EXPECT_TRUE(StringViewLite("B") <= "B");
  EXPECT_FALSE(StringViewLite("C") <= "B");
  EXPECT_FALSE(StringViewLite("B") <= "A");
  EXPECT_TRUE(StringViewLite("B") <= "B");
  EXPECT_TRUE(StringViewLite("B") <= "C");

  EXPECT_TRUE(StringViewLite("A") <= "BA");
  EXPECT_TRUE(StringViewLite("B") <= "BA");
  EXPECT_FALSE(StringViewLite("C") <= "BA");
  EXPECT_FALSE(StringViewLite("BA") <= "A");
  EXPECT_FALSE(StringViewLite("BA") <= "B");
  EXPECT_TRUE(StringViewLite("BA") <= "C");

  EXPECT_TRUE(StringViewLite("AA") <= "B");
  EXPECT_FALSE(StringViewLite("BA") <= "B");
  EXPECT_FALSE(StringViewLite("CA") <= "B");
  EXPECT_FALSE(StringViewLite("B") <= "AA");
  EXPECT_TRUE(StringViewLite("B") <= "BA");
  EXPECT_TRUE(StringViewLite("B") <= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, OperatorGreaterThan)
{
  EXPECT_GT(StringViewLite("B"), StringViewLite("A"));
  EXPECT_GT(StringViewLite("AB"), StringViewLite("A"));
  EXPECT_GT(StringViewLite("AB"), StringViewLite(""));

  EXPECT_FALSE(StringViewLite("A") > StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") > StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("C") > StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") > StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("B") > StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") > StringViewLite("C"));

  EXPECT_FALSE(StringViewLite("A") > StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("B") > StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("C") > StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("BA") > StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("BA") > StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("BA") > StringViewLite("C"));

  EXPECT_FALSE(StringViewLite("AA") > StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("BA") > StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("CA") > StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") > StringViewLite("AA"));
  EXPECT_FALSE(StringViewLite("B") > StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("B") > StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThan_string_view_LHS)
{
  EXPECT_GT(std::string_view("B"), StringViewLite("A"));
  EXPECT_GT(std::string_view("AB"), StringViewLite("A"));
  EXPECT_GT(std::string_view("AB"), StringViewLite(""));

  EXPECT_FALSE(std::string_view("A") > StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") > StringViewLite("B"));
  EXPECT_TRUE(std::string_view("C") > StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") > StringViewLite("A"));
  EXPECT_FALSE(std::string_view("B") > StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") > StringViewLite("C"));

  EXPECT_FALSE(std::string_view("A") > StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("B") > StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("C") > StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("BA") > StringViewLite("A"));
  EXPECT_TRUE(std::string_view("BA") > StringViewLite("B"));
  EXPECT_FALSE(std::string_view("BA") > StringViewLite("C"));

  EXPECT_FALSE(std::string_view("AA") > StringViewLite("B"));
  EXPECT_TRUE(std::string_view("BA") > StringViewLite("B"));
  EXPECT_TRUE(std::string_view("CA") > StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") > StringViewLite("AA"));
  EXPECT_FALSE(std::string_view("B") > StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("B") > StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThan_string_view_RHS)
{
  EXPECT_GT(StringViewLite("B"), std::string_view("A"));
  EXPECT_GT(StringViewLite("AB"), std::string_view("A"));
  EXPECT_GT(StringViewLite("AB"), std::string_view(""));

  EXPECT_FALSE(StringViewLite("A") > std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") > std::string_view("B"));
  EXPECT_TRUE(StringViewLite("C") > std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") > std::string_view("A"));
  EXPECT_FALSE(StringViewLite("B") > std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") > std::string_view("C"));

  EXPECT_FALSE(StringViewLite("A") > std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("B") > std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("C") > std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("BA") > std::string_view("A"));
  EXPECT_TRUE(StringViewLite("BA") > std::string_view("B"));
  EXPECT_FALSE(StringViewLite("BA") > std::string_view("C"));

  EXPECT_FALSE(StringViewLite("AA") > std::string_view("B"));
  EXPECT_TRUE(StringViewLite("BA") > std::string_view("B"));
  EXPECT_TRUE(StringViewLite("CA") > std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") > std::string_view("AA"));
  EXPECT_FALSE(StringViewLite("B") > std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("B") > std::string_view("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThan_CString_LHS)
{
  EXPECT_GT("B", StringViewLite("A"));
  EXPECT_GT("AB", StringViewLite("A"));
  EXPECT_GT("AB", StringViewLite(""));

  EXPECT_FALSE("A" > StringViewLite("B"));
  EXPECT_FALSE("B" > StringViewLite("B"));
  EXPECT_TRUE("C" > StringViewLite("B"));
  EXPECT_TRUE("B" > StringViewLite("A"));
  EXPECT_FALSE("B" > StringViewLite("B"));
  EXPECT_FALSE("B" > StringViewLite("C"));

  EXPECT_FALSE("A" > StringViewLite("BA"));
  EXPECT_FALSE("B" > StringViewLite("BA"));
  EXPECT_TRUE("C" > StringViewLite("BA"));
  EXPECT_TRUE("BA" > StringViewLite("A"));
  EXPECT_TRUE("BA" > StringViewLite("B"));
  EXPECT_FALSE("BA" > StringViewLite("C"));

  EXPECT_FALSE("AA" > StringViewLite("B"));
  EXPECT_TRUE("BA" > StringViewLite("B"));
  EXPECT_TRUE("CA" > StringViewLite("B"));
  EXPECT_TRUE("B" > StringViewLite("AA"));
  EXPECT_FALSE("B" > StringViewLite("BA"));
  EXPECT_FALSE("B" > StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThan_CString_RHS)
{
  EXPECT_GT(StringViewLite("B"), "A");
  EXPECT_GT(StringViewLite("AB"), "A");
  EXPECT_GT(StringViewLite("AB"), "");

  EXPECT_FALSE(StringViewLite("A") > "B");
  EXPECT_FALSE(StringViewLite("B") > "B");
  EXPECT_TRUE(StringViewLite("C") > "B");
  EXPECT_TRUE(StringViewLite("B") > "A");
  EXPECT_FALSE(StringViewLite("B") > "B");
  EXPECT_FALSE(StringViewLite("B") > "C");

  EXPECT_FALSE(StringViewLite("A") > "BA");
  EXPECT_FALSE(StringViewLite("B") > "BA");
  EXPECT_TRUE(StringViewLite("C") > "BA");
  EXPECT_TRUE(StringViewLite("BA") > "A");
  EXPECT_TRUE(StringViewLite("BA") > "B");
  EXPECT_FALSE(StringViewLite("BA") > "C");

  EXPECT_FALSE(StringViewLite("AA") > "B");
  EXPECT_TRUE(StringViewLite("BA") > "B");
  EXPECT_TRUE(StringViewLite("CA") > "B");
  EXPECT_TRUE(StringViewLite("B") > "AA");
  EXPECT_FALSE(StringViewLite("B") > "BA");
  EXPECT_FALSE(StringViewLite("B") > "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(StringViewLite("B"), StringViewLite("A"));
  EXPECT_GE(StringViewLite("AB"), StringViewLite("A"));
  EXPECT_GE(StringViewLite("AB"), StringViewLite(""));
  EXPECT_GE(StringViewLite("A"), StringViewLite("A"));

  EXPECT_FALSE(StringViewLite("A") >= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") >= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("C") >= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") >= StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("B") >= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") >= StringViewLite("C"));

  EXPECT_FALSE(StringViewLite("A") >= StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("B") >= StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("C") >= StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("BA") >= StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("BA") >= StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("BA") >= StringViewLite("C"));

  EXPECT_FALSE(StringViewLite("AA") >= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("BA") >= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("CA") >= StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") >= StringViewLite("AA"));
  EXPECT_FALSE(StringViewLite("B") >= StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("B") >= StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThanOrEqual_string_view_LHS)
{
  EXPECT_GE(std::string_view("B"), StringViewLite("A"));
  EXPECT_GE(std::string_view("AB"), StringViewLite("A"));
  EXPECT_GE(std::string_view("AB"), StringViewLite(""));
  EXPECT_GE(std::string_view("A"), StringViewLite("A"));

  EXPECT_FALSE(std::string_view("A") >= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") >= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("C") >= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") >= StringViewLite("A"));
  EXPECT_TRUE(std::string_view("B") >= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") >= StringViewLite("C"));

  EXPECT_FALSE(std::string_view("A") >= StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("B") >= StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("C") >= StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("BA") >= StringViewLite("A"));
  EXPECT_TRUE(std::string_view("BA") >= StringViewLite("B"));
  EXPECT_FALSE(std::string_view("BA") >= StringViewLite("C"));

  EXPECT_FALSE(std::string_view("AA") >= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("BA") >= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("CA") >= StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") >= StringViewLite("AA"));
  EXPECT_FALSE(std::string_view("B") >= StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("B") >= StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThanOrEqual_string_view_RHS)
{
  EXPECT_GE(StringViewLite("B"), std::string_view("A"));
  EXPECT_GE(StringViewLite("AB"), std::string_view("A"));
  EXPECT_GE(StringViewLite("AB"), std::string_view(""));
  EXPECT_GE(StringViewLite("A"), std::string_view("A"));

  EXPECT_FALSE(StringViewLite("A") >= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") >= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("C") >= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") >= std::string_view("A"));
  EXPECT_TRUE(StringViewLite("B") >= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") >= std::string_view("C"));

  EXPECT_FALSE(StringViewLite("A") >= std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("B") >= std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("C") >= std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("BA") >= std::string_view("A"));
  EXPECT_TRUE(StringViewLite("BA") >= std::string_view("B"));
  EXPECT_FALSE(StringViewLite("BA") >= std::string_view("C"));

  EXPECT_FALSE(StringViewLite("AA") >= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("BA") >= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("CA") >= std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") >= std::string_view("AA"));
  EXPECT_FALSE(StringViewLite("B") >= std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("B") >= std::string_view("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThanOrEqual_CString_LHS)
{
  EXPECT_GE("B", StringViewLite("A"));
  EXPECT_GE("AB", StringViewLite("A"));
  EXPECT_GE("AB", StringViewLite(""));
  EXPECT_GE("A", StringViewLite("A"));

  EXPECT_FALSE("A" >= StringViewLite("B"));
  EXPECT_TRUE("B" >= StringViewLite("B"));
  EXPECT_TRUE("C" >= StringViewLite("B"));
  EXPECT_TRUE("B" >= StringViewLite("A"));
  EXPECT_TRUE("B" >= StringViewLite("B"));
  EXPECT_FALSE("B" >= StringViewLite("C"));

  EXPECT_FALSE("A" >= StringViewLite("BA"));
  EXPECT_FALSE("B" >= StringViewLite("BA"));
  EXPECT_TRUE("C" >= StringViewLite("BA"));
  EXPECT_TRUE("BA" >= StringViewLite("A"));
  EXPECT_TRUE("BA" >= StringViewLite("B"));
  EXPECT_FALSE("BA" >= StringViewLite("C"));

  EXPECT_FALSE("AA" >= StringViewLite("B"));
  EXPECT_TRUE("BA" >= StringViewLite("B"));
  EXPECT_TRUE("CA" >= StringViewLite("B"));
  EXPECT_TRUE("B" >= StringViewLite("AA"));
  EXPECT_FALSE("B" >= StringViewLite("BA"));
  EXPECT_FALSE("B" >= StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorGreaterThanOrEqual_CString_RHS)
{
  EXPECT_GE(StringViewLite("B"), "A");
  EXPECT_GE(StringViewLite("AB"), "A");
  EXPECT_GE(StringViewLite("AB"), "");
  EXPECT_GE(StringViewLite("A"), "A");

  EXPECT_FALSE(StringViewLite("A") >= "B");
  EXPECT_TRUE(StringViewLite("B") >= "B");
  EXPECT_TRUE(StringViewLite("C") >= "B");
  EXPECT_TRUE(StringViewLite("B") >= "A");
  EXPECT_TRUE(StringViewLite("B") >= "B");
  EXPECT_FALSE(StringViewLite("B") >= "C");

  EXPECT_FALSE(StringViewLite("A") >= "BA");
  EXPECT_FALSE(StringViewLite("B") >= "BA");
  EXPECT_TRUE(StringViewLite("C") >= "BA");
  EXPECT_TRUE(StringViewLite("BA") >= "A");
  EXPECT_TRUE(StringViewLite("BA") >= "B");
  EXPECT_FALSE(StringViewLite("BA") >= "C");

  EXPECT_FALSE(StringViewLite("AA") >= "B");
  EXPECT_TRUE(StringViewLite("BA") >= "B");
  EXPECT_TRUE(StringViewLite("CA") >= "B");
  EXPECT_TRUE(StringViewLite("B") >= "AA");
  EXPECT_FALSE(StringViewLite("B") >= "BA");
  EXPECT_FALSE(StringViewLite("B") >= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_StringViewLite, OperatorEqual)
{
  EXPECT_TRUE(StringViewLite("A") == StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("") == StringViewLite(""));
  EXPECT_FALSE(StringViewLite("A") == StringViewLite(""));
  EXPECT_FALSE(StringViewLite("A") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("A") == StringViewLite("a"));

  EXPECT_FALSE(StringViewLite("A") == StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("C") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") == StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("B") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") == StringViewLite("C"));

  EXPECT_FALSE(StringViewLite("A") == StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("B") == StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("C") == StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("BA") == StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("BA") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("BA") == StringViewLite("C"));

  EXPECT_FALSE(StringViewLite("AA") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("BA") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("CA") == StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") == StringViewLite("AA"));
  EXPECT_FALSE(StringViewLite("B") == StringViewLite("BA"));
  EXPECT_FALSE(StringViewLite("B") == StringViewLite("CA"));
}


TEST(TestString_StringViewLite, OperatorEqual_string_view_LHS)
{
  EXPECT_TRUE(std::string_view("A") == StringViewLite("A"));
  EXPECT_TRUE(std::string_view("") == StringViewLite(""));
  EXPECT_FALSE(std::string_view("A") == StringViewLite(""));
  EXPECT_FALSE(std::string_view("A") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("A") == StringViewLite("a"));

  EXPECT_FALSE(std::string_view("A") == StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("C") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") == StringViewLite("A"));
  EXPECT_TRUE(std::string_view("B") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") == StringViewLite("C"));

  EXPECT_FALSE(std::string_view("A") == StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("B") == StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("C") == StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("BA") == StringViewLite("A"));
  EXPECT_FALSE(std::string_view("BA") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("BA") == StringViewLite("C"));

  EXPECT_FALSE(std::string_view("AA") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("BA") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("CA") == StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") == StringViewLite("AA"));
  EXPECT_FALSE(std::string_view("B") == StringViewLite("BA"));
  EXPECT_FALSE(std::string_view("B") == StringViewLite("CA"));
}


TEST(TestString_StringViewLite, OperatorEqual_string_view_RHS)
{
  EXPECT_TRUE(StringViewLite("A") == std::string_view("A"));
  EXPECT_TRUE(StringViewLite("") == std::string_view(""));
  EXPECT_FALSE(StringViewLite("A") == std::string_view(""));
  EXPECT_FALSE(StringViewLite("A") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("A") == std::string_view("a"));

  EXPECT_FALSE(StringViewLite("A") == std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("C") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") == std::string_view("A"));
  EXPECT_TRUE(StringViewLite("B") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") == std::string_view("C"));

  EXPECT_FALSE(StringViewLite("A") == std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("B") == std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("C") == std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("BA") == std::string_view("A"));
  EXPECT_FALSE(StringViewLite("BA") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("BA") == std::string_view("C"));

  EXPECT_FALSE(StringViewLite("AA") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("BA") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("CA") == std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") == std::string_view("AA"));
  EXPECT_FALSE(StringViewLite("B") == std::string_view("BA"));
  EXPECT_FALSE(StringViewLite("B") == std::string_view("CA"));
}


TEST(TestString_StringViewLite, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE("A" == StringViewLite("A"));
  EXPECT_TRUE("" == StringViewLite(""));    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE("A" == StringViewLite(""));
  EXPECT_FALSE("A" == StringViewLite("B"));
  EXPECT_FALSE("A" == StringViewLite("a"));

  EXPECT_FALSE("A" == StringViewLite("B"));
  EXPECT_TRUE("B" == StringViewLite("B"));
  EXPECT_FALSE("C" == StringViewLite("B"));
  EXPECT_FALSE("B" == StringViewLite("A"));
  EXPECT_TRUE("B" == StringViewLite("B"));
  EXPECT_FALSE("B" == StringViewLite("C"));

  EXPECT_FALSE("A" == StringViewLite("BA"));
  EXPECT_FALSE("B" == StringViewLite("BA"));
  EXPECT_FALSE("C" == StringViewLite("BA"));
  EXPECT_FALSE("BA" == StringViewLite("A"));
  EXPECT_FALSE("BA" == StringViewLite("B"));
  EXPECT_FALSE("BA" == StringViewLite("C"));

  EXPECT_FALSE("AA" == StringViewLite("B"));
  EXPECT_FALSE("BA" == StringViewLite("B"));
  EXPECT_FALSE("CA" == StringViewLite("B"));
  EXPECT_FALSE("B" == StringViewLite("AA"));
  EXPECT_FALSE("B" == StringViewLite("BA"));
  EXPECT_FALSE("B" == StringViewLite("CA"));
}


TEST(TestString_StringViewLite, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(StringViewLite("A") == "A");
  EXPECT_TRUE(StringViewLite("") == "");      // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(StringViewLite("A") == "");    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(StringViewLite("A") == "B");
  EXPECT_FALSE(StringViewLite("A") == "a");

  EXPECT_FALSE(StringViewLite("A") == "B");
  EXPECT_TRUE(StringViewLite("B") == "B");
  EXPECT_FALSE(StringViewLite("C") == "B");
  EXPECT_FALSE(StringViewLite("B") == "A");
  EXPECT_TRUE(StringViewLite("B") == "B");
  EXPECT_FALSE(StringViewLite("B") == "C");

  EXPECT_FALSE(StringViewLite("A") == "BA");
  EXPECT_FALSE(StringViewLite("B") == "BA");
  EXPECT_FALSE(StringViewLite("C") == "BA");
  EXPECT_FALSE(StringViewLite("BA") == "A");
  EXPECT_FALSE(StringViewLite("BA") == "B");
  EXPECT_FALSE(StringViewLite("BA") == "C");

  EXPECT_FALSE(StringViewLite("AA") == "B");
  EXPECT_FALSE(StringViewLite("BA") == "B");
  EXPECT_FALSE(StringViewLite("CA") == "B");
  EXPECT_FALSE(StringViewLite("B") == "AA");
  EXPECT_FALSE(StringViewLite("B") == "BA");
  EXPECT_FALSE(StringViewLite("B") == "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, OperatorNotEqual)
{
  EXPECT_FALSE(StringViewLite("A") != StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("") != StringViewLite(""));
  EXPECT_TRUE(StringViewLite("A") != StringViewLite(""));
  EXPECT_TRUE(StringViewLite("A") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("A") != StringViewLite("a"));

  EXPECT_TRUE(StringViewLite("A") != StringViewLite("B"));
  EXPECT_FALSE(StringViewLite("B") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("C") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") != StringViewLite("A"));
  EXPECT_FALSE(StringViewLite("B") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") != StringViewLite("C"));

  EXPECT_TRUE(StringViewLite("A") != StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("B") != StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("C") != StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("BA") != StringViewLite("A"));
  EXPECT_TRUE(StringViewLite("BA") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("BA") != StringViewLite("C"));

  EXPECT_TRUE(StringViewLite("AA") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("BA") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("CA") != StringViewLite("B"));
  EXPECT_TRUE(StringViewLite("B") != StringViewLite("AA"));
  EXPECT_TRUE(StringViewLite("B") != StringViewLite("BA"));
  EXPECT_TRUE(StringViewLite("B") != StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorNotEqual_string_view_LHS)
{
  EXPECT_FALSE(std::string_view("A") != StringViewLite("A"));
  EXPECT_FALSE(std::string_view("") != StringViewLite(""));
  EXPECT_TRUE(std::string_view("A") != StringViewLite(""));
  EXPECT_TRUE(std::string_view("A") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("A") != StringViewLite("a"));

  EXPECT_TRUE(std::string_view("A") != StringViewLite("B"));
  EXPECT_FALSE(std::string_view("B") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("C") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") != StringViewLite("A"));
  EXPECT_FALSE(std::string_view("B") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") != StringViewLite("C"));

  EXPECT_TRUE(std::string_view("A") != StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("B") != StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("C") != StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("BA") != StringViewLite("A"));
  EXPECT_TRUE(std::string_view("BA") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("BA") != StringViewLite("C"));

  EXPECT_TRUE(std::string_view("AA") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("BA") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("CA") != StringViewLite("B"));
  EXPECT_TRUE(std::string_view("B") != StringViewLite("AA"));
  EXPECT_TRUE(std::string_view("B") != StringViewLite("BA"));
  EXPECT_TRUE(std::string_view("B") != StringViewLite("CA"));
}

TEST(TestString_StringViewLite, OperatorNotEqual_string_view_RHS)
{
  EXPECT_FALSE(StringViewLite("A") != std::string_view("A"));
  EXPECT_FALSE(StringViewLite("") != std::string_view(""));
  EXPECT_TRUE(StringViewLite("A") != std::string_view(""));
  EXPECT_TRUE(StringViewLite("A") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("A") != std::string_view("a"));

  EXPECT_TRUE(StringViewLite("A") != std::string_view("B"));
  EXPECT_FALSE(StringViewLite("B") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("C") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") != std::string_view("A"));
  EXPECT_FALSE(StringViewLite("B") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") != std::string_view("C"));

  EXPECT_TRUE(StringViewLite("A") != std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("B") != std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("C") != std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("BA") != std::string_view("A"));
  EXPECT_TRUE(StringViewLite("BA") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("BA") != std::string_view("C"));

  EXPECT_TRUE(StringViewLite("AA") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("BA") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("CA") != std::string_view("B"));
  EXPECT_TRUE(StringViewLite("B") != std::string_view("AA"));
  EXPECT_TRUE(StringViewLite("B") != std::string_view("BA"));
  EXPECT_TRUE(StringViewLite("B") != std::string_view("CA"));
}


TEST(TestString_StringViewLite, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE("A" != StringViewLite("A"));
  EXPECT_FALSE("" != StringViewLite(""));    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE("A" != StringViewLite(""));
  EXPECT_TRUE("A" != StringViewLite("B"));
  EXPECT_TRUE("A" != StringViewLite("a"));

  EXPECT_TRUE("A" != StringViewLite("B"));
  EXPECT_FALSE("B" != StringViewLite("B"));
  EXPECT_TRUE("C" != StringViewLite("B"));
  EXPECT_TRUE("B" != StringViewLite("A"));
  EXPECT_FALSE("B" != StringViewLite("B"));
  EXPECT_TRUE("B" != StringViewLite("C"));

  EXPECT_TRUE("A" != StringViewLite("BA"));
  EXPECT_TRUE("B" != StringViewLite("BA"));
  EXPECT_TRUE("C" != StringViewLite("BA"));
  EXPECT_TRUE("BA" != StringViewLite("A"));
  EXPECT_TRUE("BA" != StringViewLite("B"));
  EXPECT_TRUE("BA" != StringViewLite("C"));

  EXPECT_TRUE("AA" != StringViewLite("B"));
  EXPECT_TRUE("BA" != StringViewLite("B"));
  EXPECT_TRUE("CA" != StringViewLite("B"));
  EXPECT_TRUE("B" != StringViewLite("AA"));
  EXPECT_TRUE("B" != StringViewLite("BA"));
  EXPECT_TRUE("B" != StringViewLite("CA"));
}


TEST(TestString_StringViewLite, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(StringViewLite("A") != "A");
  EXPECT_FALSE(StringViewLite("") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(StringViewLite("A") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(StringViewLite("A") != "B");
  EXPECT_TRUE(StringViewLite("A") != "a");

  EXPECT_TRUE(StringViewLite("A") != "B");
  EXPECT_FALSE(StringViewLite("B") != "B");
  EXPECT_TRUE(StringViewLite("C") != "B");
  EXPECT_TRUE(StringViewLite("B") != "A");
  EXPECT_FALSE(StringViewLite("B") != "B");
  EXPECT_TRUE(StringViewLite("B") != "C");

  EXPECT_TRUE(StringViewLite("A") != "BA");
  EXPECT_TRUE(StringViewLite("B") != "BA");
  EXPECT_TRUE(StringViewLite("C") != "BA");
  EXPECT_TRUE(StringViewLite("BA") != "A");
  EXPECT_TRUE(StringViewLite("BA") != "B");
  EXPECT_TRUE(StringViewLite("BA") != "C");

  EXPECT_TRUE(StringViewLite("AA") != "B");
  EXPECT_TRUE(StringViewLite("BA") != "B");
  EXPECT_TRUE(StringViewLite("CA") != "B");
  EXPECT_TRUE(StringViewLite("B") != "AA");
  EXPECT_TRUE(StringViewLite("B") != "BA");
  EXPECT_TRUE(StringViewLite("B") != "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator null handling
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringViewLite, OperatorLessThan_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(StringViewLite("A") < pszNull);
  EXPECT_FALSE(StringViewLite("A") < strViewNull);
  EXPECT_TRUE(pszNull < StringViewLite("A"));
  EXPECT_TRUE(strViewNull < StringViewLite("A"));

  // Compare against empty string
  EXPECT_FALSE(StringViewLite("") < pszNull);
  EXPECT_FALSE(StringViewLite("") < strViewNull);
  EXPECT_FALSE(pszNull < StringViewLite(""));
  EXPECT_FALSE(strViewNull < StringViewLite(""));

  // Compare against null
  EXPECT_FALSE(pszNull < strViewNull);
  EXPECT_FALSE(strViewNull < pszNull);
  EXPECT_FALSE(strViewNull < strViewNull);
}


TEST(TestString_StringViewLite, OperatorLessThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(StringViewLite("A") <= pszNull);
  EXPECT_FALSE(StringViewLite("A") <= strViewNull);
  EXPECT_TRUE(pszNull <= StringViewLite("A"));
  EXPECT_TRUE(strViewNull <= StringViewLite("A"));

  // Compare against empty string
  EXPECT_TRUE(StringViewLite("") <= pszNull);
  EXPECT_TRUE(StringViewLite("") <= strViewNull);
  EXPECT_TRUE(pszNull <= StringViewLite(""));
  EXPECT_TRUE(strViewNull <= StringViewLite(""));

  // Compare against null
  EXPECT_TRUE(pszNull <= strViewNull);
  EXPECT_TRUE(strViewNull <= pszNull);
  EXPECT_TRUE(strViewNull <= strViewNull);
}


TEST(TestString_StringViewLite, OperatorGreaterThan_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(StringViewLite("A") > pszNull);
  EXPECT_TRUE(StringViewLite("A") > strViewNull);
  EXPECT_FALSE(pszNull > StringViewLite("A"));
  EXPECT_FALSE(strViewNull > StringViewLite("A"));

  // Compare against empty string
  EXPECT_FALSE(StringViewLite("") > pszNull);
  EXPECT_FALSE(StringViewLite("") > strViewNull);
  EXPECT_FALSE(pszNull > StringViewLite(""));
  EXPECT_FALSE(strViewNull > StringViewLite(""));

  // Compare against null
  EXPECT_FALSE(pszNull > strViewNull);
  EXPECT_FALSE(strViewNull > pszNull);
  EXPECT_FALSE(strViewNull > strViewNull);
}


TEST(TestString_StringViewLite, OperatorGreaterThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(StringViewLite("A") >= pszNull);
  EXPECT_TRUE(StringViewLite("A") >= strViewNull);
  EXPECT_FALSE(pszNull >= StringViewLite("A"));
  EXPECT_FALSE(strViewNull >= StringViewLite("A"));

  // Compare against empty string
  EXPECT_TRUE(StringViewLite("") >= pszNull);
  EXPECT_TRUE(StringViewLite("") >= strViewNull);
  EXPECT_TRUE(pszNull >= StringViewLite(""));
  EXPECT_TRUE(strViewNull >= StringViewLite(""));

  // Compare against null
  EXPECT_TRUE(pszNull >= strViewNull);
  EXPECT_TRUE(strViewNull >= pszNull);
  EXPECT_TRUE(strViewNull >= strViewNull);
}


TEST(TestString_StringViewLite, OperatorEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_FALSE(StringViewLite("A") == pszNull);
  EXPECT_FALSE(StringViewLite("A") == strViewNull);
  EXPECT_FALSE(pszNull == StringViewLite("A"));
  EXPECT_FALSE(strViewNull == StringViewLite("A"));

  // Compare against empty string
  EXPECT_TRUE(pszNull == StringViewLite(""));
  EXPECT_TRUE(strViewNull == StringViewLite(""));
  EXPECT_TRUE(StringViewLite("") == pszNull);
  EXPECT_TRUE(StringViewLite("") == strViewNull);

  // Compare against null
  EXPECT_TRUE(pszNull == strViewNull);
  EXPECT_TRUE(strViewNull == pszNull);
  EXPECT_TRUE(strViewNull == strViewNull);
}


TEST(TestString_StringViewLite, OperatorNotEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr StringViewLite strViewNull(nullptr);

  EXPECT_TRUE(StringViewLite("A") != pszNull);
  EXPECT_TRUE(StringViewLite("A") != strViewNull);
  EXPECT_TRUE(pszNull != StringViewLite("A"));
  EXPECT_TRUE(strViewNull != StringViewLite("A"));

  // Compare against empty string
  EXPECT_FALSE(pszNull != StringViewLite(""));
  EXPECT_FALSE(strViewNull != StringViewLite(""));
  EXPECT_FALSE(StringViewLite("") != pszNull);
  EXPECT_FALSE(StringViewLite("") != strViewNull);

  // Compare against null
  EXPECT_FALSE(pszNull != strViewNull);
  EXPECT_FALSE(strViewNull != pszNull);
  EXPECT_FALSE(strViewNull != strViewNull);
}
