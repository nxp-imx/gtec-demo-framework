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

#include <FslBase/Log/String/LogCStringPointer.hpp>
#include <FslBase/String/CStringPointer.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestString_CStringPointer = TestFixtureFslBase;

  CStringPointer Convert(const std::string& str)
  {
    return {str.c_str()};
  }
}


TEST(TestString_CStringPointer, Construct)
{
  CStringPointer strView;

  EXPECT_TRUE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.data(), CStringPointer().data());
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(TestString_CStringPointer, Construct_FromZeroTerminated)
{
  const auto* const psz = "Hello world";
  auto lenPsz = std::strlen(psz);
  CStringPointer strView(psz);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), lenPsz);
  EXPECT_EQ(strView.length(), lenPsz);
}


TEST(TestString_CStringPointer, Construct_FromStr)
{
  std::string str("Hello world");
  CStringPointer strView = Convert(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}

TEST(TestString_CStringPointer, opEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_CStringPointer, opEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 == strView2);
}

TEST(TestString_CStringPointer, opEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringPointer, opEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringPointer, opEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringPointer, opEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 == strView2);
}


TEST(TestString_CStringPointer, opNotEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_CStringPointer, opNotEqual1)
{
  std::string str1("Hello world");
  std::string str2("Hello world");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 != strView2);
}

TEST(TestString_CStringPointer, opNotEqual2)
{
  std::string str1("Hello world");
  std::string str2("Hello worlD");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringPointer, opNotEqual3)
{
  std::string str1("Hello world");
  std::string str2("Hello worl");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringPointer, opNotEqual4)
{
  std::string str1("Hello world");
  std::string str2("Hello world2");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringPointer, opNotEqual5)
{
  std::string str1("Hello world");
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 != strView2);
}


TEST(TestString_CStringPointer, opLess_Empty)
{
  std::string str1;
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_CStringPointer, opLess1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_CStringPointer, opLess2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_CStringPointer, opLess3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}


TEST(TestString_CStringPointer, opLess4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 < strView2);
}


TEST(TestString_CStringPointer, opLess5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 < strView2);
}

TEST(TestString_CStringPointer, opLessOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringPointer, opLessOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringPointer, opLessOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}


TEST(TestString_CStringPointer, opLessOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringPointer, opLessOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 <= strView2);
}


TEST(TestString_CStringPointer, opLessOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 <= strView2);
}

TEST(TestString_CStringPointer, opGreater_Empty)
{
  std::string str1;
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringPointer, opGreater1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringPointer, opGreater2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}


TEST(TestString_CStringPointer, opGreater3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringPointer, opGreater4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 > strView2);
}


TEST(TestString_CStringPointer, opGreater5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 > strView2);
}

TEST(TestString_CStringPointer, opGreaterOrEqual_Empty)
{
  std::string str1;
  std::string str2;
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_CStringPointer, opGreaterOrEqual1)
{
  std::string str1("ab");
  std::string str2("ac");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_CStringPointer, opGreaterOrEqual2)
{
  std::string str1("ab");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_CStringPointer, opGreaterOrEqual3)
{
  std::string str1("ab");
  std::string str2("ab");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}


TEST(TestString_CStringPointer, opGreaterOrEqual4)
{
  std::string str1("a");
  std::string str2("aa");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_FALSE(strView1 >= strView2);
}


TEST(TestString_CStringPointer, opGreaterOrEqual5)
{
  std::string str1("aa");
  std::string str2("a");
  CStringPointer strView1 = Convert(str1);
  CStringPointer strView2 = Convert(str2);

  EXPECT_TRUE(strView1 >= strView2);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// compare
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, compare)
{
  EXPECT_TRUE(CStringPointer("A").compare(CStringPointer("B")) < 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("B")) == 0);
  EXPECT_TRUE(CStringPointer("C").compare(CStringPointer("B")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("A")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("C")) < 0);

  EXPECT_TRUE(CStringPointer("A").compare(CStringPointer("BA")) < 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("BA")) < 0);
  EXPECT_TRUE(CStringPointer("C").compare(CStringPointer("BA")) > 0);
  EXPECT_TRUE(CStringPointer("BA").compare(CStringPointer("A")) > 0);
  EXPECT_TRUE(CStringPointer("BA").compare(CStringPointer("B")) > 0);
  EXPECT_TRUE(CStringPointer("BA").compare(CStringPointer("C")) < 0);

  EXPECT_TRUE(CStringPointer("AA").compare(CStringPointer("B")) < 0);
  EXPECT_TRUE(CStringPointer("BA").compare(CStringPointer("B")) > 0);
  EXPECT_TRUE(CStringPointer("CA").compare(CStringPointer("B")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("AA")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("BA")) < 0);
  EXPECT_TRUE(CStringPointer("B").compare(CStringPointer("CA")) < 0);
}

TEST(TestString_CStringPointer, compare_string_view)
{
  EXPECT_TRUE(CStringPointer("A").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("B")) == 0);
  EXPECT_TRUE(CStringPointer("C").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(CStringPointer("A").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(CStringPointer("C").compare(std::string_view("BA")) > 0);
  EXPECT_TRUE(CStringPointer("BA").compare(std::string_view("A")) > 0);
  EXPECT_TRUE(CStringPointer("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(CStringPointer("BA").compare(std::string_view("C")) < 0);

  EXPECT_TRUE(CStringPointer("AA").compare(std::string_view("B")) < 0);
  EXPECT_TRUE(CStringPointer("BA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(CStringPointer("CA").compare(std::string_view("B")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("AA")) > 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("BA")) < 0);
  EXPECT_TRUE(CStringPointer("B").compare(std::string_view("CA")) < 0);
}

TEST(TestString_CStringPointer, compare_CString)
{
  EXPECT_TRUE(CStringPointer("A").compare("B") < 0);
  EXPECT_TRUE(CStringPointer("B").compare("B") == 0);
  EXPECT_TRUE(CStringPointer("C").compare("B") > 0);
  EXPECT_TRUE(CStringPointer("B").compare("A") > 0);
  EXPECT_TRUE(CStringPointer("B").compare("C") < 0);

  EXPECT_TRUE(CStringPointer("A").compare("BA") < 0);
  EXPECT_TRUE(CStringPointer("B").compare("BA") < 0);
  EXPECT_TRUE(CStringPointer("C").compare("BA") > 0);
  EXPECT_TRUE(CStringPointer("BA").compare("A") > 0);
  EXPECT_TRUE(CStringPointer("BA").compare("B") > 0);
  EXPECT_TRUE(CStringPointer("BA").compare("C") < 0);

  EXPECT_TRUE(CStringPointer("AA").compare("B") < 0);
  EXPECT_TRUE(CStringPointer("BA").compare("B") > 0);
  EXPECT_TRUE(CStringPointer("CA").compare("B") > 0);
  EXPECT_TRUE(CStringPointer("B").compare("AA") > 0);
  EXPECT_TRUE(CStringPointer("B").compare("BA") < 0);
  EXPECT_TRUE(CStringPointer("B").compare("CA") < 0);
}


TEST(TestString_CStringPointer, compare_Null)
{
  EXPECT_TRUE(CStringPointer().compare(CStringPointer()) == 0);
  EXPECT_TRUE(CStringPointer("").compare(CStringPointer()) == 0);
  EXPECT_TRUE(CStringPointer().compare(CStringPointer("")) == 0);
}

TEST(TestString_CStringPointer, compare_Null_CString)
{
  EXPECT_TRUE(CStringPointer().compare(nullptr) == 0);
  EXPECT_TRUE(CStringPointer("").compare(nullptr) == 0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, OperatorLessThan)
{
  EXPECT_LT(CStringPointer("A"), CStringPointer("B"));
  EXPECT_LT(CStringPointer("A"), CStringPointer("AB"));
  EXPECT_LT(CStringPointer(""), CStringPointer("AB"));

  EXPECT_TRUE(CStringPointer("A") < CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") < CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("C") < CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") < CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("B") < CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") < CStringPointer("C"));

  EXPECT_TRUE(CStringPointer("A") < CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("B") < CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("C") < CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("BA") < CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("BA") < CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("BA") < CStringPointer("C"));

  EXPECT_TRUE(CStringPointer("AA") < CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("BA") < CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("CA") < CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") < CStringPointer("AA"));
  EXPECT_TRUE(CStringPointer("B") < CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("B") < CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorLessThan_string_view_LHS)
{
  EXPECT_LT(std::string_view("A"), CStringPointer("B"));
  EXPECT_LT(std::string_view("A"), CStringPointer("AB"));
  EXPECT_LT(std::string_view(""), CStringPointer("AB"));

  EXPECT_TRUE(std::string_view("A") < CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") < CStringPointer("B"));
  EXPECT_FALSE(std::string_view("C") < CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") < CStringPointer("A"));
  EXPECT_FALSE(std::string_view("B") < CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") < CStringPointer("C"));

  EXPECT_TRUE(std::string_view("A") < CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("B") < CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("C") < CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("BA") < CStringPointer("A"));
  EXPECT_FALSE(std::string_view("BA") < CStringPointer("B"));
  EXPECT_TRUE(std::string_view("BA") < CStringPointer("C"));

  EXPECT_TRUE(std::string_view("AA") < CStringPointer("B"));
  EXPECT_FALSE(std::string_view("BA") < CStringPointer("B"));
  EXPECT_FALSE(std::string_view("CA") < CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") < CStringPointer("AA"));
  EXPECT_TRUE(std::string_view("B") < CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("B") < CStringPointer("CA"));
}


TEST(TestString_CStringPointer, OperatorLessThan_string_view_RHS)
{
  EXPECT_LT(CStringPointer("A"), std::string_view("B"));
  EXPECT_LT(CStringPointer("A"), std::string_view("AB"));
  EXPECT_LT(CStringPointer(""), std::string_view("AB"));

  EXPECT_TRUE(CStringPointer("A") < std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") < std::string_view("B"));
  EXPECT_FALSE(CStringPointer("C") < std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") < std::string_view("A"));
  EXPECT_FALSE(CStringPointer("B") < std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") < std::string_view("C"));

  EXPECT_TRUE(CStringPointer("A") < std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("B") < std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("C") < std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("BA") < std::string_view("A"));
  EXPECT_FALSE(CStringPointer("BA") < std::string_view("B"));
  EXPECT_TRUE(CStringPointer("BA") < std::string_view("C"));

  EXPECT_TRUE(CStringPointer("AA") < std::string_view("B"));
  EXPECT_FALSE(CStringPointer("BA") < std::string_view("B"));
  EXPECT_FALSE(CStringPointer("CA") < std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") < std::string_view("AA"));
  EXPECT_TRUE(CStringPointer("B") < std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("B") < std::string_view("CA"));
}


TEST(TestString_CStringPointer, OperatorLessThan_CString_LHS)
{
  EXPECT_LT("A", CStringPointer("B"));
  EXPECT_LT("A", CStringPointer("AB"));
  EXPECT_LT("", CStringPointer("AB"));

  EXPECT_TRUE("A" < CStringPointer("B"));
  EXPECT_FALSE("B" < CStringPointer("B"));
  EXPECT_FALSE("C" < CStringPointer("B"));
  EXPECT_FALSE("B" < CStringPointer("A"));
  EXPECT_FALSE("B" < CStringPointer("B"));
  EXPECT_TRUE("B" < CStringPointer("C"));

  EXPECT_TRUE("A" < CStringPointer("BA"));
  EXPECT_TRUE("B" < CStringPointer("BA"));
  EXPECT_FALSE("C" < CStringPointer("BA"));
  EXPECT_FALSE("BA" < CStringPointer("A"));
  EXPECT_FALSE("BA" < CStringPointer("B"));
  EXPECT_TRUE("BA" < CStringPointer("C"));

  EXPECT_TRUE("AA" < CStringPointer("B"));
  EXPECT_FALSE("BA" < CStringPointer("B"));
  EXPECT_FALSE("CA" < CStringPointer("B"));
  EXPECT_FALSE("B" < CStringPointer("AA"));
  EXPECT_TRUE("B" < CStringPointer("BA"));
  EXPECT_TRUE("B" < CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorLessThan_CString_RHS)
{
  EXPECT_LT(CStringPointer("A"), "B");
  EXPECT_LT(CStringPointer("A"), "AB");
  EXPECT_LT(CStringPointer(""), "AB");

  EXPECT_TRUE(CStringPointer("A") < "B");
  EXPECT_FALSE(CStringPointer("B") < "B");
  EXPECT_FALSE(CStringPointer("C") < "B");
  EXPECT_FALSE(CStringPointer("B") < "A");
  EXPECT_FALSE(CStringPointer("B") < "B");
  EXPECT_TRUE(CStringPointer("B") < "C");

  EXPECT_TRUE(CStringPointer("A") < "BA");
  EXPECT_TRUE(CStringPointer("B") < "BA");
  EXPECT_FALSE(CStringPointer("C") < "BA");
  EXPECT_FALSE(CStringPointer("BA") < "A");
  EXPECT_FALSE(CStringPointer("BA") < "B");
  EXPECT_TRUE(CStringPointer("BA") < "C");

  EXPECT_TRUE(CStringPointer("AA") < "B");
  EXPECT_FALSE(CStringPointer("BA") < "B");
  EXPECT_FALSE(CStringPointer("CA") < "B");
  EXPECT_FALSE(CStringPointer("B") < "AA");
  EXPECT_TRUE(CStringPointer("B") < "BA");
  EXPECT_TRUE(CStringPointer("B") < "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, OperatorLessThanOrEqual)
{
  EXPECT_LE(CStringPointer("A"), CStringPointer("B"));
  EXPECT_LE(CStringPointer("A"), CStringPointer("AB"));
  EXPECT_LE(CStringPointer(""), CStringPointer("AB"));
  EXPECT_LE(CStringPointer("A"), CStringPointer("A"));

  EXPECT_TRUE(CStringPointer("A") <= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") <= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("C") <= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") <= CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("B") <= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") <= CStringPointer("C"));

  EXPECT_TRUE(CStringPointer("A") <= CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("B") <= CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("C") <= CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("BA") <= CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("BA") <= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("BA") <= CStringPointer("C"));

  EXPECT_TRUE(CStringPointer("AA") <= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("BA") <= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("CA") <= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") <= CStringPointer("AA"));
  EXPECT_TRUE(CStringPointer("B") <= CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("B") <= CStringPointer("CA"));
}


TEST(TestString_CStringPointer, OperatorLessThanOrEqual_string_view_LHS)
{
  EXPECT_LE(std::string_view("A"), CStringPointer("B"));
  EXPECT_LE(std::string_view("A"), CStringPointer("AB"));
  EXPECT_LE(std::string_view(""), CStringPointer("AB"));
  EXPECT_LE(std::string_view("A"), CStringPointer("A"));

  EXPECT_TRUE(std::string_view("A") <= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") <= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("C") <= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") <= CStringPointer("A"));
  EXPECT_TRUE(std::string_view("B") <= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") <= CStringPointer("C"));

  EXPECT_TRUE(std::string_view("A") <= CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("B") <= CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("C") <= CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("BA") <= CStringPointer("A"));
  EXPECT_FALSE(std::string_view("BA") <= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("BA") <= CStringPointer("C"));

  EXPECT_TRUE(std::string_view("AA") <= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("BA") <= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("CA") <= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") <= CStringPointer("AA"));
  EXPECT_TRUE(std::string_view("B") <= CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("B") <= CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorLessThanOrEqual_string_view_RHS)
{
  EXPECT_LE(CStringPointer("A"), std::string_view("B"));
  EXPECT_LE(CStringPointer("A"), std::string_view("AB"));
  EXPECT_LE(CStringPointer(""), std::string_view("AB"));
  EXPECT_LE(CStringPointer("A"), std::string_view("A"));

  EXPECT_TRUE(CStringPointer("A") <= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") <= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("C") <= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") <= std::string_view("A"));
  EXPECT_TRUE(CStringPointer("B") <= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") <= std::string_view("C"));

  EXPECT_TRUE(CStringPointer("A") <= std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("B") <= std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("C") <= std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("BA") <= std::string_view("A"));
  EXPECT_FALSE(CStringPointer("BA") <= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("BA") <= std::string_view("C"));

  EXPECT_TRUE(CStringPointer("AA") <= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("BA") <= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("CA") <= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") <= std::string_view("AA"));
  EXPECT_TRUE(CStringPointer("B") <= std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("B") <= std::string_view("CA"));
}

TEST(TestString_CStringPointer, OperatorLessThanOrEqual_CString_LHS)
{
  EXPECT_LE("A", CStringPointer("B"));
  EXPECT_LE("A", CStringPointer("AB"));
  EXPECT_LE("", CStringPointer("AB"));
  EXPECT_LE("A", CStringPointer("A"));

  EXPECT_TRUE("A" <= CStringPointer("B"));
  EXPECT_TRUE("B" <= CStringPointer("B"));
  EXPECT_FALSE("C" <= CStringPointer("B"));
  EXPECT_FALSE("B" <= CStringPointer("A"));
  EXPECT_TRUE("B" <= CStringPointer("B"));
  EXPECT_TRUE("B" <= CStringPointer("C"));

  EXPECT_TRUE("A" <= CStringPointer("BA"));
  EXPECT_TRUE("B" <= CStringPointer("BA"));
  EXPECT_FALSE("C" <= CStringPointer("BA"));
  EXPECT_FALSE("BA" <= CStringPointer("A"));
  EXPECT_FALSE("BA" <= CStringPointer("B"));
  EXPECT_TRUE("BA" <= CStringPointer("C"));

  EXPECT_TRUE("AA" <= CStringPointer("B"));
  EXPECT_FALSE("BA" <= CStringPointer("B"));
  EXPECT_FALSE("CA" <= CStringPointer("B"));
  EXPECT_FALSE("B" <= CStringPointer("AA"));
  EXPECT_TRUE("B" <= CStringPointer("BA"));
  EXPECT_TRUE("B" <= CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorLessThanOrEqual_CString_RHS)
{
  EXPECT_LE(CStringPointer("A"), "B");
  EXPECT_LE(CStringPointer("A"), "AB");
  EXPECT_LE(CStringPointer(""), "AB");
  EXPECT_LE(CStringPointer("A"), "A");

  EXPECT_TRUE(CStringPointer("A") <= "B");
  EXPECT_TRUE(CStringPointer("B") <= "B");
  EXPECT_FALSE(CStringPointer("C") <= "B");
  EXPECT_FALSE(CStringPointer("B") <= "A");
  EXPECT_TRUE(CStringPointer("B") <= "B");
  EXPECT_TRUE(CStringPointer("B") <= "C");

  EXPECT_TRUE(CStringPointer("A") <= "BA");
  EXPECT_TRUE(CStringPointer("B") <= "BA");
  EXPECT_FALSE(CStringPointer("C") <= "BA");
  EXPECT_FALSE(CStringPointer("BA") <= "A");
  EXPECT_FALSE(CStringPointer("BA") <= "B");
  EXPECT_TRUE(CStringPointer("BA") <= "C");

  EXPECT_TRUE(CStringPointer("AA") <= "B");
  EXPECT_FALSE(CStringPointer("BA") <= "B");
  EXPECT_FALSE(CStringPointer("CA") <= "B");
  EXPECT_FALSE(CStringPointer("B") <= "AA");
  EXPECT_TRUE(CStringPointer("B") <= "BA");
  EXPECT_TRUE(CStringPointer("B") <= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, OperatorGreaterThan)
{
  EXPECT_GT(CStringPointer("B"), CStringPointer("A"));
  EXPECT_GT(CStringPointer("AB"), CStringPointer("A"));
  EXPECT_GT(CStringPointer("AB"), CStringPointer(""));

  EXPECT_FALSE(CStringPointer("A") > CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") > CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("C") > CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") > CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("B") > CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") > CStringPointer("C"));

  EXPECT_FALSE(CStringPointer("A") > CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("B") > CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("C") > CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("BA") > CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("BA") > CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("BA") > CStringPointer("C"));

  EXPECT_FALSE(CStringPointer("AA") > CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("BA") > CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("CA") > CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") > CStringPointer("AA"));
  EXPECT_FALSE(CStringPointer("B") > CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("B") > CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThan_string_view_LHS)
{
  EXPECT_GT(std::string_view("B"), CStringPointer("A"));
  EXPECT_GT(std::string_view("AB"), CStringPointer("A"));
  EXPECT_GT(std::string_view("AB"), CStringPointer(""));

  EXPECT_FALSE(std::string_view("A") > CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") > CStringPointer("B"));
  EXPECT_TRUE(std::string_view("C") > CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") > CStringPointer("A"));
  EXPECT_FALSE(std::string_view("B") > CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") > CStringPointer("C"));

  EXPECT_FALSE(std::string_view("A") > CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("B") > CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("C") > CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("BA") > CStringPointer("A"));
  EXPECT_TRUE(std::string_view("BA") > CStringPointer("B"));
  EXPECT_FALSE(std::string_view("BA") > CStringPointer("C"));

  EXPECT_FALSE(std::string_view("AA") > CStringPointer("B"));
  EXPECT_TRUE(std::string_view("BA") > CStringPointer("B"));
  EXPECT_TRUE(std::string_view("CA") > CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") > CStringPointer("AA"));
  EXPECT_FALSE(std::string_view("B") > CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("B") > CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThan_string_view_RHS)
{
  EXPECT_GT(CStringPointer("B"), std::string_view("A"));
  EXPECT_GT(CStringPointer("AB"), std::string_view("A"));
  EXPECT_GT(CStringPointer("AB"), std::string_view(""));

  EXPECT_FALSE(CStringPointer("A") > std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") > std::string_view("B"));
  EXPECT_TRUE(CStringPointer("C") > std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") > std::string_view("A"));
  EXPECT_FALSE(CStringPointer("B") > std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") > std::string_view("C"));

  EXPECT_FALSE(CStringPointer("A") > std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("B") > std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("C") > std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("BA") > std::string_view("A"));
  EXPECT_TRUE(CStringPointer("BA") > std::string_view("B"));
  EXPECT_FALSE(CStringPointer("BA") > std::string_view("C"));

  EXPECT_FALSE(CStringPointer("AA") > std::string_view("B"));
  EXPECT_TRUE(CStringPointer("BA") > std::string_view("B"));
  EXPECT_TRUE(CStringPointer("CA") > std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") > std::string_view("AA"));
  EXPECT_FALSE(CStringPointer("B") > std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("B") > std::string_view("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThan_CString_LHS)
{
  EXPECT_GT("B", CStringPointer("A"));
  EXPECT_GT("AB", CStringPointer("A"));
  EXPECT_GT("AB", CStringPointer(""));

  EXPECT_FALSE("A" > CStringPointer("B"));
  EXPECT_FALSE("B" > CStringPointer("B"));
  EXPECT_TRUE("C" > CStringPointer("B"));
  EXPECT_TRUE("B" > CStringPointer("A"));
  EXPECT_FALSE("B" > CStringPointer("B"));
  EXPECT_FALSE("B" > CStringPointer("C"));

  EXPECT_FALSE("A" > CStringPointer("BA"));
  EXPECT_FALSE("B" > CStringPointer("BA"));
  EXPECT_TRUE("C" > CStringPointer("BA"));
  EXPECT_TRUE("BA" > CStringPointer("A"));
  EXPECT_TRUE("BA" > CStringPointer("B"));
  EXPECT_FALSE("BA" > CStringPointer("C"));

  EXPECT_FALSE("AA" > CStringPointer("B"));
  EXPECT_TRUE("BA" > CStringPointer("B"));
  EXPECT_TRUE("CA" > CStringPointer("B"));
  EXPECT_TRUE("B" > CStringPointer("AA"));
  EXPECT_FALSE("B" > CStringPointer("BA"));
  EXPECT_FALSE("B" > CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThan_CString_RHS)
{
  EXPECT_GT(CStringPointer("B"), "A");
  EXPECT_GT(CStringPointer("AB"), "A");
  EXPECT_GT(CStringPointer("AB"), "");

  EXPECT_FALSE(CStringPointer("A") > "B");
  EXPECT_FALSE(CStringPointer("B") > "B");
  EXPECT_TRUE(CStringPointer("C") > "B");
  EXPECT_TRUE(CStringPointer("B") > "A");
  EXPECT_FALSE(CStringPointer("B") > "B");
  EXPECT_FALSE(CStringPointer("B") > "C");

  EXPECT_FALSE(CStringPointer("A") > "BA");
  EXPECT_FALSE(CStringPointer("B") > "BA");
  EXPECT_TRUE(CStringPointer("C") > "BA");
  EXPECT_TRUE(CStringPointer("BA") > "A");
  EXPECT_TRUE(CStringPointer("BA") > "B");
  EXPECT_FALSE(CStringPointer("BA") > "C");

  EXPECT_FALSE(CStringPointer("AA") > "B");
  EXPECT_TRUE(CStringPointer("BA") > "B");
  EXPECT_TRUE(CStringPointer("CA") > "B");
  EXPECT_TRUE(CStringPointer("B") > "AA");
  EXPECT_FALSE(CStringPointer("B") > "BA");
  EXPECT_FALSE(CStringPointer("B") > "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, OperatorGreaterThanOrEqual)
{
  EXPECT_GE(CStringPointer("B"), CStringPointer("A"));
  EXPECT_GE(CStringPointer("AB"), CStringPointer("A"));
  EXPECT_GE(CStringPointer("AB"), CStringPointer(""));
  EXPECT_GE(CStringPointer("A"), CStringPointer("A"));

  EXPECT_FALSE(CStringPointer("A") >= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") >= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("C") >= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") >= CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("B") >= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") >= CStringPointer("C"));

  EXPECT_FALSE(CStringPointer("A") >= CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("B") >= CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("C") >= CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("BA") >= CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("BA") >= CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("BA") >= CStringPointer("C"));

  EXPECT_FALSE(CStringPointer("AA") >= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("BA") >= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("CA") >= CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") >= CStringPointer("AA"));
  EXPECT_FALSE(CStringPointer("B") >= CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("B") >= CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThanOrEqual_string_view_LHS)
{
  EXPECT_GE(std::string_view("B"), CStringPointer("A"));
  EXPECT_GE(std::string_view("AB"), CStringPointer("A"));
  EXPECT_GE(std::string_view("AB"), CStringPointer(""));
  EXPECT_GE(std::string_view("A"), CStringPointer("A"));

  EXPECT_FALSE(std::string_view("A") >= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") >= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("C") >= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") >= CStringPointer("A"));
  EXPECT_TRUE(std::string_view("B") >= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") >= CStringPointer("C"));

  EXPECT_FALSE(std::string_view("A") >= CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("B") >= CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("C") >= CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("BA") >= CStringPointer("A"));
  EXPECT_TRUE(std::string_view("BA") >= CStringPointer("B"));
  EXPECT_FALSE(std::string_view("BA") >= CStringPointer("C"));

  EXPECT_FALSE(std::string_view("AA") >= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("BA") >= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("CA") >= CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") >= CStringPointer("AA"));
  EXPECT_FALSE(std::string_view("B") >= CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("B") >= CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThanOrEqual_string_view_RHS)
{
  EXPECT_GE(CStringPointer("B"), std::string_view("A"));
  EXPECT_GE(CStringPointer("AB"), std::string_view("A"));
  EXPECT_GE(CStringPointer("AB"), std::string_view(""));
  EXPECT_GE(CStringPointer("A"), std::string_view("A"));

  EXPECT_FALSE(CStringPointer("A") >= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") >= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("C") >= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") >= std::string_view("A"));
  EXPECT_TRUE(CStringPointer("B") >= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") >= std::string_view("C"));

  EXPECT_FALSE(CStringPointer("A") >= std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("B") >= std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("C") >= std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("BA") >= std::string_view("A"));
  EXPECT_TRUE(CStringPointer("BA") >= std::string_view("B"));
  EXPECT_FALSE(CStringPointer("BA") >= std::string_view("C"));

  EXPECT_FALSE(CStringPointer("AA") >= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("BA") >= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("CA") >= std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") >= std::string_view("AA"));
  EXPECT_FALSE(CStringPointer("B") >= std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("B") >= std::string_view("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThanOrEqual_CString_LHS)
{
  EXPECT_GE("B", CStringPointer("A"));
  EXPECT_GE("AB", CStringPointer("A"));
  EXPECT_GE("AB", CStringPointer(""));
  EXPECT_GE("A", CStringPointer("A"));

  EXPECT_FALSE("A" >= CStringPointer("B"));
  EXPECT_TRUE("B" >= CStringPointer("B"));
  EXPECT_TRUE("C" >= CStringPointer("B"));
  EXPECT_TRUE("B" >= CStringPointer("A"));
  EXPECT_TRUE("B" >= CStringPointer("B"));
  EXPECT_FALSE("B" >= CStringPointer("C"));

  EXPECT_FALSE("A" >= CStringPointer("BA"));
  EXPECT_FALSE("B" >= CStringPointer("BA"));
  EXPECT_TRUE("C" >= CStringPointer("BA"));
  EXPECT_TRUE("BA" >= CStringPointer("A"));
  EXPECT_TRUE("BA" >= CStringPointer("B"));
  EXPECT_FALSE("BA" >= CStringPointer("C"));

  EXPECT_FALSE("AA" >= CStringPointer("B"));
  EXPECT_TRUE("BA" >= CStringPointer("B"));
  EXPECT_TRUE("CA" >= CStringPointer("B"));
  EXPECT_TRUE("B" >= CStringPointer("AA"));
  EXPECT_FALSE("B" >= CStringPointer("BA"));
  EXPECT_FALSE("B" >= CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorGreaterThanOrEqual_CString_RHS)
{
  EXPECT_GE(CStringPointer("B"), "A");
  EXPECT_GE(CStringPointer("AB"), "A");
  EXPECT_GE(CStringPointer("AB"), "");
  EXPECT_GE(CStringPointer("A"), "A");

  EXPECT_FALSE(CStringPointer("A") >= "B");
  EXPECT_TRUE(CStringPointer("B") >= "B");
  EXPECT_TRUE(CStringPointer("C") >= "B");
  EXPECT_TRUE(CStringPointer("B") >= "A");
  EXPECT_TRUE(CStringPointer("B") >= "B");
  EXPECT_FALSE(CStringPointer("B") >= "C");

  EXPECT_FALSE(CStringPointer("A") >= "BA");
  EXPECT_FALSE(CStringPointer("B") >= "BA");
  EXPECT_TRUE(CStringPointer("C") >= "BA");
  EXPECT_TRUE(CStringPointer("BA") >= "A");
  EXPECT_TRUE(CStringPointer("BA") >= "B");
  EXPECT_FALSE(CStringPointer("BA") >= "C");

  EXPECT_FALSE(CStringPointer("AA") >= "B");
  EXPECT_TRUE(CStringPointer("BA") >= "B");
  EXPECT_TRUE(CStringPointer("CA") >= "B");
  EXPECT_TRUE(CStringPointer("B") >= "AA");
  EXPECT_FALSE(CStringPointer("B") >= "BA");
  EXPECT_FALSE(CStringPointer("B") >= "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_CStringPointer, OperatorEqual)
{
  EXPECT_TRUE(CStringPointer("A") == CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("") == CStringPointer(""));
  EXPECT_FALSE(CStringPointer("A") == CStringPointer(""));
  EXPECT_FALSE(CStringPointer("A") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("A") == CStringPointer("a"));

  EXPECT_FALSE(CStringPointer("A") == CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("C") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") == CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("B") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") == CStringPointer("C"));

  EXPECT_FALSE(CStringPointer("A") == CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("B") == CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("C") == CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("BA") == CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("BA") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("BA") == CStringPointer("C"));

  EXPECT_FALSE(CStringPointer("AA") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("BA") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("CA") == CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") == CStringPointer("AA"));
  EXPECT_FALSE(CStringPointer("B") == CStringPointer("BA"));
  EXPECT_FALSE(CStringPointer("B") == CStringPointer("CA"));
}


TEST(TestString_CStringPointer, OperatorEqual_string_view_LHS)
{
  EXPECT_TRUE(std::string_view("A") == CStringPointer("A"));
  EXPECT_TRUE(std::string_view("") == CStringPointer(""));
  EXPECT_FALSE(std::string_view("A") == CStringPointer(""));
  EXPECT_FALSE(std::string_view("A") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("A") == CStringPointer("a"));

  EXPECT_FALSE(std::string_view("A") == CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("C") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") == CStringPointer("A"));
  EXPECT_TRUE(std::string_view("B") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") == CStringPointer("C"));

  EXPECT_FALSE(std::string_view("A") == CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("B") == CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("C") == CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("BA") == CStringPointer("A"));
  EXPECT_FALSE(std::string_view("BA") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("BA") == CStringPointer("C"));

  EXPECT_FALSE(std::string_view("AA") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("BA") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("CA") == CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") == CStringPointer("AA"));
  EXPECT_FALSE(std::string_view("B") == CStringPointer("BA"));
  EXPECT_FALSE(std::string_view("B") == CStringPointer("CA"));
}


TEST(TestString_CStringPointer, OperatorEqual_string_view_RHS)
{
  EXPECT_TRUE(CStringPointer("A") == std::string_view("A"));
  EXPECT_TRUE(CStringPointer("") == std::string_view(""));
  EXPECT_FALSE(CStringPointer("A") == std::string_view(""));
  EXPECT_FALSE(CStringPointer("A") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("A") == std::string_view("a"));

  EXPECT_FALSE(CStringPointer("A") == std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("C") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") == std::string_view("A"));
  EXPECT_TRUE(CStringPointer("B") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") == std::string_view("C"));

  EXPECT_FALSE(CStringPointer("A") == std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("B") == std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("C") == std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("BA") == std::string_view("A"));
  EXPECT_FALSE(CStringPointer("BA") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("BA") == std::string_view("C"));

  EXPECT_FALSE(CStringPointer("AA") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("BA") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("CA") == std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") == std::string_view("AA"));
  EXPECT_FALSE(CStringPointer("B") == std::string_view("BA"));
  EXPECT_FALSE(CStringPointer("B") == std::string_view("CA"));
}

TEST(TestString_CStringPointer, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE("A" == CStringPointer("A"));
  EXPECT_TRUE("" == CStringPointer(""));    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE("A" == CStringPointer(""));
  EXPECT_FALSE("A" == CStringPointer("B"));
  EXPECT_FALSE("A" == CStringPointer("a"));

  EXPECT_FALSE("A" == CStringPointer("B"));
  EXPECT_TRUE("B" == CStringPointer("B"));
  EXPECT_FALSE("C" == CStringPointer("B"));
  EXPECT_FALSE("B" == CStringPointer("A"));
  EXPECT_TRUE("B" == CStringPointer("B"));
  EXPECT_FALSE("B" == CStringPointer("C"));

  EXPECT_FALSE("A" == CStringPointer("BA"));
  EXPECT_FALSE("B" == CStringPointer("BA"));
  EXPECT_FALSE("C" == CStringPointer("BA"));
  EXPECT_FALSE("BA" == CStringPointer("A"));
  EXPECT_FALSE("BA" == CStringPointer("B"));
  EXPECT_FALSE("BA" == CStringPointer("C"));

  EXPECT_FALSE("AA" == CStringPointer("B"));
  EXPECT_FALSE("BA" == CStringPointer("B"));
  EXPECT_FALSE("CA" == CStringPointer("B"));
  EXPECT_FALSE("B" == CStringPointer("AA"));
  EXPECT_FALSE("B" == CStringPointer("BA"));
  EXPECT_FALSE("B" == CStringPointer("CA"));
}


TEST(TestString_CStringPointer, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(CStringPointer("A") == "A");
  EXPECT_TRUE(CStringPointer("") == "");      // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(CStringPointer("A") == "");    // NOLINT(readability-container-size-empty)
  EXPECT_FALSE(CStringPointer("A") == "B");
  EXPECT_FALSE(CStringPointer("A") == "a");

  EXPECT_FALSE(CStringPointer("A") == "B");
  EXPECT_TRUE(CStringPointer("B") == "B");
  EXPECT_FALSE(CStringPointer("C") == "B");
  EXPECT_FALSE(CStringPointer("B") == "A");
  EXPECT_TRUE(CStringPointer("B") == "B");
  EXPECT_FALSE(CStringPointer("B") == "C");

  EXPECT_FALSE(CStringPointer("A") == "BA");
  EXPECT_FALSE(CStringPointer("B") == "BA");
  EXPECT_FALSE(CStringPointer("C") == "BA");
  EXPECT_FALSE(CStringPointer("BA") == "A");
  EXPECT_FALSE(CStringPointer("BA") == "B");
  EXPECT_FALSE(CStringPointer("BA") == "C");

  EXPECT_FALSE(CStringPointer("AA") == "B");
  EXPECT_FALSE(CStringPointer("BA") == "B");
  EXPECT_FALSE(CStringPointer("CA") == "B");
  EXPECT_FALSE(CStringPointer("B") == "AA");
  EXPECT_FALSE(CStringPointer("B") == "BA");
  EXPECT_FALSE(CStringPointer("B") == "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, OperatorNotEqual)
{
  EXPECT_FALSE(CStringPointer("A") != CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("") != CStringPointer(""));
  EXPECT_TRUE(CStringPointer("A") != CStringPointer(""));
  EXPECT_TRUE(CStringPointer("A") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("A") != CStringPointer("a"));

  EXPECT_TRUE(CStringPointer("A") != CStringPointer("B"));
  EXPECT_FALSE(CStringPointer("B") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("C") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") != CStringPointer("A"));
  EXPECT_FALSE(CStringPointer("B") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") != CStringPointer("C"));

  EXPECT_TRUE(CStringPointer("A") != CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("B") != CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("C") != CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("BA") != CStringPointer("A"));
  EXPECT_TRUE(CStringPointer("BA") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("BA") != CStringPointer("C"));

  EXPECT_TRUE(CStringPointer("AA") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("BA") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("CA") != CStringPointer("B"));
  EXPECT_TRUE(CStringPointer("B") != CStringPointer("AA"));
  EXPECT_TRUE(CStringPointer("B") != CStringPointer("BA"));
  EXPECT_TRUE(CStringPointer("B") != CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorNotEqual_string_view_LHS)
{
  EXPECT_FALSE(std::string_view("A") != CStringPointer("A"));
  EXPECT_FALSE(std::string_view("") != CStringPointer(""));
  EXPECT_TRUE(std::string_view("A") != CStringPointer(""));
  EXPECT_TRUE(std::string_view("A") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("A") != CStringPointer("a"));

  EXPECT_TRUE(std::string_view("A") != CStringPointer("B"));
  EXPECT_FALSE(std::string_view("B") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("C") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") != CStringPointer("A"));
  EXPECT_FALSE(std::string_view("B") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") != CStringPointer("C"));

  EXPECT_TRUE(std::string_view("A") != CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("B") != CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("C") != CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("BA") != CStringPointer("A"));
  EXPECT_TRUE(std::string_view("BA") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("BA") != CStringPointer("C"));

  EXPECT_TRUE(std::string_view("AA") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("BA") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("CA") != CStringPointer("B"));
  EXPECT_TRUE(std::string_view("B") != CStringPointer("AA"));
  EXPECT_TRUE(std::string_view("B") != CStringPointer("BA"));
  EXPECT_TRUE(std::string_view("B") != CStringPointer("CA"));
}

TEST(TestString_CStringPointer, OperatorNotEqual_string_view_RHS)
{
  EXPECT_FALSE(CStringPointer("A") != std::string_view("A"));
  EXPECT_FALSE(CStringPointer("") != std::string_view(""));
  EXPECT_TRUE(CStringPointer("A") != std::string_view(""));
  EXPECT_TRUE(CStringPointer("A") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("A") != std::string_view("a"));

  EXPECT_TRUE(CStringPointer("A") != std::string_view("B"));
  EXPECT_FALSE(CStringPointer("B") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("C") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") != std::string_view("A"));
  EXPECT_FALSE(CStringPointer("B") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") != std::string_view("C"));

  EXPECT_TRUE(CStringPointer("A") != std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("B") != std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("C") != std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("BA") != std::string_view("A"));
  EXPECT_TRUE(CStringPointer("BA") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("BA") != std::string_view("C"));

  EXPECT_TRUE(CStringPointer("AA") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("BA") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("CA") != std::string_view("B"));
  EXPECT_TRUE(CStringPointer("B") != std::string_view("AA"));
  EXPECT_TRUE(CStringPointer("B") != std::string_view("BA"));
  EXPECT_TRUE(CStringPointer("B") != std::string_view("CA"));
}

TEST(TestString_CStringPointer, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE("A" != CStringPointer("A"));
  EXPECT_FALSE("" != CStringPointer(""));    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE("A" != CStringPointer(""));
  EXPECT_TRUE("A" != CStringPointer("B"));
  EXPECT_TRUE("A" != CStringPointer("a"));

  EXPECT_TRUE("A" != CStringPointer("B"));
  EXPECT_FALSE("B" != CStringPointer("B"));
  EXPECT_TRUE("C" != CStringPointer("B"));
  EXPECT_TRUE("B" != CStringPointer("A"));
  EXPECT_FALSE("B" != CStringPointer("B"));
  EXPECT_TRUE("B" != CStringPointer("C"));

  EXPECT_TRUE("A" != CStringPointer("BA"));
  EXPECT_TRUE("B" != CStringPointer("BA"));
  EXPECT_TRUE("C" != CStringPointer("BA"));
  EXPECT_TRUE("BA" != CStringPointer("A"));
  EXPECT_TRUE("BA" != CStringPointer("B"));
  EXPECT_TRUE("BA" != CStringPointer("C"));

  EXPECT_TRUE("AA" != CStringPointer("B"));
  EXPECT_TRUE("BA" != CStringPointer("B"));
  EXPECT_TRUE("CA" != CStringPointer("B"));
  EXPECT_TRUE("B" != CStringPointer("AA"));
  EXPECT_TRUE("B" != CStringPointer("BA"));
  EXPECT_TRUE("B" != CStringPointer("CA"));
}


TEST(TestString_CStringPointer, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(CStringPointer("A") != "A");
  EXPECT_FALSE(CStringPointer("") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(CStringPointer("A") != "");    // NOLINT(readability-container-size-empty)
  EXPECT_TRUE(CStringPointer("A") != "B");
  EXPECT_TRUE(CStringPointer("A") != "a");

  EXPECT_TRUE(CStringPointer("A") != "B");
  EXPECT_FALSE(CStringPointer("B") != "B");
  EXPECT_TRUE(CStringPointer("C") != "B");
  EXPECT_TRUE(CStringPointer("B") != "A");
  EXPECT_FALSE(CStringPointer("B") != "B");
  EXPECT_TRUE(CStringPointer("B") != "C");

  EXPECT_TRUE(CStringPointer("A") != "BA");
  EXPECT_TRUE(CStringPointer("B") != "BA");
  EXPECT_TRUE(CStringPointer("C") != "BA");
  EXPECT_TRUE(CStringPointer("BA") != "A");
  EXPECT_TRUE(CStringPointer("BA") != "B");
  EXPECT_TRUE(CStringPointer("BA") != "C");

  EXPECT_TRUE(CStringPointer("AA") != "B");
  EXPECT_TRUE(CStringPointer("BA") != "B");
  EXPECT_TRUE(CStringPointer("CA") != "B");
  EXPECT_TRUE(CStringPointer("B") != "AA");
  EXPECT_TRUE(CStringPointer("B") != "BA");
  EXPECT_TRUE(CStringPointer("B") != "CA");
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator null handling
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringPointer, OperatorLessThan_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringPointer StrViewNull(nullptr);

  EXPECT_FALSE(CStringPointer("A") < pszNull);
  EXPECT_FALSE(CStringPointer("A") < StrViewNull);
  EXPECT_TRUE(pszNull < CStringPointer("A"));
  EXPECT_TRUE(StrViewNull < CStringPointer("A"));

  // Compare against empty string
  EXPECT_FALSE(CStringPointer("") < pszNull);
  EXPECT_FALSE(CStringPointer("") < StrViewNull);
  EXPECT_FALSE(pszNull < CStringPointer(""));
  EXPECT_FALSE(StrViewNull < CStringPointer(""));

  // Compare against null
  EXPECT_FALSE(pszNull < StrViewNull);
  EXPECT_FALSE(StrViewNull < pszNull);
  EXPECT_FALSE(StrViewNull < StrViewNull);
}


TEST(TestString_CStringPointer, OperatorLessThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringPointer StrViewNull(nullptr);

  EXPECT_FALSE(CStringPointer("A") <= pszNull);
  EXPECT_FALSE(CStringPointer("A") <= StrViewNull);
  EXPECT_TRUE(pszNull <= CStringPointer("A"));
  EXPECT_TRUE(StrViewNull <= CStringPointer("A"));

  // Compare against empty string
  EXPECT_TRUE(CStringPointer("") <= pszNull);
  EXPECT_TRUE(CStringPointer("") <= StrViewNull);
  EXPECT_TRUE(pszNull <= CStringPointer(""));
  EXPECT_TRUE(StrViewNull <= CStringPointer(""));

  // Compare against null
  EXPECT_TRUE(pszNull <= StrViewNull);
  EXPECT_TRUE(StrViewNull <= pszNull);
  EXPECT_TRUE(StrViewNull <= StrViewNull);
}


TEST(TestString_CStringPointer, OperatorGreaterThan_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringPointer StrViewNull(nullptr);

  EXPECT_TRUE(CStringPointer("A") > pszNull);
  EXPECT_TRUE(CStringPointer("A") > StrViewNull);
  EXPECT_FALSE(pszNull > CStringPointer("A"));
  EXPECT_FALSE(StrViewNull > CStringPointer("A"));

  // Compare against empty string
  EXPECT_FALSE(CStringPointer("") > pszNull);
  EXPECT_FALSE(CStringPointer("") > StrViewNull);
  EXPECT_FALSE(pszNull > CStringPointer(""));
  EXPECT_FALSE(StrViewNull > CStringPointer(""));

  // Compare against null
  EXPECT_FALSE(pszNull > StrViewNull);
  EXPECT_FALSE(StrViewNull > pszNull);
  EXPECT_FALSE(StrViewNull > StrViewNull);
}


TEST(TestString_CStringPointer, OperatorGreaterThanOrEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringPointer StrViewNull(nullptr);

  EXPECT_TRUE(CStringPointer("A") >= pszNull);
  EXPECT_TRUE(CStringPointer("A") >= StrViewNull);
  EXPECT_FALSE(pszNull >= CStringPointer("A"));
  EXPECT_FALSE(StrViewNull >= CStringPointer("A"));

  // Compare against empty string
  EXPECT_TRUE(CStringPointer("") >= pszNull);
  EXPECT_TRUE(CStringPointer("") >= StrViewNull);
  EXPECT_TRUE(pszNull >= CStringPointer(""));
  EXPECT_TRUE(StrViewNull >= CStringPointer(""));

  // Compare against null
  EXPECT_TRUE(pszNull >= StrViewNull);
  EXPECT_TRUE(StrViewNull >= pszNull);
  EXPECT_TRUE(StrViewNull >= StrViewNull);
}


TEST(TestString_CStringPointer, OperatorEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringPointer StrViewNull(nullptr);

  EXPECT_FALSE(CStringPointer("A") == pszNull);
  EXPECT_FALSE(CStringPointer("A") == StrViewNull);
  EXPECT_FALSE(pszNull == CStringPointer("A"));
  EXPECT_FALSE(StrViewNull == CStringPointer("A"));

  // Compare against empty string
  EXPECT_TRUE(pszNull == CStringPointer(""));
  EXPECT_TRUE(StrViewNull == CStringPointer(""));
  EXPECT_TRUE(CStringPointer("") == pszNull);
  EXPECT_TRUE(CStringPointer("") == StrViewNull);

  // Compare against null
  EXPECT_TRUE(pszNull == StrViewNull);
  EXPECT_TRUE(StrViewNull == pszNull);
  EXPECT_TRUE(StrViewNull == StrViewNull);
}


TEST(TestString_CStringPointer, OperatorNotEqual_Null)
{
  const char* pszNull = nullptr;
  constexpr CStringPointer StrViewNull(nullptr);

  EXPECT_TRUE(CStringPointer("A") != pszNull);
  EXPECT_TRUE(CStringPointer("A") != StrViewNull);
  EXPECT_TRUE(pszNull != CStringPointer("A"));
  EXPECT_TRUE(StrViewNull != CStringPointer("A"));

  // Compare against empty string
  EXPECT_FALSE(pszNull != CStringPointer(""));
  EXPECT_FALSE(StrViewNull != CStringPointer(""));
  EXPECT_FALSE(CStringPointer("") != pszNull);
  EXPECT_FALSE(CStringPointer("") != StrViewNull);

  // Compare against null
  EXPECT_FALSE(pszNull != StrViewNull);
  EXPECT_FALSE(StrViewNull != pszNull);
  EXPECT_FALSE(StrViewNull != StrViewNull);
}
