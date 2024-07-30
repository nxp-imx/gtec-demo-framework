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
#include <FslGraphics/ColorChannelValueU16.hpp>
#include <FslGraphics/Log/LogColorChannelValueU16.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_ColorChannelValueU16 = TestFixtureFslGraphics;
}


TEST(Test_ColorChannelValueU16, Construct_Empty)
{
  ColorChannelValueU16 color;

  EXPECT_EQ(0u, color.RawValue);
}


TEST(Test_ColorChannelValueU16, Construct)
{
  ColorChannelValueU16 color(0xFFFF);

  EXPECT_EQ(0xFFFF, color.RawValue);
}


TEST(Test_ColorChannelValueU16, Create_UInt8)
{
  const uint8_t val = 0xFF;
  ColorChannelValueU16 color = ColorChannelValueU16::Create(val);

  EXPECT_EQ(NumericCast<uint16_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU16, Create_Int16)
{
  const int16_t val = 0x7FFF;
  ColorChannelValueU16 color = ColorChannelValueU16::Create(val);

  EXPECT_EQ(NumericCast<uint16_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU16, Create_UInt16)
{
  const uint16_t val = 0xFFFF;
  ColorChannelValueU16 color = ColorChannelValueU16::Create(val);

  EXPECT_EQ(val, color.RawValue);
}


TEST(Test_ColorChannelValueU16, Create_Int32)
{
  const int32_t val = 0xFFFF;
  ColorChannelValueU16 color = ColorChannelValueU16::Create(val);

  EXPECT_EQ(NumericCast<uint16_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU16, Create_UInt32)
{
  const uint32_t val = 0xFFFF;
  ColorChannelValueU16 color = ColorChannelValueU16::Create(val);

  EXPECT_EQ(NumericCast<uint16_t>(val), color.RawValue);
}


TEST(Test_ColorChannelValueU16, Equals)
{
  ColorChannelValueU16 col1(0x1234);
  ColorChannelValueU16 col2(0x1234);
  ColorChannelValueU16 col3(0x2345);

  EXPECT_TRUE(col1 == col1);
  EXPECT_TRUE(col2 == col2);
  EXPECT_TRUE(col1 == col2);
  EXPECT_TRUE(col2 == col1);
  EXPECT_FALSE(col1 == col3);
  EXPECT_FALSE(col3 == col1);
}


TEST(Test_ColorChannelValueU16, NotEquals)
{
  ColorChannelValueU16 col1(0x1234);
  ColorChannelValueU16 col2(0x1234);
  ColorChannelValueU16 col3(0x2345);

  EXPECT_FALSE(col1 != col1);
  EXPECT_FALSE(col2 != col2);
  EXPECT_FALSE(col1 != col2);
  EXPECT_FALSE(col2 != col1);
  EXPECT_TRUE(col1 != col3);
  EXPECT_TRUE(col3 != col1);
}
