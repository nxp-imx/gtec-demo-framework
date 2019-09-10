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
#include <FslBase/Math/Matrix.hpp>
#include <FslBase/Math/MathHelper.hpp>
#include <FslBase/Log/Math/LogMatrix.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>
#include <array>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using TestMath_Matrix = TestFixtureFslBase;


  void CheckLerp(const Matrix& value1, const Matrix& value2, const float t, const Matrix& res)
  {
    const float* const pValue1 = value1.DirectAccess();
    const float* const pValue2 = value2.DirectAccess();
    const float* const pRes = res.DirectAccess();
    ASSERT_NE(pValue1, nullptr);
    ASSERT_NE(pValue2, nullptr);
    ASSERT_NE(pRes, nullptr);
    for (uint32_t i = 0; i < (4 * 4); ++i)
    {
      const float expectedValue = pValue1[i] + ((pValue2[i] - pValue1[i]) * t);
      EXPECT_FLOAT_EQ(expectedValue, pRes[i]) << " at index: " << i;
    }
  }

  constexpr unsigned int _M11 = (4 * 0) + 0;
  constexpr unsigned int _M12 = (4 * 0) + 1;
  constexpr unsigned int _M13 = (4 * 0) + 2;
  constexpr unsigned int _M14 = (4 * 0) + 3;

  constexpr unsigned int _M21 = (4 * 1) + 0;
  constexpr unsigned int _M22 = (4 * 1) + 1;
  constexpr unsigned int _M23 = (4 * 1) + 2;
  constexpr unsigned int _M24 = (4 * 1) + 3;

  constexpr unsigned int _M31 = (4 * 2) + 0;
  constexpr unsigned int _M32 = (4 * 2) + 1;
  constexpr unsigned int _M33 = (4 * 2) + 2;
  constexpr unsigned int _M34 = (4 * 2) + 3;

  constexpr unsigned int _M41 = (4 * 3) + 0;
  constexpr unsigned int _M42 = (4 * 3) + 1;
  constexpr unsigned int _M43 = (4 * 3) + 2;
  constexpr unsigned int _M44 = (4 * 3) + 3;

  // Compile time evaluated matrix multiply :)
  constexpr Matrix ConstMultiply(const float* const pLhs, const float* const pRhs)
  {
    return Matrix((((pLhs[_M11] * pRhs[_M11]) + (pLhs[_M12] * pRhs[_M21])) + (pLhs[_M13] * pRhs[_M31])) + (pLhs[_M14] * pRhs[_M41]),
                  (((pLhs[_M11] * pRhs[_M12]) + (pLhs[_M12] * pRhs[_M22])) + (pLhs[_M13] * pRhs[_M32])) + (pLhs[_M14] * pRhs[_M42]),
                  (((pLhs[_M11] * pRhs[_M13]) + (pLhs[_M12] * pRhs[_M23])) + (pLhs[_M13] * pRhs[_M33])) + (pLhs[_M14] * pRhs[_M43]),
                  (((pLhs[_M11] * pRhs[_M14]) + (pLhs[_M12] * pRhs[_M24])) + (pLhs[_M13] * pRhs[_M34])) + (pLhs[_M14] * pRhs[_M44]),
                  (((pLhs[_M21] * pRhs[_M11]) + (pLhs[_M22] * pRhs[_M21])) + (pLhs[_M23] * pRhs[_M31])) + (pLhs[_M24] * pRhs[_M41]),
                  (((pLhs[_M21] * pRhs[_M12]) + (pLhs[_M22] * pRhs[_M22])) + (pLhs[_M23] * pRhs[_M32])) + (pLhs[_M24] * pRhs[_M42]),
                  (((pLhs[_M21] * pRhs[_M13]) + (pLhs[_M22] * pRhs[_M23])) + (pLhs[_M23] * pRhs[_M33])) + (pLhs[_M24] * pRhs[_M43]),
                  (((pLhs[_M21] * pRhs[_M14]) + (pLhs[_M22] * pRhs[_M24])) + (pLhs[_M23] * pRhs[_M34])) + (pLhs[_M24] * pRhs[_M44]),
                  (((pLhs[_M31] * pRhs[_M11]) + (pLhs[_M32] * pRhs[_M21])) + (pLhs[_M33] * pRhs[_M31])) + (pLhs[_M34] * pRhs[_M41]),
                  (((pLhs[_M31] * pRhs[_M12]) + (pLhs[_M32] * pRhs[_M22])) + (pLhs[_M33] * pRhs[_M32])) + (pLhs[_M34] * pRhs[_M42]),
                  (((pLhs[_M31] * pRhs[_M13]) + (pLhs[_M32] * pRhs[_M23])) + (pLhs[_M33] * pRhs[_M33])) + (pLhs[_M34] * pRhs[_M43]),
                  (((pLhs[_M31] * pRhs[_M14]) + (pLhs[_M32] * pRhs[_M24])) + (pLhs[_M33] * pRhs[_M34])) + (pLhs[_M34] * pRhs[_M44]),
                  (((pLhs[_M41] * pRhs[_M11]) + (pLhs[_M42] * pRhs[_M21])) + (pLhs[_M43] * pRhs[_M31])) + (pLhs[_M44] * pRhs[_M41]),
                  (((pLhs[_M41] * pRhs[_M12]) + (pLhs[_M42] * pRhs[_M22])) + (pLhs[_M43] * pRhs[_M32])) + (pLhs[_M44] * pRhs[_M42]),
                  (((pLhs[_M41] * pRhs[_M13]) + (pLhs[_M42] * pRhs[_M23])) + (pLhs[_M43] * pRhs[_M33])) + (pLhs[_M44] * pRhs[_M43]),
                  (((pLhs[_M41] * pRhs[_M14]) + (pLhs[_M42] * pRhs[_M24])) + (pLhs[_M43] * pRhs[_M34])) + (pLhs[_M44] * pRhs[_M44]));
  }

  // Compile time evaluated matrix multiply :)
  constexpr Matrix ConstMultiply(const Matrix& lhs, const Matrix& rhs)
  {
    return ConstMultiply(lhs.DirectAccess(), rhs.DirectAccess());
  }
}


TEST(TestMath_Matrix, Construct_Default)
{
  Matrix value;
  const float* const pDirect = value.DirectAccess();

  ASSERT_NE(pDirect, nullptr);
  for (uint32_t i = 0; i < (4 * 4); ++i)
  {
    EXPECT_EQ(0.0f, pDirect[i]) << " at index: " << i;
  }
}


TEST(TestMath_Matrix, Construct)
{
  Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
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
  EXPECT_EQ(10.0f, pDirect[9]);
  EXPECT_EQ(11.0f, pDirect[10]);
  EXPECT_EQ(12.0f, pDirect[11]);
  EXPECT_EQ(13.0f, pDirect[12]);
  EXPECT_EQ(14.0f, pDirect[13]);
  EXPECT_EQ(15.0f, pDirect[14]);
  EXPECT_EQ(16.0f, pDirect[15]);
}


TEST(TestMath_Matrix, Clear)
{
  Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  value.Clear();

  const float* const pDirect = value.DirectAccess();
  ASSERT_NE(pDirect, nullptr);
  for (uint32_t i = 0; i < (4 * 4); ++i)
  {
    EXPECT_EQ(0.0f, pDirect[i]) << " at index: " << i;
  }
}


TEST(TestMath_Matrix, Determinant)
{
  auto value = Matrix::GetIdentity();
  EXPECT_FLOAT_EQ(1.0f, value.Determinant());
}


TEST(TestMath_Matrix, GetBackward)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(9.0f, 10.0f, 11.0f), value.GetBackward());
}


TEST(TestMath_Matrix, GetDown)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(-5.0f, -6.0f, -7.0f), value.GetDown());
}


TEST(TestMath_Matrix, GetForward)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(-9.0f, -10.0f, -11.0f), value.GetForward());
}


TEST(TestMath_Matrix, GetLeft)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(-1.0f, -2.0f, -3.0f), value.GetLeft());
}


TEST(TestMath_Matrix, GetRight)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(1.0f, 2.0f, 3.0f), value.GetRight());
}


TEST(TestMath_Matrix, GetUp)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(5.0f, 6.0f, 7.0f), value.GetUp());
}


TEST(TestMath_Matrix, GetTranslation)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(Vector3(13.0f, 14.0f, 15.0f), value.GetTranslation());
}


TEST(TestMath_Matrix, SetBackward)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetBackward(Vector3(40.0f, 41.0f, 42.0f));

  EXPECT_EQ(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 40.0f, 41.0f, 42.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), res);
}


TEST(TestMath_Matrix, SetDown)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetDown(Vector3(40.0f, 41.0f, 42.0f));
  EXPECT_EQ(Matrix(1.0f, 2.0f, 3.0f, 4.0f, -40.0f, -41.0f, -42.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), res);
}


TEST(TestMath_Matrix, SetForward)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetForward(Vector3(40.0f, 41.0f, 42.0f));
  EXPECT_EQ(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, -40.0f, -41.0f, -42.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), res);
}


TEST(TestMath_Matrix, SetLeft)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetLeft(Vector3(40.0f, 41.0f, 42.0f));
  EXPECT_EQ(Matrix(-40.0f, -41.0f, -42.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), res);
}


TEST(TestMath_Matrix, SetRight)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetRight(Vector3(40.0f, 41.0f, 42.0f));
  EXPECT_EQ(Matrix(40.0f, 41.0f, 42.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), res);
}


TEST(TestMath_Matrix, SetUp)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetUp(Vector3(40.0f, 41.0f, 42.0f));
  EXPECT_EQ(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 40.0f, 41.0f, 42.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), res);
}


TEST(TestMath_Matrix, SetTranslation)
{
  //-----------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32----M33----M34----M41----M42----M43----M44
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  Matrix res = value;
  res.SetTranslation(Vector3(40.0f, 41.0f, 42.0f));

  EXPECT_EQ(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 40.0f, 41.0f, 42.0f, 16.0f), res);
}


TEST(TestMath_Matrix, Invert_Identity)
{
  const auto identity = Matrix::GetIdentity();
  const auto res = Matrix::Invert(identity);
  EXPECT_EQ(identity, res);
}


TEST(TestMath_Matrix, Transpose_Identity)
{
  const auto identity = Matrix::GetIdentity();
  const auto res = Matrix::Transpose(identity);
  EXPECT_EQ(identity, res);
}


TEST(TestMath_Matrix, SetIdentity)
{
  Matrix value(-1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  value.SetIdentity();
  EXPECT_EQ(Matrix::GetIdentity(), value);
}


TEST(TestMath_Matrix, CreateScale_float)
{
  auto matScale = Matrix::CreateScale(2.0f);

  //--------------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32---M33---M34---M41---M42---M43---M44
  //--------------------0-----1-----2-----3-----4-----5-----6-----7-----8-----9-----10----11----12----13----14----15
  const Matrix expected(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  EXPECT_EQ(expected, matScale);

  Matrix res2;
  Matrix::CreateScale(2.0f, res2);
  EXPECT_EQ(expected, res2);
}


TEST(TestMath_Matrix, CreateScale_floats)
{
  auto matScale = Matrix::CreateScale(2.0f, 3.0f, 4.0f);

  //--------------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32---M33---M34---M41---M42---M43---M44
  //--------------------0-----1-----2-----3-----4-----5-----6-----7-----8-----9-----10----11----12----13----14----15
  const Matrix expected(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  EXPECT_EQ(expected, matScale);

  Matrix res2;
  Matrix::CreateScale(2.0f, 3.0f, 4.0f, res2);
  EXPECT_EQ(expected, res2);
}


TEST(TestMath_Matrix, CreateScale_Vector3)
{
  auto matScale = Matrix::CreateScale(Vector3(2.0f, 3.0f, 4.0f));

  //--------------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32---M33---M34---M41---M42---M43---M44
  //--------------------0-----1-----2-----3-----4-----5-----6-----7-----8-----9-----10----11----12----13----14----15
  const Matrix expected(2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  EXPECT_EQ(expected, matScale);

  Matrix res2;
  Matrix::CreateScale(Vector3(2.0f, 3.0f, 4.0f), res2);
  EXPECT_EQ(expected, res2);
}


TEST(TestMath_Matrix, CreateTranslation_floats)
{
  auto matScale = Matrix::CreateTranslation(2.0f, 3.0f, 4.0f);

  //--------------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32---M33---M34---M41---M42---M43---M44
  //--------------------0-----1-----2-----3-----4-----5-----6-----7-----8-----9-----10----11----12----13----14----15
  const Matrix expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 3.0f, 4.0f, 1.0f);

  EXPECT_EQ(expected, matScale);

  Matrix res2;
  Matrix::CreateTranslation(2.0f, 3.0f, 4.0f, res2);
  EXPECT_EQ(expected, res2);
}


TEST(TestMath_Matrix, CreateTranslation_Vector3)
{
  auto matScale = Matrix::CreateTranslation(Vector3(2.0f, 3.0f, 4.0f));

  //--------------------M11---M12---M13---M14---M21---M22---M23---M24---M31---M32---M33---M34---M41---M42---M43---M44
  //--------------------0-----1-----2-----3-----4-----5-----6-----7-----8-----9-----10----11----12----13----14----15
  const Matrix expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 3.0f, 4.0f, 1.0f);

  EXPECT_EQ(expected, matScale);

  Matrix res2;
  Matrix::CreateTranslation(Vector3(2.0f, 3.0f, 4.0f), res2);
  EXPECT_EQ(expected, res2);
}


TEST(TestMath_Matrix, CreateOrthographic)
{
  constexpr float width = 320.0f;
  constexpr float height = 240.0f;
  constexpr float zNear = 0.001f;
  constexpr float zFar = 10.0f;
  const auto res = Matrix::CreateOrthographic(width, height, zNear, zFar);

  constexpr float m11 = 2.0f / width;
  constexpr float m22 = 2.0f / height;
  constexpr float m33 = 1.0f / (zNear - zFar);
  constexpr float m43 = zNear / (zNear - zFar);

  //--------------------M11--M12---M13---M14---M21---M22--M23---M24---M31---M32---M33--M34---M41---M42---M43---M44
  //--------------------0----1-----2-----3-----4-----5----6-----7-----8-----9-----10---11----12----13----14----15
  const Matrix expected(m11, 0.0f, 0.0f, 0.0f, 0.0f, m22, 0.0f, 0.0f, 0.0f, 0.0f, m33, 0.0f, 0.0f, 0.0f, m43, 1.0f);

  EXPECT_EQ(expected, res);
}


TEST(TestMath_Matrix, CreateOrthographic_Invalid)
{
  EXPECT_THROW(Matrix::CreateOrthographic(0.0f, 240.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(-320.0f, 240.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(320.0f, 0.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(320.0f, -240.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(320.0f, 240.0f, 0.0f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(320.0f, 240.0f, -0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(320.0f, 240.0f, 0.001f, 0.001f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreateOrthographic(320.0f, 240.0f, 0.001f, -10.0f), std::invalid_argument);
}


TEST(TestMath_Matrix, CreatePerspective_Invalid)
{
  EXPECT_THROW(Matrix::CreatePerspective(0.0f, 240.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(-320.0f, 240.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(320.0f, 0.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(320.0f, -240.0f, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(320.0f, 240.0f, 0.0f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(320.0f, 240.0f, -0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(320.0f, 240.0f, 0.001f, 0.001f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspective(320.0f, 240.0f, 0.001f, -10.0f), std::invalid_argument);
}


TEST(TestMath_Matrix, CreatePerspectiveFieldOfView_Invalid)
{
  float fieldOfView = MathHelper::ToRadians(90.0f);
  float aspectRatio = 320.0f / 240.0f;
  EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(0.0f, aspectRatio, 0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(-fieldOfView, aspectRatio, 0.001f, 10.0f), std::invalid_argument);

  // Aspect ratio of zero
  // EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(fieldOfView, 0.0f, 0.001f, 10.0f), std::invalid_argument);

  EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, 0.000f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, -0.001f, 10.0f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, 0.001f, 0.001f), std::invalid_argument);
  EXPECT_THROW(Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, 0.001f, -10.0f), std::invalid_argument);
}


TEST(TestMath_Matrix, CreateRotationX)
{
  const auto res = Matrix::CreateRotationX(MathHelper::RADS45);

  const float cosRadians = std::cos(MathHelper::RADS45);
  const float sinRadians = std::sin(MathHelper::RADS45);
  const float m22 = cosRadians;
  const float m23 = sinRadians;
  const float m32 = -sinRadians;
  const float m33 = cosRadians;

  //--------------------M11---M12---M13---M14---M21---M22--M23--M24---M31---M32--M33--M34---M41---M42---M43---M44
  //--------------------0-----1-----2-----3-----4-----5----6----7-----8-----9----10---11----12----13----14----15
  const Matrix expected(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, m22, m23, 0.0f, 0.0f, m32, m33, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  EXPECT_EQ(expected, res);
}


TEST(TestMath_Matrix, CreateRotationY)
{
  const auto res = Matrix::CreateRotationY(MathHelper::RADS45);

  const float cosRadians = std::cos(MathHelper::RADS45);
  const float sinRadians = std::sin(MathHelper::RADS45);
  const float m11 = cosRadians;
  const float m13 = -sinRadians;
  const float m31 = sinRadians;
  const float m33 = cosRadians;

  //--------------------M11--M12---M13--M14---M21---M22---M23---M24---M31--M32---M33--M34---M41---M42---M43---M44
  //--------------------0----1-----2----3-----4-----5-----6-----7-----8----9-----10---11----12----13----14----15
  const Matrix expected(m11, 0.0f, m13, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, m31, 0.0f, m33, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  EXPECT_EQ(expected, res);
}


TEST(TestMath_Matrix, CreateRotationZ)
{
  const auto res = Matrix::CreateRotationZ(MathHelper::RADS45);

  const float cosRadians = std::cos(MathHelper::RADS45);
  const float sinRadians = std::sin(MathHelper::RADS45);
  const float m11 = cosRadians;
  const float m12 = sinRadians;
  const float m21 = -sinRadians;
  const float m22 = cosRadians;

  //--------------------M11--M12--M13---M14---M21--M22--M23---M24---M31---M32---M33---M34---M41---M42---M43---M44
  //--------------------0----1----2-----3-----4----5----6-----7-----8-----9-----10----11----12----13----14----15
  const Matrix expected(m11, m12, 0.0f, 0.0f, m21, m22, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  EXPECT_EQ(expected, res);
}


TEST(TestMath_Matrix, Add)
{
  const Matrix value1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  const Matrix value2(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);
  const Matrix expectedResult(102.0f, 104.0f, 106.0f, 108.0f, 110.0f, 112.0f, 114.0f, 116.0f, 118.0f, 120.0f, 122.0f, 124.0f, 126.0f, 128.0f, 130.0f,
                              132.0f);

  auto res = Matrix::Add(value1, value2);
  EXPECT_EQ(expectedResult, res);

  Matrix res2;
  Matrix::Add(value1, value2, res2);
  EXPECT_EQ(expectedResult, res2);

  Matrix res3 = value1;
  res3 += value2;
  EXPECT_EQ(expectedResult, res3);

  const Matrix res4 = value1 + value2;
  EXPECT_EQ(expectedResult, res4);
}


TEST(TestMath_Matrix, Sub)
{
  const Matrix value1(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);
  const Matrix value2(16.0f, 15.0f, 14.0f, 13.0f, 12.0f, 11.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f);
  // 101-16=85, 102-15=87, 103-14=89, 104-13=91, 105-12, 106-11, 107-10, 108-9, 109-8, 110-7, 111-6, 112-5, 113-4, 114-3, 115-2, 116-1
  const Matrix expectedResult(85.0f, 87.0f, 89.0f, 91.0f, 93.0f, 95.0f, 97.0f, 99.0f, 101.0f, 103.0f, 105.0f, 107.0f, 109.0f, 111.0f, 113.0f, 115.0f);

  const auto res = Matrix::Subtract(value1, value2);
  EXPECT_EQ(expectedResult, res);

  Matrix res2;
  Matrix::Subtract(value1, value2, res2);
  EXPECT_EQ(expectedResult, res2);

  Matrix res3 = value1;
  res3 -= value2;
  EXPECT_EQ(expectedResult, res3);

  const Matrix res4 = value1 - value2;
  EXPECT_EQ(expectedResult, res4);
}


TEST(TestMath_Matrix, Divide)
{
  const Matrix value1(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);
  const Matrix value2(16.0f, 15.0f, 14.0f, 13.0f, 12.0f, 11.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.5f);
  const Matrix expectedResult(101.0f / 16.0f, 102.0f / 15.0f, 103.0f / 14.0f, 104.0f / 13.0f, 105.0f / 12.0f, 106.0f / 11.0f, 107.0f / 10.0f,
                              108.0f / 9.0f, 109.0f / 8.0f, 110.0f / 7.0f, 111.0f / 6.0f, 112.0f / 5.0f, 113.0f / 4.0f, 114.0f / 3.0f, 115.0f / 2.0f,
                              116.0f / 1.5f);

  const auto res = Matrix::Divide(value1, value2);
  EXPECT_EQ(expectedResult, res);

  Matrix res2;
  Matrix::Divide(value1, value2, res2);
  EXPECT_EQ(expectedResult, res2);

  // Matrix res3 = value1;
  // res3 /= value2;
  // EXPECT_EQ(expectedResult, res3);

  // const Matrix res4 = value1 / value2;
  // EXPECT_EQ(expectedResult, res4);
}


TEST(TestMath_Matrix, Divide_float)
{
  const Matrix value1(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);
  const float value2 = 2.0f;
  const Matrix expectedResult(101.0f / value2, 102.0f / value2, 103.0f / value2, 104.0f / value2, 105.0f / value2, 106.0f / value2, 107.0f / value2,
                              108.0f / value2, 109.0f / value2, 110.0f / value2, 111.0f / value2, 112.0f / value2, 113.0f / value2, 114.0f / value2,
                              115.0f / value2, 116.0f / value2);

  const auto res = Matrix::Divide(value1, value2);
  EXPECT_EQ(expectedResult, res);

  Matrix res2;
  Matrix::Divide(value1, value2, res2);
  EXPECT_EQ(expectedResult, res2);

  // Matrix res3 = value1;
  // res3 /= divValue;
  // EXPECT_EQ(expectedResult, res3);

  // const Matrix res4 = value1 / divValue;
  // EXPECT_EQ(expectedResult, res4);
}


TEST(TestMath_Matrix, Multiply_float)
{
  const Matrix value1(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);
  const float value2 = 2.0f;
  const Matrix expectedResult(101.0f * value2, 102.0f * value2, 103.0f * value2, 104.0f * value2, 105.0f * value2, 106.0f * value2, 107.0f * value2,
                              108.0f * value2, 109.0f * value2, 110.0f * value2, 111.0f * value2, 112.0f * value2, 113.0f * value2, 114.0f * value2,
                              115.0f * value2, 116.0f * value2);

  const auto res = Matrix::Multiply(value1, value2);
  EXPECT_EQ(expectedResult, res);

  Matrix res2;
  Matrix::Multiply(value1, value2, res2);
  EXPECT_EQ(expectedResult, res2);

  // Matrix res3 = value1;
  // res3 *= value2;
  // EXPECT_EQ(expectedResult, res3);

  // const Matrix res4 = value1 * value2;
  // EXPECT_EQ(expectedResult, res4);
}


TEST(TestMath_Matrix, Multiply)
{
  constexpr Matrix value1(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f,
                          116.0f);
  constexpr Matrix value2(16.0f, 15.0f, 14.0f, 13.0f, 12.0f, 11.0f, 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.5f);

  constexpr Matrix expectedResult = ConstMultiply(value1, value2);

  const auto res = Matrix::Multiply(value1, value2);
  EXPECT_EQ(expectedResult, res);

  Matrix res2;
  Matrix::Multiply(value1, value2, res2);
  EXPECT_EQ(expectedResult, res2);

  Matrix res3 = value1;
  res3 *= value2;
  EXPECT_EQ(expectedResult, res3);

  const Matrix res4 = value1 * value2;
  EXPECT_EQ(expectedResult, res4);
}


TEST(TestMath_Matrix, Lerp)
{
  const Matrix value1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  const Matrix value2(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);

  EXPECT_EQ(value1, Matrix::Lerp(value1, value2, 0.0f));
  EXPECT_EQ(value2, Matrix::Lerp(value1, value2, 1.0f));

  CheckLerp(value1, value2, 0.25f, Matrix::Lerp(value1, value2, 0.25f));
  CheckLerp(value1, value2, 0.50f, Matrix::Lerp(value1, value2, 0.50f));
  CheckLerp(value1, value2, 0.75f, Matrix::Lerp(value1, value2, 0.75f));
}


TEST(TestMath_Matrix, Lerp_Ref)
{
  const Matrix value1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  const Matrix value2(101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f, 108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f);

  Matrix res1;
  Matrix::Lerp(value1, value2, 0.0f, res1);
  Matrix res2;
  Matrix::Lerp(value1, value2, 1.0f, res2);
  EXPECT_EQ(value1, res1);
  EXPECT_EQ(value2, res2);

  Matrix res3;
  Matrix::Lerp(value1, value2, 0.25f, res3);
  CheckLerp(value1, value2, 0.25f, res3);
  Matrix::Lerp(value1, value2, 0.50f, res3);
  CheckLerp(value1, value2, 0.50f, res3);
  Matrix::Lerp(value1, value2, 0.75f, res3);
  CheckLerp(value1, value2, 0.75f, res3);
}


TEST(TestMath_Matrix, Negate)
{
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  const Matrix expectedResult(-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -9.0f, -10.0f, -11.0f, -12.0f, -13.0f, -14.0f, -15.0f, -16.0f);

  const auto res1 = Matrix::Negate(value);
  EXPECT_EQ(expectedResult, res1);

  Matrix res2;
  Matrix::Negate(value, res2);
  EXPECT_EQ(expectedResult, res2);

  const Matrix res3 = -value;
  EXPECT_EQ(expectedResult, res3);
}


TEST(TestMath_Matrix, Equal)
{
  const auto value = Matrix::GetIdentity();
  const Matrix value1(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);
  const Matrix value2(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_EQ(value, value);
  EXPECT_EQ(value1, value2);
  EXPECT_EQ(value2, value1);
  EXPECT_EQ(value1, value1);
  EXPECT_EQ(value2, value2);
}


TEST(TestMath_Matrix, NotEqual)
{
  const Matrix value(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

  EXPECT_NE(Matrix(-1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, -2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, -3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, -4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, -5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, -6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, -7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, -8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, -9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, -10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, -11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, -12.0f, 13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, -13.0f, 14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, -14.0f, 15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, -15.0f, 16.0f), value);
  EXPECT_NE(Matrix(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, -16.0f), value);
}
