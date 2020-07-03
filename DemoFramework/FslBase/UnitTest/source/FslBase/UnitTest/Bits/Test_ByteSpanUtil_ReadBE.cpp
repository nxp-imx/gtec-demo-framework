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

#include <FslBase/Bits/ByteSpanUtil_ReadBE.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBits_ByteSpanUtil_ReadBE = TestFixtureFslBase;
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadUInt8BE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteSpanUtil::ReadUInt8BE(contentSpan, i));
  }
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadUInt16BE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(0x0102u, ByteSpanUtil::ReadUInt16BE(contentSpan, 0));
  EXPECT_EQ(0x0203u, ByteSpanUtil::ReadUInt16BE(contentSpan, 1));
  EXPECT_EQ(0x03FFu, ByteSpanUtil::ReadUInt16BE(contentSpan, 2));
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadUInt32BE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(0x010203FFu, ByteSpanUtil::ReadUInt32BE(contentSpan, 0));
  EXPECT_EQ(0x0203FF13u, ByteSpanUtil::ReadUInt32BE(contentSpan, 1));
  EXPECT_EQ(0x03FF1337u, ByteSpanUtil::ReadUInt32BE(contentSpan, 2));
  EXPECT_EQ(0xFF133742u, ByteSpanUtil::ReadUInt32BE(contentSpan, 3));
  EXPECT_EQ(0x1337427Fu, ByteSpanUtil::ReadUInt32BE(contentSpan, 4));
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadUInt64BE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(0x010203FF1337427Fu, ByteSpanUtil::ReadUInt64BE(contentSpan, 0));
  EXPECT_EQ(0x0203FF1337427F10u, ByteSpanUtil::ReadUInt64BE(contentSpan, 1));
  EXPECT_EQ(0x03FF1337427F1011u, ByteSpanUtil::ReadUInt64BE(contentSpan, 2));
  EXPECT_EQ(0xFF1337427F101112u, ByteSpanUtil::ReadUInt64BE(contentSpan, 3));
  EXPECT_EQ(0x1337427F10111213u, ByteSpanUtil::ReadUInt64BE(contentSpan, 4));
  EXPECT_EQ(0x37427F1011121314u, ByteSpanUtil::ReadUInt64BE(contentSpan, 5));
  EXPECT_EQ(0x427F101112131415u, ByteSpanUtil::ReadUInt64BE(contentSpan, 6));
  EXPECT_EQ(0x7F10111213141516u, ByteSpanUtil::ReadUInt64BE(contentSpan, 7));
  EXPECT_EQ(0x1011121314151617u, ByteSpanUtil::ReadUInt64BE(contentSpan, 8));
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadInt8BE)
{
  std::array<int8_t, 4> content = {0x01, 0x02, 0x03, -1};
  const ReadOnlySpan<uint8_t> contentSpan(reinterpret_cast<const uint8_t*>(content.data()), content.size());

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteSpanUtil::ReadInt8BE(contentSpan, i));
  }
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadInt16BE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0102u)), ByteSpanUtil::ReadInt16BE(contentSpan, 0));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0203u)), ByteSpanUtil::ReadInt16BE(contentSpan, 1));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x03FFu)), ByteSpanUtil::ReadInt16BE(contentSpan, 2));
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadInt32BE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x010203FFu)), ByteSpanUtil::ReadInt32BE(contentSpan, 0));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x0203FF13u)), ByteSpanUtil::ReadInt32BE(contentSpan, 1));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x03FF1337u)), ByteSpanUtil::ReadInt32BE(contentSpan, 2));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0xFF133742u)), ByteSpanUtil::ReadInt32BE(contentSpan, 3));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x1337427Fu)), ByteSpanUtil::ReadInt32BE(contentSpan, 4));
}


TEST(TestBits_ByteSpanUtil_ReadBE, ReadInt64BE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x010203FF1337427Fu)), ByteSpanUtil::ReadInt64BE(contentSpan, 0));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x0203FF1337427F10u)), ByteSpanUtil::ReadInt64BE(contentSpan, 1));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x03FF1337427F1011u)), ByteSpanUtil::ReadInt64BE(contentSpan, 2));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0xFF1337427F101112u)), ByteSpanUtil::ReadInt64BE(contentSpan, 3));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1337427F10111213u)), ByteSpanUtil::ReadInt64BE(contentSpan, 4));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x37427F1011121314u)), ByteSpanUtil::ReadInt64BE(contentSpan, 5));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x427F101112131415u)), ByteSpanUtil::ReadInt64BE(contentSpan, 6));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x7F10111213141516u)), ByteSpanUtil::ReadInt64BE(contentSpan, 7));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1011121314151617u)), ByteSpanUtil::ReadInt64BE(contentSpan, 8));
}
