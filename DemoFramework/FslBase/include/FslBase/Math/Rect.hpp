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

#include <algorithm>
#include <FslBase/Math/Point2.hpp>
#include <FslBase/Math/Vector2.hpp>

namespace Fsl
{
  //! @brief A float rect.
  //! @note  This structure focuses on keeping the precision right at the edges.
  //!        But if you move the rects left,top coordinates the right, bottom precision will suffer.
  //         So if you want to be precise you better handle such a move by doing it 'right' yourself and then storing the
  //         result in a Rect.
  struct Rect
  {
  private:
    float m_left{0};
    float m_top{0};
    float m_right{0};
    float m_bottom{0};

  public:
    Rect() = default;
    Rect(const float x, const float y, const float width, const float height);
    Rect(const float left, const float top, const float right, const float bottom, const bool reserved);

    static Rect Empty()
    {
      return {};
    }

    //! @brief Check if the rectangle is considered t be valid
    bool IsValid() const
    {
      return m_right >= m_left && m_bottom >= m_top;
    }

    void SetX(const float value)
    {
      m_right = value + (m_right - m_left);
      m_left = value;
    }

    void SetY(const float value)
    {
      m_bottom = value + (m_bottom - m_top);
      m_top = value;
    }

    float X() const
    {
      return m_left;
    }

    float Y() const
    {
      return m_top;
    }

    float Width() const
    {
      return m_right - m_left;
    }

    float Height() const
    {
      return m_bottom - m_top;
    }

    float Left() const
    {
      return m_left;
    }

    float Top() const
    {
      return m_top;
    }

    float Right() const
    {
      return m_right;
    }

    float Bottom() const
    {
      return m_bottom;
    }

    inline Vector2 GetSize() const
    {
      return Vector2(Width(), Height());
    }

    inline Vector2 TopLeft() const
    {
      return Vector2(m_left, m_top);
    }

    inline Vector2 TopRight() const
    {
      return Vector2(m_right, m_top);
    }

    inline Vector2 BottomLeft() const
    {
      return Vector2(m_left, m_bottom);
    }

    inline Vector2 BottomRight() const
    {
      return Vector2(m_right, m_bottom);
    }

    void SetWidth(const float value);
    void SetHeight(const float value);

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const int x, const int y) const
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const float x, const float y) const
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const Point2& value) const
    {
      return (value.X >= Left() && value.X < Right() && value.Y >= Top() && value.Y < Bottom());
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const Vector2& value) const
    {
      return (value.X >= Left() && value.X < Right() && value.Y >= Top() && value.Y < Bottom());
    }

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    bool Contains(const Rect& value) const
    {
      return (value.Left() >= Left() && value.Right() <= Right() && value.Top() >= Top() && value.Bottom() <= Bottom());
    }


    //! @brief Get the start location of this rect
    Vector2 GetLocation() const
    {
      return Vector2(m_left, m_top);
    }


    //! @brief Set the start location of this rect
    void SetLocation(const Vector2& location)
    {
      m_right = location.X + (m_right - m_left);
      m_bottom = location.Y + (m_bottom - m_top);
      m_left = location.X;
      m_top = location.Y;
    }


    //! @brief Get the center of this rect
    Vector2 GetCenter() const
    {
      return Vector2(m_left + (Width() / 2), m_right + (Height() / 2));
    }


    //! @brief Each corner of the Rectangle is pushed away from the center of the rectangle by the specified amounts.
    //!        This results in the width and height of the Rectangle increasing by twice the values provide
    void Inflate(const float horizontalValue, const float verticalValue)
    {
      m_left -= horizontalValue;
      m_top -= verticalValue;
      m_right += horizontalValue;
      m_bottom += verticalValue;
    }


    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    bool IsEmpty() const
    {
      return (Width() == 0 && Height() == 0 && X() == 0 && Y() == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    bool Intersects(const Rect& value) const
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
        return Rect(leftSide, topSide, rightSide - leftSide, bottomSide - topSide);
      }
      return Rect();
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static Rect Union(const Rect& rect1, const Rect& rect2)
    {
      const float left = std::min(rect1.Left(), rect2.Left());
      const float top = std::min(rect1.Top(), rect2.Top());
      const float right = std::max(rect1.Right(), rect2.Right());
      const float bottom = std::max(rect1.Bottom(), rect2.Bottom());
      return Rect(left, top, right, bottom, true);
    }


    bool operator==(const Rect& rhs) const
    {
      return (m_left == rhs.m_left && m_top == rhs.m_top && m_right == rhs.m_right && m_bottom == rhs.m_bottom);
    }


    bool operator!=(const Rect& rhs) const
    {
      return (m_left != rhs.m_left || m_top != rhs.m_top || m_right != rhs.m_right || m_bottom != rhs.m_bottom);
    }
  };
}


#endif
