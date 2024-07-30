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

#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <FslGraphics/ColorsU16.hpp>
#include <FslGraphics/Log/LogColorU16.hpp>
#include <FslGraphics/Log/LogPackedColor64.hpp>
#include <FslSimpleUI/Base/UIColors.hpp>

using namespace Fsl;

namespace
{
  using Test_UIColors = TestFixtureFslBase;
}


TEST(Test_UIColors, Compare)
{
  EXPECT_EQ(UI::UIColors::Transparent().AsPackedColor64(), ColorsU16::Transparent().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::TransparentWhite().AsPackedColor64(), ColorsU16::TransparentWhite().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Black().AsPackedColor64(), ColorsU16::Black().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Red().AsPackedColor64(), ColorsU16::Red().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Green().AsPackedColor64(), ColorsU16::Green().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Blue().AsPackedColor64(), ColorsU16::Blue().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Cyan().AsPackedColor64(), ColorsU16::Cyan().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Yellow().AsPackedColor64(), ColorsU16::Yellow().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::White().AsPackedColor64(), ColorsU16::White().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Orange().AsPackedColor64(), ColorsU16::Orange().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Pink().AsPackedColor64(), ColorsU16::Pink().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Purple().AsPackedColor64(), ColorsU16::Purple().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Marrom().AsPackedColor64(), ColorsU16::Marrom().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Brown().AsPackedColor64(), ColorsU16::Brown().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Olive().AsPackedColor64(), ColorsU16::Olive().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Silver().AsPackedColor64(), ColorsU16::Silver().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::DarkBlue().AsPackedColor64(), ColorsU16::DarkBlue().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::LightBlue().AsPackedColor64(), ColorsU16::LightBlue().AsPackedColor64());
  EXPECT_EQ(UI::UIColors::Lime().AsPackedColor64(), ColorsU16::Lime().AsPackedColor64());
}
