/****************************************************************************************************************************************************
 * Copyright 2019 NXP
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


TEST(TestString_StringViewLite, Construct_FromNullTerminated)
{
  const auto psz = "Hello world";
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
