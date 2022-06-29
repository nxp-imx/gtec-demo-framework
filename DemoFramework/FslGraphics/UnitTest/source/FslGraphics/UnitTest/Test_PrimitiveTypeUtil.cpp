/****************************************************************************************************************************************************
 * Copyright 2018, 2022 NXP
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

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList_min_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::min(), PrimitiveType::LineList));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList_max_int32)
{
  EXPECT_EQ(0x7FFFFFFF / 2, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::max(), PrimitiveType::LineList));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList_min_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::min(), PrimitiveType::LineList));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList_max_uint32)
{
  EXPECT_EQ(0xFFFFFFFFu / 2u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::max(), PrimitiveType::LineList));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip_min_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::min(), PrimitiveType::LineStrip));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip_max_int32)
{
  EXPECT_EQ(0x7FFFFFFF - 1, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::max(), PrimitiveType::LineStrip));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip_min_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::min(), PrimitiveType::LineStrip));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip_max_uint32)
{
  EXPECT_EQ(0xFFFFFFFFu - 1u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::max(), PrimitiveType::LineStrip));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList_min_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::min(), PrimitiveType::TriangleList));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList_max_int32)
{
  EXPECT_EQ(0x7FFFFFFF / 3, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::max(), PrimitiveType::TriangleList));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList_min_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::min(), PrimitiveType::TriangleList));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList_max_uint32)
{
  EXPECT_EQ(0xFFFFFFFFu / 3u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::max(), PrimitiveType::TriangleList));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip_min_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::min(), PrimitiveType::TriangleStrip));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip_max_int32)
{
  EXPECT_EQ(0x7FFFFFFF - 2, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<int32_t>::max(), PrimitiveType::TriangleStrip));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip_min_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::min(), PrimitiveType::TriangleStrip));
}

TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip_max_uint32)
{
  EXPECT_EQ(0xFFFFFFFFu - 2u, PrimitiveTypeUtil::CalcPrimitiveCount(std::numeric_limits<uint32_t>::max(), PrimitiveType::TriangleStrip));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(0), PrimitiveType::LineList));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(1), PrimitiveType::LineList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(2), PrimitiveType::LineList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(3), PrimitiveType::LineList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(4), PrimitiveType::LineList));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(0), PrimitiveType::LineStrip));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(1), PrimitiveType::LineStrip));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(2), PrimitiveType::LineStrip));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(3), PrimitiveType::LineStrip));
  EXPECT_EQ(3, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(4), PrimitiveType::LineStrip));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(0), PrimitiveType::TriangleList));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(1), PrimitiveType::TriangleList));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(2), PrimitiveType::TriangleList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(3), PrimitiveType::TriangleList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(4), PrimitiveType::TriangleList));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(5), PrimitiveType::TriangleList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(6), PrimitiveType::TriangleList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(7), PrimitiveType::TriangleList));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(8), PrimitiveType::TriangleList));
  EXPECT_EQ(3, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(9), PrimitiveType::TriangleList));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip_int32)
{
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(0), PrimitiveType::TriangleStrip));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(1), PrimitiveType::TriangleStrip));
  EXPECT_EQ(0, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(2), PrimitiveType::TriangleStrip));
  EXPECT_EQ(1, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(3), PrimitiveType::TriangleStrip));
  EXPECT_EQ(2, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(4), PrimitiveType::TriangleStrip));
  EXPECT_EQ(3, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(5), PrimitiveType::TriangleStrip));
  EXPECT_EQ(4, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(6), PrimitiveType::TriangleStrip));
  EXPECT_EQ(5, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(7), PrimitiveType::TriangleStrip));
  EXPECT_EQ(6, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(8), PrimitiveType::TriangleStrip));
  EXPECT_EQ(7, PrimitiveTypeUtil::CalcPrimitiveCount(int32_t(9), PrimitiveType::TriangleStrip));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineList_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(0), PrimitiveType::LineList));
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(1), PrimitiveType::LineList));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(2), PrimitiveType::LineList));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(3), PrimitiveType::LineList));
  EXPECT_EQ(2u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(4), PrimitiveType::LineList));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_LineStrip_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(0), PrimitiveType::LineStrip));
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(1), PrimitiveType::LineStrip));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(2), PrimitiveType::LineStrip));
  EXPECT_EQ(2u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(3), PrimitiveType::LineStrip));
  EXPECT_EQ(3u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(4), PrimitiveType::LineStrip));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleList_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(0), PrimitiveType::TriangleList));
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(1), PrimitiveType::TriangleList));
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(2), PrimitiveType::TriangleList));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(3), PrimitiveType::TriangleList));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(4), PrimitiveType::TriangleList));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(5), PrimitiveType::TriangleList));
  EXPECT_EQ(2u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(6), PrimitiveType::TriangleList));
  EXPECT_EQ(2u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(7), PrimitiveType::TriangleList));
  EXPECT_EQ(2u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(8), PrimitiveType::TriangleList));
  EXPECT_EQ(3u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(9), PrimitiveType::TriangleList));
}


TEST(Test_PrimitiveTypeUtil, CalcPrimitiveCount_TriangleStrip_uint32)
{
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(0), PrimitiveType::TriangleStrip));
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(1), PrimitiveType::TriangleStrip));
  EXPECT_EQ(0u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(2), PrimitiveType::TriangleStrip));
  EXPECT_EQ(1u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(3), PrimitiveType::TriangleStrip));
  EXPECT_EQ(2u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(4), PrimitiveType::TriangleStrip));
  EXPECT_EQ(3u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(5), PrimitiveType::TriangleStrip));
  EXPECT_EQ(4u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(6), PrimitiveType::TriangleStrip));
  EXPECT_EQ(5u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(7), PrimitiveType::TriangleStrip));
  EXPECT_EQ(6u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(8), PrimitiveType::TriangleStrip));
  EXPECT_EQ(7u, PrimitiveTypeUtil::CalcPrimitiveCount(uint32_t(9), PrimitiveType::TriangleStrip));
}
