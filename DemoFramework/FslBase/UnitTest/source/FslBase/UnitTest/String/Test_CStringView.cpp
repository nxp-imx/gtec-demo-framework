/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Log/String/LogCStringView.hpp>
#include <FslBase/String/CStringView.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestString_CStringView = TestFixtureFslBase;

  CStringView Convert(const std::string& str)
  {
    return CStringView(str.data(), str.size());
  }
}


TEST(TestString_CStringView, Construct)
{
  CStringView strView;

  EXPECT_TRUE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.data(), CStringView().data());
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(TestString_CStringView, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  CStringView strView(psz, lenPsz);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), lenPsz);
  EXPECT_EQ(strView.length(), lenPsz);
}


TEST(TestString_CStringView, Construct_FromStr)
{
  std::string str("Hello world");
  CStringView strView = Convert(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}

TEST(TestString_CStringView, front)
{
  std::string str("Hello world");
  CStringView strView = Convert(str);

  EXPECT_EQ(strView.front(), str.front());
}


TEST(TestString_CStringView, back)
{
  std::string str("Hello world");
  CStringView strView = Convert(str);

  EXPECT_EQ(strView.back(), str.back());
}


TEST(TestString_CStringView, opIndex)
{
  std::string str("Hello world");
  CStringView strView = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(strView[i], str[i]);
  }
}


TEST(TestString_CStringView, At)
{
  std::string str("Hello world");
  CStringView strView = Convert(str);

  for (std::size_t i = 0; i < str.size(); ++i)
  {
    EXPECT_EQ(strView.at(i), str.at(i));
  }
}


TEST(TestString_CStringView, At_OutOfBounds)
{
  std::string str("Hello world");
  CStringView strView = Convert(str);

  EXPECT_THROW(strView.at(str.size()), std::out_of_range);
}


TEST(TestString_CStringView, At_Empty_OutOfBounds)
{
  std::string str;
  CStringView strView = Convert(str);

  EXPECT_THROW(strView.at(str.size()), std::out_of_range);
}


TEST(TestString_CStringView, opEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_CStringView, opEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_CStringView, opEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringView, opEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringView, opEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringView, opEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringView, opNotEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_CStringView, opNotEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_CStringView, opNotEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringView, opNotEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringView, opNotEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringView, opNotEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringView, opLess_Empty)
{
  std::string str1;
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_CStringView, opLess1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_CStringView, opLess2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_CStringView, opLess3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_CStringView, opLess4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_CStringView, opLess5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}

TEST(TestString_CStringView, opLessOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringView, opLessOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringView, opLessOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}


TEST(TestString_CStringView, opLessOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringView, opLessOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringView, opLessOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}

TEST(TestString_CStringView, opGreater_Empty)
{
  std::string str1;
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringView, opGreater1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringView, opGreater2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}


TEST(TestString_CStringView, opGreater3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringView, opGreater4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringView, opGreater5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}

TEST(TestString_CStringView, opGreaterOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_CStringView, opGreaterOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_CStringView, opGreaterOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_CStringView, opGreaterOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_CStringView, opGreaterOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_CStringView, opGreaterOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringView strView1 = Convert(str1);
  CStringView strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// compare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, compare)
{
  EXPECT_TRUE(CStringView("A").compare(CStringView("B")) < 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("B")) == 0);
  EXPECT_TRUE(CStringView("C").compare(CStringView("B")) > 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("A")) > 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("C")) < 0);

  EXPECT_TRUE(CStringView("A").compare(CStringView("BA")) < 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("BA")) < 0);
  EXPECT_TRUE(CStringView("C").compare(CStringView("BA")) > 0);
  EXPECT_TRUE(CStringView("BA").compare(CStringView("A")) > 0);
  EXPECT_TRUE(CStringView("BA").compare(CStringView("B")) > 0);
  EXPECT_TRUE(CStringView("BA").compare(CStringView("C")) < 0);

  EXPECT_TRUE(CStringView("AA").compare(CStringView("B")) < 0);
  EXPECT_TRUE(CStringView("BA").compare(CStringView("B")) > 0);
  EXPECT_TRUE(CStringView("CA").compare(CStringView("B")) > 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("AA")) > 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("BA")) < 0);
  EXPECT_TRUE(CStringView("B").compare(CStringView("CA")) < 0);
}

TEST(TestString_CStringView, compare_StringViewLite)
{
  EXPECT_TRUE(CStringView("A").compare(StringViewLite("B")) < 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("B")) == 0);
  EXPECT_TRUE(CStringView("C").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("A")) > 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("C")) < 0);

  EXPECT_TRUE(CStringView("A").compare(StringViewLite("BA")) < 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("BA")) < 0);
  EXPECT_TRUE(CStringView("C").compare(StringViewLite("BA")) > 0);
  EXPECT_TRUE(CStringView("BA").compare(StringViewLite("A")) > 0);
  EXPECT_TRUE(CStringView("BA").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(CStringView("BA").compare(StringViewLite("C")) < 0);

  EXPECT_TRUE(CStringView("AA").compare(StringViewLite("B")) < 0);
  EXPECT_TRUE(CStringView("BA").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(CStringView("CA").compare(StringViewLite("B")) > 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("AA")) > 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("BA")) < 0);
  EXPECT_TRUE(CStringView("B").compare(StringViewLite("CA")) < 0);
}

TEST(TestString_CStringView, compare_CString)
{
  EXPECT_TRUE(CStringView("A").compare("B") < 0);
  EXPECT_TRUE(CStringView("B").compare("B") == 0);
  EXPECT_TRUE(CStringView("C").compare("B") > 0);
  EXPECT_TRUE(CStringView("B").compare("A") > 0);
  EXPECT_TRUE(CStringView("B").compare("C") < 0);

  EXPECT_TRUE(CStringView("A").compare("BA") < 0);
  EXPECT_TRUE(CStringView("B").compare("BA") < 0);
  EXPECT_TRUE(CStringView("C").compare("BA") > 0);
  EXPECT_TRUE(CStringView("BA").compare("A") > 0);
  EXPECT_TRUE(CStringView("BA").compare("B") > 0);
  EXPECT_TRUE(CStringView("BA").compare("C") < 0);

  EXPECT_TRUE(CStringView("AA").compare("B") < 0);
  EXPECT_TRUE(CStringView("BA").compare("B") > 0);
  EXPECT_TRUE(CStringView("CA").compare("B") > 0);
  EXPECT_TRUE(CStringView("B").compare("AA") > 0);
  EXPECT_TRUE(CStringView("B").compare("BA") < 0);
  EXPECT_TRUE(CStringView("B").compare("CA") < 0);
}


TEST(TestString_CStringView, compare_Null)
{
  EXPECT_TRUE(CStringView().compare(CStringView()) == 0);
  EXPECT_TRUE(CStringView("").compare(CStringView()) == 0);
  EXPECT_TRUE(CStringView().compare(CStringView("")) == 0);
}

TEST(TestString_CStringView, compare_Null_CString)
{
  EXPECT_TRUE(CStringView().compare(nullptr) == 0);
  EXPECT_TRUE(CStringView("").compare(nullptr) == 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// starts_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, starts_with)
{
  EXPECT_TRUE(CStringView().starts_with(CStringView()));
  EXPECT_TRUE(CStringView().starts_with(CStringView("")));

  EXPECT_TRUE(CStringView("").starts_with(CStringView()));
  EXPECT_TRUE(CStringView("").starts_with(CStringView("")));

  EXPECT_TRUE(CStringView("A").starts_with(CStringView()));
  EXPECT_TRUE(CStringView("A").starts_with(CStringView("")));
  EXPECT_TRUE(CStringView("A").starts_with(CStringView("A")));

  EXPECT_TRUE(CStringView("AB").starts_with(CStringView()));
  EXPECT_TRUE(CStringView("AB").starts_with(CStringView("")));
  EXPECT_TRUE(CStringView("AB").starts_with(CStringView("A")));
  EXPECT_TRUE(CStringView("AB").starts_with(CStringView("AB")));

  EXPECT_TRUE(CStringView("ABC").starts_with(CStringView()));
  EXPECT_TRUE(CStringView("ABC").starts_with(CStringView("")));
  EXPECT_TRUE(CStringView("ABC").starts_with(CStringView("A")));
  EXPECT_TRUE(CStringView("ABC").starts_with(CStringView("AB")));
  EXPECT_TRUE(CStringView("ABC").starts_with(CStringView("ABC")));

  EXPECT_FALSE(CStringView().starts_with(CStringView("A")));

  EXPECT_FALSE(CStringView("").starts_with(CStringView("A")));

  EXPECT_FALSE(CStringView("A").starts_with(CStringView("B")));
  EXPECT_FALSE(CStringView("A").starts_with(CStringView("AB")));

  EXPECT_FALSE(CStringView("AB").starts_with(CStringView("B")));
  EXPECT_FALSE(CStringView("AB").starts_with(CStringView("AC")));
  EXPECT_FALSE(CStringView("AB").starts_with(CStringView("ABC")));

  EXPECT_FALSE(CStringView("ABC").starts_with(CStringView("B")));
  EXPECT_FALSE(CStringView("ABC").starts_with(CStringView("AC")));
  EXPECT_FALSE(CStringView("ABC").starts_with(CStringView("ABD")));
  EXPECT_FALSE(CStringView("ABC").starts_with(CStringView("ABDA")));
}

TEST(TestString_CStringView, starts_with_StringViewLite)
{
  EXPECT_TRUE(CStringView().starts_with(StringViewLite()));
  EXPECT_TRUE(CStringView().starts_with(StringViewLite("")));

  EXPECT_TRUE(CStringView("").starts_with(StringViewLite()));
  EXPECT_TRUE(CStringView("").starts_with(StringViewLite("")));

  EXPECT_TRUE(CStringView("A").starts_with(StringViewLite()));
  EXPECT_TRUE(CStringView("A").starts_with(StringViewLite("")));
  EXPECT_TRUE(CStringView("A").starts_with(StringViewLite("A")));

  EXPECT_TRUE(CStringView("AB").starts_with(StringViewLite()));
  EXPECT_TRUE(CStringView("AB").starts_with(StringViewLite("")));
  EXPECT_TRUE(CStringView("AB").starts_with(StringViewLite("A")));
  EXPECT_TRUE(CStringView("AB").starts_with(StringViewLite("AB")));

  EXPECT_TRUE(CStringView("ABC").starts_with(StringViewLite()));
  EXPECT_TRUE(CStringView("ABC").starts_with(StringViewLite("")));
  EXPECT_TRUE(CStringView("ABC").starts_with(StringViewLite("A")));
  EXPECT_TRUE(CStringView("ABC").starts_with(StringViewLite("AB")));
  EXPECT_TRUE(CStringView("ABC").starts_with(StringViewLite("ABC")));

  EXPECT_FALSE(CStringView().starts_with(StringViewLite("A")));

  EXPECT_FALSE(CStringView("").starts_with(StringViewLite("A")));

  EXPECT_FALSE(CStringView("A").starts_with(StringViewLite("B")));
  EXPECT_FALSE(CStringView("A").starts_with(StringViewLite("AB")));

  EXPECT_FALSE(CStringView("AB").starts_with(StringViewLite("B")));
  EXPECT_FALSE(CStringView("AB").starts_with(StringViewLite("AC")));
  EXPECT_FALSE(CStringView("AB").starts_with(StringViewLite("ABC")));

  EXPECT_FALSE(CStringView("ABC").starts_with(StringViewLite("B")));
  EXPECT_FALSE(CStringView("ABC").starts_with(StringViewLite("AC")));
  EXPECT_FALSE(CStringView("ABC").starts_with(StringViewLite("ABD")));
  EXPECT_FALSE(CStringView("ABC").starts_with(StringViewLite("ABDA")));
}

TEST(TestString_CStringView, starts_with_CString)
{
  EXPECT_TRUE(CStringView().starts_with(nullptr));
  EXPECT_TRUE(CStringView().starts_with(""));

  EXPECT_TRUE(CStringView("").starts_with(nullptr));
  EXPECT_TRUE(CStringView("").starts_with(""));

  EXPECT_TRUE(CStringView("A").starts_with(nullptr));
  EXPECT_TRUE(CStringView("A").starts_with(""));
  EXPECT_TRUE(CStringView("A").starts_with("A"));

  EXPECT_TRUE(CStringView("AB").starts_with(nullptr));
  EXPECT_TRUE(CStringView("AB").starts_with(""));
  EXPECT_TRUE(CStringView("AB").starts_with("A"));
  EXPECT_TRUE(CStringView("AB").starts_with("AB"));

  EXPECT_TRUE(CStringView("ABC").starts_with(nullptr));
  EXPECT_TRUE(CStringView("ABC").starts_with(""));
  EXPECT_TRUE(CStringView("ABC").starts_with("A"));
  EXPECT_TRUE(CStringView("ABC").starts_with("AB"));
  EXPECT_TRUE(CStringView("ABC").starts_with("ABC"));

  EXPECT_FALSE(CStringView().starts_with("A"));

  EXPECT_FALSE(CStringView("").starts_with("A"));

  EXPECT_FALSE(CStringView("A").starts_with("B"));
  EXPECT_FALSE(CStringView("A").starts_with("AB"));

  EXPECT_FALSE(CStringView("AB").starts_with("B"));
  EXPECT_FALSE(CStringView("AB").starts_with("AC"));
  EXPECT_FALSE(CStringView("AB").starts_with("ABC"));

  EXPECT_FALSE(CStringView("ABC").starts_with("B"));
  EXPECT_FALSE(CStringView("ABC").starts_with("AC"));
  EXPECT_FALSE(CStringView("ABC").starts_with("ABD"));
  EXPECT_FALSE(CStringView("ABC").starts_with("ABDA"));
}

TEST(TestString_CStringView, starts_with_char)
{
  EXPECT_TRUE(CStringView("A").starts_with('A'));
  EXPECT_TRUE(CStringView("B").starts_with('B'));

  EXPECT_TRUE(CStringView("AB").starts_with('A'));
  EXPECT_TRUE(CStringView("BC").starts_with('B'));

  EXPECT_FALSE(CStringView().starts_with(' '));
  EXPECT_FALSE(CStringView().starts_with('a'));

  EXPECT_FALSE(CStringView("").starts_with(' '));
  EXPECT_FALSE(CStringView("").starts_with('a'));

  EXPECT_FALSE(CStringView("A").starts_with(' '));
  EXPECT_FALSE(CStringView("A").starts_with('a'));

  EXPECT_FALSE(CStringView("AB").starts_with(' '));
  EXPECT_FALSE(CStringView("AB").starts_with('a'));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ends_with
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, ends_with)
{
  EXPECT_TRUE(CStringView().ends_with(CStringView()));
  EXPECT_TRUE(CStringView().ends_with(CStringView("")));

  EXPECT_TRUE(CStringView("").ends_with(CStringView()));
  EXPECT_TRUE(CStringView("").ends_with(CStringView("")));

  EXPECT_TRUE(CStringView("A").ends_with(CStringView()));
  EXPECT_TRUE(CStringView("A").ends_with(CStringView("")));
  EXPECT_TRUE(CStringView("A").ends_with(CStringView("A")));

  EXPECT_TRUE(CStringView("BA").ends_with(CStringView()));
  EXPECT_TRUE(CStringView("BA").ends_with(CStringView("")));
  EXPECT_TRUE(CStringView("BA").ends_with(CStringView("A")));
  EXPECT_TRUE(CStringView("BA").ends_with(CStringView("BA")));

  EXPECT_TRUE(CStringView("CBA").ends_with(CStringView()));
  EXPECT_TRUE(CStringView("CBA").ends_with(CStringView("")));
  EXPECT_TRUE(CStringView("CBA").ends_with(CStringView("A")));
  EXPECT_TRUE(CStringView("CBA").ends_with(CStringView("BA")));
  EXPECT_TRUE(CStringView("CBA").ends_with(CStringView("CBA")));

  EXPECT_FALSE(CStringView().ends_with(CStringView("A")));

  EXPECT_FALSE(CStringView("").ends_with(CStringView("A")));

  EXPECT_FALSE(CStringView("A").ends_with(CStringView("B")));
  EXPECT_FALSE(CStringView("A").ends_with(CStringView("BA")));

  EXPECT_FALSE(CStringView("BA").ends_with(CStringView("B")));
  EXPECT_FALSE(CStringView("BA").ends_with(CStringView("CA")));
  EXPECT_FALSE(CStringView("BA").ends_with(CStringView("CBA")));

  EXPECT_FALSE(CStringView("CBA").ends_with(CStringView("B")));
  EXPECT_FALSE(CStringView("CBA").ends_with(CStringView("CA")));
  EXPECT_FALSE(CStringView("CBA").ends_with(CStringView("DBA")));
  EXPECT_FALSE(CStringView("CBA").ends_with(CStringView("ADBA")));
}

TEST(TestString_CStringView, ends_with_StringViewLite)
{
  EXPECT_TRUE(CStringView().ends_with(StringViewLite()));
  EXPECT_TRUE(CStringView().ends_with(StringViewLite("")));

  EXPECT_TRUE(CStringView("").ends_with(StringViewLite()));
  EXPECT_TRUE(CStringView("").ends_with(StringViewLite("")));

  EXPECT_TRUE(CStringView("A").ends_with(StringViewLite()));
  EXPECT_TRUE(CStringView("A").ends_with(StringViewLite("")));
  EXPECT_TRUE(CStringView("A").ends_with(StringViewLite("A")));

  EXPECT_TRUE(CStringView("BA").ends_with(StringViewLite()));
  EXPECT_TRUE(CStringView("BA").ends_with(StringViewLite("")));
  EXPECT_TRUE(CStringView("BA").ends_with(StringViewLite("A")));
  EXPECT_TRUE(CStringView("BA").ends_with(StringViewLite("BA")));

  EXPECT_TRUE(CStringView("CBA").ends_with(StringViewLite()));
  EXPECT_TRUE(CStringView("CBA").ends_with(StringViewLite("")));
  EXPECT_TRUE(CStringView("CBA").ends_with(StringViewLite("A")));
  EXPECT_TRUE(CStringView("CBA").ends_with(StringViewLite("BA")));
  EXPECT_TRUE(CStringView("CBA").ends_with(StringViewLite("CBA")));

  EXPECT_FALSE(CStringView().ends_with(StringViewLite("A")));

  EXPECT_FALSE(CStringView("").ends_with(StringViewLite("A")));

  EXPECT_FALSE(CStringView("A").ends_with(StringViewLite("B")));
  EXPECT_FALSE(CStringView("A").ends_with(StringViewLite("BA")));

  EXPECT_FALSE(CStringView("BA").ends_with(StringViewLite("B")));
  EXPECT_FALSE(CStringView("BA").ends_with(StringViewLite("CA")));
  EXPECT_FALSE(CStringView("BA").ends_with(StringViewLite("CBA")));

  EXPECT_FALSE(CStringView("CBA").ends_with(StringViewLite("B")));
  EXPECT_FALSE(CStringView("CBA").ends_with(StringViewLite("CA")));
  EXPECT_FALSE(CStringView("CBA").ends_with(StringViewLite("DBA")));
  EXPECT_FALSE(CStringView("CBA").ends_with(StringViewLite("ADBA")));
}

TEST(TestString_CStringView, ends_with_CString)
{
  EXPECT_TRUE(CStringView().ends_with(nullptr));
  EXPECT_TRUE(CStringView().ends_with(""));

  EXPECT_TRUE(CStringView("").ends_with(nullptr));
  EXPECT_TRUE(CStringView("").ends_with(""));

  EXPECT_TRUE(CStringView("A").ends_with(nullptr));
  EXPECT_TRUE(CStringView("A").ends_with(""));
  EXPECT_TRUE(CStringView("A").ends_with("A"));

  EXPECT_TRUE(CStringView("BA").ends_with(nullptr));
  EXPECT_TRUE(CStringView("BA").ends_with(""));
  EXPECT_TRUE(CStringView("BA").ends_with("A"));
  EXPECT_TRUE(CStringView("BA").ends_with("BA"));

  EXPECT_TRUE(CStringView("CBA").ends_with(nullptr));
  EXPECT_TRUE(CStringView("CBA").ends_with(""));
  EXPECT_TRUE(CStringView("CBA").ends_with("A"));
  EXPECT_TRUE(CStringView("CBA").ends_with("BA"));
  EXPECT_TRUE(CStringView("CBA").ends_with("CBA"));

  EXPECT_FALSE(CStringView().ends_with("A"));

  EXPECT_FALSE(CStringView("").ends_with("A"));

  EXPECT_FALSE(CStringView("A").ends_with("B"));
  EXPECT_FALSE(CStringView("A").ends_with("BA"));

  EXPECT_FALSE(CStringView("BA").ends_with("B"));
  EXPECT_FALSE(CStringView("BA").ends_with("CA"));
  EXPECT_FALSE(CStringView("BA").ends_with("CBA"));

  EXPECT_FALSE(CStringView("CBA").ends_with("B"));
  EXPECT_FALSE(CStringView("CBA").ends_with("CA"));
  EXPECT_FALSE(CStringView("CBA").ends_with("DBA"));
  EXPECT_FALSE(CStringView("CBA").ends_with("ADBA"));
}


TEST(TestString_CStringView, ends_with_char)
{
  EXPECT_TRUE(CStringView("A").ends_with('A'));
  EXPECT_TRUE(CStringView("B").ends_with('B'));

  EXPECT_TRUE(CStringView("BA").ends_with('A'));
  EXPECT_TRUE(CStringView("CB").ends_with('B'));

  EXPECT_FALSE(CStringView().ends_with(' '));
  EXPECT_FALSE(CStringView().ends_with('a'));

  EXPECT_FALSE(CStringView("").ends_with(' '));
  EXPECT_FALSE(CStringView("").ends_with('a'));

  EXPECT_FALSE(CStringView("A").ends_with(' '));
  EXPECT_FALSE(CStringView("A").ends_with('a'));

  EXPECT_FALSE(CStringView("BA").ends_with(' '));
  EXPECT_FALSE(CStringView("BA").ends_with('a'));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// find
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, find_char)
{
  EXPECT_EQ(0u, CStringView("a").find('a'));
  EXPECT_EQ(CStringView::npos, CStringView("a").find('a', 1u));

  EXPECT_EQ(0u, CStringView("aa").find('a'));
  EXPECT_EQ(1u, CStringView("aa").find('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("aa").find('a', 2u));

  EXPECT_EQ(0u, CStringView("aba").find('a'));
  EXPECT_EQ(2u, CStringView("aba").find('a', 1u));
  EXPECT_EQ(2u, CStringView("aba").find('a', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("aba").find('a', 3u));

  EXPECT_EQ(2u, CStringView("cba").find('a'));
  EXPECT_EQ(2u, CStringView("cba").find('a', 1u));
  EXPECT_EQ(2u, CStringView("cba").find('a', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("cba").find('a', 3u));


  EXPECT_EQ(CStringView::npos, CStringView().find(' '));
  EXPECT_EQ(CStringView::npos, CStringView().find('a'));
  EXPECT_EQ(CStringView::npos, CStringView().find(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView().find('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView().find(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView().find('a', 2u));

  EXPECT_EQ(CStringView::npos, CStringView("").find(' '));
  EXPECT_EQ(CStringView::npos, CStringView("").find('a'));
  EXPECT_EQ(CStringView::npos, CStringView("").find(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("").find('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("").find(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("").find('a', 2u));

  EXPECT_EQ(CStringView::npos, CStringView("a").find(' '));
  EXPECT_EQ(CStringView::npos, CStringView("b").find('a'));
  EXPECT_EQ(CStringView::npos, CStringView("a").find(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("b").find('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("a").find(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("b").find('a', 2u));

  EXPECT_EQ(CStringView::npos, CStringView("ab").find(' '));
  EXPECT_EQ(CStringView::npos, CStringView("bc").find('a'));
  EXPECT_EQ(CStringView::npos, CStringView("ab").find(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("bc").find('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("ab").find(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("bc").find('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// rfind
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, rfind_char)
{
  EXPECT_EQ(0u, CStringView("a").rfind('a'));
  EXPECT_EQ(0u, CStringView("a").rfind('a', 0u));
  EXPECT_EQ(0u, CStringView("a").rfind('a', 1u));

  EXPECT_EQ(1u, CStringView("aa").rfind('a'));
  EXPECT_EQ(0u, CStringView("aa").rfind('a', 0u));
  EXPECT_EQ(1u, CStringView("aa").rfind('a', 1u));
  EXPECT_EQ(1u, CStringView("aa").rfind('a', 2u));

  EXPECT_EQ(2u, CStringView("aba").rfind('a'));
  EXPECT_EQ(0u, CStringView("aba").rfind('a', 0u));
  EXPECT_EQ(0u, CStringView("aba").rfind('a', 1u));
  EXPECT_EQ(2u, CStringView("aba").rfind('a', 2u));
  EXPECT_EQ(2u, CStringView("aba").rfind('a', 3u));

  EXPECT_EQ(2u, CStringView("cba").rfind('a'));
  EXPECT_EQ(CStringView::npos, CStringView("cba").rfind('a', 0u));
  EXPECT_EQ(CStringView::npos, CStringView("cba").rfind('a', 1u));
  EXPECT_EQ(2u, CStringView("cba").rfind('a', 2u));
  EXPECT_EQ(2u, CStringView("cba").rfind('a', 3u));

  EXPECT_EQ(CStringView::npos, CStringView().rfind(' '));
  EXPECT_EQ(CStringView::npos, CStringView().rfind('a'));
  EXPECT_EQ(CStringView::npos, CStringView().rfind(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView().rfind('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView().rfind(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView().rfind('a', 2u));

  EXPECT_EQ(CStringView::npos, CStringView("").rfind(' '));
  EXPECT_EQ(CStringView::npos, CStringView("").rfind('a'));
  EXPECT_EQ(CStringView::npos, CStringView("").rfind(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("").rfind('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("").rfind(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("").rfind('a', 2u));

  EXPECT_EQ(CStringView::npos, CStringView("a").rfind(' '));
  EXPECT_EQ(CStringView::npos, CStringView("b").rfind('a'));
  EXPECT_EQ(CStringView::npos, CStringView("a").rfind(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("b").rfind('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("a").rfind(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("b").rfind('a', 2u));

  EXPECT_EQ(CStringView::npos, CStringView("ab").rfind(' '));
  EXPECT_EQ(CStringView::npos, CStringView("bc").rfind('a'));
  EXPECT_EQ(CStringView::npos, CStringView("ab").rfind(' ', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("bc").rfind('a', 1u));
  EXPECT_EQ(CStringView::npos, CStringView("ab").rfind(' ', 2u));
  EXPECT_EQ(CStringView::npos, CStringView("bc").rfind('a', 2u));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, OperatorLessThan)
{
  EXPECT_LT(CStringView("A"), CStringView("B"));
  EXPECT_LT(CStringView("A"), CStringView("AB"));
  EXPECT_LT(CStringView(""), CStringView("AB"));

  EXPECT_TRUE(CStringView("A") < CStringView("B"));
  EXPECT_FALSE(CStringView("B") < CStringView("B"));
  EXPECT_FALSE(CStringView("C") < CStringView("B"));
  EXPECT_FALSE(CStringView("B") < CStringView("A"));
  EXPECT_FALSE(CStringView("B") < CStringView("B"));
  EXPECT_TRUE(CStringView("B") < CStringView("C"));

  EXPECT_TRUE(CStringView("A") < CStringView("BA"));
  EXPECT_TRUE(CStringView("B") < CStringView("BA"));
  EXPECT_FALSE(CStringView("C") < CStringView("BA"));
  EXPECT_FALSE(CStringView("BA") < CStringView("A"));
  EXPECT_FALSE(CStringView("BA") < CStringView("B"));
  EXPECT_TRUE(CStringView("BA") < CStringView("C"));

  EXPECT_TRUE(CStringView("AA") < CStringView("B"));
  EXPECT_FALSE(CStringView("BA") < CStringView("B"));
  EXPECT_FALSE(CStringView("CA") < CStringView("B"));
  EXPECT_FALSE(CStringView("B") < CStringView("AA"));
  EXPECT_TRUE(CStringView("B") < CStringView("BA"));
  EXPECT_TRUE(CStringView("B") < CStringView("CA"));
}

TEST(TestString_CStringView, OperatorLessThan_StringViewLite_LHS)
{
  EXPECT_LT(StringViewLite("A"), CStringView("B"));
  EXPECT_LT(StringViewLite("A"), CStringView("AB"));
  EXPECT_LT(StringViewLite(""), CStringView("AB"));

  EXPECT_TRUE(StringViewLite("A") < CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") < CStringView("B"));
  EXPECT_FALSE(StringViewLite("C") < CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") < CStringView("A"));
  EXPECT_FALSE(StringViewLite("B") < CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") < CStringView("C"));

  EXPECT_TRUE(StringViewLite("A") < CStringView("BA"));
  EXPECT_TRUE(StringViewLite("B") < CStringView("BA"));
  EXPECT_FALSE(StringViewLite("C") < CStringView("BA"));
  EXPECT_FALSE(StringViewLite("BA") < CStringView("A"));
  EXPECT_FALSE(StringViewLite("BA") < CStringView("B"));
  EXPECT_TRUE(StringViewLite("BA") < CStringView("C"));

  EXPECT_TRUE(StringViewLite("AA") < CStringView("B"));
  EXPECT_FALSE(StringViewLite("BA") < CStringView("B"));
  EXPECT_FALSE(StringViewLite("CA") < CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") < CStringView("AA"));
  EXPECT_TRUE(StringViewLite("B") < CStringView("BA"));
  EXPECT_TRUE(StringViewLite("B") < CStringView("CA"));
}


TEST(TestString_CStringView, OperatorLessThan_StringViewLite_RHS)
{
  EXPECT_LT(CStringView("A"), StringViewLite("B"));
  EXPECT_LT(CStringView("A"), StringViewLite("AB"));
  EXPECT_LT(CStringView(""), StringViewLite("AB"));

  EXPECT_TRUE(CStringView("A") < StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") < StringViewLite("B"));
  EXPECT_FALSE(CStringView("C") < StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") < StringViewLite("A"));
  EXPECT_FALSE(CStringView("B") < StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") < StringViewLite("C"));

  EXPECT_TRUE(CStringView("A") < StringViewLite("BA"));
  EXPECT_TRUE(CStringView("B") < StringViewLite("BA"));
  EXPECT_FALSE(CStringView("C") < StringViewLite("BA"));
  EXPECT_FALSE(CStringView("BA") < StringViewLite("A"));
  EXPECT_FALSE(CStringView("BA") < StringViewLite("B"));
  EXPECT_TRUE(CStringView("BA") < StringViewLite("C"));

  EXPECT_TRUE(CStringView("AA") < StringViewLite("B"));
  EXPECT_FALSE(CStringView("BA") < StringViewLite("B"));
  EXPECT_FALSE(CStringView("CA") < StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") < StringViewLite("AA"));
  EXPECT_TRUE(CStringView("B") < StringViewLite("BA"));
  EXPECT_TRUE(CStringView("B") < StringViewLite("CA"));
}


TEST(TestString_CStringView, OperatorLessThan_CString_LHS)
{
  EXPECT_LT("A", CStringView("B"));
  EXPECT_LT("A", CStringView("AB"));
  EXPECT_LT("", CStringView("AB"));

  EXPECT_TRUE("A" < CStringView("B"));
  EXPECT_FALSE("B" < CStringView("B"));
  EXPECT_FALSE("C" < CStringView("B"));
  EXPECT_FALSE("B" < CStringView("A"));
  EXPECT_FALSE("B" < CStringView("B"));
  EXPECT_TRUE("B" < CStringView("C"));

  EXPECT_TRUE("A" < CStringView("BA"));
  EXPECT_TRUE("B" < CStringView("BA"));
  EXPECT_FALSE("C" < CStringView("BA"));
  EXPECT_FALSE("BA" < CStringView("A"));
  EXPECT_FALSE("BA" < CStringView("B"));
  EXPECT_TRUE("BA" < CStringView("C"));

  EXPECT_TRUE("AA" < CStringView("B"));
  EXPECT_FALSE("BA" < CStringView("B"));
  EXPECT_FALSE("CA" < CStringView("B"));
  EXPECT_FALSE("B" < CStringView("AA"));
  EXPECT_TRUE("B" < CStringView("BA"));
  EXPECT_TRUE("B" < CStringView("CA"));
}

TEST(TestString_CStringView, OperatorLessThan_CString_RHS)
{
  EXPECT_LT(CStringView("A"), "B");
  EXPECT_LT(CStringView("A"), "AB");
  EXPECT_LT(CStringView(""), "AB");

  EXPECT_TRUE(CStringView("A") < "B");
  EXPECT_FALSE(CStringView("B") < "B");
  EXPECT_FALSE(CStringView("C") < "B");
  EXPECT_FALSE(CStringView("B") < "A");
  EXPECT_FALSE(CStringView("B") < "B");
  EXPECT_TRUE(CStringView("B") < "C");

  EXPECT_TRUE(CStringView("A") < "BA");
  EXPECT_TRUE(CStringView("B") < "BA");
  EXPECT_FALSE(CStringView("C") < "BA");
  EXPECT_FALSE(CStringView("BA") < "A");
  EXPECT_FALSE(CStringView("BA") < "B");
  EXPECT_TRUE(CStringView("BA") < "C");

  EXPECT_TRUE(CStringView("AA") < "B");
  EXPECT_FALSE(CStringView("BA") < "B");
  EXPECT_FALSE(CStringView("CA") < "B");
  EXPECT_FALSE(CStringView("B") < "AA");
  EXPECT_TRUE(CStringView("B") < "BA");
  EXPECT_TRUE(CStringView("B") < "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, OperatorLessThanOrEqual)
{
  EXPECT_LE(CStringView("A"), CStringView("B"));
  EXPECT_LE(CStringView("A"), CStringView("AB"));
  EXPECT_LE(CStringView(""), CStringView("AB"));
  EXPECT_LE(CStringView("A"), CStringView("A"));

  EXPECT_TRUE(CStringView("A") <= CStringView("B"));
  EXPECT_TRUE(CStringView("B") <= CStringView("B"));
  EXPECT_FALSE(CStringView("C") <= CStringView("B"));
  EXPECT_FALSE(CStringView("B") <= CStringView("A"));
  EXPECT_TRUE(CStringView("B") <= CStringView("B"));
  EXPECT_TRUE(CStringView("B") <= CStringView("C"));

  EXPECT_TRUE(CStringView("A") <= CStringView("BA"));
  EXPECT_TRUE(CStringView("B") <= CStringView("BA"));
  EXPECT_FALSE(CStringView("C") <= CStringView("BA"));
  EXPECT_FALSE(CStringView("BA") <= CStringView("A"));
  EXPECT_FALSE(CStringView("BA") <= CStringView("B"));
  EXPECT_TRUE(CStringView("BA") <= CStringView("C"));

  EXPECT_TRUE(CStringView("AA") <= CStringView("B"));
  EXPECT_FALSE(CStringView("BA") <= CStringView("B"));
  EXPECT_FALSE(CStringView("CA") <= CStringView("B"));
  EXPECT_FALSE(CStringView("B") <= CStringView("AA"));
  EXPECT_TRUE(CStringView("B") <= CStringView("BA"));
  EXPECT_TRUE(CStringView("B") <= CStringView("CA"));
}


TEST(TestString_CStringView, OperatorLessThanOrEqual_StringViewLite_LHS)
{
  EXPECT_LE(StringViewLite("A"), CStringView("B"));
  EXPECT_LE(StringViewLite("A"), CStringView("AB"));
  EXPECT_LE(StringViewLite(""), CStringView("AB"));
  EXPECT_LE(StringViewLite("A"), CStringView("A"));

  EXPECT_TRUE(StringViewLite("A") <= CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") <= CStringView("B"));
  EXPECT_FALSE(StringViewLite("C") <= CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") <= CStringView("A"));
  EXPECT_TRUE(StringViewLite("B") <= CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") <= CStringView("C"));

  EXPECT_TRUE(StringViewLite("A") <= CStringView("BA"));
  EXPECT_TRUE(StringViewLite("B") <= CStringView("BA"));
  EXPECT_FALSE(StringViewLite("C") <= CStringView("BA"));
  EXPECT_FALSE(StringViewLite("BA") <= CStringView("A"));
  EXPECT_FALSE(StringViewLite("BA") <= CStringView("B"));
  EXPECT_TRUE(StringViewLite("BA") <= CStringView("C"));

  EXPECT_TRUE(StringViewLite("AA") <= CStringView("B"));
  EXPECT_FALSE(StringViewLite("BA") <= CStringView("B"));
  EXPECT_FALSE(StringViewLite("CA") <= CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") <= CStringView("AA"));
  EXPECT_TRUE(StringViewLite("B") <= CStringView("BA"));
  EXPECT_TRUE(StringViewLite("B") <= CStringView("CA"));
}

TEST(TestString_CStringView, OperatorLessThanOrEqual_StringViewLite_RHS)
{
  EXPECT_LE(CStringView("A"), StringViewLite("B"));
  EXPECT_LE(CStringView("A"), StringViewLite("AB"));
  EXPECT_LE(CStringView(""), StringViewLite("AB"));
  EXPECT_LE(CStringView("A"), StringViewLite("A"));

  EXPECT_TRUE(CStringView("A") <= StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") <= StringViewLite("B"));
  EXPECT_FALSE(CStringView("C") <= StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") <= StringViewLite("A"));
  EXPECT_TRUE(CStringView("B") <= StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") <= StringViewLite("C"));

  EXPECT_TRUE(CStringView("A") <= StringViewLite("BA"));
  EXPECT_TRUE(CStringView("B") <= StringViewLite("BA"));
  EXPECT_FALSE(CStringView("C") <= StringViewLite("BA"));
  EXPECT_FALSE(CStringView("BA") <= StringViewLite("A"));
  EXPECT_FALSE(CStringView("BA") <= StringViewLite("B"));
  EXPECT_TRUE(CStringView("BA") <= StringViewLite("C"));

  EXPECT_TRUE(CStringView("AA") <= StringViewLite("B"));
  EXPECT_FALSE(CStringView("BA") <= StringViewLite("B"));
  EXPECT_FALSE(CStringView("CA") <= StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") <= StringViewLite("AA"));
  EXPECT_TRUE(CStringView("B") <= StringViewLite("BA"));
  EXPECT_TRUE(CStringView("B") <= StringViewLite("CA"));
}

TEST(TestString_CStringView, OperatorLessThanOrEqual_CString_LHS)
{
  EXPECT_LE("A", CStringView("B"));
  EXPECT_LE("A", CStringView("AB"));
  EXPECT_LE("", CStringView("AB"));
  EXPECT_LE("A", CStringView("A"));

  EXPECT_TRUE("A" <= CStringView("B"));
  EXPECT_TRUE("B" <= CStringView("B"));
  EXPECT_FALSE("C" <= CStringView("B"));
  EXPECT_FALSE("B" <= CStringView("A"));
  EXPECT_TRUE("B" <= CStringView("B"));
  EXPECT_TRUE("B" <= CStringView("C"));

  EXPECT_TRUE("A" <= CStringView("BA"));
  EXPECT_TRUE("B" <= CStringView("BA"));
  EXPECT_FALSE("C" <= CStringView("BA"));
  EXPECT_FALSE("BA" <= CStringView("A"));
  EXPECT_FALSE("BA" <= CStringView("B"));
  EXPECT_TRUE("BA" <= CStringView("C"));

  EXPECT_TRUE("AA" <= CStringView("B"));
  EXPECT_FALSE("BA" <= CStringView("B"));
  EXPECT_FALSE("CA" <= CStringView("B"));
  EXPECT_FALSE("B" <= CStringView("AA"));
  EXPECT_TRUE("B" <= CStringView("BA"));
  EXPECT_TRUE("B" <= CStringView("CA"));
}

TEST(TestString_CStringView, OperatorLessThanOrEqual_CString_RHS)
{
  EXPECT_LE(CStringView("A"), "B");
  EXPECT_LE(CStringView("A"), "AB");
  EXPECT_LE(CStringView(""), "AB");
  EXPECT_LE(CStringView("A"), "A");

  EXPECT_TRUE(CStringView("A") <= "B");
  EXPECT_TRUE(CStringView("B") <= "B");
  EXPECT_FALSE(CStringView("C") <= "B");
  EXPECT_FALSE(CStringView("B") <= "A");
  EXPECT_TRUE(CStringView("B") <= "B");
  EXPECT_TRUE(CStringView("B") <= "C");

  EXPECT_TRUE(CStringView("A") <= "BA");
  EXPECT_TRUE(CStringView("B") <= "BA");
  EXPECT_FALSE(CStringView("C") <= "BA");
  EXPECT_FALSE(CStringView("BA") <= "A");
  EXPECT_FALSE(CStringView("BA") <= "B");
  EXPECT_TRUE(CStringView("BA") <= "C");

  EXPECT_TRUE(CStringView("AA") <= "B");
  EXPECT_FALSE(CStringView("BA") <= "B");
  EXPECT_FALSE(CStringView("CA") <= "B");
  EXPECT_FALSE(CStringView("B") <= "AA");
  EXPECT_TRUE(CStringView("B") <= "BA");
  EXPECT_TRUE(CStringView("B") <= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, OperatorGreaterThan)
{
  EXPECT_GT(CStringView("B"), CStringView("A"));
  EXPECT_GT(CStringView("AB"), CStringView("A"));
  EXPECT_GT(CStringView("AB"), CStringView(""));

  EXPECT_FALSE(CStringView("A") > CStringView("B"));
  EXPECT_FALSE(CStringView("B") > CStringView("B"));
  EXPECT_TRUE(CStringView("C") > CStringView("B"));
  EXPECT_TRUE(CStringView("B") > CStringView("A"));
  EXPECT_FALSE(CStringView("B") > CStringView("B"));
  EXPECT_FALSE(CStringView("B") > CStringView("C"));

  EXPECT_FALSE(CStringView("A") > CStringView("BA"));
  EXPECT_FALSE(CStringView("B") > CStringView("BA"));
  EXPECT_TRUE(CStringView("C") > CStringView("BA"));
  EXPECT_TRUE(CStringView("BA") > CStringView("A"));
  EXPECT_TRUE(CStringView("BA") > CStringView("B"));
  EXPECT_FALSE(CStringView("BA") > CStringView("C"));

  EXPECT_FALSE(CStringView("AA") > CStringView("B"));
  EXPECT_TRUE(CStringView("BA") > CStringView("B"));
  EXPECT_TRUE(CStringView("CA") > CStringView("B"));
  EXPECT_TRUE(CStringView("B") > CStringView("AA"));
  EXPECT_FALSE(CStringView("B") > CStringView("BA"));
  EXPECT_FALSE(CStringView("B") > CStringView("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThan_StringViewLite_LHS)
{
  EXPECT_GT(StringViewLite("B"), CStringView("A"));
  EXPECT_GT(StringViewLite("AB"), CStringView("A"));
  EXPECT_GT(StringViewLite("AB"), CStringView(""));

  EXPECT_FALSE(StringViewLite("A") > CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") > CStringView("B"));
  EXPECT_TRUE(StringViewLite("C") > CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") > CStringView("A"));
  EXPECT_FALSE(StringViewLite("B") > CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") > CStringView("C"));

  EXPECT_FALSE(StringViewLite("A") > CStringView("BA"));
  EXPECT_FALSE(StringViewLite("B") > CStringView("BA"));
  EXPECT_TRUE(StringViewLite("C") > CStringView("BA"));
  EXPECT_TRUE(StringViewLite("BA") > CStringView("A"));
  EXPECT_TRUE(StringViewLite("BA") > CStringView("B"));
  EXPECT_FALSE(StringViewLite("BA") > CStringView("C"));

  EXPECT_FALSE(StringViewLite("AA") > CStringView("B"));
  EXPECT_TRUE(StringViewLite("BA") > CStringView("B"));
  EXPECT_TRUE(StringViewLite("CA") > CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") > CStringView("AA"));
  EXPECT_FALSE(StringViewLite("B") > CStringView("BA"));
  EXPECT_FALSE(StringViewLite("B") > CStringView("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThan_StringViewLite_RHS)
{
  EXPECT_GT(CStringView("B"), StringViewLite("A"));
  EXPECT_GT(CStringView("AB"), StringViewLite("A"));
  EXPECT_GT(CStringView("AB"), StringViewLite(""));

  EXPECT_FALSE(CStringView("A") > StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") > StringViewLite("B"));
  EXPECT_TRUE(CStringView("C") > StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") > StringViewLite("A"));
  EXPECT_FALSE(CStringView("B") > StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") > StringViewLite("C"));

  EXPECT_FALSE(CStringView("A") > StringViewLite("BA"));
  EXPECT_FALSE(CStringView("B") > StringViewLite("BA"));
  EXPECT_TRUE(CStringView("C") > StringViewLite("BA"));
  EXPECT_TRUE(CStringView("BA") > StringViewLite("A"));
  EXPECT_TRUE(CStringView("BA") > StringViewLite("B"));
  EXPECT_FALSE(CStringView("BA") > StringViewLite("C"));

  EXPECT_FALSE(CStringView("AA") > StringViewLite("B"));
  EXPECT_TRUE(CStringView("BA") > StringViewLite("B"));
  EXPECT_TRUE(CStringView("CA") > StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") > StringViewLite("AA"));
  EXPECT_FALSE(CStringView("B") > StringViewLite("BA"));
  EXPECT_FALSE(CStringView("B") > StringViewLite("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThan_CString_LHS)
{
  EXPECT_GT("B", CStringView("A"));
  EXPECT_GT("AB", CStringView("A"));
  EXPECT_GT("AB", CStringView(""));

  EXPECT_FALSE("A" > CStringView("B"));
  EXPECT_FALSE("B" > CStringView("B"));
  EXPECT_TRUE("C" > CStringView("B"));
  EXPECT_TRUE("B" > CStringView("A"));
  EXPECT_FALSE("B" > CStringView("B"));
  EXPECT_FALSE("B" > CStringView("C"));

  EXPECT_FALSE("A" > CStringView("BA"));
  EXPECT_FALSE("B" > CStringView("BA"));
  EXPECT_TRUE("C" > CStringView("BA"));
  EXPECT_TRUE("BA" > CStringView("A"));
  EXPECT_TRUE("BA" > CStringView("B"));
  EXPECT_FALSE("BA" > CStringView("C"));

  EXPECT_FALSE("AA" > CStringView("B"));
  EXPECT_TRUE("BA" > CStringView("B"));
  EXPECT_TRUE("CA" > CStringView("B"));
  EXPECT_TRUE("B" > CStringView("AA"));
  EXPECT_FALSE("B" > CStringView("BA"));
  EXPECT_FALSE("B" > CStringView("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThan_CString_RHS)
{
  EXPECT_GT(CStringView("B"), "A");
  EXPECT_GT(CStringView("AB"), "A");
  EXPECT_GT(CStringView("AB"), "");

  EXPECT_FALSE(CStringView("A") > "B");
  EXPECT_FALSE(CStringView("B") > "B");
  EXPECT_TRUE(CStringView("C") > "B");
  EXPECT_TRUE(CStringView("B") > "A");
  EXPECT_FALSE(CStringView("B") > "B");
  EXPECT_FALSE(CStringView("B") > "C");

  EXPECT_FALSE(CStringView("A") > "BA");
  EXPECT_FALSE(CStringView("B") > "BA");
  EXPECT_TRUE(CStringView("C") > "BA");
  EXPECT_TRUE(CStringView("BA") > "A");
  EXPECT_TRUE(CStringView("BA") > "B");
  EXPECT_FALSE(CStringView("BA") > "C");

  EXPECT_FALSE(CStringView("AA") > "B");
  EXPECT_TRUE(CStringView("BA") > "B");
  EXPECT_TRUE(CStringView("CA") > "B");
  EXPECT_TRUE(CStringView("B") > "AA");
  EXPECT_FALSE(CStringView("B") > "BA");
  EXPECT_FALSE(CStringView("B") > "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(CStringView("B"), CStringView("A"));
  EXPECT_GE(CStringView("AB"), CStringView("A"));
  EXPECT_GE(CStringView("AB"), CStringView(""));
  EXPECT_GE(CStringView("A"), CStringView("A"));

  EXPECT_FALSE(CStringView("A") >= CStringView("B"));
  EXPECT_TRUE(CStringView("B") >= CStringView("B"));
  EXPECT_TRUE(CStringView("C") >= CStringView("B"));
  EXPECT_TRUE(CStringView("B") >= CStringView("A"));
  EXPECT_TRUE(CStringView("B") >= CStringView("B"));
  EXPECT_FALSE(CStringView("B") >= CStringView("C"));

  EXPECT_FALSE(CStringView("A") >= CStringView("BA"));
  EXPECT_FALSE(CStringView("B") >= CStringView("BA"));
  EXPECT_TRUE(CStringView("C") >= CStringView("BA"));
  EXPECT_TRUE(CStringView("BA") >= CStringView("A"));
  EXPECT_TRUE(CStringView("BA") >= CStringView("B"));
  EXPECT_FALSE(CStringView("BA") >= CStringView("C"));

  EXPECT_FALSE(CStringView("AA") >= CStringView("B"));
  EXPECT_TRUE(CStringView("BA") >= CStringView("B"));
  EXPECT_TRUE(CStringView("CA") >= CStringView("B"));
  EXPECT_TRUE(CStringView("B") >= CStringView("AA"));
  EXPECT_FALSE(CStringView("B") >= CStringView("BA"));
  EXPECT_FALSE(CStringView("B") >= CStringView("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThanOrEqual_StringViewLite_LHS)
{
  EXPECT_GE(StringViewLite("B"), CStringView("A"));
  EXPECT_GE(StringViewLite("AB"), CStringView("A"));
  EXPECT_GE(StringViewLite("AB"), CStringView(""));
  EXPECT_GE(StringViewLite("A"), CStringView("A"));

  EXPECT_FALSE(StringViewLite("A") >= CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") >= CStringView("B"));
  EXPECT_TRUE(StringViewLite("C") >= CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") >= CStringView("A"));
  EXPECT_TRUE(StringViewLite("B") >= CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") >= CStringView("C"));

  EXPECT_FALSE(StringViewLite("A") >= CStringView("BA"));
  EXPECT_FALSE(StringViewLite("B") >= CStringView("BA"));
  EXPECT_TRUE(StringViewLite("C") >= CStringView("BA"));
  EXPECT_TRUE(StringViewLite("BA") >= CStringView("A"));
  EXPECT_TRUE(StringViewLite("BA") >= CStringView("B"));
  EXPECT_FALSE(StringViewLite("BA") >= CStringView("C"));

  EXPECT_FALSE(StringViewLite("AA") >= CStringView("B"));
  EXPECT_TRUE(StringViewLite("BA") >= CStringView("B"));
  EXPECT_TRUE(StringViewLite("CA") >= CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") >= CStringView("AA"));
  EXPECT_FALSE(StringViewLite("B") >= CStringView("BA"));
  EXPECT_FALSE(StringViewLite("B") >= CStringView("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThanOrEqual_StringViewLite_RHS)
{
  EXPECT_GE(CStringView("B"), StringViewLite("A"));
  EXPECT_GE(CStringView("AB"), StringViewLite("A"));
  EXPECT_GE(CStringView("AB"), StringViewLite(""));
  EXPECT_GE(CStringView("A"), StringViewLite("A"));

  EXPECT_FALSE(CStringView("A") >= StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") >= StringViewLite("B"));
  EXPECT_TRUE(CStringView("C") >= StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") >= StringViewLite("A"));
  EXPECT_TRUE(CStringView("B") >= StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") >= StringViewLite("C"));

  EXPECT_FALSE(CStringView("A") >= StringViewLite("BA"));
  EXPECT_FALSE(CStringView("B") >= StringViewLite("BA"));
  EXPECT_TRUE(CStringView("C") >= StringViewLite("BA"));
  EXPECT_TRUE(CStringView("BA") >= StringViewLite("A"));
  EXPECT_TRUE(CStringView("BA") >= StringViewLite("B"));
  EXPECT_FALSE(CStringView("BA") >= StringViewLite("C"));

  EXPECT_FALSE(CStringView("AA") >= StringViewLite("B"));
  EXPECT_TRUE(CStringView("BA") >= StringViewLite("B"));
  EXPECT_TRUE(CStringView("CA") >= StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") >= StringViewLite("AA"));
  EXPECT_FALSE(CStringView("B") >= StringViewLite("BA"));
  EXPECT_FALSE(CStringView("B") >= StringViewLite("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThanOrEqual_CString_LHS)
{
  EXPECT_GE("B", CStringView("A"));
  EXPECT_GE("AB", CStringView("A"));
  EXPECT_GE("AB", CStringView(""));
  EXPECT_GE("A", CStringView("A"));

  EXPECT_FALSE("A" >= CStringView("B"));
  EXPECT_TRUE("B" >= CStringView("B"));
  EXPECT_TRUE("C" >= CStringView("B"));
  EXPECT_TRUE("B" >= CStringView("A"));
  EXPECT_TRUE("B" >= CStringView("B"));
  EXPECT_FALSE("B" >= CStringView("C"));

  EXPECT_FALSE("A" >= CStringView("BA"));
  EXPECT_FALSE("B" >= CStringView("BA"));
  EXPECT_TRUE("C" >= CStringView("BA"));
  EXPECT_TRUE("BA" >= CStringView("A"));
  EXPECT_TRUE("BA" >= CStringView("B"));
  EXPECT_FALSE("BA" >= CStringView("C"));

  EXPECT_FALSE("AA" >= CStringView("B"));
  EXPECT_TRUE("BA" >= CStringView("B"));
  EXPECT_TRUE("CA" >= CStringView("B"));
  EXPECT_TRUE("B" >= CStringView("AA"));
  EXPECT_FALSE("B" >= CStringView("BA"));
  EXPECT_FALSE("B" >= CStringView("CA"));
}

TEST(TestString_CStringView, OperatorGreaterThanOrEqual_CString_RHS)
{
  EXPECT_GE(CStringView("B"), "A");
  EXPECT_GE(CStringView("AB"), "A");
  EXPECT_GE(CStringView("AB"), "");
  EXPECT_GE(CStringView("A"), "A");

  EXPECT_FALSE(CStringView("A") >= "B");
  EXPECT_TRUE(CStringView("B") >= "B");
  EXPECT_TRUE(CStringView("C") >= "B");
  EXPECT_TRUE(CStringView("B") >= "A");
  EXPECT_TRUE(CStringView("B") >= "B");
  EXPECT_FALSE(CStringView("B") >= "C");

  EXPECT_FALSE(CStringView("A") >= "BA");
  EXPECT_FALSE(CStringView("B") >= "BA");
  EXPECT_TRUE(CStringView("C") >= "BA");
  EXPECT_TRUE(CStringView("BA") >= "A");
  EXPECT_TRUE(CStringView("BA") >= "B");
  EXPECT_FALSE(CStringView("BA") >= "C");

  EXPECT_FALSE(CStringView("AA") >= "B");
  EXPECT_TRUE(CStringView("BA") >= "B");
  EXPECT_TRUE(CStringView("CA") >= "B");
  EXPECT_TRUE(CStringView("B") >= "AA");
  EXPECT_FALSE(CStringView("B") >= "BA");
  EXPECT_FALSE(CStringView("B") >= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_CStringView, OperatorEqual)
{
  EXPECT_TRUE(CStringView("A") == CStringView("A"));
  EXPECT_TRUE(CStringView("") == CStringView(""));
  EXPECT_FALSE(CStringView("A") == CStringView(""));
  EXPECT_FALSE(CStringView("A") == CStringView("B"));
  EXPECT_FALSE(CStringView("A") == CStringView("a"));

  EXPECT_FALSE(CStringView("A") == CStringView("B"));
  EXPECT_TRUE(CStringView("B") == CStringView("B"));
  EXPECT_FALSE(CStringView("C") == CStringView("B"));
  EXPECT_FALSE(CStringView("B") == CStringView("A"));
  EXPECT_TRUE(CStringView("B") == CStringView("B"));
  EXPECT_FALSE(CStringView("B") == CStringView("C"));

  EXPECT_FALSE(CStringView("A") == CStringView("BA"));
  EXPECT_FALSE(CStringView("B") == CStringView("BA"));
  EXPECT_FALSE(CStringView("C") == CStringView("BA"));
  EXPECT_FALSE(CStringView("BA") == CStringView("A"));
  EXPECT_FALSE(CStringView("BA") == CStringView("B"));
  EXPECT_FALSE(CStringView("BA") == CStringView("C"));

  EXPECT_FALSE(CStringView("AA") == CStringView("B"));
  EXPECT_FALSE(CStringView("BA") == CStringView("B"));
  EXPECT_FALSE(CStringView("CA") == CStringView("B"));
  EXPECT_FALSE(CStringView("B") == CStringView("AA"));
  EXPECT_FALSE(CStringView("B") == CStringView("BA"));
  EXPECT_FALSE(CStringView("B") == CStringView("CA"));
}


TEST(TestString_CStringView, OperatorEqual_StringViewLite_LHS)
{
  EXPECT_TRUE(StringViewLite("A") == CStringView("A"));
  EXPECT_TRUE(StringViewLite("") == CStringView(""));
  EXPECT_FALSE(StringViewLite("A") == CStringView(""));
  EXPECT_FALSE(StringViewLite("A") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("A") == CStringView("a"));

  EXPECT_FALSE(StringViewLite("A") == CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("C") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") == CStringView("A"));
  EXPECT_TRUE(StringViewLite("B") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") == CStringView("C"));

  EXPECT_FALSE(StringViewLite("A") == CStringView("BA"));
  EXPECT_FALSE(StringViewLite("B") == CStringView("BA"));
  EXPECT_FALSE(StringViewLite("C") == CStringView("BA"));
  EXPECT_FALSE(StringViewLite("BA") == CStringView("A"));
  EXPECT_FALSE(StringViewLite("BA") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("BA") == CStringView("C"));

  EXPECT_FALSE(StringViewLite("AA") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("BA") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("CA") == CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") == CStringView("AA"));
  EXPECT_FALSE(StringViewLite("B") == CStringView("BA"));
  EXPECT_FALSE(StringViewLite("B") == CStringView("CA"));
}


TEST(TestString_CStringView, OperatorEqual_StringViewLite_RHS)
{
  EXPECT_TRUE(CStringView("A") == StringViewLite("A"));
  EXPECT_TRUE(CStringView("") == StringViewLite(""));
  EXPECT_FALSE(CStringView("A") == StringViewLite(""));
  EXPECT_FALSE(CStringView("A") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("A") == StringViewLite("a"));

  EXPECT_FALSE(CStringView("A") == StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("C") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") == StringViewLite("A"));
  EXPECT_TRUE(CStringView("B") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") == StringViewLite("C"));

  EXPECT_FALSE(CStringView("A") == StringViewLite("BA"));
  EXPECT_FALSE(CStringView("B") == StringViewLite("BA"));
  EXPECT_FALSE(CStringView("C") == StringViewLite("BA"));
  EXPECT_FALSE(CStringView("BA") == StringViewLite("A"));
  EXPECT_FALSE(CStringView("BA") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("BA") == StringViewLite("C"));

  EXPECT_FALSE(CStringView("AA") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("BA") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("CA") == StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") == StringViewLite("AA"));
  EXPECT_FALSE(CStringView("B") == StringViewLite("BA"));
  EXPECT_FALSE(CStringView("B") == StringViewLite("CA"));
}

TEST(TestString_CStringView, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE("A" == CStringView("A"));
  EXPECT_TRUE("" == CStringView(""));    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE("A" == CStringView(""));
  EXPECT_FALSE("A" == CStringView("B"));
  EXPECT_FALSE("A" == CStringView("a"));

  EXPECT_FALSE("A" == CStringView("B"));
  EXPECT_TRUE("B" == CStringView("B"));
  EXPECT_FALSE("C" == CStringView("B"));
  EXPECT_FALSE("B" == CStringView("A"));
  EXPECT_TRUE("B" == CStringView("B"));
  EXPECT_FALSE("B" == CStringView("C"));

  EXPECT_FALSE("A" == CStringView("BA"));
  EXPECT_FALSE("B" == CStringView("BA"));
  EXPECT_FALSE("C" == CStringView("BA"));
  EXPECT_FALSE("BA" == CStringView("A"));
  EXPECT_FALSE("BA" == CStringView("B"));
  EXPECT_FALSE("BA" == CStringView("C"));

  EXPECT_FALSE("AA" == CStringView("B"));
  EXPECT_FALSE("BA" == CStringView("B"));
  EXPECT_FALSE("CA" == CStringView("B"));
  EXPECT_FALSE("B" == CStringView("AA"));
  EXPECT_FALSE("B" == CStringView("BA"));
  EXPECT_FALSE("B" == CStringView("CA"));
}


TEST(TestString_CStringView, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(CStringView("A") == "A");
  EXPECT_TRUE(CStringView("") == "");      // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(CStringView("A") == "");    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(CStringView("A") == "B");
  EXPECT_FALSE(CStringView("A") == "a");

  EXPECT_FALSE(CStringView("A") == "B");
  EXPECT_TRUE(CStringView("B") == "B");
  EXPECT_FALSE(CStringView("C") == "B");
  EXPECT_FALSE(CStringView("B") == "A");
  EXPECT_TRUE(CStringView("B") == "B");
  EXPECT_FALSE(CStringView("B") == "C");

  EXPECT_FALSE(CStringView("A") == "BA");
  EXPECT_FALSE(CStringView("B") == "BA");
  EXPECT_FALSE(CStringView("C") == "BA");
  EXPECT_FALSE(CStringView("BA") == "A");
  EXPECT_FALSE(CStringView("BA") == "B");
  EXPECT_FALSE(CStringView("BA") == "C");

  EXPECT_FALSE(CStringView("AA") == "B");
  EXPECT_FALSE(CStringView("BA") == "B");
  EXPECT_FALSE(CStringView("CA") == "B");
  EXPECT_FALSE(CStringView("B") == "AA");
  EXPECT_FALSE(CStringView("B") == "BA");
  EXPECT_FALSE(CStringView("B") == "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, OperatorNotEqual)
{
  EXPECT_FALSE(CStringView("A") != CStringView("A"));
  EXPECT_FALSE(CStringView("") != CStringView(""));
  EXPECT_TRUE(CStringView("A") != CStringView(""));
  EXPECT_TRUE(CStringView("A") != CStringView("B"));
  EXPECT_TRUE(CStringView("A") != CStringView("a"));

  EXPECT_TRUE(CStringView("A") != CStringView("B"));
  EXPECT_FALSE(CStringView("B") != CStringView("B"));
  EXPECT_TRUE(CStringView("C") != CStringView("B"));
  EXPECT_TRUE(CStringView("B") != CStringView("A"));
  EXPECT_FALSE(CStringView("B") != CStringView("B"));
  EXPECT_TRUE(CStringView("B") != CStringView("C"));

  EXPECT_TRUE(CStringView("A") != CStringView("BA"));
  EXPECT_TRUE(CStringView("B") != CStringView("BA"));
  EXPECT_TRUE(CStringView("C") != CStringView("BA"));
  EXPECT_TRUE(CStringView("BA") != CStringView("A"));
  EXPECT_TRUE(CStringView("BA") != CStringView("B"));
  EXPECT_TRUE(CStringView("BA") != CStringView("C"));

  EXPECT_TRUE(CStringView("AA") != CStringView("B"));
  EXPECT_TRUE(CStringView("BA") != CStringView("B"));
  EXPECT_TRUE(CStringView("CA") != CStringView("B"));
  EXPECT_TRUE(CStringView("B") != CStringView("AA"));
  EXPECT_TRUE(CStringView("B") != CStringView("BA"));
  EXPECT_TRUE(CStringView("B") != CStringView("CA"));
}

TEST(TestString_CStringView, OperatorNotEqual_StringViewLite_LHS)
{
  EXPECT_FALSE(StringViewLite("A") != CStringView("A"));
  EXPECT_FALSE(StringViewLite("") != CStringView(""));
  EXPECT_TRUE(StringViewLite("A") != CStringView(""));
  EXPECT_TRUE(StringViewLite("A") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("A") != CStringView("a"));

  EXPECT_TRUE(StringViewLite("A") != CStringView("B"));
  EXPECT_FALSE(StringViewLite("B") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("C") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") != CStringView("A"));
  EXPECT_FALSE(StringViewLite("B") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") != CStringView("C"));

  EXPECT_TRUE(StringViewLite("A") != CStringView("BA"));
  EXPECT_TRUE(StringViewLite("B") != CStringView("BA"));
  EXPECT_TRUE(StringViewLite("C") != CStringView("BA"));
  EXPECT_TRUE(StringViewLite("BA") != CStringView("A"));
  EXPECT_TRUE(StringViewLite("BA") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("BA") != CStringView("C"));

  EXPECT_TRUE(StringViewLite("AA") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("BA") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("CA") != CStringView("B"));
  EXPECT_TRUE(StringViewLite("B") != CStringView("AA"));
  EXPECT_TRUE(StringViewLite("B") != CStringView("BA"));
  EXPECT_TRUE(StringViewLite("B") != CStringView("CA"));
}

TEST(TestString_CStringView, OperatorNotEqual_StringViewLite_RHS)
{
  EXPECT_FALSE(CStringView("A") != StringViewLite("A"));
  EXPECT_FALSE(CStringView("") != StringViewLite(""));
  EXPECT_TRUE(CStringView("A") != StringViewLite(""));
  EXPECT_TRUE(CStringView("A") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("A") != StringViewLite("a"));

  EXPECT_TRUE(CStringView("A") != StringViewLite("B"));
  EXPECT_FALSE(CStringView("B") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("C") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") != StringViewLite("A"));
  EXPECT_FALSE(CStringView("B") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") != StringViewLite("C"));

  EXPECT_TRUE(CStringView("A") != StringViewLite("BA"));
  EXPECT_TRUE(CStringView("B") != StringViewLite("BA"));
  EXPECT_TRUE(CStringView("C") != StringViewLite("BA"));
  EXPECT_TRUE(CStringView("BA") != StringViewLite("A"));
  EXPECT_TRUE(CStringView("BA") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("BA") != StringViewLite("C"));

  EXPECT_TRUE(CStringView("AA") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("BA") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("CA") != StringViewLite("B"));
  EXPECT_TRUE(CStringView("B") != StringViewLite("AA"));
  EXPECT_TRUE(CStringView("B") != StringViewLite("BA"));
  EXPECT_TRUE(CStringView("B") != StringViewLite("CA"));
}

TEST(TestString_CStringView, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE("A" != CStringView("A"));
  EXPECT_FALSE("" != CStringView(""));    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE("A" != CStringView(""));
  EXPECT_TRUE("A" != CStringView("B"));
  EXPECT_TRUE("A" != CStringView("a"));

  EXPECT_TRUE("A" != CStringView("B"));
  EXPECT_FALSE("B" != CStringView("B"));
  EXPECT_TRUE("C" != CStringView("B"));
  EXPECT_TRUE("B" != CStringView("A"));
  EXPECT_FALSE("B" != CStringView("B"));
  EXPECT_TRUE("B" != CStringView("C"));

  EXPECT_TRUE("A" != CStringView("BA"));
  EXPECT_TRUE("B" != CStringView("BA"));
  EXPECT_TRUE("C" != CStringView("BA"));
  EXPECT_TRUE("BA" != CStringView("A"));
  EXPECT_TRUE("BA" != CStringView("B"));
  EXPECT_TRUE("BA" != CStringView("C"));

  EXPECT_TRUE("AA" != CStringView("B"));
  EXPECT_TRUE("BA" != CStringView("B"));
  EXPECT_TRUE("CA" != CStringView("B"));
  EXPECT_TRUE("B" != CStringView("AA"));
  EXPECT_TRUE("B" != CStringView("BA"));
  EXPECT_TRUE("B" != CStringView("CA"));
}


TEST(TestString_CStringView, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(CStringView("A") != "A");
  EXPECT_FALSE(CStringView("") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(CStringView("A") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(CStringView("A") != "B");
  EXPECT_TRUE(CStringView("A") != "a");

  EXPECT_TRUE(CStringView("A") != "B");
  EXPECT_FALSE(CStringView("B") != "B");
  EXPECT_TRUE(CStringView("C") != "B");
  EXPECT_TRUE(CStringView("B") != "A");
  EXPECT_FALSE(CStringView("B") != "B");
  EXPECT_TRUE(CStringView("B") != "C");

  EXPECT_TRUE(CStringView("A") != "BA");
  EXPECT_TRUE(CStringView("B") != "BA");
  EXPECT_TRUE(CStringView("C") != "BA");
  EXPECT_TRUE(CStringView("BA") != "A");
  EXPECT_TRUE(CStringView("BA") != "B");
  EXPECT_TRUE(CStringView("BA") != "C");

  EXPECT_TRUE(CStringView("AA") != "B");
  EXPECT_TRUE(CStringView("BA") != "B");
  EXPECT_TRUE(CStringView("CA") != "B");
  EXPECT_TRUE(CStringView("B") != "AA");
  EXPECT_TRUE(CStringView("B") != "BA");
  EXPECT_TRUE(CStringView("B") != "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator null handling
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringView, OperatorLessThan_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringView strViewNull(nullptr);

  EXPECT_FALSE(CStringView("A") < pszNull);
  EXPECT_FALSE(CStringView("A") < strViewNull);
  EXPECT_TRUE(pszNull < CStringView("A"));
  EXPECT_TRUE(strViewNull < CStringView("A"));

  // Compare against empty string
  EXPECT_FALSE(CStringView("") < pszNull);
  EXPECT_FALSE(CStringView("") < strViewNull);
  EXPECT_FALSE(pszNull < CStringView(""));
  EXPECT_FALSE(strViewNull < CStringView(""));

  // Compare against null
  EXPECT_FALSE(pszNull < strViewNull);
  EXPECT_FALSE(strViewNull < pszNull);
  EXPECT_FALSE(strViewNull < strViewNull);
}


TEST(TestString_CStringView, OperatorLessThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringView strViewNull(nullptr);

  EXPECT_FALSE(CStringView("A") <= pszNull);
  EXPECT_FALSE(CStringView("A") <= strViewNull);
  EXPECT_TRUE(pszNull <= CStringView("A"));
  EXPECT_TRUE(strViewNull <= CStringView("A"));

  // Compare against empty string
  EXPECT_TRUE(CStringView("") <= pszNull);
  EXPECT_TRUE(CStringView("") <= strViewNull);
  EXPECT_TRUE(pszNull <= CStringView(""));
  EXPECT_TRUE(strViewNull <= CStringView(""));

  // Compare against null
  EXPECT_TRUE(pszNull <= strViewNull);
  EXPECT_TRUE(strViewNull <= pszNull);
  EXPECT_TRUE(strViewNull <= strViewNull);
}


TEST(TestString_CStringView, OperatorGreaterThan_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringView strViewNull(nullptr);

  EXPECT_TRUE(CStringView("A") > pszNull);
  EXPECT_TRUE(CStringView("A") > strViewNull);
  EXPECT_FALSE(pszNull > CStringView("A"));
  EXPECT_FALSE(strViewNull > CStringView("A"));

  // Compare against empty string
  EXPECT_FALSE(CStringView("") > pszNull);
  EXPECT_FALSE(CStringView("") > strViewNull);
  EXPECT_FALSE(pszNull > CStringView(""));
  EXPECT_FALSE(strViewNull > CStringView(""));

  // Compare against null
  EXPECT_FALSE(pszNull > strViewNull);
  EXPECT_FALSE(strViewNull > pszNull);
  EXPECT_FALSE(strViewNull > strViewNull);
}


TEST(TestString_CStringView, OperatorGreaterThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringView strViewNull(nullptr);

  EXPECT_TRUE(CStringView("A") >= pszNull);
  EXPECT_TRUE(CStringView("A") >= strViewNull);
  EXPECT_FALSE(pszNull >= CStringView("A"));
  EXPECT_FALSE(strViewNull >= CStringView("A"));

  // Compare against empty string
  EXPECT_TRUE(CStringView("") >= pszNull);
  EXPECT_TRUE(CStringView("") >= strViewNull);
  EXPECT_TRUE(pszNull >= CStringView(""));
  EXPECT_TRUE(strViewNull >= CStringView(""));

  // Compare against null
  EXPECT_TRUE(pszNull >= strViewNull);
  EXPECT_TRUE(strViewNull >= pszNull);
  EXPECT_TRUE(strViewNull >= strViewNull);
}


TEST(TestString_CStringView, OperatorEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringView strViewNull(nullptr);

  EXPECT_FALSE(CStringView("A") == pszNull);
  EXPECT_FALSE(CStringView("A") == strViewNull);
  EXPECT_FALSE(pszNull == CStringView("A"));
  EXPECT_FALSE(strViewNull == CStringView("A"));

  // Compare against empty string
  EXPECT_TRUE(pszNull == CStringView(""));
  EXPECT_TRUE(strViewNull == CStringView(""));
  EXPECT_TRUE(CStringView("") == pszNull);
  EXPECT_TRUE(CStringView("") == strViewNull);

  // Compare against null
  EXPECT_TRUE(pszNull == strViewNull);
  EXPECT_TRUE(strViewNull == pszNull);
  EXPECT_TRUE(strViewNull == strViewNull);
}


TEST(TestString_CStringView, OperatorNotEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringView strViewNull(nullptr);

  EXPECT_TRUE(CStringView("A") != pszNull);
  EXPECT_TRUE(CStringView("A") != strViewNull);
  EXPECT_TRUE(pszNull != CStringView("A"));
  EXPECT_TRUE(strViewNull != CStringView("A"));

  // Compare against empty string
  EXPECT_FALSE(pszNull != CStringView(""));
  EXPECT_FALSE(strViewNull != CStringView(""));
  EXPECT_FALSE(CStringView("") != pszNull);
  EXPECT_FALSE(CStringView("") != strViewNull);

  // Compare against null
  EXPECT_FALSE(pszNull != strViewNull);
  EXPECT_FALSE(strViewNull != pszNull);
  EXPECT_FALSE(strViewNull != strViewNull);
}
