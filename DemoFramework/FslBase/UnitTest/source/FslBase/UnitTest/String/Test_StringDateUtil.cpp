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

#include <FslBase/Exceptions.hpp>
#include <FslBase/String/StringDateUtil.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <cstring>

using namespace Fsl;

namespace
{
  using TestString_Test_StringDateUtil = TestFixtureFslBase;
}


TEST(TestString_Test_StringDateUtil, ToString_Empty)
{
  std::time_t dateTime{};
  EXPECT_EQ(StringDateUtil::ToString(dateTime), "1970-01-01T00:00:00Z");
}

TEST(TestString_Test_StringDateUtil, ParseString_Empty)
{
  EXPECT_THROW(StringDateUtil::Parse(""), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_Default)
{
  std::time_t dateTime{};
  EXPECT_EQ(dateTime, StringDateUtil::Parse("1970-01-01T00:00:00Z"));
}

// TEST(TestString_Test_StringDateUtil, ParseString_InvalidYear)
//{
//  EXPECT_THROW(StringDateUtil::Parse("1969-01-01T00:00:00Z"), FormatException);
//}

TEST(TestString_Test_StringDateUtil, ParseString_InvalidMonth_Low)
{
  EXPECT_THROW(StringDateUtil::Parse("1970-00-01T00:00:00Z"), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_InvalidMonth_High)
{
  EXPECT_THROW(StringDateUtil::Parse("1970-13-01T00:00:00Z"), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_InvalidDay_Low)
{
  EXPECT_THROW(StringDateUtil::Parse("1970-01-00T00:00:00Z"), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_InvalidDay_High)
{
  EXPECT_THROW(StringDateUtil::Parse("1970-01-32T00:00:00Z"), FormatException);
}


TEST(TestString_Test_StringDateUtil, ParseString_InvalidHour)
{
  EXPECT_THROW(StringDateUtil::Parse("1970-01-01T24:00:00Z"), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_InvalidMinute)
{
  EXPECT_THROW(StringDateUtil::Parse("1970-01-01T00:60:00Z"), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_InvalidSecond)
{
  // 60 indicate a extra leap second
  // EXPECT_THROW(StringDateUtil::Parse("1970-01-01T00:00:60Z"), FormatException);
  EXPECT_THROW(StringDateUtil::Parse("1970-01-01T00:00:61Z"), FormatException);
}

TEST(TestString_Test_StringDateUtil, ParseString_BackAndForth)
{
  StringViewLite strTime("2021-12-24T12:34:56Z");
  auto time = StringDateUtil::Parse(strTime);
  EXPECT_EQ(StringDateUtil::ToString(time), strTime);
}
