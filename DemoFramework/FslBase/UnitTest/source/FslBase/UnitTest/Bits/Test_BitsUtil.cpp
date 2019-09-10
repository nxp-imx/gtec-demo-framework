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
  EXPECT_EQ(BitsUtil::Count(0), 0);

  EXPECT_EQ(BitsUtil::Count(0x00000001), 1);
  EXPECT_EQ(BitsUtil::Count(0x10000000), 1);

  EXPECT_EQ(BitsUtil::Count(0x10000001), 2);

  EXPECT_EQ(BitsUtil::Count(0xFFFFFFFF), 32);
}


TEST(TestBits_BitsUtil, IndexOf_unsigned)
{
  EXPECT_EQ(BitsUtil::IndexOf(0u), -1);

  EXPECT_EQ(BitsUtil::IndexOf(0x00000001u), 0);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000002u), 1);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000004u), 2);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000008u), 3);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000010u), 4);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000020u), 5);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000040u), 6);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000080u), 7);

  EXPECT_EQ(BitsUtil::IndexOf(0x10000000u), 28);

  EXPECT_EQ(BitsUtil::IndexOf(0x10000010u), 4);

  EXPECT_EQ(BitsUtil::IndexOf(0xFFFFFFFFu), 0);
}


TEST(TestBits_BitsUtil, IndexOf_signed)
{
  EXPECT_EQ(BitsUtil::IndexOf(0), -1);

  EXPECT_EQ(BitsUtil::IndexOf(0x00000001), 0);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000002), 1);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000004), 2);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000008), 3);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000010), 4);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000020), 5);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000040), 6);
  EXPECT_EQ(BitsUtil::IndexOf(0x00000080), 7);

  EXPECT_EQ(BitsUtil::IndexOf(0x10000000), 28);

  EXPECT_EQ(BitsUtil::IndexOf(0x10000010), 4);

  EXPECT_EQ(BitsUtil::IndexOf(0xFFFFFFFF), 0);
}


TEST(TestBits_BitsUtil, NextPowerOfTwo_unsigned)
{
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(0u), 1u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(1u), 1u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(2u), 2u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(3u), 4u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(4u), 4u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(5u), 8u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(6u), 8u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(7u), 8u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(8u), 8u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(9u), 16u);

  EXPECT_EQ(BitsUtil::NextPowerOfTwo(31u), 32u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(32u), 32u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(33u), 64u);

  EXPECT_EQ(BitsUtil::NextPowerOfTwo(127u), 128u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(128u), 128u);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(129u), 256u);
}


TEST(TestBits_BitsUtil, NextPowerOfTwo_signed)
{
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(-33), -64);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(-32), -32);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(-31), -32);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(-1), -1);

  EXPECT_EQ(BitsUtil::NextPowerOfTwo(0), 1);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(1), 1);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(2), 2);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(3), 4);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(4), 4);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(5), 8);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(6), 8);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(7), 8);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(8), 8);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(9), 16);

  EXPECT_EQ(BitsUtil::NextPowerOfTwo(31), 32);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(32), 32);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(33), 64);

  EXPECT_EQ(BitsUtil::NextPowerOfTwo(127), 128);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(128), 128);
  EXPECT_EQ(BitsUtil::NextPowerOfTwo(129), 256);
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
