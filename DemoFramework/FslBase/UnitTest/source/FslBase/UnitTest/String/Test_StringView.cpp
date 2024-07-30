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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

namespace
{
  using TestString_StringView = TestFixtureFslBase;

  // Not necessary but made it easier to convert the existing std::string_view tests
  std::string_view Convert(const std::string& str)
  {
    return str;
  }
}


TEST(TestString_StringView, Construct)
{
  std::string_view strView;

  EXPECT_TRUE(strView.empty());
  EXPECT_EQ(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(TestString_StringView, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  std::string_view strView(psz, lenPsz);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), lenPsz);
  EXPECT_EQ(strView.length(), lenPsz);
}


TEST(TestString_StringView, Construct_FromStr)
{
  std::string str("Hello world");
  std::string_view strView = Convert(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}


TEST(TestString_StringView, SubStr)
{
  std::string_view strView("0123456789");

  EXPECT_EQ(strView, strView.substr());
  EXPECT_EQ("123456789", strView.substr(1u));
  EXPECT_EQ("9", strView.substr(9u));

  EXPECT_EQ("12", strView.substr(1u, 2));
  EXPECT_EQ("9", strView.substr(9u, 2));

  // its ok to read the last char
  EXPECT_EQ("", strView.substr(10u));
}

TEST(TestString_StringView, SubStr_Empty)
{
  std::string_view strView;

  EXPECT_EQ(std::string_view(), strView.substr());
  EXPECT_EQ("", strView.substr());
}


TEST(TestString_StringView, SubStr_InvalidPos)
{
  std::string_view strView("0123456789");

  EXPECT_THROW(auto res = strView.substr(11u), std::out_of_range);
}


TEST(TestString_StringView, front)
{
  std::string str("Hello world");
  std::string_view strView = Convert(str);

  EXPECT_EQ(strView.front(), str.front());
}


TEST(TestString_StringView, back)
{
  std::string str("Hello world");
  std::string_view strView = Convert(str);

  EXPECT_EQ(strView.back(), str.back());
}


TEST(TestString_StringView, opIndex)
{
  std::string str("Hello world");
  std::string_view strView = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(strView[i], str[i]);
  }
}


TEST(TestString_StringView, At)
{
  std::string str("Hello world");
  std::string_view strView = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(strView.at(i), str.at(i));
  }
}


TEST(TestString_StringView, At_OutOfBounds)
{
  std::string str("Hello world");
  std::string_view strView = Convert(str);

  EXPECT_THROW(auto res = strView.at(str.size()), std::out_of_range);
}


TEST(TestString_StringView, At_Empty_OutOfBounds)
{
  std::string str;
  std::string_view strView = Convert(str);

  EXPECT_THROW(auto res = strView.at(str.size()), std::out_of_range);
}


TEST(TestString_StringView, opEqual_Empty)
{
  std::string str1;
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_StringView, opEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_StringView, opEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringView, opEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringView, opEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringView, opEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_StringView, opNotEqual_Empty)
{
  std::string str1;
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_StringView, opNotEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_StringView, opNotEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringView, opNotEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringView, opNotEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringView, opNotEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_StringView, opLess_Empty)
{
  std::string str1;
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_StringView, opLess1)
{
  std::string str1("ab");
  std::string str2("ac");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_StringView, opLess2)
{
  std::string str1("ab");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_StringView, opLess3)
{
  std::string str1("ab");
  std::string str2("ab");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_StringView, opLess4)
{
  std::string str1("a");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_StringView, opLess5)
{
  std::string str1("aa");
  std::string str2("a");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}

TEST(TestString_StringView, opLessOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringView, opLessOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringView, opLessOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}


TEST(TestString_StringView, opLessOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringView, opLessOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_StringView, opLessOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}

TEST(TestString_StringView, opGreater_Empty)
{
  std::string str1;
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringView, opGreater1)
{
  std::string str1("ab");
  std::string str2("ac");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringView, opGreater2)
{
  std::string str1("ab");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}


TEST(TestString_StringView, opGreater3)
{
  std::string str1("ab");
  std::string str2("ab");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringView, opGreater4)
{
  std::string str1("a");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_StringView, opGreater5)
{
  std::string str1("aa");
  std::string str2("a");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}

TEST(TestString_StringView, opGreaterOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_StringView, opGreaterOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_StringView, opGreaterOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_StringView, opGreaterOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_StringView, opGreaterOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_StringView, opGreaterOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  std::string_view strView1 = Convert(str1);
  std::string_view strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// compare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, compare)
{
  EXPECT_TRUE(std::string_view("A").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("B")) == 0);
  EXPECT_TRUE(std::string_view("C").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(std::string_view("A").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(std::string_view("C").compare(std::string_view("BA")) > 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(std::string_view("AA").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("CA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("AA")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("CA")) < 0);
}

TEST(TestString_StringView, compare_string_view)
{
  EXPECT_TRUE(std::string_view("A").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("B")) == 0);
  EXPECT_TRUE(std::string_view("C").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(std::string_view("A").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(std::string_view("C").compare(std::string_view("BA")) > 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(std::string_view("AA").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(std::string_view("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("CA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("AA")) > 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(std::string_view("B").compare(std::string_view("CA")) < 0);
}

TEST(TestString_StringView, compare_CString)
{
  EXPECT_TRUE(std::string_view("A").compare("B") < 0);
  EXPECT_TRUE(std::string_view("B").compare("B") == 0);
  EXPECT_TRUE(std::string_view("C").compare("B") > 0);
  EXPECT_TRUE(std::string_view("B").compare("A") > 0);
  EXPECT_TRUE(std::string_view("B").compare("C") < 0);

  EXPECT_TRUE(std::string_view("A").compare("BA") < 0);
  EXPECT_TRUE(std::string_view("B").compare("BA") < 0);
  EXPECT_TRUE(std::string_view("C").compare("BA") > 0);
  EXPECT_TRUE(std::string_view("BA").compare("A") > 0);
  EXPECT_TRUE(std::string_view("BA").compare("B") > 0);
  EXPECT_TRUE(std::string_view("BA").compare("C") < 0);

  EXPECT_TRUE(std::string_view("AA").compare("B") < 0);
  EXPECT_TRUE(std::string_view("BA").compare("B") > 0);
  EXPECT_TRUE(std::string_view("CA").compare("B") > 0);
  EXPECT_TRUE(std::string_view("B").compare("AA") > 0);
  EXPECT_TRUE(std::string_view("B").compare("BA") < 0);
  EXPECT_TRUE(std::string_view("B").compare("CA") < 0);
}


TEST(TestString_StringView, compare_Empty)
{
  EXPECT_TRUE(std::string_view().compare(std::string_view()) == 0);
  EXPECT_TRUE(std::string_view("").compare(std::string_view()) == 0);
  EXPECT_TRUE(std::string_view().compare(std::string_view("")) == 0);
}

// TEST(TestString_StringView, compare_Null_CString)
//{
//   // This is unfortunately undefined behavior instead of just a empty view!
//   EXPECT_TRUE(std::string_view().compare(nullptr) == 0);
//   EXPECT_TRUE(std::string_view("").compare(nullptr) == 0);
// }

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// starts_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, starts_with)
{
  EXPECT_TRUE(std::string_view().starts_with(std::string_view()));
  EXPECT_TRUE(std::string_view().starts_with(std::string_view("")));

  EXPECT_TRUE(std::string_view("").starts_with(std::string_view()));
  EXPECT_TRUE(std::string_view("").starts_with(std::string_view("")));

  EXPECT_TRUE(std::string_view("A").starts_with(std::string_view()));
  EXPECT_TRUE(std::string_view("A").starts_with(std::string_view("")));
  EXPECT_TRUE(std::string_view("A").starts_with(std::string_view("A")));

  EXPECT_TRUE(std::string_view("AB").starts_with(std::string_view()));
  EXPECT_TRUE(std::string_view("AB").starts_with(std::string_view("")));
  EXPECT_TRUE(std::string_view("AB").starts_with(std::string_view("A")));
  EXPECT_TRUE(std::string_view("AB").starts_with(std::string_view("AB")));

  EXPECT_TRUE(std::string_view("ABC").starts_with(std::string_view()));
  EXPECT_TRUE(std::string_view("ABC").starts_with(std::string_view("")));
  EXPECT_TRUE(std::string_view("ABC").starts_with(std::string_view("A")));
  EXPECT_TRUE(std::string_view("ABC").starts_with(std::string_view("AB")));
  EXPECT_TRUE(std::string_view("ABC").starts_with(std::string_view("ABC")));

  EXPECT_FALSE(std::string_view().starts_with(std::string_view("A")));

  EXPECT_FALSE(std::string_view("").starts_with(std::string_view("A")));

  EXPECT_FALSE(std::string_view("A").starts_with(std::string_view("B")));
  EXPECT_FALSE(std::string_view("A").starts_with(std::string_view("AB")));

  EXPECT_FALSE(std::string_view("AB").starts_with(std::string_view("B")));
  EXPECT_FALSE(std::string_view("AB").starts_with(std::string_view("AC")));
  EXPECT_FALSE(std::string_view("AB").starts_with(std::string_view("ABC")));

  EXPECT_FALSE(std::string_view("ABC").starts_with(std::string_view("B")));
  EXPECT_FALSE(std::string_view("ABC").starts_with(std::string_view("AC")));
  EXPECT_FALSE(std::string_view("ABC").starts_with(std::string_view("ABD")));
  EXPECT_FALSE(std::string_view("ABC").starts_with(std::string_view("ABDA")));
}


TEST(TestString_StringView, starts_with_CString)
{
  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view().starts_with(nullptr));
  EXPECT_TRUE(std::string_view().starts_with(""));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("").starts_with(nullptr));
  EXPECT_TRUE(std::string_view("").starts_with(""));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("A").starts_with(nullptr));
  EXPECT_TRUE(std::string_view("A").starts_with(""));
  EXPECT_TRUE(std::string_view("A").starts_with("A"));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("AB").starts_with(nullptr));
  EXPECT_TRUE(std::string_view("AB").starts_with(""));
  EXPECT_TRUE(std::string_view("AB").starts_with("A"));
  EXPECT_TRUE(std::string_view("AB").starts_with("AB"));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("ABC").starts_with(nullptr));
  EXPECT_TRUE(std::string_view("ABC").starts_with(""));
  EXPECT_TRUE(std::string_view("ABC").starts_with("A"));
  EXPECT_TRUE(std::string_view("ABC").starts_with("AB"));
  EXPECT_TRUE(std::string_view("ABC").starts_with("ABC"));

  EXPECT_FALSE(std::string_view().starts_with("A"));

  EXPECT_FALSE(std::string_view("").starts_with("A"));

  EXPECT_FALSE(std::string_view("A").starts_with("B"));
  EXPECT_FALSE(std::string_view("A").starts_with("AB"));

  EXPECT_FALSE(std::string_view("AB").starts_with("B"));
  EXPECT_FALSE(std::string_view("AB").starts_with("AC"));
  EXPECT_FALSE(std::string_view("AB").starts_with("ABC"));

  EXPECT_FALSE(std::string_view("ABC").starts_with("B"));
  EXPECT_FALSE(std::string_view("ABC").starts_with("AC"));
  EXPECT_FALSE(std::string_view("ABC").starts_with("ABD"));
  EXPECT_FALSE(std::string_view("ABC").starts_with("ABDA"));
}

TEST(TestString_StringView, starts_with_char)
{
  EXPECT_TRUE(std::string_view("A").starts_with('A'));
  EXPECT_TRUE(std::string_view("B").starts_with('B'));

  EXPECT_TRUE(std::string_view("AB").starts_with('A'));
  EXPECT_TRUE(std::string_view("BC").starts_with('B'));

  EXPECT_FALSE(std::string_view().starts_with(' '));
  EXPECT_FALSE(std::string_view().starts_with('a'));

  EXPECT_FALSE(std::string_view("").starts_with(' '));
  EXPECT_FALSE(std::string_view("").starts_with('a'));

  EXPECT_FALSE(std::string_view("A").starts_with(' '));
  EXPECT_FALSE(std::string_view("A").starts_with('a'));

  EXPECT_FALSE(std::string_view("AB").starts_with(' '));
  EXPECT_FALSE(std::string_view("AB").starts_with('a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ends_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, ends_with)
{
  EXPECT_TRUE(std::string_view().ends_with(std::string_view()));
  EXPECT_TRUE(std::string_view().ends_with(std::string_view("")));

  EXPECT_TRUE(std::string_view("").ends_with(std::string_view()));
  EXPECT_TRUE(std::string_view("").ends_with(std::string_view("")));

  EXPECT_TRUE(std::string_view("A").ends_with(std::string_view()));
  EXPECT_TRUE(std::string_view("A").ends_with(std::string_view("")));
  EXPECT_TRUE(std::string_view("A").ends_with(std::string_view("A")));

  EXPECT_TRUE(std::string_view("BA").ends_with(std::string_view()));
  EXPECT_TRUE(std::string_view("BA").ends_with(std::string_view("")));
  EXPECT_TRUE(std::string_view("BA").ends_with(std::string_view("A")));
  EXPECT_TRUE(std::string_view("BA").ends_with(std::string_view("BA")));

  EXPECT_TRUE(std::string_view("CBA").ends_with(std::string_view()));
  EXPECT_TRUE(std::string_view("CBA").ends_with(std::string_view("")));
  EXPECT_TRUE(std::string_view("CBA").ends_with(std::string_view("A")));
  EXPECT_TRUE(std::string_view("CBA").ends_with(std::string_view("BA")));
  EXPECT_TRUE(std::string_view("CBA").ends_with(std::string_view("CBA")));

  EXPECT_FALSE(std::string_view().ends_with(std::string_view("A")));

  EXPECT_FALSE(std::string_view("").ends_with(std::string_view("A")));

  EXPECT_FALSE(std::string_view("A").ends_with(std::string_view("B")));
  EXPECT_FALSE(std::string_view("A").ends_with(std::string_view("BA")));

  EXPECT_FALSE(std::string_view("BA").ends_with(std::string_view("B")));
  EXPECT_FALSE(std::string_view("BA").ends_with(std::string_view("CA")));
  EXPECT_FALSE(std::string_view("BA").ends_with(std::string_view("CBA")));

  EXPECT_FALSE(std::string_view("CBA").ends_with(std::string_view("B")));
  EXPECT_FALSE(std::string_view("CBA").ends_with(std::string_view("CA")));
  EXPECT_FALSE(std::string_view("CBA").ends_with(std::string_view("DBA")));
  EXPECT_FALSE(std::string_view("CBA").ends_with(std::string_view("ADBA")));
}


TEST(TestString_StringView, ends_with_CString)
{
  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view().ends_with(nullptr));
  EXPECT_TRUE(std::string_view().ends_with(""));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("").ends_with(nullptr));
  EXPECT_TRUE(std::string_view("").ends_with(""));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("A").ends_with(nullptr));
  EXPECT_TRUE(std::string_view("A").ends_with(""));
  EXPECT_TRUE(std::string_view("A").ends_with("A"));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("BA").ends_with(nullptr));
  EXPECT_TRUE(std::string_view("BA").ends_with(""));
  EXPECT_TRUE(std::string_view("BA").ends_with("A"));
  EXPECT_TRUE(std::string_view("BA").ends_with("BA"));

  // This is unfortunately undefined behavior instead of just a empty view!
  // EXPECT_TRUE(std::string_view("CBA").ends_with(nullptr));
  EXPECT_TRUE(std::string_view("CBA").ends_with(""));
  EXPECT_TRUE(std::string_view("CBA").ends_with("A"));
  EXPECT_TRUE(std::string_view("CBA").ends_with("BA"));
  EXPECT_TRUE(std::string_view("CBA").ends_with("CBA"));

  EXPECT_FALSE(std::string_view().ends_with("A"));

  EXPECT_FALSE(std::string_view("").ends_with("A"));

  EXPECT_FALSE(std::string_view("A").ends_with("B"));
  EXPECT_FALSE(std::string_view("A").ends_with("BA"));

  EXPECT_FALSE(std::string_view("BA").ends_with("B"));
  EXPECT_FALSE(std::string_view("BA").ends_with("CA"));
  EXPECT_FALSE(std::string_view("BA").ends_with("CBA"));

  EXPECT_FALSE(std::string_view("CBA").ends_with("B"));
  EXPECT_FALSE(std::string_view("CBA").ends_with("CA"));
  EXPECT_FALSE(std::string_view("CBA").ends_with("DBA"));
  EXPECT_FALSE(std::string_view("CBA").ends_with("ADBA"));
}


TEST(TestString_StringView, ends_with_char)
{
  EXPECT_TRUE(std::string_view("A").ends_with('A'));
  EXPECT_TRUE(std::string_view("B").ends_with('B'));

  EXPECT_TRUE(std::string_view("BA").ends_with('A'));
  EXPECT_TRUE(std::string_view("CB").ends_with('B'));

  EXPECT_FALSE(std::string_view().ends_with(' '));
  EXPECT_FALSE(std::string_view().ends_with('a'));

  EXPECT_FALSE(std::string_view("").ends_with(' '));
  EXPECT_FALSE(std::string_view("").ends_with('a'));

  EXPECT_FALSE(std::string_view("A").ends_with(' '));
  EXPECT_FALSE(std::string_view("A").ends_with('a'));

  EXPECT_FALSE(std::string_view("BA").ends_with(' '));
  EXPECT_FALSE(std::string_view("BA").ends_with('a'));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// find
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, find_char)
{
  EXPECT_EQ(0u, std::string_view("a").find('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("a").find('a', 1u));

  EXPECT_EQ(0u, std::string_view("aa").find('a'));
  EXPECT_EQ(1u, std::string_view("aa").find('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("aa").find('a', 2u));

  EXPECT_EQ(0u, std::string_view("aba").find('a'));
  EXPECT_EQ(2u, std::string_view("aba").find('a', 1u));
  EXPECT_EQ(2u, std::string_view("aba").find('a', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("aba").find('a', 3u));

  EXPECT_EQ(2u, std::string_view("cba").find('a'));
  EXPECT_EQ(2u, std::string_view("cba").find('a', 1u));
  EXPECT_EQ(2u, std::string_view("cba").find('a', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("cba").find('a', 3u));


  EXPECT_EQ(std::string_view::npos, std::string_view().find(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view().find('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view().find(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view().find('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view().find(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view().find('a', 2u));

  EXPECT_EQ(std::string_view::npos, std::string_view("").find(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view("").find('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("").find(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("").find('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("").find(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("").find('a', 2u));

  EXPECT_EQ(std::string_view::npos, std::string_view("a").find(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view("b").find('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("a").find(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("b").find('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("a").find(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("b").find('a', 2u));

  EXPECT_EQ(std::string_view::npos, std::string_view("ab").find(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view("bc").find('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("ab").find(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("bc").find('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("ab").find(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("bc").find('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// rfind
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, rfind_char)
{
  EXPECT_EQ(0u, std::string_view("a").rfind('a'));
  EXPECT_EQ(0u, std::string_view("a").rfind('a', 0u));
  EXPECT_EQ(0u, std::string_view("a").rfind('a', 1u));

  EXPECT_EQ(1u, std::string_view("aa").rfind('a'));
  EXPECT_EQ(0u, std::string_view("aa").rfind('a', 0u));
  EXPECT_EQ(1u, std::string_view("aa").rfind('a', 1u));
  EXPECT_EQ(1u, std::string_view("aa").rfind('a', 2u));

  EXPECT_EQ(2u, std::string_view("aba").rfind('a'));
  EXPECT_EQ(0u, std::string_view("aba").rfind('a', 0u));
  EXPECT_EQ(0u, std::string_view("aba").rfind('a', 1u));
  EXPECT_EQ(2u, std::string_view("aba").rfind('a', 2u));
  EXPECT_EQ(2u, std::string_view("aba").rfind('a', 3u));

  EXPECT_EQ(2u, std::string_view("cba").rfind('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("cba").rfind('a', 0u));
  EXPECT_EQ(std::string_view::npos, std::string_view("cba").rfind('a', 1u));
  EXPECT_EQ(2u, std::string_view("cba").rfind('a', 2u));
  EXPECT_EQ(2u, std::string_view("cba").rfind('a', 3u));

  EXPECT_EQ(std::string_view::npos, std::string_view().rfind(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view().rfind('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view().rfind(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view().rfind('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view().rfind(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view().rfind('a', 2u));

  EXPECT_EQ(std::string_view::npos, std::string_view("").rfind(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view("").rfind('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("").rfind(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("").rfind('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("").rfind(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("").rfind('a', 2u));

  EXPECT_EQ(std::string_view::npos, std::string_view("a").rfind(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view("b").rfind('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("a").rfind(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("b").rfind('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("a").rfind(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("b").rfind('a', 2u));

  EXPECT_EQ(std::string_view::npos, std::string_view("ab").rfind(' '));
  EXPECT_EQ(std::string_view::npos, std::string_view("bc").rfind('a'));
  EXPECT_EQ(std::string_view::npos, std::string_view("ab").rfind(' ', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("bc").rfind('a', 1u));
  EXPECT_EQ(std::string_view::npos, std::string_view("ab").rfind(' ', 2u));
  EXPECT_EQ(std::string_view::npos, std::string_view("bc").rfind('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, OperatorLessThan)
{
  EXPECT_LT(std::string_view("A"), std::string_view("B"));
  EXPECT_LT(std::string_view("A"), std::string_view("AB"));
  EXPECT_LT(std::string_view(""), std::string_view("AB"));

  EXPECT_TRUE(std::string_view("A") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") < std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") < std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") < std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThan_string_view_LHS)
{
  EXPECT_LT(std::string_view("A"), std::string_view("B"));
  EXPECT_LT(std::string_view("A"), std::string_view("AB"));
  EXPECT_LT(std::string_view(""), std::string_view("AB"));

  EXPECT_TRUE(std::string_view("A") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") < std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") < std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") < std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThan_string_view_RHS)
{
  EXPECT_LT(std::string_view("A"), std::string_view("B"));
  EXPECT_LT(std::string_view("A"), std::string_view("AB"));
  EXPECT_LT(std::string_view(""), std::string_view("AB"));

  EXPECT_TRUE(std::string_view("A") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") < std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") < std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") < std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") < std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") < std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThan_CString_LHS)
{
  EXPECT_LT("A", std::string_view("B"));
  EXPECT_LT("A", std::string_view("AB"));
  EXPECT_LT("", std::string_view("AB"));

  EXPECT_TRUE("A" < std::string_view("B"));
  EXPECT_FALSE("B" < std::string_view("B"));
  EXPECT_FALSE("C" < std::string_view("B"));
  EXPECT_FALSE("B" < std::string_view("A"));
  EXPECT_FALSE("B" < std::string_view("B"));
  EXPECT_TRUE("B" < std::string_view("C"));

  EXPECT_TRUE("A" < std::string_view("BA"));
  EXPECT_TRUE("B" < std::string_view("BA"));
  EXPECT_FALSE("C" < std::string_view("BA"));
  EXPECT_FALSE("BA" < std::string_view("A"));
  EXPECT_FALSE("BA" < std::string_view("B"));
  EXPECT_TRUE("BA" < std::string_view("C"));

  EXPECT_TRUE("AA" < std::string_view("B"));
  EXPECT_FALSE("BA" < std::string_view("B"));
  EXPECT_FALSE("CA" < std::string_view("B"));
  EXPECT_FALSE("B" < std::string_view("AA"));
  EXPECT_TRUE("B" < std::string_view("BA"));
  EXPECT_TRUE("B" < std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThan_CString_RHS)
{
  EXPECT_LT(std::string_view("A"), "B");
  EXPECT_LT(std::string_view("A"), "AB");
  EXPECT_LT(std::string_view(""), "AB");

  EXPECT_TRUE(std::string_view("A") < "B");
  EXPECT_FALSE(std::string_view("B") < "B");
  EXPECT_FALSE(std::string_view("C") < "B");
  EXPECT_FALSE(std::string_view("B") < "A");
  EXPECT_FALSE(std::string_view("B") < "B");
  EXPECT_TRUE(std::string_view("B") < "C");

  EXPECT_TRUE(std::string_view("A") < "BA");
  EXPECT_TRUE(std::string_view("B") < "BA");
  EXPECT_FALSE(std::string_view("C") < "BA");
  EXPECT_FALSE(std::string_view("BA") < "A");
  EXPECT_FALSE(std::string_view("BA") < "B");
  EXPECT_TRUE(std::string_view("BA") < "C");

  EXPECT_TRUE(std::string_view("AA") < "B");
  EXPECT_FALSE(std::string_view("BA") < "B");
  EXPECT_FALSE(std::string_view("CA") < "B");
  EXPECT_FALSE(std::string_view("B") < "AA");
  EXPECT_TRUE(std::string_view("B") < "BA");
  EXPECT_TRUE(std::string_view("B") < "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, OperatorLessThanOrEqual)
{
  EXPECT_LE(std::string_view("A"), std::string_view("B"));
  EXPECT_LE(std::string_view("A"), std::string_view("AB"));
  EXPECT_LE(std::string_view(""), std::string_view("AB"));
  EXPECT_LE(std::string_view("A"), std::string_view("A"));

  EXPECT_TRUE(std::string_view("A") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") <= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") <= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") <= std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThanOrEqual_string_view_lhs)
{
  EXPECT_LE(std::string_view("A"), std::string_view("B"));
  EXPECT_LE(std::string_view("A"), std::string_view("AB"));
  EXPECT_LE(std::string_view(""), std::string_view("AB"));
  EXPECT_LE(std::string_view("A"), std::string_view("A"));

  EXPECT_TRUE(std::string_view("A") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") <= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") <= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") <= std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThanOrEqual_string_view_rhs)
{
  EXPECT_LE(std::string_view("A"), std::string_view("B"));
  EXPECT_LE(std::string_view("A"), std::string_view("AB"));
  EXPECT_LE(std::string_view(""), std::string_view("AB"));
  EXPECT_LE(std::string_view("A"), std::string_view("A"));

  EXPECT_TRUE(std::string_view("A") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") <= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") <= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") <= std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") <= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThanOrEqual_CString_LHS)
{
  EXPECT_LE("A", std::string_view("B"));
  EXPECT_LE("A", std::string_view("AB"));
  EXPECT_LE("", std::string_view("AB"));
  EXPECT_LE("A", std::string_view("A"));

  EXPECT_TRUE("A" <= std::string_view("B"));
  EXPECT_TRUE("B" <= std::string_view("B"));
  EXPECT_FALSE("C" <= std::string_view("B"));
  EXPECT_FALSE("B" <= std::string_view("A"));
  EXPECT_TRUE("B" <= std::string_view("B"));
  EXPECT_TRUE("B" <= std::string_view("C"));

  EXPECT_TRUE("A" <= std::string_view("BA"));
  EXPECT_TRUE("B" <= std::string_view("BA"));
  EXPECT_FALSE("C" <= std::string_view("BA"));
  EXPECT_FALSE("BA" <= std::string_view("A"));
  EXPECT_FALSE("BA" <= std::string_view("B"));
  EXPECT_TRUE("BA" <= std::string_view("C"));

  EXPECT_TRUE("AA" <= std::string_view("B"));
  EXPECT_FALSE("BA" <= std::string_view("B"));
  EXPECT_FALSE("CA" <= std::string_view("B"));
  EXPECT_FALSE("B" <= std::string_view("AA"));
  EXPECT_TRUE("B" <= std::string_view("BA"));
  EXPECT_TRUE("B" <= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorLessThanOrEqual_CString_RHS)
{
  EXPECT_LE(std::string_view("A"), "B");
  EXPECT_LE(std::string_view("A"), "AB");
  EXPECT_LE(std::string_view(""), "AB");
  EXPECT_LE(std::string_view("A"), "A");

  EXPECT_TRUE(std::string_view("A") <= "B");
  EXPECT_TRUE(std::string_view("B") <= "B");
  EXPECT_FALSE(std::string_view("C") <= "B");
  EXPECT_FALSE(std::string_view("B") <= "A");
  EXPECT_TRUE(std::string_view("B") <= "B");
  EXPECT_TRUE(std::string_view("B") <= "C");

  EXPECT_TRUE(std::string_view("A") <= "BA");
  EXPECT_TRUE(std::string_view("B") <= "BA");
  EXPECT_FALSE(std::string_view("C") <= "BA");
  EXPECT_FALSE(std::string_view("BA") <= "A");
  EXPECT_FALSE(std::string_view("BA") <= "B");
  EXPECT_TRUE(std::string_view("BA") <= "C");

  EXPECT_TRUE(std::string_view("AA") <= "B");
  EXPECT_FALSE(std::string_view("BA") <= "B");
  EXPECT_FALSE(std::string_view("CA") <= "B");
  EXPECT_FALSE(std::string_view("B") <= "AA");
  EXPECT_TRUE(std::string_view("B") <= "BA");
  EXPECT_TRUE(std::string_view("B") <= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, OperatorGreaterThan)
{
  EXPECT_GT(std::string_view("B"), std::string_view("A"));
  EXPECT_GT(std::string_view("AB"), std::string_view("A"));
  EXPECT_GT(std::string_view("AB"), std::string_view(""));

  EXPECT_FALSE(std::string_view("A") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") > std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") > std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") > std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") > std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") > std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThan_string_view_LHS)
{
  EXPECT_GT(std::string_view("B"), std::string_view("A"));
  EXPECT_GT(std::string_view("AB"), std::string_view("A"));
  EXPECT_GT(std::string_view("AB"), std::string_view(""));

  EXPECT_FALSE(std::string_view("A") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") > std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") > std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") > std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") > std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") > std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThan_string_view_RHS)
{
  EXPECT_GT(std::string_view("B"), std::string_view("A"));
  EXPECT_GT(std::string_view("AB"), std::string_view("A"));
  EXPECT_GT(std::string_view("AB"), std::string_view(""));

  EXPECT_FALSE(std::string_view("A") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") > std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") > std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") > std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") > std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") > std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") > std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThan_CString_LHS)
{
  EXPECT_GT("B", std::string_view("A"));
  EXPECT_GT("AB", std::string_view("A"));
  EXPECT_GT("AB", std::string_view(""));

  EXPECT_FALSE("A" > std::string_view("B"));
  EXPECT_FALSE("B" > std::string_view("B"));
  EXPECT_TRUE("C" > std::string_view("B"));
  EXPECT_TRUE("B" > std::string_view("A"));
  EXPECT_FALSE("B" > std::string_view("B"));
  EXPECT_FALSE("B" > std::string_view("C"));

  EXPECT_FALSE("A" > std::string_view("BA"));
  EXPECT_FALSE("B" > std::string_view("BA"));
  EXPECT_TRUE("C" > std::string_view("BA"));
  EXPECT_TRUE("BA" > std::string_view("A"));
  EXPECT_TRUE("BA" > std::string_view("B"));
  EXPECT_FALSE("BA" > std::string_view("C"));

  EXPECT_FALSE("AA" > std::string_view("B"));
  EXPECT_TRUE("BA" > std::string_view("B"));
  EXPECT_TRUE("CA" > std::string_view("B"));
  EXPECT_TRUE("B" > std::string_view("AA"));
  EXPECT_FALSE("B" > std::string_view("BA"));
  EXPECT_FALSE("B" > std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThan_CString_RHS)
{
  EXPECT_GT(std::string_view("B"), "A");
  EXPECT_GT(std::string_view("AB"), "A");
  EXPECT_GT(std::string_view("AB"), "");

  EXPECT_FALSE(std::string_view("A") > "B");
  EXPECT_FALSE(std::string_view("B") > "B");
  EXPECT_TRUE(std::string_view("C") > "B");
  EXPECT_TRUE(std::string_view("B") > "A");
  EXPECT_FALSE(std::string_view("B") > "B");
  EXPECT_FALSE(std::string_view("B") > "C");

  EXPECT_FALSE(std::string_view("A") > "BA");
  EXPECT_FALSE(std::string_view("B") > "BA");
  EXPECT_TRUE(std::string_view("C") > "BA");
  EXPECT_TRUE(std::string_view("BA") > "A");
  EXPECT_TRUE(std::string_view("BA") > "B");
  EXPECT_FALSE(std::string_view("BA") > "C");

  EXPECT_FALSE(std::string_view("AA") > "B");
  EXPECT_TRUE(std::string_view("BA") > "B");
  EXPECT_TRUE(std::string_view("CA") > "B");
  EXPECT_TRUE(std::string_view("B") > "AA");
  EXPECT_FALSE(std::string_view("B") > "BA");
  EXPECT_FALSE(std::string_view("B") > "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(std::string_view("B"), std::string_view("A"));
  EXPECT_GE(std::string_view("AB"), std::string_view("A"));
  EXPECT_GE(std::string_view("AB"), std::string_view(""));
  EXPECT_GE(std::string_view("A"), std::string_view("A"));

  EXPECT_FALSE(std::string_view("A") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") >= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") >= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") >= std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThanOrEqual_string_view_LHS)
{
  EXPECT_GE(std::string_view("B"), std::string_view("A"));
  EXPECT_GE(std::string_view("AB"), std::string_view("A"));
  EXPECT_GE(std::string_view("AB"), std::string_view(""));
  EXPECT_GE(std::string_view("A"), std::string_view("A"));

  EXPECT_FALSE(std::string_view("A") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") >= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") >= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") >= std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThanOrEqual_string_view_RHS)
{
  EXPECT_GE(std::string_view("B"), std::string_view("A"));
  EXPECT_GE(std::string_view("AB"), std::string_view("A"));
  EXPECT_GE(std::string_view("AB"), std::string_view(""));
  EXPECT_GE(std::string_view("A"), std::string_view("A"));

  EXPECT_FALSE(std::string_view("A") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") >= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") >= std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") >= std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") >= std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThanOrEqual_CString_LHS)
{
  EXPECT_GE("B", std::string_view("A"));
  EXPECT_GE("AB", std::string_view("A"));
  EXPECT_GE("AB", std::string_view(""));
  EXPECT_GE("A", std::string_view("A"));

  EXPECT_FALSE("A" >= std::string_view("B"));
  EXPECT_TRUE("B" >= std::string_view("B"));
  EXPECT_TRUE("C" >= std::string_view("B"));
  EXPECT_TRUE("B" >= std::string_view("A"));
  EXPECT_TRUE("B" >= std::string_view("B"));
  EXPECT_FALSE("B" >= std::string_view("C"));

  EXPECT_FALSE("A" >= std::string_view("BA"));
  EXPECT_FALSE("B" >= std::string_view("BA"));
  EXPECT_TRUE("C" >= std::string_view("BA"));
  EXPECT_TRUE("BA" >= std::string_view("A"));
  EXPECT_TRUE("BA" >= std::string_view("B"));
  EXPECT_FALSE("BA" >= std::string_view("C"));

  EXPECT_FALSE("AA" >= std::string_view("B"));
  EXPECT_TRUE("BA" >= std::string_view("B"));
  EXPECT_TRUE("CA" >= std::string_view("B"));
  EXPECT_TRUE("B" >= std::string_view("AA"));
  EXPECT_FALSE("B" >= std::string_view("BA"));
  EXPECT_FALSE("B" >= std::string_view("CA"));
}

TEST(TestString_StringView, OperatorGreaterThanOrEqual_CString_RHS)
{
  EXPECT_GE(std::string_view("B"), "A");
  EXPECT_GE(std::string_view("AB"), "A");
  EXPECT_GE(std::string_view("AB"), "");
  EXPECT_GE(std::string_view("A"), "A");

  EXPECT_FALSE(std::string_view("A") >= "B");
  EXPECT_TRUE(std::string_view("B") >= "B");
  EXPECT_TRUE(std::string_view("C") >= "B");
  EXPECT_TRUE(std::string_view("B") >= "A");
  EXPECT_TRUE(std::string_view("B") >= "B");
  EXPECT_FALSE(std::string_view("B") >= "C");

  EXPECT_FALSE(std::string_view("A") >= "BA");
  EXPECT_FALSE(std::string_view("B") >= "BA");
  EXPECT_TRUE(std::string_view("C") >= "BA");
  EXPECT_TRUE(std::string_view("BA") >= "A");
  EXPECT_TRUE(std::string_view("BA") >= "B");
  EXPECT_FALSE(std::string_view("BA") >= "C");

  EXPECT_FALSE(std::string_view("AA") >= "B");
  EXPECT_TRUE(std::string_view("BA") >= "B");
  EXPECT_TRUE(std::string_view("CA") >= "B");
  EXPECT_TRUE(std::string_view("B") >= "AA");
  EXPECT_FALSE(std::string_view("B") >= "BA");
  EXPECT_FALSE(std::string_view("B") >= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_StringView, OperatorEqual)
{
  EXPECT_TRUE(std::string_view("A") == std::string_view("A"));
  EXPECT_TRUE(std::string_view("") == std::string_view(""));
  EXPECT_FALSE(std::string_view("A") == std::string_view(""));
  EXPECT_FALSE(std::string_view("A") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("A") == std::string_view("a"));

  EXPECT_FALSE(std::string_view("A") == std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("CA"));
}


TEST(TestString_StringView, OperatorEqual_string_view_LHS)
{
  EXPECT_TRUE(std::string_view("A") == std::string_view("A"));
  EXPECT_TRUE(std::string_view("") == std::string_view(""));
  EXPECT_FALSE(std::string_view("A") == std::string_view(""));
  EXPECT_FALSE(std::string_view("A") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("A") == std::string_view("a"));

  EXPECT_FALSE(std::string_view("A") == std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("CA"));
}


TEST(TestString_StringView, OperatorEqual_string_view_RHS)
{
  EXPECT_TRUE(std::string_view("A") == std::string_view("A"));
  EXPECT_TRUE(std::string_view("") == std::string_view(""));
  EXPECT_FALSE(std::string_view("A") == std::string_view(""));
  EXPECT_FALSE(std::string_view("A") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("A") == std::string_view("a"));

  EXPECT_FALSE(std::string_view("A") == std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("C") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("A"));
  EXPECT_TRUE(std::string_view("B") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("C"));

  EXPECT_FALSE(std::string_view("A") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("C") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("A"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("C"));

  EXPECT_FALSE(std::string_view("AA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("CA") == std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("AA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string_view("CA"));
}


TEST(TestString_StringView, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE("A" == std::string_view("A"));
  EXPECT_TRUE("" == std::string_view(""));    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE("A" == std::string_view(""));
  EXPECT_FALSE("A" == std::string_view("B"));
  EXPECT_FALSE("A" == std::string_view("a"));

  EXPECT_FALSE("A" == std::string_view("B"));
  EXPECT_TRUE("B" == std::string_view("B"));
  EXPECT_FALSE("C" == std::string_view("B"));
  EXPECT_FALSE("B" == std::string_view("A"));
  EXPECT_TRUE("B" == std::string_view("B"));
  EXPECT_FALSE("B" == std::string_view("C"));

  EXPECT_FALSE("A" == std::string_view("BA"));
  EXPECT_FALSE("B" == std::string_view("BA"));
  EXPECT_FALSE("C" == std::string_view("BA"));
  EXPECT_FALSE("BA" == std::string_view("A"));
  EXPECT_FALSE("BA" == std::string_view("B"));
  EXPECT_FALSE("BA" == std::string_view("C"));

  EXPECT_FALSE("AA" == std::string_view("B"));
  EXPECT_FALSE("BA" == std::string_view("B"));
  EXPECT_FALSE("CA" == std::string_view("B"));
  EXPECT_FALSE("B" == std::string_view("AA"));
  EXPECT_FALSE("B" == std::string_view("BA"));
  EXPECT_FALSE("B" == std::string_view("CA"));
}


TEST(TestString_StringView, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(std::string_view("A") == "A");
  EXPECT_TRUE(std::string_view("") == "");      // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(std::string_view("A") == "");    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(std::string_view("A") == "B");
  EXPECT_FALSE(std::string_view("A") == "a");

  EXPECT_FALSE(std::string_view("A") == "B");
  EXPECT_TRUE(std::string_view("B") == "B");
  EXPECT_FALSE(std::string_view("C") == "B");
  EXPECT_FALSE(std::string_view("B") == "A");
  EXPECT_TRUE(std::string_view("B") == "B");
  EXPECT_FALSE(std::string_view("B") == "C");

  EXPECT_FALSE(std::string_view("A") == "BA");
  EXPECT_FALSE(std::string_view("B") == "BA");
  EXPECT_FALSE(std::string_view("C") == "BA");
  EXPECT_FALSE(std::string_view("BA") == "A");
  EXPECT_FALSE(std::string_view("BA") == "B");
  EXPECT_FALSE(std::string_view("BA") == "C");

  EXPECT_FALSE(std::string_view("AA") == "B");
  EXPECT_FALSE(std::string_view("BA") == "B");
  EXPECT_FALSE(std::string_view("CA") == "B");
  EXPECT_FALSE(std::string_view("B") == "AA");
  EXPECT_FALSE(std::string_view("B") == "BA");
  EXPECT_FALSE(std::string_view("B") == "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringView, OperatorNotEqual)
{
  EXPECT_FALSE(std::string_view("A") != std::string_view("A"));
  EXPECT_FALSE(std::string_view("") != std::string_view(""));
  EXPECT_TRUE(std::string_view("A") != std::string_view(""));
  EXPECT_TRUE(std::string_view("A") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("A") != std::string_view("a"));

  EXPECT_TRUE(std::string_view("A") != std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("CA"));
}

TEST(TestString_StringView, OperatorNotEqual_string_view_LHS)
{
  EXPECT_FALSE(std::string_view("A") != std::string_view("A"));
  EXPECT_FALSE(std::string_view("") != std::string_view(""));
  EXPECT_TRUE(std::string_view("A") != std::string_view(""));
  EXPECT_TRUE(std::string_view("A") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("A") != std::string_view("a"));

  EXPECT_TRUE(std::string_view("A") != std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("CA"));
}

TEST(TestString_StringView, OperatorNotEqual_string_view_RHS)
{
  EXPECT_FALSE(std::string_view("A") != std::string_view("A"));
  EXPECT_FALSE(std::string_view("") != std::string_view(""));
  EXPECT_TRUE(std::string_view("A") != std::string_view(""));
  EXPECT_TRUE(std::string_view("A") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("A") != std::string_view("a"));

  EXPECT_TRUE(std::string_view("A") != std::string_view("B"));
  EXPECT_FALSE(std::string_view("B") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("C") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("A"));
  EXPECT_FALSE(std::string_view("B") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("C"));

  EXPECT_TRUE(std::string_view("A") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("C") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("A"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("C"));

  EXPECT_TRUE(std::string_view("AA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("CA") != std::string_view("B"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("AA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string_view("CA"));
}


TEST(TestString_StringView, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE("A" != std::string_view("A"));
  EXPECT_FALSE("" != std::string_view(""));    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE("A" != std::string_view(""));
  EXPECT_TRUE("A" != std::string_view("B"));
  EXPECT_TRUE("A" != std::string_view("a"));

  EXPECT_TRUE("A" != std::string_view("B"));
  EXPECT_FALSE("B" != std::string_view("B"));
  EXPECT_TRUE("C" != std::string_view("B"));
  EXPECT_TRUE("B" != std::string_view("A"));
  EXPECT_FALSE("B" != std::string_view("B"));
  EXPECT_TRUE("B" != std::string_view("C"));

  EXPECT_TRUE("A" != std::string_view("BA"));
  EXPECT_TRUE("B" != std::string_view("BA"));
  EXPECT_TRUE("C" != std::string_view("BA"));
  EXPECT_TRUE("BA" != std::string_view("A"));
  EXPECT_TRUE("BA" != std::string_view("B"));
  EXPECT_TRUE("BA" != std::string_view("C"));

  EXPECT_TRUE("AA" != std::string_view("B"));
  EXPECT_TRUE("BA" != std::string_view("B"));
  EXPECT_TRUE("CA" != std::string_view("B"));
  EXPECT_TRUE("B" != std::string_view("AA"));
  EXPECT_TRUE("B" != std::string_view("BA"));
  EXPECT_TRUE("B" != std::string_view("CA"));
}


TEST(TestString_StringView, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(std::string_view("A") != "A");
  EXPECT_FALSE(std::string_view("") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(std::string_view("A") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(std::string_view("A") != "B");
  EXPECT_TRUE(std::string_view("A") != "a");

  EXPECT_TRUE(std::string_view("A") != "B");
  EXPECT_FALSE(std::string_view("B") != "B");
  EXPECT_TRUE(std::string_view("C") != "B");
  EXPECT_TRUE(std::string_view("B") != "A");
  EXPECT_FALSE(std::string_view("B") != "B");
  EXPECT_TRUE(std::string_view("B") != "C");

  EXPECT_TRUE(std::string_view("A") != "BA");
  EXPECT_TRUE(std::string_view("B") != "BA");
  EXPECT_TRUE(std::string_view("C") != "BA");
  EXPECT_TRUE(std::string_view("BA") != "A");
  EXPECT_TRUE(std::string_view("BA") != "B");
  EXPECT_TRUE(std::string_view("BA") != "C");

  EXPECT_TRUE(std::string_view("AA") != "B");
  EXPECT_TRUE(std::string_view("BA") != "B");
  EXPECT_TRUE(std::string_view("CA") != "B");
  EXPECT_TRUE(std::string_view("B") != "AA");
  EXPECT_TRUE(std::string_view("B") != "BA");
  EXPECT_TRUE(std::string_view("B") != "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator null handling
// ---------------------------------------------------------------------------------------------------------------------------------------------------

// TEST(TestString_StringView, OperatorLessThan_Null)
//{
//   const char* pszNull = nullptr;
//   // This is unfortunately undefined behavior instead of just a empty view!
//   std::string_view strViewNull(nullptr);
//
//   EXPECT_FALSE(std::string_view("A") < pszNull);
//   EXPECT_FALSE(std::string_view("A") < strViewNull);
//   EXPECT_TRUE(pszNull < std::string_view("A"));
//   EXPECT_TRUE(strViewNull < std::string_view("A"));
//
//   // Compare against empty string
//   EXPECT_FALSE(std::string_view("") < pszNull);
//   EXPECT_FALSE(std::string_view("") < strViewNull);
//   EXPECT_FALSE(pszNull < std::string_view(""));
//   EXPECT_FALSE(strViewNull < std::string_view(""));
//
//   // Compare against null
//   EXPECT_FALSE(pszNull < strViewNull);
//   EXPECT_FALSE(strViewNull < pszNull);
//   EXPECT_FALSE(strViewNull < strViewNull);
// }


// TEST(TestString_StringView, OperatorLessThanOrEqual_Null)
//{
//   const char* pszNull = nullptr;
//   // This is unfortunately undefined behavior instead of just a empty view!
//   std::string_view strViewNull(nullptr);
//
//   EXPECT_FALSE(std::string_view("A") <= pszNull);
//   EXPECT_FALSE(std::string_view("A") <= strViewNull);
//   EXPECT_TRUE(pszNull <= std::string_view("A"));
//   EXPECT_TRUE(strViewNull <= std::string_view("A"));
//
//   // Compare against empty string
//   EXPECT_TRUE(std::string_view("") <= pszNull);
//   EXPECT_TRUE(std::string_view("") <= strViewNull);
//   EXPECT_TRUE(pszNull <= std::string_view(""));
//   EXPECT_TRUE(strViewNull <= std::string_view(""));
//
//   // Compare against null
//   EXPECT_TRUE(pszNull <= strViewNull);
//   EXPECT_TRUE(strViewNull <= pszNull);
//   EXPECT_TRUE(strViewNull <= strViewNull);
// }


// TEST(TestString_StringView, OperatorGreaterThan_Null)
//{
//   const char* pszNull = nullptr;
//   // This is unfortunately undefined behavior instead of just a empty view!
//   std::string_view StrViewNull(nullptr);
//
//   EXPECT_TRUE(std::string_view("A") > pszNull);
//   EXPECT_TRUE(std::string_view("A") > StrViewNull);
//   EXPECT_FALSE(pszNull > std::string_view("A"));
//   EXPECT_FALSE(StrViewNull > std::string_view("A"));
//
//   // Compare against empty string
//   EXPECT_FALSE(std::string_view("") > pszNull);
//   EXPECT_FALSE(std::string_view("") > StrViewNull);
//   EXPECT_FALSE(pszNull > std::string_view(""));
//   EXPECT_FALSE(StrViewNull > std::string_view(""));
//
//   // Compare against null
//   EXPECT_FALSE(pszNull > StrViewNull);
//   EXPECT_FALSE(StrViewNull > pszNull);
//   EXPECT_FALSE(StrViewNull > StrViewNull);
// }


// TEST(TestString_StringView, OperatorGreaterThanOrEqual_Null)
//{
//   const char* pszNull = nullptr;
//   // This is unfortunately undefined behavior instead of just a empty view!
//   std::string_view StrViewNull(nullptr);
//
//   EXPECT_TRUE(std::string_view("A") >= pszNull);
//   EXPECT_TRUE(std::string_view("A") >= StrViewNull);
//   EXPECT_FALSE(pszNull >= std::string_view("A"));
//   EXPECT_FALSE(StrViewNull >= std::string_view("A"));
//
//   // Compare against empty string
//   EXPECT_TRUE(std::string_view("") >= pszNull);
//   EXPECT_TRUE(std::string_view("") >= StrViewNull);
//   EXPECT_TRUE(pszNull >= std::string_view(""));
//   EXPECT_TRUE(StrViewNull >= std::string_view(""));
//
//   // Compare against null
//   EXPECT_TRUE(pszNull >= StrViewNull);
//   EXPECT_TRUE(StrViewNull >= pszNull);
//   EXPECT_TRUE(StrViewNull >= StrViewNull);
// }


// TEST(TestString_StringView, OperatorEqual_Null)
//{
//   const char* pszNull = nullptr;
//   // This is unfortunately undefined behavior instead of just a empty view!
//   std::string_view StrViewNull(nullptr);
//
//   EXPECT_FALSE(std::string_view("A") == pszNull);
//   EXPECT_FALSE(std::string_view("A") == StrViewNull);
//   EXPECT_FALSE(pszNull == std::string_view("A"));
//   EXPECT_FALSE(StrViewNull == std::string_view("A"));
//
//   // Compare against empty string
//   EXPECT_TRUE(pszNull == std::string_view(""));
//   EXPECT_TRUE(StrViewNull == std::string_view(""));
//   EXPECT_TRUE(std::string_view("") == pszNull);
//   EXPECT_TRUE(std::string_view("") == StrViewNull);
//
//   // Compare against null
//   EXPECT_TRUE(pszNull == StrViewNull);
//   EXPECT_TRUE(StrViewNull == pszNull);
//   EXPECT_TRUE(StrViewNull == StrViewNull);
// }


// TEST(TestString_StringView, OperatorNotEqual_Null)
//{
//   const char* pszNull = nullptr;
//   // This is unfortunately undefined behavior instead of just a empty view!
//   std::string_view StrViewNull(nullptr);
//
//   EXPECT_TRUE(std::string_view("A") != pszNull);
//   EXPECT_TRUE(std::string_view("A") != StrViewNull);
//   EXPECT_TRUE(pszNull != std::string_view("A"));
//   EXPECT_TRUE(StrViewNull != std::string_view("A"));
//
//   // Compare against empty string
//   EXPECT_FALSE(pszNull != std::string_view(""));
//   EXPECT_FALSE(StrViewNull != std::string_view(""));
//   EXPECT_FALSE(std::string_view("") != pszNull);
//   EXPECT_FALSE(std::string_view("") != StrViewNull);
//
//   // Compare against null
//   EXPECT_FALSE(pszNull != StrViewNull);
//   EXPECT_FALSE(StrViewNull != pszNull);
//   EXPECT_FALSE(StrViewNull != StrViewNull);
// }
