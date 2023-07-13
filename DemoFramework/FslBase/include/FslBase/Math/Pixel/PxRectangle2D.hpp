#ifndef FSLBASE_MATH_PIXEL_PXRECTANGLE2D_HPP
#define FSLBASE_MATH_PIXEL_PXRECTANGLE2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2020, 2023 NXP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/MathHelper_MinMax.hpp>
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <algorithm>
#include <limits>

namespace Fsl
{
  struct PxRectangle2D
  {
    using offset_type = PxPoint2;
    using extent_type = PxExtent2D;
    using raw_offset_type = offset_type::raw_value_type;
    using raw_extent_type = extent_type::raw_value_type;

    PxPoint2 Offset;
    PxExtent2D Extent;

  public:
    constexpr PxRectangle2D() noexcept = default;
    constexpr PxRectangle2D(const PxPoint2& offset, const PxExtent2D& extent) noexcept
      : Offset(offset)
      , Extent(extent)
    {
    }

    constexpr PxRectangle2D(const offset_type::value_type x, const offset_type::value_type y, const extent_type::value_type width,
                            const extent_type::value_type height) noexcept
      : Offset(x, y)
      , Extent(width, height)
    {
    }

    static constexpr PxRectangle2D FromLeftTopRightBottom(const offset_type::value_type left, const offset_type::value_type top,
                                                          const offset_type::value_type right, const offset_type::value_type bottom) noexcept
    {
      return CreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);
    }

    static constexpr PxRectangle2D UncheckedFromLeftTopRightBottom(const offset_type::value_type left, const offset_type::value_type top,
                                                                   const offset_type::value_type right, const offset_type::value_type bottom) noexcept
    {
      return UncheckedCreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);
    }

    static constexpr PxRectangle2D CreateFromLeftTopRightBottom(const int32_t left, const int32_t top, const int32_t right,
                                                                const int32_t bottom) noexcept
    {
      const auto width =
        UncheckedNumericCast<raw_extent_type>(MathHelper::Max(static_cast<int64_t>(right) - static_cast<int64_t>(left), static_cast<int64_t>(0)));
      const auto height =
        UncheckedNumericCast<raw_extent_type>(MathHelper::Max(static_cast<int64_t>(bottom) - static_cast<int64_t>(top), static_cast<int64_t>(0)));
      return {offset_type::Create(left, top), extent_type::Create(width, height)};
    }

    static constexpr PxRectangle2D UncheckedCreateFromLeftTopRightBottom(const int32_t left, const int32_t top, const int32_t right,
                                                                         const int32_t bottom) noexcept
    {
      assert(right >= left);
      assert(bottom >= top);
      const auto width = UncheckedNumericCast<raw_extent_type>(static_cast<int64_t>(right) - static_cast<int64_t>(left));
      const auto height = UncheckedNumericCast<raw_extent_type>(static_cast<int64_t>(bottom) - static_cast<int64_t>(top));
      return {offset_type::Create(left, top), extent_type::Create(width, height)};
    }

    static constexpr PxRectangle2D Empty() noexcept
    {
      return {};
    }


    constexpr inline offset_type::value_type Left() const noexcept
    {
      return Offset.X;
    }

    constexpr inline offset_type::value_type Top() const noexcept
    {
      return Offset.Y;
    }

    constexpr inline offset_type::value_type Right() const noexcept
    {
      return Offset.X + offset_type::value_type(UncheckedNumericCast<int32_t>(Extent.Width.Value));
    }

    constexpr inline offset_type::value_type Bottom() const noexcept
    {
      return Offset.Y + offset_type::value_type(UncheckedNumericCast<int32_t>(Extent.Height.Value));
    }

    constexpr inline offset_type::raw_value_type RawLeft() const noexcept
    {
      return Offset.X.Value;
    }

    constexpr inline offset_type::raw_value_type RawTop() const noexcept
    {
      return Offset.Y.Value;
    }

    constexpr inline offset_type::raw_value_type RawRight() const noexcept
    {
      return (Offset.X + offset_type::value_type(UncheckedNumericCast<int32_t>(Extent.Width.Value))).Value;
    }

    constexpr inline offset_type::raw_value_type RawBottom() const noexcept
    {
      return (Offset.Y + offset_type::value_type(UncheckedNumericCast<int32_t>(Extent.Height.Value))).Value;
    }

    //! @brief Get the center of this rect
    constexpr PxPoint2 Center() const noexcept
    {
      static_assert(static_cast<PxExtent2D::raw_value_type>(std::numeric_limits<PxPoint2::raw_value_type>::max()) <=
                      (std::numeric_limits<PxExtent2D::raw_value_type>::max() / 2),
                    "overflow should not be possible");
      return PxPoint2::Create(Offset.X.Value + static_cast<PxPoint2::raw_value_type>(Extent.Width.Value / 2),
                              Offset.Y.Value + static_cast<PxPoint2::raw_value_type>(Extent.Height.Value / 2));
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const int32_t x, const int32_t y) const noexcept
    {
      return (x >= RawLeft() && x < RawRight() && y >= RawTop() && y < RawBottom());
    }


    constexpr bool Contains(const PxValue x, const PxValue y) const noexcept
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const PxPoint2 value) const noexcept
    {
      return Contains(value.X, value.Y);
    }

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const PxRectangle2D& value) const noexcept
    {
      return Contains(value.Offset) && Contains(value.Right(), value.Bottom());
    }

    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const noexcept
    {
      return (Offset.X.Value == 0 && Offset.Y.Value == 0 && Extent.Width.Value == 0 && Extent.Height.Value == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxRectangle2D& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }

    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxRectangle2D Intersect(const PxRectangle2D& rect1, const PxRectangle2D& rect2) noexcept
    {
      if (rect1.Intersects(rect2))
      {
        const offset_type::value_type left = offset_type::value_type::Max(rect1.Left(), rect2.Left());
        const offset_type::value_type top = offset_type::value_type::Max(rect1.Top(), rect2.Top());
        const offset_type::value_type right = offset_type::value_type::Min(rect1.Right(), rect2.Right());
        const offset_type::value_type bottom = offset_type::value_type::Min(rect1.Bottom(), rect2.Bottom());
        return FromLeftTopRightBottom(left, top, right, bottom);
      }
      return {};
    }

    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxRectangle2D Union(const PxRectangle2D& rect1, const PxRectangle2D& rect2) noexcept
    {
      const offset_type::value_type left = offset_type::value_type::Min(rect1.Offset.X, rect2.Offset.X);
      const offset_type::value_type top = offset_type::value_type::Min(rect1.Offset.Y, rect2.Offset.Y);
      const offset_type::value_type right = offset_type::value_type::Max(rect1.Right(), rect2.Right());
      const offset_type::value_type bottom = offset_type::value_type::Max(rect1.Bottom(), rect2.Bottom());
      return FromLeftTopRightBottom(left, top, right, bottom);
    }

    constexpr bool operator==(const PxRectangle2D& rhs) const noexcept
    {
      return (Offset == rhs.Offset && Extent == rhs.Extent);
    }


    constexpr bool operator!=(const PxRectangle2D& rhs) const noexcept
    {
      return (Offset != rhs.Offset || Extent != rhs.Extent);
    }

    inline static constexpr PxRectangle2D Create(const raw_offset_type x, const raw_offset_type y, const raw_extent_type width,
                                                 const raw_extent_type height) noexcept
    {
      return {offset_type::value_type(x), offset_type::value_type(y), extent_type::value_type(width), extent_type::value_type(height)};
    }
  };
}


#endif
