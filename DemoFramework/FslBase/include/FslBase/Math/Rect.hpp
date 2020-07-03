#ifndef FSLBASE_MATH_RECT_HPP
#define FSLBASE_MATH_RECT_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2015 Freescale Semiconductor, Inc.
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
 *    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
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

#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  //! @brief A float rect.
  //! @note  This structure focuses on keeping the precision right at the edges.
  //!        But if you move the rects left,top coordinates the right, bottom precision will suffer.
  //         So if you want to be precise you better handle such a move by doing it 'right' yourself and then storing the
  //         result in a Rect.
  struct Rect
  {
    using value_type = float;

  private:
    float m_left{0};
    float m_top{0};
    float m_right{0};
    float m_bottom{0};

    constexpr explicit Rect(const float left, const float top, const float right, const float bottom, const OptimizationInternal /*unused*/)
      : m_left{left}
      , m_top{top}
      , m_right{right}
      , m_bottom{bottom}
    {
      assert(right >= left);
      assert(bottom >= top);
    }

  public:
    constexpr Rect() noexcept = default;
    // constexpr Rect(const float x, const float y, const float width, const float height, const OptimizationCheckFlag)

    constexpr Rect(const float x, const float y, const float width, const float height)
      : m_left(x)
      , m_top(y)
      , m_right(x + (width >= 0.0f ? width : 0.0f))
      , m_bottom(y + (height >= 0.0f ? height : 0.0f))
    {
      assert(width >= 0);
      assert(height >= 0);
    }

    static constexpr Rect FromLeftTopRightBottom(const float left, const float top, const float right, const float bottom)
    {
      return Rect(left, top, right, bottom, OptimizationInternal::Internal);
    }

    static constexpr Rect Empty()
    {
      return {};
    }

    //! @brief Check if the rectangle is considered t be valid
    constexpr bool IsValid() const
    {
      return m_right >= m_left && m_bottom >= m_top;
    }

    constexpr void SetX(const float value)
    {
      m_right = value + (m_right - m_left);
      m_left = value;
    }

    constexpr void SetY(const float value)
    {
      m_bottom = value + (m_bottom - m_top);
      m_top = value;
    }

    constexpr void SetLeftRight(const float left, const float right)
    {
      assert(left <= right);
      m_left = left;
      m_right = std::max(right, left);
    }

    constexpr void SetTopBottom(const float top, const float bottom)
    {
      assert(top <= bottom);
      m_top = top;
      m_bottom = std::max(bottom, top);
    }

    constexpr float X() const
    {
      return m_left;
    }

    constexpr float Y() const
    {
      return m_top;
    }

    constexpr float Width() const
    {
      return m_right - m_left;
    }

    constexpr float Height() const
    {
      return m_bottom - m_top;
    }

    constexpr float Left() const
    {
      return m_left;
    }

    constexpr float Top() const
    {
      return m_top;
    }

    constexpr float Right() const
    {
      return m_right;
    }

    constexpr float Bottom() const
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

    void SetWidth(const float value);
    void SetHeight(const float value);

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const int32_t x, const int32_t y) const
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const float x, const float y) const
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const Point2& value) const
    {
      return (value.X >= Left() && value.X < Right() && value.Y >= Top() && value.Y < Bottom());
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const Vector2& value) const
    {
      return (value.X >= Left() && value.X < Right() && value.Y >= Top() && value.Y < Bottom());
    }

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const Rect& value) const
    {
      return (value.Left() >= Left() && value.Right() <= Right() && value.Top() >= Top() && value.Bottom() <= Bottom());
    }


    //! @brief Get the start location of this rect
    constexpr Vector2 GetLocation() const
    {
      return {m_left, m_top};
    }


    //! @brief Set the start location of this rect
    constexpr void SetLocation(const Vector2& location)
    {
      m_right = location.X + (m_right - m_left);
      m_bottom = location.Y + (m_bottom - m_top);
      m_left = location.X;
      m_top = location.Y;
    }


    //! @brief Get the center of this rect
    constexpr Vector2 GetCenter() const
    {
      return {m_left + (Width() / 2), m_top + (Height() / 2)};
    }


    //! @brief Each corner of the Rectangle is pushed away from the center of the rectangle by the specified amounts.
    //!        This results in the width and height of the Rectangle increasing by twice the values provide
    constexpr void Inflate(const float horizontalValue, const float verticalValue)
    {
      m_left -= horizontalValue;
      m_top -= verticalValue;
      m_right += horizontalValue;
      m_bottom += verticalValue;
    }


    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const
    {
      return (Width() == 0 && Height() == 0 && X() == 0 && Y() == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const Rect& value) const
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static Rect Intersect(const Rect& rect1, const Rect& rect2)
    {
      if (rect1.Intersects(rect2))
      {
        const float rightSide = std::min(rect1.Right(), rect2.Right());
        const float leftSide = std::max(rect1.Left(), rect2.Left());
        const float topSide = std::max(rect1.Top(), rect2.Top());
        const float bottomSide = std::min(rect1.Bottom(), rect2.Bottom());
        return Rect::FromLeftTopRightBottom(leftSide, topSide, rightSide, bottomSide);
      }
      return {};
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static Rect Union(const Rect& rect1, const Rect& rect2)
    {
      const float left = std::min(rect1.Left(), rect2.Left());
      const float top = std::min(rect1.Top(), rect2.Top());
      const float right = std::max(rect1.Right(), rect2.Right());
      const float bottom = std::max(rect1.Bottom(), rect2.Bottom());
      return Rect::FromLeftTopRightBottom(left, top, right, bottom);
    }


    constexpr bool operator==(const Rect& rhs) const
    {
      return (m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom);
    }


    constexpr bool operator!=(const Rect& rhs) const
    {
      return (m_left != rhs.m_left || m_top != rhs.m_top || m_right != rhs.m_right || m_bottom != rhs.m_bottom);
    }
  };
}


#endif
