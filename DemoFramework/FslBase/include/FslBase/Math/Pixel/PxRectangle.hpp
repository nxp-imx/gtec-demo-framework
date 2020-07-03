#ifndef FSLBASE_MATH_PIXEL_PXRECTANGLE_HPP
#define FSLBASE_MATH_PIXEL_PXRECTANGLE_HPP
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

#include <FslBase/Log/Log3Core.hpp>
#include <FslBase/Math/MathHelper_Clamp.hpp>
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  struct PxRectangle
  {
    using value_type = int32_t;

  private:
    value_type m_x{0};
    value_type m_y{0};
    value_type m_width{0};
    value_type m_height{0};

  public:
    constexpr PxRectangle() noexcept = default;
    constexpr PxRectangle(const value_type x, const value_type y, const value_type width, const value_type height) noexcept
      : m_x(x)
      , m_y(y)
      , m_width(std::max(width, 0))
      , m_height(std::max(height, 0))
    {
      assert(width >= 0);
      assert(height >= 0);
    }

    constexpr PxRectangle(const PxPoint2& offset, const PxPoint2& size) noexcept
      : m_x(offset.X)
      , m_y(offset.Y)
      , m_width(std::max(size.X, 0))
      , m_height(std::max(size.Y, 0))
    {
      assert(size.X >= 0);
      assert(size.Y >= 0);
    }

    constexpr PxRectangle(const PxPoint2& offset, const PxSize2D& size) noexcept
      : m_x(offset.X)
      , m_y(offset.Y)
      , m_width(size.Width())
      , m_height(size.Height())
    {
      assert(m_width >= 0);
      assert(m_height >= 0);
    }


    static constexpr PxRectangle FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                        const value_type bottom) noexcept
    {
      assert((int64_t(right) - int64_t(left)) <= std::numeric_limits<value_type>::max());
      assert((int64_t(bottom) - int64_t(top)) <= std::numeric_limits<value_type>::max());
      // do the calcs in int64_t and then clamp it to a valid range to handle overflows
      auto width = value_type(MathHelper::Clamp(int64_t(right) - int64_t(left), int64_t(0), int64_t(std::numeric_limits<value_type>::max())));
      auto height = value_type(MathHelper::Clamp(int64_t(bottom) - int64_t(top), int64_t(0), int64_t(std::numeric_limits<value_type>::max())));
      return {left, top, width, height};
    }

    static constexpr PxRectangle FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right, const value_type bottom,
                                                        const OptimizationCheckFlag /*unused*/) noexcept
    {
      assert(left <= right);
      assert(top <= bottom);
      // Detect overflow
      assert((int64_t(right) - int64_t(left)) <= std::numeric_limits<value_type>::max());
      assert((int64_t(bottom) - int64_t(top)) <= std::numeric_limits<value_type>::max());
      return {left, top, right - left, bottom - top};
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

    constexpr inline value_type Width() const noexcept
    {
      return m_width;
    }

    constexpr inline value_type Height() const noexcept
    {
      return m_height;
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
      assert(m_width >= 0);
      return m_x + m_width;
    }

    constexpr inline value_type Bottom() const noexcept
    {
      assert(m_height >= 0);
      return m_y + m_height;
    }

    inline constexpr PxPoint2 GetSize() const noexcept
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
      return {m_x + (m_width / 2), m_y + (m_height / 2)};
    }

    //! @brief Get the start location of this rect
    constexpr PxPoint2 Location() const noexcept
    {
      return {m_x, m_y};
    }

    //! @brief Set the start x-location of this rect
    constexpr void SetX(const value_type& x) noexcept
    {
      m_x = x;
    }

    //! @brief Set the start y-location of this rect
    constexpr void SetY(const value_type& y) noexcept
    {
      m_y = y;
    }

    constexpr void SetWidth(const value_type& width)
    {
      m_width = std::max(width, 0);
    }

    constexpr void SetHeight(const value_type& height)
    {
      m_height = std::max(height, 0);
    }

    //! @brief Set the start location of this rect
    constexpr void SetLocation(const PxPoint2& location) noexcept
    {
      m_x = location.X;
      m_y = location.Y;
    }

    inline constexpr void AddX(const value_type value) noexcept
    {
      m_x += value;
    }

    inline constexpr void AddY(const value_type value) noexcept
    {
      m_y += value;
    }

    inline constexpr void Add(const PxPoint2& value) noexcept
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
      return ((((m_width == 0) && (m_height == 0)) && (m_x == 0)) && (m_y == 0));
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxRectangle& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxRectangle Intersect(const PxRectangle& rect1, const PxRectangle& rect2)
    {
      if (rect1.Intersects(rect2))
      {
        const auto rightSide = std::min(rect1.m_x + rect1.m_width, rect2.m_x + rect2.m_width);
        const auto leftSide = std::max(rect1.m_x, rect2.m_x);
        const auto topSide = std::max(rect1.m_y, rect2.m_y);
        const auto bottomSide = std::min(rect1.m_y + rect1.m_height, rect2.m_y + rect2.m_height);
        return FromLeftTopRightBottom(leftSide, topSide, rightSide, bottomSide);
      }
      return {};
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxRectangle Union(const PxRectangle& rect1, const PxRectangle& rect2)
    {
      const auto x = std::min(rect1.m_x, rect2.m_x);
      const auto y = std::min(rect1.m_y, rect2.m_y);
      return FromLeftTopRightBottom(x, y, std::max(rect1.Right(), rect2.Right()), std::max(rect1.Bottom(), rect2.Bottom()));
    }


    constexpr bool operator==(const PxRectangle& rhs) const noexcept
    {
      return ((m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_width == rhs.m_width) && (m_height == rhs.m_height));
    }


    constexpr bool operator!=(const PxRectangle& rhs) const noexcept
    {
      return ((m_x != rhs.m_x) || (m_y != rhs.m_y) || (m_width != rhs.m_width) || (m_height != rhs.m_height));
    }
  };
}


#endif
