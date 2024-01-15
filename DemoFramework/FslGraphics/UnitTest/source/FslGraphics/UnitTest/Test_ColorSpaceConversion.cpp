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

#include <FslBase/Exceptions.hpp>
#include <FslGraphics/ColorSpaceConversion.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_ColorSpaceConversion = TestFixtureFslGraphics;
}


TEST(Test_ColorSpaceConversion, Construct_MinMax_FLoat)
{
  const float convertedMin = ColorSpaceConversion::ConvertSRGBToLinearFloat(0.0f);
  const float convertedMax = ColorSpaceConversion::ConvertSRGBToLinearFloat(1.0f);
  EXPECT_EQ(0.0f, convertedMin);
  EXPECT_EQ(1.0f, convertedMax);
}


TEST(Test_ColorSpaceConversion, Construct_MinMax_UInt8)
{
  const float convertedMin = ColorSpaceConversion::ConvertSRGBToLinearFloat(static_cast<uint8_t>(0));
  const float convertedMax = ColorSpaceConversion::ConvertSRGBToLinearFloat(static_cast<uint8_t>(255));
  EXPECT_EQ(0.0f, convertedMin);
  EXPECT_EQ(1.0f, convertedMax);
}


TEST(Test_ColorSpaceConversion, ConvertLinearToSRGBUInt8_LinearFloat_RoundTrip)
{
  for (uint16_t i = 0; i <= 255; ++i)
  {
    const float linearValue = ColorSpaceConversion::ConvertSRGBToLinearFloat(static_cast<uint8_t>(i));
    const uint8_t returnedValue = ColorSpaceConversion::ConvertLinearToSRGBUInt8(linearValue);
    EXPECT_EQ(i, static_cast<uint16_t>(returnedValue));
  }
}


TEST(Test_ColorSpaceConversion, ConvertLinearToSRGBUInt8_LinearUInt16_RoundTrip)
{
  for (uint16_t i = 0; i <= 255; ++i)
  {
    const uint16_t linearValue = ColorSpaceConversion::ConvertSRGBToLinearUInt16(static_cast<uint8_t>(i));
    const uint8_t returnedValue = ColorSpaceConversion::ConvertLinearToSRGBUInt8(linearValue);
    EXPECT_EQ(i, static_cast<uint16_t>(returnedValue));
  }
}


// TEST(Test_ColorSpaceConversion, ConvertLinearToSRGBUInt8_LinearUInt8_RoundTrip)
//{
//   for (uint16_t i = 0; i <= 255; ++i)
//   {
//     const uint8_t linearValue = ColorSpaceConversion::LossyConvertSRGBToLinearUInt8(static_cast<uint8_t>(i));
//     const float linearValueF = (static_cast<float>(linearValue) / 255.0f);
//     const uint8_t returnedValue = ColorSpaceConversion::ConvertLinearToSRGBUInt8(linearValueF);
//     EXPECT_EQ(i, static_cast<uint16_t>(returnedValue));
//   }
// }
