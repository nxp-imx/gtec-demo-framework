/****************************************************************************************************************************************************
 * Copyright 2018, 2023 NXP
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
#include <FslBase/Log/Math/LogViewport.hpp>
#include <FslBase/Math/Viewport.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Viewport = TestFixtureFslBase;
}


TEST(TestMath_Viewport, Construct_Default)
{
  Viewport value;

  EXPECT_EQ(value.X(), 0);
  EXPECT_EQ(value.Y(), 0);
  EXPECT_EQ(value.Width(), 0);
  EXPECT_EQ(value.Height(), 0);
  EXPECT_EQ(value.MinDepth(), 0.0f);
  EXPECT_EQ(value.MaxDepth(), 0.0f);
  EXPECT_EQ(value.GetAspectRatio(), 0.0f);
}


TEST(TestMath_Viewport, Construct)
{
  const int32_t x = 1;
  const int32_t y = 2;
  const int32_t width = 20;
  const int32_t height = 21;
  Viewport value(x, y, width, height);

  EXPECT_EQ(value.X(), x);
  EXPECT_EQ(value.Y(), y);
  EXPECT_EQ(value.Width(), width);
  EXPECT_EQ(value.Height(), height);
  EXPECT_EQ(value.MinDepth(), 0.0f);
  EXPECT_EQ(value.MaxDepth(), 1.0f);
  EXPECT_EQ(value.GetAspectRatio(), static_cast<float>(width) / static_cast<float>(height));
}


TEST(TestMath_Viewport, Construct_Rectangle)
{
  const Rectangle rect(1, 2, 20, 21);
  Viewport value(rect);

  EXPECT_EQ(value.X(), rect.X());
  EXPECT_EQ(value.Y(), rect.Y());
  EXPECT_EQ(value.Width(), rect.Width());
  EXPECT_EQ(value.Height(), rect.Height());
  EXPECT_EQ(value.MinDepth(), 0.0f);
  EXPECT_EQ(value.MaxDepth(), 1.0f);
  EXPECT_EQ(value.GetAspectRatio(), static_cast<float>(rect.Width()) / static_cast<float>(rect.Height()));
}

TEST(TestMath_Viewport, Construct_Rectangle2)
{
  constexpr float minDepth = 32.0f;
  constexpr float maxDepth = 42.0f;
  Rectangle rect(1, 2, 20, 21);
  Viewport value(rect, minDepth, maxDepth);

  EXPECT_EQ(value.X(), rect.X());
  EXPECT_EQ(value.Y(), rect.Y());
  EXPECT_EQ(value.Width(), rect.Width());
  EXPECT_EQ(value.Height(), rect.Height());
  EXPECT_EQ(value.MinDepth(), minDepth);
  EXPECT_EQ(value.MaxDepth(), maxDepth);
  EXPECT_EQ(value.GetAspectRatio(), static_cast<float>(rect.Width()) / static_cast<float>(rect.Height()));
}
