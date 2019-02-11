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

#include <FslGraphics/PrimitiveTypeUtil.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_PrimitiveTypeUtil = TestFixtureFslGraphics;
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(0, PrimitiveType::LineList));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(1, PrimitiveType::LineList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(2, PrimitiveType::LineList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(3, PrimitiveType::LineList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(4, PrimitiveType::LineList));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(0, PrimitiveType::LineStrip));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(1, PrimitiveType::LineStrip));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(2, PrimitiveType::LineStrip));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(3, PrimitiveType::LineStrip));
  EXPECT_EQ(3, PrimitiveTypeUtil::CalcPrimitiveCount(4, PrimitiveType::LineStrip));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(0, PrimitiveType::TriangleList));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(1, PrimitiveType::TriangleList));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(2, PrimitiveType::TriangleList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(3, PrimitiveType::TriangleList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(4, PrimitiveType::TriangleList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(5, PrimitiveType::TriangleList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(6, PrimitiveType::TriangleList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(7, PrimitiveType::TriangleList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(8, PrimitiveType::TriangleList));
  EXPECT_EQ(3, PrimitiveTypeUtil::CalcPrimitiveCount(9, PrimitiveType::TriangleList));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(0, PrimitiveType::TriangleStrip));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(1, PrimitiveType::TriangleStrip));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(2, PrimitiveType::TriangleStrip));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(3, PrimitiveType::TriangleStrip));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(4, PrimitiveType::TriangleStrip));
  EXPECT_EQ(3, PrimitiveTypeUtil::CalcPrimitiveCount(5, PrimitiveType::TriangleStrip));
  EXPECT_EQ(4, PrimitiveTypeUtil::CalcPrimitiveCount(6, PrimitiveType::TriangleStrip));
  EXPECT_EQ(5, PrimitiveTypeUtil::CalcPrimitiveCount(7, PrimitiveType::TriangleStrip));
  EXPECT_EQ(6, PrimitiveTypeUtil::CalcPrimitiveCount(8, PrimitiveType::TriangleStrip));
  EXPECT_EQ(7, PrimitiveTypeUtil::CalcPrimitiveCount(9, PrimitiveType::TriangleStrip));
}
