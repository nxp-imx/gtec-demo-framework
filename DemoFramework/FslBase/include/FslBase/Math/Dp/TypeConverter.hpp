#ifndef FSLBASE_MATH_DP_TYPECONVERTER_HPP
#define FSLBASE_MATH_DP_TYPECONVERTER_HPP
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

#include <FslBase/Math/Dp/DpExtent.hpp>
#include <FslBase/Math/Dp/DpPoint.hpp>
#include <FslBase/Math/Dp/DpSize.hpp>
#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <cmath>
#include <limits>
#include <type_traits>

namespace Fsl
{
  namespace TypeConverter
  {
    // --- DpExtent

    template <>
    constexpr inline DpExtent To<DpExtent, DpPoint>(const DpPoint& value)
    {
      return {NumericCast<DpExtent::value_type>(value.X), NumericCast<DpExtent::value_type>(value.Y)};
    }

    template <>
    constexpr inline DpExtent To<DpExtent, DpSize>(const DpSize& value)
    {
      return {NumericCast<DpExtent::value_type>(value.Width()), NumericCast<DpExtent::value_type>(value.Height())};
    }

    // --- DpPoint

    template <>
    constexpr inline DpPoint To<DpPoint, DpExtent>(const DpExtent& value)
    {
      return {NumericCast<DpPoint::value_type>(value.Width), NumericCast<DpPoint::value_type>(value.Height)};
    }

    template <>
    constexpr inline DpPoint To<DpPoint, DpSize>(const DpSize& value)
    {
      static_assert(std::is_same<DpPoint::value_type, DpSize::value_type>::value, "we expect the types to be equal");
      return {value.Width(), value.Height()};
    }

    // --- DpSize

    template <>
    constexpr inline DpSize To<DpSize, DpPoint>(const DpPoint& value)
    {
      static_assert(sizeof(DpSize::value_type) == sizeof(DpPoint::value_type), "The sizes of these types should match");
      return {value.X, value.Y};
    }

    template <>
    constexpr inline DpSize To<DpSize, DpExtent>(const DpExtent& value)
    {
      return {NumericCast<DpSize::value_type>(value.Width), NumericCast<DpSize::value_type>(value.Height)};
    }

    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------------------------

    // --- DpExtent

    template <>
    constexpr inline DpExtent UncheckedTo<DpExtent, DpPoint>(const DpPoint& value) noexcept
    {
      return {UncheckedNumericCast<DpExtent::value_type>(value.X), UncheckedNumericCast<DpExtent::value_type>(value.Y)};
    }

    template <>
    constexpr inline DpExtent UncheckedTo<DpExtent, DpSize>(const DpSize& value) noexcept
    {
      return {UncheckedNumericCast<DpExtent::value_type>(value.Width()), UncheckedNumericCast<DpExtent::value_type>(value.Height())};
    }

    // --- DpPoint

    template <>
    constexpr inline DpPoint UncheckedTo<DpPoint, DpExtent>(const DpExtent& value) noexcept
    {
      return {UncheckedNumericCast<DpPoint::value_type>(value.Width), UncheckedNumericCast<DpPoint::value_type>(value.Height)};
    }

    template <>
    constexpr inline DpPoint UncheckedTo<DpPoint, DpSize>(const DpSize& value) noexcept
    {
      static_assert(std::is_same<DpPoint::value_type, DpSize::value_type>::value, "we expect the types to be equal");
      return {value.Width(), value.Height()};
    }

    // --- DpSize

    template <>
    constexpr inline DpSize UncheckedTo<DpSize, DpPoint>(const DpPoint& value) noexcept
    {
      static_assert(sizeof(DpSize::value_type) == sizeof(DpPoint::value_type), "The sizes of these types should match");
      return {value.X, value.Y};
    }

    template <>
    constexpr inline DpSize UncheckedTo<DpSize, DpExtent>(const DpExtent& value) noexcept
    {
      return {UncheckedNumericCast<DpSize::value_type>(value.Width), UncheckedNumericCast<DpSize::value_type>(value.Height)};
    }
  }
}

#endif
