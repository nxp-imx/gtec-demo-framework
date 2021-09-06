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

#include <FslGraphics3D/Build/LineBuilder.hpp>
#include <FslBase/Exceptions.hpp>
#include <FslBase/Math/BoundingBox.hpp>
#include <FslBase/Log/Math/LogBoundingBox.hpp>
#include <FslBase/Log/Math/LogRay.hpp>
#include <FslBase/Log/Math/LogVector3.hpp>
#include <FslBase/UnitTest/Helper/Common.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;
using namespace Fsl::Graphics3D;

namespace
{
  using Test_LineBuilder = TestFixtureFslBase;
}


TEST(Test_LineBuilder, Construct_Default)
{
  LineBuilder lineBuilder;

  EXPECT_TRUE(lineBuilder.IsEmpty());
  EXPECT_EQ(0u, lineBuilder.LineCount());
}


TEST(Test_LineBuilder, Construct)
{
  LineBuilder lineBuilder(4);

  EXPECT_TRUE(lineBuilder.IsEmpty());
  EXPECT_EQ(0u, lineBuilder.LineCount());
}


TEST(Test_LineBuilder, Add_Line_ToFrom_Vector3_Color)
{
  LineBuilder lineBuilder;
  lineBuilder.Add(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 20.0f, 30.0f), Color(4u, 5u, 6u, 7u));

  EXPECT_FALSE(lineBuilder.IsEmpty());
  EXPECT_EQ(1u, lineBuilder.LineCount());
}


// TEST(Test_LineBuilder, Add_Line_ToFrom_Vector3_Color_Vector4)
//{
//  LineBuilder lineBuilder;
//  lineBuilder.Add(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 20.0f, 30.0f), Vector4(4.0f, 5.0f, 6.0f, 7.0f));
//
//  EXPECT_FALSE(lineBuilder.IsEmpty());
//  EXPECT_EQ(1u, lineBuilder.LineCount());
//}


TEST(Test_LineBuilder, Add_Line_BoundingBox_Color_Color)
{
  LineBuilder lineBuilder;
  lineBuilder.Add(BoundingBox(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 20.0f, 30.0f)), Color(4u, 5u, 6u, 7u));

  EXPECT_FALSE(lineBuilder.IsEmpty());
  EXPECT_EQ(12u, lineBuilder.LineCount());
}


// TEST(Test_LineBuilder, Add_Line_BoundingBox_Color_Vector4)
//{
//  LineBuilder lineBuilder;
//  lineBuilder.Add(BoundingBox(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 20.0f, 30.0f)), Vector4(4.0f, 5.0f, 6.0f, 7.0f));
//
//  EXPECT_FALSE(lineBuilder.IsEmpty());
//  EXPECT_EQ(12u, lineBuilder.LineCount());
//}


TEST(Test_LineBuilder, Clear)
{
  LineBuilder lineBuilder(1);

  lineBuilder.Add(Vector3(1.0f, 2.0f, 3.0f), Vector3(10.0f, 20.0f, 30.0f), Color(4u, 5u, 6u, 7u));

  EXPECT_FALSE(lineBuilder.IsEmpty());

  lineBuilder.Clear();

  EXPECT_TRUE(lineBuilder.IsEmpty());
  EXPECT_EQ(0u, lineBuilder.LineCount());
}
