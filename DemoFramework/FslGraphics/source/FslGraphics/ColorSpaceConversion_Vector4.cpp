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

#include <FslGraphics/ColorSpaceConversion_Vector4.hpp>

namespace Fsl::ColorSpaceConversion
{
  //! @brief Convert a SRGB color to a linear color stored as a Vector4
  Vector4 ConvertSRGBToLinearVector4(const Color colorSRGB)
  {
    // The alpha is a linear value so it should not be converted
    return {ConvertSRGBToLinearFloat(colorSRGB.RawR()), ConvertSRGBToLinearFloat(colorSRGB.RawG()), ConvertSRGBToLinearFloat(colorSRGB.RawB()),
            ConvertLinearUInt8ToLinearFloat(colorSRGB.RawA())};
  }

  //! @brief Convert a SRGB color to a linear color stored as a Vector4
  Color ConvertLinearToSRGBColor(const Vector4& colorLinear)
  {
    // The alpha is a linear value so it should not be converted
    return {ConvertLinearToSRGBUInt8(colorLinear.X), ConvertLinearToSRGBUInt8(colorLinear.Y), ConvertLinearToSRGBUInt8(colorLinear.Z),
            ConvertLinearFloatToLinearUInt8(colorLinear.W)};
  }
}
