#ifndef FSLBASE_MATH_RECTANGLE3D_HPP
#define FSLBASE_MATH_RECTANGLE3D_HPP
/****************************************************************************************************************************************************
 * Copyright (c) 2016 Freescale Semiconductor, Inc.
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

#include <FslBase/Math/Extent3D.hpp>
#include <FslBase/Math/Offset3D.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <limits>

namespace Fsl
{
  struct Rectangle3D
  {
    Offset3D Offset;
    Extent3D Extent;

    constexpr Rectangle3D() = default;

    constexpr Rectangle3D(const Offset3D& offset, const Extent3D& extent)
      : Offset(offset)
      , Extent(extent)
    {
    }

    constexpr Rectangle3D(const int32_t x, const int32_t y, int32_t z, const uint16_t width, const uint16_t height, const uint16_t depth) noexcept
      : Offset(x, y, z)
      , Extent(static_cast<Extent3D::value_type>(width), static_cast<Extent3D::value_type>(height), static_cast<Extent3D::value_type>(depth))
    {
    }

    static Rectangle3D FromLeftTopFrontRightBottomBack(const int32_t left, const int32_t top, const int32_t front, const int32_t right,
                                                       const int32_t bottom, const int32_t back);


    static constexpr Rectangle3D Empty() noexcept
    {
      return {};
    }


    inline constexpr int32_t Left() const noexcept
    {
      return Offset.X;
    }

    inline constexpr int32_t Top() const noexcept
    {
      return Offset.Y;
    }

    inline constexpr int32_t Front() const noexcept
    {
      return Offset.Z;
    }

    inline constexpr int32_t Right() const noexcept
    {
      return Offset.X + UncheckedNumericCast<int32_t>(Extent.Width);
    }

    inline constexpr int32_t Bottom() const noexcept
    {
      return Offset.Y + UncheckedNumericCast<int32_t>(Extent.Height);
    }

    inline constexpr int32_t Back() const noexcept
    {
      return Offset.Z + UncheckedNumericCast<int32_t>(Extent.Depth);
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const int32_t x, const int32_t y, const int32_t z) const noexcept
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom() && z >= Front() && z < Back());
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const Offset3D& value) const noexcept
    {
      return Contains(value.X, value.Y, value.Z);
    }


    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const Rectangle3D& value) const noexcept
    {
      return Contains(value.Offset) && Contains(value.Right(), value.Bottom(), value.Back());
    }


    //! @brief Get the center of this rect
    constexpr Offset3D GetCenter() const noexcept
    {
      static_assert(static_cast<Extent3D::value_type>(std::numeric_limits<Offset3D::value_type>::max()) <=
                      (std::numeric_limits<Extent3D::value_type>::max() / 2),
                    "overflow should not be possible");

      return {Offset.X + static_cast<Offset3D::value_type>(Extent.Width / 2), Offset.Y + static_cast<Offset3D::value_type>(Extent.Height / 2),
              Offset.Z + static_cast<Offset3D::value_type>(Extent.Depth / 2)};
    }


    //! @brief Each corner of the Rectangle is pushed away from the center of the rectangle by the specified amounts.
    //!        This results in the width and height of the Rectangle increasing by twice the values provide
    void Inflate(const int32_t horizontalValue, const int32_t verticalValue, const int32_t depthValue);

    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const noexcept
    {
      return (Offset.X == 0 && Offset.Y == 0 && Offset.Z == 0 && Extent.Width == 0 && Extent.Height == 0 && Extent.Depth == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const Rectangle3D& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom() && value.Front() < Back() &&
             Front() < value.Back();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static Rectangle3D Intersect(const Rectangle3D& rect1, const Rectangle3D& rect2);

    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static Rectangle3D Union(const Rectangle3D& rect1, const Rectangle3D& rect2);


    constexpr bool operator==(const Rectangle3D& rhs) const noexcept
    {
      return (Offset == rhs.Offset && Extent == rhs.Extent);
    }


    constexpr bool operator!=(const Rectangle3D& rhs) const noexcept
    {
      return (Offset != rhs.Offset || Extent != rhs.Extent);
    }
  };
}


#endif
