#ifndef FSLBASE_MATH_PIXEL_PXCLIPRECTANGLE_HPP
#define FSLBASE_MATH_PIXEL_PXCLIPRECTANGLE_HPP
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

#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/Math/Pixel/PxSize1D.hpp>
#include <FslBase/Math/Pixel/PxSize2D.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>
#include <limits>

namespace Fsl
{
  //! @brief A int32_t based clip rectangle.
  //         It stores the left,top,right,bottom coords as they are used most often during clipping
  struct PxClipRectangle
  {
    using value_type = PxValue;
    using size_value_type = PxSize1D;
    using raw_value_type = value_type::raw_value_type;
    using raw_size_value_type = size_value_type::raw_value_type;

  private:
    value_type m_left{};
    value_type m_top{};
    value_type m_right{};
    value_type m_bottom{};

    constexpr explicit PxClipRectangle(const value_type left, const value_type top, const value_type right, const value_type bottom,
                                       const OptimizationInternal /*unused*/)
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
      assert(right >= left);
      assert(bottom >= top);
      // Detect overflow
      assert((int64_t(right.Value) - int64_t(left.Value)) <= std::numeric_limits<raw_value_type>::max());
      assert((int64_t(bottom.Value) - int64_t(top.Value)) <= std::numeric_limits<raw_value_type>::max());
    }

  public:
    constexpr PxClipRectangle() noexcept = default;

    constexpr PxClipRectangle(const value_type x, const value_type y, const size_value_type width, const size_value_type height)
      : m_left(x)
      , m_top(y)
      , m_right(x + width)
      , m_bottom(y + height)
    {
      assert(m_left <= m_right);
      assert(m_top <= m_bottom);
    }


    constexpr PxClipRectangle(const PxPoint2& offset, const PxSize2D& size)
      : PxClipRectangle(offset.X, offset.Y, size.Width(), size.Height())
    {
    }

    static constexpr PxClipRectangle FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                            const value_type bottom)
    {
      return PxClipRectangle(left, top, std::max(right, left), std::max(bottom, top), OptimizationInternal::Internal);
    }

    static constexpr PxClipRectangle FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                            const value_type bottom, const OptimizationCheckFlag /*unused*/)
    {
      return UncheckedFromLeftTopRightBottom(left, top, right, bottom);
    }

    static constexpr PxClipRectangle UncheckedFromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                                     const value_type bottom)
    {
      assert(right >= left);
      assert(bottom >= top);
      return PxClipRectangle(left, top, right, bottom, OptimizationInternal::Internal);
    }

    constexpr value_type Width() const
    {
      return m_right - m_left;
    }

    constexpr value_type Height() const
    {
      return m_bottom - m_top;
    }

    constexpr value_type Left() const
    {
      return m_left;
    }

    constexpr value_type Top() const
    {
      return m_top;
    }

    constexpr value_type Right() const
    {
      return m_right;
    }

    constexpr value_type Bottom() const
    {
      return m_bottom;
    }

    constexpr raw_value_type RawWidth() const
    {
      return m_right.Value - m_left.Value;
    }

    constexpr raw_value_type RawHeight() const
    {
      return m_bottom.Value - m_top.Value;
    }

    constexpr raw_value_type RawLeft() const
    {
      return m_left.Value;
    }

    constexpr raw_value_type RawTop() const
    {
      return m_top.Value;
    }

    constexpr raw_value_type RawRight() const
    {
      return m_right.Value;
    }

    constexpr raw_value_type RawBottom() const
    {
      return m_bottom.Value;
    }

    inline constexpr PxPoint2 GetSize() const
    {
      return {Width(), Height()};
    }

    inline constexpr PxPoint2 TopLeft() const
    {
      return {m_left, m_top};
    }

    inline constexpr PxPoint2 TopRight() const
    {
      return {m_right, m_top};
    }

    inline constexpr PxPoint2 BottomLeft() const
    {
      return {m_left, m_bottom};
    }

    inline constexpr PxPoint2 BottomRight() const
    {
      return {m_right, m_bottom};
    }


    //! @brief Get the start location of this rect
    constexpr PxPoint2 Location() const
    {
      return {m_left, m_top};
    }


    //! @brief Get the center of this rect
    constexpr PxPoint2 Center() const
    {
      constexpr PxValue size2Px(2);
      return {m_left + (Width() / size2Px), m_top + (Height() / size2Px)};
    }


    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const
    {
      return (m_left.Value == 0 && m_top.Value == 0 && m_right.Value == 0 && m_bottom.Value == 0);
    }

    //! @brief Set the start location of this rect
    constexpr void MoveLocation(const PxPoint2& location) noexcept
    {
      m_right = location.X + (m_right - m_left);
      m_bottom = location.Y + (m_bottom - m_top);
      m_left = location.X;
      m_top = location.Y;
    }

    constexpr void MoveLeft(const value_type value) noexcept
    {
      m_right = value + (m_right - m_left);
      m_left = value;
    }

    constexpr void MoveTop(const value_type value) noexcept
    {
      m_bottom = value + (m_bottom - m_top);
      m_top = value;
    }

    constexpr void SetWidth(const size_value_type value) noexcept
    {
      m_right = m_left + value;
    }

    constexpr void SetHeight(const size_value_type value) noexcept
    {
      m_bottom = m_top + value;
    }

    constexpr void SetWidth(const value_type value) noexcept
    {
      SetWidth(size_value_type(value));
    }

    constexpr void SetHeight(const value_type value) noexcept
    {
      SetHeight(size_value_type(value));
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const value_type x, const value_type y) const noexcept
    {
      return (x >= m_left && x < m_right && y >= m_top && y < m_bottom);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxClipRectangle& value) const noexcept
    {
      return value.m_left < m_right && m_left < value.m_right && value.m_top < m_bottom && m_top < value.m_bottom;
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxClipRectangle Intersect(const PxClipRectangle& rect1, const PxClipRectangle& rect2)
    {
      if (rect1.Intersects(rect2))
      {
        const auto rightSide = std::min(rect1.m_right, rect2.m_right);
        const auto leftSide = std::max(rect1.m_left, rect2.m_left);
        const auto topSide = std::max(rect1.m_top, rect2.m_top);
        const auto bottomSide = std::min(rect1.m_bottom, rect2.m_bottom);
        return PxClipRectangle::FromLeftTopRightBottom(leftSide, topSide, rightSide, bottomSide);
      }
      return {};
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxClipRectangle Union(const PxClipRectangle& rect1, const PxClipRectangle& rect2)
    {
      const auto left = std::min(rect1.m_left, rect2.m_left);
      const auto top = std::min(rect1.m_top, rect2.m_top);
      const auto right = std::max(rect1.m_right, rect2.m_right);
      const auto bottom = std::max(rect1.m_bottom, rect2.m_bottom);
      return PxClipRectangle::FromLeftTopRightBottom(left, top, right, bottom);
    }


    constexpr bool operator==(const PxClipRectangle& rhs) const
    {
      return (m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom);
    }


    constexpr bool operator!=(const PxClipRectangle& rhs) const
    {
      return (m_left != rhs.m_left || m_top != rhs.m_top || m_right != rhs.m_right || m_bottom != rhs.m_bottom);
    }

    inline static constexpr PxClipRectangle Create(const raw_value_type x, const raw_value_type y, const raw_size_value_type width,
                                                   const raw_size_value_type height)
    {
      return {value_type(x), value_type(y), size_value_type::Create(width), size_value_type::Create(height)};
    }

    inline static constexpr PxClipRectangle UncheckedCreate(const raw_value_type x, const raw_value_type y, const raw_size_value_type width,
                                                            const raw_size_value_type height)
    {
      return {value_type(x), value_type(y), size_value_type::UncheckedCreate(width), size_value_type::UncheckedCreate(height)};
    }

    static constexpr PxClipRectangle CreateFromLeftTopRightBottom(const raw_value_type left, const raw_value_type top, const raw_value_type right,
                                                                  const raw_value_type bottom) noexcept
    {
      return PxClipRectangle(value_type(left), value_type(top), value_type::Max(value_type(right), value_type(left)),
                             value_type::Max(value_type(bottom), value_type(top)), OptimizationInternal::Internal);
    }

    static constexpr PxClipRectangle UncheckedCreateFromLeftTopRightBottom(const raw_value_type left, const raw_value_type top,
                                                                           const raw_value_type right, const raw_value_type bottom) noexcept
    {
      assert(right >= left);
      assert(bottom >= top);
      return PxClipRectangle(value_type(left), value_type(top), value_type(right), value_type(bottom), OptimizationInternal::Internal);
    }
  };
}


#endif
