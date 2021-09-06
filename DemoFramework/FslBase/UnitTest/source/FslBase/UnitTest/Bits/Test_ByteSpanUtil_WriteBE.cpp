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
#include <FslBase/Bits/ByteSpanUtil_WriteBE.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBits_ByteSpanUtil_WriteBE = TestFixtureFslBase;
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteUInt8BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt8BE(srcSpan, i);
    ByteSpanUtil::WriteUInt8BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt8BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_EQ(srcSpan, dstSpan.AsReadOnlySpan());
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteUInt16BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt16BE(srcSpan, i);
    ByteSpanUtil::WriteUInt16BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt16BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteUInt32BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt32BE(srcSpan, i);
    ByteSpanUtil::WriteUInt32BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt32BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteUInt64BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt64BE(srcSpan, i);
    ByteSpanUtil::WriteUInt64BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt64BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(v, WriteInt8BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadInt8BE(srcSpan, i);
    ByteSpanUtil::WriteInt8BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt8BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_EQ(srcSpan, dstSpan.AsReadOnlySpan());
}


TEST(v, WriteInt16BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadInt16BE(srcSpan, i);
    ByteSpanUtil::WriteInt16BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt16BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteInt32BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadInt32BE(srcSpan, i);
    ByteSpanUtil::WriteInt32BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt32BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteInt64BE)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadInt64BE(srcSpan, i);
    ByteSpanUtil::WriteInt64BE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt64BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}

// -----------------------------------

TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_UInt8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt8BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt8BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_EQ(srcSpan, dstSpan.AsReadOnlySpan());
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_UInt16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt16BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt16BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_UInt32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt32BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt32BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_UInt64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadUInt64BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadUInt64BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_Int8)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    auto value = ByteSpanUtil::ReadInt8BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt8BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
  EXPECT_EQ(srcSpan, dstSpan.AsReadOnlySpan());
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_Int16)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 1); ++i)
  {
    auto value = ByteSpanUtil::ReadInt16BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt16BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_Int32)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 3); ++i)
  {
    auto value = ByteSpanUtil::ReadInt32BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt32BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}


TEST(TestBits_ByteSpanUtil_WriteBE, WriteBE_Int64)
{
  std::array<uint8_t, 16> src = {0x01, 0x02, 0x03, 0xFF, 0x13, 0x37, 0x42, 0x7F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  std::array<uint8_t, 16> dst{};
  const ReadOnlySpan<uint8_t> srcSpan(src.data(), src.size());
  Span<uint8_t> dstSpan(dst.data(), dst.size());

  for (std::size_t i = 0; i < (src.size() - 7); ++i)
  {
    auto value = ByteSpanUtil::ReadInt64BE(srcSpan, i);
    ByteSpanUtil::WriteBE(dstSpan, i, value);
    auto writtenValue = ByteSpanUtil::ReadInt64BE(dstSpan, i);
    EXPECT_EQ(value, writtenValue);
  }
}
