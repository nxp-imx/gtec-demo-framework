#ifndef FSLBASE_MATH_PIXEL_PXAREARECTANGLEF_HPP
#define FSLBASE_MATH_PIXEL_PXAREARECTANGLEF_HPP
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

#include <FslBase/Math/Vector2.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  //! @brief A float rectangle.
  //! @note  This structure focuses on keeping the precision right at the edges.
  //!        But if you move the rects left,top coordinates the right, bottom precision will suffer.
  //         So if you want to be precise you better handle such a move by doing it 'right' yourself and then storing the
  //         result in a PxAreaRectangleF.
  struct PxAreaRectangleF
  {
    using value_type = float;


  private:
    value_type m_left{};
    value_type m_top{};
    value_type m_right{};
    value_type m_bottom{};

    constexpr explicit PxAreaRectangleF(const value_type left, const value_type top, const value_type right, const value_type bottom,
                                        const OptimizationInternal /*unused*/)
      : m_left(left)
      , m_top(top)
      , m_right(right)
      , m_bottom(bottom)
    {
      assert(right >= left);
      assert(bottom >= top);
    }

  public:
    constexpr PxAreaRectangleF() noexcept = default;

    constexpr PxAreaRectangleF(const value_type x, const value_type y, const value_type width, const value_type height)
      : m_left(x)
      , m_top(y)
      , m_right(x + std::max(width, 0.0f))
      , m_bottom(y + std::max(height, 0.0f))
    {
      assert(width >= 0);
      assert(height >= 0);
    }

    static constexpr PxAreaRectangleF FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                             const value_type bottom)
    {
      return PxAreaRectangleF(left, top, std::max(right, left), std::max(bottom, top), OptimizationInternal::Internal);
    }

    static constexpr PxAreaRectangleF FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                             const value_type bottom, const OptimizationCheckFlag /*unused*/)
    {
      assert(right >= left);
      assert(bottom >= top);
      return PxAreaRectangleF(left, top, right, bottom, OptimizationInternal::Internal);
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

    inline constexpr Vector2 GetSize() const
    {
      return {Width(), Height()};
    }

    inline constexpr Vector2 TopLeft() const
    {
      return {m_left, m_top};
    }

    inline constexpr Vector2 TopRight() const
    {
      return {m_right, m_top};
    }

    inline constexpr Vector2 BottomLeft() const
    {
      return {m_left, m_bottom};
    }

    inline constexpr Vector2 BottomRight() const
    {
      return {m_right, m_bottom};
    }


    //! @brief Get the start location of this rect
    constexpr Vector2 Location() const
    {
      return {m_left, m_top};
    }


    //! @brief Get the center of this rect
    constexpr Vector2 Center() const
    {
      return {m_left + (Width() / 2.0f), m_top + (Height() / 2.0f)};
    }


    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const
    {
      return (m_left == 0 && m_top == 0 && m_right == 0 && m_bottom == 0);
    }

    //! @brief Set the start location of this rect
    constexpr void MoveLocation(const Vector2& location)
    {
      m_right = location.X + (m_right - m_left);
      m_bottom = location.Y + (m_bottom - m_top);
      m_left = location.X;
      m_top = location.Y;
    }

    constexpr void MoveLeft(const value_type value)
    {
      m_right = value + (m_right - m_left);
      m_left = value;
    }

    constexpr void SetLeftRight(const value_type left, const value_type right)
    {
      m_left = left;
      m_right = std::max(right, left);
    }

    constexpr void MoveTop(const value_type value)
    {
      m_bottom = value + (m_bottom - m_top);
      m_top = value;
    }

    constexpr void SetTopBottom(const value_type top, const value_type bottom)
    {
      m_top = top;
      m_bottom = std::max(bottom, top);
    }

    constexpr void SetWidth(const value_type value)
    {
      m_right = std::max(m_left + value, m_left);
    }

    constexpr void SetHeight(const value_type value)
    {
      m_bottom = std::max(m_top + value, m_top);
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const value_type x, const value_type y) const
    {
      return (x >= m_left && x < m_right && y >= m_top && y < m_bottom);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxAreaRectangleF& value) const
    {
      return value.m_left < m_right && m_left < value.m_right && value.m_top < m_bottom && m_top < value.m_bottom;
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxAreaRectangleF Intersect(const PxAreaRectangleF& rect1, const PxAreaRectangleF& rect2)
    {
      if (rect1.Intersects(rect2))
      {
        const auto rightSide = std::min(rect1.m_right, rect2.m_right);
        const auto leftSide = std::max(rect1.m_left, rect2.m_left);
        const auto topSide = std::max(rect1.m_top, rect2.m_top);
        const auto bottomSide = std::min(rect1.m_bottom, rect2.m_bottom);
        return PxAreaRectangleF::FromLeftTopRightBottom(leftSide, topSide, rightSide, bottomSide);
      }
      return {};
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxAreaRectangleF Union(const PxAreaRectangleF& rect1, const PxAreaRectangleF& rect2)
    {
      const auto left = std::min(rect1.m_left, rect2.m_left);
      const auto top = std::min(rect1.m_top, rect2.m_top);
      const auto right = std::max(rect1.m_right, rect2.m_right);
      const auto bottom = std::max(rect1.m_bottom, rect2.m_bottom);
      return PxAreaRectangleF::FromLeftTopRightBottom(left, top, right, bottom);
    }


    constexpr bool operator==(const PxAreaRectangleF& rhs) const
    {
      return (m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom);
    }


    constexpr bool operator!=(const PxAreaRectangleF& rhs) const
    {
      return (m_left != rhs.m_left || m_top != rhs.m_top || m_right != rhs.m_right || m_bottom != rhs.m_bottom);
    }
  };
}


#endif
