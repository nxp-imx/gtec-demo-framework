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

#include <FslBase/Compression/ValueCompression.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestCompression_ValueCompression = TestFixtureFslBase;
}


TEST(TestCompression_ValueCompression, WriteSimpleReadSimple0_Unsigned)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint32_t, 11> src = {1,      1337,  1,       std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::min(), 127, 984545, 23,
                                  454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    uint32_t result = 0;
    std::size_t bytesRead = ValueCompression::ReadSimple(result, temp.data(), temp.size(), i);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
  }
}


TEST(TestCompression_ValueCompression, WriteSimpleReadSimple1_Unsigned)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint32_t, 11> src = {1,      1337,  1,       std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::min(), 127, 984545, 23,
                                  454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    EXPECT_GE(bytesWritten, 0u);
    const uint32_t result = ValueCompression::ReadSimpleUInt32(temp.data(), temp.size(), i);
    EXPECT_EQ(src[i], result);
  }
}

TEST(TestCompression_ValueCompression, WriteSimpleReadSimple0_UnsignedBoundary)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint32_t, 10> src = {0, 0x7F, 0x80, 0x3FFF, 0x4000, 0x1FFFFF, 0x200000, 0xFFFFFFF, 0x10000000, 0xFFFFFFFF};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    uint32_t result = 0;
    std::size_t bytesRead = ValueCompression::ReadSimple(result, temp.data(), temp.size(), i);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
  }
}

TEST(TestCompression_ValueCompression, WriteSimpleReadSimple1_UnsignedBoundary)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint32_t, 10> src = {0, 0x7F, 0x80, 0x3FFF, 0x4000, 0x1FFFFF, 0x200000, 0xFFFFFFF, 0x10000000, 0xFFFFFFFF};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    EXPECT_GE(bytesWritten, 0u);
    const uint32_t result = ValueCompression::ReadSimpleUInt32(temp.data(), temp.size(), i);
    EXPECT_EQ(src[i], result);
  }
}

TEST(TestCompression_ValueCompression, WriteSimpleReadSimple0_Signed)
{
  std::array<uint8_t, 512> temp{};
  std::array<int32_t, 11> src = {1,       1337,  -1,      std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min(), 127, 984545, -23,
                                 -454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    int32_t result = 0;
    std::size_t bytesRead = ValueCompression::ReadSimple(result, temp.data(), temp.size(), i);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
  }
}

TEST(TestCompression_ValueCompression, WriteSimpleReadSimple1_Signed)
{
  std::array<uint8_t, 512> temp{};
  std::array<int32_t, 11> src = {1,       1337,  -1,      std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min(), 127, 984545, -23,
                                 -454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    EXPECT_GE(bytesWritten, 0u);
    const int32_t result = ValueCompression::ReadSimpleInt32(temp.data(), temp.size(), i);
    EXPECT_EQ(src[i], result);
  }
}


TEST(TestCompression_ValueCompression, WriteSimpleReadSimple0_Unsigned64)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint64_t, 11> src = {1,      1337,  1,       std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::min(), 127, 984545, 23,
                                  454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    uint64_t result = 0;
    std::size_t bytesRead = ValueCompression::ReadSimple(result, temp.data(), temp.size(), i);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
  }
}


TEST(TestCompression_ValueCompression, WriteSimpleReadSimple1_Unsigned64)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint64_t, 11> src = {1,      1337,  1,       std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::min(), 127, 984545, 23,
                                  454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    EXPECT_GE(bytesWritten, 0u);
    const uint64_t result = ValueCompression::ReadSimpleUInt64(temp.data(), temp.size(), i);
    EXPECT_EQ(src[i], result);
  }
}

TEST(TestCompression_ValueCompression, WriteSimpleReadSimple0_Unsigned64Boundary)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint64_t, 18> src = {0,                      // 0
                                  0x7F,                   // 1
                                  0x80,                   // 2
                                  0x3FFF,                 // 3
                                  0x4000,                 // 4
                                  0x1FFFFF,               // 5
                                  0x200000,               // 6
                                  0xFFFFFFF,              // 7
                                  0x10000000,             // 8
                                  0x7FFFFFFFF,            // 9
                                  0x800000000,            // 10
                                  0x3FFFFFFFFFF,          // 11
                                  0x40000000000,          // 12
                                  0x1FFFFFFFFFFFF,        // 13
                                  0x2000000000000,        // 14
                                  0xFFFFFFFFFFFFFF,       // 15
                                  0x100000000000000,      // 16
                                  0xFFFFFFFFFFFFFFFF};    // 17

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    uint64_t result = 0;
    std::size_t bytesRead = ValueCompression::ReadSimple(result, temp.data(), temp.size(), i);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
  }
}


TEST(TestCompression_ValueCompression, WriteSimpleReadSimple1_Unsigned64Boundary)
{
  std::array<uint8_t, 512> temp{};
  std::array<uint64_t, 18> src = {0,                      // 0
                                  0x7F,                   // 1
                                  0x80,                   // 2
                                  0x3FFF,                 // 3
                                  0x4000,                 // 4
                                  0x1FFFFF,               // 5
                                  0x200000,               // 6
                                  0xFFFFFFF,              // 7
                                  0x10000000,             // 8
                                  0x7FFFFFFFF,            // 9
                                  0x800000000,            // 10
                                  0x3FFFFFFFFFF,          // 11
                                  0x40000000000,          // 12
                                  0x1FFFFFFFFFFFF,        // 13
                                  0x2000000000000,        // 14
                                  0xFFFFFFFFFFFFFF,       // 15
                                  0x100000000000000,      // 16
                                  0xFFFFFFFFFFFFFFFF};    // 17

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    EXPECT_GE(bytesWritten, 0u);
    const uint64_t result = ValueCompression::ReadSimpleUInt64(temp.data(), temp.size(), i);
    EXPECT_EQ(src[i], result);
  }
}


TEST(TestCompression_ValueCompression, WriteSimpleReadSimple0_Signed64)
{
  std::array<uint8_t, 512> temp{};
  std::array<int64_t, 11> src = {1,       1337,  -1,      std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min(), 127, 984545, -23,
                                 -454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    int64_t result = 0;
    std::size_t bytesRead = ValueCompression::ReadSimple(result, temp.data(), temp.size(), i);
    EXPECT_EQ(bytesWritten, bytesRead);
    EXPECT_EQ(src[i], result);
  }
}

TEST(TestCompression_ValueCompression, WriteSimpleReadSimple1_Signed64)
{
  std::array<uint8_t, 512> temp{};
  std::array<int64_t, 11> src = {1,       1337,  -1,      std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min(), 127, 984545, -23,
                                 -454356, 40000, 0x200000};

  for (std::size_t i = 0; i < src.size(); ++i)
  {
    std::size_t bytesWritten = ValueCompression::WriteSimple(temp.data(), temp.size(), i, src[i]);
    EXPECT_GE(bytesWritten, 0u);
    const int64_t result = ValueCompression::ReadSimpleInt64(temp.data(), temp.size(), i);
    EXPECT_EQ(src[i], result);
  }
}
