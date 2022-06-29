#ifndef FSLBASE_MATH_DP_TYPECONVERTER_HPP
#define FSLBASE_MATH_DP_TYPECONVERTER_HPP
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

#include <FslBase/Math/Dp/DpExtent2D.hpp>
#include <FslBase/Math/Dp/DpPoint2.hpp>
#include <FslBase/Math/Dp/DpSize1D.hpp>
#include <FslBase/Math/Dp/DpSize1DF.hpp>
#include <FslBase/Math/Dp/DpSize2D.hpp>
#include <FslBase/Math/Dp/DpThickness.hpp>
#include <FslBase/Math/Dp/DpThicknessF.hpp>
#include <FslBase/Math/Dp/DpThicknessU.hpp>
#include <FslBase/Math/Dp/DpValue.hpp>
#include <FslBase/Math/Dp/DpValueF.hpp>
#include <FslBase/Math/Dp/DpValueU.hpp>
#include <FslBase/Math/TypeConverter_Float.hpp>
#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Fsl::TypeConverter
{
  // The in this file are responsible for converting between various DP types

  // --- DpValue

  // Basically a no-op
  template <>
  constexpr inline DpValue To<DpValue, DpValue>(const DpValue& value)
  {
    return value;
  }

  template <>
  inline DpValue ChangeTo<DpValue, DpValueF>(const DpValueF& value)
  {
    return DpValue(ChangeTo<DpValue::value_type>(value.Value));
  }

  template <>
  constexpr inline DpValue To<DpValue, DpValueU>(const DpValueU& value)
  {
    return DpValue(NumericCast<DpValue::value_type>(value.Value));
  }

  template <>
  constexpr inline DpValue To<DpValue, DpSize1D>(const DpSize1D& value)
  {
    return To<DpValue>(value.Value());
  }

  template <>
  inline DpValue ChangeTo<DpValue, DpSize1DF>(const DpSize1DF& value)
  {
    return ChangeTo<DpValue>(value.Value());
  }

  // --- DpValueF

  template <>
  constexpr inline DpValueF To<DpValueF, DpValue>(const DpValue& value)
  {
    return DpValueF(static_cast<float>(value.Value));
  }

  template <>
  constexpr inline DpValueF To<DpValueF, DpValueU>(const DpValueU& value)
  {
    return DpValueF(static_cast<float>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline DpValueF To<DpValueF, DpValueF>(const DpValueF& value)
  {
    return value;
  }

  template <>
  constexpr inline DpValueF To<DpValueF, DpSize1D>(const DpSize1D& value)
  {
    return To<DpValueF>(value.Value());
  }

  template <>
  constexpr inline DpValueF To<DpValueF, DpSize1DF>(const DpSize1DF& value)
  {
    return To<DpValueF>(value.Value());
  }

  // --- DpValueU

  template <>
  constexpr inline DpValueU To<DpValueU, DpValue>(const DpValue& value)
  {
    return DpValueU(NumericCast<DpValueU::value_type>(value.Value));
  }

  template <>
  inline DpValueU ChangeTo<DpValueU, DpValueF>(const DpValueF& value)
  {
    return DpValueU(ChangeTo<DpValueU::value_type>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline DpValueU To<DpValueU, DpValueU>(const DpValueU& value)
  {
    return value;
  }

  template <>
  constexpr inline DpValueU To<DpValueU, DpSize1D>(const DpSize1D& value)
  {
    return To<DpValueU>(value.Value());
  }

  template <>
  inline DpValueU ChangeTo<DpValueU, DpSize1DF>(const DpSize1DF& value)
  {
    return ChangeTo<DpValueU>(value.Value());
  }


  // --- DpSize1D

  template <>
  constexpr inline DpSize1D To<DpSize1D, DpValue>(const DpValue& value)
  {
    return DpSize1D(value);
  }

  template <>
  constexpr inline DpSize1D To<DpSize1D, DpValueU>(const DpValueU& value)
  {
    return DpSize1D(To<DpValue>(value));
  }

  template <>
  inline DpSize1D ChangeTo<DpSize1D, DpValueF>(const DpValueF& value)
  {
    return DpSize1D(ChangeTo<DpValue>(value));
  }

  // Basically a no-op
  template <>
  constexpr inline DpSize1D To<DpSize1D, DpSize1D>(const DpSize1D& value)
  {
    return value;
  }

  template <>
  inline DpSize1D ChangeTo<DpSize1D, DpSize1DF>(const DpSize1DF& value)
  {
    return ChangeTo<DpSize1D>(value.Value());
  }


  // --- DpSize1DF

  template <>
  constexpr inline DpSize1DF To<DpSize1DF, DpValue>(const DpValue& value)
  {
    return DpSize1DF(To<DpValueF>(value));
  }

  template <>
  constexpr inline DpSize1DF To<DpSize1DF, DpValueU>(const DpValueU& value)
  {
    return DpSize1DF(To<DpValueF>(value));
  }

  template <>
  constexpr inline DpSize1DF To<DpSize1DF, DpValueF>(const DpValueF& value)
  {
    return DpSize1DF(value);
  }

  template <>
  constexpr inline DpSize1DF To<DpSize1DF, DpSize1D>(const DpSize1D& value)
  {
    return To<DpSize1DF>(value.Value());
  }

  // Basically a no-op
  template <>
  constexpr inline DpSize1DF To<DpSize1DF, DpSize1DF>(const DpSize1DF& value)
  {
    return value;
  }

  // --- DpExtent2D

  template <>
  constexpr inline DpExtent2D To<DpExtent2D, DpPoint2>(const DpPoint2& value)
  {
    return {To<DpExtent2D::value_type>(value.X), To<DpExtent2D::value_type>(value.Y)};
  }

  template <>
  inline DpExtent2D ChangeTo<DpExtent2D, DpPoint2F>(const DpPoint2F& value)
  {
    return {ChangeTo<DpExtent2D::value_type>(value.X), ChangeTo<DpExtent2D::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpExtent2D To<DpExtent2D, DpSize2D>(const DpSize2D& value)
  {
    return {To<DpExtent2D::value_type>(value.Width()), To<DpExtent2D::value_type>(value.Height())};
  }

  // --- DpPoint2

  template <>
  constexpr inline DpPoint2 To<DpPoint2, DpExtent2D>(const DpExtent2D& value)
  {
    return {To<DpPoint2::value_type>(value.Width), To<DpPoint2::value_type>(value.Height)};
  }

  template <>
  inline DpPoint2 ChangeTo<DpPoint2, DpPoint2F>(const DpPoint2F& value)
  {
    return {ChangeTo<DpPoint2::value_type>(value.X), ChangeTo<DpPoint2::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpPoint2 To<DpPoint2, DpSize2D>(const DpSize2D& value)
  {
    return {To<DpPoint2::value_type>(value.Width()), To<DpPoint2::value_type>(value.Height())};
  }

  // --- DpPoint2F

  template <>
  constexpr inline DpPoint2F To<DpPoint2F, DpExtent2D>(const DpExtent2D& value)
  {
    return {To<DpPoint2F::value_type>(value.Width), To<DpPoint2F::value_type>(value.Height)};
  }

  template <>
  constexpr inline DpPoint2F To<DpPoint2F, DpPoint2>(const DpPoint2& value)
  {
    return {To<DpPoint2F::value_type>(value.X), To<DpPoint2F::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpPoint2F To<DpPoint2F, DpSize2D>(const DpSize2D& value)
  {
    return {To<DpPoint2F::value_type>(value.Width()), To<DpPoint2F::value_type>(value.Height())};
  }

  // --- DpSize2D

  template <>
  constexpr inline DpSize2D To<DpSize2D, DpPoint2>(const DpPoint2& value)
  {
    return {To<DpSize2D::value_type>(value.X), To<DpSize2D::value_type>(value.Y)};
  }

  template <>
  inline DpSize2D ChangeTo<DpSize2D, DpPoint2F>(const DpPoint2F& value)
  {
    return {ChangeTo<DpSize2D::value_type>(value.X), ChangeTo<DpSize2D::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpSize2D To<DpSize2D, DpExtent2D>(const DpExtent2D& value)
  {
    return {To<DpSize2D::value_type>(value.Width), To<DpSize2D::value_type>(value.Height)};
  }

  template <>
  inline DpSize2D ChangeTo<DpSize2D, DpSize2DF>(const DpSize2DF& value)
  {
    return {ChangeTo<DpSize2D::value_type>(value.Width()), ChangeTo<DpSize2D::value_type>(value.Height())};
  }

  // --- DpThicknessF

  template <>
  constexpr inline DpThicknessF To<DpThicknessF, DpThickness>(const DpThickness& value)
  {
    return {To<DpThicknessF::value_type>(value.Left()), To<DpThicknessF::value_type>(value.Top()), To<DpThicknessF::value_type>(value.Right()),
            To<DpThicknessF::value_type>(value.Bottom())};
  }

  // --- DpThicknessU

  template <>
  constexpr inline DpThicknessU To<DpThicknessU, DpThickness>(const DpThickness& value)
  {
    return {To<DpThicknessU::value_type>(value.Left()), To<DpThicknessU::value_type>(value.Top()), To<DpThicknessU::value_type>(value.Right()),
            To<DpThicknessU::value_type>(value.Bottom())};
  }

  template <>
  inline DpThicknessU ChangeTo<DpThicknessU, DpThicknessF>(const DpThicknessF& value)
  {
    return {ChangeTo<DpThicknessU::value_type>(value.Left()), ChangeTo<DpThicknessU::value_type>(value.Top()),
            ChangeTo<DpThicknessU::value_type>(value.Right()), ChangeTo<DpThicknessU::value_type>(value.Bottom())};
  }


  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------------------------------------------------------------------

  // --- DpValue

  // Basically a no-op
  template <>
  constexpr inline DpValue UncheckedTo<DpValue, DpValue>(const DpValue& value) noexcept
  {
    return value;
  }

  template <>
  inline DpValue UncheckedChangeTo<DpValue, DpValueF>(const DpValueF& value)
  {
    return DpValue(UncheckedChangeTo<DpValue::value_type>(value.Value));
  }

  template <>
  constexpr inline DpValue UncheckedTo<DpValue, DpValueU>(const DpValueU& value) noexcept
  {
    return DpValue(UncheckedNumericCast<DpValue::value_type>(value.Value));
  }

  template <>
  constexpr inline DpValue UncheckedTo<DpValue, DpSize1D>(const DpSize1D& value) noexcept
  {
    return UncheckedTo<DpValue>(value.Value());
  }

  template <>
  inline DpValue UncheckedChangeTo<DpValue, DpSize1DF>(const DpSize1DF& value)
  {
    return UncheckedChangeTo<DpValue>(value.Value());
  }


  // --- DpValueF

  // Basically a no-op
  template <>
  constexpr inline DpValueF UncheckedTo<DpValueF, DpValueF>(const DpValueF& value) noexcept
  {
    return value;
  }

  template <>
  constexpr inline DpValueF UncheckedTo<DpValueF, DpValue>(const DpValue& value) noexcept
  {
    return DpValueF(static_cast<float>(value.Value));
  }

  template <>
  constexpr inline DpValueF UncheckedTo<DpValueF, DpValueU>(const DpValueU& value) noexcept
  {
    return DpValueF(static_cast<float>(value.Value));
  }

  template <>
  constexpr inline DpValueF UncheckedTo<DpValueF, DpSize1D>(const DpSize1D& value) noexcept
  {
    return UncheckedTo<DpValueF>(value.Value());
  }


  template <>
  constexpr inline DpValueF UncheckedTo<DpValueF, DpSize1DF>(const DpSize1DF& value) noexcept
  {
    return UncheckedTo<DpValueF>(value.Value());
  }

  // --- DpValueU

  template <>
  constexpr inline DpValueU UncheckedTo<DpValueU, DpValue>(const DpValue& value) noexcept
  {
    return DpValueU(UncheckedNumericCast<DpValueU::value_type>(value.Value));
  }

  template <>
  inline DpValueU UncheckedChangeTo<DpValueU, DpValueF>(const DpValueF& value)
  {
    return DpValueU(UncheckedChangeTo<DpValueU::value_type>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline DpValueU UncheckedTo<DpValueU, DpValueU>(const DpValueU& value) noexcept
  {
    return value;
  }

  template <>
  constexpr inline DpValueU UncheckedTo<DpValueU, DpSize1D>(const DpSize1D& value) noexcept
  {
    return UncheckedTo<DpValueU>(value.Value());
  }

  template <>
  inline DpValueU UncheckedChangeTo<DpValueU, DpSize1DF>(const DpSize1DF& value)
  {
    return UncheckedChangeTo<DpValueU>(value.Value());
  }

  // --- DpSize1D

  template <>
  constexpr inline DpSize1D UncheckedTo<DpSize1D, DpValue>(const DpValue& value) noexcept
  {
    return DpSize1D(value);
  }

  template <>
  inline DpSize1D UncheckedChangeTo<DpSize1D, DpValueF>(const DpValueF& value)
  {
    return DpSize1D(UncheckedChangeTo<DpValue>(value));
  }

  template <>
  constexpr inline DpSize1D UncheckedTo<DpSize1D, DpValueU>(const DpValueU& value) noexcept
  {
    return DpSize1D(UncheckedTo<DpValue>(value));
  }

  template <>
  constexpr inline DpSize1D UncheckedTo<DpSize1D, DpSize1D>(const DpSize1D& value) noexcept
  {
    return value;
  }

  template <>
  inline DpSize1D UncheckedChangeTo<DpSize1D, DpSize1DF>(const DpSize1DF& value)
  {
    return UncheckedChangeTo<DpSize1D>(value.Value());
  }

  // --- DpSize1DF

  // Basically a no-op
  template <>
  constexpr inline DpSize1DF UncheckedTo<DpSize1DF, DpValue>(const DpValue& value) noexcept
  {
    return DpSize1DF(UncheckedTo<DpValueF>(value));
  }

  template <>
  constexpr inline DpSize1DF UncheckedTo<DpSize1DF, DpValueF>(const DpValueF& value) noexcept
  {
    return DpSize1DF(value);
  }

  template <>
  constexpr inline DpSize1DF UncheckedTo<DpSize1DF, DpValueU>(const DpValueU& value) noexcept
  {
    return DpSize1DF(UncheckedTo<DpValueF>(value));
  }

  template <>
  constexpr inline DpSize1DF UncheckedTo<DpSize1DF, DpSize1D>(const DpSize1D& value) noexcept
  {
    return UncheckedTo<DpSize1DF>(value.Value());
  }

  template <>
  constexpr inline DpSize1DF UncheckedTo<DpSize1DF, DpSize1DF>(const DpSize1DF& value) noexcept
  {
    return value;
  }

  // --- DpExtent2D

  template <>
  constexpr inline DpExtent2D UncheckedTo<DpExtent2D, DpPoint2>(const DpPoint2& value) noexcept
  {
    return {UncheckedTo<DpExtent2D::value_type>(value.X), UncheckedTo<DpExtent2D::value_type>(value.Y)};
  }

  template <>
  inline DpExtent2D UncheckedChangeTo<DpExtent2D, DpPoint2F>(const DpPoint2F& value)
  {
    return {UncheckedChangeTo<DpExtent2D::value_type>(value.X), UncheckedChangeTo<DpExtent2D::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpExtent2D UncheckedTo<DpExtent2D, DpSize2D>(const DpSize2D& value) noexcept
  {
    return {UncheckedTo<DpExtent2D::value_type>(value.Width()), UncheckedTo<DpExtent2D::value_type>(value.Height())};
  }

  template <>
  inline DpExtent2D UncheckedChangeTo<DpExtent2D, DpSize2DF>(const DpSize2DF& value)
  {
    return {UncheckedChangeTo<DpExtent2D::value_type>(value.Width()), UncheckedChangeTo<DpExtent2D::value_type>(value.Height())};
  }

  // --- DpPoint2

  template <>
  constexpr inline DpPoint2 UncheckedTo<DpPoint2, DpExtent2D>(const DpExtent2D& value) noexcept
  {
    return {UncheckedTo<DpPoint2::value_type>(value.Width), UncheckedTo<DpPoint2::value_type>(value.Height)};
  }

  template <>
  inline DpPoint2 UncheckedChangeTo<DpPoint2, DpPoint2F>(const DpPoint2F& value)
  {
    return {UncheckedChangeTo<DpPoint2::value_type>(value.X), UncheckedChangeTo<DpPoint2::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpPoint2 UncheckedTo<DpPoint2, DpSize2D>(const DpSize2D& value) noexcept
  {
    return {UncheckedTo<DpPoint2::value_type>(value.Width()), UncheckedTo<DpPoint2::value_type>(value.Height())};
  }

  // --- DpPoint2F

  template <>
  constexpr inline DpPoint2F UncheckedTo<DpPoint2F, DpExtent2D>(const DpExtent2D& value) noexcept
  {
    return {UncheckedTo<DpPoint2F::value_type>(value.Width), UncheckedTo<DpPoint2F::value_type>(value.Height)};
  }

  template <>
  constexpr inline DpPoint2F UncheckedTo<DpPoint2F, DpPoint2>(const DpPoint2& value) noexcept
  {
    return {UncheckedTo<DpPoint2F::value_type>(value.X), UncheckedTo<DpPoint2F::value_type>(value.Y)};
  }

  template <>
  constexpr inline DpPoint2F UncheckedTo<DpPoint2F, DpSize2D>(const DpSize2D& value) noexcept
  {
    return {UncheckedTo<DpPoint2F::value_type>(value.Width()), UncheckedTo<DpPoint2F::value_type>(value.Height())};
  }


  // --- DpSize2D

  template <>
  constexpr inline DpSize2D UncheckedTo<DpSize2D, DpExtent2D>(const DpExtent2D& value) noexcept
  {
    return {UncheckedTo<DpSize2D::value_type>(value.Width), UncheckedTo<DpSize2D::value_type>(value.Height)};
  }

  template <>
  constexpr inline DpSize2D UncheckedTo<DpSize2D, DpPoint2>(const DpPoint2& value) noexcept
  {
    return {UncheckedTo<DpSize2D::value_type>(value.X), UncheckedTo<DpSize2D::value_type>(value.Y)};
  }

  template <>
  inline DpSize2D UncheckedChangeTo<DpSize2D, DpPoint2F>(const DpPoint2F& value)
  {
    return {UncheckedChangeTo<DpSize2D::value_type>(value.X), UncheckedChangeTo<DpSize2D::value_type>(value.Y)};
  }

  // --- DpThicknessF

  template <>
  constexpr inline DpThicknessF UncheckedTo<DpThicknessF, DpThickness>(const DpThickness& value) noexcept
  {
    return {UncheckedTo<DpThicknessF::value_type>(value.Left()), UncheckedTo<DpThicknessF::value_type>(value.Top()),
            UncheckedTo<DpThicknessF::value_type>(value.Right()), UncheckedTo<DpThicknessF::value_type>(value.Bottom())};
  }

  // --- DpThicknessU

  template <>
  constexpr inline DpThicknessU UncheckedTo<DpThicknessU, DpThickness>(const DpThickness& value) noexcept
  {
    return {UncheckedTo<DpThicknessU::value_type>(value.Left()), UncheckedTo<DpThicknessU::value_type>(value.Top()),
            UncheckedTo<DpThicknessU::value_type>(value.Right()), UncheckedTo<DpThicknessU::value_type>(value.Bottom())};
  }

  template <>
  inline DpThicknessU UncheckedChangeTo<DpThicknessU, DpThicknessF>(const DpThicknessF& value)
  {
    return {UncheckedChangeTo<DpThicknessU::value_type>(value.Left()), UncheckedChangeTo<DpThicknessU::value_type>(value.Top()),
            UncheckedChangeTo<DpThicknessU::value_type>(value.Right()), UncheckedChangeTo<DpThicknessU::value_type>(value.Bottom())};
  }
}

#endif
