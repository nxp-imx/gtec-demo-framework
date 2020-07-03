/****************************************************************************************************************************************************
 * Copyright 2017 NXP
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

#include <FslBase/String/StringToValue.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestString_StringToValue = TestFixtureFslBase;
}


TEST(TestString_StringToValue, TryParseEmpty)
{
  uint64_t result = 0xABCDDCBA;

  EXPECT_FALSE(StringToValue::TryParse(result, ""));
  EXPECT_EQ(0u, result);
}


TEST(TestString_StringToValue, TryParse)
{
  uint64_t result = 0xABCDDCBA;

  EXPECT_TRUE(StringToValue::TryParse(result, "1"));
  EXPECT_EQ(1u, result);

  result = 0xABCDDCBA;

  EXPECT_TRUE(StringToValue::TryParse(result, "100"));
  EXPECT_EQ(100u, result);
  EXPECT_TRUE(StringToValue::TryParse(result, "4294967296"));
  EXPECT_EQ(4294967296ul, result);

  EXPECT_TRUE(StringToValue::TryParse(result, "1152921504606846976"));
  EXPECT_EQ(1152921504606846976ul, result);

  EXPECT_TRUE(StringToValue::TryParse(result, "18446744073709551615"));
  EXPECT_EQ(18446744073709551615ul, result);
}


TEST(TestString_StringToValue, TryParse_WithExtra)
{
  uint64_t result = 0xABCDDCBA;

  EXPECT_FALSE(StringToValue::TryParse(result, "1a"));
  EXPECT_EQ(0u, result);
}

TEST(TestString_StringToValue, TryParse_WithExtra2)
{
  uint64_t result = 0xABCDDCBA;

  EXPECT_FALSE(StringToValue::TryParse(result, "100."));
  EXPECT_EQ(0u, result);
}
