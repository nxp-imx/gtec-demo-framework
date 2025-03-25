/****************************************************************************************************************************************************
 * Copyright 2020, 2025 NXP
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

#include <FslBase/Bits/ByteSpanUtil_ReadLE.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBits_ByteSpanUtil_ReadLE = TestFixtureFslBase;
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadUInt8LE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteSpanUtil::ReadUInt8LE(contentSpan, i));
  }
}

TEST(TestBits_ByteSpanUtil_ReadLE, ReadUInt16LE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(0x0201u, ByteSpanUtil::ReadUInt16LE(contentSpan, 0));
  EXPECT_EQ(0x0302u, ByteSpanUtil::ReadUInt16LE(contentSpan, 1));
  EXPECT_EQ(0xFF03u, ByteSpanUtil::ReadUInt16LE(contentSpan, 2));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadUInt32LE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(0xFF030201u, ByteSpanUtil::ReadUInt32LE(contentSpan, 0));
  EXPECT_EQ(0x13FF0302u, ByteSpanUtil::ReadUInt32LE(contentSpan, 1));
  EXPECT_EQ(0x3713FF03u, ByteSpanUtil::ReadUInt32LE(contentSpan, 2));
  EXPECT_EQ(0x423713FFu, ByteSpanUtil::ReadUInt32LE(contentSpan, 3));
  EXPECT_EQ(0x7F423713u, ByteSpanUtil::ReadUInt32LE(contentSpan, 4));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadUInt64LE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(0x7F423713FF030201u, ByteSpanUtil::ReadUInt64LE(contentSpan, 0));
  EXPECT_EQ(0x107F423713FF0302u, ByteSpanUtil::ReadUInt64LE(contentSpan, 1));
  EXPECT_EQ(0x11107F423713FF03u, ByteSpanUtil::ReadUInt64LE(contentSpan, 2));
  EXPECT_EQ(0x1211107F423713FFu, ByteSpanUtil::ReadUInt64LE(contentSpan, 3));
  EXPECT_EQ(0x131211107F423713u, ByteSpanUtil::ReadUInt64LE(contentSpan, 4));
  EXPECT_EQ(0x14131211107F4237u, ByteSpanUtil::ReadUInt64LE(contentSpan, 5));
  EXPECT_EQ(0x1514131211107F42u, ByteSpanUtil::ReadUInt64LE(contentSpan, 6));
  EXPECT_EQ(0x161514131211107Fu, ByteSpanUtil::ReadUInt64LE(contentSpan, 7));
  EXPECT_EQ(0x1716151413121110u, ByteSpanUtil::ReadUInt64LE(contentSpan, 8));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadInt8LE)
{
  std::array<int8_t, 4> content = {0x01, 0x02, 0x03, -1};
  const ReadOnlySpan<uint8_t> contentSpan(reinterpret_cast<const uint8_t*>(content.data()), content.size());

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteSpanUtil::ReadInt8LE(contentSpan, i));
  }
}

TEST(TestBits_ByteSpanUtil_ReadLE, ReadInt16LE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0201u)), ByteSpanUtil::ReadInt16LE(contentSpan, 0));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0302u)), ByteSpanUtil::ReadInt16LE(contentSpan, 1));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0xFF03u)), ByteSpanUtil::ReadInt16LE(contentSpan, 2));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadInt32LE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(static_cast<int32_t>(uint32_t(0xFF030201u)), ByteSpanUtil::ReadInt32LE(contentSpan, 0));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x13FF0302u)), ByteSpanUtil::ReadInt32LE(contentSpan, 1));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x3713FF03u)), ByteSpanUtil::ReadInt32LE(contentSpan, 2));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x423713FFu)), ByteSpanUtil::ReadInt32LE(contentSpan, 3));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x7F423713u)), ByteSpanUtil::ReadInt32LE(contentSpan, 4));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadInt64LE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x7F423713FF030201u)), ByteSpanUtil::ReadInt64LE(contentSpan, 0));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x107F423713FF0302u)), ByteSpanUtil::ReadInt64LE(contentSpan, 1));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x11107F423713FF03u)), ByteSpanUtil::ReadInt64LE(contentSpan, 2));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1211107F423713FFu)), ByteSpanUtil::ReadInt64LE(contentSpan, 3));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x131211107F423713u)), ByteSpanUtil::ReadInt64LE(contentSpan, 4));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x14131211107F4237u)), ByteSpanUtil::ReadInt64LE(contentSpan, 5));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1514131211107F42u)), ByteSpanUtil::ReadInt64LE(contentSpan, 6));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x161514131211107Fu)), ByteSpanUtil::ReadInt64LE(contentSpan, 7));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1716151413121110u)), ByteSpanUtil::ReadInt64LE(contentSpan, 8));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadFloatLE)
{
  // float testValue = 1337.2f;
  // uint32_t val = std::bit_cast<uint32_t>(testValue);
  //  0x44A62666
  std::array<uint8_t, 4> content = {0x66, 0x26, 0xA7, 0x44};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(1337.2f, ByteSpanUtil::ReadFloatLE(contentSpan, 0));
}


TEST(TestBits_ByteSpanUtil_ReadLE, ReadDoubleLE)
{
  // double testValue = 123456789.1;
  // uint64_t val = std::bit_cast<uint64_t>(testValue);

  // 0x419d6f3454666666
  std::array<uint8_t, 8> content = {0x66, 0x66, 0x66, 0x54, 0x34, 0x6f, 0x9d, 0x41};
  const ReadOnlySpan<uint8_t> contentSpan(content.data(), content.size());

  EXPECT_EQ(123456789.1, ByteSpanUtil::ReadDoubleLE(contentSpan, 0));
}
