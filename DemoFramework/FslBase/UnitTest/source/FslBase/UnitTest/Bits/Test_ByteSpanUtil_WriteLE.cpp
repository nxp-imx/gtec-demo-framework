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
#include <FslBase/Bits/ByteSpanUtil_WriteLE.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Span/SpanUtil_ValueCompare.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBits_ByteSpanUtil_WriteLE = TestFixtureFslBase;
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteUInt8LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt8LE(srcSpan, i);
    ByteSpanUtil::WriteUInt8LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt8LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_TRUE(SpanUtil::ValueEquals(srcSpan, dstSpan.AsReadOnlySpan()));
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteUInt16LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt16LE(srcSpan, i);
    ByteSpanUtil::WriteUInt16LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt16LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteUInt32LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt32LE(srcSpan, i);
    ByteSpanUtil::WriteUInt32LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt32LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteUInt64LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt64LE(srcSpan, i);
    ByteSpanUtil::WriteUInt64LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt64LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteInt8LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadInt8LE(srcSpan, i);
    ByteSpanUtil::WriteInt8LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt8LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_TRUE(SpanUtil::ValueEquals(srcSpan, dstSpan.AsReadOnlySpan()));
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteInt16LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadInt16LE(srcSpan, i);
    ByteSpanUtil::WriteInt16LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt16LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteInt32LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadInt32LE(srcSpan, i);
    ByteSpanUtil::WriteInt32LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt32LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteInt64LE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadInt64LE(srcSpan, i);
    ByteSpanUtil::WriteInt64LE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt64LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


// -----------------------------------

TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_UInt8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt8LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt8LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_TRUE(SpanUtil::ValueEquals(srcSpan, dstSpan.AsReadOnlySpan()));
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteWriteLE_UInt16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt16LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt16LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_UInt32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt32LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt32LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_UInt64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt64LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt64LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Int8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadInt8LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt8LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_TRUE(SpanUtil::ValueEquals(srcSpan, dstSpan.AsReadOnlySpan()));
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Int16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadInt16LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt16LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Int32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadInt32LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt32LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Int64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadInt64LE(srcSpan, i);
    ByteSpanUtil::WriteLE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt64LE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Float)
{
  const float src = 1337.2f;
  std::array<uint8_t, 16> dst{};
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  // 0x66, 0x26, 0xA7, 0x44
  EXPECT_EQ(4u, ByteSpanUtil::WriteLE(dstSpan, src));
  EXPECT_EQ(0x66u, dstSpan[0]);
  EXPECT_EQ(0x26u, dstSpan[1]);
  EXPECT_EQ(0xA7u, dstSpan[2]);
  EXPECT_EQ(0x44u, dstSpan[3]);
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Float2)
{
  std::array<float, 9> src{1.0f,
                           0.5f,
                           1337.0f,
                           0.9834985f,
                           std::numeric_limits<float>::infinity(),
                           std::numeric_limits<float>::min(),
                           std::numeric_limits<float>::max(),
                           std::numeric_limits<float>::lowest(),
                           std::numeric_limits<float>::epsilon()};

  std::array<uint8_t, sizeof(float) * src.size()> dst{};
  Span<uint8_t> dstSpan(dst.data(), dst.size());
  ReadOnlySpan<uint8_t> srcSpan(dst.data(), dst.size());

  for (const auto& entry : src)
  {
    // Write the entry to memory, then read it back again
    EXPECT_EQ(4u, ByteSpanUtil::WriteLE(dstSpan, entry));
    EXPECT_EQ(entry, ByteSpanUtil::ReadFloatLE(srcSpan));
  }
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Double)
{
  const double src = 123456789.1;
  std::array<uint8_t, 16> dst{};
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  // 0x419d6f3454666666
  // 0x66, 0x66, 0x66, 0x54, 0x34, 0x6f, 0x9d, 0x41
  EXPECT_EQ(8u, ByteSpanUtil::WriteLE(dstSpan, src));
  EXPECT_EQ(0x66u, dstSpan[0]);
  EXPECT_EQ(0x66u, dstSpan[1]);
  EXPECT_EQ(0x66u, dstSpan[2]);
  EXPECT_EQ(0x54u, dstSpan[3]);
  EXPECT_EQ(0x34u, dstSpan[4]);
  EXPECT_EQ(0x6Fu, dstSpan[5]);
  EXPECT_EQ(0x9Du, dstSpan[6]);
  EXPECT_EQ(0x41u, dstSpan[7]);
}


TEST(TestBits_ByteSpanUtil_WriteLE, WriteLE_Double2)
{
  std::array<double, 9> src{1.0,
                            0.5,
                            1337.0,
                            0.9834985,
                            std::numeric_limits<double>::infinity(),
                            std::numeric_limits<double>::min(),
                            std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::lowest(),
                            std::numeric_limits<double>::epsilon()};

  std::array<uint8_t, sizeof(double) * src.size()> dst{};
  Span<uint8_t> dstSpan(dst.data(), dst.size());
  ReadOnlySpan<uint8_t> srcSpan(dst.data(), dst.size());

  for (const auto& entry : src)
  {
    // Write the entry to memory, then read it back again
    EXPECT_EQ(8u, ByteSpanUtil::WriteLE(dstSpan, entry));
    EXPECT_EQ(entry, ByteSpanUtil::ReadDoubleLE(srcSpan));
  }
}
