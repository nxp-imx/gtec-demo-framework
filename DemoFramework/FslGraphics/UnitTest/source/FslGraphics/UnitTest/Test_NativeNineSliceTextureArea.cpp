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

#include <FslGraphics/Log/LogNativeNineSliceTextureArea.hpp>
#include <FslGraphics/NativeNineSliceTextureArea.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_NativeNineSliceTextureArea = TestFixtureFslGraphics;
}


TEST(Test_NativeNineSliceTextureArea, Construct_Empty)
{
  NativeNineSliceTextureArea area;

  EXPECT_EQ(0.0f, area.X0);
  EXPECT_EQ(0.0f, area.Y0);
  EXPECT_EQ(0.0f, area.X1);
  EXPECT_EQ(0.0f, area.Y1);
  EXPECT_EQ(0.0f, area.X2);
  EXPECT_EQ(0.0f, area.Y2);
  EXPECT_EQ(0.0f, area.X3);
  EXPECT_EQ(0.0f, area.Y3);
}


TEST(Test_NativeNineSliceTextureArea, Construct_1)
{
  NativeNineSliceTextureArea area(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_EQ(1.0f, area.X0);
  EXPECT_EQ(2.0f, area.Y0);
  EXPECT_EQ(3.0f, area.X1);
  EXPECT_EQ(4.0f, area.Y1);
  EXPECT_EQ(5.0f, area.X2);
  EXPECT_EQ(6.0f, area.Y2);
  EXPECT_EQ(7.0f, area.X3);
  EXPECT_EQ(8.0f, area.Y3);
}


TEST(Test_NativeNineSliceTextureArea, OpEqual)
{
  NativeNineSliceTextureArea area0(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaA(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);

  EXPECT_EQ(area0, areaA);
}


TEST(Test_NativeNineSliceTextureArea, OpNotEqual)
{
  NativeNineSliceTextureArea area0(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaA(9.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaB(1.0f, 9.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaC(1.0f, 2.0f, 9.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaD(1.0f, 2.0f, 3.0f, 9.0f, 5.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaE(1.0f, 2.0f, 3.0f, 4.0f, 9.0f, 6.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaF(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 9.0f, 7.0f, 8.0f);
  NativeNineSliceTextureArea areaG(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 9.0f, 8.0f);
  NativeNineSliceTextureArea areaH(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 9.0f);

  EXPECT_NE(area0, areaA);
  EXPECT_NE(area0, areaB);
  EXPECT_NE(area0, areaC);
  EXPECT_NE(area0, areaD);
  EXPECT_NE(area0, areaE);
  EXPECT_NE(area0, areaF);
  EXPECT_NE(area0, areaG);
  EXPECT_NE(area0, areaH);
}
