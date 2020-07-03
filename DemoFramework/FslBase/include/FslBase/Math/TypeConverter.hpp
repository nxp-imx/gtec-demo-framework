#ifndef FSLBASE_MATH_TYPECONVERTER_HPP
#define FSLBASE_MATH_TYPECONVERTER_HPP
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

#include <FslBase/Math/Extent2D.hpp>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Rectangle2D.hpp>
#include <FslBase/Math/RectangleU.hpp>
#include <FslBase/TypeConverter.hpp>
#include <cassert>
#include <limits>

namespace Fsl
{
  namespace TypeConverter
  {
    template <>
    constexpr inline Point2 UncheckedTo<Point2, Extent2D>(const Extent2D& value) noexcept
    {
      assert(value.Width <= static_cast<Extent2D::value_type>(std::numeric_limits<Point2::value_type>::max()));
      assert(value.Height <= static_cast<Extent2D::value_type>(std::numeric_limits<Point2::value_type>::max()));
      return {static_cast<Point2::value_type>(value.Width), static_cast<Point2::value_type>(value.Height)};
    }

    template <>
    constexpr inline Extent2D UncheckedTo<Extent2D, Point2>(const Point2& value) noexcept
    {
      static_assert(sizeof(Extent2D::value_type) == sizeof(Point2::value_type), "The sizes of these types should match");
      assert(value.X >= 0);
      assert(value.Y >= 0);
      return {static_cast<Extent2D::value_type>(value.X), static_cast<Extent2D::value_type>(value.Y)};
    }
  }
}

#endif
