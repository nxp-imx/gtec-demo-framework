#ifndef FSLBASE_MATH_DP_TYPECONVERTER_MATH_HPP
#define FSLBASE_MATH_DP_TYPECONVERTER_MATH_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022 NXP
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

#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Math/Dp/DpPoint2F.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <cmath>

namespace Fsl::TypeConverter
{
  // The in this file are responsible for converting
  // DP     -> normal
  // normal -> DP


  // --- DpPoint2F

  template <>
  constexpr inline DpPoint2F To<DpPoint2F, Vector2>(const Vector2& value)
  {
    return DpPoint2F::Create(value.X, value.Y);
  }


  // --- Vector2

  template <>
  constexpr inline Vector2 To<Vector2, DpPoint2>(const DpPoint2& value)
  {
    return {static_cast<float>(value.X.Value), static_cast<float>(value.Y.Value)};
  }

  template <>
  constexpr inline Vector2 To<Vector2, DpPoint2F>(const DpPoint2F& value)
  {
    return {value.X.Value, value.Y.Value};
  }
}

#endif
