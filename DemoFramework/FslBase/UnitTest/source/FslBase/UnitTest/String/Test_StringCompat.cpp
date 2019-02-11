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

#include <FslBase/String/StringCompat.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslBase/Exceptions.hpp>
#include <array>
#include <cstring>
#include <limits>
#include <utility>

using namespace Fsl;

namespace
{
  using TestString_StringCompat = TestFixtureFslBase;
}


// NOTE: this class can probably be removed as visual studio supports snprintf

TEST(TestString_StringCompat, BasicString)
{
  std::array<char, 10> dst{1};

  auto res = StringCompat::sprintf_s(dst.data(), dst.size(), "%s", "hello");
  EXPECT_EQ(5, res);
  EXPECT_EQ('h', dst[0]);
  EXPECT_EQ('e', dst[1]);
  EXPECT_EQ('l', dst[2]);
  EXPECT_EQ('l', dst[3]);
  EXPECT_EQ('o', dst[4]);
  EXPECT_EQ(0, dst[5]);
}

// The windows version of this function is not really playing nice as it asserts instead of returning the promised values when

// TEST(TestString_StringCompat, DstNullPtr)
//{
//  std::array<char, 5> dst{1};
//
//  auto res = StringCompat::sprintf_s(nullptr, dst.size(), "%s", "hello");
//  EXPECT_GE(0, res);
//}
//
// TEST(TestString_StringCompat, BufferTooSmall)
//{
//  std::array<char, 5> dst{1};
//
//  auto res = StringCompat::sprintf_s(dst.data(), dst.size(), "%s", "hello");
//  EXPECT_GE(0, res);
//}
//
