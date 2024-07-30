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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Log3Fmt.hpp>
#include <FslGraphics/ColorChannelConverter.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_ColorChannelConverter = TestFixtureFslGraphics;
}


TEST(Test_ColorChannelConverter, ReduceRawU16ToRawU8)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const uint8_t linearValue = ColorChannelConverter::ReduceRawU16ToRawU8(static_cast<uint16_t>(i));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) / 257.0));
    ASSERT_EQ(expectedValue, linearValue);
  }
}


TEST(Test_ColorChannelConverter, ExpandRawU8ToRawU16)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const uint16_t linearValue = ColorChannelConverter::ExpandRawU8ToRawU16(static_cast<uint8_t>(i));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) * 257.0));
    ASSERT_EQ(expectedValue, linearValue);
  }
}


TEST(Test_ColorChannelConverter, ReduceRawU16ToU8)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const auto linearValue = ColorChannelConverter::ReduceRawU16ToU8(static_cast<uint16_t>(i));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) / 257.0));
    ASSERT_EQ(expectedValue, linearValue.RawValue);
  }
}

TEST(Test_ColorChannelConverter, ExpandRawU8ToU16)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const auto linearValue = ColorChannelConverter::ExpandRawU8ToU16(static_cast<uint8_t>(i));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) * 257.0));
    ASSERT_EQ(expectedValue, linearValue.RawValue);
  }
}


TEST(Test_ColorChannelConverter, ToU16)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const auto linearValue = ColorChannelConverter::ToU16(ColorChannelValueU8(static_cast<uint8_t>(i)));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) * 257.0));
    ASSERT_EQ(expectedValue, linearValue.RawValue);
  }
}


TEST(Test_ColorChannelConverter, ToU8)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const auto linearValue = ColorChannelConverter::ToU8(ColorChannelValueU16(static_cast<uint16_t>(i)));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) / 257.0));
    ASSERT_EQ(expectedValue, linearValue.RawValue);
  }
}


TEST(Test_ColorChannelConverter, ToRawU16)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const uint16_t linearValue = ColorChannelConverter::ToRawU16(ColorChannelValueU8(static_cast<uint8_t>(i)));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) * 257.0));
    ASSERT_EQ(expectedValue, linearValue);
  }
}


TEST(Test_ColorChannelConverter, ToRawU8)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const uint8_t linearValue = ColorChannelConverter::ToRawU8(ColorChannelValueU16(static_cast<uint16_t>(i)));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) / 257.0));
    ASSERT_EQ(expectedValue, linearValue);
  }
}


TEST(Test_ColorChannelConverter, FromRawU8ToU16)
{
  for (uint32_t i = 0; i <= 0xFF; ++i)
  {
    const ColorChannelValueU16 linearValue = ColorChannelConverter::FromRawU8ToU16(static_cast<uint8_t>(i));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) * 257.0));
    ASSERT_EQ(expectedValue, linearValue.RawValue);
  }
}


TEST(Test_ColorChannelConverter, FromRawU16ToU8)
{
  for (uint32_t i = 0; i <= 0xFFFF; ++i)
  {
    const ColorChannelValueU8 linearValue = ColorChannelConverter::FromRawU16ToU8(static_cast<uint16_t>(i));
    const auto expectedValue = static_cast<int64_t>(std::round(static_cast<double>(i) / 257.0));
    ASSERT_EQ(expectedValue, linearValue.RawValue);
  }
}


TEST(Test_ColorChannelConverter, UncheckedRawF32ToRawU16)
{
  for (uint32_t i = 0; i <= 0xFFFFF; ++i)
  {
    auto valueFloat = static_cast<float>(i) / static_cast<float>(0xFFFFF);
    const uint16_t linearValue = ColorChannelConverter::UncheckedRawF32ToRawU16(valueFloat);
    const auto expectedValue = static_cast<int64_t>(std::round(valueFloat * static_cast<float>(0xFFFF)));
    FSLLOG3_INFO_IF(expectedValue != linearValue, "expectedValue: {} linearValue: {} i={}", expectedValue, linearValue, i);
    ASSERT_EQ(expectedValue, linearValue);
  }
}


TEST(Test_ColorChannelConverter, RawF32ToRawU16)
{
  for (uint32_t i = 0; i <= 0xFFFFF; ++i)
  {
    auto valueFloat = static_cast<float>(i) / static_cast<float>(0xFFFFF);
    const uint16_t linearValue = ColorChannelConverter::RawF32ToRawU16(valueFloat);
    const auto expectedValue = static_cast<int64_t>(std::round(valueFloat * static_cast<float>(0xFFFF)));
    FSLLOG3_INFO_IF(expectedValue != linearValue, "expectedValue: {} linearValue: {} i={}", expectedValue, linearValue, i);
    ASSERT_EQ(expectedValue, linearValue);
  }
}

TEST(Test_ColorChannelConverter, RawF32ToRawU16_Values)
{
  {
    auto valueFloat = 0.0f;
    const uint16_t linearValue = ColorChannelConverter::RawF32ToRawU16(valueFloat);
    const auto expectedValue = static_cast<int64_t>(std::round(valueFloat * static_cast<float>(0xFFFF)));
    ASSERT_EQ(expectedValue, linearValue);
  }
  {
    auto valueFloat = 1.0f;
    const uint16_t linearValue = ColorChannelConverter::RawF32ToRawU16(valueFloat);
    const auto expectedValue = static_cast<int64_t>(std::round(valueFloat * static_cast<float>(0xFFFF)));
    ASSERT_EQ(expectedValue, linearValue);
  }
  {
    auto valueFloat = 9.0f / static_cast<float>(0xFFFFF);
    const uint16_t linearValue = ColorChannelConverter::RawF32ToRawU16(valueFloat);
    const auto expectedValue = static_cast<int64_t>(std::round(valueFloat * static_cast<float>(0xFFFF)));
    ASSERT_EQ(expectedValue, linearValue);
  }
}
