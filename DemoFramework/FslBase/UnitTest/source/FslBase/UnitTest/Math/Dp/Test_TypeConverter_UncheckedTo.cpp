/****************************************************************************************************************************************************
 * Copyright 2022 NXP
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

#include <FslBase/Log/Math/Dp/LogDpExtent2D.hpp>
#include <FslBase/Log/Math/Dp/LogDpPoint2.hpp>
#include <FslBase/Log/Math/Dp/LogDpPoint2F.hpp>
#include <FslBase/Log/Math/Dp/LogDpPoint2U.hpp>
#include <FslBase/Log/Math/Dp/LogDpSize2D.hpp>
#include <FslBase/Log/Math/Dp/LogDpSize2DF.hpp>
#include <FslBase/Log/Math/Dp/LogDpThickness.hpp>
#include <FslBase/Log/Math/Dp/LogDpThicknessF.hpp>
#include <FslBase/Log/Math/Dp/LogDpThicknessU.hpp>
#include <FslBase/Log/Math/Dp/LogDpValue.hpp>
#include <FslBase/Log/Math/Dp/LogDpValueF.hpp>
#include <FslBase/Log/Math/Dp/LogDpValueU.hpp>
#include <FslBase/Math/Dp/TypeConverter.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_TypeConverter_UncheckedTo = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpValue
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValue_DpValue)
{
  EXPECT_EQ(DpValue(42), TypeConverter::UncheckedTo<DpValue>(DpValue(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpValue_DpValueF)
{
  EXPECT_EQ(DpValue(42), TypeConverter::UncheckedChangeTo<DpValue>(DpValueF(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValue_DpValueU)
{
  EXPECT_EQ(DpValue(42), TypeConverter::UncheckedTo<DpValue>(DpValueU(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValue_DpSize1D)
{
  EXPECT_EQ(DpValue(42), TypeConverter::UncheckedTo<DpValue>(DpSize1D::Create(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpValue_DpSize1DF)
{
  EXPECT_EQ(DpValue(42), TypeConverter::UncheckedChangeTo<DpValue>(DpSize1DF::Create(42)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpValueF
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueF_DpValue)
{
  EXPECT_EQ(DpValueF(42), TypeConverter::UncheckedTo<DpValueF>(DpValue(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueF_DpValueF)
{
  EXPECT_EQ(DpValueF(42), TypeConverter::UncheckedTo<DpValueF>(DpValueF(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueF_DpValueU)
{
  EXPECT_EQ(DpValueF(42), TypeConverter::UncheckedTo<DpValueF>(DpValueU(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueF_DpSize1D)
{
  EXPECT_EQ(DpValueF(42), TypeConverter::UncheckedTo<DpValueF>(DpSize1D::Create(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueF_DpSize1DF)
{
  EXPECT_EQ(DpValueF(42), TypeConverter::UncheckedTo<DpValueF>(DpSize1DF::Create(42)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpValueU
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueU_DpValue)
{
  EXPECT_EQ(DpValueU(42), TypeConverter::UncheckedTo<DpValueU>(DpValue(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpValueU_DpValueF)
{
  EXPECT_EQ(DpValueU(42), TypeConverter::UncheckedChangeTo<DpValueU>(DpValueF(42)));
}

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueU_DpValueU)
{
  EXPECT_EQ(DpValueU(42), TypeConverter::UncheckedTo<DpValueU>(DpValueU(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpValueU_DpSize1D)
{
  EXPECT_EQ(DpValueU(42), TypeConverter::UncheckedTo<DpValueU>(DpSize1D::Create(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpValueU_DpSize1DF)
{
  EXPECT_EQ(DpValueU(42), TypeConverter::UncheckedChangeTo<DpValueU>(DpSize1DF::Create(42)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpSize1D
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1D_DpValue)
{
  EXPECT_EQ(DpSize1D::Create(42), TypeConverter::UncheckedTo<DpSize1D>(DpValue(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpSize1D_DpValueF)
{
  EXPECT_EQ(DpSize1D::Create(42), TypeConverter::UncheckedChangeTo<DpSize1D>(DpValueF(42)));
}

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1D_DpValueU)
{
  EXPECT_EQ(DpSize1D::Create(42), TypeConverter::UncheckedTo<DpSize1D>(DpValueU(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1D_DpSize1D)
{
  EXPECT_EQ(DpSize1D::Create(42), TypeConverter::UncheckedTo<DpSize1D>(DpSize1D::Create(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpSize1D_DpSize1DF)
{
  EXPECT_EQ(DpSize1D::Create(42), TypeConverter::UncheckedChangeTo<DpSize1D>(DpSize1DF::Create(42)));
}


// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpSize1DF
// ---------------------------------------------------------------------------------------------------------------------------------------------------


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1DF_DpValue)
{
  EXPECT_EQ(DpSize1DF::Create(42), TypeConverter::UncheckedTo<DpSize1DF>(DpValue(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1DF_DpValueF)
{
  EXPECT_EQ(DpSize1DF::Create(42), TypeConverter::UncheckedTo<DpSize1DF>(DpValueF(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1DF_DpValueU)
{
  EXPECT_EQ(DpSize1DF::Create(42), TypeConverter::UncheckedTo<DpSize1DF>(DpValueU(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1DF_DpSize1D)
{
  EXPECT_EQ(DpSize1DF::Create(42), TypeConverter::UncheckedTo<DpSize1DF>(DpSize1D::Create(42)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize1DF_DpSize1DF)
{
  EXPECT_EQ(DpSize1DF::Create(42), TypeConverter::UncheckedTo<DpSize1DF>(DpSize1DF::Create(42)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpExtent2D
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpExtent2D_DpPoint2)
{
  EXPECT_EQ(DpExtent2D::Create(42, 32), TypeConverter::UncheckedTo<DpExtent2D>(DpPoint2::Create(42, 32)));
}

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpExtent2D_DpPoint2F)
{
  EXPECT_EQ(DpExtent2D::Create(42, 32), TypeConverter::UncheckedChangeTo<DpExtent2D>(DpPoint2F::Create(42, 32)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpExtent2D_DpSize2D)
{
  EXPECT_EQ(DpExtent2D::Create(42, 32), TypeConverter::UncheckedTo<DpExtent2D>(DpSize2D::Create(42, 32)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpPoint2
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpPoint2_DpExtent2D)
{
  EXPECT_EQ(DpPoint2::Create(42, 32), TypeConverter::UncheckedTo<DpPoint2>(DpExtent2D::Create(42, 32)));
}

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpPoint2_DpPoint2F)
{
  EXPECT_EQ(DpPoint2::Create(42, 32), TypeConverter::UncheckedChangeTo<DpPoint2>(DpPoint2F::Create(42, 32)));
}

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpPoint2_DpSize2D)
{
  EXPECT_EQ(DpPoint2::Create(42, 32), TypeConverter::UncheckedTo<DpPoint2>(DpSize2D::Create(42, 32)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpPoint2F
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpPoint2F_DpExtent2D)
{
  EXPECT_EQ(DpPoint2F::Create(42, 32), TypeConverter::UncheckedTo<DpPoint2F>(DpExtent2D::Create(42, 32)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpPoint2F_DpPoint2)
{
  EXPECT_EQ(DpPoint2F::Create(42, 32), TypeConverter::UncheckedTo<DpPoint2F>(DpPoint2::Create(42, 32)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpPoint2F_DpSize2D)
{
  EXPECT_EQ(DpPoint2F::Create(42, 32), TypeConverter::UncheckedTo<DpPoint2F>(DpSize2D::Create(42, 32)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpSize2D
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize2D_DpExtent2D)
{
  EXPECT_EQ(DpSize2D::Create(42, 32), TypeConverter::UncheckedTo<DpSize2D>(DpExtent2D::Create(42, 32)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpSize2D_DpPoint2)
{
  EXPECT_EQ(DpSize2D::Create(42, 32), TypeConverter::UncheckedTo<DpSize2D>(DpPoint2::Create(42, 32)));
}


TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpSize2D_DpPoint2F)
{
  EXPECT_EQ(DpSize2D::Create(42, 32), TypeConverter::UncheckedChangeTo<DpSize2D>(DpPoint2F::Create(42, 32)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpThicknessF
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpThicknessF_DpThickness)
{
  EXPECT_EQ(DpThicknessF::Create(42, 32, 21, 19), TypeConverter::UncheckedTo<DpThicknessF>(DpThickness::Create(42, 32, 21, 19)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpThicknessU
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedTo_DpThicknessU_DpThickness)
{
  EXPECT_EQ(DpThicknessU::Create(42, 32, 21, 19), TypeConverter::To<DpThicknessU>(DpThickness::Create(42, 32, 21, 19)));
}

TEST(TestMathDp_TypeConverter_UncheckedTo, UncheckedChangeTo_DpThicknessU_DpThicknessF)
{
  EXPECT_EQ(DpThicknessU::Create(42, 32, 21, 19), TypeConverter::UncheckedChangeTo<DpThicknessU>(DpThicknessF::Create(42, 32, 21, 19)));
}
