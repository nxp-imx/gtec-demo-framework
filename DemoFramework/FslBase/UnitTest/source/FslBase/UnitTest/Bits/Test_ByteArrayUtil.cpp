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

#include <FslBase/Bits/ByteArrayUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBits_ByteArrayUtil = TestFixtureFslBase;

  template <std::size_t TSize>
  void Expect_EQ(const std::array<uint8_t, TSize>& src, const std::array<uint8_t, TSize>& dst)
  {
    for (std::size_t i = 0; i < src.size(); ++i)
    {
      EXPECT_EQ(src[i], dst[i]);
    }
  }
}


TEST(TestBits_ByteArrayUtil, ReadUInt8LE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteArrayUtil::ReadUInt8LE(content.data(), content.size(), i));
  }
}


TEST(TestBits_ByteArrayUtil, ReadUInt8BE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteArrayUtil::ReadUInt8BE(content.data(), content.size(), i));
  }
}


TEST(TestBits_ByteArrayUtil, ReadUInt16LE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};

  EXPECT_EQ(0x0201u, ByteArrayUtil::ReadUInt16LE(content.data(), content.size(), 0));
  EXPECT_EQ(0x0302u, ByteArrayUtil::ReadUInt16LE(content.data(), content.size(), 1));
  EXPECT_EQ(0xFF03u, ByteArrayUtil::ReadUInt16LE(content.data(), content.size(), 2));
}


TEST(TestBits_ByteArrayUtil, ReadUInt16BE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};

  EXPECT_EQ(0x0102u, ByteArrayUtil::ReadUInt16BE(content.data(), content.size(), 0));
  EXPECT_EQ(0x0203u, ByteArrayUtil::ReadUInt16BE(content.data(), content.size(), 1));
  EXPECT_EQ(0x03FFu, ByteArrayUtil::ReadUInt16BE(content.data(), content.size(), 2));
}


TEST(TestBits_ByteArrayUtil, ReadUInt32LE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};

  EXPECT_EQ(0xFF030201u, ByteArrayUtil::ReadUInt32LE(content.data(), content.size(), 0));
  EXPECT_EQ(0x13FF0302u, ByteArrayUtil::ReadUInt32LE(content.data(), content.size(), 1));
  EXPECT_EQ(0x3713FF03u, ByteArrayUtil::ReadUInt32LE(content.data(), content.size(), 2));
  EXPECT_EQ(0x423713FFu, ByteArrayUtil::ReadUInt32LE(content.data(), content.size(), 3));
  EXPECT_EQ(0x7F423713u, ByteArrayUtil::ReadUInt32LE(content.data(), content.size(), 4));
}


TEST(TestBits_ByteArrayUtil, ReadUInt32BE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};

  EXPECT_EQ(0x010203FFu, ByteArrayUtil::ReadUInt32BE(content.data(), content.size(), 0));
  EXPECT_EQ(0x0203FF13u, ByteArrayUtil::ReadUInt32BE(content.data(), content.size(), 1));
  EXPECT_EQ(0x03FF1337u, ByteArrayUtil::ReadUInt32BE(content.data(), content.size(), 2));
  EXPECT_EQ(0xFF133742u, ByteArrayUtil::ReadUInt32BE(content.data(), content.size(), 3));
  EXPECT_EQ(0x1337427Fu, ByteArrayUtil::ReadUInt32BE(content.data(), content.size(), 4));
}


TEST(TestBits_ByteArrayUtil, ReadUInt64LE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  EXPECT_EQ(0x7F423713FF030201u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 0));
  EXPECT_EQ(0x107F423713FF0302u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 1));
  EXPECT_EQ(0x11107F423713FF03u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 2));
  EXPECT_EQ(0x1211107F423713FFu, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 3));
  EXPECT_EQ(0x131211107F423713u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 4));
  EXPECT_EQ(0x14131211107F4237u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 5));
  EXPECT_EQ(0x1514131211107F42u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 6));
  EXPECT_EQ(0x161514131211107Fu, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 7));
  EXPECT_EQ(0x1716151413121110u, ByteArrayUtil::ReadUInt64LE(content.data(), content.size(), 8));
}


TEST(TestBits_ByteArrayUtil, ReadUInt64BE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  EXPECT_EQ(0x010203FF1337427Fu, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 0));
  EXPECT_EQ(0x0203FF1337427F10u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 1));
  EXPECT_EQ(0x03FF1337427F1011u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 2));
  EXPECT_EQ(0xFF1337427F101112u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 3));
  EXPECT_EQ(0x1337427F10111213u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 4));
  EXPECT_EQ(0x37427F1011121314u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 5));
  EXPECT_EQ(0x427F101112131415u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 6));
  EXPECT_EQ(0x7F10111213141516u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 7));
  EXPECT_EQ(0x1011121314151617u, ByteArrayUtil::ReadUInt64BE(content.data(), content.size(), 8));
}


TEST(TestBits_ByteArrayUtil, ReadInt8LE)
{
  std::array<int8_t, 4> content = {0x01, 0x02, 0x03, -1};

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteArrayUtil::ReadInt8LE(reinterpret_cast<uint8_t*>(content.data()), content.size(), i));
  }
}


TEST(TestBits_ByteArrayUtil, ReadInt8BE)
{
  std::array<int8_t, 4> content = {0x01, 0x02, 0x03, -1};

  for (std::size_t i = 0; i < content.size(); ++i)
  {
    EXPECT_EQ(content[i], ByteArrayUtil::ReadInt8BE(reinterpret_cast<uint8_t*>(content.data()), content.size(), i));
  }
}


TEST(TestBits_ByteArrayUtil, ReadInt16LE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};

  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0201u)), ByteArrayUtil::ReadInt16LE(content.data(), content.size(), 0));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0302u)), ByteArrayUtil::ReadInt16LE(content.data(), content.size(), 1));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0xFF03u)), ByteArrayUtil::ReadInt16LE(content.data(), content.size(), 2));
}


TEST(TestBits_ByteArrayUtil, ReadInt16BE)
{
  std::array<uint8_t, 4> content = {0x01, 0x02, 0x03, 0xFF};

  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0102u)), ByteArrayUtil::ReadInt16BE(content.data(), content.size(), 0));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x0203u)), ByteArrayUtil::ReadInt16BE(content.data(), content.size(), 1));
  EXPECT_EQ(static_cast<int16_t>(uint16_t(0x03FFu)), ByteArrayUtil::ReadInt16BE(content.data(), content.size(), 2));
}


TEST(TestBits_ByteArrayUtil, ReadInt32LE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};

  EXPECT_EQ(static_cast<int32_t>(uint32_t(0xFF030201u)), ByteArrayUtil::ReadInt32LE(content.data(), content.size(), 0));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x13FF0302u)), ByteArrayUtil::ReadInt32LE(content.data(), content.size(), 1));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x3713FF03u)), ByteArrayUtil::ReadInt32LE(content.data(), content.size(), 2));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x423713FFu)), ByteArrayUtil::ReadInt32LE(content.data(), content.size(), 3));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x7F423713u)), ByteArrayUtil::ReadInt32LE(content.data(), content.size(), 4));
}


TEST(TestBits_ByteArrayUtil, ReadInt32BE)
{
  std::array<uint8_t, 8> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F};

  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x010203FFu)), ByteArrayUtil::ReadInt32BE(content.data(), content.size(), 0));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x0203FF13u)), ByteArrayUtil::ReadInt32BE(content.data(), content.size(), 1));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x03FF1337u)), ByteArrayUtil::ReadInt32BE(content.data(), content.size(), 2));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0xFF133742u)), ByteArrayUtil::ReadInt32BE(content.data(), content.size(), 3));
  EXPECT_EQ(static_cast<int32_t>(uint32_t(0x1337427Fu)), ByteArrayUtil::ReadInt32BE(content.data(), content.size(), 4));
}


TEST(TestBits_ByteArrayUtil, ReadInt64LE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x7F423713FF030201u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 0));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x107F423713FF0302u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 1));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x11107F423713FF03u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 2));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1211107F423713FFu)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 3));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x131211107F423713u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 4));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x14131211107F4237u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 5));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1514131211107F42u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 6));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x161514131211107Fu)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 7));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1716151413121110u)), ByteArrayUtil::ReadInt64LE(content.data(), content.size(), 8));
}


TEST(TestBits_ByteArrayUtil, ReadInt64BE)
{
  std::array<uint8_t, 16> content = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x010203FF1337427Fu)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 0));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x0203FF1337427F10u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 1));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x03FF1337427F1011u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 2));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0xFF1337427F101112u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 3));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1337427F10111213u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 4));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x37427F1011121314u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 5));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x427F101112131415u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 6));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x7F10111213141516u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 7));
  EXPECT_EQ(static_cast<int64_t>(uint64_t(0x1011121314151617u)), ByteArrayUtil::ReadInt64BE(content.data(), content.size(), 8));
}


TEST(TestBits_ByteArrayUtil, WriteUInt8LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt8LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt8LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt8LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteUInt8BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt8BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt8BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt8BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteUInt16LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt16LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt16LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt16LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteUInt16BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt16BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt16BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt16BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteUInt32LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt32LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt32LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt32LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteUInt32BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt32BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt32BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt32BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteUInt64LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt64LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt64LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt64LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteUInt64BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt64BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteUInt64BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt64BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteInt8LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadInt8LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt8LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt8LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteInt8BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadInt8BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt8BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt8BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteInt16LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadInt16LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt16LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt16LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteInt16BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadInt16BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt16BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt16BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteInt32LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadInt32LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt32LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt32LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteInt32BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadInt32BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt32BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt32BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteInt64LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadInt64LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt64LE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt64LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteInt64BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadInt64BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteInt64BE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt64BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}

// -----------------------------------

TEST(TestBits_ByteArrayUtil, WriteLE_UInt8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt8LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt8LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteBE_UInt8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt8BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt8BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteWriteLE_UInt16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt16LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt16LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteBE_UInt16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt16BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt16BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteLE_UInt32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt32LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt32LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteBE_UInt32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt32BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt32BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteLE_UInt64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt64LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt64LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteBE_UInt64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadUInt64BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadUInt64BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteLE_Int8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadInt8LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt8LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteBE_Int8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteArrayUtil::ReadInt8BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt8BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
  Expect_EQ(src, dst);
}


TEST(TestBits_ByteArrayUtil, WriteLE_Int16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadInt16LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt16LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteBE_Int16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteArrayUtil::ReadInt16BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt16BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteLE_Int32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadInt32LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt32LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteBE_Int32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteArrayUtil::ReadInt32BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt32BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteLE_Int64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadInt64LE(src.data(), src.size(), i);
    ByteArrayUtil::WriteLE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt64LE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteArrayUtil, WriteBE_Int64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteArrayUtil::ReadInt64BE(src.data(), src.size(), i);
    ByteArrayUtil::WriteBE(dst.data(), dst.size(), i, value);
    auto writtenValue = ByteArrayUtil::ReadInt64BE(dst.data(), dst.size(), i);
    EXPECT_EQ(value, writtenValue);
  }
}

// -----------------------------------

TEST(TestBits_ByteArrayUtil, WriteBytes)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};

  ByteArrayUtil::WriteBytes(dst.data(), dst.size(), 0, src.data(), src.size());

  Expect_EQ(src, dst);
}
