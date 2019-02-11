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

#include <FslBase/Bits/BitsUtil.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestBits_BitsUtil = TestFixtureFslBase;
}


TEST(TestBits_BitsUtil, Count)
{
  EXPECT_EQ(0, BitsUtil::Count(0));

  EXPECT_EQ(1, BitsUtil::Count(0x00000001));
  EXPECT_EQ(1, BitsUtil::Count(0x10000000));

  EXPECT_EQ(2, BitsUtil::Count(0x10000001));

  EXPECT_EQ(32, BitsUtil::Count(0xFFFFFFFF));
}


TEST(TestBits_BitsUtil, IndexOf_unsigned)
{
  EXPECT_EQ(-1, BitsUtil::IndexOf(0u));

  EXPECT_EQ(0, BitsUtil::IndexOf(0x00000001u));
  EXPECT_EQ(1, BitsUtil::IndexOf(0x00000002u));
  EXPECT_EQ(2, BitsUtil::IndexOf(0x00000004u));
  EXPECT_EQ(3, BitsUtil::IndexOf(0x00000008u));
  EXPECT_EQ(4, BitsUtil::IndexOf(0x00000010u));
  EXPECT_EQ(5, BitsUtil::IndexOf(0x00000020u));
  EXPECT_EQ(6, BitsUtil::IndexOf(0x00000040u));
  EXPECT_EQ(7, BitsUtil::IndexOf(0x00000080u));

  EXPECT_EQ(28, BitsUtil::IndexOf(0x10000000u));

  EXPECT_EQ(4, BitsUtil::IndexOf(0x10000010u));

  EXPECT_EQ(0, BitsUtil::IndexOf(0xFFFFFFFFu));
}


TEST(TestBits_BitsUtil, IndexOf_signed)
{
  EXPECT_EQ(-1, BitsUtil::IndexOf(0));

  EXPECT_EQ(0, BitsUtil::IndexOf(0x00000001));
  EXPECT_EQ(1, BitsUtil::IndexOf(0x00000002));
  EXPECT_EQ(2, BitsUtil::IndexOf(0x00000004));
  EXPECT_EQ(3, BitsUtil::IndexOf(0x00000008));
  EXPECT_EQ(4, BitsUtil::IndexOf(0x00000010));
  EXPECT_EQ(5, BitsUtil::IndexOf(0x00000020));
  EXPECT_EQ(6, BitsUtil::IndexOf(0x00000040));
  EXPECT_EQ(7, BitsUtil::IndexOf(0x00000080));

  EXPECT_EQ(28, BitsUtil::IndexOf(0x10000000));

  EXPECT_EQ(4, BitsUtil::IndexOf(0x10000010));

  EXPECT_EQ(0, BitsUtil::IndexOf(0xFFFFFFFF));
}


TEST(TestBits_BitsUtil, NextPowerOfTwo_unsigned)
{
  EXPECT_EQ(1u, BitsUtil::NextPowerOfTwo(0u));
  EXPECT_EQ(1u, BitsUtil::NextPowerOfTwo(1u));
  EXPECT_EQ(2u, BitsUtil::NextPowerOfTwo(2u));
  EXPECT_EQ(4u, BitsUtil::NextPowerOfTwo(3u));
  EXPECT_EQ(4u, BitsUtil::NextPowerOfTwo(4u));
  EXPECT_EQ(8u, BitsUtil::NextPowerOfTwo(5u));
  EXPECT_EQ(8u, BitsUtil::NextPowerOfTwo(6u));
  EXPECT_EQ(8u, BitsUtil::NextPowerOfTwo(7u));
  EXPECT_EQ(8u, BitsUtil::NextPowerOfTwo(8u));
  EXPECT_EQ(16u, BitsUtil::NextPowerOfTwo(9u));

  EXPECT_EQ(32u, BitsUtil::NextPowerOfTwo(31u));
  EXPECT_EQ(32u, BitsUtil::NextPowerOfTwo(32u));
  EXPECT_EQ(64u, BitsUtil::NextPowerOfTwo(33u));

  EXPECT_EQ(128u, BitsUtil::NextPowerOfTwo(127u));
  EXPECT_EQ(128u, BitsUtil::NextPowerOfTwo(128u));
  EXPECT_EQ(256u, BitsUtil::NextPowerOfTwo(129u));
}


TEST(TestBits_BitsUtil, NextPowerOfTwo_signed)
{
  EXPECT_EQ(-64, BitsUtil::NextPowerOfTwo(-33));
  EXPECT_EQ(-32, BitsUtil::NextPowerOfTwo(-32));
  EXPECT_EQ(-32, BitsUtil::NextPowerOfTwo(-31));
  EXPECT_EQ(-1, BitsUtil::NextPowerOfTwo(-1));

  EXPECT_EQ(1, BitsUtil::NextPowerOfTwo(0));
  EXPECT_EQ(1, BitsUtil::NextPowerOfTwo(1));
  EXPECT_EQ(2, BitsUtil::NextPowerOfTwo(2));
  EXPECT_EQ(4, BitsUtil::NextPowerOfTwo(3));
  EXPECT_EQ(4, BitsUtil::NextPowerOfTwo(4));
  EXPECT_EQ(8, BitsUtil::NextPowerOfTwo(5));
  EXPECT_EQ(8, BitsUtil::NextPowerOfTwo(6));
  EXPECT_EQ(8, BitsUtil::NextPowerOfTwo(7));
  EXPECT_EQ(8, BitsUtil::NextPowerOfTwo(8));
  EXPECT_EQ(16, BitsUtil::NextPowerOfTwo(9));

  EXPECT_EQ(32, BitsUtil::NextPowerOfTwo(31));
  EXPECT_EQ(32, BitsUtil::NextPowerOfTwo(32));
  EXPECT_EQ(64, BitsUtil::NextPowerOfTwo(33));

  EXPECT_EQ(128, BitsUtil::NextPowerOfTwo(127));
  EXPECT_EQ(128, BitsUtil::NextPowerOfTwo(128));
  EXPECT_EQ(256, BitsUtil::NextPowerOfTwo(129));
}

TEST(TestBits_BitsUtil, IsPowerOfTwo)
{
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x01));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x02));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x04));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x08));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x10));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x20));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x40));
  EXPECT_TRUE(BitsUtil::IsPowerOfTwo(0x80));

  EXPECT_FALSE(BitsUtil::IsPowerOfTwo(0x00));
  EXPECT_FALSE(BitsUtil::IsPowerOfTwo(0x02 + 1));
  EXPECT_FALSE(BitsUtil::IsPowerOfTwo(0x04 - 1));
  EXPECT_FALSE(BitsUtil::IsPowerOfTwo(0x04 + 1));
  EXPECT_FALSE(BitsUtil::IsPowerOfTwo(0x08 - 1));
  EXPECT_FALSE(BitsUtil::IsPowerOfTwo(0x08 + 1));
}
