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

#include <FslBase/NumericCast.hpp>
#include <FslGraphics/ColorChannelValueU8.hpp>
#include <FslGraphics/Log/LogColorChannelValueU8.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_ColorChannelValueU8 = TestFixtureFslGraphics;
}


TEST(Test_ColorChannelValueU8, Construct_Empty)
{
  ColorChannelValueU8 color;

  EXPECT_EQ(0u, color.RawValue);
}


TEST(Test_ColorChannelValueU8, Construct)
{
  ColorChannelValueU8 color(0xFF);

  EXPECT_EQ(0xFF, color.RawValue);
}


TEST(Test_ColorChannelValueU8, Create_UInt8)
{
  const uint8_t val = 0xFF;
  ColorChannelValueU8 color = ColorChannelValueU8::Create(val);

  EXPECT_EQ(val, color.RawValue);
}


TEST(Test_ColorChannelValueU8, Create_Int16)
{
  const int16_t val = 0xFF;
  ColorChannelValueU8 color = ColorChannelValueU8::Create(val);

  EXPECT_EQ(NumericCast<uint8_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU8, Create_UInt16)
{
  const uint16_t val = 0xFF;
  ColorChannelValueU8 color = ColorChannelValueU8::Create(val);

  EXPECT_EQ(NumericCast<uint8_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU8, Create_Int32)
{
  const int32_t val = 0xFF;
  ColorChannelValueU8 color = ColorChannelValueU8::Create(val);

  EXPECT_EQ(NumericCast<uint8_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU8, Create_UInt32)
{
  const uint32_t val = 0xFF;
  ColorChannelValueU8 color = ColorChannelValueU8::Create(val);

  EXPECT_EQ(NumericCast<uint8_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU8, Equals)
{
  ColorChannelValueU8 col1(0x12);
  ColorChannelValueU8 col2(0x12);
  ColorChannelValueU8 col3(0x23);

  EXPECT_TRUE(col1 == col1);
  EXPECT_TRUE(col2 == col2);
  EXPECT_TRUE(col1 == col2);
  EXPECT_TRUE(col2 == col1);
  EXPECT_FALSE(col1 == col3);
  EXPECT_FALSE(col3 == col1);
}


TEST(Test_ColorChannelValueU8, NotEquals)
{
  ColorChannelValueU8 col1(0x12);
  ColorChannelValueU8 col2(0x12);
  ColorChannelValueU8 col3(0x23);

  EXPECT_FALSE(col1 != col1);
  EXPECT_FALSE(col2 != col2);
  EXPECT_FALSE(col1 != col2);
  EXPECT_FALSE(col2 != col1);
  EXPECT_TRUE(col1 != col3);
  EXPECT_TRUE(col3 != col1);
}
