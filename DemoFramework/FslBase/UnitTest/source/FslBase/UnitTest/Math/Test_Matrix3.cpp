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

#include <FslBase/Exceptions.hpp>
#include <FslBase/Log/Math/LogMatrix3.hpp>
#include <FslBase/Math/Matrix3.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Matrix3 = TestFixtureFslBase;
}


TEST(TestMath_Matrix3, Construct_Default)
{
  Matrix3 value;
  const float* const pDirect = value.DirectAccess();

  ASSERT_NE(pDirect, nullptr);
  for (uint32_t i = 0; i < (3 * 3); ++i)
  {
    EXPECT_EQ(0.0f, pDirect[i]) << " at index: " << i;
  }
}


TEST(TestMath_Matrix3, Construct)
{
  Matrix3 value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
  const float* const pDirect = value.DirectAccess();

  ASSERT_NE(pDirect, nullptr);
  EXPECT_EQ(1.0f, pDirect[0]);
  EXPECT_EQ(2.0f, pDirect[1]);
  EXPECT_EQ(3.0f, pDirect[2]);
  EXPECT_EQ(4.0f, pDirect[3]);
  EXPECT_EQ(5.0f, pDirect[4]);
  EXPECT_EQ(6.0f, pDirect[5]);
  EXPECT_EQ(7.0f, pDirect[6]);
  EXPECT_EQ(8.0f, pDirect[7]);
  EXPECT_EQ(9.0f, pDirect[8]);
}


TEST(TestMath_Matrix3, Clear)
{
  Matrix3 value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
  value.Clear();

  const float* const pDirect = value.DirectAccess();
  ASSERT_NE(pDirect, nullptr);
  for (uint32_t i = 0; i < (3 * 3); ++i)
  {
    EXPECT_EQ(0.0f, pDirect[i]) << " at index: " << i;
  }
}


TEST(TestMath_Matrix3, Determinant)
{
  auto value = Matrix3::GetIdentity();
  EXPECT_FLOAT_EQ(1.0f, value.Determinant());
}


TEST(TestMath_Matrix3, Invert_Identity)
{
  const auto identity = Matrix3::GetIdentity();
  const auto res = Matrix3::Invert(identity);
  EXPECT_EQ(identity, res);
}


TEST(TestMath_Matrix3, Transpose_Identity)
{
  const auto identity = Matrix3::GetIdentity();
  const auto res = Matrix3::Transpose(identity);
  EXPECT_EQ(identity, res);
}


TEST(TestMath_Matrix3, SetIdentity)
{
  Matrix3 value(-1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
  value.SetIdentity();
  EXPECT_EQ(Matrix3::GetIdentity(), value);
}


TEST(TestMath_Matrix3, Equal)
{
  const auto value = Matrix3::GetIdentity();
  const Matrix3 value1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
  const Matrix3 value2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);

  EXPECT_EQ(value, value);
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value1);
  EXPECT_EQ(value1, value1);
  EXPECT_EQ(value2, value2);
}


TEST(TestMath_Matrix3, NotEqual)
{
  const Matrix3 value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);

  EXPECT_NE(Matrix3(-1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, -2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, -3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, 3.0f, -4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, 3.0f, 4.0f, -5.0f, 6.0f, 7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, -6.0f, 7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, -7.0f, 8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, -8.0f, 9.0f), value);
  EXPECT_NE(Matrix3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, -9.0f), value);
}
