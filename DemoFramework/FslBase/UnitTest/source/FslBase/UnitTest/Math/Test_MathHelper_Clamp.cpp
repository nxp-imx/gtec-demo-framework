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

#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <limits>

using namespace Fsl;

namespace
{
  using TestMath_MathHelper_Clamp = TestFixtureFslBase;
}


TEST(TestMath_MathHelper_Clamp, ClampLessThan)
{
  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_EQ(10, MathHelper::Clamp(9, min, max));
  EXPECT_EQ(10, MathHelper::Clamp(5, min, max));
  EXPECT_EQ(10, MathHelper::Clamp(-1, min, max));
  EXPECT_EQ(10, MathHelper::Clamp(std::numeric_limits<int32_t>::min(), min, max));
}


TEST(TestMath_MathHelper_Clamp, ClampInRange)
{
  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_EQ(10, MathHelper::Clamp(10, min, max));
  EXPECT_EQ(11, MathHelper::Clamp(11, min, max));
  EXPECT_EQ(12, MathHelper::Clamp(12, min, max));
  EXPECT_EQ(13, MathHelper::Clamp(13, min, max));
  EXPECT_EQ(14, MathHelper::Clamp(14, min, max));
  EXPECT_EQ(15, MathHelper::Clamp(15, min, max));
}


TEST(TestMath_MathHelper_Clamp, ClampGreaterThan)
{
  const int32_t min = 10;
  const int32_t max = 15;
  EXPECT_EQ(15, MathHelper::Clamp(16, min, max));
  EXPECT_EQ(15, MathHelper::Clamp(17, min, max));
  EXPECT_EQ(15, MathHelper::Clamp(18, min, max));
  EXPECT_EQ(15, MathHelper::Clamp(1000, min, max));
  EXPECT_EQ(15, MathHelper::Clamp(std::numeric_limits<int32_t>::max(), min, max));
}
