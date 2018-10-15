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

namespace Fsl
{
  struct Rectangle3D
  {
    Offset3D Offset;
    Extent3D Extent;

    Rectangle3D() = default;

    Rectangle3D(const Offset3D& offset, const Extent3D& extent)
      : Offset(offset)
      , Extent(extent)
    {
    }

    Rectangle3D(const int32_t x, const int32_t y, int32_t z, const uint16_t width, const uint16_t height, const uint16_t depth)
      : Offset(x, y, z)
      , Extent(width, height, depth)
    {
    }

    Rectangle3D(const int32_t left, const int32_t top, const int32_t front, const int32_t right, const int32_t bottom, const int32_t back,
                bool reserved);


    static Rectangle3D Empty()
    {
      return {};
    }


    inline int32_t Left() const
    {
      return Offset.X;
    }

    inline int32_t Top() const
    {
      return Offset.Y;
    }

    inline int32_t Front() const
    {
      return Offset.Z;
    }

    inline int32_t Right() const
    {
      return Offset.X + Extent.Width;
    }

    inline int32_t Bottom() const
    {
      return Offset.Y + Extent.Height;
    }

    inline int32_t Back() const
    {
      return Offset.Z + Extent.Depth;
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const int32_t x, const int32_t y, const int32_t z) const
    {
      return (x >= Left() && x < Right() && y >= Top() && y < Bottom() && z >= Front() && z < Back());
    }


    //! @brief Check if the x,y coordinate is considered to be contained within this rectangle
    bool Contains(const Offset3D& value) const
    {
      return Contains(value.X, value.Y, value.Z);
    }


    //! @brief Check if the rectangle is considered to be contained within this rectangle
    bool Contains(const Rectangle3D& value) const
    {
      return Contains(value.Offset) && Contains(value.Right(), value.Bottom(), value.Back());
    }


    //! @brief Get the center of this rect
    Offset3D GetCenter() const
    {
      return Offset3D(Offset.X + (Extent.Width / 2), Offset.Y + (Extent.Height / 2), Offset.Z + (Extent.Depth / 2));
    }


    //! @brief Each corner of the Rectangle is pushed away from the center of the rectangle by the specified amounts.
    //!        This results in the width and height of the Rectangle increasing by twice the values provide
    void Inflate(const int32_t horizontalValue, const int32_t verticalValue, const int32_t depthValue);

    //! @brief Gets a value that indicates whether the Rectangle is empty
    //!        An empty rectangle has all its values set to 0.
    bool IsEmpty() const
    {
      return (Offset.X == 0 && Offset.Y == 0 && Offset.Z == 0 && Extent.Width == 0 && Extent.Height == 0 && Extent.Depth == 0);
    }


    //! @brief Determines whether a specified Rectangle intersects with this Rectangle.
    bool Intersects(const Rectangle3D& value) const
    {
      return value.Left() < Right() && Left() < value.Right() && value.Top() < Bottom() && Top() < value.Bottom() && value.Front() < Back() &&
             Front() < value.Back();
    }


    //! @brief Creates a Rectangle defining the area where one rectangle overlaps another rectangle.
    static Rectangle3D Intersect(const Rectangle3D& rect1, const Rectangle3D& rect2);

    //! @brief Creates a new Rectangle that exactly contains the two supplied rectangles
    static Rectangle3D Union(const Rectangle3D& rect1, const Rectangle3D& rect2);


    bool operator==(const Rectangle3D& rhs) const
    {
      return (Offset == rhs.Offset && Extent == rhs.Extent);
    }


    bool operator!=(const Rectangle3D& rhs) const
    {
      return (Offset != rhs.Offset || Extent != rhs.Extent);
    }
  };
}


#endif
