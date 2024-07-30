/****************************************************************************************************************************************************
 * Copyright 2019-2024 NXP
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
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using Test_StringViewLite_Compare = TestFixtureFslBase;
}


TEST(Test_StringViewLite_Compare, Construct)
{
  std::string empty;
  StringViewLite strView(empty);

  EXPECT_TRUE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(Test_StringViewLite_Compare, Construct_FromStr)
{
  std::string str("Hello world");
  StringViewLite strView(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, ToString_Empty)
{
  StringViewLite empty;
  std::string dstStr(empty);

  EXPECT_TRUE(dstStr.empty());
  EXPECT_EQ(0u, dstStr.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, ToString)
{
  StringViewLite str("hello");
  std::string dstStr(str);

  EXPECT_EQ(str, dstStr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, Set_String_Empty)
{
  StringViewLite empty;
  std::string dstStr;
  dstStr = empty;

  EXPECT_TRUE(dstStr.empty());
  EXPECT_EQ(0u, dstStr.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, Set_String)
{
  StringViewLite str("hello");
  std::string dstStr;
  dstStr = str;

  EXPECT_EQ(str, dstStr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, OperatorLessThan_String_LHS)
{
  EXPECT_LT(std::string("A"), StringViewLite("B"));
  EXPECT_LT(std::string("A"), StringViewLite("AB"));
  EXPECT_LT(std::string(""), StringViewLite("AB"));

  EXPECT_TRUE(std::string("A") < StringViewLite("B"));
  EXPECT_FALSE(std::string("B") < StringViewLite("B"));
  EXPECT_FALSE(std::string("C") < StringViewLite("B"));
  EXPECT_FALSE(std::string("B") < StringViewLite("A"));
  EXPECT_FALSE(std::string("B") < StringViewLite("B"));
  EXPECT_TRUE(std::string("B") < StringViewLite("C"));

  EXPECT_TRUE(std::string("A") < StringViewLite("BA"));
  EXPECT_TRUE(std::string("B") < StringViewLite("BA"));
  EXPECT_FALSE(std::string("C") < StringViewLite("BA"));
  EXPECT_FALSE(std::string("BA") < StringViewLite("A"));
  EXPECT_FALSE(std::string("BA") < StringViewLite("B"));
  EXPECT_TRUE(std::string("BA") < StringViewLite("C"));

  EXPECT_TRUE(std::string("AA") < StringViewLite("B"));
  EXPECT_FALSE(std::string("BA") < StringViewLite("B"));
  EXPECT_FALSE(std::string("CA") < StringViewLite("B"));
  EXPECT_FALSE(std::string("B") < StringViewLite("AA"));
  EXPECT_TRUE(std::string("B") < StringViewLite("BA"));
  EXPECT_TRUE(std::string("B") < StringViewLite("CA"));
}

TEST(Test_StringViewLite_Compare, OperatorLessThan_String_RHS)
{
  EXPECT_LT(StringViewLite("A"), std::string("B"));
  EXPECT_LT(StringViewLite("A"), std::string("AB"));
  EXPECT_LT(StringViewLite(""), std::string("AB"));

  EXPECT_TRUE(StringViewLite("A") < std::string("B"));
  EXPECT_FALSE(StringViewLite("B") < std::string("B"));
  EXPECT_FALSE(StringViewLite("C") < std::string("B"));
  EXPECT_FALSE(StringViewLite("B") < std::string("A"));
  EXPECT_FALSE(StringViewLite("B") < std::string("B"));
  EXPECT_TRUE(StringViewLite("B") < std::string("C"));

  EXPECT_TRUE(StringViewLite("A") < std::string("BA"));
  EXPECT_TRUE(StringViewLite("B") < std::string("BA"));
  EXPECT_FALSE(StringViewLite("C") < std::string("BA"));
  EXPECT_FALSE(StringViewLite("BA") < std::string("A"));
  EXPECT_FALSE(StringViewLite("BA") < std::string("B"));
  EXPECT_TRUE(StringViewLite("BA") < std::string("C"));

  EXPECT_TRUE(StringViewLite("AA") < std::string("B"));
  EXPECT_FALSE(StringViewLite("BA") < std::string("B"));
  EXPECT_FALSE(StringViewLite("CA") < std::string("B"));
  EXPECT_FALSE(StringViewLite("B") < std::string("AA"));
  EXPECT_TRUE(StringViewLite("B") < std::string("BA"));
  EXPECT_TRUE(StringViewLite("B") < std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, OperatorLessThanOrEqual_String_LHS)
{
  EXPECT_LE(std::string("A"), StringViewLite("B"));
  EXPECT_LE(std::string("A"), StringViewLite("AB"));
  EXPECT_LE(std::string(""), StringViewLite("AB"));
  EXPECT_LE(std::string("A"), StringViewLite("A"));

  EXPECT_TRUE(std::string("A") <= StringViewLite("B"));
  EXPECT_TRUE(std::string("B") <= StringViewLite("B"));
  EXPECT_FALSE(std::string("C") <= StringViewLite("B"));
  EXPECT_FALSE(std::string("B") <= StringViewLite("A"));
  EXPECT_TRUE(std::string("B") <= StringViewLite("B"));
  EXPECT_TRUE(std::string("B") <= StringViewLite("C"));

  EXPECT_TRUE(std::string("A") <= StringViewLite("BA"));
  EXPECT_TRUE(std::string("B") <= StringViewLite("BA"));
  EXPECT_FALSE(std::string("C") <= StringViewLite("BA"));
  EXPECT_FALSE(std::string("BA") <= StringViewLite("A"));
  EXPECT_FALSE(std::string("BA") <= StringViewLite("B"));
  EXPECT_TRUE(std::string("BA") <= StringViewLite("C"));

  EXPECT_TRUE(std::string("AA") <= StringViewLite("B"));
  EXPECT_FALSE(std::string("BA") <= StringViewLite("B"));
  EXPECT_FALSE(std::string("CA") <= StringViewLite("B"));
  EXPECT_FALSE(std::string("B") <= StringViewLite("AA"));
  EXPECT_TRUE(std::string("B") <= StringViewLite("BA"));
  EXPECT_TRUE(std::string("B") <= StringViewLite("CA"));
}

TEST(Test_StringViewLite_Compare, OperatorLessThanOrEqual_String_RHS)
{
  EXPECT_LE(StringViewLite("A"), std::string("B"));
  EXPECT_LE(StringViewLite("A"), std::string("AB"));
  EXPECT_LE(StringViewLite(""), std::string("AB"));
  EXPECT_LE(StringViewLite("A"), std::string("A"));

  EXPECT_TRUE(StringViewLite("A") <= std::string("B"));
  EXPECT_TRUE(StringViewLite("B") <= std::string("B"));
  EXPECT_FALSE(StringViewLite("C") <= std::string("B"));
  EXPECT_FALSE(StringViewLite("B") <= std::string("A"));
  EXPECT_TRUE(StringViewLite("B") <= std::string("B"));
  EXPECT_TRUE(StringViewLite("B") <= std::string("C"));

  EXPECT_TRUE(StringViewLite("A") <= std::string("BA"));
  EXPECT_TRUE(StringViewLite("B") <= std::string("BA"));
  EXPECT_FALSE(StringViewLite("C") <= std::string("BA"));
  EXPECT_FALSE(StringViewLite("BA") <= std::string("A"));
  EXPECT_FALSE(StringViewLite("BA") <= std::string("B"));
  EXPECT_TRUE(StringViewLite("BA") <= std::string("C"));

  EXPECT_TRUE(StringViewLite("AA") <= std::string("B"));
  EXPECT_FALSE(StringViewLite("BA") <= std::string("B"));
  EXPECT_FALSE(StringViewLite("CA") <= std::string("B"));
  EXPECT_FALSE(StringViewLite("B") <= std::string("AA"));
  EXPECT_TRUE(StringViewLite("B") <= std::string("BA"));
  EXPECT_TRUE(StringViewLite("B") <= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, OperatorGreaterThan_String_LHS)
{
  EXPECT_GT(std::string("B"), StringViewLite("A"));
  EXPECT_GT(std::string("AB"), StringViewLite("A"));
  EXPECT_GT(std::string("AB"), StringViewLite(""));

  EXPECT_FALSE(std::string("A") > StringViewLite("B"));
  EXPECT_FALSE(std::string("B") > StringViewLite("B"));
  EXPECT_TRUE(std::string("C") > StringViewLite("B"));
  EXPECT_TRUE(std::string("B") > StringViewLite("A"));
  EXPECT_FALSE(std::string("B") > StringViewLite("B"));
  EXPECT_FALSE(std::string("B") > StringViewLite("C"));

  EXPECT_FALSE(std::string("A") > StringViewLite("BA"));
  EXPECT_FALSE(std::string("B") > StringViewLite("BA"));
  EXPECT_TRUE(std::string("C") > StringViewLite("BA"));
  EXPECT_TRUE(std::string("BA") > StringViewLite("A"));
  EXPECT_TRUE(std::string("BA") > StringViewLite("B"));
  EXPECT_FALSE(std::string("BA") > StringViewLite("C"));

  EXPECT_FALSE(std::string("AA") > StringViewLite("B"));
  EXPECT_TRUE(std::string("BA") > StringViewLite("B"));
  EXPECT_TRUE(std::string("CA") > StringViewLite("B"));
  EXPECT_TRUE(std::string("B") > StringViewLite("AA"));
  EXPECT_FALSE(std::string("B") > StringViewLite("BA"));
  EXPECT_FALSE(std::string("B") > StringViewLite("CA"));
}

TEST(Test_StringViewLite_Compare, OperatorGreaterThan_String_RHS)
{
  EXPECT_GT(StringViewLite("B"), std::string("A"));
  EXPECT_GT(StringViewLite("AB"), std::string("A"));
  EXPECT_GT(StringViewLite("AB"), std::string(""));

  EXPECT_FALSE(StringViewLite("A") > std::string("B"));
  EXPECT_FALSE(StringViewLite("B") > std::string("B"));
  EXPECT_TRUE(StringViewLite("C") > std::string("B"));
  EXPECT_TRUE(StringViewLite("B") > std::string("A"));
  EXPECT_FALSE(StringViewLite("B") > std::string("B"));
  EXPECT_FALSE(StringViewLite("B") > std::string("C"));

  EXPECT_FALSE(StringViewLite("A") > std::string("BA"));
  EXPECT_FALSE(StringViewLite("B") > std::string("BA"));
  EXPECT_TRUE(StringViewLite("C") > std::string("BA"));
  EXPECT_TRUE(StringViewLite("BA") > std::string("A"));
  EXPECT_TRUE(StringViewLite("BA") > std::string("B"));
  EXPECT_FALSE(StringViewLite("BA") > std::string("C"));

  EXPECT_FALSE(StringViewLite("AA") > std::string("B"));
  EXPECT_TRUE(StringViewLite("BA") > std::string("B"));
  EXPECT_TRUE(StringViewLite("CA") > std::string("B"));
  EXPECT_TRUE(StringViewLite("B") > std::string("AA"));
  EXPECT_FALSE(StringViewLite("B") > std::string("BA"));
  EXPECT_FALSE(StringViewLite("B") > std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, OperatorGreaterThanOrEqual_String_LHS)
{
  EXPECT_GE(std::string("B"), StringViewLite("A"));
  EXPECT_GE(std::string("AB"), StringViewLite("A"));
  EXPECT_GE(std::string("AB"), StringViewLite(""));
  EXPECT_GE(std::string("A"), StringViewLite("A"));

  EXPECT_FALSE(std::string("A") >= StringViewLite("B"));
  EXPECT_TRUE(std::string("B") >= StringViewLite("B"));
  EXPECT_TRUE(std::string("C") >= StringViewLite("B"));
  EXPECT_TRUE(std::string("B") >= StringViewLite("A"));
  EXPECT_TRUE(std::string("B") >= StringViewLite("B"));
  EXPECT_FALSE(std::string("B") >= StringViewLite("C"));

  EXPECT_FALSE(std::string("A") >= StringViewLite("BA"));
  EXPECT_FALSE(std::string("B") >= StringViewLite("BA"));
  EXPECT_TRUE(std::string("C") >= StringViewLite("BA"));
  EXPECT_TRUE(std::string("BA") >= StringViewLite("A"));
  EXPECT_TRUE(std::string("BA") >= StringViewLite("B"));
  EXPECT_FALSE(std::string("BA") >= StringViewLite("C"));

  EXPECT_FALSE(std::string("AA") >= StringViewLite("B"));
  EXPECT_TRUE(std::string("BA") >= StringViewLite("B"));
  EXPECT_TRUE(std::string("CA") >= StringViewLite("B"));
  EXPECT_TRUE(std::string("B") >= StringViewLite("AA"));
  EXPECT_FALSE(std::string("B") >= StringViewLite("BA"));
  EXPECT_FALSE(std::string("B") >= StringViewLite("CA"));
}

TEST(Test_StringViewLite_Compare, OperatorGreaterThanOrEqual_String_RHS)
{
  EXPECT_GE(StringViewLite("B"), std::string("A"));
  EXPECT_GE(StringViewLite("AB"), std::string("A"));
  EXPECT_GE(StringViewLite("AB"), std::string(""));
  EXPECT_GE(StringViewLite("A"), std::string("A"));

  EXPECT_FALSE(StringViewLite("A") >= std::string("B"));
  EXPECT_TRUE(StringViewLite("B") >= std::string("B"));
  EXPECT_TRUE(StringViewLite("C") >= std::string("B"));
  EXPECT_TRUE(StringViewLite("B") >= std::string("A"));
  EXPECT_TRUE(StringViewLite("B") >= std::string("B"));
  EXPECT_FALSE(StringViewLite("B") >= std::string("C"));

  EXPECT_FALSE(StringViewLite("A") >= std::string("BA"));
  EXPECT_FALSE(StringViewLite("B") >= std::string("BA"));
  EXPECT_TRUE(StringViewLite("C") >= std::string("BA"));
  EXPECT_TRUE(StringViewLite("BA") >= std::string("A"));
  EXPECT_TRUE(StringViewLite("BA") >= std::string("B"));
  EXPECT_FALSE(StringViewLite("BA") >= std::string("C"));

  EXPECT_FALSE(StringViewLite("AA") >= std::string("B"));
  EXPECT_TRUE(StringViewLite("BA") >= std::string("B"));
  EXPECT_TRUE(StringViewLite("CA") >= std::string("B"));
  EXPECT_TRUE(StringViewLite("B") >= std::string("AA"));
  EXPECT_FALSE(StringViewLite("B") >= std::string("BA"));
  EXPECT_FALSE(StringViewLite("B") >= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator ==
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewLite_Compare, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE(std::string("A") == StringViewLite("A"));
  EXPECT_TRUE(std::string("") == StringViewLite(""));
  EXPECT_FALSE(std::string("A") == StringViewLite(""));
  EXPECT_FALSE(std::string("A") == StringViewLite("B"));
  EXPECT_FALSE(std::string("A") == StringViewLite("a"));

  EXPECT_FALSE(std::string("A") == StringViewLite("B"));
  EXPECT_TRUE(std::string("B") == StringViewLite("B"));
  EXPECT_FALSE(std::string("C") == StringViewLite("B"));
  EXPECT_FALSE(std::string("B") == StringViewLite("A"));
  EXPECT_TRUE(std::string("B") == StringViewLite("B"));
  EXPECT_FALSE(std::string("B") == StringViewLite("C"));

  EXPECT_FALSE(std::string("A") == StringViewLite("BA"));
  EXPECT_FALSE(std::string("B") == StringViewLite("BA"));
  EXPECT_FALSE(std::string("C") == StringViewLite("BA"));
  EXPECT_FALSE(std::string("BA") == StringViewLite("A"));
  EXPECT_FALSE(std::string("BA") == StringViewLite("B"));
  EXPECT_FALSE(std::string("BA") == StringViewLite("C"));

  EXPECT_FALSE(std::string("AA") == StringViewLite("B"));
  EXPECT_FALSE(std::string("BA") == StringViewLite("B"));
  EXPECT_FALSE(std::string("CA") == StringViewLite("B"));
  EXPECT_FALSE(std::string("B") == StringViewLite("AA"));
  EXPECT_FALSE(std::string("B") == StringViewLite("BA"));
  EXPECT_FALSE(std::string("B") == StringViewLite("CA"));
}


TEST(Test_StringViewLite_Compare, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(StringViewLite("A") == std::string("A"));
  EXPECT_TRUE(StringViewLite("") == std::string(""));
  EXPECT_FALSE(StringViewLite("A") == std::string(""));
  EXPECT_FALSE(StringViewLite("A") == std::string("B"));
  EXPECT_FALSE(StringViewLite("A") == std::string("a"));

  EXPECT_FALSE(StringViewLite("A") == std::string("B"));
  EXPECT_TRUE(StringViewLite("B") == std::string("B"));
  EXPECT_FALSE(StringViewLite("C") == std::string("B"));
  EXPECT_FALSE(StringViewLite("B") == std::string("A"));
  EXPECT_TRUE(StringViewLite("B") == std::string("B"));
  EXPECT_FALSE(StringViewLite("B") == std::string("C"));

  EXPECT_FALSE(StringViewLite("A") == std::string("BA"));
  EXPECT_FALSE(StringViewLite("B") == std::string("BA"));
  EXPECT_FALSE(StringViewLite("C") == std::string("BA"));
  EXPECT_FALSE(StringViewLite("BA") == std::string("A"));
  EXPECT_FALSE(StringViewLite("BA") == std::string("B"));
  EXPECT_FALSE(StringViewLite("BA") == std::string("C"));

  EXPECT_FALSE(StringViewLite("AA") == std::string("B"));
  EXPECT_FALSE(StringViewLite("BA") == std::string("B"));
  EXPECT_FALSE(StringViewLite("CA") == std::string("B"));
  EXPECT_FALSE(StringViewLite("B") == std::string("AA"));
  EXPECT_FALSE(StringViewLite("B") == std::string("BA"));
  EXPECT_FALSE(StringViewLite("B") == std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_StringViewLite_Compare, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE(std::string("A") != StringViewLite("A"));
  EXPECT_FALSE(std::string("") != StringViewLite(""));
  EXPECT_TRUE(std::string("A") != StringViewLite(""));
  EXPECT_TRUE(std::string("A") != StringViewLite("B"));
  EXPECT_TRUE(std::string("A") != StringViewLite("a"));

  EXPECT_TRUE(std::string("A") != StringViewLite("B"));
  EXPECT_FALSE(std::string("B") != StringViewLite("B"));
  EXPECT_TRUE(std::string("C") != StringViewLite("B"));
  EXPECT_TRUE(std::string("B") != StringViewLite("A"));
  EXPECT_FALSE(std::string("B") != StringViewLite("B"));
  EXPECT_TRUE(std::string("B") != StringViewLite("C"));

  EXPECT_TRUE(std::string("A") != StringViewLite("BA"));
  EXPECT_TRUE(std::string("B") != StringViewLite("BA"));
  EXPECT_TRUE(std::string("C") != StringViewLite("BA"));
  EXPECT_TRUE(std::string("BA") != StringViewLite("A"));
  EXPECT_TRUE(std::string("BA") != StringViewLite("B"));
  EXPECT_TRUE(std::string("BA") != StringViewLite("C"));

  EXPECT_TRUE(std::string("AA") != StringViewLite("B"));
  EXPECT_TRUE(std::string("BA") != StringViewLite("B"));
  EXPECT_TRUE(std::string("CA") != StringViewLite("B"));
  EXPECT_TRUE(std::string("B") != StringViewLite("AA"));
  EXPECT_TRUE(std::string("B") != StringViewLite("BA"));
  EXPECT_TRUE(std::string("B") != StringViewLite("CA"));
}


TEST(Test_StringViewLite_Compare, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(StringViewLite("A") != std::string("A"));
  EXPECT_FALSE(StringViewLite("") != std::string(""));
  EXPECT_TRUE(StringViewLite("A") != std::string(""));
  EXPECT_TRUE(StringViewLite("A") != std::string("B"));
  EXPECT_TRUE(StringViewLite("A") != std::string("a"));

  EXPECT_TRUE(StringViewLite("A") != std::string("B"));
  EXPECT_FALSE(StringViewLite("B") != std::string("B"));
  EXPECT_TRUE(StringViewLite("C") != std::string("B"));
  EXPECT_TRUE(StringViewLite("B") != std::string("A"));
  EXPECT_FALSE(StringViewLite("B") != std::string("B"));
  EXPECT_TRUE(StringViewLite("B") != std::string("C"));

  EXPECT_TRUE(StringViewLite("A") != std::string("BA"));
  EXPECT_TRUE(StringViewLite("B") != std::string("BA"));
  EXPECT_TRUE(StringViewLite("C") != std::string("BA"));
  EXPECT_TRUE(StringViewLite("BA") != std::string("A"));
  EXPECT_TRUE(StringViewLite("BA") != std::string("B"));
  EXPECT_TRUE(StringViewLite("BA") != std::string("C"));

  EXPECT_TRUE(StringViewLite("AA") != std::string("B"));
  EXPECT_TRUE(StringViewLite("BA") != std::string("B"));
  EXPECT_TRUE(StringViewLite("CA") != std::string("B"));
  EXPECT_TRUE(StringViewLite("B") != std::string("AA"));
  EXPECT_TRUE(StringViewLite("B") != std::string("BA"));
  EXPECT_TRUE(StringViewLite("B") != std::string("CA"));
}
