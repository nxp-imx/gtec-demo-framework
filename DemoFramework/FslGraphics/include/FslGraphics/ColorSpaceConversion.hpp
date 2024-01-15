#ifndef FSLGRAPHICS_COLORSPACECONVERSION_HPP
#define FSLGRAPHICS_COLORSPACECONVERSION_HPP
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

#include <FslBase/BasicTypes.hpp>
#include <FslGraphics/Color.hpp>
#include <cassert>
#include <cmath>
#include <limits>

namespace Fsl::ColorSpaceConversion
{
  //! @brief Convert a single value from SRGB to linear colorspace
  float ConvertSRGBToLinearFloat(const float valueSRGB);


  //! @brief Convert a single value from linear to SRGB colorspace
  float ConvertLinearToSRGBFloat(const float valueLinear);


  //! @brief Convert a single uint8_t value from SRGB to linear colorspace
  float ConvertSRGBToLinearFloat(const uint8_t valueSRGB);

  //! @brief Convert a single float value from linear to SRGB colorspace encoded as a uint8_t
  uint8_t ConvertLinearToSRGBUInt8(const float valueLinear);


  //! @brief Convert a single uint8_t value from SRGB to linear colorspace encoded as a uint16_t
  //! @return Values will be in the range 0 to 0xFFFF
  uint16_t ConvertSRGBToLinearUInt16(const uint8_t valueSRGB);
  uint8_t ConvertLinearToSRGBUInt8(const uint16_t colorLinear);

  //! @brief Very lossy convert a SRGB value to a linear uint8, this will result in massive precision loss for darker colors!!
  extern uint8_t LossyConvertSRGBToLinearUInt8(const uint8_t valueSRGB);

  //! @brief Very lossy convert a SRGB value to a linear uint8
  //! @warning this will result in massive precision loss for darker colors!!
  extern Color LossyConvertSRGBToLinearColor(const Color colorSRGB);
}

#endif
