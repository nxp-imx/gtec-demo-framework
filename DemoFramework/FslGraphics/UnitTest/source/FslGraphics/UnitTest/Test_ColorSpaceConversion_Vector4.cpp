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
#include <FslGraphics/ColorSpaceConversion_Vector4.hpp>
#include <FslGraphics/Log/LogColor.hpp>
#include <FslGraphics/UnitTest/Helper/TestFixtureFslGraphics.hpp>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace Fsl;

namespace
{
  using Test_ColorSpaceConversion_Vector4 = TestFixtureFslGraphics;
}


TEST(Test_ColorSpaceConversion_Vector4, ConvertSRGBToLinearVector4_RoundTrip_R)
{
  for (uint16_t i = 0; i <= 255; ++i)
  {
    const auto r = static_cast<uint8_t>(i);
    const uint8_t g = 202;
    const uint8_t b = 203;
    const uint8_t a = 204;
    const Color srcColor(r, g, b, a);
    const Vector4 linearValue = ColorSpaceConversion::ConvertSRGBToLinearVector4(srcColor);
    const Color returnedValue = ColorSpaceConversion::ConvertLinearToSRGBColor(linearValue);
    EXPECT_EQ(srcColor, returnedValue);
  }
}


TEST(Test_ColorSpaceConversion_Vector4, ConvertSRGBToLinearVector4_RoundTrip_G)
{
  for (uint16_t i = 0; i <= 255; ++i)
  {
    const uint8_t r = 201;
    const auto g = static_cast<uint8_t>(i);
    const uint8_t b = 203;
    const uint8_t a = 204;
    const Color srcColor(r, g, b, a);
    const Vector4 linearValue = ColorSpaceConversion::ConvertSRGBToLinearVector4(srcColor);
    const Color returnedValue = ColorSpaceConversion::ConvertLinearToSRGBColor(linearValue);
    EXPECT_EQ(srcColor, returnedValue);
  }
}


TEST(Test_ColorSpaceConversion_Vector4, ConvertSRGBToLinearVector4_RoundTrip_B)
{
  for (uint16_t i = 0; i <= 255; ++i)
  {
    const uint8_t r = 201;
    const uint8_t g = 202;
    const auto b = static_cast<uint8_t>(i);
    const uint8_t a = 204;
    const Color srcColor(r, g, b, a);
    const Vector4 linearValue = ColorSpaceConversion::ConvertSRGBToLinearVector4(srcColor);
    const Color returnedValue = ColorSpaceConversion::ConvertLinearToSRGBColor(linearValue);
    EXPECT_EQ(srcColor, returnedValue);
  }
}


TEST(Test_ColorSpaceConversion_Vector4, ConvertSRGBToLinearVector4_RoundTrip_A)
{
  for (uint16_t i = 0; i <= 255; ++i)
  {
    const uint8_t r = 201;
    const uint8_t g = 202;
    const uint8_t b = 203;
    const auto a = static_cast<uint8_t>(i);
    const Color srcColor(r, g, b, a);
    const Vector4 linearValue = ColorSpaceConversion::ConvertSRGBToLinearVector4(srcColor);
    const Color returnedValue = ColorSpaceConversion::ConvertLinearToSRGBColor(linearValue);
    EXPECT_EQ(srcColor, returnedValue);
  }
}
