#ifndef FSLBASE_MATH_PIXEL_PXRECTANGLE_HPP
#define FSLBASE_MATH_PIXEL_PXRECTANGLE_HPP
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
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/Math/Pixel/PxValue.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  struct PxRectangle
  {
    using value_type = PxValue;
    using raw_value_type = value_type::raw_value_type;
    using size_value_type = PxSize1D;
    using raw_size_value_type = size_value_type::raw_value_type;

  private:
    value_type m_x;
    value_type m_y;
    size_value_type m_width;
    size_value_type m_height;

  public:
    constexpr PxRectangle() noexcept = default;


    constexpr PxRectangle(const value_type x, const value_type y, const size_value_type width, const size_value_type height) noexcept
      : m_x(x)
      , m_y(y)
      , m_width(width)
      , m_height(height)
    {
    }

    constexpr PxRectangle(const value_type x, const value_type y, const size_value_type width, const value_type height) noexcept
      : PxRectangle(x, y, width, size_value_type(height))
    {
    }

    constexpr PxRectangle(const value_type x, const value_type y, const value_type width, const size_value_type height) noexcept
      : PxRectangle(x, y, size_value_type(width), height)
    {
    }

    constexpr PxRectangle(const value_type x, const value_type y, const value_type width, const value_type height) noexcept
      : PxRectangle(x, y, size_value_type(width), size_value_type(height))
    {
    }


    constexpr PxRectangle(const value_type x, const size_value_type y, const size_value_type width, const size_value_type height) noexcept
      : PxRectangle(x, y.Value(), width, height)
    {
    }

    constexpr PxRectangle(const value_type x, const size_value_type y, const size_value_type width, const value_type height) noexcept
      : PxRectangle(x, y.Value(), width, size_value_type(height))
    {
    }

    constexpr PxRectangle(const value_type x, const size_value_type y, const value_type width, const size_value_type height) noexcept
      : PxRectangle(x, y.Value(), size_value_type(width), height)
    {
    }

    constexpr PxRectangle(const value_type x, const size_value_type y, const value_type width, const value_type height) noexcept
      : PxRectangle(x, y.Value(), size_value_type(width), size_value_type(height))
    {
    }

    constexpr PxRectangle(const size_value_type x, const value_type y, const size_value_type width, const size_value_type height) noexcept
      : PxRectangle(x.Value(), y, width, height)
    {
    }

    constexpr PxRectangle(const size_value_type x, const value_type y, const size_value_type width, const value_type height) noexcept
      : PxRectangle(x.Value(), y, width, size_value_type(height))
    {
    }

    constexpr PxRectangle(const size_value_type x, const value_type y, const value_type width, const size_value_type height) noexcept
      : PxRectangle(x.Value(), y, size_value_type(width), height)
    {
    }

    constexpr PxRectangle(const size_value_type x, const value_type y, const value_type width, const value_type height) noexcept
      : PxRectangle(x.Value(), y, size_value_type(width), size_value_type(height))
    {
    }


    constexpr PxRectangle(const size_value_type x, const size_value_type y, const size_value_type width, const size_value_type height) noexcept
      : PxRectangle(x.Value(), y.Value(), width, height)
    {
    }

    constexpr PxRectangle(const size_value_type x, const size_value_type y, const size_value_type width, const value_type height) noexcept
      : PxRectangle(x.Value(), y.Value(), width, size_value_type(height))
    {
    }

    constexpr PxRectangle(const size_value_type x, const size_value_type y, const value_type width, const size_value_type height) noexcept
      : PxRectangle(x.Value(), y.Value(), size_value_type(width), height)
    {
    }

    constexpr PxRectangle(const size_value_type x, const size_value_type y, const value_type width, const value_type height) noexcept
      : PxRectangle(x.Value(), y.Value(), size_value_type(width), size_value_type(height))
    {
    }


    constexpr PxRectangle(const PxPoint2 offset, const PxPoint2 size) noexcept
      : m_x(offset.X)
      , m_y(offset.Y)
      , m_width(size.X)
      , m_height(size.Y)
    {
    }

    constexpr PxRectangle(const PxPoint2 offset, const PxSize2D size) noexcept
      : m_x(offset.X)
      , m_y(offset.Y)
      , m_width(size.Width())
      , m_height(size.Height())
    {
    }


    static constexpr PxRectangle FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                        const value_type bottom) noexcept
    {
      // Detect overflow
      assert((int64_t(right.Value) - int64_t(left.Value)) <= std::numeric_limits<raw_value_type>::max());
      assert((int64_t(bottom.Value) - int64_t(top.Value)) <= std::numeric_limits<raw_value_type>::max());
      return {left, top, size_value_type::Create(right - left), size_value_type::Create(bottom - top)};
    }

    static constexpr PxRectangle UncheckedFromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                                 const value_type bottom) noexcept
    {
      // Validate usage
      assert(left <= right);
      assert(top <= bottom);
      // Detect overflow
      assert((int64_t(right.Value) - int64_t(left.Value)) <= std::numeric_limits<raw_value_type>::max());
      assert((int64_t(bottom.Value) - int64_t(top.Value)) <= std::numeric_limits<raw_value_type>::max());
      return {left, top, size_value_type::UncheckedCreate(right - left), size_value_type::UncheckedCreate(bottom - top)};
    }


    static constexpr PxRectangle Empty() noexcept
    {
      return {};
    }

    constexpr inline value_type X() const noexcept
    {
      return m_x;
    }

    constexpr inline value_type Y() const noexcept
    {
      return m_y;
    }

    constexpr inline raw_value_type RawX() const noexcept
    {
      return m_x.Value;
    }

    constexpr inline raw_value_type RawY() const noexcept
    {
      return m_y.Value;
    }

    constexpr inline size_value_type Width() const noexcept
    {
      return m_width;
    }

    constexpr inline size_value_type Height() const noexcept
    {
      return m_height;
    }


    constexpr inline raw_size_value_type RawWidth() const noexcept
    {
      return m_width.RawValue();
    }

    constexpr inline raw_size_value_type RawHeight() const noexcept
    {
      return m_height.RawValue();
    }


    constexpr inline value_type Left() const noexcept
    {
      return m_x;
    }

    constexpr inline value_type Top() const noexcept
    {
      return m_y;
    }

    constexpr inline value_type Right() const noexcept
    {
      return m_x + m_width;
    }

    constexpr inline value_type Bottom() const noexcept
    {
      return m_y + m_height;
    }


    constexpr inline raw_value_type RawLeft() const noexcept
    {
      return m_x.Value;
    }

    constexpr inline raw_value_type RawTop() const noexcept
    {
      return m_y.Value;
    }

    constexpr inline raw_value_type RawRight() const noexcept
    {
      return m_x.Value + m_width.RawValue();
    }

    constexpr inline raw_value_type RawBottom() const noexcept
    {
      return m_y.Value + m_height.RawValue();
    }

    inline constexpr PxSize2D GetSize() const noexcept
    {
      return {m_width, m_height};
    }

    inline constexpr PxPoint2 TopLeft() const noexcept
    {
      return {m_x, m_y};
    }

    inline constexpr PxPoint2 TopRight() const noexcept
    {
      return {Right(), m_y};
    }

    inline constexpr PxPoint2 BottomLeft() const noexcept
    {
      return {m_x, Bottom()};
    }

    inline constexpr PxPoint2 BottomRight() const noexcept
    {
      return {Right(), Bottom()};
    }

    //! @brief Get the center of this rect
    constexpr PxPoint2 GetCenter() const noexcept
    {
      constexpr value_type Size2(2);
      return {m_x + (m_width / Size2), m_y + (m_height / Size2)};
    }

    //! @brief Get the start location of this rect
    constexpr PxPoint2 Location() const noexcept
    {
      return {m_x, m_y};
    }

    //! @brief Set the start x-location of this rect
    constexpr void SetX(const value_type x) noexcept
    {
      m_x = x;
    }

    //! @brief Set the start y-location of this rect
    constexpr void SetY(const value_type y) noexcept
    {
      m_y = y;
    }

    constexpr void SetWidth(const size_value_type width)
    {
      m_width = width;
    }

    constexpr void SetHeight(const size_value_type height)
    {
      m_height = height;
    }

    constexpr void SetWidth(const value_type width)
    {
      m_width = size_value_type(width);
    }

    constexpr void SetHeight(const value_type height)
    {
      m_height = size_value_type(height);
    }

    //! @brief Set the start location of this rect
    constexpr void SetLocation(const PxPoint2 location) noexcept
    {
      m_x = location.X;
      m_y = location.Y;
    }

    inline constexpr void AddX(const value_type value) noexcept
    {
      m_x += value;
    }

    inline constexpr void AddX(const size_value_type value) noexcept
    {
      m_x += value.Value();
    }


    inline constexpr void AddY(const value_type value) noexcept
    {
      m_y += value;
    }

    inline constexpr void AddY(const size_value_type value) noexcept
    {
      m_y += value.Value();
    }

    inline constexpr void Add(const PxPoint2 value) noexcept
    {
      m_x += value.X;
      m_y += value.Y;
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const value_type posX, const value_type posY) const noexcept
    {
      return ((((posX >= m_x) && (posX < Right())) && (posY >= m_y)) && (posY < Bottom()));
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const PxPoint2& value) const noexcept
    {
      return ((((value.X >= m_x) && (value.X < Right())) && (value.Y >= m_y)) && (value.Y < Bottom()));
    }

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const PxRectangle& value) const noexcept
    {
      return ((((value.m_x >= m_x) && (value.Right() <= Right())) && (value.m_y >= m_y)) && ((value.m_y + value.m_height) <= Bottom()));
    }


    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const noexcept
    {
      return m_width.RawValue() == 0 && m_height.RawValue() == 0 && m_x.Value == 0 && m_y.Value == 0;
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxRectangle& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxRectangle Intersect(const PxRectangle& rect1, const PxRectangle& rect2) noexcept
    {
      if (rect1.Intersects(rect2))
      {
        const auto left = value_type::Max(rect1.m_x, rect2.m_x);
        const auto top = value_type::Max(rect1.m_y, rect2.m_y);
        const auto right = value_type::Min(rect1.m_x + rect1.m_width, rect2.m_x + rect2.m_width);
        const auto bottom = value_type::Min(rect1.m_y + rect1.m_height, rect2.m_y + rect2.m_height);
        return UncheckedFromLeftTopRightBottom(left, top, right, bottom);
      }
      return {};
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxRectangle Union(const PxRectangle& rect1, const PxRectangle& rect2) noexcept
    {
      const auto left = value_type::Min(rect1.m_x, rect2.m_x);
      const auto top = value_type::Min(rect1.m_y, rect2.m_y);
      const auto right = value_type::Max(rect1.Right(), rect2.Right());
      const auto bottom = value_type::Max(rect1.Bottom(), rect2.Bottom());
      return UncheckedFromLeftTopRightBottom(left, top, right, bottom);
    }


    constexpr bool operator==(const PxRectangle& rhs) const noexcept
    {
      return ((m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_width == rhs.m_width) && (m_height == rhs.m_height));
    }

    constexpr bool operator!=(const PxRectangle& rhs) const noexcept
    {
      return ((m_x != rhs.m_x) || (m_y != rhs.m_y) || (m_width != rhs.m_width) || (m_height != rhs.m_height));
    }

    static constexpr PxRectangle Create(const raw_value_type x, const raw_value_type y, const raw_size_value_type width,
                                        const raw_size_value_type height) noexcept
    {
      return {value_type(x), value_type(y), size_value_type::Create(width), size_value_type::Create(height)};
    }

    static constexpr PxRectangle CreateFromLeftTopRightBottom(const raw_value_type left, const raw_value_type top, const raw_value_type right,
                                                              const raw_value_type bottom) noexcept
    {
      return FromLeftTopRightBottom(value_type(left), value_type(top), value_type(right), value_type(bottom));
    }

    static constexpr PxRectangle UncheckedCreate(const raw_value_type x, const raw_value_type y, const raw_size_value_type width,
                                                 const raw_size_value_type height) noexcept
    {
      return {value_type(x), value_type(y), size_value_type::UncheckedCreate(width), size_value_type::UncheckedCreate(height)};
    }

    static constexpr PxRectangle UncheckedCreateFromLeftTopRightBottom(const raw_value_type left, const raw_value_type top,
                                                                       const raw_value_type right, const raw_value_type bottom) noexcept
    {
      return UncheckedFromLeftTopRightBottom(value_type(left), value_type(top), value_type(right), value_type(bottom));
    }
  };
}


#endif
