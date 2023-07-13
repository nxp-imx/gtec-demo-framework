#ifndef FSLBASE_MATH_PIXEL_TYPECONVERTER_HPP
#define FSLBASE_MATH_PIXEL_TYPECONVERTER_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2022-2023 NXP
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

#include <FslBase/Math/Pixel/PxAreaRectangleF.hpp>
#include <FslBase/Math/Pixel/PxClipRectangle.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxRectangle.hpp>
#include <FslBase/Math/Pixel/PxRectangle2D.hpp>
#include <FslBase/Math/Pixel/PxRectangleU16.hpp>
#include <FslBase/Math/Pixel/PxRectangleU32.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxSize1DF.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxSize2DF.hpp>
#include <FslBase/Math/Pixel/PxThickness.hpp>
#include <FslBase/Math/Pixel/PxThicknessF.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/Math/Pixel/PxValueF.hpp>
#include <FslBase/Math/Pixel/PxValueU.hpp>
#include <FslBase/Math/TypeConverter_Float.hpp>
#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Fsl::TypeConverter
{
  // The in this file are responsible for converting between various PX types

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // To/ChangeTo
  // -------------------------------------------------------------------------------------------------------------------------------------------------


  // --- PxValue

  // Basically a no-op
  template <>
  constexpr inline PxValue To<PxValue, PxValue>(const PxValue& value)
  {
    return value;
  }

  template <>
  constexpr inline PxValue To<PxValue, PxSize1D>(const PxSize1D& value)
  {
    return PxValue::Create(value.Value());
  }

  template <>
  inline PxValue ChangeTo<PxValue, PxValueF>(const PxValueF& value)
  {
    return PxValue(ChangeTo<PxValue::value_type>(value.Value));
  }

  template <>
  constexpr inline PxValue To<PxValue, PxValueU>(const PxValueU& value)
  {
    return PxValue(NumericCast<PxValue::value_type>(value.Value));
  }

  template <>
  constexpr inline PxValue To<PxValue, PxValueU16>(const PxValueU16& value)
  {
    return PxValue(NumericCast<PxValue::value_type>(value.Value));
  }

  // --- PxValueF

  template <>
  constexpr inline PxValueF To<PxValueF, PxValue>(const PxValue& value)
  {
    return PxValueF(static_cast<float>(value.Value));
  }

  template <>
  constexpr inline PxValueF To<PxValueF, PxValueU>(const PxValueU& value)
  {
    return PxValueF(static_cast<float>(value.Value));
  }

  template <>
  constexpr inline PxValueF To<PxValueF, PxValueU16>(const PxValueU16& value)
  {
    return PxValueF(static_cast<float>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline PxValueF To<PxValueF, PxValueF>(const PxValueF& value)
  {
    return value;
  }

  template <>
  constexpr inline PxValueF To<PxValueF, PxSize1D>(const PxSize1D& value)
  {
    return PxValueF(static_cast<float>(value.RawValue()));
  }


  // --- PxValueU

  template <>
  constexpr inline PxValueU To<PxValueU, PxValue>(const PxValue& value)
  {
    return PxValueU(NumericCast<PxValueU::value_type>(value.Value));
  }

  template <>
  inline PxValueU ChangeTo<PxValueU, PxValueF>(const PxValueF& value)
  {
    return PxValueU(ChangeTo<PxValueU::value_type>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline PxValueU To<PxValueU, PxValueU>(const PxValueU& value)
  {
    return value;
  }

  template <>
  constexpr inline PxValueU To<PxValueU, PxValueU16>(const PxValueU16& value)
  {
    return value;
  }

  template <>
  constexpr inline PxValueU To<PxValueU, PxSize1D>(const PxSize1D& value)
  {
    return PxValueU(NumericCast<PxValueU::value_type>(value.RawValue()));
  }

  // --- PxValueU16

  template <>
  constexpr inline PxValueU16 To<PxValueU16, PxValue>(const PxValue& value)
  {
    return PxValueU16(NumericCast<PxValueU16::value_type>(value.Value));
  }

  template <>
  inline PxValueU16 ChangeTo<PxValueU16, PxValueF>(const PxValueF& value)
  {
    return PxValueU16(ChangeTo<PxValueU16::value_type>(value.Value));
  }


  // Basically a no-op
  template <>
  constexpr inline PxValueU16 To<PxValueU16, PxValueU16>(const PxValueU16& value)
  {
    return value;
  }

  template <>
  constexpr inline PxValueU16 To<PxValueU16, PxValueU>(const PxValueU& value)
  {
    return PxValueU16(NumericCast<PxValueU16::value_type>(value.Value));
  }

  // --- PxSize1D

  template <>
  constexpr inline PxSize1D To<PxSize1D, PxValue>(const PxValue& value)
  {
    static_assert(std::is_same<PxSize1D::raw_value_type, PxValue::raw_value_type>::value, "we expect the types to be equal");
    return PxSize1D::Create(static_cast<PxSize1D::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1D To<PxSize1D, PxValueU>(const PxValueU& value)
  {
    return PxSize1D::Create(UncheckedNumericCast<PxSize1D::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1D To<PxSize1D, PxValueU16>(const PxValueU16& value)
  {
    return PxSize1D::Create(UncheckedNumericCast<PxSize1D::raw_value_type>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline PxSize1D To<PxSize1D, PxSize1D>(const PxSize1D& value)
  {
    return value;
  }


  // --- PxSize1DF

  template <>
  constexpr inline PxSize1DF To<PxSize1DF, PxValue>(const PxValue& value)
  {
    return PxSize1DF::Create(static_cast<PxSize1DF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1DF To<PxSize1DF, PxValueU>(const PxValueU& value)
  {
    return PxSize1DF::Create(static_cast<PxSize1DF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1DF To<PxSize1DF, PxValueU16>(const PxValueU16& value)
  {
    return PxSize1DF::Create(static_cast<PxSize1DF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1DF To<PxSize1DF, PxSize1D>(const PxSize1D& value)
  {
    return PxSize1DF::UncheckedCreate(static_cast<PxSize1DF::raw_value_type>(value.Value().Value));
  }

  // --- PxAreaRectangleF

  template <>
  constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangle2D>(const PxRectangle2D& value)
  {
    return {To<PxAreaRectangleF::value_type>(value.Offset.X), To<PxAreaRectangleF::value_type>(value.Offset.Y),
            To<PxAreaRectangleF::size_value_type>(value.Extent.Width), To<PxAreaRectangleF::size_value_type>(value.Extent.Height)};
  }

  template <>
  constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangleU16>(const PxRectangleU16& value)
  {
    return {To<PxAreaRectangleF::value_type>(value.X), To<PxAreaRectangleF::value_type>(value.Y), To<PxAreaRectangleF::size_value_type>(value.Width),
            To<PxAreaRectangleF::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangleU32>(const PxRectangleU32& value)
  {
    return {To<PxAreaRectangleF::value_type>(value.X), To<PxAreaRectangleF::value_type>(value.Y), To<PxAreaRectangleF::size_value_type>(value.Width),
            To<PxAreaRectangleF::size_value_type>(value.Height)};
  }


  template <>
  constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangle>(const PxRectangle& value)
  {
    return PxAreaRectangleF::FromLeftTopRightBottom(To<PxAreaRectangleF::value_type>(value.Left()), To<PxAreaRectangleF::value_type>(value.Top()),
                                                    To<PxAreaRectangleF::value_type>(value.Right()),
                                                    To<PxAreaRectangleF::value_type>(value.Bottom()));
  }

  // --- PxClipRectangle

  template <>
  constexpr inline PxClipRectangle To<PxClipRectangle, PxRectangle>(const PxRectangle& value)
  {
    return {value.X(), value.Y(), value.Width(), value.Height()};
  }

  template <>
  constexpr inline PxClipRectangle To<PxClipRectangle, PxRectangle2D>(const PxRectangle2D& value)
  {
    static_assert(std::is_same<PxClipRectangle::value_type, PxRectangle2D::offset_type::value_type>::value, "we expect the types to be equal");

    return {value.Offset.X, value.Offset.Y, To<PxClipRectangle::size_value_type>(value.Extent.Width),
            To<PxClipRectangle::size_value_type>(value.Extent.Height)};
  }


  template <>
  constexpr inline PxClipRectangle To<PxClipRectangle, PxRectangleU16>(const PxRectangleU16& value)
  {
    return {To<PxClipRectangle::value_type>(value.X), To<PxClipRectangle::value_type>(value.Y), To<PxClipRectangle::size_value_type>(value.Width),
            To<PxClipRectangle::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxClipRectangle To<PxClipRectangle, PxRectangleU32>(const PxRectangleU32& value)
  {
    return {To<PxClipRectangle::value_type>(value.X), To<PxClipRectangle::value_type>(value.Y), To<PxClipRectangle::size_value_type>(value.Width),
            To<PxClipRectangle::size_value_type>(value.Height)};
  }

  // --- PxExtent2D

  template <>
  constexpr inline PxExtent2D To<PxExtent2D, PxPoint2>(const PxPoint2& value)
  {
    return {To<PxExtent2D::value_type>(value.X), To<PxExtent2D::value_type>(value.Y)};
  }

  template <>
  constexpr inline PxExtent2D To<PxExtent2D, PxSize2D>(const PxSize2D& value)
  {
    return {To<PxExtent2D::value_type>(value.Width()), To<PxExtent2D::value_type>(value.Height())};
  }

  // --- PxPoint2

  template <>
  constexpr inline PxPoint2 To<PxPoint2, PxExtent2D>(const PxExtent2D& value)
  {
    return {To<PxPoint2::value_type>(value.Width), To<PxPoint2::value_type>(value.Height)};
  }

  template <>
  constexpr inline PxPoint2 To<PxPoint2, PxSize2D>(const PxSize2D& value)
  {
    return {To<PxValue>(value.Width()), To<PxValue>(value.Height())};
  }

  // --- PxRectangle

  template <>
  constexpr inline PxRectangle To<PxRectangle, PxRectangleU16>(const PxRectangleU16& value)
  {
    return {To<PxRectangle::value_type>(value.X), To<PxRectangle::value_type>(value.Y), To<PxRectangle::size_value_type>(value.Width),
            To<PxRectangle::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxRectangle To<PxRectangle, PxRectangleU32>(const PxRectangleU32& value)
  {
    return {To<PxRectangle::value_type>(value.X), To<PxRectangle::value_type>(value.Y), To<PxRectangle::size_value_type>(value.Width),
            To<PxRectangle::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxRectangle To<PxRectangle, PxRectangle2D>(const PxRectangle2D& value)
  {
    static_assert(std::is_same<PxRectangle::value_type, PxRectangle2D::offset_type::value_type>::value, "we expect the types to be equal");
    return {value.Offset.X, value.Offset.Y, To<PxRectangle::size_value_type>(value.Extent.Width),
            To<PxRectangle::size_value_type>(value.Extent.Height)};
  }

  // --- PxRectangleU16

  template <>
  constexpr inline PxRectangleU16 To<PxRectangleU16, PxRectangleU32>(const PxRectangleU32& value)
  {
    return {To<PxRectangleU16::value_type>(value.X), To<PxRectangleU16::value_type>(value.Y), To<PxRectangleU16::value_type>(value.Width),
            To<PxRectangleU16::value_type>(value.Height)};
  }

  // --- PxRectangleU32

  template <>
  constexpr inline PxRectangleU32 To<PxRectangleU32, PxRectangleU16>(const PxRectangleU16& value)
  {
    return {To<PxRectangleU32::value_type>(value.X), To<PxRectangleU32::value_type>(value.Y), To<PxRectangleU32::value_type>(value.Width),
            To<PxRectangleU32::value_type>(value.Height)};
  }

  // --- PxSize2D

  template <>
  constexpr inline PxSize2D To<PxSize2D, PxPoint2>(const PxPoint2& value)
  {
    static_assert(sizeof(PxSize2D::value_type) == sizeof(PxPoint2::value_type), "The sizes of these types should match");
    return {value.X, value.Y};
  }

  template <>
  constexpr inline PxSize2D To<PxSize2D, PxExtent2D>(const PxExtent2D& value)
  {
    return {To<PxSize2D::value_type>(value.Width), To<PxSize2D::value_type>(value.Height)};
  }

  // --- PxSize2DF

  template <>
  constexpr inline PxSize2DF To<PxSize2DF, PxSize2D>(const PxSize2D& value)
  {
    return {To<PxSize1DF>(value.Width()), To<PxSize1DF>(value.Height())};
  }

  template <>
  constexpr inline PxSize2DF To<PxSize2DF, PxExtent2D>(const PxExtent2D& value)
  {
    return {To<PxSize1DF>(value.Width), To<PxSize1DF>(value.Height)};
  }

  // --- PxThickness

  template <>
  constexpr inline PxThickness To<PxThickness, PxThicknessU>(const PxThicknessU& value)
  {
    return {To<PxThickness::value_type>(value.Left), To<PxThickness::value_type>(value.Top), To<PxThickness::value_type>(value.Right),
            To<PxThickness::value_type>(value.Bottom)};
  }

  // --- PxThicknessF

  template <>
  constexpr inline PxThicknessF To<PxThicknessF, PxThicknessU>(const PxThicknessU& value)
  {
    return {To<PxThicknessF::value_type>(value.Left), To<PxThicknessF::value_type>(value.Top), To<PxThicknessF::value_type>(value.Right),
            To<PxThicknessF::value_type>(value.Bottom)};
  }

  // --- PxThicknessU

  template <>
  constexpr inline PxThicknessU To<PxThicknessU, PxThickness>(const PxThickness& value)
  {
    return {To<PxThicknessU::value_type>(value.Left()), To<PxThicknessU::value_type>(value.Top()), To<PxThicknessU::value_type>(value.Right()),
            To<PxThicknessU::value_type>(value.Bottom())};
  }

  // --- PxVector2

  template <>
  constexpr inline PxVector2 To<PxVector2, PxPoint2>(const PxPoint2& value)
  {
    return {To<PxVector2::value_type>(value.X), To<PxVector2::value_type>(value.Y)};
  }

  // -------------------------------------------------------------------------------------------------------------------------------------------------
  // UncheckedTo
  // -------------------------------------------------------------------------------------------------------------------------------------------------

  // --- PxValue

  // Basically a no-op
  template <>
  constexpr inline PxValue UncheckedTo<PxValue, PxValue>(const PxValue& value) noexcept
  {
    return value;
  }

  template <>
  constexpr inline PxValue UncheckedTo<PxValue, PxSize1D>(const PxSize1D& value) noexcept
  {
    return PxValue::Create(value.RawValue());
  }

  template <>
  inline PxValue UncheckedChangeTo<PxValue, PxValueF>(const PxValueF& value)
  {
    return PxValue(UncheckedChangeTo<PxValue::value_type>(value.Value));
  }

  template <>
  constexpr inline PxValue UncheckedTo<PxValue, PxValueU>(const PxValueU& value) noexcept
  {
    return PxValue(UncheckedNumericCast<PxValue::value_type>(value.Value));
  }

  template <>
  constexpr inline PxValue UncheckedTo<PxValue, PxValueU16>(const PxValueU16& value) noexcept
  {
    return PxValue(UncheckedNumericCast<PxValue::value_type>(value.Value));
  }

  template <>
  inline PxValue UncheckedChangeTo<PxValue, PxSize1DF>(const PxSize1DF& value)
  {
    return UncheckedChangeTo<PxValue>(value.Value());
  }

  // --- PxValueF

  // Basically a no-op
  template <>
  constexpr inline PxValueF UncheckedTo<PxValueF, PxValueF>(const PxValueF& value) noexcept
  {
    return value;
  }

  template <>
  constexpr inline PxValueF UncheckedTo<PxValueF, PxValue>(const PxValue& value) noexcept
  {
    return PxValueF(static_cast<PxValueF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxValueF UncheckedTo<PxValueF, PxValueU>(const PxValueU& value) noexcept
  {
    return PxValueF(static_cast<PxValueF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxValueF UncheckedTo<PxValueF, PxValueU16>(const PxValueU16& value) noexcept
  {
    return PxValueF(static_cast<PxValueF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxValueF UncheckedTo<PxValueF, PxSize1DF>(const PxSize1DF& value) noexcept
  {
    return value.Value();
  }

  // --- PxValueU

  template <>
  constexpr inline PxValueU UncheckedTo<PxValueU, PxValue>(const PxValue& value) noexcept
  {
    return PxValueU(UncheckedNumericCast<PxValueU::value_type>(value.Value));
  }

  template <>
  inline PxValueU UncheckedChangeTo<PxValueU, PxValueF>(const PxValueF& value)
  {
    return PxValueU(UncheckedChangeTo<PxValueU::value_type>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline PxValueU UncheckedTo<PxValueU, PxValueU>(const PxValueU& value) noexcept
  {
    return value;
  }

  template <>
  constexpr inline PxValueU UncheckedTo<PxValueU, PxSize1D>(const PxSize1D& value) noexcept
  {
    return PxValueU(UncheckedNumericCast<PxValueU::value_type>(value.RawValue()));
  }

  template <>
  inline PxValueU UncheckedChangeTo<PxValueU, PxSize1DF>(const PxSize1DF& value)
  {
    return UncheckedChangeTo<PxValueU>(value.Value());
  }


  // --- PxValueU16

  template <>
  constexpr inline PxValueU16 UncheckedTo<PxValueU16, PxValue>(const PxValue& value) noexcept
  {
    return PxValueU16(UncheckedNumericCast<PxValueU16::value_type>(value.Value));
  }

  template <>
  constexpr inline PxValueU16 UncheckedTo<PxValueU16, PxValueU>(const PxValueU& value) noexcept
  {
    return PxValueU16(UncheckedNumericCast<PxValueU16::value_type>(value.Value));
  }

  template <>
  inline PxValueU16 UncheckedChangeTo<PxValueU16, PxValueF>(const PxValueF& value)
  {
    return PxValueU16(UncheckedChangeTo<PxValueU16::value_type>(value.Value));
  }

  // Basically a no-op
  template <>
  constexpr inline PxValueU16 UncheckedTo<PxValueU16, PxValueU16>(const PxValueU16& value) noexcept
  {
    return value;
  }

  template <>
  constexpr inline PxValueU16 UncheckedTo<PxValueU16, PxSize1D>(const PxSize1D& value) noexcept
  {
    return PxValueU16(UncheckedNumericCast<PxValueU16::value_type>(value.RawValue()));
  }

  // --- PxSize1D

  template <>
  constexpr inline PxSize1D UncheckedTo<PxSize1D, PxValueU>(const PxValueU& value) noexcept
  {
    return PxSize1D::UncheckedCreate(UncheckedNumericCast<PxSize1D::value_type::value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1D UncheckedTo<PxSize1D, PxValueU16>(const PxValueU16& value) noexcept
  {
    return PxSize1D::UncheckedCreate(UncheckedNumericCast<PxSize1D::value_type::value_type>(value.Value));
  }

  template <>
  inline PxSize1D UncheckedChangeTo<PxSize1D, PxValueF>(const PxValueF& value)
  {
    return PxSize1D::Create(UncheckedChangeTo<PxSize1D::value_type::value_type>(value.Value));
  }

  template <>
  inline PxSize1D UncheckedChangeTo<PxSize1D, PxSize1DF>(const PxSize1DF& value)
  {
    return PxSize1D::UncheckedCreate(UncheckedChangeTo<PxSize1D::value_type::value_type>(value.Value().Value));
  }

  // --- PxSize1DF

  template <>
  constexpr inline PxSize1DF UncheckedTo<PxSize1DF, PxValueU>(const PxValueU& value) noexcept
  {
    return PxSize1DF::UncheckedCreate(static_cast<PxSize1DF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1DF UncheckedTo<PxSize1DF, PxValueU16>(const PxValueU16& value) noexcept
  {
    return PxSize1DF::UncheckedCreate(static_cast<PxSize1DF::raw_value_type>(value.Value));
  }

  template <>
  constexpr inline PxSize1DF UncheckedTo<PxSize1DF, PxSize1D>(const PxSize1D& value) noexcept
  {
    return PxSize1DF::UncheckedCreate(static_cast<PxSize1DF::raw_value_type>(value.RawValue()));
  }

  template <>
  constexpr inline PxSize1DF UncheckedTo<PxSize1DF, PxValue>(const PxValue& value) noexcept
  {
    return PxSize1DF(UncheckedTo<PxSize1DF::value_type>(value));
  }


  template <>
  constexpr inline PxSize1DF UncheckedTo<PxSize1DF, PxValueF>(const PxValueF& value) noexcept
  {
    return PxSize1DF(value);
  }

  template <>
  constexpr inline PxSize1DF UncheckedTo<PxSize1DF, PxSize1DF>(const PxSize1DF& value) noexcept
  {
    return value;
  }

  // --- PxAreaRectangleF

  template <>
  constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangle2D>(const PxRectangle2D& value) noexcept
  {
    return {UncheckedTo<PxAreaRectangleF::value_type>(value.Offset.X), UncheckedTo<PxAreaRectangleF::value_type>(value.Offset.Y),
            UncheckedTo<PxAreaRectangleF::size_value_type>(value.Extent.Width), UncheckedTo<PxAreaRectangleF::size_value_type>(value.Extent.Height)};
  }

  template <>
  constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangleU16>(const PxRectangleU16& value) noexcept
  {
    return {UncheckedTo<PxAreaRectangleF::value_type>(value.X), UncheckedTo<PxAreaRectangleF::value_type>(value.Y),
            UncheckedTo<PxAreaRectangleF::size_value_type>(value.Width), UncheckedTo<PxAreaRectangleF::size_value_type>(value.Height)};
  }


  template <>
  constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangleU32>(const PxRectangleU32& value) noexcept
  {
    return {UncheckedTo<PxAreaRectangleF::value_type>(value.X), UncheckedTo<PxAreaRectangleF::value_type>(value.Y),
            UncheckedTo<PxAreaRectangleF::size_value_type>(value.Width), UncheckedTo<PxAreaRectangleF::size_value_type>(value.Height)};
  }


  template <>
  constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangle>(const PxRectangle& value) noexcept
  {
    return PxAreaRectangleF::FromLeftTopRightBottom(
      UncheckedTo<PxAreaRectangleF::value_type>(value.Left()), UncheckedTo<PxAreaRectangleF::value_type>(value.Top()),
      UncheckedTo<PxAreaRectangleF::value_type>(value.Right()), UncheckedTo<PxAreaRectangleF::value_type>(value.Bottom()));
  }

  // --- PxClipRectangle

  template <>
  constexpr inline PxClipRectangle UncheckedTo<PxClipRectangle, PxRectangle>(const PxRectangle& value) noexcept
  {
    return {value.X(), value.Y(), value.Width(), value.Height()};
  }

  template <>
  constexpr inline PxClipRectangle UncheckedTo<PxClipRectangle, PxRectangle2D>(const PxRectangle2D& value) noexcept
  {
    static_assert(std::is_same<PxClipRectangle::value_type, PxRectangle2D::offset_type::value_type>::value, "we expect the types to be equal");

    return {value.Offset.X, value.Offset.Y, UncheckedTo<PxClipRectangle::size_value_type>(value.Extent.Width),
            UncheckedTo<PxClipRectangle::size_value_type>(value.Extent.Height)};
  }


  template <>
  constexpr inline PxClipRectangle UncheckedTo<PxClipRectangle, PxRectangleU16>(const PxRectangleU16& value) noexcept
  {
    return {UncheckedTo<PxClipRectangle::value_type>(value.X), UncheckedTo<PxClipRectangle::value_type>(value.Y),
            UncheckedTo<PxClipRectangle::size_value_type>(value.Width), UncheckedTo<PxClipRectangle::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxClipRectangle UncheckedTo<PxClipRectangle, PxRectangleU32>(const PxRectangleU32& value) noexcept
  {
    return {UncheckedTo<PxClipRectangle::value_type>(value.X), UncheckedTo<PxClipRectangle::value_type>(value.Y),
            UncheckedTo<PxClipRectangle::size_value_type>(value.Width), UncheckedTo<PxClipRectangle::size_value_type>(value.Height)};
  }

  // --- PxExtent2D

  template <>
  constexpr inline PxExtent2D UncheckedTo<PxExtent2D, PxPoint2>(const PxPoint2& value) noexcept
  {
    return {UncheckedTo<PxExtent2D::value_type>(value.X), UncheckedTo<PxExtent2D::value_type>(value.Y)};
  }

  template <>
  constexpr inline PxExtent2D UncheckedTo<PxExtent2D, PxSize2D>(const PxSize2D& value) noexcept
  {
    return {UncheckedTo<PxExtent2D::value_type>(value.Width()), UncheckedTo<PxExtent2D::value_type>(value.Height())};
  }

  template <>
  inline PxExtent2D UncheckedChangeTo<PxExtent2D, PxVector2>(const PxVector2& value)
  {
    return {UncheckedChangeTo<PxExtent2D::value_type>(value.X), UncheckedChangeTo<PxExtent2D::value_type>(value.Y)};
  }

  // --- PxPoint2

  template <>
  constexpr inline PxPoint2 UncheckedTo<PxPoint2, PxExtent2D>(const PxExtent2D& value) noexcept
  {
    return {UncheckedTo<PxPoint2::value_type>(value.Width), UncheckedTo<PxPoint2::value_type>(value.Height)};
  }

  template <>
  constexpr inline PxPoint2 UncheckedTo<PxPoint2, PxSize2D>(const PxSize2D& value) noexcept
  {
    return {To<PxValue>(value.Width()), To<PxValue>(value.Height())};
  }

  template <>
  inline PxPoint2 UncheckedChangeTo<PxPoint2, PxVector2>(const PxVector2& value)
  {
    return {UncheckedChangeTo<PxPoint2::value_type>(value.X), UncheckedChangeTo<PxPoint2::value_type>(value.Y)};
  }

  // --- PxRectangle

  template <>
  constexpr inline PxRectangle UncheckedTo<PxRectangle, PxRectangleU16>(const PxRectangleU16& value) noexcept
  {
    return {UncheckedTo<PxRectangle::value_type>(value.X), UncheckedTo<PxRectangle::value_type>(value.Y),
            UncheckedTo<PxRectangle::size_value_type>(value.Width), UncheckedTo<PxRectangle::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxRectangle UncheckedTo<PxRectangle, PxRectangleU32>(const PxRectangleU32& value) noexcept
  {
    return {UncheckedTo<PxRectangle::value_type>(value.X), UncheckedTo<PxRectangle::value_type>(value.Y),
            UncheckedTo<PxRectangle::size_value_type>(value.Width), UncheckedTo<PxRectangle::size_value_type>(value.Height)};
  }

  template <>
  constexpr inline PxRectangle UncheckedTo<PxRectangle, PxRectangle2D>(const PxRectangle2D& value) noexcept
  {
    static_assert(std::is_same<PxRectangle::value_type, PxRectangle2D::offset_type::value_type>::value, "we expect the types to be equal");
    return {value.Offset.X, value.Offset.Y, UncheckedTo<PxRectangle::size_value_type>(value.Extent.Width),
            UncheckedTo<PxRectangle::size_value_type>(value.Extent.Height)};
  }

  //! @brief Convert a rect to a PxRectangle
  template <>
  inline PxRectangle UncheckedChangeTo<PxRectangle, PxAreaRectangleF>(const PxAreaRectangleF& value)
  {
    // This should always be the case for a Rect (so we only assert check it)
    assert(value.Left() <= value.Right() && value.Top() <= value.Bottom());
    return PxRectangle::UncheckedFromLeftTopRightBottom(
      UncheckedChangeTo<PxRectangle::value_type>(value.Left()), UncheckedChangeTo<PxRectangle::value_type>(value.Top()),
      UncheckedChangeTo<PxRectangle::value_type>(value.Right()), UncheckedChangeTo<PxRectangle::value_type>(value.Bottom()));
  }

  // --- PxSize2D

  template <>
  constexpr inline PxSize2D UncheckedTo<PxSize2D, PxPoint2>(const PxPoint2& value) noexcept
  {
    static_assert(sizeof(PxSize2D::value_type) == sizeof(PxPoint2::value_type), "The sizes of these types should match");
    return {value.X, value.Y};
  }

  template <>
  constexpr inline PxSize2D UncheckedTo<PxSize2D, PxExtent2D>(const PxExtent2D& value) noexcept
  {
    return {UncheckedTo<PxSize2D::value_type>(value.Width), UncheckedTo<PxSize2D::value_type>(value.Height)};
  }

  template <>
  inline PxSize2D UncheckedChangeTo<PxSize2D, PxVector2>(const PxVector2& value)
  {
    return {UncheckedChangeTo<PxSize2D::value_type>(value.X), UncheckedChangeTo<PxSize2D::value_type>(value.Y)};
  }

  template <>
  inline PxSize2D UncheckedChangeTo<PxSize2D, PxSize2DF>(const PxSize2DF& value)
  {
    return {UncheckedChangeTo<PxSize2D::value_type>(value.Width()), UncheckedChangeTo<PxSize2D::value_type>(value.Height())};
  }


  // --- PxSize2DF

  template <>
  constexpr inline PxSize2DF UncheckedTo<PxSize2DF, PxPoint2>(const PxPoint2& value) noexcept
  {
    return {UncheckedTo<PxSize2DF::value_type>(value.X), UncheckedTo<PxSize2DF::value_type>(value.Y)};
  }

  template <>
  constexpr inline PxSize2DF UncheckedTo<PxSize2DF, PxSize2D>(const PxSize2D& value) noexcept
  {
    return {UncheckedTo<PxSize2DF::value_type>(value.Width()), UncheckedTo<PxSize2DF::value_type>(value.Height())};
  }

  template <>
  constexpr inline PxSize2DF UncheckedTo<PxSize2DF, PxVector2>(const PxVector2& value) noexcept
  {
    return {UncheckedTo<PxSize2DF::value_type>(value.X), UncheckedTo<PxSize2DF::value_type>(value.Y)};
  }


  // --- PxThickness

  template <>
  constexpr inline PxThickness UncheckedTo<PxThickness, PxThicknessU>(const PxThicknessU& value) noexcept
  {
    return {UncheckedTo<PxThickness::value_type>(value.Left), UncheckedTo<PxThickness::value_type>(value.Top),
            UncheckedTo<PxThickness::value_type>(value.Right), UncheckedTo<PxThickness::value_type>(value.Bottom)};
  }

  template <>
  inline PxThickness UncheckedChangeTo<PxThickness, PxThicknessF>(const PxThicknessF& value)
  {
    return {UncheckedChangeTo<PxThickness::value_type>(value.Left()), UncheckedChangeTo<PxThickness::value_type>(value.Top()),
            UncheckedChangeTo<PxThickness::value_type>(value.Right()), UncheckedChangeTo<PxThickness::value_type>(value.Bottom())};
  }

  // --- PxThicknessU

  template <>
  constexpr inline PxThicknessU UncheckedTo<PxThicknessU, PxThickness>(const PxThickness& value) noexcept
  {
    return {UncheckedTo<PxThicknessU::value_type>(value.Left()), UncheckedTo<PxThicknessU::value_type>(value.Top()),
            UncheckedTo<PxThicknessU::value_type>(value.Right()), UncheckedTo<PxThicknessU::value_type>(value.Bottom())};
  }

  template <>
  inline PxThicknessU UncheckedChangeTo<PxThicknessU, PxThicknessF>(const PxThicknessF& value)
  {
    return {UncheckedChangeTo<PxThicknessU::value_type>(value.Left()), UncheckedChangeTo<PxThicknessU::value_type>(value.Top()),
            UncheckedChangeTo<PxThicknessU::value_type>(value.Right()), UncheckedChangeTo<PxThicknessU::value_type>(value.Bottom())};
  }

  // --- PxVector2

  template <>
  constexpr inline PxVector2 UncheckedTo<PxVector2, PxPoint2>(const PxPoint2& value) noexcept
  {
    return {UncheckedTo<PxVector2::value_type>(value.X), UncheckedTo<PxVector2::value_type>(value.Y)};
  }

}

#endif
