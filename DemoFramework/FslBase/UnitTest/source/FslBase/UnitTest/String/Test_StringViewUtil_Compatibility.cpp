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
#include <string_view>

// Verify that the comparison operators work as expect on std::string_view and std::string

namespace
{
  using Test_StringViewUtil_Compatibility_Compatibility = TestFixtureFslBase;
}


TEST(Test_StringViewUtil_Compatibility, Construct)
{
  std::string empty;
  std::string_view strView(empty);

  EXPECT_TRUE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(Test_StringViewUtil_Compatibility, Construct_FromStr)
{
  std::string str("Hello world");
  std::string_view strView(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, ToString_Empty)
{
  std::string_view empty;
  auto dstStr = std::string(empty);

  EXPECT_TRUE(dstStr.empty());
  EXPECT_EQ(0u, dstStr.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, ToString)
{
  std::string_view str("hello");
  auto dstStr = std::string(str);

  EXPECT_EQ(str, dstStr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, Set_String_Empty)
{
  std::string_view empty;
  std::string dstStr;
  dstStr = empty;

  EXPECT_TRUE(dstStr.empty());
  EXPECT_EQ(0u, dstStr.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, Set_String)
{
  std::string_view str("hello");
  std::string dstStr;
  dstStr = str;

  EXPECT_EQ(str, dstStr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, OperatorLessThan_String_LHS)
{
  EXPECT_LT(std::string("A"), std::string_view("B"));
  EXPECT_LT(std::string("A"), std::string_view("AB"));
  EXPECT_LT(std::string(""), std::string_view("AB"));

  EXPECT_TRUE(std::string("A") < std::string_view("B"));
  EXPECT_FALSE(std::string("B") < std::string_view("B"));
  EXPECT_FALSE(std::string("C") < std::string_view("B"));
  EXPECT_FALSE(std::string("B") < std::string_view("A"));
  EXPECT_FALSE(std::string("B") < std::string_view("B"));
  EXPECT_TRUE(std::string("B") < std::string_view("C"));

  EXPECT_TRUE(std::string("A") < std::string_view("BA"));
  EXPECT_TRUE(std::string("B") < std::string_view("BA"));
  EXPECT_FALSE(std::string("C") < std::string_view("BA"));
  EXPECT_FALSE(std::string("BA") < std::string_view("A"));
  EXPECT_FALSE(std::string("BA") < std::string_view("B"));
  EXPECT_TRUE(std::string("BA") < std::string_view("C"));

  EXPECT_TRUE(std::string("AA") < std::string_view("B"));
  EXPECT_FALSE(std::string("BA") < std::string_view("B"));
  EXPECT_FALSE(std::string("CA") < std::string_view("B"));
  EXPECT_FALSE(std::string("B") < std::string_view("AA"));
  EXPECT_TRUE(std::string("B") < std::string_view("BA"));
  EXPECT_TRUE(std::string("B") < std::string_view("CA"));
}

TEST(Test_StringViewUtil_Compatibility, OperatorLessThan_String_RHS)
{
  EXPECT_LT(std::string_view("A"), std::string("B"));
  EXPECT_LT(std::string_view("A"), std::string("AB"));
  EXPECT_LT(std::string_view(""), std::string("AB"));

  EXPECT_TRUE(std::string_view("A") < std::string("B"));
  EXPECT_FALSE(std::string_view("B") < std::string("B"));
  EXPECT_FALSE(std::string_view("C") < std::string("B"));
  EXPECT_FALSE(std::string_view("B") < std::string("A"));
  EXPECT_FALSE(std::string_view("B") < std::string("B"));
  EXPECT_TRUE(std::string_view("B") < std::string("C"));

  EXPECT_TRUE(std::string_view("A") < std::string("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string("BA"));
  EXPECT_FALSE(std::string_view("C") < std::string("BA"));
  EXPECT_FALSE(std::string_view("BA") < std::string("A"));
  EXPECT_FALSE(std::string_view("BA") < std::string("B"));
  EXPECT_TRUE(std::string_view("BA") < std::string("C"));

  EXPECT_TRUE(std::string_view("AA") < std::string("B"));
  EXPECT_FALSE(std::string_view("BA") < std::string("B"));
  EXPECT_FALSE(std::string_view("CA") < std::string("B"));
  EXPECT_FALSE(std::string_view("B") < std::string("AA"));
  EXPECT_TRUE(std::string_view("B") < std::string("BA"));
  EXPECT_TRUE(std::string_view("B") < std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, OperatorLessThanOrEqual_String_LHS)
{
  EXPECT_LE(std::string("A"), std::string_view("B"));
  EXPECT_LE(std::string("A"), std::string_view("AB"));
  EXPECT_LE(std::string(""), std::string_view("AB"));
  EXPECT_LE(std::string("A"), std::string_view("A"));

  EXPECT_TRUE(std::string("A") <= std::string_view("B"));
  EXPECT_TRUE(std::string("B") <= std::string_view("B"));
  EXPECT_FALSE(std::string("C") <= std::string_view("B"));
  EXPECT_FALSE(std::string("B") <= std::string_view("A"));
  EXPECT_TRUE(std::string("B") <= std::string_view("B"));
  EXPECT_TRUE(std::string("B") <= std::string_view("C"));

  EXPECT_TRUE(std::string("A") <= std::string_view("BA"));
  EXPECT_TRUE(std::string("B") <= std::string_view("BA"));
  EXPECT_FALSE(std::string("C") <= std::string_view("BA"));
  EXPECT_FALSE(std::string("BA") <= std::string_view("A"));
  EXPECT_FALSE(std::string("BA") <= std::string_view("B"));
  EXPECT_TRUE(std::string("BA") <= std::string_view("C"));

  EXPECT_TRUE(std::string("AA") <= std::string_view("B"));
  EXPECT_FALSE(std::string("BA") <= std::string_view("B"));
  EXPECT_FALSE(std::string("CA") <= std::string_view("B"));
  EXPECT_FALSE(std::string("B") <= std::string_view("AA"));
  EXPECT_TRUE(std::string("B") <= std::string_view("BA"));
  EXPECT_TRUE(std::string("B") <= std::string_view("CA"));
}

TEST(Test_StringViewUtil_Compatibility, OperatorLessThanOrEqual_String_RHS)
{
  EXPECT_LE(std::string_view("A"), std::string("B"));
  EXPECT_LE(std::string_view("A"), std::string("AB"));
  EXPECT_LE(std::string_view(""), std::string("AB"));
  EXPECT_LE(std::string_view("A"), std::string("A"));

  EXPECT_TRUE(std::string_view("A") <= std::string("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string("B"));
  EXPECT_FALSE(std::string_view("C") <= std::string("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string("A"));
  EXPECT_TRUE(std::string_view("B") <= std::string("B"));
  EXPECT_TRUE(std::string_view("B") <= std::string("C"));

  EXPECT_TRUE(std::string_view("A") <= std::string("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string("BA"));
  EXPECT_FALSE(std::string_view("C") <= std::string("BA"));
  EXPECT_FALSE(std::string_view("BA") <= std::string("A"));
  EXPECT_FALSE(std::string_view("BA") <= std::string("B"));
  EXPECT_TRUE(std::string_view("BA") <= std::string("C"));

  EXPECT_TRUE(std::string_view("AA") <= std::string("B"));
  EXPECT_FALSE(std::string_view("BA") <= std::string("B"));
  EXPECT_FALSE(std::string_view("CA") <= std::string("B"));
  EXPECT_FALSE(std::string_view("B") <= std::string("AA"));
  EXPECT_TRUE(std::string_view("B") <= std::string("BA"));
  EXPECT_TRUE(std::string_view("B") <= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, OperatorGreaterThan_String_LHS)
{
  EXPECT_GT(std::string("B"), std::string_view("A"));
  EXPECT_GT(std::string("AB"), std::string_view("A"));
  EXPECT_GT(std::string("AB"), std::string_view(""));

  EXPECT_FALSE(std::string("A") > std::string_view("B"));
  EXPECT_FALSE(std::string("B") > std::string_view("B"));
  EXPECT_TRUE(std::string("C") > std::string_view("B"));
  EXPECT_TRUE(std::string("B") > std::string_view("A"));
  EXPECT_FALSE(std::string("B") > std::string_view("B"));
  EXPECT_FALSE(std::string("B") > std::string_view("C"));

  EXPECT_FALSE(std::string("A") > std::string_view("BA"));
  EXPECT_FALSE(std::string("B") > std::string_view("BA"));
  EXPECT_TRUE(std::string("C") > std::string_view("BA"));
  EXPECT_TRUE(std::string("BA") > std::string_view("A"));
  EXPECT_TRUE(std::string("BA") > std::string_view("B"));
  EXPECT_FALSE(std::string("BA") > std::string_view("C"));

  EXPECT_FALSE(std::string("AA") > std::string_view("B"));
  EXPECT_TRUE(std::string("BA") > std::string_view("B"));
  EXPECT_TRUE(std::string("CA") > std::string_view("B"));
  EXPECT_TRUE(std::string("B") > std::string_view("AA"));
  EXPECT_FALSE(std::string("B") > std::string_view("BA"));
  EXPECT_FALSE(std::string("B") > std::string_view("CA"));
}

TEST(Test_StringViewUtil_Compatibility, OperatorGreaterThan_String_RHS)
{
  EXPECT_GT(std::string_view("B"), std::string("A"));
  EXPECT_GT(std::string_view("AB"), std::string("A"));
  EXPECT_GT(std::string_view("AB"), std::string(""));

  EXPECT_FALSE(std::string_view("A") > std::string("B"));
  EXPECT_FALSE(std::string_view("B") > std::string("B"));
  EXPECT_TRUE(std::string_view("C") > std::string("B"));
  EXPECT_TRUE(std::string_view("B") > std::string("A"));
  EXPECT_FALSE(std::string_view("B") > std::string("B"));
  EXPECT_FALSE(std::string_view("B") > std::string("C"));

  EXPECT_FALSE(std::string_view("A") > std::string("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string("BA"));
  EXPECT_TRUE(std::string_view("C") > std::string("BA"));
  EXPECT_TRUE(std::string_view("BA") > std::string("A"));
  EXPECT_TRUE(std::string_view("BA") > std::string("B"));
  EXPECT_FALSE(std::string_view("BA") > std::string("C"));

  EXPECT_FALSE(std::string_view("AA") > std::string("B"));
  EXPECT_TRUE(std::string_view("BA") > std::string("B"));
  EXPECT_TRUE(std::string_view("CA") > std::string("B"));
  EXPECT_TRUE(std::string_view("B") > std::string("AA"));
  EXPECT_FALSE(std::string_view("B") > std::string("BA"));
  EXPECT_FALSE(std::string_view("B") > std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, OperatorGreaterThanOrEqual_String_LHS)
{
  EXPECT_GE(std::string("B"), std::string_view("A"));
  EXPECT_GE(std::string("AB"), std::string_view("A"));
  EXPECT_GE(std::string("AB"), std::string_view(""));
  EXPECT_GE(std::string("A"), std::string_view("A"));

  EXPECT_FALSE(std::string("A") >= std::string_view("B"));
  EXPECT_TRUE(std::string("B") >= std::string_view("B"));
  EXPECT_TRUE(std::string("C") >= std::string_view("B"));
  EXPECT_TRUE(std::string("B") >= std::string_view("A"));
  EXPECT_TRUE(std::string("B") >= std::string_view("B"));
  EXPECT_FALSE(std::string("B") >= std::string_view("C"));

  EXPECT_FALSE(std::string("A") >= std::string_view("BA"));
  EXPECT_FALSE(std::string("B") >= std::string_view("BA"));
  EXPECT_TRUE(std::string("C") >= std::string_view("BA"));
  EXPECT_TRUE(std::string("BA") >= std::string_view("A"));
  EXPECT_TRUE(std::string("BA") >= std::string_view("B"));
  EXPECT_FALSE(std::string("BA") >= std::string_view("C"));

  EXPECT_FALSE(std::string("AA") >= std::string_view("B"));
  EXPECT_TRUE(std::string("BA") >= std::string_view("B"));
  EXPECT_TRUE(std::string("CA") >= std::string_view("B"));
  EXPECT_TRUE(std::string("B") >= std::string_view("AA"));
  EXPECT_FALSE(std::string("B") >= std::string_view("BA"));
  EXPECT_FALSE(std::string("B") >= std::string_view("CA"));
}

TEST(Test_StringViewUtil_Compatibility, OperatorGreaterThanOrEqual_String_RHS)
{
  EXPECT_GE(std::string_view("B"), std::string("A"));
  EXPECT_GE(std::string_view("AB"), std::string("A"));
  EXPECT_GE(std::string_view("AB"), std::string(""));
  EXPECT_GE(std::string_view("A"), std::string("A"));

  EXPECT_FALSE(std::string_view("A") >= std::string("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string("B"));
  EXPECT_TRUE(std::string_view("C") >= std::string("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string("A"));
  EXPECT_TRUE(std::string_view("B") >= std::string("B"));
  EXPECT_FALSE(std::string_view("B") >= std::string("C"));

  EXPECT_FALSE(std::string_view("A") >= std::string("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string("BA"));
  EXPECT_TRUE(std::string_view("C") >= std::string("BA"));
  EXPECT_TRUE(std::string_view("BA") >= std::string("A"));
  EXPECT_TRUE(std::string_view("BA") >= std::string("B"));
  EXPECT_FALSE(std::string_view("BA") >= std::string("C"));

  EXPECT_FALSE(std::string_view("AA") >= std::string("B"));
  EXPECT_TRUE(std::string_view("BA") >= std::string("B"));
  EXPECT_TRUE(std::string_view("CA") >= std::string("B"));
  EXPECT_TRUE(std::string_view("B") >= std::string("AA"));
  EXPECT_FALSE(std::string_view("B") >= std::string("BA"));
  EXPECT_FALSE(std::string_view("B") >= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator ==
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_StringViewUtil_Compatibility, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE(std::string("A") == std::string_view("A"));
  EXPECT_TRUE(std::string("") == std::string_view(""));
  EXPECT_FALSE(std::string("A") == std::string_view(""));
  EXPECT_FALSE(std::string("A") == std::string_view("B"));
  EXPECT_FALSE(std::string("A") == std::string_view("a"));

  EXPECT_FALSE(std::string("A") == std::string_view("B"));
  EXPECT_TRUE(std::string("B") == std::string_view("B"));
  EXPECT_FALSE(std::string("C") == std::string_view("B"));
  EXPECT_FALSE(std::string("B") == std::string_view("A"));
  EXPECT_TRUE(std::string("B") == std::string_view("B"));
  EXPECT_FALSE(std::string("B") == std::string_view("C"));

  EXPECT_FALSE(std::string("A") == std::string_view("BA"));
  EXPECT_FALSE(std::string("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string("C") == std::string_view("BA"));
  EXPECT_FALSE(std::string("BA") == std::string_view("A"));
  EXPECT_FALSE(std::string("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string("BA") == std::string_view("C"));

  EXPECT_FALSE(std::string("AA") == std::string_view("B"));
  EXPECT_FALSE(std::string("BA") == std::string_view("B"));
  EXPECT_FALSE(std::string("CA") == std::string_view("B"));
  EXPECT_FALSE(std::string("B") == std::string_view("AA"));
  EXPECT_FALSE(std::string("B") == std::string_view("BA"));
  EXPECT_FALSE(std::string("B") == std::string_view("CA"));
}


TEST(Test_StringViewUtil_Compatibility, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(std::string_view("A") == std::string("A"));
  EXPECT_TRUE(std::string_view("") == std::string(""));
  EXPECT_FALSE(std::string_view("A") == std::string(""));
  EXPECT_FALSE(std::string_view("A") == std::string("B"));
  EXPECT_FALSE(std::string_view("A") == std::string("a"));

  EXPECT_FALSE(std::string_view("A") == std::string("B"));
  EXPECT_TRUE(std::string_view("B") == std::string("B"));
  EXPECT_FALSE(std::string_view("C") == std::string("B"));
  EXPECT_FALSE(std::string_view("B") == std::string("A"));
  EXPECT_TRUE(std::string_view("B") == std::string("B"));
  EXPECT_FALSE(std::string_view("B") == std::string("C"));

  EXPECT_FALSE(std::string_view("A") == std::string("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string("BA"));
  EXPECT_FALSE(std::string_view("C") == std::string("BA"));
  EXPECT_FALSE(std::string_view("BA") == std::string("A"));
  EXPECT_FALSE(std::string_view("BA") == std::string("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string("C"));

  EXPECT_FALSE(std::string_view("AA") == std::string("B"));
  EXPECT_FALSE(std::string_view("BA") == std::string("B"));
  EXPECT_FALSE(std::string_view("CA") == std::string("B"));
  EXPECT_FALSE(std::string_view("B") == std::string("AA"));
  EXPECT_FALSE(std::string_view("B") == std::string("BA"));
  EXPECT_FALSE(std::string_view("B") == std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_StringViewUtil_Compatibility, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE(std::string("A") != std::string_view("A"));
  EXPECT_FALSE(std::string("") != std::string_view(""));
  EXPECT_TRUE(std::string("A") != std::string_view(""));
  EXPECT_TRUE(std::string("A") != std::string_view("B"));
  EXPECT_TRUE(std::string("A") != std::string_view("a"));

  EXPECT_TRUE(std::string("A") != std::string_view("B"));
  EXPECT_FALSE(std::string("B") != std::string_view("B"));
  EXPECT_TRUE(std::string("C") != std::string_view("B"));
  EXPECT_TRUE(std::string("B") != std::string_view("A"));
  EXPECT_FALSE(std::string("B") != std::string_view("B"));
  EXPECT_TRUE(std::string("B") != std::string_view("C"));

  EXPECT_TRUE(std::string("A") != std::string_view("BA"));
  EXPECT_TRUE(std::string("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string("C") != std::string_view("BA"));
  EXPECT_TRUE(std::string("BA") != std::string_view("A"));
  EXPECT_TRUE(std::string("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string("BA") != std::string_view("C"));

  EXPECT_TRUE(std::string("AA") != std::string_view("B"));
  EXPECT_TRUE(std::string("BA") != std::string_view("B"));
  EXPECT_TRUE(std::string("CA") != std::string_view("B"));
  EXPECT_TRUE(std::string("B") != std::string_view("AA"));
  EXPECT_TRUE(std::string("B") != std::string_view("BA"));
  EXPECT_TRUE(std::string("B") != std::string_view("CA"));
}


TEST(Test_StringViewUtil_Compatibility, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(std::string_view("A") != std::string("A"));
  EXPECT_FALSE(std::string_view("") != std::string(""));
  EXPECT_TRUE(std::string_view("A") != std::string(""));
  EXPECT_TRUE(std::string_view("A") != std::string("B"));
  EXPECT_TRUE(std::string_view("A") != std::string("a"));

  EXPECT_TRUE(std::string_view("A") != std::string("B"));
  EXPECT_FALSE(std::string_view("B") != std::string("B"));
  EXPECT_TRUE(std::string_view("C") != std::string("B"));
  EXPECT_TRUE(std::string_view("B") != std::string("A"));
  EXPECT_FALSE(std::string_view("B") != std::string("B"));
  EXPECT_TRUE(std::string_view("B") != std::string("C"));

  EXPECT_TRUE(std::string_view("A") != std::string("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string("BA"));
  EXPECT_TRUE(std::string_view("C") != std::string("BA"));
  EXPECT_TRUE(std::string_view("BA") != std::string("A"));
  EXPECT_TRUE(std::string_view("BA") != std::string("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string("C"));

  EXPECT_TRUE(std::string_view("AA") != std::string("B"));
  EXPECT_TRUE(std::string_view("BA") != std::string("B"));
  EXPECT_TRUE(std::string_view("CA") != std::string("B"));
  EXPECT_TRUE(std::string_view("B") != std::string("AA"));
  EXPECT_TRUE(std::string_view("B") != std::string("BA"));
  EXPECT_TRUE(std::string_view("B") != std::string("CA"));
}
