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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogNineSlice.hpp>
#include <FslBase/Math/NineSlice.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_NineSlice = TestFixtureFslBase;
}


TEST(TestMath_NineSlice, Construct_Default)
{
  NineSlice value;

  EXPECT_EQ(0, value.SliceFromTopLeftX());
  EXPECT_EQ(0, value.SliceFromTopLeftY());
  EXPECT_EQ(0, value.SliceFromBottomRightX());
  EXPECT_EQ(0, value.SliceFromBottomRightY());
  EXPECT_EQ(0, value.SumX());
  EXPECT_EQ(0, value.SumY());
  EXPECT_TRUE(value.IsEmpty());
}


TEST(TestMath_NineSlice, Construct)
{
  NineSlice value(1, 2, 3, 4);

  EXPECT_EQ(1, value.SliceFromTopLeftX());
  EXPECT_EQ(2, value.SliceFromTopLeftY());
  EXPECT_EQ(3, value.SliceFromBottomRightX());
  EXPECT_EQ(4, value.SliceFromBottomRightY());
  EXPECT_EQ(1 + 3, value.SumX());
  EXPECT_EQ(2 + 4, value.SumY());
  EXPECT_FALSE(value.IsEmpty());
}


TEST(TestMath_NineSlice, Construct_Invalid)
{
  EXPECT_THROW(NineSlice(-1, 2, 3, 4), std::invalid_argument);
  EXPECT_THROW(NineSlice(1, -2, 3, 4), std::invalid_argument);
  EXPECT_THROW(NineSlice(1, 2, -3, 4), std::invalid_argument);
  EXPECT_THROW(NineSlice(1, 2, 3, -4), std::invalid_argument);
}


TEST(TestMath_NineSlice, Equal)
{
  NineSlice value1(1, 2, 3, 4);
  NineSlice value2(1, 2, 3, 4);

  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value1, value1);
  EXPECT_EQ(value2, value2);
}


TEST(TestMath_NineSlice, NotEqual)
{
  NineSlice value1(1, 2, 3, 4);
  NineSlice value2(5, 2, 3, 4);
  NineSlice value3(1, 5, 3, 4);
  NineSlice value4(1, 2, 5, 4);
  NineSlice value5(1, 2, 3, 5);

  EXPECT_NE(value1, value2);
  EXPECT_NE(value1, value3);
  EXPECT_NE(value1, value4);
  EXPECT_NE(value1, value5);
}
