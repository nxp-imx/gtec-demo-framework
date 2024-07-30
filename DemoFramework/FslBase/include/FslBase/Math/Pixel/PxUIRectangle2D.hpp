#ifndef FSLBASE_MATH_PIXEL_PXUIRECTANGLE2D_HPP
#define FSLBASE_MATH_PIXEL_PXUIRECTANGLE2D_HPP
/****************************************************************************************************************************************************
 * Copyright 2024 NXP
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
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxVector2.hpp>
#include <cmath>

namespace Fsl
{
  struct PxUIRectangle2D
  {
    using offset_type = PxVector2;
    using size_type = PxSize2D;
    using raw_offset_type = offset_type::raw_value_type;
    using raw_size_type = size_type::raw_value_type;

    PxVector2 Offset;
    PxSize2D Size;

  public:
    constexpr PxUIRectangle2D() noexcept = default;
    constexpr PxUIRectangle2D(const PxVector2 offset, const PxSize2D size) noexcept
      : Offset(offset)
      , Size(size)
    {
    }

    constexpr PxUIRectangle2D(const offset_type::value_type x, const offset_type::value_type y, const size_type::value_type width,
                              const size_type::value_type height) noexcept
      : Offset(x, y)
      , Size(width, height)
    {
    }

    // static PxUIRectangle2D FromLeftTopRightBottom(const offset_type::value_type left, const offset_type::value_type top,
    //                                               const offset_type::value_type right, const offset_type::value_type bottom)
    //{
    //   return CreateFromLeftTopRightBottom(left.Value, top.Value, right.Value, bottom.Value);
    // }


    // static PxUIRectangle2D CreateFromLeftTopRightBottom(const float left, const float top, const float right, const float bottom)
    //{
    //   const auto width = static_cast<raw_size_type>(std::round(right - left));
    //   const auto height = static_cast<raw_size_type>(std::round(bottom - top));
    //   return {offset_type::Create(left, top), size_type::Create(width, height)};
    // }


    static constexpr PxUIRectangle2D Empty() noexcept
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
      return Offset.X + offset_type::value_type(Size.Width());
    }

    constexpr inline offset_type::value_type Bottom() const noexcept
    {
      return Offset.Y + offset_type::value_type(Size.Height());
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
      return Right().Value;
    }

    constexpr inline offset_type::raw_value_type RawBottom() const noexcept
    {
      return Bottom().Value;
    }

    //! @brief Get the center of this rect
    constexpr PxVector2 Center() const noexcept
    {
      const int32_t halfWidth = Size.RawWidth() / 2;
      const int32_t halfHeight = Size.RawHeight() / 2;
      return PxVector2::Create(Offset.X.Value + static_cast<PxVector2::raw_value_type>(halfWidth),
                               Offset.Y.Value + static_cast<PxVector2::raw_value_type>(halfHeight));
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const int32_t x, const int32_t y) const noexcept
    {
      return (static_cast<float>(x) >= RawLeft() && static_cast<float>(x) < RawRight() && static_cast<float>(y) >= RawTop() &&
              static_cast<float>(y) < RawBottom());
    }

    constexpr bool Contains(const float x, const float y) const noexcept
    {
      return (x >= RawLeft() && x < RawRight() && y >= RawTop() && y < RawBottom());
    }

    constexpr bool Contains(const PxValue x, const PxValue y) const noexcept
    {
      return Contains(PxValueF(x), PxValueF(y));
    }

    constexpr bool Contains(const PxValueF x, const PxValueF y) const noexcept
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const PxPoint2 value) const noexcept
    {
      return Contains(value.X, value.Y);
    }

    constexpr bool Contains(const PxVector2 value) const noexcept
    {
      return Contains(value.X, value.Y);
    }

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const PxUIRectangle2D& value) const noexcept
    {
      return Contains(value.Offset) && Contains(value.Right(), value.Bottom());
    }

    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const noexcept
    {
      return (Offset.X.Value == 0 && Offset.Y.Value == 0 && Size.RawWidth() == 0 && Size.RawHeight() == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxUIRectangle2D& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }

    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxUIRectangle2D Intersect(const PxUIRectangle2D& rect1, const PxUIRectangle2D& rect2) noexcept
    {
      if (rect1.Intersects(rect2))
      {
        const offset_type::value_type left = offset_type::value_type::Max(rect1.Left(), rect2.Left());
        const offset_type::value_type top = offset_type::value_type::Max(rect1.Top(), rect2.Top());
        const offset_type::value_type right = offset_type::value_type::Min(rect1.Right(), rect2.Right());
        const offset_type::value_type bottom = offset_type::value_type::Min(rect1.Bottom(), rect2.Bottom());
        return {offset_type(left, top), size_type::Create(static_cast<int32_t>(std::round(right.Value - left.Value)),
                                                          static_cast<int32_t>(std::round(bottom.Value - top.Value)))};
      }
      return {};
    }

    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxUIRectangle2D Union(const PxUIRectangle2D& rect1, const PxUIRectangle2D& rect2) noexcept
    {
      const offset_type::value_type left = offset_type::value_type::Min(rect1.Offset.X, rect2.Offset.X);
      const offset_type::value_type top = offset_type::value_type::Min(rect1.Offset.Y, rect2.Offset.Y);
      const offset_type::value_type right = offset_type::value_type::Max(rect1.Right(), rect2.Right());
      const offset_type::value_type bottom = offset_type::value_type::Max(rect1.Bottom(), rect2.Bottom());
      return {offset_type(left, top), size_type::Create(static_cast<int32_t>(std::round(right.Value - left.Value)),
                                                        static_cast<int32_t>(std::round(bottom.Value - top.Value)))};
    }

    constexpr bool operator==(const PxUIRectangle2D& rhs) const noexcept
    {
      return (Offset == rhs.Offset && Size == rhs.Size);
    }


    constexpr bool operator!=(const PxUIRectangle2D& rhs) const noexcept
    {
      return (Offset != rhs.Offset || Size != rhs.Size);
    }

    inline static constexpr PxUIRectangle2D Create(const raw_offset_type x, const raw_offset_type y, const raw_size_type width,
                                                   const raw_size_type height) noexcept
    {
      return {offset_type::value_type(x), offset_type::value_type(y), size_type::value_type::Create(width), size_type::value_type::Create(height)};
    }
  };
}


#endif
