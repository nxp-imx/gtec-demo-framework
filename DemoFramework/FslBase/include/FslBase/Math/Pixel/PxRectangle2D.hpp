#ifndef FSLBASE_MATH_PIXEL_PXRECTANGLE2D_HPP
#define FSLBASE_MATH_PIXEL_PXRECTANGLE2D_HPP
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
#include <FslBase/Math/Pixel/PxPoint2.hpp>
#include <FslBase/UncheckedNumericCast.hpp>
#include <algorithm>
#include <limits>

namespace Fsl
{
  struct PxRectangle2D
  {
    using offset_type = PxPoint2;
    using extent_type = PxExtent2D;

    PxPoint2 Offset;
    PxExtent2D Extent;

  public:
    constexpr PxRectangle2D() noexcept = default;
    constexpr PxRectangle2D(const PxPoint2& offset, const PxExtent2D& extent) noexcept
      : Offset(offset)
      , Extent(extent)
    {
    }

    constexpr PxRectangle2D(const offset_type::value_type x, const offset_type::value_type y, const extent_type::value_type width,
                            const extent_type::value_type height) noexcept
      : Offset(x, y)
      , Extent(width, height)
    {
    }

    static constexpr PxRectangle2D FromLeftTopRightBottom(const int32_t left, const int32_t top, const int32_t right, const int32_t bottom)
    {
      static_assert(sizeof(PxExtent2D::value_type) >= sizeof(PxPoint2::value_type), "for the below check to work this has to be true");
      // we do not include type traits so this is a poor man checks
      static_assert(sizeof(int32_t) == sizeof(PxPoint2::value_type), "expect the PxPoint2 value_type to be a int32_t");
      static_assert(sizeof(uint32_t) == sizeof(PxExtent2D::value_type), "expect the PxExtent2D value_type to be a uint32_t");
      // FSLLOG3_DEBUG_INFO_IF(right > left, "right > left, capping width to zero");
      // FSLLOG3_DEBUG_INFO_IF(bottom > top, "bottom > top, capping height to zero");
      return {left, top, right >= left ? static_cast<uint32_t>(static_cast<int64_t>(right) - static_cast<int64_t>(left)) : 0u,
              bottom >= top ? static_cast<uint32_t>(static_cast<int64_t>(bottom) - static_cast<int64_t>(top)) : 0u};
    }

    static constexpr PxRectangle2D Empty() noexcept
    {
      return {};
    }


    constexpr inline int32_t Left() const noexcept
    {
      return Offset.X;
    }

    constexpr inline int32_t Top() const noexcept
    {
      return Offset.Y;
    }

    constexpr inline int32_t Right() const noexcept
    {
      return Offset.X + UncheckedNumericCast<int32_t>(Extent.Width);
    }

    constexpr inline int32_t Bottom() const noexcept
    {
      return Offset.Y + UncheckedNumericCast<int32_t>(Extent.Height);
    }

    //! @brief Get the center of this rect
    constexpr PxPoint2 Center() const noexcept
    {
      static_assert(static_cast<PxExtent2D::value_type>(std::numeric_limits<PxPoint2::value_type>::max()) <=
                      (std::numeric_limits<PxExtent2D::value_type>::max() / 2),
                    "overflow should not be possible");
      return {Offset.X + static_cast<PxPoint2::value_type>(Extent.Width / 2), Offset.Y + static_cast<PxPoint2::value_type>(Extent.Height / 2)};
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const int32_t x, const int32_t y) const noexcept
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom());
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    constexpr bool Contains(const PxPoint2& value) const noexcept
    {
      return Contains(value.X, value.Y);
    }


    //! @brief Check if the rectangle is considered to be contained within this rectangle
    constexpr bool Contains(const PxRectangle2D& value) const noexcept
    {
      return Contains(value.Offset) && Contains(value.Right(), value.Bottom());
    }

    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    constexpr bool IsEmpty() const noexcept
    {
      return (Offset.X == 0 && Offset.Y == 0 && Extent.Width == 0 && Extent.Height == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    constexpr bool Intersects(const PxRectangle2D& value) const noexcept
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom();
    }

    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static constexpr PxRectangle2D Intersect(const PxRectangle2D& rect1, const PxRectangle2D& rect2) noexcept
    {
      if (rect1.Intersects(rect2))
      {
        const int32_t rightSide = std::min(rect1.Right(), rect2.Right());
        const int32_t leftSide = std::max(rect1.Left(), rect2.Left());
        const int32_t topSide = std::max(rect1.Top(), rect2.Top());
        const int32_t bottomSide = std::min(rect1.Bottom(), rect2.Bottom());
        return FromLeftTopRightBottom(leftSide, topSide, rightSide, bottomSide);
      }
      return {};
    }

    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static constexpr PxRectangle2D Union(const PxRectangle2D& rect1, const PxRectangle2D& rect2) noexcept
    {
      const int32_t x = std::min(rect1.Offset.X, rect2.Offset.X);
      const int32_t y = std::min(rect1.Offset.Y, rect2.Offset.Y);
      const int32_t x2 = std::max(rect1.Right(), rect2.Right());
      const int32_t y2 = std::max(rect1.Bottom(), rect2.Bottom());
      return FromLeftTopRightBottom(x, y, x2, y2);
    }

    constexpr bool operator==(const PxRectangle2D& rhs) const noexcept
    {
      return (Offset == rhs.Offset && Extent == rhs.Extent);
    }


    constexpr bool operator!=(const PxRectangle2D& rhs) const noexcept
    {
      return (Offset != rhs.Offset || Extent != rhs.Extent);
    }
  };
}


#endif
