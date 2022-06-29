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

#include <FslGraphics/Log/LogTextureRectangle.hpp>
#include <FslGraphics/TextureRectangle.hpp>
#include <FslGraphics/UnitTest/Helper/Common.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_TextureRectangle = TestFixtureFslGraphics;
}


TEST(Test_TextureRectangle, Construct_Empty)
{
  TextureRectangle rectangle;

  auto rect = rectangle.GetRectangle();
  auto size = rectangle.GetTextureSize();

  EXPECT_EQ(PxRectangle(0, 0, 0, 0), rect);
  EXPECT_EQ(PxSize2D(0, 0), size);
}


TEST(Test_TextureRectangle, Equal)
{
  TextureRectangle rectangle1(PxRectangle(1, 2, 3, 4), PxSize2D(10, 20));
  TextureRectangle rectangle2(PxRectangle(1, 2, 3, 4), PxSize2D(10, 20));

  EXPECT_EQ(rectangle1, rectangle2);
}


TEST(Test_TextureRectangle, NotEqual)
{
  TextureRectangle rectangle1(PxRectangle(1, 2, 3, 4), PxSize2D(10, 20));
  TextureRectangle rectangle2(PxRectangle(1, 2, 3, 4), PxSize2D(10, 21));
  TextureRectangle rectangle3(PxRectangle(1, 2, 3, 4), PxSize2D(11, 20));
  TextureRectangle rectangle4(PxRectangle(1, 2, 3, 5), PxSize2D(10, 20));
  TextureRectangle rectangle5(PxRectangle(1, 2, 4, 4), PxSize2D(10, 20));
  TextureRectangle rectangle6(PxRectangle(1, 3, 3, 4), PxSize2D(10, 20));
  TextureRectangle rectangle7(PxRectangle(2, 1, 3, 4), PxSize2D(10, 20));

  EXPECT_NE(rectangle1, rectangle2);
  EXPECT_NE(rectangle1, rectangle3);
  EXPECT_NE(rectangle1, rectangle4);
  EXPECT_NE(rectangle1, rectangle5);
  EXPECT_NE(rectangle1, rectangle6);
  EXPECT_NE(rectangle1, rectangle7);
}
