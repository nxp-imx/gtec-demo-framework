/****************************************************************************************************************************************************
 * Copyright 2018 NXP
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

#include <FslBase/String/StringParseUtil.hpp>
#include <FslBase/SpanUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestString_ParseUtil = TestFixtureFslBase;


  template <typename T, std::size_t TSize>
  void Expect_EQ(const std::vector<T>& expected, const std::array<T, TSize>& value, const std::size_t numEntries)
  {
    EXPECT_GE(expected.size(), numEntries);
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
      EXPECT_EQ(expected[i], value[i]);
    }
  }

  template <std::size_t TSize>
  void Expect_EQ(const std::vector<float>& expected, const std::array<float, TSize>& value, const std::size_t numEntries)
  {
    EXPECT_GE(expected.size(), numEntries);
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
      EXPECT_FLOAT_EQ(expected[i], value[i]);
    }
  }

  template <std::size_t TSize>
  void Expect_EQ(const std::vector<double>& expected, const std::array<double, TSize>& value, const std::size_t numEntries)
  {
    EXPECT_GE(expected.size(), numEntries);
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
      EXPECT_DOUBLE_EQ(expected[i], value[i]);
    }
  }
}


TEST(TestString_StringUtil, ParseBool)
{
  std::size_t res = 0;
  bool value = false;

  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_FALSE(value);

  value = false;
  res = StringParseUtil::Parse(value, "1");
  EXPECT_EQ(1u, res);
  EXPECT_TRUE(value);

  value = false;
  res = StringParseUtil::Parse(value, "true");
  EXPECT_EQ(4u, res);
  EXPECT_TRUE(value);

  value = true;
  res = StringParseUtil::Parse(value, "false");
  EXPECT_EQ(5u, res);
  EXPECT_FALSE(value);
}

TEST(TestString_StringUtil, ParseBool_InvalidValues)
{
  bool value = false;
  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);
  // + -
  EXPECT_THROW(StringParseUtil::Parse(value, "-1"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "+1"), FormatException);
  // Incorrect casing
  EXPECT_THROW(StringParseUtil::Parse(value, "True"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "False"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "TRUE"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "FALSE"), FormatException);
  // Postfix characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, "trueA"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "falseA"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "10"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "00"), FormatException);

  // prefix white space characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, " 1"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, " 0"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, " true"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, " false"), FormatException);

  // postfix white space characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, "1 "), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "0 "), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "true "), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "false "), FormatException);
}


TEST(TestString_StringUtil, ParseUInt8)
{
  std::size_t res = 0;
  uint8_t value = 0;

  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_EQ(0u, value);


  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(42u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "255");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(255u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0xFF");
  EXPECT_EQ(4u, res);
  EXPECT_EQ(255u, value);
}


TEST(TestString_StringUtil, ParseUInt8_Invalid)
{
  uint8_t value = 0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // can not be negative
  EXPECT_THROW(StringParseUtil::Parse(value, "-1"), FormatException);

  // overflow
  EXPECT_THROW(StringParseUtil::Parse(value, "256"), OverflowException);

  // prefix characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, "q3"), FormatException);

  // postfix characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, "3q"), FormatException);

  // prefix white space characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, " 3"), FormatException);

  // postfix white space characters should not be accepted
  EXPECT_THROW(StringParseUtil::Parse(value, "3 "), FormatException);
}


TEST(TestString_StringUtil, ParseUInt16)
{
  std::size_t res = 0;
  uint16_t value = 0;
  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_EQ(0u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(42u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "65535");
  EXPECT_EQ(5u, res);
  EXPECT_EQ(65535u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0xFFFF");
  EXPECT_EQ(6u, res);
  EXPECT_EQ(65535u, value);
}


TEST(TestString_StringUtil, ParseUInt16_Invalid)
{
  uint16_t value = 0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // can not be negative
  EXPECT_THROW(StringParseUtil::Parse(value, "-1"), FormatException);

  // overflow
  EXPECT_THROW(StringParseUtil::Parse(value, "0x10000"), OverflowException);
}


TEST(TestString_StringUtil, ParseUInt32)
{
  std::size_t res = 0;
  uint32_t value = 0;

  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_EQ(0u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(42u, value);

  value = 0;
  res = StringParseUtil::Parse(value, "4294967295");
  EXPECT_EQ(10u, res);
  EXPECT_EQ(0xFFFFFFFFu, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0xFFFFFFFF");
  EXPECT_EQ(10u, res);
  EXPECT_EQ(0xFFFFFFFFu, value);
}


TEST(TestString_StringUtil, ParseUInt32_Invalid)
{
  uint32_t value = 0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // can not be negative
  EXPECT_THROW(StringParseUtil::Parse(value, "-1"), FormatException);

  // overflow
  EXPECT_THROW(StringParseUtil::Parse(value, "0x100000000"), OverflowException);
}


TEST(TestString_StringUtil, ParseInt8)
{
  std::size_t res = 0;
  int8_t value = 0;

  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_EQ(0, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-1");
  EXPECT_EQ(2u, res);
  EXPECT_EQ(-1, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(42, value);

  value = 0;
  res = StringParseUtil::Parse(value, "127");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(127, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0x7f");
  EXPECT_EQ(4u, res);
  EXPECT_EQ(127, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-128");
  EXPECT_EQ(4u, res);
  EXPECT_EQ(-128, value);
}


TEST(TestString_StringUtil, ParseInt8_Invalid)
{
  int8_t value = 0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // overflow
  EXPECT_THROW(StringParseUtil::Parse(value, "128"), OverflowException);
  EXPECT_THROW(StringParseUtil::Parse(value, "-129"), OverflowException);
}


TEST(TestString_StringUtil, ParseInt16)
{
  std::size_t res = 0;
  int16_t value = 0;

  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_EQ(0, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-1");
  EXPECT_EQ(2u, res);
  EXPECT_EQ(-1, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(42, value);

  value = 0;
  res = StringParseUtil::Parse(value, "32767");
  EXPECT_EQ(5u, res);
  EXPECT_EQ(0x7fff, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0x7fff");
  EXPECT_EQ(6u, res);
  EXPECT_EQ(0x7fff, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-32768");
  EXPECT_EQ(6u, res);
  EXPECT_EQ(-32768, value);
}


TEST(TestString_StringUtil, ParseInt16_Invalid)
{
  int16_t value = 0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // overflow
  EXPECT_THROW(StringParseUtil::Parse(value, "32768"), OverflowException);
  EXPECT_THROW(StringParseUtil::Parse(value, "-32769"), OverflowException);
}


TEST(TestString_StringUtil, ParseInt32)
{
  std::size_t res = 0;
  int32_t value = 0;

  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_EQ(0, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-1");
  EXPECT_EQ(2u, res);
  EXPECT_EQ(-1, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_EQ(42, value);

  value = 0;    // 1234567890
  res = StringParseUtil::Parse(value, "2147483647");
  EXPECT_EQ(10u, res);
  EXPECT_EQ(2147483647, value);

  value = 0;    // 1234567890
  res = StringParseUtil::Parse(value, "0x7FFFFFFF");
  EXPECT_EQ(10u, res);
  EXPECT_EQ(0x7fffffff, value);

  value = 0;    // 12345678901
  res = StringParseUtil::Parse(value, "-2147483648");
  EXPECT_EQ(11u, res);
  EXPECT_EQ(std::numeric_limits<int32_t>::min(), value);
}


TEST(TestString_StringUtil, ParseInt32_Invalid)
{
  int32_t value = 0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // overflow
  EXPECT_THROW(StringParseUtil::Parse(value, "2147483648"), OverflowException);
  EXPECT_THROW(StringParseUtil::Parse(value, "-2147483649"), OverflowException);
}


TEST(TestString_StringUtil, ParseFloat)
{
  std::size_t res = 0;
  float value = 0.0f;

  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  EXPECT_FLOAT_EQ(value, 0.0f);

  value = 0;
  res = StringParseUtil::Parse(value, "-1");
  EXPECT_EQ(2u, res);
  EXPECT_FLOAT_EQ(-1.0f, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  EXPECT_FLOAT_EQ(42.0f, value);

  value = 0;    // 1234567890
  res = StringParseUtil::Parse(value, "2147483647");
  EXPECT_EQ(10u, res);
  EXPECT_FLOAT_EQ(2147483647.0f, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0.5");
  EXPECT_EQ(3u, res);
  EXPECT_FLOAT_EQ(0.5f, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-0.5");
  EXPECT_EQ(4u, res);
  EXPECT_FLOAT_EQ(-0.5f, value);

  value = 0;    // 12345678901
  res = StringParseUtil::Parse(value, "-2147483648");
  EXPECT_EQ(11u, res);
  EXPECT_FLOAT_EQ(-2147483648.0f, value);
}

TEST(TestString_StringUtil, ParseFloat_Invalid)
{
  float value = 0.0f;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);
}

TEST(TestString_StringUtil, ParseDouble)
{
  std::size_t res = 0;

  double value = 0.0;
  value = 1;
  res = StringParseUtil::Parse(value, "0");
  EXPECT_EQ(1u, res);
  ASSERT_DOUBLE_EQ(0.0f, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-1");
  EXPECT_EQ(2u, res);
  ASSERT_DOUBLE_EQ(-1.0, value);

  value = 0;
  res = StringParseUtil::Parse(value, "+42");
  EXPECT_EQ(3u, res);
  ASSERT_DOUBLE_EQ(42.0, value);

  value = 0;    // 1234567890
  res = StringParseUtil::Parse(value, "2147483647");
  EXPECT_EQ(10u, res);
  ASSERT_DOUBLE_EQ(2147483647.0, value);

  value = 0;
  res = StringParseUtil::Parse(value, "0.5");
  EXPECT_EQ(3u, res);
  ASSERT_DOUBLE_EQ(0.5, value);

  value = 0;
  res = StringParseUtil::Parse(value, "-0.5");
  EXPECT_EQ(4u, res);
  ASSERT_DOUBLE_EQ(-0.5, value);

  value = 0;    // 12345678901
  res = StringParseUtil::Parse(value, "-2147483648");
  EXPECT_EQ(11u, res);
  ASSERT_DOUBLE_EQ(-2147483648.0, value);
}

TEST(TestString_StringUtil, ParseDouble_Invalid)
{
  double value = 0.0;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);
}


TEST(TestString_StringUtil, ParsePoint2)
{
  std::size_t res = 0;
  Point2 value;

  value = {1, 1};
  res = StringParseUtil::Parse(value, "[0,0]");
  EXPECT_EQ(5u, res);
  EXPECT_EQ(Point2(0, 0), value);

  value = {};
  res = StringParseUtil::Parse(value, "[1,2]");
  EXPECT_EQ(5u, res);
  EXPECT_EQ(Point2(1, 2), value);

  value = {};
  res = StringParseUtil::Parse(value, "[9,0]");
  EXPECT_EQ(5u, res);
  EXPECT_EQ(Point2(9, 0), value);

  value = {};
  res = StringParseUtil::Parse(value, "[1337,1]");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(Point2(1337, 1), value);

  value = {};
  res = StringParseUtil::Parse(value, "[2,1337]");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(Point2(2, 1337), value);

  value = {};
  res = StringParseUtil::Parse(value, "[-32768,-32760]");
  EXPECT_EQ(15u, res);
  EXPECT_EQ(Point2(-32768, -32760), value);
}


TEST(TestString_StringUtil, ParsePoint2_Invalid)
{
  Point2 value;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::Parse(value, ""), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "["), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0, 127, 0, 1, 127]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1,2,3]"), FormatException);
}


TEST(TestString_StringUtil, ParseRectangle)
{
  std::size_t res = 0;
  Rectangle value;

  value = {1, 1, 1, 1};
  res = StringParseUtil::Parse(value, "[0,0,0,0]");
  EXPECT_EQ(9u, res);
  EXPECT_EQ(Rectangle(0, 0, 0, 0), value);

  value = {};
  res = StringParseUtil::Parse(value, "[1,2,3,4]");
  EXPECT_EQ(9u, res);
  EXPECT_EQ(Rectangle(1, 2, 3, 4), value);

  value = {};
  res = StringParseUtil::Parse(value, "[9,0,1,0]");
  EXPECT_EQ(9u, res);
  EXPECT_EQ(Rectangle(9, 0, 1, 0), value);

  value = {};
  res = StringParseUtil::Parse(value, "[1337,1,101,42]");
  EXPECT_EQ(15u, res);
  EXPECT_EQ(Rectangle(1337, 1, 101, 42), value);

  value = {};
  res = StringParseUtil::Parse(value, "[2,1337,42,101]");
  EXPECT_EQ(15u, res);
  EXPECT_EQ(Rectangle(2, 1337, 42, 101), value);

  value = {};
  res = StringParseUtil::Parse(value, "[-32768,-32760,0,1]");
  EXPECT_EQ(19u, res);
  EXPECT_EQ(Rectangle(-32768, -32760, 0, 1), value);
}


TEST(TestString_StringUtil, ParseRectangle_Invalid)
{
  Rectangle value;

  // nullptr
  EXPECT_THROW(StringParseUtil::Parse(value, nullptr), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::Parse(value, ""), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "["), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0, 127, 0, 1, 127]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0,127,-1,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[0,127,1,-1]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1,2]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1,2,3]"), FormatException);
  EXPECT_THROW(StringParseUtil::Parse(value, "[1,2,3,4,5]"), FormatException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestString_StringUtil, ParseArrayBool)
{
  StringParseArrayResult res{};
  std::array<bool, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);

  res = StringParseUtil::ParseArray(dstSpan, "[true]");
  EXPECT_EQ(6u, res.CharactersConsumed);
  Expect_EQ<bool>({true}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[false]");
  EXPECT_EQ(7u, res.CharactersConsumed);
  Expect_EQ<bool>({false}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,true,0,1,false]");
  EXPECT_EQ(18u, res.CharactersConsumed);
  Expect_EQ<bool>({false, true, false, true, false}, dstArray, res.ArrayEntries);
}


TEST(TestString_StringUtil, ParseArrayBool_Invalid)
{
  std::array<bool, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  bool* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[true,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,true]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[true,true"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[true ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ true]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, true, 0, 1, false]"), FormatException);
}


TEST(TestString_StringUtil, ParseArrayUInt8)
{
  StringParseArrayResult res{};
  std::array<uint8_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);

  res = StringParseUtil::ParseArray(dstSpan, "[1]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ<uint8_t>({1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ<uint8_t>({0}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[255]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ<uint8_t>({255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,6,0,255,1]");
  EXPECT_EQ(13u, res.CharactersConsumed);
  Expect_EQ<uint8_t>({0, 6, 0, 255, 1}, dstArray, res.ArrayEntries);
}


TEST(TestString_StringUtil, ParseArrayUInt8_Invalid)
{
  std::array<uint8_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  uint8_t* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}


TEST(TestString_StringUtil, ParseArrayUInt16_Invalid)
{
  std::array<uint16_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  uint16_t* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}


TEST(TestString_StringUtil, ParseArrayUInt32)
{
  StringParseArrayResult res{};
  std::array<uint32_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);

  res = StringParseUtil::ParseArray(dstSpan, "[1]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ<uint32_t>({1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ<uint32_t>({0}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[255]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ<uint32_t>({255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,6,0,255,1]");
  EXPECT_EQ(13u, res.CharactersConsumed);
  Expect_EQ<uint32_t>({0, 6, 0, 255, 1}, dstArray, res.ArrayEntries);
}


TEST(TestString_StringUtil, ParseArrayUInt32_Invalid)
{
  std::array<uint32_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  uint32_t* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}


TEST(TestString_StringUtil, ParseArrayInt8_Invalid)
{
  std::array<int8_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  int8_t* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}

TEST(TestString_StringUtil, ParseArrayInt16_Invalid)
{
  std::array<int16_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  int16_t* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}

TEST(TestString_StringUtil, ParseArrayInt32)
{
  StringParseArrayResult res{};
  std::array<int32_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);

  res = StringParseUtil::ParseArray(dstSpan, "[1]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ<int32_t>({1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-1]");
  EXPECT_EQ(4u, res.CharactersConsumed);
  Expect_EQ<int32_t>({-1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ<int32_t>({0}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[255]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ<int32_t>({255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-255]");
  EXPECT_EQ(6u, res.CharactersConsumed);
  Expect_EQ<int32_t>({-255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,6,0,255,1]");
  EXPECT_EQ(13u, res.CharactersConsumed);
  Expect_EQ<int32_t>({0, 6, 0, 255, 1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,-6,0,255,1]");
  EXPECT_EQ(14u, res.CharactersConsumed);
  Expect_EQ<int32_t>({0, -6, 0, 255, 1}, dstArray, res.ArrayEntries);
}


TEST(TestString_StringUtil, ParseArrayInt32_Invalid)
{
  std::array<int32_t, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  int32_t* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}


TEST(TestString_StringUtil, ParseArrayFloat)
{
  StringParseArrayResult res{};
  std::array<float, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);

  res = StringParseUtil::ParseArray(dstSpan, "[1]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ({1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-1]");
  EXPECT_EQ(4u, res.CharactersConsumed);
  Expect_EQ({-1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ({0}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[255]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ({255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-255]");
  EXPECT_EQ(6u, res.CharactersConsumed);
  Expect_EQ({-255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,6,0,255,1]");
  EXPECT_EQ(13u, res.CharactersConsumed);
  Expect_EQ({0, 6, 0, 255, 1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,-6,0,255,1]");
  EXPECT_EQ(14u, res.CharactersConsumed);
  Expect_EQ({0, -6, 0, 255, 1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0.5]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ({0.5f}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-0.5]");
  EXPECT_EQ(6u, res.CharactersConsumed);
  Expect_EQ({-0.5f}, dstArray, res.ArrayEntries);
}


TEST(TestString_StringUtil, ParseArrayFloat_Invalid)
{
  std::array<float, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  float* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}

TEST(TestString_StringUtil, ParseArrayDouble)
{
  StringParseArrayResult res{};
  std::array<double, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);

  res = StringParseUtil::ParseArray(dstSpan, "[1]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ({1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-1]");
  EXPECT_EQ(4u, res.CharactersConsumed);
  Expect_EQ({-1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0]");
  EXPECT_EQ(3u, res.CharactersConsumed);
  Expect_EQ({0}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[255]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ({255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-255]");
  EXPECT_EQ(6u, res.CharactersConsumed);
  Expect_EQ({-255}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,6,0,255,1]");
  EXPECT_EQ(13u, res.CharactersConsumed);
  Expect_EQ({0, 6, 0, 255, 1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0,-6,0,255,1]");
  EXPECT_EQ(14u, res.CharactersConsumed);
  Expect_EQ({0, -6, 0, 255, 1}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[0.5]");
  EXPECT_EQ(5u, res.CharactersConsumed);
  Expect_EQ({0.5}, dstArray, res.ArrayEntries);

  res = StringParseUtil::ParseArray(dstSpan, "[-0.5]");
  EXPECT_EQ(6u, res.CharactersConsumed);
  Expect_EQ({-0.5}, dstArray, res.ArrayEntries);
}


TEST(TestString_StringUtil, ParseArrayDouble_Invalid)
{
  std::array<double, 10> dstArray{};
  auto dstSpan = SpanUtil::AsSpan(dstArray);
  double* pNull = nullptr;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, nullptr), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(SpanUtil::AsSpan(pNull, 0), "[0]"), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "["), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1,]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[,0]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0,0"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[1 ]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[ 1]"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseArray(dstSpan, "[0, 127, 0, 1, 127]"), FormatException);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

// Binary - radix 2
TEST(TestString_StringUtil, TryParseInt32Radix2)
{
  int32_t value = 0;
  value = 0;

  auto strView = StringViewLite("10");
  EXPECT_TRUE(StringParseUtil::TryParse(value, strView, 2));
  EXPECT_EQ(2, value);

  value = 0;
  strView = StringViewLite("010");
  EXPECT_TRUE(StringParseUtil::TryParse(value, strView, 2));
  EXPECT_EQ(2, value);

  value = 0;
  strView = StringViewLite("1010");
  EXPECT_TRUE(StringParseUtil::TryParse(value, strView, 2));
  EXPECT_EQ(10, value);
}


TEST(TestString_StringUtil, TryParseInt32Radix2_Invalid)
{
  int32_t value = 1;

  // binary format
  const auto* psz = "12";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 2));
  EXPECT_EQ(value, 0);
}


TEST(TestString_StringUtil, TryParseInt32Radix10)
{
  int32_t value = 0;

  value = 1;
  const auto* psz = "0";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz));
  EXPECT_EQ(value, 0);

  value = 0;
  psz = "-1";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz));
  EXPECT_EQ(-1, value);

  value = 0;
  psz = "+42";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz));
  EXPECT_EQ(42, value);

  value = 0;
  psz = "2147483647";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz));
  EXPECT_EQ(2147483647, value);

  value = 0;
  psz = "2147483647";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz));
  EXPECT_EQ(std::numeric_limits<int32_t>::max(), value);

  value = 0;
  psz = "-2147483648";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz));
  EXPECT_EQ(std::numeric_limits<int32_t>::min(), value);
}


TEST(TestString_StringUtil, TryParseInt32Radix10_Invalid)
{
  int32_t value = 0;

  // nullptr
  EXPECT_FALSE(StringParseUtil::TryParse(value, nullptr, 0, 0));

  // Format
  const auto* psz = "0x10";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz));

  psz = "0x7FFFFFFF";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz));

  // overflow
  psz = "2147483648";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz));
  psz = "-2147483649";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz));
}


TEST(TestString_StringUtil, TryParseInt32Radix16)
{
  int32_t value = 0;

  // Binary - radix 16
  value = 0;
  const auto* psz = "10";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(16, value);

  psz = "22";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(34, value);

  psz = "A0";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(160, value);

  psz = "a0";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(160, value);

  psz = "-A";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(-10, value);

  psz = "+A";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(10, value);

  psz = "FF";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(255, value);

  psz = "-FF";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(-255, value);

  psz = "-ff";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 16));
  EXPECT_EQ(-255, value);
}


TEST(TestString_StringUtil, TryParseInt32Radix16_Invalid)
{
  int32_t value = 0;

  // hex format
  const auto* psz = "G";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 16));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


// Binary - radix 2
TEST(TestString_StringUtil, TryParseInt32Radix2_SubStr)
{
  int32_t value = 0;
  value = 0;

  const auto* psz = "10";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 2));
  EXPECT_EQ(2, value);

  value = 0;
  psz = "010";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 2));
  EXPECT_EQ(2, value);

  value = 0;
  psz = "1010";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 2));
  EXPECT_EQ(10, value);
}


TEST(TestString_StringUtil, TryParseInt32Radix2_SubStr_Invalid)
{
  int32_t value = 1;

  // binary format
  const auto* psz = "12";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 2));
  EXPECT_EQ(value, 0);
}


TEST(TestString_StringUtil, TryParseInt32Radix10_SubStr)
{
  int32_t value = 0;

  value = 1;
  const auto* psz = "0";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  EXPECT_EQ(value, 0);

  value = 0;
  psz = "-1";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  EXPECT_EQ(-1, value);

  value = 0;
  psz = "+42";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  EXPECT_EQ(42, value);

  value = 0;
  psz = "2147483647";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  EXPECT_EQ(2147483647, value);

  value = 0;
  psz = "2147483647";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  EXPECT_EQ(std::numeric_limits<int32_t>::max(), value);

  value = 0;
  psz = "-2147483648";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  EXPECT_EQ(std::numeric_limits<int32_t>::min(), value);
}


TEST(TestString_StringUtil, TryParseInt32Radix10_SubStr_Invalid)
{
  int32_t value = 0;

  // nullptr
  EXPECT_FALSE(StringParseUtil::TryParse(value, nullptr, 0, 0));

  // Format
  const auto* psz = "0x10";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));

  psz = "0x7FFFFFFF";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));

  // overflow
  psz = "2147483648";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
  psz = "-2147483649";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz)));
}


TEST(TestString_StringUtil, TryParseInt32Radix16_SubStr)
{
  int32_t value = 0;

  // Binary - radix 16
  value = 0;
  const auto* psz = "10";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(16, value);

  psz = "22";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(34, value);

  psz = "A0";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(160, value);

  psz = "a0";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(160, value);

  psz = "-A";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(-10, value);

  psz = "+A";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(10, value);

  psz = "FF";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(255, value);

  psz = "-FF";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(-255, value);

  psz = "-ff";
  EXPECT_TRUE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
  EXPECT_EQ(-255, value);
}


TEST(TestString_StringUtil, TryParseInt32Radix16_SubStr_Invalid)
{
  int32_t value = 0;

  // hex format
  const auto* psz = "G";
  EXPECT_FALSE(StringParseUtil::TryParse(value, psz, 0, std::strlen(psz), 16));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestString_StringUtil, ParseTime)
{
  std::size_t res = 0;
  std::chrono::seconds value;

  res = StringParseUtil::ParseTime(value, "00:00:00");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::seconds(0), value);

  res = StringParseUtil::ParseTime(value, "00:01:00");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::minutes(1), value);

  res = StringParseUtil::ParseTime(value, "01:00:00");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::hours(1), value);

  res = StringParseUtil::ParseTime(value, "24:00:00");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::hours(24), value);

  res = StringParseUtil::ParseTime(value, "00:00:59");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::seconds(59), value);

  res = StringParseUtil::ParseTime(value, "00:59:00");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::minutes(59), value);

  res = StringParseUtil::ParseTime(value, "23:59:59");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::hours(23) + std::chrono::minutes(59) + std::chrono::seconds(59), value);

  // It's a quirk that this is allowed
  res = StringParseUtil::ParseTime(value, "24:59:59");
  EXPECT_EQ(8u, res);
  EXPECT_EQ(std::chrono::hours(24) + std::chrono::minutes(59) + std::chrono::seconds(59), value);
}


TEST(TestString_StringUtil, ParseTime_Invalid)
{
  std::chrono::seconds value;

  // nullptr
  EXPECT_THROW(StringParseUtil::ParseTime(value, nullptr), FormatException);

  // format
  EXPECT_THROW(StringParseUtil::ParseTime(value, ""), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, ":"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, ":"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "::"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "AA:BB:CC"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "0:01:01"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "00:00:60"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "00:00:61"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "00:60:00"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "00:61:00"), FormatException);
  EXPECT_THROW(StringParseUtil::ParseTime(value, "25:00:00"), FormatException);
}
