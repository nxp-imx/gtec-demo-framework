/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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

#include <FslBase/Optional.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_Optional = TestFixtureFslBase;
}


TEST(Test_Optional, Empty)
{
  Optional<int> optional;
  EXPECT_FALSE(optional.HasValue());
  EXPECT_THROW(optional.Value(), BadOptionalAccess);
}


TEST(Test_Optional, SimpleValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_EQ(value, optional.Value());
  EXPECT_EQ(value, *optional);
}


TEST(Test_Optional, SimpleValue_Assign)
{
  Optional<int> optional;

  constexpr const int value = 42;
  optional = value;

  EXPECT_TRUE(optional.HasValue());
  EXPECT_EQ(value, optional.Value());
  EXPECT_EQ(value, *optional);
}

TEST(Test_Optional, SimpleValue_ValueEqual)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE(value == optional);
}

TEST(Test_Optional, SimpleValue_ValueNotEquals)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE((value + 1) != optional);
}

TEST(Test_Optional, SimpleValue_ValueLess)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE((value - 1) < optional);
  EXPECT_FALSE(value < optional);
  EXPECT_FALSE((value + 1) < optional);
}

TEST(Test_Optional, SimpleValue_ValueLessOrEqual)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE((value - 1) <= optional);
  EXPECT_TRUE(value <= optional);
  EXPECT_FALSE((value + 1) <= optional);
}


TEST(Test_Optional, SimpleValue_ValueGreater)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_FALSE((value - 1) > optional);
  EXPECT_FALSE(value > optional);
  EXPECT_TRUE((value + 1) > optional);
}


TEST(Test_Optional, SimpleValue_ValueGreaterOrEqual)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_FALSE((value - 1) >= optional);
  EXPECT_TRUE(value >= optional);
  EXPECT_TRUE((value + 1) >= optional);
}

TEST(Test_Optional, SimpleValue_EqualValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE(optional == value);
}

TEST(Test_Optional, SimpleValue_NotEqualsValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE(optional != (value + 1));
}

TEST(Test_Optional, SimpleValue_LessValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_FALSE(optional < (value - 1));
  EXPECT_FALSE(optional < value);
  EXPECT_TRUE(optional < (value + 1));
}

TEST(Test_Optional, SimpleValue_LessOrEqualValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_FALSE(optional <= (value - 1));
  EXPECT_TRUE(optional <= value);
  EXPECT_TRUE(optional <= (value + 1));
}


TEST(Test_Optional, SimpleValue_GreaterValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE(optional > (value - 1));
  EXPECT_FALSE(optional > value);
  EXPECT_FALSE(optional > (value + 1));
}


TEST(Test_Optional, SimpleValue_GreaterOrEqualValue)
{
  constexpr const int value = 42;
  Optional<int> optional(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_TRUE(optional >= (value - 1));
  EXPECT_TRUE(optional >= value);
  EXPECT_FALSE(optional >= (value + 1));
}


TEST(Test_Optional, SimpleValue_Equal_Value_Value)
{
  constexpr const int value0 = 42;
  constexpr const int value1 = 42;
  constexpr const int value2 = 43;
  Optional<int> optional0(value0);
  Optional<int> optional1(value1);
  Optional<int> optional2(value2);

  EXPECT_TRUE(optional0 == optional1);
  EXPECT_TRUE(optional1 == optional0);
  EXPECT_FALSE(optional0 == optional2);
  EXPECT_FALSE(optional2 == optional0);
}

TEST(Test_Optional, SimpleValue_Equal_Value_Empty)
{
  constexpr const int value0 = 42;
  Optional<int> optional0(value0);
  Optional<int> optional1;

  EXPECT_FALSE(optional0 == optional1);
  EXPECT_FALSE(optional1 == optional0);
}

TEST(Test_Optional, SimpleValue_Equal_Empty_Value)
{
  constexpr const int value0 = 42;
  Optional<int> optional0(value0);
  Optional<int> optional1;

  EXPECT_FALSE(optional1 == optional0);
  EXPECT_FALSE(optional0 == optional1);
}

TEST(Test_Optional, SimpleValue_Equal_Empty_Empty)
{
  Optional<int> optional0;
  Optional<int> optional1;

  EXPECT_TRUE(optional1 == optional0);
  EXPECT_TRUE(optional0 == optional1);
}

TEST(Test_Optional, SimpleValue_NotEqual)
{
  constexpr const int value0 = 42;
  constexpr const int value1 = 43;
  Optional<int> optional0(value0);
  Optional<int> optionalGreater(value1);
  Optional<int> optionalNone;

  EXPECT_TRUE(optional0 != optionalGreater);
  EXPECT_TRUE(optional0 != optionalNone);

  EXPECT_TRUE(optionalGreater != optional0);
  EXPECT_TRUE(optionalGreater != optionalNone);

  EXPECT_TRUE(optionalNone != optional0);
  EXPECT_TRUE(optionalNone != optionalGreater);
}


TEST(Test_Optional, SimpleValue_Less)
{
  constexpr const int valueLess = 41;
  constexpr const int value = 42;
  constexpr const int valueGreater = 43;
  Optional<int> optional(value);
  Optional<int> optionalLess(valueLess);
  Optional<int> optionalGreater(valueGreater);
  Optional<int> optionalNone;

  EXPECT_TRUE(optionalLess < optional);
  EXPECT_FALSE(optional < optional);
  EXPECT_FALSE(optionalGreater < optional);
  EXPECT_TRUE(optionalNone < optional);

  EXPECT_FALSE(optionalLess < optionalLess);
  EXPECT_FALSE(optional < optionalLess);
  EXPECT_FALSE(optionalGreater < optionalLess);
  EXPECT_TRUE(optionalNone < optionalLess);

  EXPECT_TRUE(optionalLess < optionalGreater);
  EXPECT_TRUE(optional < optionalGreater);
  EXPECT_FALSE(optionalGreater < optionalGreater);
  EXPECT_TRUE(optionalNone < optionalGreater);

  EXPECT_FALSE(optionalLess < optionalNone);
  EXPECT_FALSE(optional < optionalNone);
  EXPECT_FALSE(optionalGreater < optionalNone);
  EXPECT_FALSE(optionalNone < optionalNone);
}


TEST(Test_Optional, SimpleValue_LessOrEqual)
{
  constexpr const int valueLess = 41;
  constexpr const int value = 42;
  constexpr const int valueGreater = 43;
  Optional<int> optional(value);
  Optional<int> optionalLess(valueLess);
  Optional<int> optionalGreater(valueGreater);
  Optional<int> optionalNone;

  EXPECT_TRUE(optionalLess <= optional);
  EXPECT_TRUE(optional <= optional);
  EXPECT_FALSE(optionalGreater <= optional);
  EXPECT_TRUE(optionalNone <= optional);

  EXPECT_TRUE(optionalLess <= optionalLess);
  EXPECT_FALSE(optional <= optionalLess);
  EXPECT_FALSE(optionalGreater <= optionalLess);
  EXPECT_TRUE(optionalNone <= optionalLess);

  EXPECT_TRUE(optionalLess <= optionalGreater);
  EXPECT_TRUE(optional <= optionalGreater);
  EXPECT_TRUE(optionalGreater <= optionalGreater);
  EXPECT_TRUE(optionalNone <= optionalGreater);

  EXPECT_FALSE(optionalLess <= optionalNone);
  EXPECT_FALSE(optional <= optionalNone);
  EXPECT_FALSE(optionalGreater <= optionalNone);
  EXPECT_TRUE(optionalNone <= optionalNone);
}


TEST(Test_Optional, SimpleValue_Greater)
{
  constexpr const int valueLess = 41;
  constexpr const int value = 42;
  constexpr const int valueGreater = 43;
  Optional<int> optional(value);
  Optional<int> optionalLess(valueLess);
  Optional<int> optionalGreater(valueGreater);
  Optional<int> optionalNone;

  EXPECT_FALSE(optionalLess > optional);
  EXPECT_FALSE(optional > optional);
  EXPECT_TRUE(optionalGreater > optional);
  EXPECT_FALSE(optionalNone > optional);

  EXPECT_FALSE(optionalLess > optionalLess);
  EXPECT_TRUE(optional > optionalLess);
  EXPECT_TRUE(optionalGreater > optionalLess);
  EXPECT_FALSE(optionalNone > optionalLess);

  EXPECT_FALSE(optionalLess > optionalGreater);
  EXPECT_FALSE(optional > optionalGreater);
  EXPECT_FALSE(optionalGreater > optionalGreater);
  EXPECT_FALSE(optionalNone > optionalGreater);

  EXPECT_TRUE(optionalLess > optionalNone);
  EXPECT_TRUE(optional > optionalNone);
  EXPECT_TRUE(optionalGreater > optionalNone);
  EXPECT_FALSE(optionalNone > optionalNone);
}


TEST(Test_Optional, SimpleValue_GreaterOrEqual)
{
  constexpr const int valueLess = 41;
  constexpr const int value = 42;
  constexpr const int valueGreater = 43;
  Optional<int> optional(value);
  Optional<int> optionalLess(valueLess);
  Optional<int> optionalGreater(valueGreater);
  Optional<int> optionalNone;

  EXPECT_FALSE(optionalLess >= optional);
  EXPECT_TRUE(optional >= optional);
  EXPECT_TRUE(optionalGreater >= optional);
  EXPECT_FALSE(optionalNone >= optional);

  EXPECT_TRUE(optionalLess >= optionalLess);
  EXPECT_TRUE(optional >= optionalLess);
  EXPECT_TRUE(optionalGreater >= optionalLess);
  EXPECT_FALSE(optionalNone >= optionalLess);

  EXPECT_FALSE(optionalLess >= optionalGreater);
  EXPECT_FALSE(optional >= optionalGreater);
  EXPECT_TRUE(optionalGreater >= optionalGreater);
  EXPECT_FALSE(optionalNone >= optionalGreater);

  EXPECT_TRUE(optionalLess >= optionalNone);
  EXPECT_TRUE(optional >= optionalNone);
  EXPECT_TRUE(optionalGreater >= optionalNone);
  EXPECT_TRUE(optionalNone >= optionalNone);
}


TEST(Test_Optional, SimpleValue_ValueOr)
{
  constexpr const int valueOr = 41;
  constexpr const int value = 42;
  Optional<int> optional(value);
  Optional<int> optionalNone;

  EXPECT_EQ(value, optional.ValueOr(valueOr));
  EXPECT_EQ(valueOr, optionalNone.ValueOr(valueOr));
}


TEST(Test_Optional, ComplexValue_Empty)
{
  Optional<std::unique_ptr<int>> optional;

  EXPECT_FALSE(optional.HasValue());
  EXPECT_THROW(optional.Value(), BadOptionalAccess);
}


TEST(Test_Optional, ComplexValue)
{
  constexpr const int value = 42;
  Optional<std::unique_ptr<int>> optional(std::make_unique<int>(value));

  EXPECT_TRUE(optional.HasValue());
  EXPECT_EQ(value, *optional.Value());
  EXPECT_EQ(value, **optional);
}


TEST(Test_Optional, ComplexValue_Assign)
{
  constexpr const int value = 42;
  Optional<std::unique_ptr<int>> optional;

  optional = std::make_unique<int>(value);

  EXPECT_TRUE(optional.HasValue());
  EXPECT_EQ(value, *optional.Value());
  EXPECT_EQ(value, **optional);
}
