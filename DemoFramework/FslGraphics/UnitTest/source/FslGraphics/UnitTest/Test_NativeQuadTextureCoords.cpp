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

#include <FslGraphics/NativeQuadTextureCoords.hpp>
#include <FslGraphics/Log/LogNativeQuadTextureCoords.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_NativeQuadTextureCoords = TestFixtureFslGraphics;
}


TEST(Test_NativeQuadTextureCoords, Construct_Empty)
{
  NativeQuadTextureCoords area;

  EXPECT_EQ(0.0f, area.TopLeft.X);
  EXPECT_EQ(0.0f, area.TopLeft.Y);
  EXPECT_EQ(0.0f, area.TopRight.X);
  EXPECT_EQ(0.0f, area.TopRight.Y);
  EXPECT_EQ(0.0f, area.BottomLeft.X);
  EXPECT_EQ(0.0f, area.BottomLeft.Y);
  EXPECT_EQ(0.0f, area.BottomRight.X);
  EXPECT_EQ(0.0f, area.BottomRight.Y);
}


TEST(Test_NativeQuadTextureCoords, Construct_1)
{
  NativeQuadTextureCoords area(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_EQ(1.0f, area.TopLeft.X);
  EXPECT_EQ(2.0f, area.TopLeft.Y);
  EXPECT_EQ(3.0f, area.TopRight.X);
  EXPECT_EQ(4.0f, area.TopRight.Y);
  EXPECT_EQ(5.0f, area.BottomLeft.X);
  EXPECT_EQ(6.0f, area.BottomLeft.Y);
  EXPECT_EQ(7.0f, area.BottomRight.X);
  EXPECT_EQ(8.0f, area.BottomRight.Y);
}


TEST(Test_NativeQuadTextureCoords, Construct_2)
{
  NativeQuadTextureCoords area(Vector2(1.0f, 2.0f), Vector2(3.0f, 4.0f), Vector2(5.0f, 6.0f), Vector2(7.0f, 8.0f));

  EXPECT_EQ(1.0f, area.TopLeft.X);
  EXPECT_EQ(2.0f, area.TopLeft.Y);
  EXPECT_EQ(3.0f, area.TopRight.X);
  EXPECT_EQ(4.0f, area.TopRight.Y);
  EXPECT_EQ(5.0f, area.BottomLeft.X);
  EXPECT_EQ(6.0f, area.BottomLeft.Y);
  EXPECT_EQ(7.0f, area.BottomRight.X);
  EXPECT_EQ(8.0f, area.BottomRight.Y);
}

TEST(Test_NativeQuadTextureCoords, OpEqual)
{
  NativeQuadTextureCoords area0(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords area1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_EQ(area0, area1);
}

TEST(Test_NativeQuadTextureCoords, OpNotEqual)
{
  NativeQuadTextureCoords area0(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaA(9.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaB(1.0f, 9.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaC(1.0f, 2.0f, 9.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaD(1.0f, 2.0f, 3.0f, 9.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaE(1.0f, 2.0f, 3.0f, 4.0f, 9.0f, 6.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaF(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 9.0f, 7.0f, 8.0f);
  NativeQuadTextureCoords areaG(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 9.0f, 8.0f);
  NativeQuadTextureCoords areaH(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 9.0f);

  EXPECT_NE(area0, areaA);
  EXPECT_NE(area0, areaB);
  EXPECT_NE(area0, areaC);
  EXPECT_NE(area0, areaD);
  EXPECT_NE(area0, areaE);
  EXPECT_NE(area0, areaF);
  EXPECT_NE(area0, areaG);
  EXPECT_NE(area0, areaH);
}
