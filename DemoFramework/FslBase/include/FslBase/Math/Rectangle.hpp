#ifndef FSLBASE_MATH_RECTANGLE_HPP
#define FSLBASE_MATH_RECTANGLE_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2014 Freescale Semiconductor, Inc.
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
//#include <FslBase/Math/Vector2.hpp>

namespace Fsl
{
  struct Rectangle
  {
  private:
    int32_t m_x{0};
    int32_t m_y{0};
    int32_t m_width{0};
    int32_t m_height{0};

  public:
    constexpr Rectangle() = default;
    Rectangle(const int32_t x, const int32_t y, const int32_t width, const int32_t height);
    Rectangle(const int32_t left, const int32_t top, const int32_t right, const int32_t bottom, bool reserved);

    static Rectangle Empty()
    {
      return Rectangle(0, 0, 0, 0);
    }

    constexpr inline int32_t X() const
    {
      return m_x;
    }

    constexpr inline int32_t Y() const
    {
      return m_y;
    }

    constexpr inline int32_t Width() const
    {
      return m_width;
    }

    constexpr inline int32_t Height() const
    {
      return m_height;
    }


    constexpr inline int32_t Left() const
    {
      return m_x;
    }

    constexpr inline int32_t Top() const
    {
      return m_y;
    }

    constexpr inline int32_t Right() const
    {
      return m_x + m_width;
    }

    constexpr inline int32_t Bottom() const
    {
      return m_y + m_height;
    }

    inline Point2 GetSize() const
    {
      return Point2(m_width, m_height);
    }

    inline Point2 TopLeft() const
    {
      return Point2(m_x, m_y);
    }

    inline Point2 TopRight() const
    {
      return Point2(Right(), m_y);
    }

    inline Point2 BottomLeft() const
    {
      return Point2(m_x, Bottom());
    }

    inline Point2 BottomRight() const
    {
      return Point2(Right(), Bottom());
    }

    inline void Add(const int32_t x, const int32_t y)
    {
      m_x += x;
      m_y += y;
    }

    inline void Add(const Point2& value)
    {
      m_x += value.X;
      m_y += value.Y;
    }

    inline void AddX(const int32_t value)
    {
      m_x += value;
    }

    inline void AddY(const int32_t value)
    {
      m_y += value;
    }

    inline void SetX(const int32_t value)
    {
      m_x = value;
    }

    inline void SetY(const int32_t value)
    {
      m_y = value;
    }

    void SetWidth(const int32_t value);
    void SetHeight(const int32_t value);

    inline void AddSize(const int32_t x, const int32_t y)
    {
      m_width = (m_width + x) >= 0 ? (m_width + x) : 0;
      m_height = (Height() + y) >= 0 ? (m_height + y) : 0;
    }

    inline void AddSize(const Point2& size)
    {
      m_width = (m_width + size.X) >= 0 ? (m_width + size.X) : 0;
      m_height = (Height() + size.Y) >= 0 ? (m_height + size.Y) : 0;
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const int32_t x, const int32_t y) const
    {
      return ((((x >= m_x) && (x < (m_x + m_width))) && (y >= m_y)) && (y < (m_y + m_height)));
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const float x, const float y) const
    {
      return ((((x >= x) && (x < (m_x + m_width))) && (y >= m_y)) && (y < (m_y + m_height)));
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const Point2& value) const
    {
      return ((((value.X >= m_x) && (value.X < (m_x + m_width))) && (value.Y >= m_y)) && (value.Y < (m_y + m_height)));
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    // bool Contains(const Vector2& value) const
    //{
    //  return ((((value.X >= X) && (value.X < (X + Width))) && (value.Y >= Y)) && (value.Y < (Y + Height)));
    //}

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    bool Contains(const Rectangle& value) const
    {
      return ((((value.m_x >= m_x) && ((value.m_x + value.m_width) <= (m_x + m_width))) && (value.m_y >= m_y)) &&
              ((value.m_y + value.m_height) <= (m_y + m_height)));
    }


    //! @brief Get the start location of this rect
    Point2 GetLocation() const
    {
      return Point2(m_x, m_y);
    }


    //! @brief Set the start location of this rect
    void SetLocation(const Point2& location)
    {
      m_x = location.X;
      m_y = location.Y;
    }


    //! @brief Get the center of this rect
    Point2 GetCenter() const
    {
      return Point2(m_x + (m_width / 2), m_y + (m_height / 2));
    }


    //! @brief Each corner of the Rectangle is pushed away from the center of the rectangle by the specified amounts.
    //!        This results in the width and height of the Rectangle increasing by twice the values provide
    void Inflate(const int32_t horizontalValue, const int32_t verticalValue)
    {
      m_x -= horizontalValue;
      m_y -= verticalValue;
      m_width += horizontalValue * 2;
      m_height += verticalValue * 2;
    }


    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    bool IsEmpty() const
    {
      return ((((m_width == 0) && (m_height == 0)) && (m_x == 0)) && (m_y == 0));
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    bool Intersects(const Rectangle& value) const
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static Rectangle Intersect(const Rectangle& rect1, const Rectangle& rect2)
    {
      if (rect1.Intersects(rect2))
      {
        int32_t rightSide = std::min(rect1.m_x + rect1.m_width, rect2.m_x + rect2.m_width);
        int32_t leftSide = std::max(rect1.m_x, rect2.m_x);
        int32_t topSide = std::max(rect1.m_y, rect2.m_y);
        int32_t bottomSide = std::min(rect1.m_y + rect1.m_height, rect2.m_y + rect2.m_height);
        return Rectangle(leftSide, topSide, rightSide - leftSide, bottomSide - topSide);
      }

      return Rectangle(0, 0, 0, 0);
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static Rectangle Union(const Rectangle& rect1, const Rectangle& rect2)
    {
      const int32_t x = std::min(rect1.m_x, rect2.m_x);
      const int32_t y = std::min(rect1.m_y, rect2.m_y);
      return Rectangle(x, y, std::max(rect1.Right(), rect2.Right()) - x, std::max(rect1.Bottom(), rect2.Bottom()) - y);
    }


    constexpr bool operator==(const Rectangle& rhs) const
    {
      return ((m_x == rhs.m_x) && (m_y == rhs.m_y) && (m_width == rhs.m_width) && (m_height == rhs.m_height));
    }


    constexpr bool operator!=(const Rectangle& rhs) const
    {
      return ((m_x != rhs.m_x) || (m_y != rhs.m_y) || (m_width != rhs.m_width) || (m_height != rhs.m_height));
    }
  };
}


#endif
