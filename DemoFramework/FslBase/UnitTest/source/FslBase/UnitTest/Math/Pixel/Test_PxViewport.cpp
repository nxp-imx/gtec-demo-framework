/****************************************************************************************************************************************************
 * Copyright 2023 NXP
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

#include <FslBase/Log/Math/Pixel/LogPxViewport.hpp>
#include <FslBase/Math/Pixel/PxViewport.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathPixel_PxViewport = TestFixtureFslBase;
}


TEST(TestMathPixel_PxViewport, Construct_Default)
{
  PxViewport value;

  EXPECT_EQ(value.X(), 0);
  EXPECT_EQ(value.Y(), 0);
  EXPECT_EQ(value.Width(), 0);
  EXPECT_EQ(value.Height(), 0);
  EXPECT_EQ(value.MinDepth(), 0.0f);
  EXPECT_EQ(value.MaxDepth(), 0.0f);
  EXPECT_EQ(value.GetAspectRatio(), 0.0f);
}


TEST(TestMathPixel_PxViewport, Construct)
{
  const int32_t x = 1;
  const int32_t y = 2;
  const int32_t width = 20;
  const int32_t height = 21;
  PxViewport value(x, y, width, height);

  EXPECT_EQ(value.X(), x);
  EXPECT_EQ(value.Y(), y);
  EXPECT_EQ(value.Width(), width);
  EXPECT_EQ(value.Height(), height);
  EXPECT_EQ(value.MinDepth(), 0.0f);
  EXPECT_EQ(value.MaxDepth(), 1.0f);
  EXPECT_EQ(value.GetAspectRatio(), static_cast<float>(width) / static_cast<float>(height));
}


TEST(TestMathPixel_PxViewport, Construct_Rectangle)
{
  const auto rect = PxRectangle::Create(1, 2, 20, 21);
  PxViewport value(rect);

  EXPECT_EQ(value.X(), rect.X().Value);
  EXPECT_EQ(value.Y(), rect.Y().Value);
  EXPECT_EQ(value.Width(), rect.RawWidth());
  EXPECT_EQ(value.Height(), rect.RawHeight());
  EXPECT_EQ(value.MinDepth(), 0.0f);
  EXPECT_EQ(value.MaxDepth(), 1.0f);
  EXPECT_FLOAT_EQ(value.GetAspectRatio(), static_cast<float>(rect.Width().RawValue()) / static_cast<float>(rect.Height().RawValue()));
}

TEST(TestMathPixel_PxViewport, Construct_Rectangle2)
{
  constexpr float minDepth = 32.0f;
  constexpr float maxDepth = 42.0f;
  auto rect = PxRectangle::Create(1, 2, 20, 21);
  PxViewport value(rect, minDepth, maxDepth);

  EXPECT_EQ(value.X(), rect.X().Value);
  EXPECT_EQ(value.Y(), rect.Y().Value);
  EXPECT_EQ(value.Width(), rect.RawWidth());
  EXPECT_EQ(value.Height(), rect.RawHeight());
  EXPECT_EQ(value.MinDepth(), minDepth);
  EXPECT_EQ(value.MaxDepth(), maxDepth);
  EXPECT_EQ(value.GetAspectRatio(), static_cast<float>(rect.Width().RawValue()) / static_cast<float>(rect.Height().RawValue()));
}
