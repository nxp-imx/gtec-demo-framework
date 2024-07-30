/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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

#include <FslGraphics/Debug/Render/Basic/DebugBasicCompareOp.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#
using namespace Fsl;

namespace
{
  using TestDebugRenderBasic_DebugBasicCompareOp = TestFixtureFslGraphics;
}


TEST(TestDebugRenderBasic_DebugBasicCompareOp, ToString)
{
  EXPECT_EQ(std::string("BasicCompareOp::Never"), Debug::ToString(BasicCompareOp::Never));
  EXPECT_EQ(std::string("BasicCompareOp::AlwaysMatch"), Debug::ToString(BasicCompareOp::AlwaysMatch));
  EXPECT_EQ(std::string("BasicCompareOp::Equal"), Debug::ToString(BasicCompareOp::Equal));
  EXPECT_EQ(std::string("BasicCompareOp::Greater"), Debug::ToString(BasicCompareOp::Greater));
  EXPECT_EQ(std::string("BasicCompareOp::GreaterOrEqual"), Debug::ToString(BasicCompareOp::GreaterOrEqual));
  EXPECT_EQ(std::string("BasicCompareOp::Less"), Debug::ToString(BasicCompareOp::Less));
  EXPECT_EQ(std::string("BasicCompareOp::LessOrEqual"), Debug::ToString(BasicCompareOp::LessOrEqual));
  EXPECT_EQ(std::string("BasicCompareOp::NotEqual"), Debug::ToString(BasicCompareOp::NotEqual));
}


TEST(TestDebugRenderBasic_DebugBasicCompareOp, TryToString)
{
  EXPECT_EQ(std::string("BasicCompareOp::Never"), Debug::TryToString(BasicCompareOp::Never));
  EXPECT_EQ(std::string("BasicCompareOp::AlwaysMatch"), Debug::TryToString(BasicCompareOp::AlwaysMatch));
  EXPECT_EQ(std::string("BasicCompareOp::Equal"), Debug::TryToString(BasicCompareOp::Equal));
  EXPECT_EQ(std::string("BasicCompareOp::Greater"), Debug::TryToString(BasicCompareOp::Greater));
  EXPECT_EQ(std::string("BasicCompareOp::GreaterOrEqual"), Debug::TryToString(BasicCompareOp::GreaterOrEqual));
  EXPECT_EQ(std::string("BasicCompareOp::Less"), Debug::TryToString(BasicCompareOp::Less));
  EXPECT_EQ(std::string("BasicCompareOp::LessOrEqual"), Debug::TryToString(BasicCompareOp::LessOrEqual));
  EXPECT_EQ(std::string("BasicCompareOp::NotEqual"), Debug::TryToString(BasicCompareOp::NotEqual));
}
