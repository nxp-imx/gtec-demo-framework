#ifndef FSLBASE_MATH_PIXEL_PXRECTANGLEU_HPP
#define FSLBASE_MATH_PIXEL_PXRECTANGLEU_HPP
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
#include <FslBase/Math/Pixel/PxExtent2D.hpp>
#include <FslBase/Math/Pixel/PxPoint2U.hpp>
#include <FslBase/OptimizationFlag.hpp>
#include <algorithm>
#include <cassert>

namespace Fsl
{
  struct PxRectangleU
  {
    using value_type = uint32_t;

    value_type X{0};
    value_type Y{0};
    value_type Width{0};
    value_type Height{0};

    constexpr PxRectangleU() noexcept = default;
    constexpr PxRectangleU(const value_type x, const value_type y, const value_type width, const value_type height) noexcept
      : X(x)
      , Y(y)
      , Width(width)
      , Height(height)
    {
    }

    static constexpr PxRectangleU FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right,
                                                         const value_type bottom) noexcept
    {
      return {left, top, (right >= left ? right - left : 0u), (bottom >= top ? bottom - top : 0u)};
    }

    static constexpr PxRectangleU FromLeftTopRightBottom(const value_type left, const value_type top, const value_type right, const value_type bottom,
                                                         const OptimizationCheckFlag /*unused*/) noexcept
    {
      assert(left <= right);
      assert(top <= bottom);
      return {left, top, right - left, bottom - top};
    }


    static constexpr PxRectangleU Empty() noexcept
    {
      return {};
    }

    constexpr inline value_type Left() const noexcept
    {
      return X;
    }

    constexpr inline value_type Top() const noexcept
    {
      return Y;
    }

    constexpr inline value_type Right() const noexcept
    {
      return X + Width;
    }

    constexpr inline value_type Bottom() const noexcept
    {
      return Y + Height;
    }

    inline constexpr PxExtent2D GetExtent() const noexcept
    {
      return {Width, Height};
    }

    inline constexpr PxPoint2U TopLeft() const noexcept
    {
      return {X, Y};
    }

    inline constexpr PxPoint2U TopRight() const noexcept
    {
      return {Right(), Y};
    }

    inline constexpr PxPoint2U BottomLeft() const noexcept
    {
      return {X, Bottom()};
    }

    inline constexpr PxPoint2U BottomRight() const noexcept
    {
      return {Right(), Bottom()};
    }

    inline constexpr void Add(const PxPoint2U& value) noexcept
    {
      X += value.X;
      Y += value.Y;
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const value_type posX, const value_type posY) const noexcept
    {
      return ((((posX >= X) && (posX < (X + Width))) && (posY >= Y)) && (posY < (Y + Height)));
    }

    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const PxPoint2U& value) const noexcept
    {
      return ((((value.X >= X) && (value.X < (X + Width))) && (value.Y >= Y)) && (value.Y < (Y + Height)));
    }

    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const PxRectangleU& value) const noexcept
    {
      return ((((value.X >= X) && ((value.X + value.Width) <= (X + Width))) && (value.Y >= Y)) && ((value.Y + value.Height) <= (Y + Height)));
    }


    //! @brief Get the start location of this rect
    constexpr PxPoint2U GetLocation() const noexcept
    {
      return {X, Y};
    }


    //! @brief Set the start location of this rect
    constexpr void SetLocation(const PxPoint2U& location) noexcept
    {
      X = location.X;
      Y = location.Y;
    }


    //! @brief Get the center of this rect
    constexpr PxPoint2U GetCenter() const noexcept
    {
      return {X + (Width / 2), Y + (Height / 2)};
    }

    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const noexcept
    {
      return ((((Width == 0) && (Height == 0)) && (X == 0)) && (Y == 0));
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxRectangleU& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxRectangleU Intersect(const PxRectangleU& rect1, const PxRectangleU& rect2)
    {
      if (rect1.Intersects(rect2))
      {
        const auto rightSide = std::min(rect1.X + rect1.Width, rect2.X + rect2.Width);
        const auto leftSide = std::max(rect1.X, rect2.X);
        const auto topSide = std::max(rect1.Y, rect2.Y);
        const auto bottomSide = std::min(rect1.Y + rect1.Height, rect2.Y + rect2.Height);
        return {leftSide, topSide, rightSide - leftSide, bottomSide - topSide};
      }

      return {};
    }


    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxRectangleU Union(const PxRectangleU& rect1, const PxRectangleU& rect2)
    {
      const auto x = std::min(rect1.X, rect2.X);
      const auto y = std::min(rect1.Y, rect2.Y);
      return {x, y, std::max(rect1.Right(), rect2.Right()) - x, std::max(rect1.Bottom(), rect2.Bottom()) - y};
    }


    constexpr bool operator==(const PxRectangleU& rhs) const noexcept
    {
      return ((X == rhs.X) && (Y == rhs.Y) && (Width == rhs.Width) && (Height == rhs.Height));
    }


    constexpr bool operator!=(const PxRectangleU& rhs) const noexcept
    {
      return ((X != rhs.X) || (Y != rhs.Y) || (Width != rhs.Width) || (Height != rhs.Height));
    }
  };
}


#endif
