#ifndef FSLBASE_MATH_PIXEL_TYPECONVERTER_HPP
#define FSLBASE_MATH_PIXEL_TYPECONVERTER_HPP
/****************************************************************************************************************************************************
 * Copyright 2020 NXP
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
#include <FslBase/Math/Pixel/PxRectangleU.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxThickness.hpp>
#include <FslBase/Math/Pixel/PxThicknessF.hpp>
#include <FslBase/Math/Pixel/PxThicknessU.hpp>
#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Fsl
{
  namespace TypeConverter
  {
    // --- PxAreaRectangleF

    template <>
    constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangle2D>(const PxRectangle2D& value)
    {
      return {static_cast<PxAreaRectangleF::value_type>(value.Offset.X), static_cast<PxAreaRectangleF::value_type>(value.Offset.Y),
              static_cast<PxAreaRectangleF::value_type>(value.Extent.Width), static_cast<PxAreaRectangleF::value_type>(value.Extent.Height)};
    }

    template <>
    constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangleU>(const PxRectangleU& value)
    {
      return {static_cast<PxAreaRectangleF::value_type>(value.X), static_cast<PxAreaRectangleF::value_type>(value.Y),
              static_cast<PxAreaRectangleF::value_type>(value.Width), static_cast<PxAreaRectangleF::value_type>(value.Height)};
    }


    template <>
    constexpr inline PxAreaRectangleF To<PxAreaRectangleF, PxRectangle>(const PxRectangle& value)
    {
      return PxAreaRectangleF::FromLeftTopRightBottom(static_cast<float>(value.Left()), static_cast<float>(value.Top()),
                                                      static_cast<float>(value.Right()), static_cast<float>(value.Bottom()));
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

      return {value.Offset.X, value.Offset.Y, NumericCast<PxClipRectangle::value_type>(value.Extent.Width),
              NumericCast<PxClipRectangle::value_type>(value.Extent.Height)};
    }


    template <>
    constexpr inline PxClipRectangle To<PxClipRectangle, PxRectangleU>(const PxRectangleU& value)
    {
      return {NumericCast<PxClipRectangle::value_type>(value.X), NumericCast<PxClipRectangle::value_type>(value.Y),
              NumericCast<PxClipRectangle::value_type>(value.Width), NumericCast<PxClipRectangle::value_type>(value.Height)};
    }

    // --- PxExtent2D

    template <>
    constexpr inline PxExtent2D To<PxExtent2D, PxPoint2>(const PxPoint2& value)
    {
      return {NumericCast<PxExtent2D::value_type>(value.X), NumericCast<PxExtent2D::value_type>(value.Y)};
    }

    template <>
    constexpr inline PxExtent2D To<PxExtent2D, PxSize2D>(const PxSize2D& value)
    {
      return {NumericCast<PxExtent2D::value_type>(value.Width()), NumericCast<PxExtent2D::value_type>(value.Height())};
    }

    // --- PxPoint2

    template <>
    constexpr inline PxPoint2 To<PxPoint2, PxExtent2D>(const PxExtent2D& value)
    {
      return {NumericCast<PxPoint2::value_type>(value.Width), NumericCast<PxPoint2::value_type>(value.Height)};
    }

    template <>
    constexpr inline PxPoint2 To<PxPoint2, PxSize2D>(const PxSize2D& value)
    {
      static_assert(std::is_same<PxPoint2::value_type, PxSize2D::value_type>::value, "we expect the types to be equal");
      return {value.Width(), value.Height()};
    }

    // --- PxRectangle

    template <>
    constexpr inline PxRectangle To<PxRectangle, PxRectangleU>(const PxRectangleU& value)
    {
      return {NumericCast<PxRectangle::value_type>(value.X), NumericCast<PxRectangle::value_type>(value.Y),
              NumericCast<PxRectangle::value_type>(value.Width), NumericCast<PxRectangle::value_type>(value.Height)};
    }

    template <>
    constexpr inline PxRectangle To<PxRectangle, PxRectangle2D>(const PxRectangle2D& value)
    {
      static_assert(std::is_same<PxRectangle::value_type, PxRectangle2D::offset_type::value_type>::value, "we expect the types to be equal");
      return {value.Offset.X, value.Offset.Y, NumericCast<PxRectangle::value_type>(value.Extent.Width),
              NumericCast<PxRectangle::value_type>(value.Extent.Height)};
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
      return {NumericCast<PxSize2D::value_type>(value.Width), NumericCast<PxSize2D::value_type>(value.Height)};
    }

    // --- PxSize2DF

    template <>
    constexpr inline PxSize2DF To<PxSize2DF, PxSize2D>(const PxSize2D& value)
    {
      return {static_cast<PxSize2DF::value_type>(value.Width()), static_cast<PxSize2DF::value_type>(value.Height())};
    }

    template <>
    constexpr inline PxSize2DF To<PxSize2DF, PxExtent2D>(const PxExtent2D& value)
    {
      return {static_cast<PxSize2DF::value_type>(value.Width), static_cast<PxSize2DF::value_type>(value.Height)};
    }

    // --- PxThickness

    template <>
    constexpr inline PxThickness To<PxThickness, PxThicknessU>(const PxThicknessU& value)
    {
      return {NumericCast<PxThickness::value_type>(value.Left), NumericCast<PxThickness::value_type>(value.Top),
              NumericCast<PxThickness::value_type>(value.Right), NumericCast<PxThickness::value_type>(value.Bottom)};
    }

    // --- PxThicknessF

    template <>
    constexpr inline PxThicknessF To<PxThicknessF, PxThicknessU>(const PxThicknessU& value)
    {
      return {static_cast<PxThicknessF::value_type>(value.Left), static_cast<PxThicknessF::value_type>(value.Top),
              static_cast<PxThicknessF::value_type>(value.Right), static_cast<PxThicknessF::value_type>(value.Bottom)};
    }


    // --- PxThicknessU

    template <>
    constexpr inline PxThicknessU To<PxThicknessU, PxThickness>(const PxThickness& value)
    {
      return {NumericCast<PxThicknessU::value_type>(value.Left()), NumericCast<PxThicknessU::value_type>(value.Top()),
              NumericCast<PxThicknessU::value_type>(value.Right()), NumericCast<PxThicknessU::value_type>(value.Bottom())};
    }


    // --- PxVector2

    template <>
    constexpr inline PxVector2 To<PxVector2, PxPoint2>(const PxPoint2& value)
    {
      return {static_cast<float>(value.X), static_cast<float>(value.Y)};
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // --- PxInt16

    template <>
    inline int16_t ChangeTo<int16_t, float>(const float& value)
    {
      if (std::isinf(value) || std::isnan(value))
      {
        throw ConversionException("unsupported float value");
      }
      auto rounded = std::round(value);
      if (rounded < static_cast<float>(std::numeric_limits<int16_t>::min()) || rounded > static_cast<float>(std::numeric_limits<int16_t>::max()))
      {
        throw ConversionException("overflow");
      }
      return static_cast<int16_t>(rounded);
    }

    // --- PxInt32

    template <>
    inline int32_t ChangeTo<int32_t, float>(const float& value)
    {
      if (std::isinf(value) || std::isnan(value))
      {
        throw ConversionException("unsupported float value");
      }
      auto rounded = std::round(value);
      if (rounded < static_cast<float>(std::numeric_limits<int32_t>::min()) || rounded > static_cast<float>(std::numeric_limits<int32_t>::max()))
      {
        throw ConversionException("overflow");
      }
      return static_cast<int32_t>(rounded);
    }

    // --- PxUInt16

    template <>
    inline uint16_t ChangeTo<uint16_t, float>(const float& value)
    {
      if (std::isinf(value) || std::isnan(value))
      {
        throw ConversionException("unsupported float value");
      }
      auto rounded = std::round(value);
      if (rounded < static_cast<float>(std::numeric_limits<uint16_t>::min()) || rounded > static_cast<float>(std::numeric_limits<uint16_t>::max()))
      {
        throw ConversionException("overflow");
      }
      return static_cast<uint16_t>(rounded);
    }

    // --- PxUInt32

    template <>
    inline uint32_t ChangeTo<uint32_t, float>(const float& value)
    {
      if (std::isinf(value) || std::isnan(value))
      {
        throw ConversionException("unsupported float value");
      }
      auto rounded = std::round(value);
      if (rounded < static_cast<float>(std::numeric_limits<uint32_t>::min()) || rounded > static_cast<float>(std::numeric_limits<uint32_t>::max()))
      {
        throw ConversionException("overflow");
      }
      return static_cast<uint32_t>(rounded);
    }


    // -----------------------------------------------------------------------------------------------------------------------------------------------


    // --- PxInt16

    template <>
    inline int16_t UncheckedChangeTo<int16_t, float>(const float& value)
    {
      assert(!std::isinf(value));
      assert(!std::isnan(value));
      auto rounded = std::round(value);
      assert(rounded <= static_cast<float>(std::numeric_limits<int16_t>::max()));
      assert(rounded >= static_cast<float>(std::numeric_limits<int16_t>::min()));
      return static_cast<int16_t>(rounded);
    }

    // --- PxInt32

    template <>
    inline int32_t UncheckedChangeTo<int32_t, float>(const float& value)
    {
      assert(!std::isinf(value));
      assert(!std::isnan(value));
      auto rounded = std::round(value);
      assert(rounded <= static_cast<float>(std::numeric_limits<int32_t>::max()));
      assert(rounded >= static_cast<float>(std::numeric_limits<int32_t>::min()));
      return static_cast<int32_t>(rounded);
    }

    // --- PxUInt16

    template <>
    inline uint16_t UncheckedChangeTo<uint16_t, float>(const float& value)
    {
      assert(!std::isinf(value));
      assert(!std::isnan(value));
      auto rounded = std::round(value);
      assert(rounded <= static_cast<float>(std::numeric_limits<uint16_t>::max()));
      assert(rounded >= static_cast<float>(std::numeric_limits<uint16_t>::min()));
      return static_cast<uint16_t>(rounded);
    }

    // --- PxUInt32

    template <>
    inline uint32_t UncheckedChangeTo<uint32_t, float>(const float& value)
    {
      assert(!std::isinf(value));
      assert(!std::isnan(value));
      auto rounded = std::round(value);
      assert(rounded <= static_cast<float>(std::numeric_limits<uint32_t>::max()));
      assert(rounded >= static_cast<float>(std::numeric_limits<uint32_t>::min()));
      return static_cast<uint32_t>(rounded);
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // --- PxAreaRectangleF

    template <>
    constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangle2D>(const PxRectangle2D& value) noexcept
    {
      return {static_cast<PxAreaRectangleF::value_type>(value.Offset.X), static_cast<PxAreaRectangleF::value_type>(value.Offset.Y),
              static_cast<PxAreaRectangleF::value_type>(value.Extent.Width), static_cast<PxAreaRectangleF::value_type>(value.Extent.Height)};
    }

    template <>
    constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangleU>(const PxRectangleU& value) noexcept
    {
      return {static_cast<PxAreaRectangleF::value_type>(value.X), static_cast<PxAreaRectangleF::value_type>(value.Y),
              static_cast<PxAreaRectangleF::value_type>(value.Width), static_cast<PxAreaRectangleF::value_type>(value.Height)};
    }


    template <>
    constexpr inline PxAreaRectangleF UncheckedTo<PxAreaRectangleF, PxRectangle>(const PxRectangle& value) noexcept
    {
      return PxAreaRectangleF::FromLeftTopRightBottom(static_cast<float>(value.Left()), static_cast<float>(value.Top()),
                                                      static_cast<float>(value.Right()), static_cast<float>(value.Bottom()));
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

      return {value.Offset.X, value.Offset.Y, UncheckedNumericCast<PxClipRectangle::value_type>(value.Extent.Width),
              UncheckedNumericCast<PxClipRectangle::value_type>(value.Extent.Height)};
    }


    template <>
    constexpr inline PxClipRectangle UncheckedTo<PxClipRectangle, PxRectangleU>(const PxRectangleU& value) noexcept
    {
      return {UncheckedNumericCast<PxClipRectangle::value_type>(value.X), UncheckedNumericCast<PxClipRectangle::value_type>(value.Y),
              UncheckedNumericCast<PxClipRectangle::value_type>(value.Width), UncheckedNumericCast<PxClipRectangle::value_type>(value.Height)};
    }

    // --- PxExtent2D

    template <>
    constexpr inline PxExtent2D UncheckedTo<PxExtent2D, PxPoint2>(const PxPoint2& value) noexcept
    {
      return {UncheckedNumericCast<PxExtent2D::value_type>(value.X), UncheckedNumericCast<PxExtent2D::value_type>(value.Y)};
    }

    template <>
    constexpr inline PxExtent2D UncheckedTo<PxExtent2D, PxSize2D>(const PxSize2D& value) noexcept
    {
      return {UncheckedNumericCast<PxExtent2D::value_type>(value.Width()), UncheckedNumericCast<PxExtent2D::value_type>(value.Height())};
    }

    // --- PxPoint2

    template <>
    constexpr inline PxPoint2 UncheckedTo<PxPoint2, PxExtent2D>(const PxExtent2D& value) noexcept
    {
      return {UncheckedNumericCast<PxPoint2::value_type>(value.Width), UncheckedNumericCast<PxPoint2::value_type>(value.Height)};
    }

    template <>
    constexpr inline PxPoint2 UncheckedTo<PxPoint2, PxSize2D>(const PxSize2D& value) noexcept
    {
      static_assert(std::is_same<PxPoint2::value_type, PxSize2D::value_type>::value, "we expect the types to be equal");
      return {value.Width(), value.Height()};
    }

    // PxPoint2

    template <>
    inline PxPoint2 UncheckedChangeTo<PxPoint2, PxVector2>(const PxVector2& value)
    {
      return {UncheckedChangeTo<PxPoint2::value_type>(value.X), UncheckedChangeTo<PxPoint2::value_type>(value.Y)};
    }

    // --- PxRectangle

    template <>
    constexpr inline PxRectangle UncheckedTo<PxRectangle, PxRectangleU>(const PxRectangleU& value) noexcept
    {
      return {UncheckedNumericCast<PxRectangle::value_type>(value.X), UncheckedNumericCast<PxRectangle::value_type>(value.Y),
              UncheckedNumericCast<PxRectangle::value_type>(value.Width), UncheckedNumericCast<PxRectangle::value_type>(value.Height)};
    }

    template <>
    constexpr inline PxRectangle UncheckedTo<PxRectangle, PxRectangle2D>(const PxRectangle2D& value) noexcept
    {
      static_assert(std::is_same<PxRectangle::value_type, PxRectangle2D::offset_type::value_type>::value, "we expect the types to be equal");
      return {value.Offset.X, value.Offset.Y, UncheckedNumericCast<PxRectangle::value_type>(value.Extent.Width),
              UncheckedNumericCast<PxRectangle::value_type>(value.Extent.Height)};
    }

    //! @brief Convert a rect to a PxRectangle
    template <>
    inline PxRectangle UncheckedChangeTo<PxRectangle, PxAreaRectangleF>(const PxAreaRectangleF& value)
    {
      // This should always be the case for a Rect (so we only assert check it)
      assert(value.Left() <= value.Right() && value.Top() <= value.Bottom());
      return PxRectangle::FromLeftTopRightBottom(UncheckedChangeTo<PxRectangle::value_type>(value.Left()),
                                                 UncheckedChangeTo<PxRectangle::value_type>(value.Top()),
                                                 UncheckedChangeTo<PxRectangle::value_type>(value.Right()),
                                                 UncheckedChangeTo<PxRectangle::value_type>(value.Bottom()), OptimizationCheckFlag::NoCheck);
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
      return {UncheckedNumericCast<PxSize2D::value_type>(value.Width), UncheckedNumericCast<PxSize2D::value_type>(value.Height)};
    }

    template <>
    inline PxSize2D UncheckedChangeTo<PxSize2D, PxVector2>(const PxVector2& value)
    {
      return {UncheckedChangeTo<PxSize2D::value_type>(value.X), UncheckedChangeTo<PxSize2D::value_type>(value.Y)};
    }

    // --- PxThickness

    template <>
    constexpr inline PxThickness UncheckedTo<PxThickness, PxThicknessU>(const PxThicknessU& value) noexcept
    {
      return {UncheckedNumericCast<PxThickness::value_type>(value.Left), UncheckedNumericCast<PxThickness::value_type>(value.Top),
              UncheckedNumericCast<PxThickness::value_type>(value.Right), UncheckedNumericCast<PxThickness::value_type>(value.Bottom)};
    }

    // --- PxThicknessU

    template <>
    constexpr inline PxThicknessU UncheckedTo<PxThicknessU, PxThickness>(const PxThickness& value) noexcept
    {
      return {UncheckedNumericCast<PxThicknessU::value_type>(value.Left()), UncheckedNumericCast<PxThicknessU::value_type>(value.Top()),
              UncheckedNumericCast<PxThicknessU::value_type>(value.Right()), UncheckedNumericCast<PxThicknessU::value_type>(value.Bottom())};
    }

    // --- PxVector2

    template <>
    constexpr inline PxVector2 UncheckedTo<PxVector2, PxPoint2>(const PxPoint2& value) noexcept
    {
      return {static_cast<float>(value.X), static_cast<float>(value.Y)};
    }

  }
}

#endif
