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

#include <FslBase/String/CStringViewUtil.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestString_CStringViewUtil = TestFixtureFslBase;
}


TEST(TestString_CStringViewUtil, Construct)
{
  std::string empty;
  auto strView = CStringViewUtil::AsCStringView(empty);

  EXPECT_TRUE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), 0u);
  EXPECT_EQ(strView.length(), 0u);
}


TEST(TestString_CStringViewUtil, Construct_FromStr)
{
  std::string str("Hello world");
  auto strView = CStringViewUtil::AsCStringView(str);

  EXPECT_FALSE(strView.empty());
  EXPECT_NE(strView.data(), nullptr);
  EXPECT_EQ(strView.size(), str.size());
  EXPECT_EQ(strView.length(), str.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, ToString_Empty)
{
  CStringView empty;
  auto dstStr = CStringViewUtil::ToString(empty);

  EXPECT_TRUE(dstStr.empty());
  EXPECT_EQ(0u, dstStr.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, ToString)
{
  CStringView str("hello");
  auto dstStr = CStringViewUtil::ToString(str);

  EXPECT_EQ(str, dstStr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, Set_String_Empty)
{
  CStringView empty;
  std::string dstStr;
  CStringViewUtil::Set(dstStr, empty);

  EXPECT_TRUE(dstStr.empty());
  EXPECT_EQ(0u, dstStr.size());
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, Set_String)
{
  CStringView str("hello");
  std::string dstStr;
  CStringViewUtil::Set(dstStr, str);

  EXPECT_EQ(str, dstStr);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, OperatorLessThan_String_LHS)
{
  EXPECT_LT(std::string("A"), CStringView("B"));
  EXPECT_LT(std::string("A"), CStringView("AB"));
  EXPECT_LT(std::string(""), CStringView("AB"));

  EXPECT_TRUE(std::string("A") < CStringView("B"));
  EXPECT_FALSE(std::string("B") < CStringView("B"));
  EXPECT_FALSE(std::string("C") < CStringView("B"));
  EXPECT_FALSE(std::string("B") < CStringView("A"));
  EXPECT_FALSE(std::string("B") < CStringView("B"));
  EXPECT_TRUE(std::string("B") < CStringView("C"));

  EXPECT_TRUE(std::string("A") < CStringView("BA"));
  EXPECT_TRUE(std::string("B") < CStringView("BA"));
  EXPECT_FALSE(std::string("C") < CStringView("BA"));
  EXPECT_FALSE(std::string("BA") < CStringView("A"));
  EXPECT_FALSE(std::string("BA") < CStringView("B"));
  EXPECT_TRUE(std::string("BA") < CStringView("C"));

  EXPECT_TRUE(std::string("AA") < CStringView("B"));
  EXPECT_FALSE(std::string("BA") < CStringView("B"));
  EXPECT_FALSE(std::string("CA") < CStringView("B"));
  EXPECT_FALSE(std::string("B") < CStringView("AA"));
  EXPECT_TRUE(std::string("B") < CStringView("BA"));
  EXPECT_TRUE(std::string("B") < CStringView("CA"));
}

TEST(TestString_CStringViewUtil, OperatorLessThan_String_RHS)
{
  EXPECT_LT(CStringView("A"), std::string("B"));
  EXPECT_LT(CStringView("A"), std::string("AB"));
  EXPECT_LT(CStringView(""), std::string("AB"));

  EXPECT_TRUE(CStringView("A") < std::string("B"));
  EXPECT_FALSE(CStringView("B") < std::string("B"));
  EXPECT_FALSE(CStringView("C") < std::string("B"));
  EXPECT_FALSE(CStringView("B") < std::string("A"));
  EXPECT_FALSE(CStringView("B") < std::string("B"));
  EXPECT_TRUE(CStringView("B") < std::string("C"));

  EXPECT_TRUE(CStringView("A") < std::string("BA"));
  EXPECT_TRUE(CStringView("B") < std::string("BA"));
  EXPECT_FALSE(CStringView("C") < std::string("BA"));
  EXPECT_FALSE(CStringView("BA") < std::string("A"));
  EXPECT_FALSE(CStringView("BA") < std::string("B"));
  EXPECT_TRUE(CStringView("BA") < std::string("C"));

  EXPECT_TRUE(CStringView("AA") < std::string("B"));
  EXPECT_FALSE(CStringView("BA") < std::string("B"));
  EXPECT_FALSE(CStringView("CA") < std::string("B"));
  EXPECT_FALSE(CStringView("B") < std::string("AA"));
  EXPECT_TRUE(CStringView("B") < std::string("BA"));
  EXPECT_TRUE(CStringView("B") < std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator <=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, OperatorLessThanOrEqual_String_LHS)
{
  EXPECT_LE(std::string("A"), CStringView("B"));
  EXPECT_LE(std::string("A"), CStringView("AB"));
  EXPECT_LE(std::string(""), CStringView("AB"));
  EXPECT_LE(std::string("A"), CStringView("A"));

  EXPECT_TRUE(std::string("A") <= CStringView("B"));
  EXPECT_TRUE(std::string("B") <= CStringView("B"));
  EXPECT_FALSE(std::string("C") <= CStringView("B"));
  EXPECT_FALSE(std::string("B") <= CStringView("A"));
  EXPECT_TRUE(std::string("B") <= CStringView("B"));
  EXPECT_TRUE(std::string("B") <= CStringView("C"));

  EXPECT_TRUE(std::string("A") <= CStringView("BA"));
  EXPECT_TRUE(std::string("B") <= CStringView("BA"));
  EXPECT_FALSE(std::string("C") <= CStringView("BA"));
  EXPECT_FALSE(std::string("BA") <= CStringView("A"));
  EXPECT_FALSE(std::string("BA") <= CStringView("B"));
  EXPECT_TRUE(std::string("BA") <= CStringView("C"));

  EXPECT_TRUE(std::string("AA") <= CStringView("B"));
  EXPECT_FALSE(std::string("BA") <= CStringView("B"));
  EXPECT_FALSE(std::string("CA") <= CStringView("B"));
  EXPECT_FALSE(std::string("B") <= CStringView("AA"));
  EXPECT_TRUE(std::string("B") <= CStringView("BA"));
  EXPECT_TRUE(std::string("B") <= CStringView("CA"));
}

TEST(TestString_CStringViewUtil, OperatorLessThanOrEqual_String_RHS)
{
  EXPECT_LE(CStringView("A"), std::string("B"));
  EXPECT_LE(CStringView("A"), std::string("AB"));
  EXPECT_LE(CStringView(""), std::string("AB"));
  EXPECT_LE(CStringView("A"), std::string("A"));

  EXPECT_TRUE(CStringView("A") <= std::string("B"));
  EXPECT_TRUE(CStringView("B") <= std::string("B"));
  EXPECT_FALSE(CStringView("C") <= std::string("B"));
  EXPECT_FALSE(CStringView("B") <= std::string("A"));
  EXPECT_TRUE(CStringView("B") <= std::string("B"));
  EXPECT_TRUE(CStringView("B") <= std::string("C"));

  EXPECT_TRUE(CStringView("A") <= std::string("BA"));
  EXPECT_TRUE(CStringView("B") <= std::string("BA"));
  EXPECT_FALSE(CStringView("C") <= std::string("BA"));
  EXPECT_FALSE(CStringView("BA") <= std::string("A"));
  EXPECT_FALSE(CStringView("BA") <= std::string("B"));
  EXPECT_TRUE(CStringView("BA") <= std::string("C"));

  EXPECT_TRUE(CStringView("AA") <= std::string("B"));
  EXPECT_FALSE(CStringView("BA") <= std::string("B"));
  EXPECT_FALSE(CStringView("CA") <= std::string("B"));
  EXPECT_FALSE(CStringView("B") <= std::string("AA"));
  EXPECT_TRUE(CStringView("B") <= std::string("BA"));
  EXPECT_TRUE(CStringView("B") <= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, OperatorGreaterThan_String_LHS)
{
  EXPECT_GT(std::string("B"), CStringView("A"));
  EXPECT_GT(std::string("AB"), CStringView("A"));
  EXPECT_GT(std::string("AB"), CStringView(""));

  EXPECT_FALSE(std::string("A") > CStringView("B"));
  EXPECT_FALSE(std::string("B") > CStringView("B"));
  EXPECT_TRUE(std::string("C") > CStringView("B"));
  EXPECT_TRUE(std::string("B") > CStringView("A"));
  EXPECT_FALSE(std::string("B") > CStringView("B"));
  EXPECT_FALSE(std::string("B") > CStringView("C"));

  EXPECT_FALSE(std::string("A") > CStringView("BA"));
  EXPECT_FALSE(std::string("B") > CStringView("BA"));
  EXPECT_TRUE(std::string("C") > CStringView("BA"));
  EXPECT_TRUE(std::string("BA") > CStringView("A"));
  EXPECT_TRUE(std::string("BA") > CStringView("B"));
  EXPECT_FALSE(std::string("BA") > CStringView("C"));

  EXPECT_FALSE(std::string("AA") > CStringView("B"));
  EXPECT_TRUE(std::string("BA") > CStringView("B"));
  EXPECT_TRUE(std::string("CA") > CStringView("B"));
  EXPECT_TRUE(std::string("B") > CStringView("AA"));
  EXPECT_FALSE(std::string("B") > CStringView("BA"));
  EXPECT_FALSE(std::string("B") > CStringView("CA"));
}

TEST(TestString_CStringViewUtil, OperatorGreaterThan_String_RHS)
{
  EXPECT_GT(CStringView("B"), std::string("A"));
  EXPECT_GT(CStringView("AB"), std::string("A"));
  EXPECT_GT(CStringView("AB"), std::string(""));

  EXPECT_FALSE(CStringView("A") > std::string("B"));
  EXPECT_FALSE(CStringView("B") > std::string("B"));
  EXPECT_TRUE(CStringView("C") > std::string("B"));
  EXPECT_TRUE(CStringView("B") > std::string("A"));
  EXPECT_FALSE(CStringView("B") > std::string("B"));
  EXPECT_FALSE(CStringView("B") > std::string("C"));

  EXPECT_FALSE(CStringView("A") > std::string("BA"));
  EXPECT_FALSE(CStringView("B") > std::string("BA"));
  EXPECT_TRUE(CStringView("C") > std::string("BA"));
  EXPECT_TRUE(CStringView("BA") > std::string("A"));
  EXPECT_TRUE(CStringView("BA") > std::string("B"));
  EXPECT_FALSE(CStringView("BA") > std::string("C"));

  EXPECT_FALSE(CStringView("AA") > std::string("B"));
  EXPECT_TRUE(CStringView("BA") > std::string("B"));
  EXPECT_TRUE(CStringView("CA") > std::string("B"));
  EXPECT_TRUE(CStringView("B") > std::string("AA"));
  EXPECT_FALSE(CStringView("B") > std::string("BA"));
  EXPECT_FALSE(CStringView("B") > std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator >=
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, OperatorGreaterThanOrEqual_String_LHS)
{
  EXPECT_GE(std::string("B"), CStringView("A"));
  EXPECT_GE(std::string("AB"), CStringView("A"));
  EXPECT_GE(std::string("AB"), CStringView(""));
  EXPECT_GE(std::string("A"), CStringView("A"));

  EXPECT_FALSE(std::string("A") >= CStringView("B"));
  EXPECT_TRUE(std::string("B") >= CStringView("B"));
  EXPECT_TRUE(std::string("C") >= CStringView("B"));
  EXPECT_TRUE(std::string("B") >= CStringView("A"));
  EXPECT_TRUE(std::string("B") >= CStringView("B"));
  EXPECT_FALSE(std::string("B") >= CStringView("C"));

  EXPECT_FALSE(std::string("A") >= CStringView("BA"));
  EXPECT_FALSE(std::string("B") >= CStringView("BA"));
  EXPECT_TRUE(std::string("C") >= CStringView("BA"));
  EXPECT_TRUE(std::string("BA") >= CStringView("A"));
  EXPECT_TRUE(std::string("BA") >= CStringView("B"));
  EXPECT_FALSE(std::string("BA") >= CStringView("C"));

  EXPECT_FALSE(std::string("AA") >= CStringView("B"));
  EXPECT_TRUE(std::string("BA") >= CStringView("B"));
  EXPECT_TRUE(std::string("CA") >= CStringView("B"));
  EXPECT_TRUE(std::string("B") >= CStringView("AA"));
  EXPECT_FALSE(std::string("B") >= CStringView("BA"));
  EXPECT_FALSE(std::string("B") >= CStringView("CA"));
}

TEST(TestString_CStringViewUtil, OperatorGreaterThanOrEqual_String_RHS)
{
  EXPECT_GE(CStringView("B"), std::string("A"));
  EXPECT_GE(CStringView("AB"), std::string("A"));
  EXPECT_GE(CStringView("AB"), std::string(""));
  EXPECT_GE(CStringView("A"), std::string("A"));

  EXPECT_FALSE(CStringView("A") >= std::string("B"));
  EXPECT_TRUE(CStringView("B") >= std::string("B"));
  EXPECT_TRUE(CStringView("C") >= std::string("B"));
  EXPECT_TRUE(CStringView("B") >= std::string("A"));
  EXPECT_TRUE(CStringView("B") >= std::string("B"));
  EXPECT_FALSE(CStringView("B") >= std::string("C"));

  EXPECT_FALSE(CStringView("A") >= std::string("BA"));
  EXPECT_FALSE(CStringView("B") >= std::string("BA"));
  EXPECT_TRUE(CStringView("C") >= std::string("BA"));
  EXPECT_TRUE(CStringView("BA") >= std::string("A"));
  EXPECT_TRUE(CStringView("BA") >= std::string("B"));
  EXPECT_FALSE(CStringView("BA") >= std::string("C"));

  EXPECT_FALSE(CStringView("AA") >= std::string("B"));
  EXPECT_TRUE(CStringView("BA") >= std::string("B"));
  EXPECT_TRUE(CStringView("CA") >= std::string("B"));
  EXPECT_TRUE(CStringView("B") >= std::string("AA"));
  EXPECT_FALSE(CStringView("B") >= std::string("BA"));
  EXPECT_FALSE(CStringView("B") >= std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator ==
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_CStringViewUtil, OperatorEqual_CString_LHS)
{
  EXPECT_TRUE(std::string("A") == CStringView("A"));
  EXPECT_TRUE(std::string("") == CStringView(""));
  EXPECT_FALSE(std::string("A") == CStringView(""));
  EXPECT_FALSE(std::string("A") == CStringView("B"));
  EXPECT_FALSE(std::string("A") == CStringView("a"));

  EXPECT_FALSE(std::string("A") == CStringView("B"));
  EXPECT_TRUE(std::string("B") == CStringView("B"));
  EXPECT_FALSE(std::string("C") == CStringView("B"));
  EXPECT_FALSE(std::string("B") == CStringView("A"));
  EXPECT_TRUE(std::string("B") == CStringView("B"));
  EXPECT_FALSE(std::string("B") == CStringView("C"));

  EXPECT_FALSE(std::string("A") == CStringView("BA"));
  EXPECT_FALSE(std::string("B") == CStringView("BA"));
  EXPECT_FALSE(std::string("C") == CStringView("BA"));
  EXPECT_FALSE(std::string("BA") == CStringView("A"));
  EXPECT_FALSE(std::string("BA") == CStringView("B"));
  EXPECT_FALSE(std::string("BA") == CStringView("C"));

  EXPECT_FALSE(std::string("AA") == CStringView("B"));
  EXPECT_FALSE(std::string("BA") == CStringView("B"));
  EXPECT_FALSE(std::string("CA") == CStringView("B"));
  EXPECT_FALSE(std::string("B") == CStringView("AA"));
  EXPECT_FALSE(std::string("B") == CStringView("BA"));
  EXPECT_FALSE(std::string("B") == CStringView("CA"));
}


TEST(TestString_CStringViewUtil, OperatorEqual_CString_RHS)
{
  EXPECT_TRUE(CStringView("A") == std::string("A"));
  EXPECT_TRUE(CStringView("") == std::string(""));
  EXPECT_FALSE(CStringView("A") == std::string(""));
  EXPECT_FALSE(CStringView("A") == std::string("B"));
  EXPECT_FALSE(CStringView("A") == std::string("a"));

  EXPECT_FALSE(CStringView("A") == std::string("B"));
  EXPECT_TRUE(CStringView("B") == std::string("B"));
  EXPECT_FALSE(CStringView("C") == std::string("B"));
  EXPECT_FALSE(CStringView("B") == std::string("A"));
  EXPECT_TRUE(CStringView("B") == std::string("B"));
  EXPECT_FALSE(CStringView("B") == std::string("C"));

  EXPECT_FALSE(CStringView("A") == std::string("BA"));
  EXPECT_FALSE(CStringView("B") == std::string("BA"));
  EXPECT_FALSE(CStringView("C") == std::string("BA"));
  EXPECT_FALSE(CStringView("BA") == std::string("A"));
  EXPECT_FALSE(CStringView("BA") == std::string("B"));
  EXPECT_FALSE(CStringView("BA") == std::string("C"));

  EXPECT_FALSE(CStringView("AA") == std::string("B"));
  EXPECT_FALSE(CStringView("BA") == std::string("B"));
  EXPECT_FALSE(CStringView("CA") == std::string("B"));
  EXPECT_FALSE(CStringView("B") == std::string("AA"));
  EXPECT_FALSE(CStringView("B") == std::string("BA"));
  EXPECT_FALSE(CStringView("B") == std::string("CA"));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Operator !=
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_CStringViewUtil, OperatorNotEqual_CString_LHS)
{
  EXPECT_FALSE(std::string("A") != CStringView("A"));
  EXPECT_FALSE(std::string("") != CStringView(""));
  EXPECT_TRUE(std::string("A") != CStringView(""));
  EXPECT_TRUE(std::string("A") != CStringView("B"));
  EXPECT_TRUE(std::string("A") != CStringView("a"));

  EXPECT_TRUE(std::string("A") != CStringView("B"));
  EXPECT_FALSE(std::string("B") != CStringView("B"));
  EXPECT_TRUE(std::string("C") != CStringView("B"));
  EXPECT_TRUE(std::string("B") != CStringView("A"));
  EXPECT_FALSE(std::string("B") != CStringView("B"));
  EXPECT_TRUE(std::string("B") != CStringView("C"));

  EXPECT_TRUE(std::string("A") != CStringView("BA"));
  EXPECT_TRUE(std::string("B") != CStringView("BA"));
  EXPECT_TRUE(std::string("C") != CStringView("BA"));
  EXPECT_TRUE(std::string("BA") != CStringView("A"));
  EXPECT_TRUE(std::string("BA") != CStringView("B"));
  EXPECT_TRUE(std::string("BA") != CStringView("C"));

  EXPECT_TRUE(std::string("AA") != CStringView("B"));
  EXPECT_TRUE(std::string("BA") != CStringView("B"));
  EXPECT_TRUE(std::string("CA") != CStringView("B"));
  EXPECT_TRUE(std::string("B") != CStringView("AA"));
  EXPECT_TRUE(std::string("B") != CStringView("BA"));
  EXPECT_TRUE(std::string("B") != CStringView("CA"));
}


TEST(TestString_CStringViewUtil, OperatorNotEqual_CString_RHS)
{
  EXPECT_FALSE(CStringView("A") != std::string("A"));
  EXPECT_FALSE(CStringView("") != std::string(""));
  EXPECT_TRUE(CStringView("A") != std::string(""));
  EXPECT_TRUE(CStringView("A") != std::string("B"));
  EXPECT_TRUE(CStringView("A") != std::string("a"));

  EXPECT_TRUE(CStringView("A") != std::string("B"));
  EXPECT_FALSE(CStringView("B") != std::string("B"));
  EXPECT_TRUE(CStringView("C") != std::string("B"));
  EXPECT_TRUE(CStringView("B") != std::string("A"));
  EXPECT_FALSE(CStringView("B") != std::string("B"));
  EXPECT_TRUE(CStringView("B") != std::string("C"));

  EXPECT_TRUE(CStringView("A") != std::string("BA"));
  EXPECT_TRUE(CStringView("B") != std::string("BA"));
  EXPECT_TRUE(CStringView("C") != std::string("BA"));
  EXPECT_TRUE(CStringView("BA") != std::string("A"));
  EXPECT_TRUE(CStringView("BA") != std::string("B"));
  EXPECT_TRUE(CStringView("BA") != std::string("C"));

  EXPECT_TRUE(CStringView("AA") != std::string("B"));
  EXPECT_TRUE(CStringView("BA") != std::string("B"));
  EXPECT_TRUE(CStringView("CA") != std::string("B"));
  EXPECT_TRUE(CStringView("B") != std::string("AA"));
  EXPECT_TRUE(CStringView("B") != std::string("BA"));
  EXPECT_TRUE(CStringView("B") != std::string("CA"));
}
