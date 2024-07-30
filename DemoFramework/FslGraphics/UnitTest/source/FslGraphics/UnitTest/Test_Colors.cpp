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

#include <FslGraphics/Colors.hpp>
#include <FslGraphics/ColorsU16.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/Log/LogColorU16.hpp>
#include <FslGraphics/Log/LogPackedColor32.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>

using namespace Fsl;

namespace
{
  using Test_Colors = TestFixtureFslGraphics;
}


TEST(Test_Colors, Compare)
{
  EXPECT_EQ(Colors::Transparent().AsPackedColor32(), ColorsU16::Transparent().AsPackedColor32());
  EXPECT_EQ(Colors::TransparentWhite().AsPackedColor32(), ColorsU16::TransparentWhite().AsPackedColor32());
  EXPECT_EQ(Colors::Black().AsPackedColor32(), ColorsU16::Black().AsPackedColor32());
  EXPECT_EQ(Colors::Red().AsPackedColor32(), ColorsU16::Red().AsPackedColor32());
  EXPECT_EQ(Colors::Green().AsPackedColor32(), ColorsU16::Green().AsPackedColor32());
  EXPECT_EQ(Colors::Blue().AsPackedColor32(), ColorsU16::Blue().AsPackedColor32());
  EXPECT_EQ(Colors::Cyan().AsPackedColor32(), ColorsU16::Cyan().AsPackedColor32());
  EXPECT_EQ(Colors::Yellow().AsPackedColor32(), ColorsU16::Yellow().AsPackedColor32());
  EXPECT_EQ(Colors::White().AsPackedColor32(), ColorsU16::White().AsPackedColor32());
  EXPECT_EQ(Colors::Orange().AsPackedColor32(), ColorsU16::Orange().AsPackedColor32());
  EXPECT_EQ(Colors::Pink().AsPackedColor32(), ColorsU16::Pink().AsPackedColor32());
  EXPECT_EQ(Colors::Purple().AsPackedColor32(), ColorsU16::Purple().AsPackedColor32());
  EXPECT_EQ(Colors::Marrom().AsPackedColor32(), ColorsU16::Marrom().AsPackedColor32());
  EXPECT_EQ(Colors::Brown().AsPackedColor32(), ColorsU16::Brown().AsPackedColor32());
  EXPECT_EQ(Colors::Olive().AsPackedColor32(), ColorsU16::Olive().AsPackedColor32());
  EXPECT_EQ(Colors::Silver().AsPackedColor32(), ColorsU16::Silver().AsPackedColor32());
  EXPECT_EQ(Colors::DarkBlue().AsPackedColor32(), ColorsU16::DarkBlue().AsPackedColor32());
  EXPECT_EQ(Colors::LightBlue().AsPackedColor32(), ColorsU16::LightBlue().AsPackedColor32());
  EXPECT_EQ(Colors::Lime().AsPackedColor32(), ColorsU16::Lime().AsPackedColor32());
}
