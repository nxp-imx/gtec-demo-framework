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
#include <FslBase/Log/Math/LogVector2.hpp>
#include <FslBase/Math/Dp/TypeConverter_Math.hpp>
#include <FslBase/UnitTest/Helper/TestFixtureFslBase.hpp>

using namespace Fsl;

namespace
{
  using TestMathDp_TypeConverter_Math_To = TestFixtureFslBase;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// DpPoint2F
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_Math_To, To_DpPoint2F_Vector2)
{
  EXPECT_EQ(DpPoint2F::Create(42.2f, 19.3f), TypeConverter::To<DpPoint2F>(Vector2(42.2f, 19.3f)));
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Vector2
// ---------------------------------------------------------------------------------------------------------------------------------------------------

TEST(TestMathDp_TypeConverter_Math_To, To_Vector2_DpPoint2)
{
  EXPECT_EQ(Vector2(42, 19), TypeConverter::To<Vector2>(DpPoint2::Create(42, 19)));
}

TEST(TestMathDp_TypeConverter_Math_To, To_Vector2_DpPoint2F)
{
  EXPECT_EQ(Vector2(42.2f, 19.3f), TypeConverter::To<Vector2>(DpPoint2F::Create(42.2f, 19.3f)));
}
