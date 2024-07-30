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

#include <FslGraphics/Log/LogColorChannelValueU16.hpp>
#include <FslGraphics/Log/LogColorChannelValueU8.hpp>
#include <FslGraphics/PackedColor32.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <cmath>

using namespace Fsl;

namespace
{
  using Test_PackedColor32 = TestFixtureFslGraphics;


  uint8_t RawU16ToRawU8(const uint32_t col)
  {
    const double normalizedValue = static_cast<double>(MathHelper::Clamp(col, 0u, 0xFFFFu)) / static_cast<double>(0xFFFF);
    const auto roundedValue = static_cast<int32_t>(std::round(normalizedValue * 255));
    return NumericCast<uint8_t>(roundedValue);
  }

  uint8_t RawU16ToRawU8(const int32_t col)
  {
    return RawU16ToRawU8(NumericCast<uint32_t>(col));
  };

  ColorChannelValueU8 AsU16(const ColorChannelValueU16 col)
  {
    return ColorChannelValueU8::Create(RawU16ToRawU8(col.RawValue));
  }
}


TEST(Test_PackedColor32, Construct_Default)
{
  PackedColor32 color;

  EXPECT_EQ(0u, color.RawR());
  EXPECT_EQ(0u, color.RawG());
  EXPECT_EQ(0u, color.RawB());
  EXPECT_EQ(0u, color.RawA());
}


TEST(Test_PackedColor32, Construct)
{
  PackedColor32 color(0x12345678);

  EXPECT_EQ(0x12u, color.RawA());
  EXPECT_EQ(0x34u, color.RawR());
  EXPECT_EQ(0x56u, color.RawG());
  EXPECT_EQ(0x78u, color.RawB());
}


TEST(Test_PackedColor32, ConstructU8)
{
  const ColorChannelValueU8 r(0x12);
  const ColorChannelValueU8 g(0x34);
  const ColorChannelValueU8 b(0x56);
  const ColorChannelValueU8 a(0x78);

  PackedColor32 color(r, g, b, a);

  EXPECT_EQ(r, color.R());
  EXPECT_EQ(g, color.G());
  EXPECT_EQ(b, color.B());
  EXPECT_EQ(a, color.A());

  for (uint32_t i = 0; i < 0xFF; ++i)
  {
    const ColorChannelValueU8 valI(i);
    const ColorChannelValueU8 valA(0x12);
    const ColorChannelValueU8 valB(0x34);
    const ColorChannelValueU8 valC(0x56);
    const ColorChannelValueU8 valD(0x78);

    PackedColor32 color1(valI, valB, valC, valD);
    PackedColor32 color2(valA, valI, valC, valD);
    PackedColor32 color3(valA, valB, valI, valD);
    PackedColor32 color4(valA, valB, valC, valI);

    ASSERT_EQ(valI, color1.R());
    ASSERT_EQ(valB, color1.G());
    ASSERT_EQ(valC, color1.B());
    ASSERT_EQ(valD, color1.A());

    ASSERT_EQ(valA, color2.R());
    ASSERT_EQ(valI, color2.G());
    ASSERT_EQ(valC, color2.B());
    ASSERT_EQ(valD, color2.A());

    ASSERT_EQ(valA, color3.R());
    ASSERT_EQ(valB, color3.G());
    ASSERT_EQ(valI, color3.B());
    ASSERT_EQ(valD, color3.A());

    ASSERT_EQ(valA, color4.R());
    ASSERT_EQ(valB, color4.G());
    ASSERT_EQ(valC, color4.B());
    ASSERT_EQ(valI, color4.A());
  }
}


TEST(Test_PackedColor32, ConstructU16)
{
  const ColorChannelValueU16 r(0x1234);
  const ColorChannelValueU16 g(0x5678);
  const ColorChannelValueU16 b(0x9ABC);
  const ColorChannelValueU16 a(0xDEF0);

  PackedColor32 color(r, g, b, a);

  EXPECT_EQ(AsU16(r), color.R());
  EXPECT_EQ(AsU16(g), color.G());
  EXPECT_EQ(AsU16(b), color.B());
  EXPECT_EQ(AsU16(a), color.A());

  for (uint32_t i = 0; i < 0xFFFF; ++i)
  {
    const ColorChannelValueU16 valI(i);
    const ColorChannelValueU16 valA(0x1234);
    const ColorChannelValueU16 valB(0x5678);
    const ColorChannelValueU16 valC(0x9ABC);
    const ColorChannelValueU16 valD(0xDEF0);

    PackedColor32 color1(valI, valB, valC, valD);
    PackedColor32 color2(valA, valI, valC, valD);
    PackedColor32 color3(valA, valB, valI, valD);
    PackedColor32 color4(valA, valB, valC, valI);

    ASSERT_EQ(AsU16(valI), color1.R());
    ASSERT_EQ(AsU16(valB), color1.G());
    ASSERT_EQ(AsU16(valC), color1.B());
    ASSERT_EQ(AsU16(valD), color1.A());

    ASSERT_EQ(AsU16(valA), color2.R());
    ASSERT_EQ(AsU16(valI), color2.G());
    ASSERT_EQ(AsU16(valC), color2.B());
    ASSERT_EQ(AsU16(valD), color2.A());

    ASSERT_EQ(AsU16(valA), color3.R());
    ASSERT_EQ(AsU16(valB), color3.G());
    ASSERT_EQ(AsU16(valI), color3.B());
    ASSERT_EQ(AsU16(valD), color3.A());

    ASSERT_EQ(AsU16(valA), color4.R());
    ASSERT_EQ(AsU16(valB), color4.G());
    ASSERT_EQ(AsU16(valC), color4.B());
    ASSERT_EQ(AsU16(valI), color4.A());
  }
}


TEST(Test_PackedColor32, Construct_Value)
{
  PackedColor32 color1(0xFF010203);
  PackedColor32 color2(0x01FF0203);
  PackedColor32 color3(0x0102FF03);
  PackedColor32 color4(0x010203FF);

  EXPECT_EQ(0xFFu, color1.RawA());
  EXPECT_EQ(0x01u, color1.RawR());
  EXPECT_EQ(0x02u, color1.RawG());
  EXPECT_EQ(0x03u, color1.RawB());

  EXPECT_EQ(0x01u, color2.RawA());
  EXPECT_EQ(0xFFu, color2.RawR());
  EXPECT_EQ(0x02u, color2.RawG());
  EXPECT_EQ(0x03u, color2.RawB());

  EXPECT_EQ(0x01u, color3.RawA());
  EXPECT_EQ(0x02u, color3.RawR());
  EXPECT_EQ(0xFFu, color3.RawG());
  EXPECT_EQ(0x03u, color3.RawB());

  EXPECT_EQ(0x01u, color4.RawA());
  EXPECT_EQ(0x02u, color4.RawR());
  EXPECT_EQ(0x03u, color4.RawG());
  EXPECT_EQ(0xFFu, color4.RawB());
}

TEST(Test_PackedColor32, Create)
{
  PackedColor32 color = PackedColor32::Create(0x12345678);

  EXPECT_EQ(0x12u, color.RawA());
  EXPECT_EQ(0x34u, color.RawR());
  EXPECT_EQ(0x56u, color.RawG());
  EXPECT_EQ(0x78u, color.RawB());
}


TEST(Test_PackedColor32, CreateR8G8B8A8UNorm_UInt8)
{
  const uint8_t r = 0x12;
  const uint8_t g = 0x34;
  const uint8_t b = 0x56;
  const uint8_t a = 0x78;

  PackedColor32 color = PackedColor32::CreateR8G8B8A8UNorm(r, g, b, a);

  EXPECT_EQ(r, color.RawR());
  EXPECT_EQ(g, color.RawG());
  EXPECT_EQ(b, color.RawB());
  EXPECT_EQ(a, color.RawA());
}


TEST(Test_PackedColor32, CreateR8G8B8A8UNorm_Int16)
{
  const int16_t r = 0x12;
  const int16_t g = 0x34;
  const int16_t b = 0x56;
  const int16_t a = 0x78;

  PackedColor32 color = PackedColor32::CreateR8G8B8A8UNorm(r, g, b, a);

  EXPECT_EQ(r, color.RawR());
  EXPECT_EQ(g, color.RawG());
  EXPECT_EQ(b, color.RawB());
  EXPECT_EQ(a, color.RawA());
}


TEST(Test_PackedColor32, CreateR8G8B8A8UNorm_UInt16)
{
  const uint16_t r = 0x12;
  const uint16_t g = 0x34;
  const uint16_t b = 0x56;
  const uint16_t a = 0x78;

  PackedColor32 color = PackedColor32::CreateR8G8B8A8UNorm(r, g, b, a);

  EXPECT_EQ(r, color.RawR());
  EXPECT_EQ(g, color.RawG());
  EXPECT_EQ(b, color.RawB());
  EXPECT_EQ(a, color.RawA());
}


TEST(Test_PackedColor32, CreateR8G8B8A8UNorm_Int32)
{
  const int32_t r = 0x12;
  const int32_t g = 0x34;
  const int32_t b = 0x56;
  const int32_t a = 0x78;

  PackedColor32 color = PackedColor32::CreateR8G8B8A8UNorm(r, g, b, a);

  EXPECT_EQ(r, color.RawR());
  EXPECT_EQ(g, color.RawG());
  EXPECT_EQ(b, color.RawB());
  EXPECT_EQ(a, color.RawA());
}


TEST(Test_PackedColor32, CreateR8G8B8A8UNorm_UInt32)
{
  const uint32_t r = 0x12;
  const uint32_t g = 0x34;
  const uint32_t b = 0x56;
  const uint32_t a = 0x78;

  PackedColor32 color = PackedColor32::CreateR8G8B8A8UNorm(r, g, b, a);

  EXPECT_EQ(r, color.RawR());
  EXPECT_EQ(g, color.RawG());
  EXPECT_EQ(b, color.RawB());
  EXPECT_EQ(a, color.RawA());
}


TEST(Test_PackedColor32, CreateR16G16B16A16UNorm_UInt16)
{
  const uint16_t r = 0x1234;
  const uint16_t g = 0x5678;
  const uint16_t b = 0x9ABC;
  const uint16_t a = 0xDEF0;

  PackedColor32 color = PackedColor32::CreateR16G16B16A16UNorm(r, g, b, a);

  EXPECT_EQ(RawU16ToRawU8(r), color.RawR());
  EXPECT_EQ(RawU16ToRawU8(g), color.RawG());
  EXPECT_EQ(RawU16ToRawU8(b), color.RawB());
  EXPECT_EQ(RawU16ToRawU8(a), color.RawA());
}


TEST(Test_PackedColor32, CreateR16G16B16A16UNorm_Int32)
{
  const int32_t r = 0x1234;
  const int32_t g = 0x5678;
  const int32_t b = 0x9ABC;
  const int32_t a = 0xDEF0;

  PackedColor32 color = PackedColor32::CreateR16G16B16A16UNorm(r, g, b, a);

  EXPECT_EQ(RawU16ToRawU8(r), color.RawR());
  EXPECT_EQ(RawU16ToRawU8(g), color.RawG());
  EXPECT_EQ(RawU16ToRawU8(b), color.RawB());
  EXPECT_EQ(RawU16ToRawU8(a), color.RawA());
}


TEST(Test_PackedColor32, CreateR16G16B16A16UNorm_UInt32)
{
  const uint32_t r = 0x1234;
  const uint32_t g = 0x5678;
  const uint32_t b = 0x9ABC;
  const uint32_t a = 0xDEF0;

  PackedColor32 color = PackedColor32::CreateR16G16B16A16UNorm(r, g, b, a);

  EXPECT_EQ(RawU16ToRawU8(r), color.RawR());
  EXPECT_EQ(RawU16ToRawU8(g), color.RawG());
  EXPECT_EQ(RawU16ToRawU8(b), color.RawB());
  EXPECT_EQ(RawU16ToRawU8(a), color.RawA());
}


TEST(Test_PackedColor32, Op_Equal)
{
  PackedColor32 color1(0x12345678);
  PackedColor32 color2(0x12345678);
  PackedColor32 color3(0x22345678);

  EXPECT_TRUE(color1 == color2);
  EXPECT_FALSE(color1 == color3);
}


TEST(Test_PackedColor32, Op_NotEqual)
{
  PackedColor32 color1(0x12345678);
  PackedColor32 color2(0x22345678);

  EXPECT_TRUE(color1 != color2);
  EXPECT_TRUE(color2 != color1);
  EXPECT_FALSE(color1 != color1);
  EXPECT_FALSE(color2 != color2);
}
