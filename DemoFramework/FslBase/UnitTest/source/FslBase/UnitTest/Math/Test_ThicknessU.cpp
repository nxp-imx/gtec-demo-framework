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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogThicknessU.hpp>
#include <FslBase/Math/ThicknessU.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_ThicknessU = TestFixtureFslBase;
}


TEST(TestMath_ThicknessU, Default)
{
  ThicknessU value;

  EXPECT_EQ(0u, value.Left);
  EXPECT_EQ(0u, value.Top);
  EXPECT_EQ(0u, value.Right);
  EXPECT_EQ(0u, value.Bottom);
  EXPECT_EQ(0u, value.SumX());
  EXPECT_EQ(0u, value.SumY());
}


TEST(TestMath_ThicknessU, Values)
{
  uint32_t left = 1;
  uint32_t top = 2;
  uint32_t right = 3;
  uint32_t bottom = 4;
  ThicknessU value(left, top, right, bottom);

  EXPECT_EQ(left, value.Left);
  EXPECT_EQ(top, value.Top);
  EXPECT_EQ(right, value.Right);
  EXPECT_EQ(bottom, value.Bottom);
  EXPECT_EQ(left + right, value.SumX());
  EXPECT_EQ(top + bottom, value.SumY());
}

TEST(TestMath_ThicknessU, OperatorEqual)
{
  uint32_t left = 1;
  uint32_t top = 2;
  uint32_t right = 3;
  uint32_t bottom = 4;
  ThicknessU value1(left, top, right, bottom);
  ThicknessU value2(left, top, right, bottom);

  EXPECT_EQ(value1, value2);
}


TEST(TestMath_ThicknessU, OperatorNotEqual)
{
  uint32_t left = 1;
  uint32_t top = 2;
  uint32_t right = 3;
  uint32_t bottom = 4;
  ThicknessU value1(left, top, right, bottom);
  ThicknessU value2(left, top, right, 5);

  EXPECT_NE(value1, value2);
}
