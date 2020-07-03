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

#include <FslBase/Compression/ValueCompression_Span.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestCompression_ValueCompression_Span = TestFixtureFslBase;
}

TEST(TestCompression_ValueCompression_Span, ReadInt32)
{
  constexpr std::array<uint8_t, 5> temp{0, 1, 2, 3, 4};
  ReadOnlySpan<uint8_t> remainingSpan(temp.data(), temp.size());

  EXPECT_EQ(0, ValueCompression::ReadSimpleInt32(remainingSpan));
  EXPECT_EQ(-1, ValueCompression::ReadSimpleInt32(remainingSpan));
  EXPECT_EQ(1, ValueCompression::ReadSimpleInt32(remainingSpan));
  EXPECT_EQ(-2, ValueCompression::ReadSimpleInt32(remainingSpan));
  EXPECT_EQ(2, ValueCompression::ReadSimpleInt32(remainingSpan));
  EXPECT_EQ(0u, remainingSpan.size());
}

TEST(TestCompression_ValueCompression_Span, ReadUInt32)
{
  constexpr std::array<uint8_t, 5> temp{0, 1, 2, 3, 4};
  ReadOnlySpan<uint8_t> remainingSpan(temp.data(), temp.size());

  EXPECT_EQ(0u, ValueCompression::ReadSimpleUInt32(remainingSpan));
  EXPECT_EQ(1u, ValueCompression::ReadSimpleUInt32(remainingSpan));
  EXPECT_EQ(2u, ValueCompression::ReadSimpleUInt32(remainingSpan));
  EXPECT_EQ(3u, ValueCompression::ReadSimpleUInt32(remainingSpan));
  EXPECT_EQ(4u, ValueCompression::ReadSimpleUInt32(remainingSpan));
  EXPECT_EQ(0u, remainingSpan.size());
}

TEST(TestCompression_ValueCompression_Span, ReadInt16)
{
  constexpr std::array<uint8_t, 5> temp{0, 1, 2, 3, 4};
  ReadOnlySpan<uint8_t> remainingSpan(temp.data(), temp.size());

  EXPECT_EQ(0, ValueCompression::ReadSimpleInt16(remainingSpan));
  EXPECT_EQ(-1, ValueCompression::ReadSimpleInt16(remainingSpan));
  EXPECT_EQ(1, ValueCompression::ReadSimpleInt16(remainingSpan));
  EXPECT_EQ(-2, ValueCompression::ReadSimpleInt16(remainingSpan));
  EXPECT_EQ(2, ValueCompression::ReadSimpleInt16(remainingSpan));
  EXPECT_EQ(0u, remainingSpan.size());
}

TEST(TestCompression_ValueCompression_Span, ReadUInt16)
{
  constexpr std::array<uint8_t, 5> temp{0, 1, 2, 3, 4};
  ReadOnlySpan<uint8_t> remainingSpan(temp.data(), temp.size());

  EXPECT_EQ(0u, ValueCompression::ReadSimpleUInt16(remainingSpan));
  EXPECT_EQ(1u, ValueCompression::ReadSimpleUInt16(remainingSpan));
  EXPECT_EQ(2u, ValueCompression::ReadSimpleUInt16(remainingSpan));
  EXPECT_EQ(3u, ValueCompression::ReadSimpleUInt16(remainingSpan));
  EXPECT_EQ(4u, ValueCompression::ReadSimpleUInt16(remainingSpan));
  EXPECT_EQ(0u, remainingSpan.size());
}


TEST(TestCompression_ValueCompression_Span, WriteSimpleReadSimple_Unsigned)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint32_t, 11> src = {1,      1337,  1,       std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::min(), 127, 984545, 23,
                                  454356, 40000, 0x200000};

  Span<uint8_t> dstSpan(temp.data(), temp.size());
  ReadOnlySpan<uint8_t> remainingSpan(temp.data(), temp.size());

  std::size_t currentOffset = 0;
  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(dstSpan, currentOffset, src[i]);
    uint32_t result = 0;
    auto bytesRead = ValueCompression::ReadSimple(result, dstSpan, currentOffset);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
    result = ValueCompression::ReadSimpleUInt32(remainingSpan);
    EXPECT_EQ(src[i], result);
    currentOffset += bytesWritten;
  }
}


TEST(TestCompression_ValueCompression_Span, WriteSimpleReadSimple_Signed)
{
  std::array<uint8_t, 512> temp{};
  std::array<int32_t, 11> src = {1,       1337,  -1,      std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min(), 127, 984545, -23,
                                 -454356, 40000, 0x200000};
  Span<uint8_t> dstSpan(temp.data(), temp.size());
  ReadOnlySpan<uint8_t> remainingSpan(temp.data(), temp.size());

  std::size_t currentOffset = 0;
  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(dstSpan, currentOffset, src[i]);
    int32_t result = 0;
    auto bytesRead = ValueCompression::ReadSimple(result, dstSpan, currentOffset);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
    result = ValueCompression::ReadSimpleInt32(remainingSpan);
    EXPECT_EQ(src[i], result);
    currentOffset += bytesWritten;
  }
}
